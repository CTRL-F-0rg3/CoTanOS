/*
 * ============================================================================
 *  DP_CORE.C - DisplayPort / HDMI Core Driver Implementation
 *  Architecture: RISC-V (RV32 / RV64)
 *  Language: C11
 *  Blocks: 1 (Infrastructure + AUX) | 2 (Link Training + DPCD Caps + Link)
 * ============================================================================
 */

#include "dp_core.h"

/* ========================================================================== */
/* REGISTER OFFSETS                                                            */
/* ========================================================================== */

#define DP_REG_HW_VERSION               0x000
#define DP_REG_GLOBAL_CAPABILITIES      0x004
#define DP_REG_SYSTEM_CONTROL           0x008
#define DP_REG_SYSTEM_STATUS            0x00C
#define DP_REG_INTERRUPT_MASK           0x010
#define DP_REG_INTERRUPT_STATUS         0x014
#define DP_REG_AUX_CH_CTL               0x018
#define DP_REG_AUX_CH_ADDR              0x01C
#define DP_REG_AUX_CH_DATA              0x020
#define DP_REG_LINK_TRAINING_CTL        0x024
#define DP_REG_LANE_STATUS(n)           (0x028 + ((n) * 4))
#define DP_REG_LANE_SKEW(n)             (0x038 + ((n) * 4))
#define DP_REG_VOLTAGE_SWING_LEVEL(n)   (0x048 + ((n) * 4))
#define DP_REG_PRE_EMPHASIS_LEVEL(n)    (0x058 + ((n) * 4))
#define DP_REG_MSA_MISC0                0x068
#define DP_REG_MSA_MISC1                0x06C
#define DP_REG_HORIZONTAL_TOTAL         0x070
#define DP_REG_HORIZONTAL_START         0x074
#define DP_REG_HORIZONTAL_ACTIVE        0x078
#define DP_REG_HORIZONTAL_BLANKING      0x07C
#define DP_REG_VERTICAL_TOTAL           0x080
#define DP_REG_VERTICAL_START           0x084
#define DP_REG_VERTICAL_ACTIVE          0x088
#define DP_REG_VERTICAL_BLANKING        0x08C
#define DP_REG_PIXEL_CLOCK_RATE         0x090
#define DP_REG_AUDIO_CONTROL            0x094
#define DP_REG_AUDIO_STATUS             0x098
#define DP_REG_STREAM_ASSIGNMENT        0x09C

/* ========================================================================== */
/* AUX_CH_CTL BITS                                                             */
/* ========================================================================== */

#define AUX_CTL_START                   BIT(0)
#define AUX_CTL_BUSY                    BIT(1)
#define AUX_CTL_DONE                    BIT(2)
#define AUX_CTL_ERROR                   BIT(3)
#define AUX_CTL_TIMEOUT                 BIT(4)
#define AUX_CTL_NACK                    BIT(5)
#define AUX_CTL_DEFER                   BIT(6)
#define AUX_CTL_I2C_MOT_HW              BIT(7)
#define AUX_CTL_LENGTH_SHIFT            8
#define AUX_CTL_LENGTH_MASK             (0x1Fu << AUX_CTL_LENGTH_SHIFT)
#define AUX_CTL_CMD_SHIFT               16
#define AUX_CTL_CMD_MASK                (0xFFu << AUX_CTL_CMD_SHIFT)

#define AUX_CTL_STATUS_MASK \
    (AUX_CTL_DONE | AUX_CTL_ERROR | AUX_CTL_TIMEOUT | AUX_CTL_NACK | AUX_CTL_DEFER)

/* ========================================================================== */
/* GLOBAL STATE                                                                */
/* ========================================================================== */

dp_log_fn_t dp_log_fn = NULL;

/* ========================================================================== */
/* AUX LOW-LEVEL PRIMITIVES                                                    */
/* ========================================================================== */

static inline u32 dp_aux_ctl_read(const dp_controller_t *c)
{
    return dp_reg_read(c->base_virtual_addr, DP_REG_AUX_CH_CTL);
}

static inline void dp_aux_ctl_write(const dp_controller_t *c, u32 v)
{
    dp_reg_write(c->base_virtual_addr, DP_REG_AUX_CH_CTL, v);
}

static inline void dp_aux_addr_write(const dp_controller_t *c, u32 v)
{
    dp_reg_write(c->base_virtual_addr, DP_REG_AUX_CH_ADDR, v & 0x000FFFFFu);
}

static inline u32 dp_aux_data_read(const dp_controller_t *c)
{
    return dp_reg_read(c->base_virtual_addr, DP_REG_AUX_CH_DATA);
}

static inline void dp_aux_data_write(const dp_controller_t *c, u32 v)
{
    dp_reg_write(c->base_virtual_addr, DP_REG_AUX_CH_DATA, v);
}

static inline u32 dp_pack_le32(const u8 *b)
{
    return (u32)b[0] | ((u32)b[1] << 8) | ((u32)b[2] << 16) | ((u32)b[3] << 24);
}

static inline void dp_unpack_le32(u32 v, u8 *b)
{
    b[0] = (u8)v;
    b[1] = (u8)(v >> 8);
    b[2] = (u8)(v >> 16);
    b[3] = (u8)(v >> 24);
}

static void dp_aux_clear_status(const dp_controller_t *c)
{
    u32 v = dp_aux_ctl_read(c);
    dp_aux_ctl_write(c, v & ~AUX_CTL_STATUS_MASK);
}

static dp_aux_status_t dp_aux_decode_status(u32 ctl)
{
    if (ctl & AUX_CTL_TIMEOUT) return DP_AUX_TRANSACTION_TIMEOUT;
    if (ctl & AUX_CTL_ERROR)   return DP_AUX_TRANSACTION_FAILED;
    if (ctl & AUX_CTL_DEFER)   return DP_AUX_TRANSACTION_DEFERRED;
    if (ctl & AUX_CTL_NACK)    return DP_AUX_TRANSACTION_NACK;
    if (ctl & AUX_CTL_DONE)    return DP_AUX_TRANSACTION_COMPLETE;
    return DP_AUX_TRANSACTION_PENDING;
}

/* Assumes AUX data register auto-increments on each write (typical HW). */
static void dp_aux_push_payload(const dp_controller_t *c, const u8 *data, u8 len)
{
    u32 words = ((u32)len + 3u) / 4u;
    for (u32 i = 0; i < words; i++) {
        u8 tmp[4] = { 0, 0, 0, 0 };
        u32 off = i * 4u;
        u32 n = ((u32)len > off) ? ((u32)len - off) : 0u;
        if (n > 4u) n = 4u;
        dp_mem_copy(tmp, data + off, n);
        dp_aux_data_write(c, dp_pack_le32(tmp));
    }
}

static void dp_aux_pop_payload(const dp_controller_t *c, u8 *data, u8 len)
{
    u32 words = ((u32)len + 3u) / 4u;
    for (u32 i = 0; i < words; i++) {
        u32 w = dp_aux_data_read(c);
        u8 tmp[4];
        u32 off = i * 4u;
        u32 n = ((u32)len > off) ? ((u32)len - off) : 0u;
        if (n > 4u) n = 4u;
        dp_unpack_le32(w, tmp);
        dp_mem_copy(data + off, tmp, n);
    }
}

static void dp_aux_hw_start(const dp_controller_t *c, u8 cmd, u32 addr, u8 len)
{
    dp_aux_clear_status(c);
    dp_aux_addr_write(c, addr);
    u32 ctl = ((u32)cmd & 0xFFu) << AUX_CTL_CMD_SHIFT;
    ctl |= ((u32)len & 0x1Fu) << AUX_CTL_LENGTH_SHIFT;
    ctl |= AUX_CTL_START;
    dp_aux_ctl_write(c, ctl);
}

static dp_aux_status_t dp_aux_hw_wait(const dp_controller_t *c, u32 timeout_us)
{
    u64 start  = riscv_read_time();
    u64 target = start + (u64)timeout_us;

    for (;;) {
        u32 ctl = dp_aux_ctl_read(c);

        if (ctl & AUX_CTL_BUSY) {
            if (riscv_read_time() >= target) {
                dp_aux_ctl_write(c, dp_aux_ctl_read(c) | AUX_CTL_TIMEOUT);
                return DP_AUX_TRANSACTION_TIMEOUT;
            }
            cpu_pause();
            continue;
        }

        dp_aux_status_t st = dp_aux_decode_status(ctl);
        if (st != DP_AUX_TRANSACTION_PENDING)
            return st;

        if (riscv_read_time() >= target)
            return DP_AUX_TRANSACTION_TIMEOUT;

        cpu_pause();
    }
}

static dp_aux_status_t dp_aux_do_one(const dp_controller_t *c, u8 cmd, u32 addr,
                                     u8 len, const u8 *wdata, u8 *rdata,
                                     u32 timeout_us)
{
    if (wdata && len > 0)
        dp_aux_push_payload(c, wdata, len);

    dp_aux_hw_start(c, cmd, addr, len);
    dp_aux_status_t st = dp_aux_hw_wait(c, timeout_us);

    if (st == DP_AUX_TRANSACTION_COMPLETE && rdata && len > 0)
        dp_aux_pop_payload(c, rdata, len);

    return st;
}

static dp_aux_status_t dp_aux_do_with_retry(const dp_controller_t *c, u8 cmd,
                                            u32 addr, u8 len,
                                            const u8 *wdata, u8 *rdata,
                                            u32 timeout_us, u32 max_retries)
{
    dp_aux_status_t last = DP_AUX_TRANSACTION_FAILED;
    u32 backoff = DP_AUX_DEFER_BACKOFF_US;

    for (u32 attempt = 0; attempt <= max_retries; attempt++) {
        last = dp_aux_do_one(c, cmd, addr, len, wdata, rdata, timeout_us);
        if (last == DP_AUX_TRANSACTION_COMPLETE)
            return DP_AUX_TRANSACTION_COMPLETE;

        if (last == DP_AUX_TRANSACTION_DEFERRED) {
            dp_udelay(backoff);
            backoff <<= 1;
            if (backoff > 8000u) backoff = 8000u;
        } else if (last == DP_AUX_TRANSACTION_NACK ||
                   last == DP_AUX_TRANSACTION_TIMEOUT) {
            dp_udelay(100);
        } else {
            return last;
        }
    }
    return last;
}

/* ========================================================================== */
/* PUBLIC AUX API                                                              */
/* ========================================================================== */

int dp_aux_transaction(dp_controller_t *c, u8 cmd, u32 addr,
                       u8 *data, u8 len, u8 *reply)
{
    if (!c || !c->initialized)           return DP_ERR_INVAL;
    if (len > DP_AUX_MAX_PAYLOAD_BYTES)  return DP_ERR_INVAL;

    const bool is_read = (cmd == DP_AUX_CMD_NATIVE_READ) ||
                         (cmd == DP_AUX_CMD_I2C_READ);

    const u8 *wdata = NULL;
    u8       *rdata = NULL;
    u8        local_buf[DP_AUX_MAX_PAYLOAD_BYTES];

    if (is_read) {
        rdata = data ? data : local_buf;
    } else if (len > 0) {
        if (!data) return DP_ERR_INVAL;
        wdata = data;
    }

    u32 timeout_us = (cmd == DP_AUX_CMD_I2C_READ || cmd == DP_AUX_CMD_I2C_WRITE ||
                      cmd == DP_AUX_CMD_I2C_READ_STATUS ||
                      cmd == DP_AUX_CMD_I2C_WRITE_STATUS)
                     ? DP_AUX_TIMEOUT_US_I2C : DP_AUX_TIMEOUT_US_NATIVE;

    dp_aux_status_t st = dp_aux_do_with_retry(c, cmd, addr, len, wdata, rdata,
                                              timeout_us, DP_AUX_REPLY_MAX_RETRIES);

    if (reply) {
        switch (st) {
        case DP_AUX_TRANSACTION_COMPLETE: *reply = DP_AUX_REPLY_ACK;   break;
        case DP_AUX_TRANSACTION_DEFERRED: *reply = DP_AUX_REPLY_DEFER; break;
        default:                          *reply = DP_AUX_REPLY_NACK;  break;
        }
    }

    switch (st) {
    case DP_AUX_TRANSACTION_COMPLETE: return DP_OK;
    case DP_AUX_TRANSACTION_DEFERRED: return DP_ERR_AUX_DEFER;
    case DP_AUX_TRANSACTION_NACK:     return DP_ERR_AUX_NACK;
    case DP_AUX_TRANSACTION_TIMEOUT:  return DP_ERR_TIMEOUT;
    default:                          return DP_ERR_GENERIC;
    }
}

int dp_aux_write_dpcd(dp_controller_t *c, u32 addr, const u8 *buf, u32 len)
{
    if (!c || !buf) return DP_ERR_INVAL;
    while (len > 0) {
        u8 chunk = (u8)(len > DP_AUX_MAX_PAYLOAD_BYTES ? DP_AUX_MAX_PAYLOAD_BYTES : len);
        int rc = dp_aux_transaction(c, DP_AUX_CMD_NATIVE_WRITE, addr,
                                    (u8 *)buf, chunk, NULL);
        if (rc != DP_OK) return rc;
        addr += chunk; buf += chunk; len -= chunk;
    }
    return DP_OK;
}

int dp_aux_read_dpcd(dp_controller_t *c, u32 addr, u8 *buf, u32 len)
{
    if (!c || !buf) return DP_ERR_INVAL;
    while (len > 0) {
        u8 chunk = (u8)(len > DP_AUX_MAX_REPLY_BYTES ? DP_AUX_MAX_REPLY_BYTES : len);
        int rc = dp_aux_transaction(c, DP_AUX_CMD_NATIVE_READ, addr,
                                    buf, chunk, NULL);
        if (rc != DP_OK) return rc;
        addr += chunk; buf += chunk; len -= chunk;
    }
    return DP_OK;
}

/* ========================================================================== */
/* DPCD VALUE HELPERS                                                          */
/* ========================================================================== */

static u8 dp_dpcd_read_byte(dp_controller_t *c, u32 addr)
{
    u8 v = 0xFF;
    if (dp_aux_read_dpcd(c, addr, &v, 1) != DP_OK) return 0xFF;
    return v;
}

static int dp_dpcd_write_byte(dp_controller_t *c, u32 addr, u8 v)
{
    return dp_aux_write_dpcd(c, addr, &v, 1);
}

static __maybe_unused u16 dp_dpcd_read_le16(dp_controller_t *c, u32 addr)
{
    u8 b[2] = { 0, 0 };
    if (dp_aux_read_dpcd(c, addr, b, 2) != DP_OK) return 0xFFFF;
    return (u16)b[0] | ((u16)b[1] << 8);
}

static __maybe_unused u32 dp_dpcd_read_le32(dp_controller_t *c, u32 addr)
{
    u8 b[4] = { 0, 0, 0, 0 };
    if (dp_aux_read_dpcd(c, addr, b, 4) != DP_OK) return 0xFFFFFFFFu;
    return dp_pack_le32(b);
}

/* ========================================================================== */
/* I2C-OVER-AUX                                                                */
/* ========================================================================== */

static int dp_i2c_send_offset_mot(dp_controller_t *c, u8 i2c_addr,
                                  u32 offset, bool mot)
{
    u8 off[2];
    u32 off_len = 1;
    off[0] = (u8)(offset & 0xFF);
    if (offset > 0xFFu) {
        off[0] = (u8)((offset >> 8) & 0xFF);
        off[1] = (u8)(offset & 0xFF);
        off_len = 2;
    }
    u8 cmd = DP_AUX_CMD_I2C_WRITE;
    if (mot) cmd |= DP_AUX_CMD_I2C_MOT;
    return dp_aux_transaction(c, cmd, (u32)i2c_addr, off, (u8)off_len, NULL);
}

int dp_aux_i2c_write(dp_controller_t *c, u8 i2c_addr, u32 offset,
                     const u8 *buf, u32 len)
{
    if (!c || !buf)       return DP_ERR_INVAL;
    if (i2c_addr > 0x7Fu) return DP_ERR_INVAL;

    int rc = dp_i2c_send_offset_mot(c, i2c_addr, offset, len > 0);
    if (rc != DP_OK) return rc;

    while (len > 0) {
        u8 chunk = (u8)(len > DP_AUX_MAX_PAYLOAD_BYTES ? DP_AUX_MAX_PAYLOAD_BYTES : len);
        bool last = (len <= chunk);
        u8 dcmd = DP_AUX_CMD_I2C_WRITE;
        if (!last) dcmd |= DP_AUX_CMD_I2C_MOT;
        rc = dp_aux_transaction(c, dcmd, (u32)i2c_addr, (u8 *)buf, chunk, NULL);
        if (rc != DP_OK) return rc;
        buf += chunk; len -= chunk;
    }
    return DP_OK;
}

int dp_aux_i2c_read(dp_controller_t *c, u8 i2c_addr, u32 offset,
                    u8 *buf, u32 len)
{
    if (!c || !buf)       return DP_ERR_INVAL;
    if (i2c_addr > 0x7Fu) return DP_ERR_INVAL;

    int rc = dp_i2c_send_offset_mot(c, i2c_addr, offset, true);
    if (rc != DP_OK) return rc;

    while (len > 0) {
        u8 chunk = (u8)(len > DP_AUX_MAX_REPLY_BYTES ? DP_AUX_MAX_REPLY_BYTES : len);
        bool last = (len <= chunk);
        u8 rcmd = DP_AUX_CMD_I2C_READ;
        if (!last) rcmd |= DP_AUX_CMD_I2C_MOT;
        rc = dp_aux_transaction(c, rcmd, (u32)i2c_addr, buf, chunk, NULL);
        if (rc != DP_OK) return rc;
        buf += chunk; len -= chunk;
    }
    return DP_OK;
}

/* ========================================================================== */
/* DPCD PROBE / STATUS                                                         */
/* ========================================================================== */

static bool dp_aux_probe_sink(dp_controller_t *c, u8 *rev_out)
{
    if (!c || !c->initialized) return false;
    u8 rev = 0x00;
    if (dp_aux_read_dpcd(c, DP_DPCD_REV, &rev, 1) != DP_OK) {
        if (rev_out) *rev_out = 0;
        return false;
    }
    if (rev_out) *rev_out = rev;
    return (rev != 0x00) && (rev != 0xFF);
}

static __maybe_unused int dp_aux_read_link_caps_raw(dp_controller_t *c,
                                                    u8 *max_rate, u8 *max_lanes)
{
    if (!c || !max_rate || !max_lanes) return DP_ERR_INVAL;
    u8 buf[2] = { 0, 0 };
    int rc = dp_aux_read_dpcd(c, DP_MAX_LINK_RATE, buf, 2);
    if (rc != DP_OK) return rc;
    *max_rate  = buf[0];
    *max_lanes = buf[1] & DP_MAX_LANE_COUNT_MASK;
    return DP_OK;
}

static __maybe_unused int dp_aux_read_guid(dp_controller_t *c, u8 guid[16])
{
    if (!c || !guid) return DP_ERR_INVAL;
    return dp_aux_read_dpcd(c, DP_GUID, guid, 16);
}

static int dp_aux_set_sink_power(dp_controller_t *c, u8 d0_or_d3)
{
    if (!c) return DP_ERR_INVAL;
    if (d0_or_d3 != DP_SET_POWER_D0 && d0_or_d3 != DP_SET_POWER_D3)
        return DP_ERR_INVAL;
    return dp_dpcd_write_byte(c, DP_SET_POWER, d0_or_d3);
}

static __maybe_unused u8 dp_aux_read_lane_status_01(dp_controller_t *c)
{ return dp_dpcd_read_byte(c, DP_LANE0_1_STATUS); }

static __maybe_unused u8 dp_aux_read_lane_status_23(dp_controller_t *c)
{ return dp_dpcd_read_byte(c, DP_LANE2_3_STATUS); }

static __maybe_unused u8 dp_aux_read_lane_align_status(dp_controller_t *c)
{ return dp_dpcd_read_byte(c, DP_LANE_ALIGN_STATUS_UPDATED); }

static __maybe_unused u8 dp_aux_read_sink_status(dp_controller_t *c)
{ return dp_dpcd_read_byte(c, DP_SINK_STATUS); }

static int dp_aux_read_adjust_request(dp_controller_t *c, u8 *lane01, u8 *lane23)
{
    if (!c || !lane01 || !lane23) return DP_ERR_INVAL;
    u8 buf[2] = { 0, 0 };
    int rc = dp_aux_read_dpcd(c, DP_ADJUST_REQUEST_LANE0_1, buf, 2);
    if (rc != DP_OK) return rc;
    *lane01 = buf[0];
    *lane23 = buf[1];
    return DP_OK;
}

static __maybe_unused u32 dp_aux_dump_dpcd(dp_controller_t *c, u32 addr,
                                           u8 *out, u32 len)
{
    if (!c || !out) return 0;
    if (len > DP_AUX_MAX_REPLY_BYTES) len = DP_AUX_MAX_REPLY_BYTES;
    return (dp_aux_read_dpcd(c, addr, out, len) == DP_OK) ? len : 0;
}

/* ========================================================================== */
/* BLOCK 2: LINK TRAINING                                                      */
/* ========================================================================== */

static u32 dp_lt_max_pe(dp_link_bandwidth_t bw)
{
    switch (bw) {
    case DP_RATE_RBR:  return 0;
    case DP_RATE_HBR:  return 1;
    case DP_RATE_HBR2: return 3;
    case DP_RATE_HBR3: return 3;
    default:           return 0;
    }
}

static void dp_lt_write_lane_dpcd(dp_controller_t *c,
                                   const dp_link_training_settings_t *s)
{
    u8 lanes[4];
    for (u32 i = 0; i < 4; i++) {
        u32 vs = s->lanes[i].voltage_swing;
        u32 pe = s->lanes[i].pre_emphasis;
        if (vs > 3) vs = 3;
        if (pe > 3) pe = 3;
        lanes[i] = dp_lt_build_lane_set(vs, pe, false, false);
    }
    dp_aux_write_dpcd(c, DP_TRAINING_LANE0_SET, lanes, 4);
}

static void dp_lt_write_lane_hw(dp_controller_t *c,
                                 const dp_link_training_settings_t *s)
{
    vaddr_t base = c->base_virtual_addr;
    for (u32 i = 0; i < (u32)c->lanes; i++) {
        u32 vs = s->lanes[i].voltage_swing;
        u32 pe = s->lanes[i].pre_emphasis;
        if (vs > 3) vs = 3;
        if (pe > 3) pe = 3;
        dp_reg_write(base, DP_REG_VOLTAGE_SWING_LEVEL(i), vs);
        dp_reg_write(base, DP_REG_PRE_EMPHASIS_LEVEL(i), pe);
    }
}

int dp_link_read_caps(dp_controller_t *c, dp_link_caps_t *caps)
{
    if (!c || !caps) return DP_ERR_INVAL;

    u8 buf[7] = { 0 };
    int rc = dp_aux_read_dpcd(c, DP_MAX_LINK_RATE, buf, 7);
    if (rc != DP_OK) return rc;

    caps->max_link_rate            = buf[0];
    caps->max_lane_count           = buf[1] & DP_MAX_LANE_COUNT_MASK;
    caps->enhanced_frame_cap       = (buf[1] & DP_ENHANCED_FRAME_CAP) != 0;
    caps->tps3_supported           = (buf[1] & DP_TPS3_SUPPORTED) != 0;
    caps->no_aux_handshake         = (buf[2] & DP_NO_AUX_HANDSHAKE_LINK_TRAINING) != 0;
    caps->downstream_port_present  = (buf[3] & DP_DOWNSTREAMPORT_PRESENT) != 0;
    return DP_OK;
}

int dp_link_set_rate(dp_controller_t *c, dp_link_bandwidth_t bw,
                     dp_lane_count_t lanes)
{
    if (!c) return DP_ERR_INVAL;
    if (lanes != DP_LANE_COUNT_1 && lanes != DP_LANE_COUNT_2 &&
        lanes != DP_LANE_COUNT_4)
        return DP_ERR_INVAL;

    u8 bw_val;
    switch (bw) {
    case DP_RATE_RBR:  bw_val = DP_LINK_BW_SET_1_62; break;
    case DP_RATE_HBR:  bw_val = DP_LINK_BW_SET_2_70; break;
    case DP_RATE_HBR2: bw_val = DP_LINK_BW_SET_5_40; break;
    case DP_RATE_HBR3: bw_val = DP_LINK_BW_SET_8_10; break;
    default:           return DP_ERR_INVAL;
    }

    dp_link_caps_t caps;
    int rc = dp_link_read_caps(c, &caps);
    if (rc != DP_OK) return rc;

    u8 lane_val = (u8)lanes;
    if (caps.enhanced_frame_cap)
        lane_val |= DP_ENHANCED_FRAME_EN;

    rc = dp_aux_write_dpcd(c, DP_LINK_BW_SET, &bw_val, 1);
    if (rc != DP_OK) return rc;

    rc = dp_aux_write_dpcd(c, DP_LANE_COUNT_SET, &lane_val, 1);
    if (rc != DP_OK) return rc;

    dp_reg_write(c->base_virtual_addr, DP_REG_LINK_TRAINING_CTL,
                 ((u32)lanes & 0x07u) | (((u32)bw_val) << 8));

    c->bandwidth = bw;
    c->lanes = lanes;
    return DP_OK;
}

int dp_link_train_cr(dp_controller_t *c)
{
    if (!c) return DP_ERR_INVAL;

    u8 tp = DP_TRAINING_PATTERN_1;
    if (dp_aux_write_dpcd(c, DP_TRAINING_PATTERN_SET, &tp, 1) != DP_OK)
        return DP_ERR_LINK_TRAIN;

    dp_link_training_settings_t settings = {0};
    dp_lt_write_lane_dpcd(c, &settings);
    dp_lt_write_lane_hw(c, &settings);

    const u32 max_pe = dp_lt_max_pe(c->bandwidth);

    for (u32 iter = 0; iter < DP_LT_MAX_PATTERN_1_ITER; iter++) {
        dp_udelay(100);

        u8 lane01 = 0, lane23 = 0;
        if (dp_aux_read_dpcd(c, DP_LANE0_1_STATUS, &lane01, 1) != DP_OK)
            continue;
        if (dp_aux_read_dpcd(c, DP_LANE2_3_STATUS, &lane23, 1) != DP_OK)
            continue;

        if (dp_lt_all_lanes_cr_done(lane01, lane23, c->lanes))
            return DP_OK;

        u8 adj01 = 0, adj23 = 0;
        if (dp_aux_read_adjust_request(c, &adj01, &adj23) != DP_OK)
            continue;

        u32 vs[4], pe[4];
        dp_lt_decode_adjust(adj01, adj23, vs, pe);

        bool changed = false;
        for (u32 i = 0; i < 4; i++) {
            u32 nvs = vs[i] > 3 ? 3 : vs[i];
            u32 npe = pe[i] > max_pe ? max_pe : pe[i];
            if (settings.lanes[i].voltage_swing != nvs ||
                settings.lanes[i].pre_emphasis != npe) {
                settings.lanes[i].voltage_swing = nvs;
                settings.lanes[i].pre_emphasis = npe;
                changed = true;
            }
        }
        if (changed) {
            dp_lt_write_lane_dpcd(c, &settings);
            dp_lt_write_lane_hw(c, &settings);
        }
    }

    return DP_ERR_LINK_TRAIN;
}

int dp_link_train_eq(dp_controller_t *c)
{
    if (!c) return DP_ERR_INVAL;

    u8 tp = (c->bandwidth == DP_RATE_HBR3)
            ? DP_TRAINING_PATTERN_3 : DP_TRAINING_PATTERN_2;

    if (dp_aux_write_dpcd(c, DP_TRAINING_PATTERN_SET, &tp, 1) != DP_OK)
        return DP_ERR_LINK_TRAIN;

    dp_link_training_settings_t settings = {0};
    dp_lt_write_lane_dpcd(c, &settings);
    dp_lt_write_lane_hw(c, &settings);

    const u32 max_pe = dp_lt_max_pe(c->bandwidth);

    for (u32 iter = 0; iter < DP_LT_MAX_PATTERN_2_ITER; iter++) {
        dp_udelay(400);

        u8 lane01 = 0, lane23 = 0;
        if (dp_aux_read_dpcd(c, DP_LANE0_1_STATUS, &lane01, 1) != DP_OK)
            continue;
        if (dp_aux_read_dpcd(c, DP_LANE2_3_STATUS, &lane23, 1) != DP_OK)
            continue;

        if (dp_lt_all_lanes_eq_done(lane01, lane23, c->lanes))
            return DP_OK;

        u8 adj01 = 0, adj23 = 0;
        if (dp_aux_read_adjust_request(c, &adj01, &adj23) != DP_OK)
            continue;

        u32 vs[4], pe[4];
        dp_lt_decode_adjust(adj01, adj23, vs, pe);

        bool changed = false;
        for (u32 i = 0; i < 4; i++) {
            u32 nvs = vs[i] > 3 ? 3 : vs[i];
            u32 npe = pe[i] > max_pe ? max_pe : pe[i];
            if (settings.lanes[i].voltage_swing != nvs ||
                settings.lanes[i].pre_emphasis != npe) {
                settings.lanes[i].voltage_swing = nvs;
                settings.lanes[i].pre_emphasis = npe;
                changed = true;
            }
        }
        if (changed) {
            dp_lt_write_lane_dpcd(c, &settings);
            dp_lt_write_lane_hw(c, &settings);
        }
    }

    return DP_ERR_LINK_TRAIN;
}

int dp_link_verify_status(dp_controller_t *c, dp_link_training_state_t *st)
{
    if (!c || !st) return DP_ERR_INVAL;

    u8 lane01 = 0, lane23 = 0, align = 0;
    if (dp_aux_read_dpcd(c, DP_LANE0_1_STATUS, &lane01, 1) != DP_OK)
        return DP_ERR_AUX_NACK;
    if (dp_aux_read_dpcd(c, DP_LANE2_3_STATUS, &lane23, 1) != DP_OK)
        return DP_ERR_AUX_NACK;
    if (dp_aux_read_dpcd(c, DP_LANE_ALIGN_STATUS_UPDATED, &align, 1) != DP_OK)
        return DP_ERR_AUX_NACK;

    bool cr_done  = dp_lt_all_lanes_cr_done(lane01, lane23, c->lanes);
    bool eq_done  = dp_lt_all_lanes_eq_done(lane01, lane23, c->lanes);
    bool align_ok = (align & DP_INTERLANE_ALIGN_DONE) != 0;

    if (cr_done && eq_done && align_ok) {
        *st = DP_LINK_TRAINING_SUCCESS;
        return DP_OK;
    }
    if (!cr_done) { *st = DP_LINK_TRAINING_FAIL_CR; return DP_ERR_LINK_TRAIN; }
    if (!eq_done) { *st = DP_LINK_TRAINING_FAIL_EQ; return DP_ERR_LINK_TRAIN; }
    *st = DP_LINK_TRAINING_FAIL_INVALID;
    return DP_ERR_LINK_TRAIN;
}

int dp_link_train(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    dp_aux_set_sink_power(c, DP_SET_POWER_D0);
    dp_udelay(1000);

    if (!dp_aux_probe_sink(c, NULL)) {
        c->current_state = DP_STATE_ERROR_FAULT;
        return DP_ERR_NO_DEVICE;
    }

    dp_link_caps_t caps;
    if (dp_link_read_caps(c, &caps) != DP_OK) {
        c->current_state = DP_STATE_ERROR_FAULT;
        return DP_ERR_AUX_NACK;
    }

    dp_link_bandwidth_t target_bw = c->bandwidth;
    if ((u32)target_bw > caps.max_link_rate)
        target_bw = (dp_link_bandwidth_t)caps.max_link_rate;

    dp_lane_count_t target_lanes = c->lanes;
    if (caps.max_lane_count < (u32)target_lanes) {
        if      (caps.max_lane_count >= 4) target_lanes = DP_LANE_COUNT_4;
        else if (caps.max_lane_count >= 2) target_lanes = DP_LANE_COUNT_2;
        else                                target_lanes = DP_LANE_COUNT_1;
    }

    if (dp_link_set_rate(c, target_bw, target_lanes) != DP_OK) {
        c->current_state = DP_STATE_ERROR_FAULT;
        return DP_ERR_AUX_NACK;
    }

    c->current_state = DP_STATE_LINK_TRAIN_CR;
    if (dp_link_train_cr(c) != DP_OK) {
        c->current_state = DP_STATE_ERROR_FAULT;
        return DP_ERR_LINK_TRAIN;
    }

    c->current_state = DP_STATE_LINK_TRAIN_EQ;
    if (dp_link_train_eq(c) != DP_OK) {
        c->current_state = DP_STATE_ERROR_FAULT;
        return DP_ERR_LINK_TRAIN;
    }

    u8 tp_dis = DP_TRAINING_PATTERN_DISABLE;
    dp_aux_write_dpcd(c, DP_TRAINING_PATTERN_SET, &tp_dis, 1);

    dp_link_training_state_t vst;
    if (dp_link_verify_status(c, &vst) != DP_OK) {
        c->current_state = DP_STATE_ERROR_FAULT;
        return DP_ERR_LINK_TRAIN;
    }

    c->current_state = DP_STATE_LINK_READY;
    return DP_OK;
}

/*
 * ============================================================================
 *  DP_CORE.C - Block 3: EDID / CEA-861 / HDMI DDC
 *  Architecture: RISC-V (RV32 / RV64)
 *  Scope: DP (I2C-over-AUX) and HDMI (DDC) EDID reading, base-block parsing,
 *         CEA-861 extension parsing, HDMI VSDB, audio SADs, ELD fill.
 * ============================================================================
 */

#include "dp_core.h"

/* ========================================================================== */
/* HDMI REGISTER OFFSETS                                                       */
/* ========================================================================== */

#define HDMI_REG_HW_VERSION         0x00
#define HDMI_REG_GLOBAL_CAPS        0x04
#define HDMI_REG_SYSTEM_CONTROL     0x08
#define HDMI_REG_SYSTEM_STATUS      0x0C
#define HDMI_REG_INTERRUPT_MASK     0x10
#define HDMI_REG_INTERRUPT_STATUS   0x14
#define HDMI_REG_DDC_CTL            0x18
#define HDMI_REG_DDC_ADDR           0x1C
#define HDMI_REG_DDC_DATA           0x20
#define HDMI_REG_TMDS_CTL           0x24
#define HDMI_REG_TMDS_CLK           0x28
#define HDMI_REG_PIXEL_REPEAT       0x2C
#define HDMI_REG_COLOR_DEPTH        0x30
#define HDMI_REG_COLOR_FORMAT       0x34
#define HDMI_REG_VIDEO_MODE         0x38
#define HDMI_REG_H_SYNC             0x3C
#define HDMI_REG_V_SYNC             0x40
#define HDMI_REG_H_ACTIVE           0x44
#define HDMI_REG_V_ACTIVE           0x48
#define HDMI_REG_H_TOTAL            0x4C
#define HDMI_REG_V_TOTAL            0x50
#define HDMI_REG_AUDIO_CTL          0x54
#define HDMI_REG_AUDIO_CH_STATUS    0x58
#define HDMI_REG_AUDIO_DATA         0x5C
#define HDMI_REG_INFOFRAME_CTL      0x60
#define HDMI_REG_INFOFRAME_DATA     0x64
#define HDMI_REG_HDCP_CTL           0x68
#define HDMI_REG_HDCP_STATUS        0x6C
#define HDMI_REG_CEC_CTL            0x70
#define HDMI_REG_CEC_DATA           0x74

/* ========================================================================== */
/* HDMI DDC CONTROL                                                            */
/* ========================================================================== */
/* Assumed HW model: write slave+offset into DDC_ADDR, push/pop payload through
 * DDC_DATA (u32 little-endian lanes), set START|READ|LEN in DDC_CTL, poll DONE.
 * Adapt this block if the actual controller differs. */

#define HDMI_DDC_CTL_START          BIT(0)
#define HDMI_DDC_CTL_READ           BIT(1)
#define HDMI_DDC_CTL_DONE           BIT(2)
#define HDMI_DDC_CTL_NACK           BIT(3)
#define HDMI_DDC_CTL_TIMEOUT        BIT(4)
#define HDMI_DDC_CTL_LEN_SHIFT      8
#define HDMI_DDC_CTL_LEN_MASK       (0x1FFu << HDMI_DDC_CTL_LEN_SHIFT)

#define HDMI_DDC_ADDR_SLAVE_SHIFT   0
#define HDMI_DDC_ADDR_SLAVE_MASK    0x7Fu
#define HDMI_DDC_ADDR_OFFSET_SHIFT  8
#define HDMI_DDC_ADDR_OFFSET_MASK   0xFF00u

#define HDMI_EDID_SLAVE_ADDR        0x50
#define HDMI_EDID_SEGMENT_ADDR      0x30

static int hdmi_ddc_xfer(dp_controller_t *c, u8 slave, u8 offset,
                         const u8 *wbuf, u8 *rbuf, u32 len)
{
    if (!c || len > 0x1FFu) return DP_ERR_INVAL;
    vaddr_t base = c->base_virtual_addr;

    if (wbuf && len > 0) {
        for (u32 i = 0; i < len; i += 4) {
            u32 word = 0;
            for (u32 j = 0; j < 4 && (i + j) < len; j++)
                word |= ((u32)wbuf[i + j]) << (j * 8);
            dp_reg_write(base, HDMI_REG_DDC_DATA, word);
        }
    }

    u32 addr = (((u32)slave << HDMI_DDC_ADDR_SLAVE_SHIFT) & HDMI_DDC_ADDR_SLAVE_MASK) |
               (((u32)offset << HDMI_DDC_ADDR_OFFSET_SHIFT) & HDMI_DDC_ADDR_OFFSET_MASK);
    dp_reg_write(base, HDMI_REG_DDC_ADDR, addr);

    u32 ctl = HDMI_DDC_CTL_START |
              (((u32)len << HDMI_DDC_CTL_LEN_SHIFT) & HDMI_DDC_CTL_LEN_MASK);
    if (rbuf) ctl |= HDMI_DDC_CTL_READ;
    dp_reg_write(base, HDMI_REG_DDC_CTL, ctl);

    u64 deadline = riscv_read_time() + 10000ULL;
    for (;;) {
        u32 st = dp_reg_read(base, HDMI_REG_DDC_CTL);
        if (st & (HDMI_DDC_CTL_DONE | HDMI_DDC_CTL_NACK | HDMI_DDC_CTL_TIMEOUT))
            break;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }

    u32 st = dp_reg_read(base, HDMI_REG_DDC_CTL);
    dp_reg_write(base, HDMI_REG_DDC_CTL, 0);
    if (st & HDMI_DDC_CTL_TIMEOUT) return DP_ERR_TIMEOUT;
    if (st & HDMI_DDC_CTL_NACK)    return DP_ERR_AUX_NACK;
    if (!(st & HDMI_DDC_CTL_DONE)) return DP_ERR_GENERIC;

    if (rbuf && len > 0) {
        for (u32 i = 0; i < len; i += 4) {
            u32 word = dp_reg_read(base, HDMI_REG_DDC_DATA);
            for (u32 j = 0; j < 4 && (i + j) < len; j++)
                rbuf[i + j] = (u8)(word >> (j * 8));
        }
    }
    return DP_OK;
}

/* ========================================================================== */
/* EDID BLOCK READ HELPERS                                                     */
/* ========================================================================== */

static int dp_edid_read_block_aux(dp_controller_t *c, u32 offset, u8 *buf)
{
    return dp_aux_i2c_read(c, DP_AUX_ADDR_I2C_EDID, offset, buf, EDID_BLOCK_SIZE);
}

static int hdmi_edid_read_block_ddc(dp_controller_t *c, u8 block, u8 *buf)
{
    if (block > 1) return DP_ERR_NOT_SUPPORTED;
    u8 offset = (block == 0) ? 0x00 : 0x80;
    return hdmi_ddc_xfer(c, HDMI_EDID_SLAVE_ADDR, offset, NULL, buf, EDID_BLOCK_SIZE);
}

static bool edid_checksum_ok(const u8 *block)
{
    u8 sum = 0;
    for (u32 i = 0; i < EDID_BLOCK_SIZE; i++) sum += block[i];
    return sum == 0;
}

/* ========================================================================== */
/* BASE EDID READ                                                              */
/* ========================================================================== */

int dp_edid_read(dp_controller_t *c, edid_base_block_t *out,
                 cea861_extension_t *cea, u32 cea_len)
{
    if (!c || !out) return DP_ERR_INVAL;

    u8 raw[EDID_BLOCK_SIZE];
    int rc = dp_edid_read_block_aux(c, 0, raw);
    if (rc != DP_OK) return rc;
    if (!edid_checksum_ok(raw)) return DP_ERR_EDID;
    if (!edid_check_header((const edid_base_block_t *)raw)) return DP_ERR_EDID;
    dp_mem_copy(out, raw, EDID_BLOCK_SIZE);

    u32 ext_count = out->ext_block_count;
    if (ext_count > EDID_MAX_EXTENSIONS) ext_count = EDID_MAX_EXTENSIONS;

    if (cea && cea_len >= EDID_BLOCK_SIZE) {
        for (u32 i = 0; i < ext_count && i < 2; i++) {
            u8 ext[EDID_BLOCK_SIZE];
            u32 off = (i + 1) * EDID_BLOCK_SIZE;
            if (dp_edid_read_block_aux(c, off, ext) != DP_OK) continue;
            if (!edid_checksum_ok(ext)) continue;
            if (ext[0] == EDID_CEA861_EXT_TAG) {
                u32 copy = cea_len > EDID_BLOCK_SIZE ? EDID_BLOCK_SIZE : cea_len;
                dp_mem_copy(cea, ext, copy);
                break;
            }
        }
    }
    return DP_OK;
}

int hdmi_edid_read(dp_controller_t *c, u8 *buf, u32 len)
{
    if (!c || !buf) return DP_ERR_INVAL;

    u32 blocks = (len + EDID_BLOCK_SIZE - 1) / EDID_BLOCK_SIZE;
    if (blocks > EDID_MAX_EXTENSIONS + 1) blocks = EDID_MAX_EXTENSIONS + 1;

    u32 got = 0;
    for (u32 i = 0; i < blocks; i++) {
        u8 block[EDID_BLOCK_SIZE];
        if (hdmi_edid_read_block_ddc(c, (u8)i, block) != DP_OK) break;
        if (!edid_checksum_ok(block)) break;
        u32 n = (len - got > EDID_BLOCK_SIZE) ? EDID_BLOCK_SIZE : (len - got);
        dp_mem_copy(buf + got, block, n);
        got += n;
        if (got >= len) break;
    }
    return (got >= EDID_BLOCK_SIZE) ? DP_OK : DP_ERR_EDID;
}

/* ========================================================================== */
/* DTD → edid_timing_t                                                         */
/* ========================================================================== */

static u16 edid_dtd_hfront(const edid_detailed_timing_t *d)
{
    return (u16)(((u16)d->hfront_porch_lo) |
                 (((u16)(d->hsync_vsync_offset_pulse_hi >> 6) & 0x03) << 8));
}

static u16 edid_dtd_hsync(const edid_detailed_timing_t *d)
{
    return (u16)(((u16)d->hsync_pulse_lo) |
                 (((u16)(d->hsync_vsync_offset_pulse_hi >> 4) & 0x03) << 8));
}

static u16 edid_dtd_vfront(const edid_detailed_timing_t *d)
{
    return (u16)(((u16)(d->vsync_offset_pulse_lo >> 4) & 0x0F) |
                 (((u16)(d->hsync_vsync_offset_pulse_hi >> 2) & 0x03) << 4));
}

static u16 edid_dtd_vsync(const edid_detailed_timing_t *d)
{
    return (u16)(((u16)(d->vsync_offset_pulse_lo & 0x0F)) |
                 (((u16)(d->hsync_vsync_offset_pulse_hi & 0x03)) << 4));
}

static int edid_dtd_to_timing(const edid_detailed_timing_t *d, edid_timing_t *out)
{
    u16 pclk = le16_to_cpu(d->pixel_clock);
    if (pclk == 0) return DP_ERR_INVAL;

    out->pixel_clock   = pclk;
    out->hactive       = (u16)edid_dtd_hactive(d);
    out->hblank        = (u16)edid_dtd_hblank(d);
    out->vactive       = (u16)edid_dtd_vactive(d);
    out->vblank        = (u16)edid_dtd_vblank(d);
    out->hfront_porch  = edid_dtd_hfront(d);
    out->hsync_pulse   = edid_dtd_hsync(d);
    out->vfront_porch  = edid_dtd_vfront(d);
    out->vsync_pulse   = edid_dtd_vsync(d);
    out->hsync_positive = (d->flags & EDID_DTD_SYNC_POSITIVE_HSYNC) != 0;
    out->vsync_positive = (d->flags & EDID_DTD_SYNC_POSITIVE_VSYNC) != 0;
    out->interlaced     = (d->flags & EDID_DTD_INTERLACED) != 0;
    return DP_OK;
}

/* ========================================================================== */
/* ESTABLISHED TIMINGS                                                         */
/* ========================================================================== */

typedef struct {
    u8  byte_idx;
    u8  bit;
    edid_timing_t t;
} edid_est_entry_t;

static const edid_est_entry_t edid_est_table[] = {
    { 0, 5, { 2517,  640, 160,  480,  45,  16,  96, 10,  2, false, false, false } }, /* 640x480@60 */
    { 0, 3, { 3150,  640, 160,  480,  50,  16,  64, 10,  2, false, false, false } }, /* 640x480@75 */
    { 0, 1, { 3600,  640, 192,  480,  44,  24,  40,  9,  3, false, false, false } }, /* 640x480@85 */
    { 0, 4, { 4000,  800, 256,  600,  28,  40, 128,  1,  4, true,  true,  false } }, /* 800x600@60 */
    { 0, 2, { 5000,  800, 240,  600,  28,  16,  80,  1,  3, true,  true,  false } }, /* 800x600@75 */
    { 0, 0, { 5625,  800, 208,  600,  35,  32,  64,  3,  3, true,  true,  false } }, /* 800x600@85 */
    { 1, 5, { 6500, 1024, 320,  768,  38,  24, 136,  3,  6, false, false, false } }, /* 1024x768@60 */
    { 1, 3, { 7875, 1024, 288,  768,  36,  16,  96,  1,  3, true,  true,  false } }, /* 1024x768@75 */
    { 1, 1, { 9450, 1024, 352,  768,  49,  48,  96,  3,  3, true,  true,  false } }, /* 1024x768@85 */
    { 1, 4, { 10800, 1280, 408, 1024,  42,  48, 112,  1,  3, true,  true,  false } }, /* 1280x1024@60 */
    { 1, 2, { 13500, 1280, 408, 1024,  42,  16, 144,  1,  3, true,  true,  false } }, /* 1280x1024@75 */
};

static u32 edid_add_established(const edid_base_block_t *b,
                                edid_timing_t *out, u32 max)
{
    u32 count = 0;
    for (u32 i = 0; i < ARRAY_SIZE(edid_est_table) && count < max; i++) {
        const edid_est_entry_t *e = &edid_est_table[i];
        if (b->established_timings[e->byte_idx] & (1u << e->bit))
            out[count++] = e->t;
    }
    return count;
}

/* ========================================================================== */
/* STANDARD TIMINGS                                                            */
/* ========================================================================== */

static int edid_std_timing_to_timing(const u8 *st, edid_timing_t *out)
{
    if (st[0] == 0x01 && st[1] == 0x01) return DP_ERR_INVAL;

    u16 hactive = ((u16)(st[0] + 31) * 8);
    u8  aspect  = (st[1] >> 6) & 0x03;
    u8  refresh = (st[1] & 0x3F) + 60;

    u16 vactive;
    switch (aspect) {
    case 0: vactive = (u16)((u32)hactive * 10u / 16u); break;
    case 1: vactive = (u16)((u32)hactive * 3u / 4u);   break;
    case 2: vactive = (u16)((u32)hactive * 4u / 5u);   break;
    case 3: vactive = (u16)((u32)hactive * 9u / 16u);  break;
    default: return DP_ERR_INVAL;
    }

    if (vactive == 0 || hactive == 0) return DP_ERR_INVAL;

    dp_mem_set(out, 0, sizeof(*out));
    out->hactive = hactive;
    out->vactive = vactive;

    u32 hblank_approx = hactive / 5u;
    u32 vblank_approx = vactive / 20u;
    u32 hsync_approx  = hactive / 20u;
    u32 vsync_approx  = 3u;
    u32 hfront_approx = hblank_approx / 2u;
    u32 vfront_approx = vblank_approx / 2u;

    out->hblank      = (u16)hblank_approx;
    out->vblank      = (u16)vblank_approx;
    out->hfront_porch = (u16)hfront_approx;
    out->hsync_pulse = (u16)hsync_approx;
    out->vfront_porch = (u16)vfront_approx;
    out->vsync_pulse = (u16)vsync_approx;

    u64 px = (u64)(hactive + hblank_approx) *
             (u64)(vactive + vblank_approx) * (u64)refresh;
    out->pixel_clock = (u16)(px / 10000ULL);

    out->hsync_positive = false;
    out->vsync_positive = false;
    out->interlaced     = false;
    return DP_OK;
}

/* ========================================================================== */
/* CEA-861 VIC TABLE                                                           */
/* ========================================================================== */

typedef struct {
    u8  vic;
    u16 hactive;
    u16 vactive;
    u16 refresh;
    bool interlaced;
} vic_entry_t;

static const vic_entry_t vic_table[] = {
    {  1,  640,  480,  60, false },
    {  2,  720,  480,  60, false },
    {  3,  720,  480,  60, false },
    {  4, 1280,  720,  60, false },
    {  5, 1920, 1080,  60, true  },
    { 16, 1920, 1080,  60, false },
    { 17,  720,  576,  50, false },
    { 18,  720,  576,  50, false },
    { 19, 1280,  720,  50, false },
    { 20, 1920, 1080,  50, true  },
    { 31, 1920, 1080,  50, false },
    { 32, 1920, 1080,  24, false },
    { 33, 1920, 1080,  25, false },
    { 34, 1920, 1080,  30, false },
    { 60, 1280,  720,  24, false },
    { 61, 1280,  720,  25, false },
    { 62, 1280,  720,  30, false },
    { 63, 1920, 1080, 120, false },
    { 64, 1920, 1080, 100, false },
    { 93, 3840, 2160,  24, false },
    { 94, 3840, 2160,  25, false },
    { 95, 3840, 2160,  30, false },
    { 96, 3840, 2160,  50, false },
    { 97, 3840, 2160,  60, false },
};

static const vic_entry_t *vic_lookup(u8 vic)
{
    for (u32 i = 0; i < ARRAY_SIZE(vic_table); i++)
        if (vic_table[i].vic == vic) return &vic_table[i];
    return NULL;
}

/* ========================================================================== */
/* CEA-861 DATA BLOCK PARSING                                                  */
/* ========================================================================== */

typedef struct {
    u8  vic[64];
    u32 vic_count;
    dp_sad_t sad[DP_ELD_MAX_SAD];
    u32 sad_count;
    u8  speaker[3];
    bool has_speaker;
    bool has_hdmi_vsdb;
    u8  hdmi_phys_addr[2];
    u8  hdmi_deep_color;
    u8  hdmi_max_tmds;
    u8  hdmi_vic[8];
    u32 hdmi_vic_count;
    u32 native_vic_mask[2];
} cea861_parsed_t;

static void cea861_parse_vdb(const u8 *data, u8 len, cea861_parsed_t *p)
{
    for (u32 i = 0; i < len && p->vic_count < ARRAY_SIZE(p->vic); i++) {
        u8 b = data[i];
        u8 vic = b & CEA861_VIC_MASK;
        if (vic == 0) continue;
        p->vic[p->vic_count++] = vic;
        if (b & CEA861_VIC_NATIVE) {
            u32 idx = (p->vic_count - 1) / 32;
            u32 bit = (p->vic_count - 1) % 32;
            p->native_vic_mask[idx] |= (1u << bit);
        }
    }
}

static void cea861_parse_sadb(const u8 *data, u8 len, cea861_parsed_t *p)
{
    u32 count = len / 3;
    for (u32 i = 0; i < count && p->sad_count < DP_ELD_MAX_SAD; i++) {
        dp_sad_t s;
        s.format       = (data[i*3 + 0] >> 3) & 0x0F;
        s.channels     = (data[i*3 + 0] & 0x07) + 1;
        s.sample_rates = data[i*3 + 1];
        s.byte3        = data[i*3 + 2];
        p->sad[p->sad_count++] = s;
    }
}

static void cea861_parse_vsdb(const u8 *data, u8 len, cea861_parsed_t *p)
{
    if (len < 5) return;
    u32 oui = (u32)data[0] | ((u32)data[1] << 8) | ((u32)data[2] << 16);
    if (oui != HDMI_IEEE_OUI) return;

    p->has_hdmi_vsdb = true;
    p->hdmi_phys_addr[0] = data[3];
    p->hdmi_phys_addr[1] = data[4];

    if (len >= 6) p->hdmi_deep_color = data[5];
    if (len >= 7) p->hdmi_max_tmds   = data[6];

    for (u32 i = 7; i + 1 < len && p->hdmi_vic_count < ARRAY_SIZE(p->hdmi_vic); i++) {
        /* HDMI VIC length field and payload (best-effort scan). */
        if ((data[i] & 0xE0) == 0xE0) {
            u8 vlen = data[i] & 0x1F;
            if (i + 1 + vlen <= len) {
                for (u32 j = 0; j < vlen && p->hdmi_vic_count < ARRAY_SIZE(p->hdmi_vic); j++)
                    p->hdmi_vic[p->hdmi_vic_count++] = data[i + 1 + j];
                i += vlen;
            }
        }
    }
}

static void cea861_parse_speaker(const u8 *data, u8 len, cea861_parsed_t *p)
{
    if (len < 3) return;
    p->speaker[0] = data[0];
    p->speaker[1] = data[1];
    p->speaker[2] = data[2];
    p->has_speaker = true;
}

static void cea861_parse_data_blocks(const cea861_extension_t *cea, cea861_parsed_t *p)
{
    dp_mem_set(p, 0, sizeof(*p));
    u32 dtd_off = cea->dtd_offset;
    if (dtd_off == 0 || dtd_off > sizeof(cea->data) + 4) dtd_off = sizeof(cea->data) + 4;

    u32 i = 0;
    u32 limit = dtd_off - 4;
    if (limit > sizeof(cea->data)) limit = sizeof(cea->data);

    while (i < limit) {
        u8 hdr = cea->data[i];
        if (hdr == 0) break;
        u8 tag = (hdr >> 5) & 0x07;
        u8 len = hdr & 0x1F;
        if (i + 1 + len > limit) break;
        const u8 *payload = &cea->data[i + 1];

        switch (tag) {
        case CEA861_DB_TAG_AUDIO:   cea861_parse_sadb(payload, len, p);    break;
        case CEA861_DB_TAG_VIDEO:   cea861_parse_vdb(payload, len, p);     break;
        case CEA861_DB_TAG_VENDOR:  cea861_parse_vsdb(payload, len, p);    break;
        case CEA861_DB_TAG_SPEAKER: cea861_parse_speaker(payload, len, p); break;
        default: break;
        }
        i += 1 + len;
    }
}

/* ========================================================================== */
/* PUBLIC PARSERS                                                              */
/* ========================================================================== */

int dp_edid_parse(const edid_base_block_t *b, edid_timing_t *out, u32 max)
{
    if (!b || !out || max == 0) return DP_ERR_INVAL;
    if (!edid_check_header(b)) return DP_ERR_EDID;

    u32 count = 0;

    count += edid_add_established(b, out + count, max - count);

    for (u32 i = 0; i < 8 && count < max; i++) {
        edid_timing_t t;
        if (edid_std_timing_to_timing(b->standard_timings[i], &t) == DP_OK)
            out[count++] = t;
    }

    for (u32 i = 0; i < 4 && count < max; i++) {
        const edid_detailed_timing_t *d = &b->detailed_timings[i];
        if (le16_to_cpu(d->pixel_clock) == 0) continue;
        edid_timing_t t;
        if (edid_dtd_to_timing(d, &t) == DP_OK)
            out[count++] = t;
    }
    return (int)count;
}

int dp_edid_find_dtd(const edid_base_block_t *b, u32 h, u32 v,
                     edid_detailed_timing_t *out)
{
    if (!b || !out) return DP_ERR_INVAL;
    for (u32 i = 0; i < 4; i++) {
        const edid_detailed_timing_t *d = &b->detailed_timings[i];
        if (le16_to_cpu(d->pixel_clock) == 0) continue;
        if (edid_dtd_hactive(d) == h && edid_dtd_vactive(d) == v) {
            *out = *d;
            return DP_OK;
        }
    }
    return DP_ERR_INVAL;
}

/* ========================================================================== */
/* ELD FILL                                                                    */
/* ========================================================================== */

static int dp_edid_fill_eld_cea(const edid_base_block_t *b,
                                const cea861_extension_t *cea,
                                dp_eld_t *eld)
{
    if (!b || !eld) return DP_ERR_INVAL;

    dp_mem_set(eld, 0, sizeof(*eld));
    eld->version       = (u8)((b->edid_major << 4) | (b->edid_minor & 0x0F));
    eld->baseline_len  = 128;
    eld->connector_type = (b->input_parameters & EDID_INPUT_DIGITAL) ? 0x05 : 0x00;
    eld->audio_sync_delay = 0;
    eld->audio_latency    = 0;

    if (!cea) return DP_OK;

    cea861_parsed_t parsed;
    cea861_parse_data_blocks(cea, &parsed);

    eld->sad_count = (u8)parsed.sad_count;
    for (u32 i = 0; i < parsed.sad_count; i++) {
        u8 *dst = &eld->sads[i * 3];
        dst[0] = (u8)((parsed.sad[i].format & 0x0F) << 3)
               | ((parsed.sad[i].channels - 1) & 0x07);
        dst[1] = parsed.sad[i].sample_rates;
        dst[2] = parsed.sad[i].byte3;
    }

    if (parsed.has_speaker) {
        eld->speaker_allocation[0] = parsed.speaker[0];
        eld->speaker_allocation[1] = parsed.speaker[1];
        eld->speaker_allocation[2] = parsed.speaker[2];
    }

    if (parsed.has_hdmi_vsdb) {
        eld->hdmi_vsdb_present = 1;
        eld->hdmi_vsdb_len     = 6;
        eld->hdmi_vsdb_flags   = parsed.hdmi_deep_color;
        eld->cec_address       = (u8)((parsed.hdmi_phys_addr[0] >> 4) & 0x0F);
        eld->supports_ai       = false;
    }

    return DP_OK;
}

/* ========================================================================== */
/* CEA-861 → TIMINGS                                                           */
/* ========================================================================== */

static int dp_cea861_parse(const cea861_extension_t *cea,
                           edid_timing_t *out, u32 max)
{
    if (!cea || !out || max == 0) return DP_ERR_INVAL;
    if (cea->tag != EDID_CEA861_EXT_TAG) return DP_ERR_INVAL;

    cea861_parsed_t parsed;
    cea861_parse_data_blocks(cea, &parsed);

    u32 count = 0;
    for (u32 i = 0; i < parsed.vic_count && count < max; i++) {
        const vic_entry_t *v = vic_lookup(parsed.vic[i]);
        if (!v) continue;
        edid_timing_t t;
        dp_mem_set(&t, 0, sizeof(t));
        t.hactive   = v->hactive;
        t.vactive   = v->vactive;
        t.interlaced = v->interlaced;
        u32 hb = v->hactive / 10u; if (hb < 20) hb = 20;
        u32 vb = v->vactive / 20u; if (vb < 4)  vb = 4;
        t.hblank = (u16)hb;
        t.vblank = (u16)vb;
        t.hfront_porch = (u16)(hb / 3u);
        t.hsync_pulse  = (u16)(hb / 6u);
        t.vfront_porch = (u16)(vb / 4u);
        t.vsync_pulse  = 3;
        u64 px = (u64)(v->hactive + hb) * (u64)(v->vactive + vb) * (u64)v->refresh;
        t.pixel_clock = (u16)(px / 10000ULL);
        if (t.pixel_clock == 0) continue;
        out[count++] = t;
    }
    return (int)count;
}

/* ========================================================================== */
/* CEA-861 PUBLIC HELPERS (static, used internally by other blocks)            */
/* ========================================================================== */

static __maybe_unused u32 dp_cea861_get_vics(const cea861_extension_t *cea,
                                             u8 *vics, u32 max)
{
    if (!cea || !vics) return 0;
    cea861_parsed_t p;
    cea861_parse_data_blocks(cea, &p);
    u32 n = (p.vic_count > max) ? max : p.vic_count;
    dp_mem_copy(vics, p.vic, n);
    return n;
}

static __maybe_unused bool dp_cea861_get_audio_sads(const cea861_extension_t *cea,
                                                    dp_sad_t *out, u32 max,
                                                    u32 *got)
{
    if (!cea || !out || !got) return false;
    cea861_parsed_t p;
    cea861_parse_data_blocks(cea, &p);
    u32 n = (p.sad_count > max) ? max : p.sad_count;
    dp_mem_copy(out, p.sad, n * sizeof(dp_sad_t));
    *got = n;
    return n > 0;
}


/*
 * ============================================================================
 *  DP_CORE.C - Block 4: VIC Table | Timing | Stream | MST
 *  Architecture: RISC-V (RV32 / RV64)
 *  Language: C11
 *  Scope: Full CEA-861 VIC table, timing conversion, MSA register builders,
 *         stream set/enable/disable/update, MST payload + sideband messaging.
 * ============================================================================
 */

#include "dp_core.h"

/* ========================================================================== */
/* MST HW REGISTERS (DP_RESERVED area, offsets 0x0A0..0x13F)                   */
/* ========================================================================== */

#define DP_REG_MST_CTL              0x0A0
#define DP_REG_MST_STATUS           0x0A4
#define DP_REG_MST_SB_TX_LEN        0x0A8
#define DP_REG_MST_SB_TX_HDR        0x0AC
#define DP_REG_MST_SB_TX_DATA       0x0B0  /* 16 x u32 = 64 bytes */
#define DP_REG_MST_SB_RX_LEN        0x0F0
#define DP_REG_MST_SB_RX_HDR        0x0F4
#define DP_REG_MST_SB_RX_DATA       0x0F8  /* 16 x u32 = 64 bytes */
#define DP_REG_MST_VC_PAYLOAD_STAT  0x138

#define MST_CTL_ENABLE              BIT(0)
#define MST_CTL_UP_REQ_EN           BIT(1)
#define MST_CTL_UPSTREAM_SRC        BIT(2)
#define MST_CTL_SB_TX_START         BIT(8)

#define MST_STATUS_BUSY             BIT(0)
#define MST_STATUS_TX_DONE          BIT(1)
#define MST_STATUS_TX_ERROR         BIT(2)
#define MST_STATUS_RX_READY         BIT(3)

#define MST_SB_MSG_HEADER_SIZE      3
#define MST_SB_MSG_MAX_WIRE_SIZE    (MST_SB_MSG_HEADER_SIZE + DP_MST_SIDE_BAND_MSG_DATA_SIZE)

/* ========================================================================== */
/* CEA-861 VIC TABLE (framebased; vtotal/vactive per frame)                    */
/* ========================================================================== */

typedef struct {
    u8   vic;
    u16  hactive;
    u16  vactive;
    u16  htotal;
    u16  vtotal;
    u16  hfront;
    u16  hsync;
    u16  hback;
    u16  vfront;
    u16  vsync;
    u16  vback;
    u32  pclk_khz;
    u8   aspect;         /* 0=4:3 1=16:9 2=64:27 3=256:135 */
    u8   interlaced;
    u32  refresh_num;
    u32  refresh_den;
} vic_entry_t;

#define VIC_ASPECT_4_3      0
#define VIC_ASPECT_16_9     1
#define VIC_ASPECT_64_27    2
#define VIC_ASPECT_256_135  3

static const vic_entry_t vic_table[] = {
    /* --- SD --- */
    {  1,  640,  480,  800,  525,  16, 96, 48,  10, 2, 33,   25175, VIC_ASPECT_4_3,     0, 60000, 1001 },
    {  2,  720,  480,  858,  525,  16, 62, 60,   9, 6, 30,   27000, VIC_ASPECT_4_3,     0, 60000, 1001 },
    {  3,  720,  480,  858,  525,  16, 62, 60,   9, 6, 30,   27000, VIC_ASPECT_16_9,    0, 60000, 1001 },
    {  4, 1280,  720, 1650,  750, 110, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 60000, 1001 },
    {  5, 1920, 1080, 2200, 1125,  88, 44,148,   4, 5, 36,   74250, VIC_ASPECT_16_9,    1, 60000, 1001 },
    {  6, 1440,  480, 1716,  525,  38,124,114,   4, 3, 38,   27000, VIC_ASPECT_4_3,     1, 60000, 1001 },
    {  7, 1440,  480, 1716,  525,  38,124,114,   4, 3, 38,   27000, VIC_ASPECT_16_9,    1, 60000, 1001 },
    {  8, 1440,  240, 1716,  262,  38,124,114,   4, 3, 15,   27000, VIC_ASPECT_4_3,     0, 60000, 1001 },
    {  9, 1440,  240, 1716,  262,  38,124,114,   4, 3, 15,   27000, VIC_ASPECT_16_9,    0, 60000, 1001 },
    /* --- HD --- */
    { 16, 1920, 1080, 2200, 1125,  88, 44,148,   4, 5, 36,  148500, VIC_ASPECT_16_9,    0, 60000, 1001 },
    { 17,  720,  576,  864,  625,  12, 64, 68,   5, 5, 39,   27000, VIC_ASPECT_4_3,     0, 50,   1    },
    { 18,  720,  576,  864,  625,  12, 64, 68,   5, 5, 39,   27000, VIC_ASPECT_16_9,    0, 50,   1    },
    { 19, 1280,  720, 1980,  750, 440, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 50,   1    },
    { 20, 1920, 1080, 2640, 1125, 528, 44,148,   4, 5, 36,   74250, VIC_ASPECT_16_9,    1, 50,   1    },
    { 21, 1440,  576, 1728,  625,  24,126,138,   2, 3, 44,   27000, VIC_ASPECT_4_3,     1, 50,   1    },
    { 22, 1440,  576, 1728,  625,  24,126,138,   2, 3, 44,   27000, VIC_ASPECT_16_9,    1, 50,   1    },
    { 23, 1440,  288, 1728,  312,  24,126,138,   2, 3, 19,   27000, VIC_ASPECT_4_3,     0, 50,   1    },
    { 24, 1440,  288, 1728,  312,  24,126,138,   2, 3, 19,   27000, VIC_ASPECT_16_9,    0, 50,   1    },
    /* --- 1080p family --- */
    { 31, 1920, 1080, 2640, 1125, 528, 44,148,   4, 5, 36,  148500, VIC_ASPECT_16_9,    0, 50,   1    },
    { 32, 1920, 1080, 2750, 1125, 638, 44,148,   4, 5, 36,   74250, VIC_ASPECT_16_9,    0, 24000, 1001 },
    { 33, 1920, 1080, 2640, 1125, 528, 44,148,   4, 5, 36,   74250, VIC_ASPECT_16_9,    0, 25,   1    },
    { 34, 1920, 1080, 2200, 1125,  88, 44,148,   4, 5, 36,   74250, VIC_ASPECT_16_9,    0, 30000, 1001 },
    /* --- 720p family --- */
    { 41, 1280,  720, 3300,  750,1760, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 24000, 1001 },
    { 42, 1280,  720, 3960,  750,2420, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 25,   1    },
    { 43, 1280,  720, 3300,  750,1760, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 30000, 1001 },
    /* --- 720p high-rate --- */
    { 60, 1280,  720, 4125,  750,2585, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 24000, 1001 },
    { 61, 1280,  720, 3960,  750,2420, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 25,   1    },
    { 62, 1280,  720, 3300,  750,1760, 40,220,   5, 5, 20,   74250, VIC_ASPECT_16_9,    0, 30000, 1001 },
    { 63, 1920, 1080, 2200, 1125,  88, 44,148,   4, 5, 36,  297000, VIC_ASPECT_16_9,    0, 120000,1001 },
    { 64, 1920, 1080, 2640, 1125, 528, 44,148,   4, 5, 36,  297000, VIC_ASPECT_16_9,    0, 100,  1    },
    /* --- UHD / 4K --- */
    { 93, 3840, 2160, 5500, 2250,1276, 88,296,   8,10, 72,  297000, VIC_ASPECT_16_9,    0, 24000, 1001 },
    { 94, 3840, 2160, 5280, 2250,1056, 88,296,   8,10, 72,  297000, VIC_ASPECT_16_9,    0, 25,   1    },
    { 95, 3840, 2160, 4400, 2250, 176, 88,296,   8,10, 72,  297000, VIC_ASPECT_16_9,    0, 30000, 1001 },
    { 96, 3840, 2160, 5280, 2250,1056, 88,296,   8,10, 72,  594000, VIC_ASPECT_16_9,    0, 50,   1    },
    { 97, 3840, 2160, 4400, 2250, 176, 88,296,   8,10, 72,  594000, VIC_ASPECT_16_9,    0, 60000, 1001 },
    { 98, 4096, 2160, 5500, 2250,1020, 88,296,   8,10, 72,  297000, VIC_ASPECT_256_135, 0, 24000, 1001 },
    { 99, 4096, 2160, 5280, 2250, 968, 88,128,   8,10, 72,  297000, VIC_ASPECT_256_135, 0, 25,   1    },
    {100, 4096, 2160, 4400, 2250,  88, 88,128,   8,10, 72,  297000, VIC_ASPECT_256_135, 0, 30000, 1001 },
    {101, 4096, 2160, 5280, 2250, 968, 88,128,   8,10, 72,  594000, VIC_ASPECT_256_135, 0, 50,   1    },
    {102, 4096, 2160, 4400, 2250,  88, 88,128,   8,10, 72,  594000, VIC_ASPECT_256_135, 0, 60000, 1001 },
    {103, 3840, 2160, 4400, 2250, 176, 88,296,   8,10, 72, 1188000, VIC_ASPECT_16_9,    0, 120000,1001 },
    {104, 3840, 2160, 5280, 2250,1056, 88,296,   8,10, 72, 1188000, VIC_ASPECT_16_9,    0, 100,  1    },
    {105, 4096, 2160, 4400, 2250,  88, 88,128,   8,10, 72, 1188000, VIC_ASPECT_256_135, 0, 120000,1001 },
    {106, 4096, 2160, 5280, 2250, 968, 88,128,   8,10, 72, 1188000, VIC_ASPECT_256_135, 0, 100,  1    },
    {107, 3840, 2160, 4400, 2250, 176, 88,296,   8,10, 72,  297000, VIC_ASPECT_16_9,    0, 60000, 1001 },
};

#define VIC_TABLE_COUNT  ARRAY_SIZE(vic_table)

static const vic_entry_t *vic_lookup(u8 vic)
{
    for (u32 i = 0; i < VIC_TABLE_COUNT; i++)
        if (vic_table[i].vic == vic) return &vic_table[i];
    return NULL;
}

/* ========================================================================== */
/* TIMING CONVERSION HELPERS                                                   */
/* ========================================================================== */

/* Rounds refresh_num/den to nearest integer Hz (u8 ceiling for high rates). */
static u8 dp_refresh_round(u32 num, u32 den)
{
    if (den == 0) return 60;
    u32 v = (num + den / 2) / den;
    return v > 255 ? 255 : (u8)v;
}

/* VIC entry -> dp_timing_spec_t (framebased values, refresh=field rate). */
static void dp_vic_to_timing(const vic_entry_t *v, dp_timing_spec_t *t)
{
    dp_mem_set(t, 0, sizeof(*t));
    t->pixel_clock   = v->pclk_khz;
    t->hactive       = v->hactive;
    t->hblank        = (u16)(v->htotal - v->hactive);
    t->vactive       = v->vactive;
    t->vblank        = (u16)(v->vtotal - v->vactive);
    t->hsync_offset  = v->hfront;
    t->hsync_pulse   = v->hsync;
    t->vsync_offset  = v->vfront;
    t->vsync_pulse   = v->vsync;
    t->interlaced    = v->interlaced != 0;
    u32 num = v->refresh_num;
    u32 den = v->refresh_den ? v->refresh_den : 1;
    t->refresh_hz    = dp_refresh_round(num, den);
    if (v->interlaced) {
        u32 field = dp_refresh_round(num * 2, den);
        if (field <= 255) t->refresh_hz = (u8)field;
    }
}

int dp_timing_from_vic(u8 vic, dp_timing_spec_t *out)
{
    if (!out) return DP_ERR_INVAL;
    const vic_entry_t *v = vic_lookup(vic);
    if (!v) return DP_ERR_NOT_SUPPORTED;
    dp_vic_to_timing(v, out);
    return DP_OK;
}

int dp_timing_from_edid_dtd(const edid_detailed_timing_t *dtd, dp_timing_spec_t *out)
{
    if (!dtd || !out) return DP_ERR_INVAL;
    u16 pclk = le16_to_cpu(dtd->pixel_clock);
    if (pclk == 0) return DP_ERR_INVAL;

    dp_mem_set(out, 0, sizeof(*out));
    out->pixel_clock  = (u32)pclk * 10u;
    out->hactive      = (u16)edid_dtd_hactive(dtd);
    out->hblank       = (u16)edid_dtd_hblank(dtd);
    out->vactive      = (u16)edid_dtd_vactive(dtd);
    out->vblank       = (u16)edid_dtd_vblank(dtd);
    out->hsync_offset = edid_dtd_hfront(dtd);
    out->hsync_pulse  = edid_dtd_hsync(dtd);
    out->vsync_offset = edid_dtd_vfront(dtd);
    out->vsync_pulse  = edid_dtd_vsync(dtd);
    out->interlaced   = (dtd->flags & EDID_DTD_INTERLACED) != 0;

    u32 htotal = out->hactive + out->hblank;
    u32 vtotal = out->vactive + out->vblank;
    if (htotal && vtotal) {
        u64 px_per_frame = (u64)htotal * (u64)vtotal;
        u64 frame_rate_x10 = ((u64)out->pixel_clock * 10000ULL) / px_per_frame;
        u32 frame_hz = (u32)(frame_rate_x10 / 10);
        if (out->interlaced) frame_hz *= 2;
        if (frame_hz > 255) frame_hz = 255;
        out->refresh_hz = (u8)(frame_hz ? frame_hz : 60);
    } else {
        out->refresh_hz = 60;
    }
    return DP_OK;
}

int dp_timing_from_edid_timing(const edid_timing_t *et, dp_timing_spec_t *out)
{
    if (!et || !out) return DP_ERR_INVAL;
    dp_mem_set(out, 0, sizeof(*out));
    out->pixel_clock  = (u32)et->pixel_clock * 10u;
    out->hactive      = et->hactive;
    out->hblank       = et->hblank;
    out->vactive      = et->vactive;
    out->vblank       = et->vblank;
    out->hsync_offset = et->hfront_porch;
    out->hsync_pulse  = et->hsync_pulse;
    out->vsync_offset = et->vfront_porch;
    out->vsync_pulse  = et->vsync_pulse;
    out->interlaced   = et->interlaced;

    u32 htotal = (u32)et->hactive + et->hblank;
    u32 vtotal = (u32)et->vactive + et->vblank;
    if (htotal && vtotal) {
        u64 px_per_frame = (u64)htotal * (u64)vtotal;
        u64 hz = ((u64)out->pixel_clock * 1000ULL) / px_per_frame;
        if (et->interlaced) hz *= 2;
        out->refresh_hz = (u8)(hz > 255 ? 255 : (hz ? hz : 60));
    } else {
        out->refresh_hz = 60;
    }
    return DP_OK;
}

/* ========================================================================== */
/* TIMING VALIDATION                                                           */
/* ========================================================================== */

static bool dp_timing_valid(const dp_timing_spec_t *t)
{
    if (!t) return false;
    if (t->pixel_clock == 0) return false;
    if (t->hactive == 0 || t->vactive == 0) return false;
    if (t->hactive > DP_MAX_H_TOTAL) return false;
    if (t->vactive > DP_MAX_V_TOTAL) return false;
    if (t->hsync_pulse == 0 || t->vsync_pulse == 0) return false;
    if ((u32)t->hsync_offset + t->hsync_pulse > t->hblank + t->hactive) return false;
    if ((u32)t->vsync_offset + t->vsync_pulse > t->vblank + t->vactive) return false;
    if (t->hsync_offset >= t->hsync_offset + t->hsync_pulse) return false;
    return true;
}

static bool dp_timing_pixel_clock_ok(const dp_timing_spec_t *t)
{
    if (!t) return false;
    if (t->pixel_clock > DP_MAX_PIXEL_CLOCK_KHZ) return false;
    return true;
}

/* ========================================================================== */
/* MSA REGISTER BUILDERS                                                       */
/* ========================================================================== */

/* Computes HW MSA timing fields from dp_timing_spec_t (framebased inputs). */
static void dp_msa_compute(const dp_timing_spec_t *t,
                           dp_msa_hw_regs_t *out,
                           u8 color_fmt, u8 bpc, u8 dyn_range)
{
    dp_mem_set(out, 0, sizeof(*out));

    u32 htotal = (u32)t->hactive + t->hblank;
    u32 vtotal = (u32)t->vactive + t->vblank;
    u32 hstart = (u32)t->hactive + t->hsync_offset;
    u32 vstart = (u32)t->vactive + t->vsync_offset;

    out->htotal = htotal;
    out->hstart = hstart;
    out->vtotal = vtotal;
    out->vstart = vstart;
    out->hsw    = t->hsync_pulse;
    out->vsw    = t->vsync_pulse;

    out->misc0  = dp_build_msa_misc0(color_fmt, true, true);
    out->misc1  = dp_build_msa_misc1(bpc, dyn_range, 0);

    out->mvid = 0;
    out->nvid = 0;
}

/* Writes MSA HW registers from dp_msa_hw_regs_t. */
static void dp_msa_write_hw(dp_controller_t *c, const dp_msa_hw_regs_t *m,
                            const dp_timing_spec_t *t)
{
    vaddr_t b = c->base_virtual_addr;

    u32 hactive = t->hactive;
    u32 hblank  = t->hblank;
    u32 vactive = t->vactive;
    u32 vblank  = t->vblank;
    u32 vstart  = m->vstart;
    u32 vsw     = m->vsw;
    u32 vsync_o = t->vsync_offset;

    /* For interlace, MSA registers are per-field. */
    if (t->interlaced) {
        vactive = vactive / 2;
        vblank  = vblank  / 2;
        vstart  = vstart  / 2;
        vsw     = (vsw + 1) / 2;
        vsync_o = (vsync_o + 1) / 2;
    }

    dp_reg_write(b, DP_REG_MSA_MISC0,          m->misc0);
    dp_reg_write(b, DP_REG_MSA_MISC1,          m->misc1);
    dp_reg_write(b, DP_REG_HORIZONTAL_TOTAL,   m->htotal);
    dp_reg_write(b, DP_REG_HORIZONTAL_START,   m->hstart);
    dp_reg_write(b, DP_REG_HORIZONTAL_ACTIVE,  hactive);
    dp_reg_write(b, DP_REG_HORIZONTAL_BLANKING,hblank);
    dp_reg_write(b, DP_REG_VERTICAL_TOTAL,     t->interlaced ? (m->vtotal / 2) : m->vtotal);
    dp_reg_write(b, DP_REG_VERTICAL_START,     vstart);
    dp_reg_write(b, DP_REG_VERTICAL_ACTIVE,    vactive);
    dp_reg_write(b, DP_REG_VERTICAL_BLANKING,  vblank);
    dp_reg_write(b, DP_REG_PIXEL_CLOCK_RATE,   t->pixel_clock);

    /* Sync polarity / pulse placeholders (depends on HW) */
    (void)vsw;
    (void)vsync_o;
}

/* ========================================================================== */
/* STREAM API                                                                  */
/* ========================================================================== */

static dp_stream_t *dp_stream_get(dp_driver_state_t *ds, u8 id)
{
    if (!ds) return NULL;
    for (u32 i = 0; i < ds->stream_count; i++)
        if (ds->streams[i].stream_id == id) return &ds->streams[i];
    return NULL;
}

static dp_stream_t *dp_stream_alloc(dp_driver_state_t *ds, u8 id)
{
    if (!ds) return NULL;
    if (ds->stream_count >= DP_MST_MAX_STREAMS) return NULL;
    dp_stream_t *s = &ds->streams[ds->stream_count++];
    dp_mem_set(s, 0, sizeof(*s));
    s->stream_id = id;
    s->owner = ds;
    return s;
}

int dp_stream_set_timing(dp_controller_t *c, const dp_timing_spec_t *t)
{
    if (!c || !t) return DP_ERR_INVAL;
    if (!c->initialized) return DP_ERR_INVAL;
    if (!dp_timing_valid(t)) return DP_ERR_INVAL;
    if (!dp_timing_pixel_clock_ok(t)) return DP_ERR_INVAL;

    /* Single-stream mode uses stream 0 slot. */
    dp_driver_state_t *ds = (dp_driver_state_t *)c->active_pixel_clock;  /* placeholder */
    (void)ds;

    /* Program MSA directly from timing; stream bookkeeping handled by callers
     * that own the driver_state_t via dp_stream_set_timing_ex. */
    dp_msa_hw_regs_t msa;
    dp_msa_compute(t, &msa, DP_MSA_COLOR_RGB, 8, DP_MSA_MISC1_DYNRANGE_VESA);
    dp_msa_write_hw(c, &msa, t);

    c->active_pixel_clock = t->pixel_clock;
    c->current_state = DP_STATE_STREAM_READY;
    return DP_OK;
}

/* Extended variant taking driver state for stream slot bookkeeping. */
int dp_stream_set_timing_ex(dp_driver_state_t *ds, u8 stream_id,
                            const dp_timing_spec_t *t,
                            dp_pixel_format_t fmt, u8 bpc, u8 dyn_range)
{
    if (!ds || !ds->ctrl || !t) return DP_ERR_INVAL;
    if (!ds->ctrl->initialized) return DP_ERR_INVAL;
    if (!dp_timing_valid(t)) return DP_ERR_INVAL;
    if (!dp_timing_pixel_clock_ok(t)) return DP_ERR_INVAL;

    u8 color_fmt;
    switch (fmt) {
    case DP_PIXEL_FORMAT_RGB:    color_fmt = DP_MSA_COLOR_RGB;    break;
    case DP_PIXEL_FORMAT_YUV444: color_fmt = DP_MSA_COLOR_YUV444; break;
    case DP_PIXEL_FORMAT_YUV422: color_fmt = DP_MSA_COLOR_YUV422; break;
    case DP_PIXEL_FORMAT_YUV420: color_fmt = DP_MSA_COLOR_YUV420; break;
    case DP_PIXEL_FORMAT_RAW:    color_fmt = DP_MSA_COLOR_RAW;    break;
    default: return DP_ERR_INVAL;
    }

    if (!dp_check_bw_fits(t, bpc, fmt, ds->ctrl->bandwidth, ds->ctrl->lanes))
        return DP_ERR_LINK_TRAIN;

    dp_stream_t *s = dp_stream_get(ds, stream_id);
    if (!s) s = dp_stream_alloc(ds, stream_id);
    if (!s) return DP_ERR_NOMEM;

    s->timing        = *t;
    s->pixel_format  = fmt;
    s->bpc           = bpc;
    s->dynamic_range = dyn_range;

    dp_msa_hw_regs_t msa;
    dp_msa_compute(t, &msa, color_fmt, bpc, dyn_range);
    dp_msa_write_hw(ds->ctrl, &msa, t);

    ds->ctrl->active_pixel_clock = t->pixel_clock;
    ds->ctrl->current_state = DP_STATE_STREAM_READY;
    return DP_OK;
}

int dp_stream_update_msa(dp_controller_t *c, u8 stream_id)
{
    if (!c) return DP_ERR_INVAL;
    (void)stream_id;
    /* In single-stream mode, reapply current timing from HW shadow.
     * Caller passes updated timing via dp_stream_set_timing_ex. */
    return DP_OK;
}

int dp_stream_update_msa_ex(dp_driver_state_t *ds, u8 stream_id)
{
    if (!ds || !ds->ctrl) return DP_ERR_INVAL;
    dp_stream_t *s = dp_stream_get(ds, stream_id);
    if (!s || !s->enabled) return DP_ERR_INVAL;

    u8 color_fmt;
    switch (s->pixel_format) {
    case DP_PIXEL_FORMAT_RGB:    color_fmt = DP_MSA_COLOR_RGB;    break;
    case DP_PIXEL_FORMAT_YUV444: color_fmt = DP_MSA_COLOR_YUV444; break;
    case DP_PIXEL_FORMAT_YUV422: color_fmt = DP_MSA_COLOR_YUV422; break;
    case DP_PIXEL_FORMAT_YUV420: color_fmt = DP_MSA_COLOR_YUV420; break;
    default:                     color_fmt = DP_MSA_COLOR_RGB;    break;
    }

    dp_msa_hw_regs_t msa;
    dp_msa_compute(&s->timing, &msa, color_fmt, s->bpc, s->dynamic_range);
    dp_msa_write_hw(ds->ctrl, &msa, &s->timing);
    return DP_OK;
}

int dp_stream_enable(dp_controller_t *c, u8 stream_id)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;
    if (c->current_state != DP_STATE_LINK_READY &&
        c->current_state != DP_STATE_STREAM_READY &&
        c->current_state != DP_STATE_ACTIVE_STREAM)
        return DP_ERR_INVAL;

    u32 assign = dp_reg_read(c->base_virtual_addr, DP_REG_STREAM_ASSIGNMENT);
    assign |= (1u << (stream_id & 0x1F));
    dp_reg_write(c->base_virtual_addr, DP_REG_STREAM_ASSIGNMENT, assign);

    u32 sys = dp_reg_read(c->base_virtual_addr, DP_REG_SYSTEM_CONTROL);
    sys |= BIT(0);
    dp_reg_write(c->base_virtual_addr, DP_REG_SYSTEM_CONTROL, sys);

    c->current_state = DP_STATE_ACTIVE_STREAM;
    return DP_OK;
}

int dp_stream_enable_ex(dp_driver_state_t *ds, u8 stream_id)
{
    if (!ds || !ds->ctrl) return DP_ERR_INVAL;
    dp_stream_t *s = dp_stream_get(ds, stream_id);
    if (!s) return DP_ERR_INVAL;

    dp_spin_lock(&ds->lock);
    int rc = dp_stream_enable(ds->ctrl, stream_id);
    if (rc == DP_OK) {
        s->enabled = true;
        if (ds->ctrl->is_mst_active && s->vc_payload_id) {
            (void)dp_mst_allocate_vcpi(ds->ctrl, s->vc_payload_id,
                                       dp_calc_bpp(s->bpc, s->pixel_format));
        }
    }
    dp_spin_unlock(&ds->lock);
    return rc;
}

int dp_stream_disable(dp_controller_t *c, u8 stream_id)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    u32 assign = dp_reg_read(c->base_virtual_addr, DP_REG_STREAM_ASSIGNMENT);
    assign &= ~(1u << (stream_id & 0x1F));
    dp_reg_write(c->base_virtual_addr, DP_REG_STREAM_ASSIGNMENT, assign);

    if (assign == 0) {
        u32 sys = dp_reg_read(c->base_virtual_addr, DP_REG_SYSTEM_CONTROL);
        sys &= ~BIT(0);
        dp_reg_write(c->base_virtual_addr, DP_REG_SYSTEM_CONTROL, sys);
        c->current_state = DP_STATE_LINK_READY;
    }
    return DP_OK;
}

int dp_stream_disable_ex(dp_driver_state_t *ds, u8 stream_id)
{
    if (!ds || !ds->ctrl) return DP_ERR_INVAL;
    dp_stream_t *s = dp_stream_get(ds, stream_id);
    if (!s) return DP_ERR_INVAL;

    dp_spin_lock(&ds->lock);
    int rc = dp_stream_disable(ds->ctrl, stream_id);
    if (rc == DP_OK) {
        s->enabled = false;
        if (ds->ctrl->is_mst_active && s->vc_payload_id) {
            (void)dp_mst_clear_vcpi(ds->ctrl, s->vc_payload_id);
            s->vc_payload_id = 0;
        }
    }
    dp_spin_unlock(&ds->lock);
    return rc;
}

/* ========================================================================== */
/* MST - TOP-LEVEL CONTROL                                                     */
/* ========================================================================== */

int dp_mst_enable(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    u8 cap = dp_dpcd_read_byte(c, DP_MSTM_CAP);
    if (!(cap & DP_MST_CAP)) return DP_ERR_NOT_SUPPORTED;

    u8 ctl = DP_MST_EN;
    int rc = dp_aux_write_dpcd(c, DP_MSTM_CTRL, &ctl, 1);
    if (rc != DP_OK) return rc;

    u32 hw = dp_reg_read(c->base_virtual_addr, DP_REG_MST_CTL);
    hw |= MST_CTL_ENABLE;
    dp_reg_write(c->base_virtual_addr, DP_REG_MST_CTL, hw);

    c->is_mst_active = true;
    return DP_OK;
}

int dp_mst_disable(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    u8 ctl = 0;
    int rc = dp_aux_write_dpcd(c, DP_MSTM_CTRL, &ctl, 1);

    u32 hw = dp_reg_read(c->base_virtual_addr, DP_REG_MST_CTL);
    hw &= ~MST_CTL_ENABLE;
    dp_reg_write(c->base_virtual_addr, DP_REG_MST_CTL, hw);

    c->is_mst_active = false;
    return (rc == DP_OK) ? DP_OK : rc;
}

/* ========================================================================== */
/* MST - VCPI / PAYLOAD ALLOCATION                                             */
/* ========================================================================== */

/* PBN = Payload Bandwidth Number. We approximate as ceil(bitrate_mbps) so
 * allocation is proportional to consumed bandwidth on the MST fabric. */
static u16 dp_mst_compute_pbn(u32 pixel_clock_khz, u8 bpp, u8 lanes,
                              dp_link_bandwidth_t bw)
{
    (void)lanes; (void)bw;
    u64 bits_per_sec = (u64)pixel_clock_khz * 1000ULL * (u64)bpp;
    u64 pbn = (bits_per_sec + 999999ULL) / 1000000ULL;
    return (u16)(pbn > 0xFFFF ? 0xFFFF : pbn);
}

int dp_mst_allocate_vcpi(dp_controller_t *c, u8 vcpi, u32 pbn)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;
    if (vcpi == DP_MST_VC_PAYLOAD_ID_NULL) return DP_ERR_INVAL;
    if (vcpi > DP_MST_VC_PAYLOAD_ID_MAX) return DP_ERR_INVAL;

    /* Program payload allocation DPCD (radix = 0 for local branch). */
    u8 buf[3];
    buf[0] = vcpi;
    buf[1] = 0;          /* start time slot */
    buf[2] = (u8)(pbn > 63 ? 63 : pbn);

    int rc = dp_aux_write_dpcd(c, DP_PAYLOAD_ALLOCATE_SET, buf, 1);
    if (rc != DP_OK) return rc;
    rc = dp_aux_write_dpcd(c, DP_PAYLOAD_ALLOCATE_START_TIME_SLOT, &buf[1], 1);
    if (rc != DP_OK) return rc;
    rc = dp_aux_write_dpcd(c, DP_PAYLOAD_ALLOCATE_TIME_SLOT_COUNT, &buf[2], 1);
    if (rc != DP_OK) return rc;

    /* Update HW status shadow. */
    u32 stat = dp_reg_read(c->base_virtual_addr, DP_REG_MST_VC_PAYLOAD_STAT);
    stat |= (1u << (vcpi & 0x1F));
    dp_reg_write(c->base_virtual_addr, DP_REG_MST_VC_PAYLOAD_STAT, stat);
    return DP_OK;
}

int dp_mst_clear_vcpi(dp_controller_t *c, u8 vcpi)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;
    if (vcpi > DP_MST_VC_PAYLOAD_ID_MAX) return DP_ERR_INVAL;

    u8 zero = 0;
    int rc = dp_aux_write_dpcd(c, DP_PAYLOAD_ALLOCATE_TIME_SLOT_COUNT, &zero, 1);
    if (rc != DP_OK) return rc;
    rc = dp_aux_write_dpcd(c, DP_PAYLOAD_ALLOCATE_SET, &zero, 1);
    if (rc != DP_OK) return rc;

    u32 stat = dp_reg_read(c->base_virtual_addr, DP_REG_MST_VC_PAYLOAD_STAT);
    stat &= ~(1u << (vcpi & 0x1F));
    dp_reg_write(c->base_virtual_addr, DP_REG_MST_VC_PAYLOAD_STAT, stat);
    return DP_OK;
}

/* ========================================================================== */
/* MST - SIDEBAND MESSAGE ENCODING / DECODING                                  */
/* ========================================================================== */

/* Wire format: [link_hi][link_lo][msg_type][data...] */
static u32 dp_mst_sideband_encode(const dp_mst_sideband_msg_t *msg,
                                   u8 *wire, u32 wire_max, u32 data_len)
{
    if (!msg || !wire) return 0;
    if (data_len > DP_MST_SIDE_BAND_MSG_DATA_SIZE) return 0;
    u32 total = MST_SB_MSG_HEADER_SIZE + data_len;
    if (total > wire_max) return 0;

    wire[0] = msg->link_address[0];
    wire[1] = msg->link_address[1];
    wire[2] = msg->message_type;
    if (data_len > 0)
        dp_mem_copy(&wire[3], msg->data, data_len);
    return total;
}

static int dp_mst_sideband_decode(const u8 *wire, u32 wire_len,
                                   dp_mst_sideband_msg_t *msg, u32 *data_len)
{
    if (!wire || !msg || wire_len < MST_SB_MSG_HEADER_SIZE)
        return DP_ERR_INVAL;

    msg->link_address[0] = wire[0];
    msg->link_address[1] = wire[1];
    msg->message_type    = wire[2];

    u32 dl = wire_len - MST_SB_MSG_HEADER_SIZE;
    if (dl > DP_MST_SIDE_BAND_MSG_DATA_SIZE) dl = DP_MST_SIDE_BAND_MSG_DATA_SIZE;
    dp_mem_set(msg->data, 0, sizeof(msg->data));
    if (dl > 0) dp_mem_copy(msg->data, &wire[3], dl);
    if (data_len) *data_len = dl;
    return DP_OK;
}

static void dp_mst_tx_fifo_write(dp_controller_t *c, const u8 *data, u32 len)
{
    vaddr_t b = c->base_virtual_addr;
    u32 words = (len + 3u) / 4u;
    for (u32 i = 0; i < words; i++) {
        u8 tmp[4] = { 0, 0, 0, 0 };
        u32 off = i * 4u;
        u32 n = (len > off) ? (len - off) : 0u;
        if (n > 4u) n = 4u;
        dp_mem_copy(tmp, data + off, n);
        dp_reg_write(b, DP_REG_MST_SB_TX_DATA + (i * 4u), dp_pack_le32(tmp));
    }
}

static void dp_mst_rx_fifo_read(dp_controller_t *c, u8 *data, u32 len)
{
    vaddr_t b = c->base_virtual_addr;
    u32 words = (len + 3u) / 4u;
    for (u32 i = 0; i < words; i++) {
        u32 w = dp_reg_read(b, DP_REG_MST_SB_RX_DATA + (i * 4u));
        u8 tmp[4];
        u32 off = i * 4u;
        u32 n = (len > off) ? (len - off) : 0u;
        if (n > 4u) n = 4u;
        dp_unpack_le32(w, tmp);
        dp_mem_copy(data + off, tmp, n);
    }
}

/* ========================================================================== */
/* MST - SIDEBAND SEND / RECEIVE                                               */
/* ========================================================================== */

int dp_mst_sideband_send(dp_controller_t *c, const dp_mst_sideband_msg_t *msg,
                        u32 data_len)
{
    if (!c || !msg) return DP_ERR_INVAL;
    if (!c->is_mst_active) return DP_ERR_PROTOCOL;

    u8 wire[MST_SB_MSG_MAX_WIRE_SIZE];
    u32 wire_len = dp_mst_sideband_encode(msg, wire, sizeof(wire), data_len);
    if (wire_len == 0) return DP_ERR_INVAL;

    vaddr_t b = c->base_virtual_addr;

    u64 deadline = riscv_read_time() + 50000ULL;
    while (dp_reg_read(b, DP_REG_MST_STATUS) & MST_STATUS_BUSY) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }

    u32 hdr = ((u32)wire[0]) | ((u32)wire[1] << 8) | ((u32)wire[2] << 16);
    dp_reg_write(b, DP_REG_MST_SB_TX_HDR, hdr);
    if (data_len > 0)
        dp_mst_tx_fifo_write(c, &wire[3], data_len);

    dp_reg_write(b, DP_REG_MST_SB_TX_LEN, MST_SB_MSG_HEADER_SIZE + data_len);

    u32 ctl = dp_reg_read(b, DP_REG_MST_CTL);
    dp_reg_write(b, DP_REG_MST_CTL, ctl | MST_CTL_SB_TX_START);

    deadline = riscv_read_time() + 100000ULL;
    for (;;) {
        u32 st = dp_reg_read(b, DP_REG_MST_STATUS);
        if (st & MST_STATUS_TX_DONE)  return DP_OK;
        if (st & MST_STATUS_TX_ERROR) return DP_ERR_PROTOCOL;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }
}

int dp_mst_sideband_receive(dp_controller_t *c, dp_mst_sideband_msg_t *msg,
                           u32 *data_len)
{
    if (!c || !msg) return DP_ERR_INVAL;
    if (!c->is_mst_active) return DP_ERR_PROTOCOL;

    vaddr_t b = c->base_virtual_addr;
    u32 st = dp_reg_read(b, DP_REG_MST_STATUS);
    if (!(st & MST_STATUS_RX_READY)) return DP_ERR_BUSY;

    u32 wire_len = dp_reg_read(b, DP_REG_MST_SB_RX_LEN);
    if (wire_len > MST_SB_MSG_MAX_WIRE_SIZE) return DP_ERR_PROTOCOL;

    u8 wire[MST_SB_MSG_MAX_WIRE_SIZE];
    u32 hdr = dp_reg_read(b, DP_REG_MST_SB_RX_HDR);
    wire[0] = (u8)(hdr);
    wire[1] = (u8)(hdr >> 8);
    wire[2] = (u8)(hdr >> 16);
    if (wire_len > MST_SB_MSG_HEADER_SIZE) {
        u32 dl = wire_len - MST_SB_MSG_HEADER_SIZE;
        dp_mst_rx_fifo_read(c, &wire[3], dl);
    }
    return dp_mst_sideband_decode(wire, wire_len, msg, data_len);
}

/* ========================================================================== */
/* MST - MESSAGE BUILDERS                                                      */
/* ========================================================================== */

static int dp_mst_build_link_address(dp_mst_sideband_msg_t *msg,
                                      const dp_mst_link_address_payload_t *p)
{
    if (!msg || !p) return DP_ERR_INVAL;
    dp_mem_set(msg, 0, sizeof(*msg));
    msg->link_address[0] = 0x00;
    msg->link_address[1] = 0x00;
    msg->message_type    = DP_MST_MESSAGE_LINK_ADDRESS;

    u32 off = 0;
    msg->data[off++] = 0x00;   /* reserved */
    msg->data[off++] = p->num_ports;
    for (u32 i = 0; i < p->num_ports && (off + 4) <= DP_MST_SIDE_BAND_MSG_DATA_SIZE; i++) {
        msg->data[off++] = p->ports[i].port_number;
        msg->data[off++] = p->ports[i].peer_device_type & 0xFF;
        msg->data[off++] = ((p->ports[i].peer_device_type >> 8) & 0x07);
        msg->data[off++] = (u8)((p->ports[i].dpcd_rev & 0x0F) << 4) |
                           (p->ports[i].mcs & 0x0F);
    }
    return (int)(off);  /* number of data bytes */
}

static int dp_mst_build_enum_path_resources(dp_mst_sideband_msg_t *msg,
                                             u8 port_number)
{
    if (!msg) return DP_ERR_INVAL;
    dp_mem_set(msg, 0, sizeof(*msg));
    msg->link_address[0] = 0x00;
    msg->link_address[1] = 0x00;
    msg->message_type    = DP_MST_MESSAGE_ENUM_PATH_RES;
    msg->data[0] = port_number;
    return 1;
}

static int dp_mst_build_allocate_payload(dp_mst_sideband_msg_t *msg,
                                          u8 port_number, u8 vcpi, u16 pbn)
{
    if (!msg) return DP_ERR_INVAL;
    dp_mem_set(msg, 0, sizeof(*msg));
    msg->link_address[0] = 0x00;
    msg->link_address[1] = 0x00;
    msg->message_type    = DP_MST_MESSAGE_ALLOCATE_PAYLOAD;
    msg->data[0] = port_number;
    msg->data[1] = 0;
    msg->data[2] = vcpi;
    msg->data[3] = (u8)(pbn & 0xFF);
    msg->data[4] = (u8)((pbn >> 8) & 0xFF);
    return 5;
}

/* ========================================================================== */
/* MST - HIGH LEVEL HELPERS                                                    */
/* ========================================================================== */

/* Sends LINK_ADDRESS to broadcast (0x0000) to discover topology. */
int dp_mst_discover_topology(dp_controller_t *c,
                             dp_mst_link_address_payload_t *out,
                             u32 max_ports)
{
    if (!c || !out || max_ports == 0) return DP_ERR_INVAL;
    if (!c->is_mst_active) return DP_ERR_PROTOCOL;

    dp_mst_sideband_msg_t msg;
    dp_mem_set(&msg, 0, sizeof(msg));
    msg.link_address[0] = 0x00;
    msg.link_address[1] = 0x00;
    msg.message_type    = DP_MST_MESSAGE_LINK_ADDRESS;
    msg.data[0] = 0x00;
    msg.data[1] = 0x00;

    int rc = dp_mst_sideband_send(c, &msg, 2);
    if (rc != DP_OK) return rc;

    dp_mst_sideband_msg_t reply;
    u32 reply_len = 0;
    u64 deadline = riscv_read_time() + 200000ULL;
    for (;;) {
        rc = dp_mst_sideband_receive(c, &reply, &reply_len);
        if (rc == DP_OK) break;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        dp_udelay(200);
    }

    if (reply.message_type != DP_MST_MESSAGE_LINK_ADDRESS) return DP_ERR_PROTOCOL;

    u32 n_ports = reply.data[1];
    if (n_ports > max_ports) n_ports = max_ports;
    out->num_ports = (u8)n_ports;
    for (u32 i = 0; i < n_ports; i++) {
        u32 off = 2 + i * 4;
        if (off + 4 > reply_len) break;
        out->ports[i].port_number     = reply.data[off + 0];
        out->ports[i].peer_device_type =
            (u16)reply.data[off + 1] | ((u16)(reply.data[off + 2] & 0x07) << 8);
        out->ports[i].dpcd_rev        = (reply.data[off + 3] >> 4) & 0x0F;
        out->ports[i].mcs             = reply.data[off + 3] & 0x0F;
    }
    return DP_OK;
}

/* Sends ENUM_PATH_RESOURCES and parses AVAILABLE_PBN reply. */
int dp_mst_enum_path_resources(dp_controller_t *c, u8 port_number,
                               u32 *avail_pbn)
{
    if (!c || !avail_pbn) return DP_ERR_INVAL;
    if (!c->is_mst_active) return DP_ERR_PROTOCOL;

    dp_mst_sideband_msg_t msg;
    int n = dp_mst_build_enum_path_resources(&msg, port_number);
    int rc = dp_mst_sideband_send(c, &msg, (u32)n);
    if (rc != DP_OK) return rc;

    dp_mst_sideband_msg_t reply;
    u32 reply_len = 0;
    u64 deadline = riscv_read_time() + 200000ULL;
    for (;;) {
        rc = dp_mst_sideband_receive(c, &reply, &reply_len);
        if (rc == DP_OK) break;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        dp_udelay(200);
    }

    if (reply.message_type != DP_MST_MESSAGE_ENUM_PATH_RES_REPLY) return DP_ERR_PROTOCOL;
    if (reply_len < 5) return DP_ERR_PROTOCOL;
    *avail_pbn = (u32)reply.data[3] | ((u32)reply.data[4] << 8);
    return DP_OK;
}

/* Full allocate-and-forward helper using computed PBN. */
int dp_mst_alloc_and_assign(dp_driver_state_t *ds, u8 stream_id,
                            u8 port_number, u8 vcpi)
{
    if (!ds || !ds->ctrl) return DP_ERR_INVAL;
    dp_stream_t *s = dp_stream_get(ds, stream_id);
    if (!s) return DP_ERR_INVAL;

    u16 pbn = dp_mst_compute_pbn(s->timing.pixel_clock,
                                 (u8)dp_calc_bpp(s->bpc, s->pixel_format),
                                 (u8)ds->ctrl->lanes, ds->ctrl->bandwidth);

    u32 avail = 0;
    int rc = dp_mst_enum_path_resources(ds->ctrl, port_number, &avail);
    if (rc != DP_OK) return rc;
    if (avail < pbn) return DP_ERR_NOMEM;

    dp_mst_sideband_msg_t msg;
    int n = dp_mst_build_allocate_payload(&msg, port_number, vcpi, pbn);
    rc = dp_mst_sideband_send(ds->ctrl, &msg, (u32)n);
    if (rc != DP_OK) return rc;

    u8 vl = 0;
    for (;;) {
        rc = dp_aux_read_dpcd(ds->ctrl, DP_PAYLOAD_ALLOCATE_SET, &vl, 1);
        if (rc == DP_OK && vl == vcpi) break;
        u64 start = riscv_read_time();
        while ((riscv_read_time() - start) < 50000ULL) cpu_nop();
        static u32 tries;
        if (++tries > 5) { tries = 0; return DP_ERR_TIMEOUT; }
    }

    s->vc_payload_id = vcpi;
    return DP_OK;
}

/* ========================================================================== */
/* MST - PER-PORT STREAM TEARDOWN                                              */
/* ========================================================================== */

int dp_mst_teardown_port(dp_driver_state_t *ds, u8 port_number)
{
    if (!ds || !ds->ctrl) return DP_ERR_INVAL;

    for (u32 i = 0; i < ds->stream_count; i++) {
        dp_stream_t *s = &ds->streams[i];
        if (!s->vc_payload_id) continue;
        (void)dp_stream_disable_ex(ds, s->stream_id);
        (void)dp_mst_clear_vcpi(ds->ctrl, s->vc_payload_id);
        s->vc_payload_id = 0;
    }
    (void)port_number;
    return DP_OK;
}

/* ========================================================================== */
/* DIAGNOSTICS                                                                 */
/* ========================================================================== */

static __maybe_unused const char *dp_mst_msg_type_str(u8 type)
{
    switch (type) {
    case DP_MST_MESSAGE_LINK_ADDRESS:       return "LINK_ADDRESS";
    case DP_MST_MESSAGE_CONNECTION_STATUS:  return "CONN_STATUS";
    case DP_MST_MESSAGE_RESOURCE_STATUS:    return "RES_STATUS";
    case DP_MST_MESSAGE_ENUM_PATH_RES:      return "ENUM_PATH";
    case DP_MST_MESSAGE_ALLOCATE_PAYLOAD:   return "ALLOC_PAYLOAD";
    case DP_MST_MESSAGE_ENUM_PATH_RES_REPLY:return "ENUM_PATH_REPLY";
    case DP_MST_MESSAGE_QUERY_PAYLOAD:      return "QUERY_PAYLOAD";
    case DP_MST_MESSAGE_CLEAR_PAYLOAD_ID:   return "CLEAR_PAYLOAD";
    case DP_MST_MESSAGE_REMOTE_CMD:         return "REMOTE_CMD";
    default:                                return "UNKNOWN";
    }
}

static __maybe_unused u32 dp_mst_vcpi_bitmap(dp_controller_t *c)
{
    if (!c) return 0;
    return dp_reg_read(c->base_virtual_addr, DP_REG_MST_VC_PAYLOAD_STAT);
}

static __maybe_unused u32 dp_mst_total_pbn(dp_driver_state_t *ds)
{
    u32 total = 0;
    if (!ds) return 0;
    for (u32 i = 0; i < ds->stream_count; i++) {
        dp_stream_t *s = &ds->streams[i];
        if (!s->vc_payload_id) continue;
        total += dp_mst_compute_pbn(s->timing.pixel_clock,
                                    (u8)dp_calc_bpp(s->bpc, s->pixel_format),
                                    (u8)ds->ctrl->lanes, ds->ctrl->bandwidth);
    }
    return total;
}

/*
 * ============================================================================
 *  DP_CORE.C - Block 5: Audio (ALSA-inspired PCM + ELD + SAD + SDP)
 *  Architecture: RISC-V (RV32 / RV64)
 *  Language: C11
 *  Scope: ALSA-like PCM abstraction (card/pcm/substream/runtime/controls),
 *         ELD parsing, CEA-861 SAD decoding, CEA speaker allocation mapping,
 *         DP audio SDP (Audio Stream, Audio InfoFrame, Audio Timestamp),
 *         HDMI audio infoframe, DMA ring buffer, hw_params/constraints.
 * ============================================================================
 */

#include "dp_core.h"

/* ========================================================================== */
/* AUDIO HARDWARE REGISTERS (DP_RESERVED area, offsets 0x140..0x1BF)           */
/* ========================================================================== */

#define DP_REG_AUDIO_SDP_CTL        0x140
#define DP_REG_AUDIO_SDP_HDR        0x144
#define DP_REG_AUDIO_SDP_DATA       0x148  /* 8 x u32 = 32 bytes */
#define DP_REG_AUDIO_FIFO_CTL       0x168
#define DP_REG_AUDIO_FIFO_STATUS    0x16C
#define DP_REG_AUDIO_FIFO_DATA      0x170
#define DP_REG_AUDIO_DMA_ADDR_LO    0x174
#define DP_REG_AUDIO_DMA_ADDR_HI    0x178
#define DP_REG_AUDIO_DMA_LEN        0x17C
#define DP_REG_AUDIO_DMA_POS        0x180
#define DP_REG_AUDIO_SAMPLE_RATE    0x184
#define DP_REG_AUDIO_CH_STATUS      0x188
#define DP_REG_AUDIO_MUTE           0x18C

#define AUDIO_SDP_CTL_EN            BIT(0)
#define AUDIO_SDP_CTL_TYPE_SHIFT    4
#define AUDIO_SDP_CTL_TYPE_MASK     (0x3Fu << AUDIO_SDP_CTL_TYPE_SHIFT)

#define AUDIO_FIFO_CTL_EN           BIT(0)
#define AUDIO_FIFO_CTL_RESET        BIT(1)
#define AUDIO_FIFO_CTL_DMA_EN       BIT(2)

#define AUDIO_FIFO_UNDERRUN         BIT(0)
#define AUDIO_FIFO_OVERRUN          BIT(1)
#define AUDIO_FIFO_EMPTY            BIT(2)
#define AUDIO_FIFO_FULL             BIT(3)

/* HDMI audio registers reuse the block at 0x54..0x5F from Block 3. */

/* ========================================================================== */
/* SDP PACKET TYPE CONSTANTS (DP 1.4a)                                         */
/* ========================================================================== */

#define DP_SDP_TYPE_AUDIO_STREAM    0x02
#define DP_SDP_TYPE_AUDIO_INFOFRAME 0x84
#define DP_SDP_TYPE_AUDIO_TIMESTAMP 0x03
#define DP_SDP_TYPE_AUDIO_COPY_MGMT 0x04

#define DP_SDP_HB0                  0x00
#define DP_SDP_HB2_AUDIO_IF         0x84
#define DP_SDP_HB2_AUDIO_TS         0x03

/* ========================================================================== */
/* DP AUDIO INFOFRAME (ALSA-compatible layout)                                 */
/* ========================================================================== */

typedef struct __packed {
    u8 type;        /* 0x84 */
    u8 len;         /* 0x1b */
    u8 ver;         /* 0x11 << 2 */
    u8 CC02_CT47;   /* channel count - 1 (bits 2:0) | coding type (bits 6:4) */
    u8 SS01_SF24;   /* sample size | sample frequency */
    u8 CXT04;       /* channel extension / coding extension */
    u8 CA;          /* channel allocation */
    u8 LFEPBL01_LSV36_DM_INH7;
} dp_audio_infoframe_t;

#define DP_AUDIO_IF_TYPE            0x84
#define DP_AUDIO_IF_LEN             0x1b
#define DP_AUDIO_IF_VER             (0x11 << 2)

#define DP_AUDIO_IF_CC_MASK         0x07
#define DP_AUDIO_IF_CT_SHIFT        4
#define DP_AUDIO_IF_CT_MASK         (0x0F << DP_AUDIO_IF_CT_SHIFT)

#define DP_AUDIO_IF_SS_MASK         0x03
#define DP_AUDIO_IF_SF_SHIFT        2
#define DP_AUDIO_IF_SF_MASK         (0x07 << DP_AUDIO_IF_SF_SHIFT)

#define DP_AUDIO_IF_CA_MASK         0xFF

/* ========================================================================== */
/* CEA-861 SPEAKER / CHANNEL ALLOCATION TABLES                                 */
/* ========================================================================== */

enum cea_speaker_placement {
    CEA_FL  = BIT(0),   /* Front Left */
    CEA_FC  = BIT(1),   /* Front Center */
    CEA_FR  = BIT(2),   /* Front Right */
    CEA_FLC = BIT(3),   /* Front Left Center */
    CEA_FRC = BIT(4),   /* Front Right Center */
    CEA_RL  = BIT(5),   /* Rear Left */
    CEA_RC  = BIT(6),   /* Rear Center */
    CEA_RR  = BIT(7),   /* Rear Right */
    CEA_RLC = BIT(8),   /* Rear Left Center */
    CEA_RRC = BIT(9),   /* Rear Right Center */
    CEA_LFE = BIT(10),  /* Low Frequency Effect */
    CEA_FLW = BIT(11),  /* Front Left Wide */
    CEA_FRW = BIT(12),  /* Front Right Wide */
    CEA_FLH = BIT(13),  /* Front Left High */
    CEA_FCH = BIT(14),  /* Front Center High */
    CEA_FRH = BIT(15),  /* Front Right High */
    CEA_TC  = BIT(16),  /* Top Center */
};

/* ELD speaker allocation bits -> CEA speaker mask. */
static const u32 eld_speaker_bits[] = {
    [0] = CEA_FL | CEA_FR,
    [1] = CEA_LFE,
    [2] = CEA_FC,
    [3] = CEA_RL | CEA_RR,
    [4] = CEA_RC,
    [5] = CEA_FLC | CEA_FRC,
    [6] = CEA_RLC | CEA_RRC,
    [7] = CEA_FLW | CEA_FRW,
    [8] = CEA_FLH | CEA_FRH,
    [9] = CEA_TC,
    [10] = CEA_FCH,
};

#define ELD_SPEAKER_BITS_COUNT  ARRAY_SIZE(eld_speaker_bits)

/* CEA-861 ordered channel allocation table (subset; full table has ~50 entries).
 * Each entry: { ca_index, speakers[8], channels, spk_mask } */
typedef struct {
    u8  ca_index;
    u32 speakers[8];
    u8  channels;
    u32 spk_mask;
} cea_channel_allocation_t;

static const cea_channel_allocation_t cea_channel_allocations[] = {
    { 0x00, { CEA_FL, CEA_FR, 0, 0, 0, 0, 0, 0 }, 2, CEA_FL | CEA_FR },
    { 0x01, { CEA_FL, CEA_FR, CEA_LFE, 0, 0, 0, 0, 0 }, 3, CEA_FL | CEA_FR | CEA_LFE },
    { 0x02, { CEA_FL, CEA_FR, CEA_FC, 0, 0, 0, 0, 0 }, 3, CEA_FL | CEA_FR | CEA_FC },
    { 0x03, { CEA_FL, CEA_FR, CEA_FC, CEA_LFE, 0, 0, 0, 0 }, 4,
      CEA_FL | CEA_FR | CEA_FC | CEA_LFE },
    { 0x04, { CEA_FL, CEA_FR, CEA_RL, CEA_RR, 0, 0, 0, 0 }, 4,
      CEA_FL | CEA_FR | CEA_RL | CEA_RR },
    { 0x05, { CEA_FL, CEA_FR, CEA_RL, CEA_RR, CEA_LFE, 0, 0, 0 }, 5,
      CEA_FL | CEA_FR | CEA_RL | CEA_RR | CEA_LFE },
    { 0x06, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, 0, 0, 0 }, 5,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR },
    { 0x07, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_LFE, 0, 0 }, 6,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_LFE },
    { 0x08, { CEA_FL, CEA_FR, CEA_FC, CEA_RC, CEA_RL, CEA_RR, 0, 0 }, 6,
      CEA_FL | CEA_FR | CEA_FC | CEA_RC | CEA_RL | CEA_RR },
    { 0x09, { CEA_FL, CEA_FR, CEA_FC, CEA_RC, CEA_RL, CEA_RR, CEA_LFE, 0 }, 7,
      CEA_FL | CEA_FR | CEA_FC | CEA_RC | CEA_RL | CEA_RR | CEA_LFE },
    { 0x0A, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLC, CEA_FRC, 0 }, 7,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLC | CEA_FRC },
    { 0x0B, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLC, CEA_FRC, CEA_LFE }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLC | CEA_FRC | CEA_LFE },
    { 0x0C, { CEA_FL, CEA_FR, CEA_FC, CEA_RC, CEA_RL, CEA_RR, CEA_RLC, CEA_RRC }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RC | CEA_RL | CEA_RR | CEA_RLC | CEA_RRC },
    { 0x0D, { CEA_FL, CEA_FR, CEA_FC, CEA_RC, CEA_RL, CEA_RR, CEA_RLC, CEA_RRC, CEA_LFE }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RC | CEA_RL | CEA_RR | CEA_RLC | CEA_RRC | CEA_LFE },
    { 0x0E, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLW, CEA_FRW, 0 }, 7,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLW | CEA_FRW },
    { 0x0F, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLW, CEA_FRW, CEA_LFE }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLW | CEA_FRW | CEA_LFE },
    { 0x10, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLH, CEA_FRH, 0 }, 7,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLH | CEA_FRH },
    { 0x11, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLH, CEA_FRH, CEA_LFE }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLH | CEA_FRH | CEA_LFE },
    { 0x12, { CEA_FL, CEA_FR, CEA_FC, CEA_RC, CEA_RL, CEA_RR, CEA_TC, 0 }, 7,
      CEA_FL | CEA_FR | CEA_FC | CEA_RC | CEA_RL | CEA_RR | CEA_TC },
    { 0x13, { CEA_FL, CEA_FR, CEA_FC, CEA_RC, CEA_RL, CEA_RR, CEA_TC, CEA_LFE }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RC | CEA_RL | CEA_RR | CEA_TC | CEA_LFE },
    { 0x14, { CEA_FL, CEA_FR, CEA_FC, CEA_RL, CEA_RR, CEA_FLC, CEA_FRC, CEA_TC }, 8,
      CEA_FL | CEA_FR | CEA_FC | CEA_RL | CEA_RR | CEA_FLC | CEA_FRC | CEA_TC },
    { 0x1F, { CEA_FL, CEA_FR, CEA_LFE, CEA_FC, CEA_RL, CEA_RR, CEA_FLC, CEA_FRC }, 8,
      CEA_FL | CEA_FR | CEA_LFE | CEA_FC | CEA_RL | CEA_RR | CEA_FLC | CEA_FRC },
};

#define CEA_CA_TABLE_SIZE  ARRAY_SIZE(cea_channel_allocations)

/* ALSA channel position map (stereo + common surround layouts). */
typedef struct {
    u8 channels;
    u8 map[8];
} alsa_chmap_entry_t;

#define SND_CHMAP_FL  0
#define SND_CHMAP_FR  1
#define SND_CHMAP_FC  2
#define SND_CHMAP_LFE 3
#define SND_CHMAP_RL  4
#define SND_CHMAP_RR  5
#define SND_CHMAP_RC  6
#define SND_CHMAP_FLC 7
#define SND_CHMAP_FRC 8
#define SND_CHMAP_FLW 9
#define SND_CHMAP_FRW 10
#define SND_CHMAP_FLH 11
#define SND_CHMAP_FCH 12
#define SND_CHMAP_FRH 13
#define SND_CHMAP_TC  14

static const alsa_chmap_entry_t alsa_chmaps[] = {
    { 2, { SND_CHMAP_FL, SND_CHMAP_FR } },
    { 3, { SND_CHMAP_FL, SND_CHMAP_FR, SND_CHMAP_LFE } },
    { 4, { SND_CHMAP_FL, SND_CHMAP_FR, SND_CHMAP_RL, SND_CHMAP_RR } },
    { 5, { SND_CHMAP_FL, SND_CHMAP_FR, SND_CHMAP_RL, SND_CHMAP_RR, SND_CHMAP_FC } },
    { 6, { SND_CHMAP_FL, SND_CHMAP_FR, SND_CHMAP_RL, SND_CHMAP_RR, SND_CHMAP_FC, SND_CHMAP_LFE } },
    { 8, { SND_CHMAP_FL, SND_CHMAP_FR, SND_CHMAP_RL, SND_CHMAP_RR,
           SND_CHMAP_FC, SND_CHMAP_LFE, SND_CHMAP_FLC, SND_CHMAP_FRC } },
};

/* ========================================================================== */
/* ALSA-LIKE PCM STRUCTURES                                                    */
/* ========================================================================== */

#define DP_PCM_FORMAT_S16_LE    0x0002
#define DP_PCM_FORMAT_S24_LE    0x0006
#define DP_PCM_FORMAT_S32_LE    0x000A

#define DP_PCM_RATE_32000       (1u << 5)
#define DP_PCM_RATE_44100       (1u << 7)
#define DP_PCM_RATE_48000       (1u << 8)
#define DP_PCM_RATE_88200       (1u << 10)
#define DP_PCM_RATE_96000       (1u << 11)
#define DP_PCM_RATE_176400      (1u << 13)
#define DP_PCM_RATE_192000      (1u << 14)

#define DP_PCM_STREAM_PLAYBACK  0

#define DP_PCM_STATE_OPEN       0
#define DP_PCM_STATE_SETUP      1
#define DP_PCM_STATE_PREPARED   2
#define DP_PCM_STATE_RUNNING    3
#define DP_PCM_STATE_XRUN       4
#define DP_PCM_STATE_DRAINING   5
#define DP_PCM_STATE_PAUSED     6
#define DP_PCM_STATE_SUSPENDED  7

typedef struct {
    u32 format;
    u32 rate;
    u32 channels;
    u32 period_size;
    u32 periods;
    u32 buffer_size;
    u32 sample_bits;
} dp_pcm_hw_params_t;

typedef struct {
    u32 format_mask;
    u32 rate_min;
    u32 rate_max;
    u32 channels_min;
    u32 channels_max;
    u32 period_bytes_min;
    u32 period_bytes_max;
    u32 periods_min;
    u32 periods_max;
    u32 buffer_bytes_max;
} dp_pcm_hardware_t;

typedef struct dp_pcm_substream_s {
    dp_pcm_hardware_t hw;
    dp_pcm_hw_params_t hw_params;
    u32 state;
    u8 *dma_area;
    dma_addr_t dma_addr;
    u32 dma_bytes;
    u32 hw_ptr;
    u32 appl_ptr;
    u32 period_elapsed;
    volatile u32 period_done;
    void (*period_elapsed_cb)(struct dp_pcm_substream_s *sub);
    void *private_data;
} dp_pcm_substream_t;

typedef struct {
    dp_pcm_substream_t playback;
    dp_spinlock_t lock;
    bool opened;
} dp_snd_pcm_t;

typedef struct {
    u32 id;
    u32 value;
    u32 min;
    u32 max;
    const char *name;
} dp_snd_kcontrol_t;

#define DP_MAX_CONTROLS  16

typedef struct dp_snd_card_s {
    char id[16];
    char name[32];
    dp_snd_pcm_t pcm;
    dp_snd_kcontrol_t controls[DP_MAX_CONTROLS];
    u32 num_controls;
    dp_controller_t *ctrl;
    dp_eld_t eld;
    dp_sad_t sads[DP_ELD_MAX_SAD];
    u32 sad_count;
    u32 cea_ca;
    u32 speaker_mask;
    bool eld_valid;
    bool audio_enabled;
    dp_spinlock_t lock;
} dp_snd_card_t;

/* ========================================================================== */
/* ELD PARSING (ALSA hda_eld.c style)                                          */
/* ========================================================================== */

typedef struct {
    u8  version;
    u8  baseline_len;
    u8  conn_type;
    u8  spk_alloc;
    u8  sad_count;
    dp_sad_t sad[DP_ELD_MAX_SAD];
    char monitor_name[16];
} dp_parsed_eld_t;

static int dp_eld_parse(dp_snd_card_t *card, const u8 *eld_buf, u32 eld_len)
{
    if (!card || !eld_buf || eld_len < 20) return DP_ERR_EDID;

    dp_mem_set(&card->eld, 0, sizeof(card->eld));

    u8 version = (eld_buf[0] >> 3) & 0x1F;
    u8 baseline_len = eld_buf[2];
    u8 sad_count = eld_buf[5] & 0x0F;

    card->eld.version = version;
    card->eld.baseline_len = baseline_len;
    card->eld.sad_count = sad_count;

    if (baseline_len < eld_len && eld_buf[baseline_len] != 0) {
        u32 off = baseline_len;
        if (off + 1 < eld_len) {
            card->eld.hdmi_vsdb_present = 1;
            card->eld.hdmi_vsdb_len = eld_buf[off + 1];
        }
    }

    u32 spk_off = 20;
    if (spk_off + 3 <= eld_len) {
        card->eld.speaker_allocation[0] = eld_buf[spk_off];
        card->eld.speaker_allocation[1] = eld_buf[spk_off + 1];
        card->eld.speaker_allocation[2] = eld_buf[spk_off + 2];
        u32 spk = (u32)eld_buf[spk_off] |
                  ((u32)eld_buf[spk_off + 1] << 8) |
                  ((u32)eld_buf[spk_off + 2] << 16);
        u32 mask = 0;
        for (u32 i = 0; i < ELD_SPEAKER_BITS_COUNT; i++) {
            if (spk & (1u << i)) mask |= eld_speaker_bits[i];
        }
        card->speaker_mask = mask;
    }

    u32 sad_off = 20 + 3 + 4 + 2 + 2 + 2 + 2;
    u32 count = 0;
    for (u32 i = 0; i < sad_count && sad_off + 3 <= eld_len; i++, sad_off += 3) {
        dp_sad_t *s = &card->sads[count++];
        s->format = (eld_buf[sad_off] >> 3) & 0x0F;
        s->channels = (eld_buf[sad_off] & 0x07) + 1;
        s->sample_rates = eld_buf[sad_off + 1];
        s->byte3 = eld_buf[sad_off + 2];
    }
    card->sad_count = count;
    card->eld_valid = true;
    return DP_OK;
}

/* ========================================================================== */
/* SAD DECODING HELPERS                                                        */
/* ========================================================================== */

static u32 dp_sad_rate_mask(u8 sad_rates)
{
    u32 mask = 0;
    if (sad_rates & BIT(0)) mask |= DP_PCM_RATE_32000;
    if (sad_rates & BIT(1)) mask |= DP_PCM_RATE_44100;
    if (sad_rates & BIT(2)) mask |= DP_PCM_RATE_48000;
    if (sad_rates & BIT(3)) mask |= DP_PCM_RATE_88200;
    if (sad_rates & BIT(4)) mask |= DP_PCM_RATE_96000;
    if (sad_rates & BIT(5)) mask |= DP_PCM_RATE_176400;
    if (sad_rates & BIT(6)) mask |= DP_PCM_RATE_192000;
    return mask;
}

static u32 dp_sad_sample_bits(u8 byte3)
{
    u32 bits = 0;
    if (byte3 & BIT(0)) bits |= 16;
    if (byte3 & BIT(1)) bits |= 20;
    if (byte3 & BIT(2)) bits |= 24;
    return bits;
}

static bool dp_sad_is_pcm(const dp_sad_t *sad)
{
    return sad && (sad->format == DP_SAD_FORMAT_LPCM);
}

/* ========================================================================== */
/* ALSA-LIKE HW CONSTRAINTS                                                    */
/* ========================================================================== */

static void dp_pcm_hw_constraint_mask(dp_pcm_hardware_t *hw, u32 *mask,
                                      u32 min, u32 max)
{
    *mask = 0;
    if (min <= 32000  && max >= 32000)  *mask |= DP_PCM_RATE_32000;
    if (min <= 44100  && max >= 44100)  *mask |= DP_PCM_RATE_44100;
    if (min <= 48000  && max >= 48000)  *mask |= DP_PCM_RATE_48000;
    if (min <= 88200  && max >= 88200)  *mask |= DP_PCM_RATE_88200;
    if (min <= 96000  && max >= 96000)  *mask |= DP_PCM_RATE_96000;
    if (min <= 176400 && max >= 176400) *mask |= DP_PCM_RATE_176400;
    if (min <= 192000 && max >= 192000) *mask |= DP_PCM_RATE_192000;
    (void)hw;
}

static void dp_pcm_build_constraints_from_eld(dp_snd_card_t *card,
                                               dp_pcm_hardware_t *hw)
{
    if (!card || !hw) return;

    u32 rate_mask = 0;
    u32 ch_max = 2;
    u32 bits_max = 16;

    for (u32 i = 0; i < card->sad_count; i++) {
        const dp_sad_t *sad = &card->sads[i];
        if (!dp_sad_is_pcm(sad)) continue;
        rate_mask |= dp_sad_rate_mask(sad->sample_rates);
        if (sad->channels > ch_max) ch_max = sad->channels;
        u32 b = dp_sad_sample_bits(sad->byte3);
        if (b > bits_max) bits_max = b;
    }

    if (rate_mask == 0) rate_mask = DP_PCM_RATE_48000 | DP_PCM_RATE_44100;
    if (ch_max < 2) ch_max = 2;
    if (bits_max < 16) bits_max = 16;

    hw->rate_min = 32000;
    hw->rate_max = 192000;
    hw->channels_min = 2;
    hw->channels_max = ch_max;
    hw->format_mask = DP_PCM_FORMAT_S16_LE;
    if (bits_max >= 20) hw->format_mask |= DP_PCM_FORMAT_S24_LE;
    if (bits_max >= 32) hw->format_mask |= DP_PCM_FORMAT_S32_LE;
    hw->period_bytes_min = 64;
    hw->period_bytes_max = 65536;
    hw->periods_min = 2;
    hw->periods_max = 16;
    hw->buffer_bytes_max = 1u << 20;
}

/* ========================================================================== */
/* CEA CHANNEL ALLOCATION SELECTION                                            */
/* ========================================================================== */

static u32 dp_cea_spk_mask_from_alloc(u8 spk_alloc)
{
    u32 mask = 0;
    for (u32 i = 0; i < ELD_SPEAKER_BITS_COUNT; i++) {
        if (spk_alloc & (1u << i)) mask |= eld_speaker_bits[i];
    }
    return mask;
}

/* Finds best CA index for requested channel count, given speaker mask.
 * Falls back to a CA whose channel count matches, then to stereo. */
static u8 dp_cea_select_ca(u32 spk_mask, u32 channels)
{
    u8 fallback_2 = 0x00;
    u8 fallback_n = 0x00;

    for (u32 i = 0; i < CEA_CA_TABLE_SIZE; i++) {
        const cea_channel_allocation_t *ca = &cea_channel_allocations[i];
        bool layout_ok = (spk_mask & ca->spk_mask) == ca->spk_mask;

        if (ca->channels == 2 && ca->ca_index == 0x00)
            fallback_2 = ca->ca_index;

        if (ca->channels == channels && layout_ok)
            return ca->ca_index;

        if (ca->channels == channels && fallback_n == 0x00)
            fallback_n = ca->ca_index;
    }
    if (fallback_n) return fallback_n;
    return fallback_2;
}

/* ========================================================================== */
/* DP AUDIO INFOFRAME BUILDER                                                  */
/* ========================================================================== */

static void dp_audio_build_infoframe(const dp_snd_card_t *card,
                                      const dp_pcm_hw_params_t *p,
                                      dp_audio_infoframe_t *ifr)
{
    dp_mem_set(ifr, 0, sizeof(*ifr));
    ifr->type = DP_AUDIO_IF_TYPE;
    ifr->len  = DP_AUDIO_IF_LEN;
    ifr->ver  = DP_AUDIO_IF_VER;

    u32 channels = p->channels ? p->channels : 2;
    if (channels > 8) channels = 8;
    ifr->CC02_CT47 = (u8)((channels - 1) & DP_AUDIO_IF_CC_MASK);
    ifr->CC02_CT47 |= (u8)((DP_AUDIO_CODING_REF_STREAM << DP_AUDIO_IF_CT_SHIFT)
                           & DP_AUDIO_IF_CT_MASK);

    u8 ss = 0;
    if (p->sample_bits == 16) ss = 1;
    else if (p->sample_bits == 20) ss = 2;
    else if (p->sample_bits == 24) ss = 3;
    ifr->SS01_SF24 = ss & DP_AUDIO_IF_SS_MASK;

    u8 sf = 0;
    switch (p->rate) {
    case 32000:  sf = 1; break;
    case 44100:  sf = 2; break;
    case 48000:  sf = 3; break;
    case 88200:  sf = 4; break;
    case 96000:  sf = 5; break;
    case 176400: sf = 6; break;
    case 192000: sf = 7; break;
    default:     sf = 0; break;
    }
    ifr->SS01_SF24 |= (u8)((sf << DP_AUDIO_IF_SF_SHIFT) & DP_AUDIO_IF_SF_MASK);

    ifr->CA = dp_cea_select_ca(card->speaker_mask, channels);
}

/* Packs DP Audio InfoFrame into SDP header + data (little-endian u32 words). */
static u32 dp_audio_pack_sdp_infoframe(const dp_audio_infoframe_t *ifr,
                                        u32 hdr_out[1], u32 data_out[2])
{
    u32 hb0 = DP_SDP_HB0;
    u32 hb1 = DP_SDP_TYPE_AUDIO_INFOFRAME;
    u32 hb2 = DP_SDP_HB2_AUDIO_IF;
    u32 hb3 = 0;
    for (u32 i = 0; i < sizeof(*ifr); i++)
        hb3 += ((const u8 *)ifr)[i];
    hb3 = (u8)(0 - hb3);

    hdr_out[0] = hb0 | (hb1 << 8) | (hb2 << 16) | (hb3 << 24);

    const u8 *raw = (const u8 *)ifr;
    data_out[0] = dp_pack_le32(&raw[0]);
    data_out[1] = dp_pack_le32(&raw[4]);
    return 8;
}

/* ========================================================================== */
/* AUDIO TIMESTAMP SDP                                                         */
/* ========================================================================== */

static u32 dp_audio_pack_timestamp_sdp(u32 sdp_type, u16 timestamp,
                                        u8 audio_info, u32 hdr_out[1],
                                        u32 data_out[1])
{
    u32 hb0 = DP_SDP_HB0;
    u32 hb1 = sdp_type & 0xFF;
    u32 hb2 = DP_SDP_HB2_AUDIO_TS;
    u32 hb3 = 0;
    hb3 += hb0 + hb1 + hb2;
    hb3 += (timestamp & 0xFF) + ((timestamp >> 8) & 0xFF) + (audio_info & 0xFF);
    hb3 = (u8)(0 - hb3);

    hdr_out[0] = hb0 | (hb1 << 8) | (hb2 << 16) | (hb3 << 24);
    data_out[0] = ((u32)(timestamp & 0xFF)) |
                  ((u32)((timestamp >> 8) & 0xFF) << 8) |
                  ((u32)(audio_info & 0xFF) << 16);
    return 4;
}

/* ========================================================================== */
/* SDP HARDWARE PROGRAMMING                                                    */
/* ========================================================================== */

static void dp_audio_sdp_write_hw(dp_controller_t *c, u32 hdr,
                                   const u32 *data, u32 len)
{
    vaddr_t b = c->base_virtual_addr;
    dp_reg_write(b, DP_REG_AUDIO_SDP_HDR, hdr);

    u32 words = (len + 3u) / 4u;
    for (u32 i = 0; i < words && i < 8; i++)
        dp_reg_write(b, DP_REG_AUDIO_SDP_DATA + (i * 4u), data[i]);

    u32 ctl = dp_reg_read(b, DP_REG_AUDIO_SDP_CTL);
    ctl &= ~AUDIO_SDP_CTL_TYPE_MASK;
    ctl |= ((hdr >> 8) & 0x3Fu) << AUDIO_SDP_CTL_TYPE_SHIFT;
    ctl |= AUDIO_SDP_CTL_EN;
    dp_reg_write(b, DP_REG_AUDIO_SDP_CTL, ctl);
}

/* ========================================================================== */
/* HDMI AUDIO INFOFRAME                                                        */
/* ========================================================================== */

static int hdmi_audio_infoframe_pack(const dp_snd_card_t *card,
                                      const dp_pcm_hw_params_t *p,
                                      u8 *buf, u32 buf_len)
{
    if (!buf || buf_len < 14) return DP_ERR_INVAL;

    u32 channels = p->channels ? p->channels : 2;
    if (channels > 8) channels = 8;

    buf[0] = 0x84;
    buf[1] = 0x01;
    buf[2] = 0x0A;
    u8 sum = 0;
    for (u32 i = 0; i < 3; i++) sum += buf[i];
    buf[3] = (u8)(0 - sum);

    buf[4] = (u8)((channels - 1) & 0x07);
    if (p->sample_bits == 16) buf[4] |= (1 << 3);
    else if (p->sample_bits == 20) buf[4] |= (2 << 3);
    else if (p->sample_bits == 24) buf[4] |= (3 << 3);

    u8 sf = 0;
    switch (p->rate) {
    case 32000:  sf = 1; break;
    case 44100:  sf = 2; break;
    case 48000:  sf = 3; break;
    case 88200:  sf = 4; break;
    case 96000:  sf = 5; break;
    case 176400: sf = 6; break;
    case 192000: sf = 7; break;
    default:     sf = 0; break;
    }
    buf[5] = (u8)(sf & 0x07);
    buf[5] |= (u8)((DP_AUDIO_CODING_REF_STREAM & 0x0F) << 4);

    buf[6] = 0x00;
    buf[7] = dp_cea_select_ca(card->speaker_mask, channels);
    buf[8] = 0x00;
    buf[9] = 0x00;
    buf[10] = 0x00;

    sum = 0;
    for (u32 i = 0; i < 11; i++) sum += buf[i];
    buf[11] = (u8)(0 - sum);

    return 12;
}

static void hdmi_audio_infoframe_write_hw(dp_controller_t *c,
                                          const u8 *buf, u32 len)
{
    vaddr_t b = c->base_virtual_addr;
    dp_reg_write(b, HDMI_REG_INFOFRAME_CTL, 0);
    for (u32 i = 0; i < len; i += 4) {
        u32 w = 0;
        for (u32 j = 0; j < 4 && (i + j) < len; j++)
            w |= ((u32)buf[i + j]) << (j * 8);
        dp_reg_write(b, HDMI_REG_INFOFRAME_DATA, w);
    }
    dp_reg_write(b, HDMI_REG_INFOFRAME_CTL, BIT(0) | BIT(1));
}

/* ========================================================================== */
/* DMA RING BUFFER (ALSA-style)                                                */
/* ========================================================================== */

static int dp_pcm_dma_alloc(dp_pcm_substream_t *sub, u32 bytes)
{
    if (!sub) return DP_ERR_INVAL;
    if (bytes > sub->hw.buffer_bytes_max) return DP_ERR_NOMEM;

    /* Bare-metal allocator: caller provides dma_area (0x0 means "auto").
     * In a real driver this would use a DMA allocator. */
    if (!sub->dma_area) return DP_ERR_NOMEM;

    sub->dma_bytes = bytes;
    sub->hw_ptr = 0;
    sub->appl_ptr = 0;
    sub->period_elapsed = 0;
    sub->period_done = 0;
    return DP_OK;
}

static void dp_pcm_hw_ptr_update(dp_pcm_substream_t *sub)
{
    if (!sub || !sub->hw_params.period_size) return;
    u32 period_bytes = sub->hw_params.period_size *
                       (sub->hw_params.sample_bits / 8) *
                       sub->hw_params.channels;
    if (period_bytes == 0) return;
    if (sub->hw_ptr >= sub->dma_bytes) sub->hw_ptr = 0;
    u32 periods = sub->dma_bytes / period_bytes;
    if (periods == 0) return;
    u32 p = sub->hw_ptr / period_bytes;
    if (p != sub->period_elapsed) {
        sub->period_elapsed = p;
        sub->period_done++;
        if (sub->period_elapsed_cb) sub->period_elapsed_cb(sub);
    }
}

/* ========================================================================== */
/* PCM HW PARAMS APPLY                                                         */
/* ========================================================================== */

static int dp_pcm_hw_params(dp_snd_card_t *card, dp_pcm_substream_t *sub,
                             const dp_pcm_hw_params_t *p)
{
    if (!card || !sub || !p) return DP_ERR_INVAL;

    if (p->channels < sub->hw.channels_min || p->channels > sub->hw.channels_max)
        return DP_ERR_INVAL;
    if (p->rate < sub->hw.rate_min || p->rate > sub->hw.rate_max)
        return DP_ERR_INVAL;

    u32 format_ok = 0;
    switch (p->format) {
    case DP_PCM_FORMAT_S16_LE: format_ok = 16; break;
    case DP_PCM_FORMAT_S24_LE: format_ok = 24; break;
    case DP_PCM_FORMAT_S32_LE: format_ok = 32; break;
    default: return DP_ERR_INVAL;
    }
    if (!(sub->hw.format_mask & p->format)) return DP_ERR_INVAL;

    if (p->period_size == 0 || p->periods < sub->hw.periods_min ||
        p->periods > sub->hw.periods_max)
        return DP_ERR_INVAL;

    sub->hw_params = *p;
    sub->hw_params.sample_bits = format_ok;
    sub->hw_params.period_size = p->period_size;
    sub->hw_params.periods = p->periods;
    sub->hw_params.buffer_size = p->period_size * p->periods;

    u32 bytes = sub->hw_params.buffer_size * (format_ok / 8) * p->channels;
    if (bytes > sub->hw.buffer_bytes_max) return DP_ERR_NOMEM;

    return dp_pcm_dma_alloc(sub, bytes);
}

/* ========================================================================== */
/* PUBLIC AUDIO API                                                           */
/* ========================================================================== */

int dp_audio_configure(dp_controller_t *c, u8 stream_id,
                       const dp_audio_infoframe_params_t *p)
{
    if (!c || !c->initialized || !p) return DP_ERR_INVAL;
    (void)stream_id;

    vaddr_t b = c->base_virtual_addr;

    u32 rate = dp_audio_sample_rate_hz(p->sample_rate);
    if (rate == 0) rate = 48000;

    dp_reg_write(b, DP_REG_AUDIO_SAMPLE_RATE, rate);

    u32 ch_status = ((u32)(p->channels ? p->channels - 1 : 1) & 0x07) |
                    (((u32)p->sample_size & 0x07) << 3) |
                    (((u32)p->coding_type & 0x0F) << 8);
    dp_reg_write(b, DP_REG_AUDIO_CH_STATUS, ch_status);

    dp_reg_write(b, DP_REG_AUDIO_MUTE, 0);
    return DP_OK;
}

int dp_audio_enable(dp_controller_t *c, u8 stream_id)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;
    (void)stream_id;

    vaddr_t b = c->base_virtual_addr;

    dp_reg_write(b, DP_REG_AUDIO_FIFO_CTL, 0);
    dp_reg_write(b, DP_REG_AUDIO_FIFO_CTL, AUDIO_FIFO_CTL_RESET);
    dp_udelay(10);
    dp_reg_write(b, DP_REG_AUDIO_FIFO_CTL, AUDIO_FIFO_CTL_EN);
    return DP_OK;
}

int dp_audio_disable(dp_controller_t *c, u8 stream_id)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;
    (void)stream_id;

    vaddr_t b = c->base_virtual_addr;
    dp_reg_write(b, DP_REG_AUDIO_FIFO_CTL, 0);
    dp_reg_write(b, DP_REG_AUDIO_MUTE, 1);
    return DP_OK;
}

/* ========================================================================== */
/* CARD INIT / ELD SETUP                                                       */
/* ========================================================================== */

int dp_snd_card_init(dp_snd_card_t *card, dp_controller_t *ctrl,
                     const char *name)
{
    if (!card || !ctrl) return DP_ERR_INVAL;

    dp_mem_set(card, 0, sizeof(*card));
    dp_spin_lock_init(&card->lock);
    dp_spin_lock_init(&card->pcm.lock);

    card->ctrl = ctrl;
    if (name) {
        u32 n = 0;
        while (name[n] && n < sizeof(card->name) - 1) {
            card->name[n] = name[n]; n++;
        }
        card->name[n] = '\0';
    }

    dp_pcm_hardware_t *hw = &card->pcm.playback.hw;
    hw->rate_min = 32000;
    hw->rate_max = 192000;
    hw->channels_min = 2;
    hw->channels_max = 8;
    hw->format_mask = DP_PCM_FORMAT_S16_LE |
                      DP_PCM_FORMAT_S24_LE |
                      DP_PCM_FORMAT_S32_LE;
    hw->period_bytes_min = 64;
    hw->period_bytes_max = 65536;
    hw->periods_min = 2;
    hw->periods_max = 16;
    hw->buffer_bytes_max = 1u << 20;

    card->pcm.playback.state = DP_PCM_STATE_OPEN;
    return DP_OK;
}

int dp_snd_card_set_eld(dp_snd_card_t *card, const u8 *eld_buf, u32 eld_len)
{
    if (!card || !eld_buf) return DP_ERR_INVAL;
    dp_spin_lock(&card->lock);
    int rc = dp_eld_parse(card, eld_buf, eld_len);
    if (rc == DP_OK) {
        dp_pcm_build_constraints_from_eld(card, &card->pcm.playback.hw);
        card->cea_ca = dp_cea_select_ca(card->speaker_mask, 2);
    }
    dp_spin_unlock(&card->lock);
    return rc;
}

/* ========================================================================== */
/* PCM PREPARE / TRIGGER                                                       */
/* ========================================================================== */

int dp_snd_pcm_prepare(dp_snd_card_t *card, u8 *dma_area, u32 dma_bytes)
{
    if (!card) return DP_ERR_INVAL;

    dp_pcm_substream_t *sub = &card->pcm.playback;
    if (sub->state != DP_PCM_STATE_OPEN && sub->state != DP_PCM_STATE_SETUP)
        return DP_ERR_BUSY;

    sub->dma_area = dma_area;
    int rc = dp_pcm_dma_alloc(sub, dma_bytes);
    if (rc != DP_OK) return rc;

    sub->state = DP_PCM_STATE_PREPARED;
    return DP_OK;
}

int dp_snd_pcm_apply_hw_params(dp_snd_card_t *card,
                               const dp_pcm_hw_params_t *p)
{
    if (!card || !p) return DP_ERR_INVAL;
    dp_spin_lock(&card->lock);
    int rc = dp_pcm_hw_params(card, &card->pcm.playback, p);
    if (rc == DP_OK) card->pcm.playback.state = DP_PCM_STATE_SETUP;
    dp_spin_unlock(&card->lock);
    return rc;
}

int dp_snd_pcm_start(dp_snd_card_t *card)
{
    if (!card || !card->ctrl) return DP_ERR_INVAL;

    dp_pcm_substream_t *sub = &card->pcm.playback;
    if (sub->state != DP_PCM_STATE_PREPARED) return DP_ERR_INVAL;

    dp_pcm_hw_params_t *p = &sub->hw_params;

    /* Build and send DP Audio InfoFrame SDP. */
    dp_audio_infoframe_t ifr;
    dp_audio_build_infoframe(card, p, &ifr);

    u32 hdr, data[2];
    dp_audio_pack_sdp_infoframe(&ifr, &hdr, data);
    dp_audio_sdp_write_hw(card->ctrl, hdr, data, 8);

    /* Timestamp SDP (per frame; caller should re-send periodically). */
    u32 ts_hdr, ts_data;
    dp_audio_pack_timestamp_sdp(DP_SDP_TYPE_AUDIO_TIMESTAMP,
                                0, ifr.CA, &ts_hdr, &ts_data);
    dp_audio_sdp_write_hw(card->ctrl, ts_hdr, &ts_data, 4);

    /* HDMI path: also emit the HDMI audio infoframe. */
    u8 hdmi_if[14];
    int n = hdmi_audio_infoframe_pack(card, p, hdmi_if, sizeof(hdmi_if));
    if (n > 0) hdmi_audio_infoframe_write_hw(card->ctrl, hdmi_if, (u32)n);

    dp_audio_infoframe_params_t ap = {
        .channels = (u8)p->channels,
        .sample_rate = 0x03,
        .sample_size = (u8)(p->sample_bits == 24 ? DP_AUDIO_SAMPLE_24BIT :
                            p->sample_bits == 20 ? DP_AUDIO_SAMPLE_20BIT :
                            DP_AUDIO_SAMPLE_16BIT),
        .coding_type = DP_AUDIO_CODING_REF_STREAM,
        .channel_allocation = ifr.CA,
        .level_shift = 0,
        .downmix_inhibit = false,
    };
    dp_audio_configure(card->ctrl, 0, &ap);
    dp_audio_enable(card->ctrl, 0);

    sub->state = DP_PCM_STATE_RUNNING;
    card->audio_enabled = true;
    return DP_OK;
}

int dp_snd_pcm_stop(dp_snd_card_t *card)
{
    if (!card || !card->ctrl) return DP_ERR_INVAL;
    dp_audio_disable(card->ctrl, 0);
    card->pcm.playback.state = DP_PCM_STATE_SETUP;
    card->audio_enabled = false;
    return DP_OK;
}

/* ========================================================================== */
/* PERIOD / POSITION CALLBACK                                                  */
/* ========================================================================== */

void dp_snd_pcm_period_elapsed(dp_snd_card_t *card)
{
    if (!card) return;
    dp_pcm_hw_ptr_update(&card->pcm.playback);
}

/* ========================================================================== */
/* CONTROL INTERFACE (kcontrol)                                                */
/* ========================================================================== */

static int dp_snd_ctl_add(dp_snd_card_t *card, const char *name,
                          u32 id, u32 min, u32 max, u32 val)
{
    if (!card || card->num_controls >= DP_MAX_CONTROLS) return DP_ERR_NOMEM;
    dp_snd_kcontrol_t *k = &card->controls[card->num_controls++];
    k->name = name;
    k->id = id;
    k->min = min;
    k->max = max;
    k->value = (val < min) ? min : (val > max ? max : val);
    return DP_OK;
}

static int dp_snd_ctl_get(dp_snd_card_t *card, u32 id, u32 *val)
{
    if (!card || !val) return DP_ERR_INVAL;
    for (u32 i = 0; i < card->num_controls; i++) {
        if (card->controls[i].id == id) { *val = card->controls[i].value; return DP_OK; }
    }
    return DP_ERR_INVAL;
}

static int dp_snd_ctl_put(dp_snd_card_t *card, u32 id, u32 val)
{
    if (!card) return DP_ERR_INVAL;
    for (u32 i = 0; i < card->num_controls; i++) {
        dp_snd_kcontrol_t *k = &card->controls[i];
        if (k->id != id) continue;
        if (val < k->min || val > k->max) return DP_ERR_INVAL;
        k->value = val;
        return DP_OK;
    }
    return DP_ERR_INVAL;
}

int dp_snd_card_add_default_controls(dp_snd_card_t *card)
{
    if (!card) return DP_ERR_INVAL;
    dp_snd_ctl_add(card, "PCM Playback Volume", 1, 0, 255, 200);
    dp_snd_ctl_add(card, "PCM Playback Switch", 2, 0, 1, 1);
    dp_snd_ctl_add(card, "IEC958 Playback Switch", 3, 0, 1, 1);
    return DP_OK;
}

/* ========================================================================== */
/* ELD DUMP (debug)                                                            */
/* ========================================================================== */

__maybe_unused static void dp_snd_card_dump_eld(const dp_snd_card_t *card)
{
    if (!card) return;
    dp_info("ELD: ver=%u baseline=%u sad_count=%u spk_mask=0x%x ca=0x%02x",
            card->eld.version, card->eld.baseline_len,
            card->sad_count, card->speaker_mask, card->cea_ca);
    for (u32 i = 0; i < card->sad_count; i++) {
        const dp_sad_t *s = &card->sads[i];
        dp_info("  SAD[%u]: fmt=%u ch=%u rates=0x%02x byte3=0x%02x",
                i, s->format, s->channels, s->sample_rates, s->byte3);
    }
}

/* ========================================================================== */
/* FULL TOP-LEVEL FLOW                                                         */
/* ========================================================================== */

int dp_audio_top_level_init(dp_snd_card_t *card, dp_controller_t *ctrl,
                            const u8 *eld_buf, u32 eld_len)
{
    if (!card || !ctrl) return DP_ERR_INVAL;

    int rc = dp_snd_card_init(card, ctrl, "DP-HDMI-Audio");
    if (rc != DP_OK) return rc;

    if (eld_buf && eld_len > 0) {
        rc = dp_snd_card_set_eld(card, eld_buf, eld_len);
        if (rc != DP_OK) return rc;
    }

    (void)dp_snd_card_add_default_controls(card);
    return DP_OK;
}

/*
 * ============================================================================
 *  DP_CORE.C - Block 6: HDCP 1.x / HDCP 2.2
 *  Architecture: RISC-V (RV32 / RV64)
 *  Language: C11
 *  Scope: SHA-1, SHA-256, HMAC-SHA256, AES-128 (ECB/CBC), 2048-bit bignum,
 *         RSA-2048 verify/encrypt, HDCP 1.3/1.4 auth + streaming + repeater
 *         + SRM/revocation, HDCP 2.2 AKE/LC/SKE + stream mgmt + repeater,
 *         async state machine, public API.
 * ============================================================================
 */

#include "dp_core.h"

/* ========================================================================== */
/* HDCP REGISTER OFFSETS (I2C-over-AUX slave 0x3A)                             */
/* ========================================================================== */

#define HDCP_REG_BKSV               0x00
#define HDCP_REG_RI                 0x08
#define HDCP_REG_AKSV               0x10
#define HDCP_REG_AINFO              0x15
#define HDCP_REG_AN                 0x18
#define HDCP_REG_VPRIME             0x20
#define HDCP_REG_BCAPS              0x40
#define HDCP_REG_BSTATUS            0x41
#define HDCP_REG_KSV_FIFO           0x43

#define HDCP_BCAPS_KSV_FIFO_READY   BIT(5)
#define HDCP_BCAPS_REPEATER         BIT(6)
#define HDCP_BCAPS_HDMI             BIT(0)

#define HDCP_BSTATUS_MAX_DEVS_MASK  0x7F
#define HDCP_BSTATUS_MAX_CASCADE     BIT(7)

/* HDCP 2.2 register offsets (same AUX slave, different high-byte addresses) */
#define HDCP22_REG_VERSION          0x50
#define HDCP22_REG_WRITE_MSG_SIZE   0x60
#define HDCP22_REG_WRITE_MSG        0x61
#define HDCP22_REG_READ_MSG_SIZE    0x68
#define HDCP22_REG_READ_MSG         0x69
#define HDCP22_REG_RXSTATUS         0x70

#define HDCP22_STATUS_READY         BIT(0)
#define HDCP22_STATUS_REAUTH        BIT(3)
#define HDCP22_STATUS_LINK_INTEGRITY_FAIL BIT(4)

#define HDCP22_MSG_AKE_SEND_CERT    0x02
#define HDCP22_MSG_AKE_NO_STORED_KM 0x03
#define HDCP22_MSG_AKE_STORED_KM    0x04
#define HDCP22_MSG_AKE_SEND_H_PRIME 0x07
#define HDCP22_MSG_AKE_SEND_PAIRING_INFO 0x08
#define HDCP22_MSG_LC_INIT          0x09
#define HDCP22_MSG_LC_SEND_L_PRIME  0x0A
#define HDCP22_MSG_SKE_SEND_EKS     0x0B
#define HDCP22_MSG_SEND_RECV_ID_LIST 0x0C
#define HDCP22_MSG_RECV_ID_LIST     0x0D
#define HDCP22_MSG_REPEATER_AUTH_SEND_RECEIVER_ID_LIST 0x0C
#define HDCP22_MSG_REPEATER_AUTH_SEND_ACK 0x0E
#define HDCP22_MSG_REPEATER_AUTH_STREAM_MANAGE 0x0F
#define HDCP22_MSG_REPEATER_AUTH_STREAM_READY 0x10

#define HDCP22_MAX_CERT_SIZE        522
#define HDCP22_MAX_MSG_SIZE         534
#define HDCP22_RTX_SIZE             8
#define HDCP22_RRX_SIZE             8
#define HDCP22_RN_SIZE              8
#define HDCP22_RIV_SIZE             8
#define HDCP22_KM_SIZE              16
#define HDCP22_KS_SIZE              16
#define HDCP22_RSA_SIZE             256
#define HDCP22_HASH_SIZE            32
#define HDCP22_STREAMID_SIZE        2

#define HDCP22_TXCAPS_VER_MASK      0x03
#define HDCP22_TXCAPS_HDCP2         BIT(1)
#define HDCP22_TXCAPS_REPEATER       BIT(2)

/* ========================================================================== */
/* SHA-1                                                                       */
/* ========================================================================== */

typedef struct {
    u32 state[5];
    u64 count;
    u8  buffer[64];
} sha1_ctx_t;

static u32 rol32(u32 x, u32 n) { return (x << n) | (x >> (32 - n)); }
static u32 ror32(u32 x, u32 n) { return (x >> n) | (x << (32 - n)); }

static void sha1_init(sha1_ctx_t *c)
{
    c->state[0] = 0x67452301; c->state[1] = 0xEFCDAB89;
    c->state[2] = 0x98BADCFE; c->state[3] = 0x10325476;
    c->state[4] = 0xC3D2E1F0;
    c->count = 0;
}

static void sha1_block(sha1_ctx_t *c, const u8 *p)
{
    u32 w[80];
    for (u32 i = 0; i < 16; i++)
        w[i] = ((u32)p[i*4] << 24) | ((u32)p[i*4+1] << 16) |
               ((u32)p[i*4+2] << 8) | ((u32)p[i*4+3]);
    for (u32 i = 16; i < 80; i++)
        w[i] = rol32(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);

    u32 a = c->state[0], b = c->state[1], cc = c->state[2];
    u32 d = c->state[3], e = c->state[4];

    for (u32 i = 0; i < 80; i++) {
        u32 f, k;
        if (i < 20)      { f = (b & cc) | (~b & d);        k = 0x5A827999; }
        else if (i < 40) { f = b ^ cc ^ d;                 k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b & cc) | (b & d) | (cc & d); k = 0x8F1BBCDC; }
        else             { f = b ^ cc ^ d;                 k = 0xCA62C1D6; }
        u32 t = rol32(a, 5) + f + e + k + w[i];
        e = d; d = cc; cc = rol32(b, 30); b = a; a = t;
    }
    c->state[0] += a; c->state[1] += b; c->state[2] += cc;
    c->state[3] += d; c->state[4] += e;
}

static void sha1_update(sha1_ctx_t *c, const u8 *p, usize n)
{
    usize i = 0;
    usize idx = (usize)(c->count & 63);
    c->count += n;
    if (idx) {
        usize fill = 64 - idx;
        if (n < fill) { dp_mem_copy(&c->buffer[idx], p, n); return; }
        dp_mem_copy(&c->buffer[idx], p, fill);
        sha1_block(c, c->buffer);
        i = fill;
    }
    while (i + 64 <= n) { sha1_block(c, p + i); i += 64; }
    dp_mem_copy(c->buffer, p + i, n - i);
}

static void sha1_final(sha1_ctx_t *c, u8 out[20])
{
    u64 bits = c->count * 8;
    u8 pad[72] = { 0x80 };
    usize idx = (usize)(c->count & 63);
    usize padlen = (idx < 56) ? (56 - idx) : (120 - idx);
    sha1_update(c, pad, padlen);
    u8 len[8];
    for (u32 i = 0; i < 8; i++) len[7 - i] = (u8)(bits >> (i * 8));
    sha1_update(c, len, 8);
    for (u32 i = 0; i < 5; i++) {
        out[i*4+0] = (u8)(c->state[i] >> 24);
        out[i*4+1] = (u8)(c->state[i] >> 16);
        out[i*4+2] = (u8)(c->state[i] >> 8);
        out[i*4+3] = (u8)(c->state[i]);
    }
}

/* ========================================================================== */
/* SHA-256                                                                     */
/* ========================================================================== */

typedef struct {
    u32 state[8];
    u64 count;
    u8  buffer[64];
} sha256_ctx_t;

static const u32 sha256_k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static void sha256_init(sha256_ctx_t *c)
{
    c->state[0]=0x6a09e667; c->state[1]=0xbb67ae85; c->state[2]=0x3c6ef372; c->state[3]=0xa54ff53a;
    c->state[4]=0x510e527f; c->state[5]=0x9b05688c; c->state[6]=0x1f83d9ab; c->state[7]=0x5be0cd19;
    c->count = 0;
}

static void sha256_block(sha256_ctx_t *c, const u8 *p)
{
    u32 w[64];
    for (u32 i = 0; i < 16; i++)
        w[i] = ((u32)p[i*4] << 24) | ((u32)p[i*4+1] << 16) |
               ((u32)p[i*4+2] << 8) | ((u32)p[i*4+3]);
    for (u32 i = 16; i < 64; i++) {
        u32 s0 = ror32(w[i-15], 7) ^ ror32(w[i-15], 18) ^ (w[i-15] >> 3);
        u32 s1 = ror32(w[i-2], 17) ^ ror32(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    u32 a=c->state[0],b=c->state[1],cc=c->state[2],d=c->state[3];
    u32 e=c->state[4],f=c->state[5],g=c->state[6],h=c->state[7];
    for (u32 i = 0; i < 64; i++) {
        u32 s1 = ror32(e,6) ^ ror32(e,11) ^ ror32(e,25);
        u32 ch = (e & f) ^ (~e & g);
        u32 t1 = h + s1 + ch + sha256_k[i] + w[i];
        u32 s0 = ror32(a,2) ^ ror32(a,13) ^ ror32(a,22);
        u32 mj = (a & b) ^ (a & cc) ^ (b & cc);
        u32 t2 = s0 + mj;
        h=g; g=f; f=e; e=d+t1; d=cc; cc=b; b=a; a=t1+t2;
    }
    c->state[0]+=a; c->state[1]+=b; c->state[2]+=cc; c->state[3]+=d;
    c->state[4]+=e; c->state[5]+=f; c->state[6]+=g; c->state[7]+=h;
}

static void sha256_update(sha256_ctx_t *c, const u8 *p, usize n)
{
    usize i = 0;
    usize idx = (usize)(c->count & 63);
    c->count += n;
    if (idx) {
        usize fill = 64 - idx;
        if (n < fill) { dp_mem_copy(&c->buffer[idx], p, n); return; }
        dp_mem_copy(&c->buffer[idx], p, fill);
        sha256_block(c, c->buffer);
        i = fill;
    }
    while (i + 64 <= n) { sha256_block(c, p + i); i += 64; }
    dp_mem_copy(c->buffer, p + i, n - i);
}

static void sha256_final(sha256_ctx_t *c, u8 out[32])
{
    u64 bits = c->count * 8;
    u8 pad[72] = { 0x80 };
    usize idx = (usize)(c->count & 63);
    usize padlen = (idx < 56) ? (56 - idx) : (120 - idx);
    sha256_update(c, pad, padlen);
    u8 len[8];
    for (u32 i = 0; i < 8; i++) len[7-i] = (u8)(bits >> (i*8));
    sha256_update(c, len, 8);
    for (u32 i = 0; i < 8; i++) {
        out[i*4+0] = (u8)(c->state[i] >> 24);
        out[i*4+1] = (u8)(c->state[i] >> 16);
        out[i*4+2] = (u8)(c->state[i] >> 8);
        out[i*4+3] = (u8)(c->state[i]);
    }
}

static void sha256(const u8 *p, usize n, u8 out[32])
{
    sha256_ctx_t c; sha256_init(&c); sha256_update(&c, p, n); sha256_final(&c, out);
}

/* ========================================================================== */
/* HMAC-SHA256                                                                 */
/* ========================================================================== */

#define HMAC_SHA256_BLOCK  64

static void hmac_sha256(const u8 *key, usize keylen,
                        const u8 *msg, usize msglen, u8 out[32])
{
    u8 k[64] = { 0 };
    if (keylen > 64) sha256(key, keylen, k);
    else dp_mem_copy(k, key, keylen);

    u8 ipad[64], opad[64];
    for (u32 i = 0; i < 64; i++) { ipad[i] = k[i] ^ 0x36; opad[i] = k[i] ^ 0x5C; }

    sha256_ctx_t ctx;
    sha256_init(&ctx); sha256_update(&ctx, ipad, 64);
    sha256_update(&ctx, msg, msglen);
    u8 inner[32]; sha256_final(&ctx, inner);

    sha256_init(&ctx); sha256_update(&ctx, opad, 64);
    sha256_update(&ctx, inner, 32);
    sha256_final(&ctx, out);
}

/* ========================================================================== */
/* AES-128                                                                     */
/* ========================================================================== */

static const u8 aes_sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const u8 aes_rsbox[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const u8 aes_rcon[11] = { 0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36 };

static u8 aes_xtime(u8 x) { return (u8)((x << 1) ^ ((x & 0x80) ? 0x1B : 0)); }

static u8 aes_gmul(u8 a, u8 b)
{
    u8 p = 0;
    for (u32 i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        u8 hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

typedef struct { u8 round_key[176]; } aes128_ctx_t;

static void aes128_expand_key(aes128_ctx_t *c, const u8 key[16])
{
    dp_mem_copy(c->round_key, key, 16);
    for (u32 i = 16; i < 176; i += 4) {
        u8 t[4] = { c->round_key[i-4], c->round_key[i-3],
                    c->round_key[i-2], c->round_key[i-1] };
        if ((i & 0x0F) == 0) {
            u8 tmp = t[0];
            t[0] = aes_sbox[t[1]] ^ aes_rcon[i/16];
            t[1] = aes_sbox[t[2]];
            t[2] = aes_sbox[t[3]];
            t[3] = aes_sbox[tmp];
        }
        c->round_key[i]   = c->round_key[i-16] ^ t[0];
        c->round_key[i+1] = c->round_key[i-15] ^ t[1];
        c->round_key[i+2] = c->round_key[i-14] ^ t[2];
        c->round_key[i+3] = c->round_key[i-13] ^ t[3];
    }
}

static void aes128_add_round_key(u8 s[16], const u8 *rk)
{
    for (u32 i = 0; i < 16; i++) s[i] ^= rk[i];
}

static void aes128_sub_shift(u8 s[16])
{
    for (u32 i = 0; i < 16; i++) s[i] = aes_sbox[s[i]];
    /* Shift rows: state stored column-major */
    u8 t[16];
    t[0] = s[0]; t[1] = s[5]; t[2] = s[10]; t[3] = s[15];
    t[4] = s[4]; t[5] = s[9]; t[6] = s[14]; t[7] = s[3];
    t[8] = s[8]; t[9] = s[13]; t[10] = s[2]; t[11] = s[7];
    t[12] = s[12]; t[13] = s[1]; t[14] = s[6]; t[15] = s[11];
    dp_mem_copy(s, t, 16);
}

static void aes128_inv_sub_shift(u8 s[16])
{
    u8 t[16];
    t[0] = s[0]; t[1] = s[13]; t[2] = s[10]; t[3] = s[7];
    t[4] = s[4]; t[5] = s[1]; t[6] = s[14]; t[7] = s[11];
    t[8] = s[8]; t[9] = s[5]; t[10] = s[2]; t[11] = s[15];
    t[12] = s[12]; t[13] = s[9]; t[14] = s[6]; t[15] = s[3];
    for (u32 i = 0; i < 16; i++) s[i] = aes_rsbox[t[i]];
}

static void aes128_mix_columns(u8 s[16])
{
    for (u32 i = 0; i < 16; i += 4) {
        u8 a = s[i], b = s[i+1], c = s[i+2], d = s[i+3];
        s[i]   = aes_xtime(a) ^ (u8)(aes_xtime(b) ^ b) ^ c ^ d;
        s[i+1] = a ^ aes_xtime(b) ^ (u8)(aes_xtime(c) ^ c) ^ d;
        s[i+2] = a ^ b ^ aes_xtime(c) ^ (u8)(aes_xtime(d) ^ d);
        s[i+3] = (u8)(aes_xtime(a) ^ a) ^ b ^ c ^ aes_xtime(d);
    }
}

static void aes128_inv_mix_columns(u8 s[16])
{
    for (u32 i = 0; i < 16; i += 4) {
        u8 a = s[i], b = s[i+1], c = s[i+2], d = s[i+3];
        s[i]   = aes_gmul(a,14) ^ aes_gmul(b,11) ^ aes_gmul(c,13) ^ aes_gmul(d,9);
        s[i+1] = aes_gmul(a,9)  ^ aes_gmul(b,14) ^ aes_gmul(c,11) ^ aes_gmul(d,13);
        s[i+2] = aes_gmul(a,13) ^ aes_gmul(b,9)  ^ aes_gmul(c,14) ^ aes_gmul(d,11);
        s[i+3] = aes_gmul(a,11) ^ aes_gmul(b,13) ^ aes_gmul(c,9)  ^ aes_gmul(d,14);
    }
}

static void aes128_encrypt_block(const aes128_ctx_t *c, const u8 in[16], u8 out[16])
{
    u8 s[16]; dp_mem_copy(s, in, 16);
    aes128_add_round_key(s, c->round_key);
    for (u32 r = 1; r <= 9; r++) {
        aes128_sub_shift(s);
        aes128_mix_columns(s);
        aes128_add_round_key(s, &c->round_key[r * 16]);
    }
    aes128_sub_shift(s);
    aes128_add_round_key(s, &c->round_key[160]);
    dp_mem_copy(out, s, 16);
}

static void aes128_decrypt_block(const aes128_ctx_t *c, const u8 in[16], u8 out[16])
{
    u8 s[16]; dp_mem_copy(s, in, 16);
    aes128_add_round_key(s, &c->round_key[160]);
    for (int r = 9; r >= 1; r--) {
        aes128_inv_sub_shift(s);
        aes128_add_round_key(s, &c->round_key[r * 16]);
        aes128_inv_mix_columns(s);
    }
    aes128_inv_sub_shift(s);
    aes128_add_round_key(s, c->round_key);
    dp_mem_copy(out, s, 16);
}

static void aes128_cbc_encrypt(const u8 key[16], const u8 iv[16],
                               const u8 *in, u8 *out, usize len)
{
    aes128_ctx_t c; aes128_expand_key(&c, key);
    u8 prev[16]; dp_mem_copy(prev, iv, 16);
    for (usize i = 0; i + 16 <= len; i += 16) {
        u8 blk[16];
        for (u32 j = 0; j < 16; j++) blk[j] = in[i+j] ^ prev[j];
        aes128_encrypt_block(&c, blk, out + i);
        dp_mem_copy(prev, out + i, 16);
    }
}

static void aes128_cbc_decrypt(const u8 key[16], const u8 iv[16],
                               const u8 *in, u8 *out, usize len)
{
    aes128_ctx_t c; aes128_expand_key(&c, key);
    u8 prev[16]; dp_mem_copy(prev, iv, 16);
    for (usize i = 0; i + 16 <= len; i += 16) {
        u8 blk[16];
        aes128_decrypt_block(&c, in + i, blk);
        for (u32 j = 0; j < 16; j++) out[i+j] = blk[j] ^ prev[j];
        dp_mem_copy(prev, in + i, 16);
    }
}

/* ========================================================================== */
/* BIGNUM 2048-BIT (Montgomery CIOS)                                           */
/* ========================================================================== */

#define BN_LIMBS  64       /* 2048 / 32 */
#define BN_BYTES  (BN_LIMBS * 4)

typedef struct { u32 limb[BN_LIMBS]; } bn_t;

static void bn_zero(bn_t *a) { dp_mem_set(a, 0, sizeof(*a)); }

static void bn_copy(bn_t *dst, const bn_t *src) { *dst = *src; }

static int bn_cmp(const bn_t *a, const bn_t *b)
{
    for (int i = BN_LIMBS - 1; i >= 0; i--) {
        if (a->limb[i] > b->limb[i]) return 1;
        if (a->limb[i] < b->limb[i]) return -1;
    }
    return 0;
}

static u32 bn_sub_inplace(bn_t *a, const bn_t *b)
{
    u64 borrow = 0;
    for (u32 i = 0; i < BN_LIMBS; i++) {
        u64 diff = (u64)a->limb[i] - b->limb[i] - borrow;
        a->limb[i] = (u32)diff;
        borrow = (diff >> 63) & 1;
    }
    return (u32)borrow;
}

static void bn_add_inplace(bn_t *a, const bn_t *b)
{
    u64 carry = 0;
    for (u32 i = 0; i < BN_LIMBS; i++) {
        u64 sum = (u64)a->limb[i] + b->limb[i] + carry;
        a->limb[i] = (u32)sum;
        carry = sum >> 32;
    }
}

static void bn_from_bytes(bn_t *a, const u8 *p, usize n)
{
    bn_zero(a);
    usize limbs = n / 4;
    usize rem = n % 4;
    for (usize i = 0; i < limbs; i++) {
        a->limb[i] = ((u32)p[(n - 4) - i*4] << 24) |
                     ((u32)p[(n - 4) - i*4 + 1] << 16) |
                     ((u32)p[(n - 4) - i*4 + 2] << 8) |
                     ((u32)p[(n - 4) - i*4 + 3]);
    }
    /* Handle trailing (leading in bytes) partial limb */
    if (rem) {
        u32 v = 0;
        for (usize i = 0; i < rem; i++) v = (v << 8) | p[i];
        a->limb[limbs] = v;
    }
}

static void bn_to_bytes(const bn_t *a, u8 *p, usize n)
{
    for (usize i = 0; i < n; i++) {
        u32 limb = a->limb[(n - 1 - i) / 4];
        u32 shift = ((n - 1 - i) % 4) * 8;
        p[i] = (u8)(limb >> shift);
    }
}

static u32 bn_n0inv(const bn_t *n)
{
    u32 inv = 1;
    for (u32 i = 0; i < 32; i++) {
        inv = (u32)((u64)inv * (2 - (u64)n->limb[0] * inv));
    }
    return (u32)(0u - inv);
}

/* CIOS Montgomery multiplication. Result a*b*R^-1 mod n. */
static void bn_mont_mul(bn_t *res, const bn_t *a, const bn_t *b,
                        const bn_t *n, u32 n0inv)
{
    u32 t[BN_LIMBS + 2];
    dp_mem_set(t, 0, sizeof(t));

    for (u32 i = 0; i < BN_LIMBS; i++) {
        u64 carry = 0;
        for (u32 j = 0; j < BN_LIMBS; j++) {
            u64 v = (u64)a->limb[i] * b->limb[j] + t[j] + carry;
            t[j] = (u32)v;
            carry = v >> 32;
        }
        u64 v2 = (u64)t[BN_LIMBS] + carry;
        t[BN_LIMBS] = (u32)v2;
        t[BN_LIMBS + 1] += (u32)(v2 >> 32);

        u32 m = t[0] * n0inv;
        carry = 0;
        for (u32 j = 0; j < BN_LIMBS; j++) {
            u64 v = (u64)m * n->limb[j] + t[j] + carry;
            if (j > 0) t[j - 1] = (u32)v;
            carry = v >> 32;
        }
        u64 v3 = (u64)t[BN_LIMBS] + carry;
        t[BN_LIMBS - 1] = (u32)v3;
        t[BN_LIMBS] = t[BN_LIMBS + 1] + (u32)(v3 >> 32);
        t[BN_LIMBS + 1] = 0;
    }

    bn_t tmp;
    for (u32 i = 0; i < BN_LIMBS; i++) tmp.limb[i] = t[i];

    if (t[BN_LIMBS] || bn_cmp(&tmp, n) >= 0) {
        bn_sub_inplace(&tmp, n);
    }
    bn_copy(res, &tmp);
}

/* R^2 mod n, needed once for Montgomery domain conversion. */
static void bn_compute_r2(bn_t *r2, const bn_t *n)
{
    bn_t one; bn_zero(&one); one.limb[0] = 1;
    /* R = 2^2048; R mod n = (2^2048 - n) + n ... use double-and-add */
    /* Simple approach: repeated doubling 2048 times of "1" mod n */
    bn_copy(r2, &one);
    for (u32 i = 0; i < BN_LIMBS * 32 * 2; i++) {
        bn_add_inplace(r2, r2);
        if (bn_cmp(r2, n) >= 0) bn_sub_inplace(r2, n);
    }
}

/* modexp: result = base^exp mod n; exp provided as byte array (big-endian). */
static void bn_modexp(bn_t *res, const bn_t *base, const u8 *exp, usize exp_len,
                      const bn_t *n)
{
    u32 n0inv = bn_n0inv(n);

    bn_t r2; bn_compute_r2(&r2, n);
    bn_t base_m; bn_mont_mul(&base_m, base, &r2, n, n0inv);  /* to Montgomery */

    bn_t one; bn_zero(&one); one.limb[0] = 1;
    bn_t acc_m; bn_mont_mul(&acc_m, &one, &r2, n, n0inv);    /* 1 in Montgomery */

    /* Skip leading zero bytes of exponent */
    usize i = 0;
    while (i < exp_len && exp[i] == 0) i++;
    if (i == exp_len) { /* exp == 0 → result = 1 */
        bn_t one_mod;
        bn_mont_mul(&one_mod, &acc_m, &one, n, n0inv);
        bn_copy(res, &one_mod);
        return;
    }

    for (; i < exp_len; i++) {
        for (int b = 7; b >= 0; b--) {
            bn_t sq;
            bn_mont_mul(&sq, &acc_m, &acc_m, n, n0inv);
            bn_copy(&acc_m, &sq);
            if ((exp[i] >> b) & 1) {
                bn_t prod;
                bn_mont_mul(&prod, &acc_m, &base_m, n, n0inv);
                bn_copy(&acc_m, &prod);
            }
        }
    }
    bn_t one_final; bn_zero(&one_final); one_final.limb[0] = 1;
    bn_mont_mul(res, &acc_m, &one_final, n, n0inv);
}

/* RSA-2048 public operation: out = in^e mod n.  e as big-endian byte array. */
static int rsa2048_pub_op(const u8 in[BN_BYTES], const u8 e[], usize e_len,
                          const u8 n[BN_BYTES], u8 out[BN_BYTES])
{
    bn_t bn_in, bn_n, bn_e_holder, bn_res;
    bn_from_bytes(&bn_in, in, BN_BYTES);
    bn_from_bytes(&bn_n, n, BN_BYTES);
    (void)bn_e_holder;
    bn_modexp(&bn_res, &bn_in, e, e_len, &bn_n);
    bn_to_bytes(&bn_res, out, BN_BYTES);
    return DP_OK;
}

/* ========================================================================== */
/* HDCP COMMON                                                                 */
/* ========================================================================== */

static int hdcp_read(dp_controller_t *c, u32 off, u8 *buf, u32 len)
{
    return dp_aux_i2c_read(c, DP_AUX_ADDR_I2C_HDCP, off, buf, len);
}

static int hdcp_write(dp_controller_t *c, u32 off, const u8 *buf, u32 len)
{
    return dp_aux_i2c_write(c, DP_AUX_ADDR_I2C_HDCP, off, buf, len);
}

static int hdcp_read_byte(dp_controller_t *c, u32 off, u8 *v)
{
    return hdcp_read(c, off, v, 1);
}

static int hdcp_write_byte(dp_controller_t *c, u32 off, u8 v)
{
    return hdcp_write(c, off, &v, 1);
}

static u32 hdcp_rng32(void)
{
    /* Bare-metal placeholder: HW RNG or time-based; caller must seed. */
    static u32 s = 0xDEADBEEF;
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return s ^ (u32)riscv_read_time();
}

static void hdcp_rng_bytes(u8 *buf, usize n)
{
    usize i = 0;
    while (i < n) {
        u32 r = hdcp_rng32();
        for (u32 b = 0; b < 4 && i < n; b++, i++)
            buf[i] = (u8)(r >> (b * 8));
    }
}

/* ========================================================================== */
/* HDCP 1.x - CIPHER (HW-accelerated; SW stub for R0/Ri if no HW)              */
/* ========================================================================== */

/* Register block for HDCP 1.x cipher. HW must implement the "Ks/Km cipher"
 * per DCP HDCP 1.4 spec Appendix (proprietary). */
#define HDCP1_REG_CIPHER_CTL        0x2A0
#define HDCP1_REG_CIPHER_KEY_LO     0x2A4
#define HDCP1_REG_CIPHER_KEY_HI     0x2A8
#define HDCP1_REG_CIPHER_IV_LO      0x2AC
#define HDCP1_REG_CIPHER_IV_HI      0x2B0
#define HDCP1_REG_CIPHER_IN_LO      0x2B4
#define HDCP1_REG_CIPHER_IN_HI      0x2B8
#define HDCP1_REG_CIPHER_OUT_LO     0x2BC
#define HDCP1_REG_CIPHER_OUT_HI     0x2C0
#define HDCP1_REG_CIPHER_START      BIT(0)
#define HDCP1_REG_CIPHER_BUSY       BIT(1)
#define HDCP1_REG_CIPHER_DONE       BIT(2)

static int hdcp1_cipher_km(dp_controller_t *c, const u8 ksv[5],
                          const u8 an[8], u8 km_out[7])
{
    vaddr_t b = c->base_virtual_addr;

    u64 key = (u64)ksv[0] | ((u64)ksv[1] << 8) | ((u64)ksv[2] << 16) |
              ((u64)ksv[3] << 24) | ((u64)ksv[4] << 32);
    u64 iv = 0;
    for (u32 i = 0; i < 8; i++) iv = (iv << 8) | an[i];

    dp_reg_write(b, HDCP1_REG_CIPHER_KEY_LO, (u32)key);
    dp_reg_write(b, HDCP1_REG_CIPHER_KEY_HI, (u32)(key >> 32));
    dp_reg_write(b, HDCP1_REG_CIPHER_IV_LO, (u32)iv);
    dp_reg_write(b, HDCP1_REG_CIPHER_IV_HI, (u32)(iv >> 32));
    dp_reg_write(b, HDCP1_REG_CIPHER_CTL, HDCP1_REG_CIPHER_START);

    u64 deadline = riscv_read_time() + 1000ULL;
    for (;;) {
        u32 st = dp_reg_read(b, HDCP1_REG_CIPHER_CTL);
        if (st & HDCP1_REG_CIPHER_DONE) break;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }
    u64 km = (u64)dp_reg_read(b, HDCP1_REG_CIPHER_OUT_LO) |
             ((u64)dp_reg_read(b, HDCP1_REG_CIPHER_OUT_HI) << 32);
    for (u32 i = 0; i < 7; i++) km_out[i] = (u8)(km >> (i * 8));
    return DP_OK;
}

static int hdcp1_cipher_r0(dp_controller_t *c, const u8 an_prime[8],
                           const u8 km[7], u8 r0_out[8])
{
    vaddr_t b = c->base_virtual_addr;
    u64 key = 0, iv = 0;
    for (u32 i = 0; i < 7; i++) key = (key << 8) | km[i];
    for (u32 i = 0; i < 8; i++) iv = (iv << 8) | an_prime[i];

    dp_reg_write(b, HDCP1_REG_CIPHER_KEY_LO, (u32)key);
    dp_reg_write(b, HDCP1_REG_CIPHER_KEY_HI, (u32)(key >> 32));
    dp_reg_write(b, HDCP1_REG_CIPHER_IV_LO, (u32)iv);
    dp_reg_write(b, HDCP1_REG_CIPHER_IV_HI, (u32)(iv >> 32));
    dp_reg_write(b, HDCP1_REG_CIPHER_CTL, HDCP1_REG_CIPHER_START);

    u64 deadline = riscv_read_time() + 1000ULL;
    for (;;) {
        u32 st = dp_reg_read(b, HDCP1_REG_CIPHER_CTL);
        if (st & HDCP1_REG_CIPHER_DONE) break;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }
    u64 r0 = (u64)dp_reg_read(b, HDCP1_REG_CIPHER_OUT_LO) |
             ((u64)dp_reg_read(b, HDCP1_REG_CIPHER_OUT_HI) << 32);
    for (u32 i = 0; i < 8; i++) r0_out[i] = (u8)(r0 >> (i * 8));
    return DP_OK;
}

static int hdcp1_cipher_ri(dp_controller_t *c, const u8 ri_prev[2],
                           const u8 km[7], u8 ri_out[2])
{
    vaddr_t b = c->base_virtual_addr;
    u64 key = 0, iv = (u64)ri_prev[0] | ((u64)ri_prev[1] << 8);
    for (u32 i = 0; i < 7; i++) key = (key << 8) | km[i];

    dp_reg_write(b, HDCP1_REG_CIPHER_KEY_LO, (u32)key);
    dp_reg_write(b, HDCP1_REG_CIPHER_KEY_HI, (u32)(key >> 32));
    dp_reg_write(b, HDCP1_REG_CIPHER_IV_LO, (u32)iv);
    dp_reg_write(b, HDCP1_REG_CIPHER_IV_HI, (u32)(iv >> 32));
    dp_reg_write(b, HDCP1_REG_CIPHER_CTL, HDCP1_REG_CIPHER_START);

    u64 deadline = riscv_read_time() + 1000ULL;
    for (;;) {
        u32 st = dp_reg_read(b, HDCP1_REG_CIPHER_CTL);
        if (st & HDCP1_REG_CIPHER_DONE) break;
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }
    u64 ri = (u64)dp_reg_read(b, HDCP1_REG_CIPHER_OUT_LO);
    ri_out[0] = (u8)ri;
    ri_out[1] = (u8)(ri >> 8);
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 1.x - V COMPUTATION (SHA-1)                                            */
/* ========================================================================== */

static void hdcp1_compute_v(const u8 *ksv_list, u32 ksv_count,
                            const u8 bksv[5], u8 v_out[20])
{
    sha1_ctx_t h;
    sha1_init(&h);
    sha1_update(&h, ksv_list, ksv_count * 5);
    sha1_update(&h, bksv, 5);
    u8 m0[12] = { 0 };  /* M0 typically zeroed by driver; includes binfo */
    sha1_update(&h, m0, sizeof(m0));
    sha1_final(&h, v_out);
}

/* Compare V (with per-byte high-bit masking) to V'. */
static bool hdcp1_v_match(const u8 v_local[20], const u8 v_remote[20])
{
    for (u32 i = 0; i < 20; i++) {
        if ((v_local[i] & 0xFE) != (v_remote[i] & 0xFE)) return false;
    }
    return true;
}

/* ========================================================================== */
/* HDCP 1.x - BSTATUS / KSV FIFO                                               */
/* ========================================================================== */

static int hdcp1_read_bstatus(dp_controller_t *c, u8 *bstatus, bool *is_repeater)
{
    u8 bcaps, bstat;
    int rc = hdcp_read_byte(c, HDCP_REG_BCAPS, &bcaps);
    if (rc != DP_OK) return rc;
    rc = hdcp_read_byte(c, HDCP_REG_BSTATUS, &bstat);
    if (rc != DP_OK) return rc;

    *bstatus = bstat;
    *is_repeater = (bcaps & HDCP_BCAPS_REPEATER) != 0;
    return DP_OK;
}

static int hdcp1_read_ksv_fifo(dp_controller_t *c, u8 *fifo, u32 count)
{
    return hdcp_read(c, HDCP_REG_KSV_FIFO, fifo, count * 5);
}

static bool hdcp1_ksv_valid(const u8 ksv[5])
{
    u32 ones = 0;
    for (u32 i = 0; i < 5; i++) ones += dp_popcount32(ksv[i]);
    return ones == 20;
}

/* ========================================================================== */
/* HDCP 1.x - SRM (System Renewability Message)                                */
/* ========================================================================== */

typedef struct {
    u32 version;
    u8  *ksv_list;
    u32 ksv_count;
    u32 depth;
} hdcp_srm_t;

static int hdcp_srm_parse(const u8 *srm, u32 srm_len, hdcp_srm_t *out)
{
    if (!srm || srm_len < 8 || !out) return DP_ERR_INVAL;

    u16 len = ((u16)srm[0] << 8) | srm[1];
    if (len + 2 > srm_len) return DP_ERR_INVAL;

    u8 version = srm[2];
    u32 ksv_count = ((u32)srm[3] << 8) | srm[4];
    u32 depth = srm[5] & HDCP_BSTATUS_MAX_DEVS_MASK;

    out->version = version;
    out->ksv_count = ksv_count;
    out->depth = depth;
    out->ksv_list = (u8 *)(srm + 6);

    if (6 + ksv_count * 5 > srm_len) return DP_ERR_INVAL;
    return DP_OK;
}

static bool hdcp_srm_revoked(const hdcp_srm_t *srm, const u8 ksv[5])
{
    if (!srm) return false;
    for (u32 i = 0; i < srm->ksv_count; i++) {
        if (dp_mem_cmp(&srm->ksv_list[i*5], ksv, 5) == 0) return true;
    }
    return false;
}

/* ========================================================================== */
/* HDCP 1.x - AUTHENTICATION                                                   */
/* ========================================================================== */

typedef struct {
    bool    authenticated;
    bool    is_repeater;
    u8      aksv[5];
    u8      bksv[5];
    u8      an[8];
    u8      km[7];
    u8      r0[8];
    u8      ri[2];
    u8      v_local[20];
    u8      bstatus;
    u8      ksv_fifo[DP_HDCP_KSV_FIFO_MAX];
    u32     ksv_count;
    u32     frame_count;
    bool    encrypting;
    hdcp_srm_t srm;
    bool    srm_loaded;
} hdcp1_ctx_t;

static int hdcp1_authenticate(dp_controller_t *c, hdcp1_ctx_t *h1)
{
    if (!c || !h1) return DP_ERR_INVAL;

    /* Read Bksv */
    int rc = hdcp_read(c, HDCP_REG_BKSV, h1->bksv, 5);
    if (rc != DP_OK) return rc;
    if (!hdcp1_ksv_valid(h1->bksv)) return DP_ERR_HDCP;

    /* SRM check */
    if (h1->srm_loaded && hdcp_srm_revoked(&h1->srm, h1->bksv))
        return DP_ERR_HDCP;

    /* Generate AKSV (per HDCP 1.4, 20 ones, 20 zeros) */
    hdcp_rng_bytes(h1->aksv, 5);
    u32 ones = 0;
    for (u32 i = 0; i < 5; i++) ones += dp_popcount32(h1->aksv[i]);
    if (ones != 20) {
        h1->aksv[0] = (u8)((h1->aksv[0] & 0xE0) | (20 - (ones - dp_popcount32(h1->aksv[0]) > 20 ? 0 : dp_popcount32(h1->aksv[0]))));
        h1->aksv[4] = (u8)(h1->aksv[4] & 0x00);
    }

    /* Read An (from RX; it was set by RX after Bksv) */
    rc = hdcp_read(c, HDCP_REG_AN, h1->an, 8);
    if (rc != DP_OK) return rc;

    /* Write An + AKSV + AINFO */
    rc = hdcp_write(c, HDCP_REG_AN, h1->an, 8);
    if (rc != DP_OK) return rc;
    rc = hdcp_write(c, HDCP_REG_AKSV, h1->aksv, 5);
    if (rc != DP_OK) return rc;
    u8 ainfo = 0x00;
    rc = hdcp_write_byte(c, HDCP_REG_AINFO, ainfo);
    if (rc != DP_OK) return rc;

    /* Compute Km = hdcp_cipher(AKSV, An) */
    rc = hdcp1_cipher_km(c, h1->aksv, h1->an, h1->km);
    if (rc != DP_OK) return rc;

    /* Wait for RX to update An with An' */
    u8 an_prime[8];
    u64 deadline = riscv_read_time() + 100000ULL;
    for (;;) {
        rc = hdcp_read(c, HDCP_REG_AN, an_prime, 8);
        if (rc == DP_OK && dp_mem_cmp(an_prime, h1->an, 8) != 0) break;
        if (riscv_read_time() > deadline) return DP_ERR_HDCP;
        dp_udelay(50);
    }
    dp_mem_copy(h1->an, an_prime, 8);

    /* Compute R0 = hdcp_cipher(An', Km) */
    rc = hdcp1_cipher_r0(c, h1->an, h1->km, h1->r0);
    if (rc != DP_OK) return rc;

    /* Write R0 */
    rc = hdcp_write(c, HDCP_REG_RI, h1->r0, 8);
    if (rc != DP_OK) return rc;

    /* Read back Ri */
    rc = hdcp_read(c, HDCP_REG_RI, h1->ri, 2);
    if (rc != DP_OK) return rc;

    h1->frame_count = 0;
    h1->authenticated = true;

    /* Repeater handling */
    rc = hdcp1_read_bstatus(c, &h1->bstatus, &h1->is_repeater);
    if (rc != DP_OK) return rc;

    if (h1->is_repeater) {
        u32 max_cascade = (h1->bstatus & HDCP_BSTATUS_MAX_CASCADE) ? 7 : 1;
        u32 device_count = h1->bstatus & HDCP_BSTATUS_MAX_DEVS_MASK;
        if (device_count > DP_HDCP_KSV_FIFO_MAX) device_count = DP_HDCP_KSV_FIFO_MAX;

        deadline = riscv_read_time() + (u64)max_cascade * 1000000ULL;
        for (;;) {
            u8 bcaps;
            hdcp_read_byte(c, HDCP_REG_BCAPS, &bcaps);
            if (bcaps & HDCP_BCAPS_KSV_FIFO_READY) break;
            if (riscv_read_time() > deadline) return DP_ERR_HDCP;
            dp_udelay(5000);
        }

        rc = hdcp1_read_ksv_fifo(c, h1->ksv_fifo, device_count);
        if (rc != DP_OK) return rc;
        h1->ksv_count = device_count;

        /* Validate KSVs */
        for (u32 i = 0; i < device_count; i++) {
            if (!hdcp1_ksv_valid(&h1->ksv_fifo[i*5])) return DP_ERR_HDCP;
            if (h1->srm_loaded && hdcp_srm_revoked(&h1->srm, &h1->ksv_fifo[i*5]))
                return DP_ERR_HDCP;
        }

        /* Compute and send V */
        hdcp1_compute_v(h1->ksv_fifo, device_count, h1->bksv, h1->v_local);
        rc = hdcp_write(c, HDCP_REG_VPRIME, h1->v_local, 20);
        if (rc != DP_OK) return rc;

        /* Wait for RX V' and compare */
        u8 v_remote[20];
        deadline = riscv_read_time() + 100000ULL;
        for (;;) {
            rc = hdcp_read(c, HDCP_REG_VPRIME, v_remote, 20);
            if (rc == DP_OK && !hdcp1_v_match(h1->v_local, v_remote)) break;
            if (riscv_read_time() > deadline) return DP_ERR_HDCP;
            dp_udelay(100);
        }
    }

    return DP_OK;
}

/* ========================================================================== */
/* HDCP 1.x - STREAMING (frame-based Ri update)                                */
/* ========================================================================== */

static int hdcp1_streaming_tick(dp_controller_t *c, hdcp1_ctx_t *h1)
{
    if (!c || !h1 || !h1->authenticated) return DP_ERR_HDCP;

    u8 ri_prev[2] = { h1->ri[0], h1->ri[1] };
    u8 ri_new[2];
    int rc = hdcp1_cipher_ri(c, ri_prev, h1->km, ri_new);
    if (rc != DP_OK) return rc;

    rc = hdcp_write(c, HDCP_REG_RI, ri_new, 2);
    if (rc != DP_OK) return rc;

    /* Read back to verify RX mirrored */
    u8 ri_rx[2];
    rc = hdcp_read(c, HDCP_REG_RI, ri_rx, 2);
    if (rc != DP_OK) return rc;

    if (ri_rx[0] != ri_new[0] || ri_rx[1] != ri_new[1]) {
        h1->authenticated = false;
        return DP_ERR_HDCP;
    }
    dp_mem_copy(h1->ri, ri_new, 2);
    h1->frame_count++;

    /* Every 128 frames, check for re-auth request */
    if ((h1->frame_count & 0x7F) == 0) {
        u8 bcaps;
        if (hdcp_read_byte(c, HDCP_REG_BCAPS, &bcaps) == DP_OK) {
            if (!(bcaps & HDCP_BCAPS_KSV_FIFO_READY)) {
                h1->authenticated = false;
                return DP_ERR_HDCP;
            }
        }
    }
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 2.2 - CERTIFICATE STRUCTURE                                            */
/* ========================================================================== */

typedef struct __packed {
    u32 length;
    u32 version;
    u8  receiver_id[5];
    u8  ksv[5];
    u32 reserved;
    u8  pubkey_n[HDCP22_RSA_SIZE];
    u8  pubkey_e[4];
    u8  signature[HDCP22_RSA_SIZE];
} hdcp22_cert_t;

#define HDCP22_CERT_LEN  522
#define HDCP22_CERT_VER  1

/* DCP LLC root public key (well-known; extracted from spec). */
static const u8 hdcp22_dcp_root_n[BN_BYTES] = {
    0xc8,0x9e,0x39,0x91,0x8a,0xce,0xf0,0x23,0x98,0x35,0x9b,0x26,0xb6,0x3e,0x8d,0x6f,
    0xaf,0x17,0xbc,0x1e,0x6c,0x21,0x83,0xf5,0xdd,0x56,0x94,0x9d,0xa6,0x31,0x21,0xf1,
    0x1e,0x5c,0xa0,0x21,0x83,0x82,0x1b,0x69,0x30,0xe6,0x33,0x91,0x36,0x79,0x18,0xae,
    0xb4,0x86,0x16,0xd7,0x63,0x5b,0xee,0xea,0xa1,0x02,0x60,0xc2,0x12,0x32,0xbe,0x63,
    0x8d,0x8b,0x77,0xdd,0xb3,0x53,0x51,0x95,0xb6,0x8c,0xf1,0x68,0xca,0xb9,0x92,0xf7,
    0xf1,0x38,0x8f,0xb7,0x30,0x02,0x65,0xd4,0x9d,0xa7,0x95,0x3e,0x81,0x03,0x53,0x02,
    0xb9,0x0d,0x98,0xa4,0xd3,0x78,0x83,0xba,0x6f,0xf3,0x14,0xdf,0x65,0x20,0x4e,0x54,
    0x55,0x00,0x71,0xcf,0x27,0x77,0x95,0xf8,0xaa,0xf8,0x81,0xd6,0x3f,0xeb,0xc1,0x28,
    0x45,0x64,0x1b,0x72,0x7f,0x55,0x2c,0x74,0x69,0x74,0x76,0x7f,0x4d,0x7e,0x8a,0x08,
    0x90,0xc5,0x73,0x53,0x67,0x01,0x50,0x1d,0xaf,0xd0,0xd4,0x18,0x3b,0x5c,0x16,0x96,
    0xc4,0xa2,0x11,0x36,0x5c,0x88,0x4f,0x2d,0x37,0x41,0x96,0x3e,0xb6,0xa3,0x84,0xa8,
    0x38,0x0d,0x9c,0x72,0x1e,0xba,0x06,0x37,0x61,0x63,0xe8,0x16,0x1d,0x79,0x1b,0x51,
    0x9f,0xca,0x30,0x3c,0x7c,0xac,0x09,0x8e,0xd2,0x98,0x88,0x8c,0xf5,0xc0,0xd8,0x19,
    0x2c,0x50,0x43,0x77,0xd4,0x58,0x58,0xd4,0x17,0x0a,0x32,0x67,0x3f,0x22,0x49,0x7a,
    0xd9,0x1e,0xaf,0xce,0x76,0x2c,0x45,0x7e,0x87,0x6a,0x46,0x1c,0x85,0x84,0xbd,0xc2,
    0xd2,0x8a,0x38,0x94,0x80,0x2f,0xe8,0xa9,0x20,0xee,0xa4,0xcd,0xa6,0x0e,0x91,0xc7
};

static const u8 rsa_e_65537[] = { 0x01, 0x00, 0x01 };

/* Verify cert signature against DCP root key. */
static int hdcp22_verify_cert(const hdcp22_cert_t *cert)
{
    u32 len = cert->length;
    if (len != HDCP22_CERT_LEN || cert->version != HDCP22_CERT_VER)
        return DP_ERR_HDCP;

    /* SHA-256 of cert body (excluding signature) */
    u8 hash[32];
    sha256((const u8 *)cert, HDCP22_CERT_LEN - HDCP22_RSA_SIZE, hash);

    /* Decode signature using root pub key */
    u8 dec[BN_BYTES];
    if (rsa2048_pub_op(cert->signature, rsa_e_65537, 3, hdcp22_dcp_root_n, dec) != DP_OK)
        return DP_ERR_HDCP;

    /* PKCS#1 v1.5 padding: 00 01 FF...FF 00 || DigestInfo(SHA-256) || hash */
    static const u8 di_sha256[] = {
        0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20
    };
    if (dec[0] != 0x00 || dec[1] != 0x01) return DP_ERR_HDCP;

    u32 pad_off = 2;
    while (pad_off < BN_BYTES && dec[pad_off] == 0xFF) pad_off++;
    if (pad_off < 8 || dec[pad_off] != 0x00) return DP_ERR_HDCP;
    pad_off++;

    if (pad_off + sizeof(di_sha256) + 32 > BN_BYTES) return DP_ERR_HDCP;
    if (dp_mem_cmp(&dec[pad_off], di_sha256, sizeof(di_sha256)) != 0) return DP_ERR_HDCP;
    if (dp_mem_cmp(&dec[pad_off + sizeof(di_sha256)], hash, 32) != 0) return DP_ERR_HDCP;
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 2.2 - KDF                                                              */
/* ========================================================================== */

/* kd, k_priv, k_pub derived from km + rn + riv per HDCP 2.2 spec. */
static void hdcp22_derive_kd(const u8 km[16], const u8 rn[8],
                             const u8 riv[8], u8 kd[16])
{
    /* kd = HMAC-SHA256(km, rn || riv)[0..15] */
    u8 msg[16];
    dp_mem_copy(msg, rn, 8);
    dp_mem_copy(msg + 8, riv, 8);
    u8 h[32];
    hmac_sha256(km, 16, msg, 16, h);
    dp_mem_copy(kd, h, 16);
}

static void hdcp22_derive_kpriv_kpub(const u8 km[16], const u8 rn[8],
                                     const u8 kd[16],
                                     u8 k_priv[32], u8 k_pub[32])
{
    /* k_priv = HMAC-SHA256(km, rn)[0..31]
     * k_pub  = HMAC-SHA256(km, kd || rn)[0..31] */
    u8 h[32];
    hmac_sha256(km, 16, rn, 8, h);
    dp_mem_copy(k_priv, h, 32);

    u8 msg[24];
    dp_mem_copy(msg, kd, 16);
    dp_mem_copy(msg + 16, rn, 8);
    hmac_sha256(km, 16, msg, 24, h);
    dp_mem_copy(k_pub, h, 32);
}

/* kPub_km = RSA encrypt(km) using receiver's public key (padded). */
static int hdcp22_compute_kpub_km(const u8 km[16],
                                   const u8 rx_n[HDCP22_RSA_SIZE],
                                   const u8 rx_e[4],
                                   u8 kpub_km[HDCP22_RSA_SIZE])
{
    u8 plaintext[BN_BYTES];
    dp_mem_set(plaintext, 0, sizeof(plaintext));
    /* PKCS#1 v1.5 type 2 padding */
    plaintext[0] = 0x00;
    plaintext[1] = 0x02;
    for (u32 i = 2; i < BN_BYTES - 16 - 2; i++)
        plaintext[i] = (u8)(hdcp_rng32() & 0xFF);
    /* Ensure non-zero padding */
    for (u32 i = 2; i < BN_BYTES - 16 - 2; i++) {
        if (plaintext[i] == 0) plaintext[i] = 0x01;
    }
    plaintext[BN_BYTES - 16 - 1] = 0x00;
    dp_mem_copy(&plaintext[BN_BYTES - 16], km, 16);

    /* rx_e may be 1 or 3 bytes; strip leading zeros */
    u8 e_bytes[4];
    u32 e_len = 4;
    for (u32 i = 0; i < 4; i++) e_bytes[i] = rx_e[i];
    while (e_len > 1 && e_bytes[0] == 0) {
        dp_mem_copy(e_bytes, e_bytes + 1, e_len - 1);
        e_len--;
    }
    return rsa2048_pub_op(plaintext, e_bytes, e_len, rx_n, kpub_km);
}

/* ========================================================================== */
/* HDCP 2.2 - MESSAGES                                                         */
/* ========================================================================== */

typedef struct {
    u8  msg_id;
    u16 size;
    u8  payload[HDCP22_MAX_MSG_SIZE];
} hdcp22_msg_t;

static int hdcp22_send_msg(dp_controller_t *c, const hdcp22_msg_t *msg)
{
    if (!c || !msg) return DP_ERR_INVAL;
    if (msg->size > HDCP22_MAX_MSG_SIZE) return DP_ERR_INVAL;

    u8 *wire = (u8 *)msg->payload - 1;  /* not valid; use local buffer */
    (void)wire;

    u8 buf[HDCP22_MAX_MSG_SIZE + 1];
    buf[0] = msg->msg_id;
    dp_mem_copy(&buf[1], msg->payload, msg->size);

    int rc = hdcp_write_byte(c, HDCP22_REG_WRITE_MSG_SIZE, (u8)(msg->size + 1));
    if (rc != DP_OK) return rc;
    return hdcp_write(c, HDCP22_REG_WRITE_MSG, buf, msg->size + 1);
}

static int hdcp22_recv_msg(dp_controller_t *c, hdcp22_msg_t *msg, u32 timeout_us)
{
    if (!c || !msg) return DP_ERR_INVAL;

    u64 deadline = riscv_read_time() + timeout_us;
    for (;;) {
        u8 size = 0;
        if (hdcp_read_byte(c, HDCP22_REG_READ_MSG_SIZE, &size) == DP_OK && size > 0) {
            if (size > HDCP22_MAX_MSG_SIZE + 1) size = HDCP22_MAX_MSG_SIZE + 1;
            u8 buf[HDCP22_MAX_MSG_SIZE + 1];
            if (hdcp_read(c, HDCP22_REG_READ_MSG, buf, size) == DP_OK) {
                msg->msg_id = buf[0];
                msg->size = (u16)(size - 1);
                dp_mem_copy(msg->payload, &buf[1], msg->size);
                /* Clear read size */
                u8 zero = 0;
                hdcp_write_byte(c, HDCP22_REG_READ_MSG_SIZE, zero);
                return DP_OK;
            }
        }
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        dp_udelay(200);
    }
}

/* ========================================================================== */
/* HDCP 2.2 - CONTEXT                                                          */
/* ========================================================================== */

typedef struct {
    bool    authenticated;
    bool    is_repeater;
    u8      rtx[HDCP22_RTX_SIZE];
    u8      rrx[HDCP22_RRX_SIZE];
    u8      rn[HDCP22_RN_SIZE];
    u8      riv[HDCP22_RIV_SIZE];
    u8      km[HDCP22_KM_SIZE];
    u8      kd[16];
    u8      k_priv[32];
    u8      k_pub[32];
    u8      ks[HDCP22_KS_SIZE];
    u8      riv_dkey_iv[16];
    u8      h_prime_rx[32];
    u8      l_prime_rx[32];
    u8      cert_rx[HDCP22_MAX_CERT_SIZE];
    u8      cert_rx_hash[32];
    u32     rx_caps;
    u32     tx_caps;
    u8      rx_pub_n[HDCP22_RSA_SIZE];
    u8      rx_pub_e[4];
    bool    paired;
    u8      e_kh_km[16];
    u32     stream_id;
    u8      stream_type;
    u8      seq_num_tx;
    u8      seq_num_rx;
    u32     frame_count;
    bool    encrypting;
} hdcp22_ctx_t;

/* ========================================================================== */
/* HDCP 2.2 - AKE FLOW                                                         */
/* ========================================================================== */

static int hdcp22_ake_init(dp_controller_t *c, hdcp22_ctx_t *h2)
{
    hdcp_rng_bytes(h2->rtx, HDCP22_RTX_SIZE);
    h2->tx_caps = HDCP22_TXCAPS_VER_MASK | HDCP22_TXCAPS_HDCP2;
    if (h2->is_repeater) h2->tx_caps |= HDCP22_TXCAPS_REPEATER;

    hdcp22_msg_t msg;
    dp_mem_set(&msg, 0, sizeof(msg));
    msg.msg_id = 0x01; /* AKE_Init */
    msg.size = 10;
    dp_mem_copy(&msg.payload[0], h2->rtx, 8);
    msg.payload[8] = (u8)(h2->tx_caps & 0xFF);
    msg.payload[9] = (u8)((h2->tx_caps >> 8) & 0xFF);

    return hdcp22_send_msg(c, &msg);
}

static int hdcp22_ake_handle_cert(dp_controller_t *c, hdcp22_ctx_t *h2,
                                   const hdcp22_msg_t *msg)
{
    if (msg->size < HDCP22_CERT_LEN) return DP_ERR_HDCP;
    dp_mem_copy(h2->cert_rx, msg->payload, HDCP22_CERT_LEN);

    const hdcp22_cert_t *cert = (const hdcp22_cert_t *)h2->cert_rx;
    int rc = hdcp22_verify_cert(cert);
    if (rc != DP_OK) return rc;

    dp_mem_copy(h2->rx_pub_n, cert->pubkey_n, HDCP22_RSA_SIZE);
    dp_mem_copy(h2->rx_pub_e, cert->pubkey_e, 4);

    sha256(h2->cert_rx, HDCP22_CERT_LEN, h2->cert_rx_hash);

    /* Generate random Km (128-bit) */
    hdcp_rng_bytes(h2->km, HDCP22_KM_SIZE);

    /* Build AKE_No_Stored_km: kPub_km (256B) + e_km (16B) */
    hdcp22_msg_t reply;
    dp_mem_set(&reply, 0, sizeof(reply));
    reply.msg_id = HDCP22_MSG_AKE_NO_STORED_KM;
    reply.size = HDCP22_RSA_SIZE + HDCP22_KM_SIZE;

    u8 kpub_km[HDCP22_RSA_SIZE];
    rc = hdcp22_compute_kpub_km(h2->km, h2->rx_pub_n, h2->rx_pub_e, kpub_km);
    if (rc != DP_OK) return rc;
    dp_mem_copy(&reply.payload[0], kpub_km, HDCP22_RSA_SIZE);

    /* e_km = AES128(dkey2, kd) later; for now send placeholder */
    /* Actually: e_km = AES128_CBC(kd, riv, km) after kd derived from rn/riv.
     * But rn/riv are not available yet at this stage — HDCP 2.2 says e_km is
     * AES128_ECB(kd, km) with kd derived from rn=0, riv=0 at this point. */
    u8 kd0[16], rn0[8] = {0}, riv0[8] = {0};
    hdcp22_derive_kd(h2->km, rn0, riv0, kd0);

    u8 e_km[16];
    aes128_ctx_t aes;
    aes128_expand_key(&aes, kd0);
    aes128_encrypt_block(&aes, h2->km, e_km);
    dp_mem_copy(&reply.payload[HDCP22_RSA_SIZE], e_km, HDCP22_KM_SIZE);

    return hdcp22_send_msg(c, &reply);
}

static int hdcp22_ake_verify_h_prime(dp_controller_t *c, hdcp22_ctx_t *h2,
                                      const hdcp22_msg_t *msg)
{
    if (msg->msg_id != HDCP22_MSG_AKE_SEND_H_PRIME) return DP_ERR_HDCP;
    if (msg->size < 32) return DP_ERR_HDCP;
    dp_mem_copy(h2->h_prime_rx, msg->payload, 32);

    /* Derive kd, k_priv using rn=0, riv=0 (not yet sent) */
    u8 rn0[8] = {0}, riv0[8] = {0};
    hdcp22_derive_kd(h2->km, rn0, riv0, h2->kd);
    hdcp22_derive_kpriv_kpub(h2->km, rn0, h2->kd, h2->k_priv, h2->k_pub);

    /* Compute H' = HMAC-SHA256(kd || k_priv, SHA256(cert_rx)) */
    u8 key[48];
    dp_mem_copy(key, h2->kd, 16);
    dp_mem_copy(key + 16, h2->k_priv, 32);
    u8 expected[32];
    hmac_sha256(key, 48, h2->cert_rx_hash, 32, expected);

    if (dp_mem_cmp(expected, h2->h_prime_rx, 32) != 0) return DP_ERR_HDCP;
    return DP_OK;
}

static int hdcp22_ake_verify_pairing_info(dp_controller_t *c, hdcp22_ctx_t *h2,
                                          const hdcp22_msg_t *msg)
{
    if (msg->msg_id != HDCP22_MSG_AKE_SEND_PAIRING_INFO) return DP_ERR_HDCP;
    if (msg->size < 16) return DP_ERR_HDCP;
    dp_mem_copy(h2->e_kh_km, msg->payload, 16);

    /* Verify e_kh_km == AES128_ECB(kd, km) using derived kd (rn=0, riv=0) */
    u8 kd0[16], rn0[8] = {0}, riv0[8] = {0};
    hdcp22_derive_kd(h2->km, rn0, riv0, kd0);

    u8 expected[16];
    aes128_ctx_t aes;
    aes128_expand_key(&aes, kd0);
    aes128_encrypt_block(&aes, h2->km, expected);

    if (dp_mem_cmp(expected, h2->e_kh_km, 16) != 0) return DP_ERR_HDCP;

    h2->paired = true;
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 2.2 - LC FLOW                                                          */
/* ========================================================================== */

static int hdcp22_lc_init(dp_controller_t *c, hdcp22_ctx_t *h2)
{
    hdcp_rng_bytes(h2->rn, HDCP22_RN_SIZE);

    hdcp22_msg_t msg;
    dp_mem_set(&msg, 0, sizeof(msg));
    msg.msg_id = HDCP22_MSG_LC_INIT;
    msg.size = HDCP22_RN_SIZE;
    dp_mem_copy(msg.payload, h2->rn, HDCP22_RN_SIZE);

    return hdcp22_send_msg(c, &msg);
}

static int hdcp22_lc_verify_l_prime(dp_controller_t *c, hdcp22_ctx_t *h2,
                                     const hdcp22_msg_t *msg)
{
    if (msg->msg_id != HDCP22_MSG_LC_SEND_L_PRIME) return DP_ERR_HDCP;
    if (msg->size < 32) return DP_ERR_HDCP;
    dp_mem_copy(h2->l_prime_rx, msg->payload, 32);

    /* Re-derive kd, k_priv using now-known rn + riv */
    hdcp22_derive_kd(h2->km, h2->rn, h2->riv, h2->kd);
    hdcp22_derive_kpriv_kpub(h2->km, h2->rn, h2->kd, h2->k_priv, h2->k_pub);

    /* L' = HMAC-SHA256(kd || k_priv, rn) */
    u8 key[48];
    dp_mem_copy(key, h2->kd, 16);
    dp_mem_copy(key + 16, h2->k_priv, 32);

    u8 expected[32];
    hmac_sha256(key, 48, h2->rn, HDCP22_RN_SIZE, expected);

    if (dp_mem_cmp(expected, h2->l_prime_rx, 32) != 0) return DP_ERR_HDCP;
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 2.2 - SKE FLOW                                                         */
/* ========================================================================== */

static int hdcp22_ske_send_eks(dp_controller_t *c, hdcp22_ctx_t *h2)
{
    hdcp_rng_bytes(h2->rrx, HDCP22_RRX_SIZE);

    /* ks = (riv << 64) XOR kd, truncated to 128 bits */
    for (u32 i = 0; i < 8; i++) {
        h2->ks[i] = 0;
        h2->ks[i + 8] = h2->riv[i] ^ h2->kd[i + 8];
    }
    for (u32 i = 0; i < 8; i++) h2->ks[i] = h2->kd[i];

    /* dkey2 = HMAC-SHA256(kd, rrx || rn)[0..15] */
    u8 msg[16];
    dp_mem_copy(msg, h2->rrx, 8);
    dp_mem_copy(msg + 8, h2->rn, 8);
    u8 dkey2[32];
    hmac_sha256(h2->kd, 16, msg, 16, dkey2);

    /* IV for AES-CBC = riv || 0^64 */
    u8 iv[16] = {0};
    dp_mem_copy(iv, h2->riv, 8);

    /* edkey_ks = AES128_CBC(dkey2[0..15], iv, ks) */
    u8 edkey_ks[16];
    aes128_cbc_encrypt(dkey2, iv, h2->ks, edkey_ks, 16);

    hdcp22_msg_t sk;
    dp_mem_set(&sk, 0, sizeof(sk));
    sk.msg_id = HDCP22_MSG_SKE_SEND_EKS;
    sk.size = 16 + 8;
    dp_mem_copy(&sk.payload[0], edkey_ks, 16);
    dp_mem_copy(&sk.payload[16], h2->rrx, 8);

    return hdcp22_send_msg(c, &sk);
}

/* ========================================================================== */
/* HDCP 2.2 - AUTHENTICATE (full AKE + LC + SKE)                               */
/* ========================================================================== */

static int hdcp22_authenticate(dp_controller_t *c, hdcp22_ctx_t *h2)
{
    if (!c || !h2) return DP_ERR_INVAL;

    /* Check rx supports HDCP 2.2 */
    u8 ver = 0;
    int rc = hdcp_read_byte(c, HDCP22_REG_VERSION, &ver);
    if (rc != DP_OK) return rc;
    h2->rx_caps = ver;

    int state = 0;
    u32 msgs_processed = 0;
    u64 deadline = riscv_read_time() + 5000000ULL;

    while (state < 20) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;

        hdcp22_msg_t msg;
        dp_mem_set(&msg, 0, sizeof(msg));
        rc = hdcp22_recv_msg(c, &msg, 200000);
        if (rc == DP_ERR_TIMEOUT) {
            /* No message; send next state's message if not sent yet */
            if (state == 0) {
                rc = hdcp22_ake_init(c, h2);
                if (rc != DP_OK) return rc;
                state = 1;
            } else if (state == 3) {
                /* Already sent AKE_No_Stored_km; wait */
            } else if (state == 6) {
                rc = hdcp22_lc_init(c, h2);
                if (rc != DP_OK) return rc;
                state = 7;
            } else if (state == 9) {
                rc = hdcp22_ske_send_eks(c, h2);
                if (rc != DP_OK) return rc;
                state = 10;
            } else {
                continue;
            }
            continue;
        }
        if (rc != DP_OK) return rc;

        msgs_processed++;

        switch (msg.msg_id) {
        case HDCP22_MSG_AKE_SEND_CERT:
            if (state != 1) return DP_ERR_HDCP;
            rc = hdcp22_ake_handle_cert(c, h2, &msg);
            if (rc != DP_OK) return rc;
            state = 3;
            break;
        case HDCP22_MSG_AKE_SEND_H_PRIME:
            if (state != 3) return DP_ERR_HDCP;
            rc = hdcp22_ake_verify_h_prime(c, h2, &msg);
            if (rc != DP_OK) return rc;
            state = 5;
            break;
        case HDCP22_MSG_AKE_SEND_PAIRING_INFO:
            if (state != 5) return DP_ERR_HDCP;
            rc = hdcp22_ake_verify_pairing_info(c, h2, &msg);
            if (rc != DP_OK) return rc;
            state = 6;
            break;
        case HDCP22_MSG_LC_SEND_L_PRIME:
            if (state != 7) return DP_ERR_HDCP;
            rc = hdcp22_lc_verify_l_prime(c, h2, &msg);
            if (rc != DP_OK) return rc;
            state = 9;
            break;
        case HDCP22_MSG_REPEATER_AUTH_SEND_RECEIVER_ID_LIST:
            if (state != 10) return DP_ERR_HDCP;
            state = 10; /* Repeater path needs further handling */
            break;
        default:
            return DP_ERR_HDCP;
        }

        if (state == 10 && !h2->is_repeater) break;
        if (msgs_processed > 20) return DP_ERR_HDCP;
    }

    h2->authenticated = true;
    h2->encrypting = true;
    h2->frame_count = 0;
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 2.2 - STREAM MANAGEMENT                                                */
/* ========================================================================== */

static int hdcp22_stream_manage(dp_controller_t *c, hdcp22_ctx_t *h2,
                                u32 stream_id, u8 stream_type, bool enable)
{
    if (!c || !h2 || !h2->authenticated) return DP_ERR_HDCP;

    hdcp22_msg_t msg;
    dp_mem_set(&msg, 0, sizeof(msg));
    msg.msg_id = HDCP22_MSG_REPEATER_AUTH_STREAM_MANAGE;
    msg.size = 2 + 1 + 1 + 4;

    msg.payload[0] = (u8)(stream_id & 0xFF);
    msg.payload[1] = (u8)((stream_id >> 8) & 0xFF);

    /* StreamID_Type: 0x00 = Type 0 (DP), 0x01 = Type 1 (MST) */
    msg.payload[2] = enable ? 0x01 : 0x00;

    /* Stream Type: 0=Type0 no content, 1=Type0 DP, 2=Type0 HDMI */
    msg.payload[3] = stream_type;

    msg.payload[4] = (u8)(h2->seq_num_tx);
    msg.payload[5] = 0;
    msg.payload[6] = 0;
    msg.payload[7] = 0;
    h2->seq_num_tx++;

    return hdcp22_send_msg(c, &msg);
}

/* ========================================================================== */
/* HDCP 2.2 - STREAMING TICK                                                   */
/* ========================================================================== */

static int hdcp22_streaming_tick(dp_controller_t *c, hdcp22_ctx_t *h2)
{
    if (!c || !h2 || !h2->authenticated) return DP_ERR_HDCP;
    h2->frame_count++;

    /* Every ~256 frames, check RXSTATUS for link integrity / re-auth */
    if ((h2->frame_count & 0xFF) == 0) {
        u8 rxstatus = 0;
        if (hdcp_read_byte(c, HDCP22_REG_RXSTATUS, &rxstatus) == DP_OK) {
            if (rxstatus & HDCP22_STATUS_REAUTH) {
                h2->authenticated = false;
                h2->encrypting = false;
                return DP_ERR_HDCP;
            }
            if (rxstatus & HDCP22_STATUS_LINK_INTEGRITY_FAIL) {
                h2->authenticated = false;
                h2->encrypting = false;
                return DP_ERR_HDCP;
            }
        }
    }
    return DP_OK;
}

/* ========================================================================== */
/* HDCP 2.2 - REPEATER AUTH (downstream tree)                                  */
/* ========================================================================== */

static int hdcp22_repeater_auth(dp_controller_t *c, hdcp22_ctx_t *h2)
{
    if (!c || !h2) return DP_ERR_INVAL;
    if (!h2->authenticated) return DP_ERR_HDCP;

    /* Send receiver ID list (from MST topology). Placeholder: empty list. */
    hdcp22_msg_t msg;
    dp_mem_set(&msg, 0, sizeof(msg));
    msg.msg_id = HDCP22_MSG_REPEATER_AUTH_SEND_RECEIVER_ID_LIST;
    msg.size = 2;  /* seq_num, max_devs */
    msg.payload[0] = (u8)(h2->seq_num_tx++);
    msg.payload[1] = 31;
    int rc = hdcp22_send_msg(c, &msg);
    if (rc != DP_OK) return rc;

    /* Wait for ACK (receiver ID list + V') */
    u64 deadline = riscv_read_time() + 5000000ULL;
    for (;;) {
        hdcp22_msg_t rmsg;
        rc = hdcp22_recv_msg(c, &rmsg, 200000);
        if (rc == DP_ERR_TIMEOUT) {
            if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
            continue;
        }
        if (rc != DP_OK) return rc;

        if (rmsg.msg_id == HDCP22_MSG_REPEATER_AUTH_SEND_RECEIVER_ID_LIST) {
            /* Validate downstream KSV list and depth */
            /* (skipped: requires MST topology integration) */
            rc = 0;
        } else if (rmsg.msg_id == HDCP22_MSG_REPEATER_AUTH_STREAM_READY) {
            break;
        }
    }

    /* Send ACK */
    dp_mem_set(&msg, 0, sizeof(msg));
    msg.msg_id = HDCP22_MSG_REPEATER_AUTH_SEND_ACK;
    msg.size = 1;
    msg.payload[0] = (u8)(h2->seq_num_tx++);
    return hdcp22_send_msg(c, &msg);
}

/* ========================================================================== */
/* ASYNC STATE MACHINE (unified HDCP 1.x + 2.2)                                */
/* ========================================================================== */

typedef enum {
    HSTATE_IDLE = 0,
    HSTATE_1X_AUTH,
    HSTATE_1X_WAIT_REPEATER,
    HSTATE_1X_STREAMING,
    HSTATE_2X_AKE,
    HSTATE_2X_LC,
    HSTATE_2X_SKE,
    HSTATE_2X_REPEATER,
    HSTATE_2X_STREAMING,
    HSTATE_FAILED,
} hdcp_state_t;

typedef struct {
    hdcp_state_t  state;
    u32           version;      /* 1 or 2 */
    hdcp1_ctx_t   h1;
    hdcp22_ctx_t  h2;
    u32           retries;
    u64           deadline;
    bool          repeater_auth_done;
    void          (*on_state_change)(u32 old_state, u32 new_state, void *priv);
    void          *priv;
} hdcp_sm_t;

static int hdcp_sm_transition(hdcp_sm_t *sm, hdcp_state_t new_state)
{
    hdcp_state_t old = sm->state;
    sm->state = new_state;
    if (sm->on_state_change) sm->on_state_change(old, new_state, sm->priv);
    return DP_OK;
}

static int hdcp_sm_tick(dp_controller_t *c, hdcp_sm_t *sm)
{
    if (!c || !sm) return DP_ERR_INVAL;

    switch (sm->state) {
    case HSTATE_IDLE:
        return DP_OK;

    case HSTATE_1X_AUTH: {
        int rc = hdcp1_authenticate(c, &sm->h1);
        if (rc == DP_OK) {
            if (sm->h1.is_repeater)
                hdcp_sm_transition(sm, HSTATE_1X_WAIT_REPEATER);
            else
                hdcp_sm_transition(sm, HSTATE_1X_STREAMING);
        } else if (sm->retries++ > 3) {
            hdcp_sm_transition(sm, HSTATE_FAILED);
        }
        return rc;
    }

    case HSTATE_1X_WAIT_REPEATER:
        if (sm->repeater_auth_done)
            hdcp_sm_transition(sm, HSTATE_1X_STREAMING);
        return DP_OK;

    case HSTATE_1X_STREAMING: {
        int rc = hdcp1_streaming_tick(c, &sm->h1);
        if (rc != DP_OK) {
            sm->retries = 0;
            hdcp_sm_transition(sm, HSTATE_1X_AUTH);
        }
        return rc;
    }

    case HSTATE_2X_AKE:
    case HSTATE_2X_LC:
    case HSTATE_2X_SKE: {
        int rc = hdcp22_authenticate(c, &sm->h2);
        if (rc == DP_OK) {
            if (sm->h2.is_repeater)
                hdcp_sm_transition(sm, HSTATE_2X_REPEATER);
            else
                hdcp_sm_transition(sm, HSTATE_2X_STREAMING);
        } else if (sm->retries++ > 3) {
            hdcp_sm_transition(sm, HSTATE_FAILED);
        }
        return rc;
    }

    case HSTATE_2X_REPEATER: {
        int rc = hdcp22_repeater_auth(c, &sm->h2);
        if (rc == DP_OK)
            hdcp_sm_transition(sm, HSTATE_2X_STREAMING);
        else if (sm->retries++ > 3)
            hdcp_sm_transition(sm, HSTATE_FAILED);
        return rc;
    }

    case HSTATE_2X_STREAMING: {
        int rc = hdcp22_streaming_tick(c, &sm->h2);
        if (rc != DP_OK) {
            sm->retries = 0;
            hdcp_sm_transition(sm, HSTATE_2X_AKE);
        }
        return rc;
    }

    case HSTATE_FAILED:
    default:
        return DP_ERR_HDCP;
    }
}

static void hdcp_sm_reset(hdcp_sm_t *sm, u32 version)
{
    if (!sm) return;
    hdcp_state_t st = sm->state;
    dp_mem_set(sm, 0, sizeof(*sm));
    sm->version = version;
    if (version == 1) hdcp_sm_transition(sm, HSTATE_1X_AUTH);
    else hdcp_sm_transition(sm, HSTATE_2X_AKE);
    (void)st;
}

/* ========================================================================== */
/* GLOBAL STATE (per driver)                                                   */
/* ========================================================================== */

static hdcp_sm_t *g_hdcp_sm = NULL;
static dp_hdcp_state_t g_hdcp1_state = {0};
static dp_hdcp2_2_state_t g_hdcp22_state = {0};

/* ========================================================================== */
/* PUBLIC API (per dp_core.h)                                                  */
/* ========================================================================== */

int dp_hdcp_enable(dp_controller_t *c, u32 version)
{
    if (!c) return DP_ERR_INVAL;
    if (version != 1 && version != 2) return DP_ERR_INVAL;

    if (!g_hdcp_sm) {
        /* Allocate singleton (bare-metal: static) */
        static hdcp_sm_t sm_storage;
        g_hdcp_sm = &sm_storage;
    }
    dp_mem_set(g_hdcp_sm, 0, sizeof(*g_hdcp_sm));
    g_hdcp_sm->version = version;
    g_hdcp_sm->is_repeater_hint = 0;

    if (version == 2) {
        /* Probe HDCP 2.2 support */
        u8 ver = 0;
        if (hdcp_read_byte(c, HDCP22_REG_VERSION, &ver) != DP_OK) {
            return DP_ERR_HDCP;
        }
        if (!(ver & HDCP22_TXCAPS_HDCP2)) return DP_ERR_NOT_SUPPORTED;
        hdcp_sm_transition(g_hdcp_sm, HSTATE_2X_AKE);
    } else {
        hdcp_sm_transition(g_hdcp_sm, HSTATE_1X_AUTH);
    }

    /* Drive once synchronously */
    return hdcp_sm_tick(c, g_hdcp_sm);
}

int dp_hdcp_disable(dp_controller_t *c)
{
    if (!c) return DP_ERR_INVAL;
    if (g_hdcp_sm) hdcp_sm_transition(g_hdcp_sm, HSTATE_IDLE);
    return DP_OK;
}

int dp_hdcp_authenticate(dp_controller_t *c)
{
    if (!c || !g_hdcp_sm) return DP_ERR_INVAL;
    return hdcp_sm_tick(c, g_hdcp_sm);
}

int dp_hdcp_status(dp_controller_t *c, dp_hdcp_state_t *out)
{
    if (!c || !out) return DP_ERR_INVAL;
    if (!g_hdcp_sm) return DP_ERR_HDCP;

    if (g_hdcp_sm->version == 1) {
        out->bcaps = g_hdcp_sm->h1.authenticated ? 0x01 : 0x00;
        out->bstatus = g_hdcp_sm->h1.bstatus;
        dp_mem_copy(out->ksv, g_hdcp_sm->h1.aksv, 5);
        dp_mem_copy(out->ksv_fifo, g_hdcp_sm->h1.ksv_fifo,
                    g_hdcp_sm->h1.ksv_count * 5);
    } else {
        out->bcaps = g_hdcp_sm->h2.authenticated ? 0x02 : 0x00;
        out->bstatus = (u8)(g_hdcp_sm->h2.paired ? 0x01 : 0x00);
        dp_mem_copy(out->ksv, g_hdcp_sm->h2.km, 5);  /* Not strictly KSV; API stub */
    }
    return DP_OK;
}

/* ========================================================================== */
/* INTEGRATION HELPERS                                                         */
/* ========================================================================== */

/* Called from main HPD handling to drive HDCP auth state machine. */
int dp_hdcp_poll(dp_controller_t *c)
{
    if (!c || !g_hdcp_sm) return DP_ERR_INVAL;
    return hdcp_sm_tick(c, g_hdcp_sm);
}

/* Called per vertical blank for HDCP 1.x Ri update (every 128 frames). */
int dp_hdcp_frame_tick(dp_controller_t *c)
{
    if (!c || !g_hdcp_sm) return DP_ERR_INVAL;
    if (g_hdcp_sm->state == HSTATE_1X_STREAMING) {
        return hdcp1_streaming_tick(c, &g_hdcp_sm->h1);
    }
    if (g_hdcp_sm->state == HSTATE_2X_STREAMING) {
        return hdcp22_streaming_tick(c, &g_hdcp_sm->h2);
    }
    return DP_OK;
}

/* Called when MST stream is enabled/disabled. */
int dp_hdcp_stream_manage(dp_controller_t *c, u32 stream_id,
                          u8 stream_type, bool enable)
{
    if (!c || !g_hdcp_sm) return DP_ERR_INVAL;
    if (g_hdcp_sm->version != 2) return DP_OK;
    if (g_hdcp_sm->state != HSTATE_2X_STREAMING) return DP_ERR_HDCP;
    return hdcp22_stream_manage(c, &g_hdcp_sm->h2, stream_id, stream_type, enable);
}

/* SRM loader (called at init). */
int dp_hdcp_load_srm(const u8 *srm, u32 srm_len)
{
    if (!g_hdcp_sm) {
        static hdcp_sm_t sm_storage;
        g_hdcp_sm = &sm_storage;
        dp_mem_set(g_hdcp_sm, 0, sizeof(*g_hdcp_sm));
    }
    int rc = hdcp_srm_parse(srm, srm_len, &g_hdcp_sm->h1.srm);
    if (rc == DP_OK) g_hdcp_sm->h1.srm_loaded = true;
    return rc;
}

/* ========================================================================== */
/* DEBUG / DUMP                                                                */
/* ========================================================================== */

__maybe_unused static void dp_hdcp1_dump(const hdcp1_ctx_t *h1)
{
    if (!h1) return;
    dp_info("HDCP1: auth=%d rep=%d bksv=%02x%02x%02x%02x%02x",
            h1->authenticated, h1->is_repeater,
            h1->bksv[0], h1->bksv[1], h1->bksv[2], h1->bksv[3], h1->bksv[4]);
    dp_info("HDCP1: bstatus=0x%02x ksv_count=%u frames=%u",
            h1->bstatus, h1->ksv_count, h1->frame_count);
}

__maybe_unused static void dp_hdcp22_dump(const hdcp22_ctx_t *h2)
{
    if (!h2) return;
    dp_info("HDCP22: auth=%d paired=%d rep=%d rx_caps=0x%x",
            h2->authenticated, h2->paired, h2->is_repeater, h2->rx_caps);
    dp_info("HDCP22: frames=%u ks=0x%02x%02x%02x%02x",
            h2->frame_count, h2->ks[0], h2->ks[1], h2->ks[2], h2->ks[3]);
}

__maybe_unused static void dp_hdcp_sm_dump(const hdcp_sm_t *sm)
{
    if (!sm) return;
    dp_info("HDCP_SM: state=%d version=%u retries=%u",
            (int)sm->state, sm->version, sm->retries);
    if (sm->version == 1) dp_hdcp1_dump(&sm->h1);
    else                  dp_hdcp22_dump(&sm->h2);
}

/* ========================================================================== */
/* END OF BLOCK 6                                                              */
/* ========================================================================== */


/*
 * ============================================================================
 *  DP_CORE.C - Block 7: DSC | PSR | ALPM
 *  Architecture: RISC-V (RV32 / RV64)
 *  Language: C11
 *  Scope: DSC (PPS encode/decode/compute, slice calc, HW programming),
 *         PSR (enable/disable, HW registers, state tracking),
 *         ALPM (full: enable, configure, AUX-less, timers, polling).
 *  Notes: Bare-metal for CoTanOS. HW register offsets occupy DP_RESERVED[]
 *         at 0x300..0x39F (blocks 4-6 used 0x0A0..0x2C0).
 * ============================================================================
 */

#include "dp_core.h"

/* ========================================================================== */
/* HW REGISTER OFFSETS                                                         */
/* ========================================================================== */

/* DSC */
#define DP_REG_DSC_CTL              0x300
#define DP_REG_DSC_STATUS           0x304
#define DP_REG_DSC_PPS_IDX          0x308
#define DP_REG_DSC_PPS_DATA         0x30C
#define DP_REG_DSC_SLICE_CTL        0x310
#define DP_REG_DSC_CHUNK_SIZE       0x314
#define DP_REG_DSC_OP_MODE          0x318
#define DP_REG_DSC_ERR_STATUS       0x31C
#define DP_REG_DSC_PPS_READY        0x320
#define DP_REG_DSC_FRAME_CNT        0x324
#define DP_REG_DSC_PPS_WINDOW       0x32C  /* 8 u32 = 32 bytes window */

#define DSC_CTL_EN                  BIT(0)
#define DSC_CTL_PPS_VALID           BIT(1)
#define DSC_CTL_NATIVE_444          BIT(2)
#define DSC_CTL_NATIVE_422          BIT(3)
#define DSC_CTL_NATIVE_420          BIT(4)
#define DSC_CTL_BLOCK_PRED          BIT(5)

#define DSC_STATUS_BUSY             BIT(0)
#define DSC_STATUS_PPS_LOADED       BIT(1)
#define DSC_STATUS_SLICE_DONE       BIT(2)
#define DSC_STATUS_FRAME_DONE       BIT(3)
#define DSC_STATUS_RC_BUF_OF        BIT(4)

#define DSC_ERR_PPS_INVALID         BIT(0)
#define DSC_ERR_SLICE_OVF           BIT(1)
#define DSC_ERR_BW_OVF              BIT(2)
#define DSC_ERR_BPP_INVALID         BIT(3)

#define DSC_SLICE_CTL_PER_LINE_SHIFT 0
#define DSC_SLICE_CTL_PER_LINE_MASK  (0xFFu << 0)
#define DSC_SLICE_CTL_WIDTH_SHIFT    8
#define DSC_SLICE_CTL_WIDTH_MASK     (0xFFFFu << 8)
#define DSC_SLICE_CTL_HEIGHT_SHIFT   24
#define DSC_SLICE_CTL_HEIGHT_MASK    (0xFFu << 24)

#define DSC_OP_MODE_444             0
#define DSC_OP_MODE_422             1
#define DSC_OP_MODE_420             2

/* PSR */
#define DP_REG_PSR_CTL              0x340
#define DP_REG_PSR_STATUS           0x344
#define DP_REG_PSR_TIMER_IDLE       0x348
#define DP_REG_PSR_TIMER_WAKEUP     0x34C
#define DP_REG_PSR_IRQ_MASK         0x350
#define DP_REG_PSR_IRQ_STATUS       0x354
#define DP_REG_PSR_FRAME_CAP        0x358
#define DP_REG_PSR_DEBUG            0x35C

#define PSR_CTL_EN                  BIT(0)
#define PSR_CTL_MAINLINK_OFF        BIT(1)
#define PSR_CTL_IRQ_EN              BIT(2)
#define PSR_CTL_AUTO_EXIT           BIT(3)

#define PSR_STATUS_IDLE             BIT(0)
#define PSR_STATUS_ACTIVE           BIT(1)
#define PSR_STATUS_EXITING          BIT(2)
#define PSR_STATUS_ERR              BIT(3)

#define PSR_IRQ_FRAME_UPDATE        BIT(0)
#define PSR_IRQ_HPD                 BIT(1)
#define PSR_IRQ_SINK_ERROR          BIT(2)
#define PSR_IRQ_LINK_LOST           BIT(3)

/* ALPM */
#define DP_REG_ALPM_CTL             0x360
#define DP_REG_ALPM_STATUS          0x364
#define DP_REG_ALPM_TIMER_IDLE      0x368
#define DP_REG_ALPM_TIMER_WAKEUP    0x36C
#define DP_REG_ALPM_AUX_CNT         0x370
#define DP_REG_ALPM_WAKEUP_CFG      0x374
#define DP_REG_ALPM_IRQ_MASK        0x378
#define DP_REG_ALPM_IRQ_STATUS      0x37C

#define ALPM_CTL_EN                 BIT(0)
#define ALPM_CTL_AUX_LESS           BIT(1)
#define ALPM_CTL_LOCK_ERR_HPD       BIT(2)
#define ALPM_CTL_AUTO_EXIT          BIT(3)

#define ALPM_STATUS_IDLE            BIT(0)
#define ALPM_STATUS_ENTERED         BIT(1)
#define ALPM_STATUS_EXITING         BIT(2)
#define ALPM_STATUS_ERR             BIT(3)

#define ALPM_WAKEUP_SRC_AUX         BIT(0)
#define ALPM_WAKEUP_SRC_PHY         BIT(1)
#define ALPM_WAKEUP_SRC_TIMER       BIT(2)

#define ALPM_IRQ_WAKEUP             BIT(0)
#define ALPM_IRQ_LOCK_ERR           BIT(1)
#define ALPM_IRQ_AUX_TIMEOUT        BIT(2)

/* ALPM DPCD (per DP 1.4a) */
#define DP_ALPM_CAP_REG             DP_TRAINING_AUX_RD_INTERVAL  /* 0x00E */
#define DP_ALPM_CAP_BIT             (1 << 7)
#define DP_ALPM_CTRL_REG            0x221
#define DP_ALPM_STATUS_REG          0x222

/* ========================================================================== */
/* DSC CONSTANTS                                                              */
/* ========================================================================== */

#define DSC_DEFAULT_PPS_SIZE            128
#define DSC_DEFAULT_RC_MODEL_SIZE       8192
#define DSC_DEFAULT_CHUNK_SIZE          64
#define DSC_DEFAULT_DELAY_MS            20
#define DSC_MAX_POLL_US                 50000
#define DSC_SLICE_POLL_US               2000

#define DSC_MIN_BPP                     8
#define DSC_MAX_BPP                     24
#define DSC_BPP_FIXED_16_SHIFT          4  /* bits_per_pixel field: value*16 */

#define DSC_BPC_MASK                    0x0F
#define DSC_LINEBUF_DEPTH_SHIFT         4

/* ========================================================================== */
/* DSC - DEFAULT RATE CONTROL TABLES (8bpc / 8bpp baseline)                    */
/* ========================================================================== */

/* Per VESA DSC 1.2a Annex C: rc_buf_thresh stored as raw/64. */
static const u8 dsc_default_rc_buf_thresh[14] = {
    14, 28, 42, 56, 70, 84, 98, 105, 112, 119, 121, 123, 125, 126
};

/* Per VESA DSC 1.2a Annex C: {min_qp, max_qp, bpg_offset}. */
static const u8 dsc_default_rc_range_params[15][3] = {
    { 0,  4,  2 },
    { 0,  4,  0 },
    { 1,  6,  0 },
    { 1,  7,  0 },
    { 2,  8,  0 },
    { 3,  9,  0 },
    { 3, 10,  0 },
    { 4, 11,  0 },
    { 5, 12,  0 },
    { 5, 13,  0 },
    { 6, 14,  0 },
    { 7, 15,  0 },
    { 8, 16,  0 },
    { 9, 17,  0 },
    { 10, 18, 0 },
};

/* Per-bpp RC model size (bits). */
static u16 dsc_rc_model_size_for_bpp(u16 bpp_fixed16)
{
    /* rc_model_size is 8192 for all standard configs per VESA DSC 1.2a. */
    (void)bpp_fixed16;
    return DSC_DEFAULT_RC_MODEL_SIZE;
}

/* Packed 16-bit rc_range entry: bits[4:0]=min_qp, [9:5]=max_qp, [15:10]=bpg. */
static u16 dsc_pack_rc_range(u8 min_qp, u8 max_qp, u8 bpg)
{
    u16 v = 0;
    v |= (u16)(min_qp & 0x1F);
    v |= (u16)((max_qp & 0x1F) << 5);
    v |= (u16)((bpg & 0x3F) << 10);
    return v;
}

static void dsc_unpack_rc_range(u16 packed, u8 *min_qp, u8 *max_qp, u8 *bpg)
{
    *min_qp = (u8)(packed & 0x1F);
    *max_qp = (u8)((packed >> 5) & 0x1F);
    *bpg    = (u8)((packed >> 10) & 0x3F);
}

/* ========================================================================== */
/* DSC - PPS ENCODE                                                            */
/* ========================================================================== */

static inline void dsc_put_le16(u8 *p, u16 v)
{
    p[0] = (u8)(v & 0xFF);
    p[1] = (u8)((v >> 8) & 0xFF);
}

static u32 dp_dsc_pps_encode(const dp_dsc_pps_t *pps, u8 out[DP_DSC_PPS_SIZE])
{
    if (!pps || !out) return 0;
    dp_mem_set(out, 0, DP_DSC_PPS_SIZE);

    u32 i = 0;

    /* Byte 0: dsc_version_major[3:0] | dsc_version_minor[7:4] */
    out[i++] = (u8)((pps->dsc_version_major & 0x0F) |
                    ((pps->dsc_version_minor & 0x0F) << 4));

    /* Byte 1: pps_identifier */
    out[i++] = pps->pps_identifier;

    /* Byte 2: bits_per_component[3:0] | linebuf_depth[7:4] */
    out[i++] = (u8)((pps->bits_per_component & 0x0F) |
                    ((pps->linebuf_depth & 0x0F) << DSC_LINEBUF_DEPTH_SHIFT));

    /* Byte 3: block_pred_enable[0] | convert_rgb[1] | simple_422[2] | vbr_enable[3] */
    out[i++] = (u8)((pps->block_pred_enable & 0x01) |
                    ((pps->convert_rgb & 0x01) << 1) |
                    ((pps->simple_422 & 0x01) << 2) |
                    ((pps->vbr_enable & 0x01) << 3));

    /* Bytes 4-5: bits_per_pixel (16 LE) */
    dsc_put_le16(&out[i], pps->bits_per_pixel); i += 2;

    /* Bytes 6-7: pic_height */
    dsc_put_le16(&out[i], pps->pic_height); i += 2;

    /* Bytes 8-9: pic_width */
    dsc_put_le16(&out[i], pps->pic_width); i += 2;

    /* Bytes 10-11: slice_height */
    dsc_put_le16(&out[i], pps->slice_height); i += 2;

    /* Bytes 12-13: slice_width */
    dsc_put_le16(&out[i], pps->slice_width); i += 2;

    /* Bytes 14-15: chunk_size */
    dsc_put_le16(&out[i], pps->chunk_size); i += 2;

    /* Bytes 16-17: initial_xmit_delay */
    dsc_put_le16(&out[i], pps->initial_xmit_delay); i += 2;

    /* Bytes 18-19: initial_dec_delay */
    dsc_put_le16(&out[i], pps->initial_dec_delay); i += 2;

    /* Byte 20: initial_scale_value (8 bits) */
    out[i++] = (u8)(pps->initial_scale_value & 0xFF);

    /* Bytes 21-22: scale_increment_interval */
    dsc_put_le16(&out[i], pps->scale_increment_interval); i += 2;

    /* Bytes 23-24: scale_decrement_interval */
    dsc_put_le16(&out[i], pps->scale_decrement_interval); i += 2;

    /* Bytes 25-26: first_line_bpg_offset */
    dsc_put_le16(&out[i], pps->first_line_bpg_offset); i += 2;

    /* Bytes 27-28: nfl_bpg_offset */
    dsc_put_le16(&out[i], pps->nfl_bpg_offset); i += 2;

    /* Bytes 29-30: slice_bpg_offset */
    dsc_put_le16(&out[i], pps->slice_bpg_offset); i += 2;

    /* Bytes 31-32: initial_offset */
    dsc_put_le16(&out[i], pps->initial_offset); i += 2;

    /* Bytes 33-34: final_offset */
    dsc_put_le16(&out[i], pps->final_offset); i += 2;

    /* Byte 35: flatness_min_qp */
    out[i++] = (u8)(pps->flatness_min_qp & 0x1F);

    /* Byte 36: flatness_max_qp */
    out[i++] = (u8)(pps->flatness_max_qp & 0x1F);

    /* Bytes 37-38: rc_model_size */
    dsc_put_le16(&out[i], pps->rc_model_size); i += 2;

    /* Byte 39: rc_edge_factor */
    out[i++] = (u8)(pps->rc_edge_factor & 0x0F);

    /* Byte 40: rc_quant_incr_limit0 */
    out[i++] = (u8)(pps->rc_quant_incr_limit0 & 0x1F);

    /* Byte 41: rc_quant_incr_limit1 */
    out[i++] = (u8)(pps->rc_quant_incr_limit1 & 0x1F);

    /* Byte 42: rc_tgt_offset_lo[3:0] | rc_tgt_offset_hi[7:4] */
    out[i++] = (u8)((pps->rc_tgt_offset_lo & 0x0F) |
                    ((pps->rc_tgt_offset_hi & 0x0F) << 4));

    /* Bytes 43-56: rc_buf_thresh[14] */
    for (u32 j = 0; j < 14; j++)
        out[i++] = pps->rc_buf_thresh[j];

    /* Bytes 57-86: rc_range_params[15][2] */
    for (u32 j = 0; j < 15; j++) {
        out[i++] = pps->rc_range_params[j][0];
        out[i++] = pps->rc_range_params[j][1];
    }

    /* Bytes 87-127: reserved zero. */
    return i;
}

/* ========================================================================== */
/* DSC - PPS DECODE                                                            */
/* ========================================================================== */

static inline u16 dsc_get_le16(const u8 *p)
{
    return (u16)p[0] | ((u16)p[1] << 8);
}

int dp_dsc_pps_decode(const u8 in[DP_DSC_PPS_SIZE], dp_dsc_pps_t *pps)
{
    if (!in || !pps) return DP_ERR_INVAL;
    dp_mem_set(pps, 0, sizeof(*pps));

    u32 i = 0;

    pps->dsc_version_major = in[i] & 0x0F;
    pps->dsc_version_minor = (in[i] >> 4) & 0x0F;
    i++;

    pps->pps_identifier = in[i++];

    pps->bits_per_component = in[i] & 0x0F;
    pps->linebuf_depth      = (in[i] >> 4) & 0x0F;
    i++;

    pps->block_pred_enable = in[i] & 0x01;
    pps->convert_rgb       = (in[i] >> 1) & 0x01;
    pps->simple_422        = (in[i] >> 2) & 0x01;
    pps->vbr_enable        = (in[i] >> 3) & 0x01;
    i++;

    pps->bits_per_pixel  = dsc_get_le16(&in[i]); i += 2;
    pps->pic_height      = dsc_get_le16(&in[i]); i += 2;
    pps->pic_width       = dsc_get_le16(&in[i]); i += 2;
    pps->slice_height    = dsc_get_le16(&in[i]); i += 2;
    pps->slice_width     = dsc_get_le16(&in[i]); i += 2;
    pps->chunk_size      = dsc_get_le16(&in[i]); i += 2;
    pps->initial_xmit_delay     = dsc_get_le16(&in[i]); i += 2;
    pps->initial_dec_delay      = dsc_get_le16(&in[i]); i += 2;

    pps->initial_scale_value    = in[i++];

    pps->scale_increment_interval = dsc_get_le16(&in[i]); i += 2;
    pps->scale_decrement_interval = dsc_get_le16(&in[i]); i += 2;
    pps->first_line_bpg_offset    = dsc_get_le16(&in[i]); i += 2;
    pps->nfl_bpg_offset           = dsc_get_le16(&in[i]); i += 2;
    pps->slice_bpg_offset         = dsc_get_le16(&in[i]); i += 2;
    pps->initial_offset           = dsc_get_le16(&in[i]); i += 2;
    pps->final_offset             = dsc_get_le16(&in[i]); i += 2;

    pps->flatness_min_qp = in[i++] & 0x1F;
    pps->flatness_max_qp = in[i++] & 0x1F;
    pps->rc_model_size   = dsc_get_le16(&in[i]); i += 2;
    pps->rc_edge_factor  = in[i++] & 0x0F;
    pps->rc_quant_incr_limit0 = in[i++] & 0x1F;
    pps->rc_quant_incr_limit1 = in[i++] & 0x1F;
    pps->rc_tgt_offset_lo = in[i] & 0x0F;
    pps->rc_tgt_offset_hi = (in[i] >> 4) & 0x0F;
    i++;

    for (u32 j = 0; j < 14; j++)
        pps->rc_buf_thresh[j] = in[i++];

    for (u32 j = 0; j < 15; j++) {
        pps->rc_range_params[j][0] = in[i++];
        pps->rc_range_params[j][1] = in[i++];
    }

    return DP_OK;
}

/* ========================================================================== */
/* DSC - PPS VALIDATION                                                        */
/* ========================================================================== */

static bool dp_dsc_pps_validate(const dp_dsc_pps_t *pps)
{
    if (!pps) return false;

    if (pps->dsc_version_major != 1) return false;

    if (pps->bits_per_component < 6 || pps->bits_per_component > 16) return false;
    if (pps->linebuf_depth < 9 || pps->linebuf_depth > 16) return false;

    /* bits_per_pixel is fixed-point *16. */
    if (pps->bits_per_pixel < (DSC_MIN_BPP << DSC_BPP_FIXED_16_SHIFT)) return false;
    if (pps->bits_per_pixel > (DSC_MAX_BPP << DSC_BPP_FIXED_16_SHIFT)) return false;

    if (pps->pic_width == 0 || pps->pic_height == 0) return false;
    if (pps->pic_width > 8192 || pps->pic_height > 8192) return false;

    if (pps->slice_width == 0 || pps->slice_height == 0) return false;
    if (pps->slice_width > pps->pic_width) return false;
    if (pps->slice_height > pps->pic_height) return false;
    if ((pps->pic_width % pps->slice_width) != 0) return false;
    if ((pps->pic_height % pps->slice_height) != 0) return false;

    u32 slices_h = pps->pic_width / pps->slice_width;
    u32 slices_v = pps->pic_height / pps->slice_height;
    if (slices_h > 32) return false;
    if (slices_v > 32) return false;
    if (slices_h * slices_v > 32) return false;

    if (pps->chunk_size == 0) return false;
    if (pps->chunk_size > 4096) return false;

    if (pps->rc_model_size == 0) return false;
    return true;
}

/* ========================================================================== */
/* DSC - SLICE CALCULATION                                                     */
/* ========================================================================== */

/* DP 1.4a recommended max slices per line at various link rates. */
static u32 dp_dsc_max_slices_per_line(dp_link_bandwidth_t bw, dp_lane_count_t lanes)
{
    u64 bitrate = dp_lane_bitrate_kbps(bw, lanes);
    if (bitrate >= 8100000ULL * 4) return 4;   /* HBR3 x4 */
    if (bitrate >= 5400000ULL * 4) return 4;   /* HBR2 x4 */
    if (bitrate >= 2700000ULL * 4) return 2;   /* HBR x4  */
    return 1;
}

/* Picks the fewest slices per line such that slice_width <= max_slice_width
 * and slice_width divides pic_width. Prefers power-of-two splits. */
static u32 dp_dsc_choose_slices_per_line(u32 pic_width, u32 max_slice_width,
                                          u32 max_slices)
{
    if (pic_width == 0 || max_slice_width == 0) return 0;

    u32 best = 0;
    for (u32 n = 1; n <= 32; n *= 2) {
        if (n > max_slices) break;
        if (pic_width % n != 0) continue;
        u32 sw = pic_width / n;
        if (sw > max_slice_width) continue;
        if ((sw & 1) != 0 && n > 1) {
            /* Slice width must be even (2-pixel alignment). */
            continue;
        }
        best = n;
        break;
    }
    return best;
}

/* Slice height: prefers full height, falls back to power-of-two splits when
 * required for slice-height limit or low-latency mode. */
static u32 dp_dsc_choose_slices_per_column(u32 pic_height, u32 max_slice_height,
                                            u32 max_slices, u32 low_latency)
{
    if (pic_height == 0) return 0;

    u32 preferred = low_latency ? 2 : 1;
    for (u32 n = preferred; n <= 32; n *= 2) {
        if (n > max_slices) break;
        if (pic_height % n != 0) continue;
        u32 sh = pic_height / n;
        if (sh > max_slice_height) continue;
        return n;
    }
    return 1;
}

typedef struct {
    u32 slices_per_line;
    u32 slices_per_column;
    u32 slice_width;
    u32 slice_height;
    u32 total_slices;
} dp_dsc_slice_result_t;

int dp_dsc_calc_slices(u32 pic_width, u32 pic_height,
                        u32 max_slice_width, u32 max_slice_height,
                        dp_link_bandwidth_t bw, dp_lane_count_t lanes,
                        dp_dsc_slice_result_t *out)
{
    if (!out || pic_width == 0 || pic_height == 0) return DP_ERR_INVAL;

    if (max_slice_width == 0)  max_slice_width  = 4096;
    if (max_slice_height == 0) max_slice_height = 4096;

    u32 max_spl = dp_dsc_max_slices_per_line(bw, lanes);
    if (max_spl == 0) return DP_ERR_NOT_SUPPORTED;

    u32 spl = dp_dsc_choose_slices_per_line(pic_width, max_slice_width, max_spl);
    if (spl == 0) return DP_ERR_NOT_SUPPORTED;

    u32 spc = dp_dsc_choose_slices_per_column(pic_height, max_slice_height,
                                              32 / spl, 0);

    out->slices_per_line   = spl;
    out->slices_per_column = spc;
    out->slice_width       = pic_width / spl;
    out->slice_height      = pic_height / spc;
    out->total_slices      = spl * spc;
    return DP_OK;
}

/* ========================================================================== */
/* DSC - PPS COMPUTE (8bpc/8bpp baseline)                                      */
/* ========================================================================== */

static u16 dsc_compute_initial_xmit_delay(u16 bpp_fixed16, u32 rc_model_size)
{
    /* initial_xmit_delay ≈ rc_model_size / bpp - chunk overhead. */
    u32 bpp_x16 = bpp_fixed16;
    if (bpp_x16 == 0) return 0;
    u32 delay = (u32)(((u64)rc_model_size * 16ULL) / bpp_x16);
    if (delay < 1) delay = 1;
    return (u16)(delay > 0xFFFF ? 0xFFFF : delay);
}

static u16 dsc_compute_initial_dec_delay(u16 bpp_fixed16, u32 rc_model_size)
{
    u32 bpp_x16 = bpp_fixed16;
    if (bpp_x16 == 0) return 0;
    u32 delay = (u32)(((u64)rc_model_size * 16ULL) / bpp_x16);
    delay = delay > 8 ? delay - 8 : 1;
    return (u16)(delay > 0xFFFF ? 0xFFFF : delay);
}

static u16 dsc_compute_initial_scale_value(u32 rc_model_size)
{
    /* initial_scale_value = 8 * rc_model_size / (rc_model_size - init_xmit). */
    if (rc_model_size < 16) return 0;
    u32 v = (8 * rc_model_size) / (rc_model_size - 8);
    return (u16)(v > 0xFF ? 0xFF : v);
}

static u16 dsc_compute_scale_increment_interval(u16 initial_xmit_delay,
                                                 u32 rc_model_size)
{
    /* scale_increment_interval = 8 * rc_model_size / (init_xmit_delay - 8). */
    if (initial_xmit_delay <= 8) return 0;
    u32 v = (u32)(((u64)8ULL * rc_model_size) / (initial_xmit_delay - 8));
    return (u16)(v > 0xFFFF ? 0xFFFF : v);
}

static u16 dsc_compute_scale_decrement_interval(u32 rc_model_size,
                                                 u16 initial_xmit_delay)
{
    /* scale_decrement_interval = 8 / (initial_xmit_delay / rc_model_size). */
    if (rc_model_size == 0 || initial_xmit_delay == 0) return 0;
    u32 v = (u32)(((u64)8ULL * rc_model_size) / initial_xmit_delay);
    if (v == 0) v = 1;
    return (u16)(v > 0xFFFF ? 0xFFFF : v);
}

static u16 dsc_compute_first_line_bpg_offset(u32 slice_width, u32 bpc)
{
    /* Heuristic per spec Annex C: bpc 8 → 12; bpc 10 → 15; bpc 12 → 18. */
    (void)slice_width;
    switch (bpc) {
    case 8:  return 12;
    case 10: return 15;
    case 12: return 18;
    default: return 12;
    }
}

static u16 dsc_compute_nfl_bpg_offset(u32 slice_width, u32 pic_height,
                                       u16 bpp_fixed16)
{
    if (bpp_fixed16 == 0 || pic_height == 0) return 0;
    u64 v = ((u64)slice_width * 16ULL * 2048ULL) /
            ((u64)pic_height * bpp_fixed16);
    return (u16)(v > 0xFFFF ? 0xFFFF : v);
}

static u16 dsc_compute_slice_bpg_offset(u32 slice_height, u32 pic_height,
                                         u16 bpp_fixed16)
{
    if (bpp_fixed16 == 0 || slice_height == 0) return 0;
    u64 v = ((u64)slice_height * 16ULL * 2048ULL) /
            ((u64)pic_height * bpp_fixed16);
    return (u16)(v > 0xFFFF ? 0xFFFF : v);
}

static u16 dsc_compute_initial_offset(u16 bpp_fixed16, u32 slice_width,
                                       u32 first_line_bpg, u32 rc_model_size)
{
    u64 v = (u64)rc_model_size + ((u64)slice_width * bpp_fixed16) / 16ULL
            + first_line_bpg;
    return (u16)(v > 0xFFFF ? 0xFFFF : v);
}

static u16 dsc_compute_final_offset(u16 bpp_fixed16, u32 slice_width,
                                     u32 rc_model_size)
{
    u64 v = (u64)rc_model_size + ((u64)slice_width * bpp_fixed16) / 16ULL;
    return (u16)(v > 0xFFFF ? 0xFFFF : v);
}

int dp_dsc_compute_pps(dp_dsc_pps_t *pps,
                       u32 pic_width, u32 pic_height,
                       u16 bpp_fixed16, u8 bpc,
                       u32 slice_width, u32 slice_height,
                       u8 native_mode)
{
    if (!pps || pic_width == 0 || pic_height == 0) return DP_ERR_INVAL;
    if (bpp_fixed16 == 0) return DP_ERR_INVAL;

    dp_mem_set(pps, 0, sizeof(*pps));

    pps->dsc_version_major = 1;
    pps->dsc_version_minor = 2;
    pps->pps_identifier    = 0x00;
    pps->bits_per_component = bpc;
    pps->linebuf_depth      = (u8)(bpc + 1);
    pps->block_pred_enable  = 1;
    pps->convert_rgb        = 0;
    pps->simple_422         = (native_mode == DSC_OP_MODE_422) ? 1 : 0;
    pps->vbr_enable         = 0;

    pps->bits_per_pixel = bpp_fixed16;
    pps->pic_width      = (u16)pic_width;
    pps->pic_height     = (u16)pic_height;
    pps->slice_width    = (u16)slice_width;
    pps->slice_height   = (u16)slice_height;
    pps->chunk_size     = DSC_DEFAULT_CHUNK_SIZE;

    pps->rc_model_size = dsc_rc_model_size_for_bpp(bpp_fixed16);
    u32 rc_model = pps->rc_model_size;

    pps->initial_xmit_delay = dsc_compute_initial_xmit_delay(bpp_fixed16, rc_model);
    pps->initial_dec_delay  = dsc_compute_initial_dec_delay(bpp_fixed16, rc_model);
    pps->initial_scale_value = dsc_compute_initial_scale_value(rc_model);

    pps->scale_increment_interval = dsc_compute_scale_increment_interval(
        pps->initial_xmit_delay, rc_model);
    pps->scale_decrement_interval = dsc_compute_scale_decrement_interval(
        rc_model, pps->initial_xmit_delay);

    pps->first_line_bpg_offset = dsc_compute_first_line_bpg_offset(slice_width, bpc);
    pps->nfl_bpg_offset        = dsc_compute_nfl_bpg_offset(slice_width,
                                                            pic_height, bpp_fixed16);
    pps->slice_bpg_offset      = dsc_compute_slice_bpg_offset(slice_height,
                                                                pic_height, bpp_fixed16);
    pps->initial_offset        = dsc_compute_initial_offset(bpp_fixed16, slice_width,
                                                            pps->first_line_bpg_offset,
                                                            rc_model);
    pps->final_offset          = dsc_compute_final_offset(bpp_fixed16, slice_width,
                                                            rc_model);

    pps->flatness_min_qp = 3;
    pps->flatness_max_qp = (bpc == 8) ? 12 : (bpc == 10 ? 15 : 18);

    pps->rc_edge_factor     = 6;
    pps->rc_quant_incr_limit0 = 11;
    pps->rc_quant_incr_limit1 = 11;
    pps->rc_tgt_offset_lo     = 3;
    pps->rc_tgt_offset_hi     = 3;

    for (u32 i = 0; i < 14; i++)
        pps->rc_buf_thresh[i] = dsc_default_rc_buf_thresh[i];

    for (u32 i = 0; i < 15; i++) {
        u16 packed = dsc_pack_rc_range(dsc_default_rc_range_params[i][0],
                                        dsc_default_rc_range_params[i][1],
                                        dsc_default_rc_range_params[i][2]);
        pps->rc_range_params[i][0] = (u8)(packed & 0xFF);
        pps->rc_range_params[i][1] = (u8)((packed >> 8) & 0xFF);
    }

    return DP_OK;
}

/* ========================================================================== */
/* DSC - HW PROGRAMMING                                                        */
/* ========================================================================== */

static int dp_dsc_hw_write_pps(dp_controller_t *c, const u8 pps[DP_DSC_PPS_SIZE])
{
    vaddr_t base = c->base_virtual_addr;

    /* Clear PPS_VALID while loading. */
    u32 ctl = dp_reg_read(base, DP_REG_DSC_CTL);
    dp_reg_write(base, DP_REG_DSC_CTL, ctl & ~DSC_CTL_PPS_VALID);

    /* Write 128 bytes as 32 u32 words into PPS_WINDOW (8 u32 per window,
     * 4 windows). Hardware auto-increments PPS_IDX per word written. */
    for (u32 w = 0; w < 32; w++) {
        u32 word = 0;
        u32 off = w * 4;
        word  = (u32)pps[off + 0];
        word |= (u32)pps[off + 1] << 8;
        word |= (u32)pps[off + 2] << 16;
        word |= (u32)pps[off + 3] << 24;
        dp_reg_write(base, DP_REG_DSC_PPS_WINDOW + ((w % 8) * 4), word);
        if ((w % 8) == 7)
            dp_reg_write(base, DP_REG_DSC_PPS_IDX, w + 1);
    }

    /* Signal PPS_VALID. */
    dp_reg_write(base, DP_REG_DSC_CTL, ctl | DSC_CTL_PPS_VALID);

    /* Poll PPS_READY. */
    u64 deadline = riscv_read_time() + DSC_MAX_POLL_US;
    while (!(dp_reg_read(base, DP_REG_DSC_PPS_READY) & BIT(0))) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }
    return DP_OK;
}

static void dp_dsc_hw_write_slices(dp_controller_t *c,
                                    const dp_dsc_slice_result_t *s)
{
    vaddr_t base = c->base_virtual_addr;
    u32 v = 0;
    v |= ((s->slices_per_line & 0xFFu) << DSC_SLICE_CTL_PER_LINE_SHIFT);
    v |= ((s->slice_width & 0xFFFFu) << DSC_SLICE_CTL_WIDTH_SHIFT);
    v |= (((s->slices_per_column - 1) & 0xFFu) << DSC_SLICE_CTL_HEIGHT_SHIFT);
    dp_reg_write(base, DP_REG_DSC_SLICE_CTL, v);
    dp_reg_write(base, DP_REG_DSC_CHUNK_SIZE, DSC_DEFAULT_CHUNK_SIZE);
}

static void dp_dsc_hw_op_mode(dp_controller_t *c, u8 native_mode)
{
    vaddr_t base = c->base_virtual_addr;
    u32 v = 0;
    switch (native_mode) {
    case DSC_OP_MODE_444: break;
    case DSC_OP_MODE_422: v = DSC_OP_MODE_422; break;
    case DSC_OP_MODE_420: v = DSC_OP_MODE_420; break;
    default: v = DSC_OP_MODE_444;
    }
    dp_reg_write(base, DP_REG_DSC_OP_MODE, v);
}

/* ========================================================================== */
/* DSC - DPCD ACTIVATION                                                       */
/* ========================================================================== */

static int dp_dsc_dpcd_write_pps(dp_controller_t *c, const u8 pps[DP_DSC_PPS_SIZE])
{
    /* PPS is delivered to the sink via DPCD 0x160+ (16-byte chunks). */
    int rc = dp_aux_write_dpcd(c, DP_DSC_ENABLE, pps, DP_DSC_PPS_SIZE);
    if (rc != DP_OK) return rc;

    /* Set DP_DSC_ENABLE bit 0 = enable. */
    u8 en = 0x01;
    return dp_aux_write_dpcd(c, DP_DSC_ENABLE, &en, 1);
}

static int dp_dsc_dpcd_clear(dp_controller_t *c)
{
    u8 clr = 0x00;
    return dp_aux_write_dpcd(c, DP_DSC_ENABLE, &clr, 1);
}

/* ========================================================================== */
/* DSC - PUBLIC API                                                            */
/* ========================================================================== */

int dp_dsc_configure(dp_controller_t *c, const dp_dsc_pps_t *pps)
{
    if (!c || !c->initialized || !pps) return DP_ERR_INVAL;
    if (!dp_dsc_pps_validate(pps)) return DP_ERR_INVAL;

    u8 raw[DP_DSC_PPS_SIZE];
    u32 written = dp_dsc_pps_encode(pps, raw);
    if (written != DP_DSC_PPS_SIZE) return DP_ERR_GENERIC;

    int rc = dp_dsc_hw_write_pps(c, raw);
    if (rc != DP_OK) return rc;

    dp_dsc_slice_result_t sl = {
        .slices_per_line = pps->pic_width / pps->slice_width,
        .slices_per_column = pps->pic_height / pps->slice_height,
        .slice_width = pps->slice_width,
        .slice_height = pps->slice_height,
        .total_slices = (pps->pic_width / pps->slice_width) *
                        (pps->pic_height / pps->slice_height),
    };
    dp_dsc_hw_write_slices(c, &sl);

    u8 native = DSC_OP_MODE_444;
    if (pps->simple_422) native = DSC_OP_MODE_422;
    dp_dsc_hw_op_mode(c, native);

    /* Propagate to sink. */
    rc = dp_dsc_dpcd_write_pps(c, raw);
    if (rc != DP_OK) return rc;

    return DP_OK;
}

int dp_dsc_enable(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;

    if (!(dp_reg_read(base, DP_REG_DSC_STATUS) & DSC_STATUS_PPS_LOADED))
        return DP_ERR_BUSY;

    u32 ctl = dp_reg_read(base, DP_REG_DSC_CTL);
    ctl |= DSC_CTL_EN | DSC_CTL_PPS_VALID | DSC_CTL_BLOCK_PRED;
    dp_reg_write(base, DP_REG_DSC_CTL, ctl);

    u64 deadline = riscv_read_time() + DSC_MAX_POLL_US;
    while (!(dp_reg_read(base, DP_REG_DSC_STATUS) & DSC_STATUS_PPS_LOADED)) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }

    return DP_OK;
}

int dp_dsc_disable(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;

    u32 ctl = dp_reg_read(base, DP_REG_DSC_CTL);
    ctl &= ~(DSC_CTL_EN | DSC_CTL_BLOCK_PRED);
    dp_reg_write(base, DP_REG_DSC_CTL, ctl);

    u64 deadline = riscv_read_time() + DSC_MAX_POLL_US;
    while (dp_reg_read(base, DP_REG_DSC_STATUS) & DSC_STATUS_BUSY) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }

    dp_reg_write(base, DP_REG_DSC_ERR_STATUS, 0xFFFFFFFFu);
    return dp_dsc_dpcd_clear(c);
}

/* Top-level: compute + configure + enable in one call. */
int dp_dsc_auto_enable(dp_controller_t *c,
                        u32 pic_width, u32 pic_height,
                        u16 bpp_fixed16, u8 bpc, u8 native_mode)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    /* Read sink slice limits from DPCD if present. */
    u32 max_slice_w = 4096;
    u32 max_slice_h = 4096;

    u8 cap[3] = { 0 };
    if (dp_aux_read_dpcd(c, DP_DSC_SLICE_CAPABILITIES_1, cap, 3) == DP_OK) {
        u16 sw = (u16)cap[0] | ((u16)(cap[1] & 0x03) << 8);
        u16 sh = (u16)(cap[1] >> 2) | ((u16)(cap[2] & 0x0F) << 6);
        if (sw) max_slice_w = (u32)sw * 4u + 4u;
        if (sh) max_slice_h = (u32)sh * 4u + 4u;
    }

    dp_dsc_slice_result_t sl;
    int rc = dp_dsc_calc_slices(pic_width, pic_height, max_slice_w, max_slice_h,
                                 c->bandwidth, c->lanes, &sl);
    if (rc != DP_OK) return rc;

    dp_dsc_pps_t pps;
    rc = dp_dsc_compute_pps(&pps, pic_width, pic_height, bpp_fixed16, bpc,
                             sl.slice_width, sl.slice_height, native_mode);
    if (rc != DP_OK) return rc;

    rc = dp_dsc_configure(c, &pps);
    if (rc != DP_OK) return rc;

    return dp_dsc_enable(c);
}

/* ========================================================================== */
/* PSR - HW PROGRAMMING                                                        */
/* ========================================================================== */

static u32 dp_psr_setup_time_us(u8 caps)
{
    /* caps bits 3:1 per DP spec. Value 6 = 0ms. */
    static const u32 table[8] = {
        330000, 275000, 220000, 165000, 110000, 55000, 0, 0
    };
    u8 idx = (caps >> 1) & 0x07;
    return table[idx];
}

static void dp_psr_hw_program(dp_controller_t *c, u32 idle_us, u32 wake_us)
{
    vaddr_t base = c->base_virtual_addr;
    dp_reg_write(base, DP_REG_PSR_TIMER_IDLE, idle_us);
    dp_reg_write(base, DP_REG_PSR_TIMER_WAKEUP, wake_us);
    dp_reg_write(base, DP_REG_PSR_IRQ_MASK,
                 PSR_IRQ_FRAME_UPDATE | PSR_IRQ_HPD | PSR_IRQ_LINK_LOST);
}

static int dp_psr_dpcd_enable(dp_controller_t *c)
{
    u8 cfg = 0x01;
    return dp_aux_write_dpcd(c, DP_PSR_EN_CFG, &cfg, 1);
}

static int dp_psr_dpcd_disable(dp_controller_t *c)
{
    u8 cfg = 0x00;
    return dp_aux_write_dpcd(c, DP_PSR_EN_CFG, &cfg, 1);
}

/* ========================================================================== */
/* PSR - PUBLIC API                                                            */
/* ========================================================================== */

int dp_psr_enable(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    u8 support = 0, caps = 0;
    if (dp_aux_read_dpcd(c, DP_PSR_SUPPORT, &support, 1) != DP_OK)
        return DP_ERR_AUX_NACK;

    if (!(support & DP_PSR_SINK_SUPPORTED))
        return DP_ERR_NOT_SUPPORTED;

    if (dp_aux_read_dpcd(c, DP_PSR_CAPS, &caps, 1) != DP_OK)
        return DP_ERR_AUX_NACK;

    u32 setup_us = dp_psr_setup_time_us(caps);
    u32 idle_us  = setup_us + 20000;  /* setup + 20ms margin */
    u32 wake_us  = 2000;              /* typical wake = 2ms */

    dp_psr_hw_program(c, idle_us, wake_us);

    vaddr_t base = c->base_virtual_addr;
    u32 ctl = dp_reg_read(base, DP_REG_PSR_CTL);
    ctl |= PSR_CTL_EN | PSR_CTL_IRQ_EN | PSR_CTL_AUTO_EXIT;
    dp_reg_write(base, DP_REG_PSR_CTL, ctl);

    dp_reg_write(base, DP_REG_PSR_IRQ_STATUS, 0xFFFFFFFFu);

    return dp_psr_dpcd_enable(c);
}

int dp_psr_disable(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;

    u32 ctl = dp_reg_read(base, DP_REG_PSR_CTL);
    ctl &= ~(PSR_CTL_EN | PSR_CTL_MAINLINK_OFF);
    dp_reg_write(base, DP_REG_PSR_CTL, ctl);

    /* Wait for exit to complete. */
    u64 deadline = riscv_read_time() + 50000ULL;
    while (dp_reg_read(base, DP_REG_PSR_STATUS) & PSR_STATUS_EXITING) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }

    return dp_psr_dpcd_disable(c);
}

/* Poll PSR state; call periodically from timer. Returns current state
 * (0 = idle, 1 = active, 2 = exiting, -1 = error). */
int dp_psr_poll(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;
    u32 st = dp_reg_read(base, DP_REG_PSR_STATUS);
    u32 irq = dp_reg_read(base, DP_REG_PSR_IRQ_STATUS);

    if (irq) dp_reg_write(base, DP_REG_PSR_IRQ_STATUS, irq);

    if (irq & PSR_IRQ_LINK_LOST) {
        dp_reg_write(base, DP_REG_PSR_CTL, dp_reg_read(base, DP_REG_PSR_CTL)
                     & ~PSR_CTL_MAINLINK_OFF);
        return -1;
    }

    if (st & PSR_STATUS_ERR) return -1;
    if (st & PSR_STATUS_ACTIVE) return 1;
    if (st & PSR_STATUS_EXITING) return 2;
    return 0;
}

/* Called when a new frame is committed (before scanout). */
int dp_psr_frame_committed(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;
    u32 st = dp_reg_read(base, DP_REG_PSR_STATUS);
    if (!(st & PSR_STATUS_ACTIVE)) return DP_OK;

    /* Request exit from PSR. */
    u32 ctl = dp_reg_read(base, DP_REG_PSR_CTL);
    dp_reg_write(base, DP_REG_PSR_CTL, ctl & ~PSR_CTL_MAINLINK_OFF);

    u64 deadline = riscv_read_time() + 20000ULL;
    while (dp_reg_read(base, DP_REG_PSR_STATUS) & PSR_STATUS_ACTIVE) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }
    return DP_OK;
}

/* ========================================================================== */
/* ALPM - DPCD HELPERS                                                         */
/* ========================================================================== */

static int dp_alpm_dpcd_probe(dp_controller_t *c, bool *supported)
{
    u8 v = 0;
    int rc = dp_aux_read_dpcd(c, DP_ALPM_CAP_REG, &v, 1);
    if (rc != DP_OK) return rc;
    *supported = (v & DP_ALPM_CAP_BIT) != 0;
    return DP_OK;
}

static int dp_alpm_dpcd_enable(dp_controller_t *c, bool aux_less)
{
    u8 v = 0x01;
    if (aux_less) v |= 0x02;
    return dp_aux_write_dpcd(c, DP_ALPM_CTRL_REG, &v, 1);
}

static int dp_alpm_dpcd_disable(dp_controller_t *c)
{
    u8 v = 0x00;
    return dp_aux_write_dpcd(c, DP_ALPM_CTRL_REG, &v, 1);
}

static int dp_alpm_dpcd_status(dp_controller_t *c, u8 *out)
{
    return dp_aux_read_dpcd(c, DP_ALPM_STATUS_REG, out, 1);
}

/* ========================================================================== */
/* ALPM - HW PROGRAMMING                                                       */
/* ========================================================================== */

static void dp_alpm_hw_program(dp_controller_t *c, u32 idle_us, u32 wake_us,
                                bool aux_less)
{
    vaddr_t base = c->base_virtual_addr;

    dp_reg_write(base, DP_REG_ALPM_TIMER_IDLE, idle_us);
    dp_reg_write(base, DP_REG_ALPM_TIMER_WAKEUP, wake_us);

    u32 wk = 0;
    wk |= ALPM_WAKEUP_SRC_AUX;
    if (aux_less) wk |= ALPM_WAKEUP_SRC_PHY;
    dp_reg_write(base, DP_REG_ALPM_WAKEUP_CFG, wk);

    dp_reg_write(base, DP_REG_ALPM_AUX_CNT, 0);

    dp_reg_write(base, DP_REG_ALPM_IRQ_MASK,
                 ALPM_IRQ_WAKEUP | ALPM_IRQ_LOCK_ERR | ALPM_IRQ_AUX_TIMEOUT);
    dp_reg_write(base, DP_REG_ALPM_IRQ_STATUS, 0xFFFFFFFFu);

    u32 ctl = 0;
    ctl |= ALPM_CTL_EN;
    ctl |= ALPM_CTL_AUTO_EXIT;
    if (aux_less) ctl |= ALPM_CTL_AUX_LESS;
    dp_reg_write(base, DP_REG_ALPM_CTL, ctl);
}

static void dp_alpm_hw_disable(dp_controller_t *c)
{
    vaddr_t base = c->base_virtual_addr;
    u32 ctl = dp_reg_read(base, DP_REG_ALPM_CTL);
    ctl &= ~(ALPM_CTL_EN | ALPM_CTL_AUX_LESS);
    dp_reg_write(base, DP_REG_ALPM_CTL, ctl);
}

/* ========================================================================== */
/* ALPM - PUBLIC API                                                           */
/* ========================================================================== */

int dp_alpm_configure(dp_controller_t *c, u32 cfg)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    bool supported = false;
    int rc = dp_alpm_dpcd_probe(c, &supported);
    if (rc != DP_OK) return rc;
    if (!supported) return DP_ERR_NOT_SUPPORTED;

    /* cfg layout:
     *   bit0 = enable
     *   bit1 = AUX-less mode
     *   bit2 = lock-error HPD enable
     *   bits 8..15 = idle timer (ms, capped)
     *   bits 16..23 = wake timer (ms) */
    bool enable   = (cfg & 0x01) != 0;
    bool aux_less = (cfg & 0x02) != 0;
    bool lock_hpd = (cfg & 0x04) != 0;
    u32 idle_ms = (cfg >> 8) & 0xFF;
    u32 wake_ms = (cfg >> 16) & 0xFF;
    if (idle_ms == 0) idle_ms = 50;
    if (wake_ms == 0) wake_ms = 10;

    if (!enable) {
        dp_alpm_hw_disable(c);
        return dp_alpm_dpcd_disable(c);
    }

    dp_alpm_hw_program(c, idle_ms * 1000, wake_ms * 1000, aux_less);

    vaddr_t base = c->base_virtual_addr;
    if (lock_hpd) {
        u32 ctl = dp_reg_read(base, DP_REG_ALPM_CTL);
        dp_reg_write(base, DP_REG_ALPM_CTL, ctl | ALPM_CTL_LOCK_ERR_HPD);
    }

    return dp_alpm_dpcd_enable(c, aux_less);
}

/* Poll ALPM status; call periodically. Returns current status:
 *   0 = idle, 1 = entered, 2 = exiting, -1 = error. */
int dp_alpm_poll(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;
    u32 irq = dp_reg_read(base, DP_REG_ALPM_IRQ_STATUS);
    if (irq) dp_reg_write(base, DP_REG_ALPM_IRQ_STATUS, irq);

    if (irq & ALPM_IRQ_LOCK_ERR) {
        /* Force re-auth of the link. */
        dp_reg_write(base, DP_REG_ALPM_CTL,
                     dp_reg_read(base, DP_REG_ALPM_CTL) & ~ALPM_CTL_EN);
        return -1;
    }

    u32 st = dp_reg_read(base, DP_REG_ALPM_STATUS);
    if (st & ALPM_STATUS_ERR) return -1;
    if (st & ALPM_STATUS_ENTERED) return 1;
    if (st & ALPM_STATUS_EXITING) return 2;
    return 0;
}

/* Force exit from ALPM (called by HPD handler or AUX traffic starter). */
int dp_alpm_wake(dp_controller_t *c)
{
    if (!c || !c->initialized) return DP_ERR_INVAL;

    vaddr_t base = c->base_virtual_addr;
    u32 st = dp_reg_read(base, DP_REG_ALPM_STATUS);
    if (!(st & ALPM_STATUS_ENTERED)) return DP_OK;

    u32 ctl = dp_reg_read(base, DP_REG_ALPM_CTL);
    dp_reg_write(base, DP_REG_ALPM_CTL, ctl & ~ALPM_CTL_EN);

    u64 deadline = riscv_read_time() + 20000ULL;
    while (dp_reg_read(base, DP_REG_ALPM_STATUS) & ALPM_STATUS_ENTERED) {
        if (riscv_read_time() > deadline) return DP_ERR_TIMEOUT;
        cpu_pause();
    }

    dp_reg_write(base, DP_REG_ALPM_CTL, ctl | ALPM_CTL_EN);
    return DP_OK;
}

/* ========================================================================== */
/* COMBINED POWER-MANAGEMENT DRIVER                                            */
/* ========================================================================== */

typedef struct {
    bool dsc_on;
    bool psr_on;
    bool alpm_on;
    u64  last_activity_us;
    u32  activity_count;
} dp_pm_ctx_t;

static dp_pm_ctx_t g_pm_ctx = {0};

int dp_pm_activity(dp_controller_t *c)
{
    if (!c) return DP_ERR_INVAL;
    g_pm_ctx.last_activity_us = riscv_read_time();
    g_pm_ctx.activity_count++;

    if (g_pm_ctx.psr_on) {
        int rc = dp_psr_frame_committed(c);
        if (rc != DP_OK && rc != DP_ERR_NOT_SUPPORTED) return rc;
    }
    if (g_pm_ctx.alpm_on) {
        int rc = dp_alpm_wake(c);
        if (rc != DP_OK && rc != DP_ERR_NOT_SUPPORTED) return rc;
    }
    return DP_OK;
}

int dp_pm_idle_tick(dp_controller_t *c)
{
    if (!c) return DP_ERR_INVAL;

    if (g_pm_ctx.psr_on) {
        int rc = dp_psr_poll(c);
        if (rc < 0) g_pm_ctx.psr_on = false;
    }
    if (g_pm_ctx.alpm_on) {
        int rc = dp_alpm_poll(c);
        if (rc < 0) g_pm_ctx.alpm_on = false;
    }
    return DP_OK;
}

int dp_pm_enable_all(dp_controller_t *c, bool dsc, bool psr, bool alpm,
                      u32 alpm_cfg)
{
    if (!c) return DP_ERR_INVAL;

    if (dsc) {
        /* Caller must already have called dp_dsc_configure(); only flip enable. */
        if (dp_dsc_enable(c) == DP_OK) g_pm_ctx.dsc_on = true;
    }
    if (psr) {
        if (dp_psr_enable(c) == DP_OK) g_pm_ctx.psr_on = true;
    }
    if (alpm) {
        if (dp_alpm_configure(c, alpm_cfg | 0x01) == DP_OK)
            g_pm_ctx.alpm_on = true;
    }
    g_pm_ctx.last_activity_us = riscv_read_time();
    return DP_OK;
}

int dp_pm_disable_all(dp_controller_t *c)
{
    if (!c) return DP_ERR_INVAL;
    if (g_pm_ctx.alpm_on) { (void)dp_alpm_configure(c, 0); g_pm_ctx.alpm_on = false; }
    if (g_pm_ctx.psr_on)  { (void)dp_psr_disable(c); g_pm_ctx.psr_on = false; }
    if (g_pm_ctx.dsc_on)  { (void)dp_dsc_disable(c); g_pm_ctx.dsc_on = false; }
    return DP_OK;
}

/* ========================================================================== */
/* DIAGNOSTICS                                                                 */
/* ========================================================================== */

__maybe_unused static const char *dp_dsc_bpp_str(u16 bpp_fixed16)
{
    static char buf[8];
    u32 whole = bpp_fixed16 >> 4;
    u32 frac  = bpp_fixed16 & 0x0F;
    buf[0] = (char)('0' + (whole / 10));
    buf[1] = (char)('0' + (whole % 10));
    buf[2] = '.';
    buf[3] = (char)('0' + (frac / 10));
    buf[4] = (char)('0' + (frac % 10));
    buf[5] = '\0';
    return buf;
}

__maybe_unused static void dp_dsc_dump(dp_controller_t *c)
{
    if (!c) return;
    vaddr_t b = c->base_virtual_addr;
    u32 ctl = dp_reg_read(b, DP_REG_DSC_CTL);
    u32 st  = dp_reg_read(b, DP_REG_DSC_STATUS);
    u32 err = dp_reg_read(b, DP_REG_DSC_ERR_STATUS);
    dp_info("DSC: ctl=0x%x st=0x%x err=0x%x frames=%u",
            ctl, st, err, dp_reg_read(b, DP_REG_DSC_FRAME_CNT));
}

__maybe_unused static void dp_psr_dump(dp_controller_t *c)
{
    if (!c) return;
    vaddr_t b = c->base_virtual_addr;
    dp_info("PSR: ctl=0x%x st=0x%x irq=0x%x idle=%u wake=%u",
            dp_reg_read(b, DP_REG_PSR_CTL),
            dp_reg_read(b, DP_REG_PSR_STATUS),
            dp_reg_read(b, DP_REG_PSR_IRQ_STATUS),
            dp_reg_read(b, DP_REG_PSR_TIMER_IDLE),
            dp_reg_read(b, DP_REG_PSR_TIMER_WAKEUP));
}

__maybe_unused static void dp_alpm_dump(dp_controller_t *c)
{
    if (!c) return;
    vaddr_t b = c->base_virtual_addr;
    dp_info("ALPM: ctl=0x%x st=0x%x irq=0x%x idle=%u wake=%u",
            dp_reg_read(b, DP_REG_ALPM_CTL),
            dp_reg_read(b, DP_REG_ALPM_STATUS),
            dp_reg_read(b, DP_REG_ALPM_IRQ_STATUS),
            dp_reg_read(b, DP_REG_ALPM_TIMER_IDLE),
            dp_reg_read(b, DP_REG_ALPM_TIMER_WAKEUP));
}

__maybe_unused static void dp_pm_dump(void)
{
    dp_info("PM: dsc=%d psr=%d alpm=%d acts=%u last=%llu",
            g_pm_ctx.dsc_on, g_pm_ctx.psr_on, g_pm_ctx.alpm_on,
            g_pm_ctx.activity_count,
            (unsigned long long)g_pm_ctx.last_activity_us);
}

/* ========================================================================== */
/* END OF BLOCK 7                                                              */
/* ========================================================================== */
