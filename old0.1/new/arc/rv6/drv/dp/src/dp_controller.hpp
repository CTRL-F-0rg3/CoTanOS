/*
 * ============================================================================
 *  dp_controller.hpp - C++ complementary layer for dp_core
 *  --------------------------------------------------------------------------
 *  Companion to dp_core.c / dp_core.h.
 *  Provides RAII, typed enums, event callbacks, orchestration sequences,
 *  and gap-fillers on top of the C API. It does NOT reimplement DP logic.
 *  The raw C API is always reachable via Controller::native().
 *
 *  Requires:  C++17 (optional C++20 for std::span usage)
 *  Includes:  dp_core.h  (which pulls in the full driver state model)
 * ============================================================================
 */

#pragma once

extern "C" {
#include "dp_core.h"
}

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>
#include <vector>
#include <optional>
#include <functional>
#include <string_view>
#include <atomic>

#if defined(__has_include)
#  if __has_include("core_io.h")
#    include "core_io.h"
#    define DP_HAS_CORE_IO 1
#  endif
#endif

namespace dp {

/* ========================================================================== */
/* Version / feature flags                                                     */
/* ========================================================================== */

constexpr int kApiVersion = 1;

#if defined(__cpp_lib_span)
#  define DP_HAS_SPAN 1
#  include <span>
#endif

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

/* ========================================================================== */
/* Error enum (mirrors DP_ERR_* from C header)                                 */
/* ========================================================================== */

enum class Error : int {
    Ok           =  0,
    Generic      = -1,
    Timeout      = -2,
    Invalid      = -3,
    NoMemory     = -4,
    Busy         = -5,
    AuxNack      = -6,
    AuxDefer     = -7,
    AuxCrc       = -8,
    LinkTrain    = -9,
    Edid         = -10,
    NoDevice     = -11,
    NotSupported = -12,
    Protocol     = -13,
    Hdcp         = -14,
};

constexpr std::string_view to_string(Error e) noexcept {
    switch (e) {
    case Error::Ok:           return "Ok";
    case Error::Generic:      return "Generic";
    case Error::Timeout:      return "Timeout";
    case Error::Invalid:      return "Invalid";
    case Error::NoMemory:     return "NoMemory";
    case Error::Busy:         return "Busy";
    case Error::AuxNack:      return "AuxNack";
    case Error::AuxDefer:     return "AuxDefer";
    case Error::AuxCrc:       return "AuxCrc";
    case Error::LinkTrain:    return "LinkTrain";
    case Error::Edid:         return "Edid";
    case Error::NoDevice:     return "NoDevice";
    case Error::NotSupported: return "NotSupported";
    case Error::Protocol:     return "Protocol";
    case Error::Hdcp:         return "Hdcp";
    }
    return "Unknown";
}

constexpr Error from_c(int rc) noexcept {
    switch (rc) {
    case DP_OK:                 return Error::Ok;
    case DP_ERR_GENERIC:        return Error::Generic;
    case DP_ERR_TIMEOUT:        return Error::Timeout;
    case DP_ERR_INVAL:          return Error::Invalid;
    case DP_ERR_NOMEM:          return Error::NoMemory;
    case DP_ERR_BUSY:           return Error::Busy;
    case DP_ERR_AUX_NACK:       return Error::AuxNack;
    case DP_ERR_AUX_DEFER:      return Error::AuxDefer;
    case DP_ERR_AUX_CRC:        return Error::AuxCrc;
    case DP_ERR_LINK_TRAIN:     return Error::LinkTrain;
    case DP_ERR_EDID:           return Error::Edid;
    case DP_ERR_NO_DEVICE:      return Error::NoDevice;
    case DP_ERR_NOT_SUPPORTED:  return Error::NotSupported;
    case DP_ERR_PROTOCOL:       return Error::Protocol;
    case DP_ERR_HDCP:           return Error::Hdcp;
    default:                    return Error::Generic;
    }
}

/* ========================================================================== */
/* Result<T>                                                                   */
/* ========================================================================== */

template <typename T>
class Result {
public:
    Result(T value) noexcept : value_(std::move(value)), err_(Error::Ok) {}
    Result(Error e) noexcept : err_(e) {}

    bool has_value() const noexcept { return err_ == Error::Ok; }
    explicit operator bool() const noexcept { return has_value(); }

    T& value() & { return value_; }
    const T& value() const& { return value_; }
    T&& value() && { return std::move(value_); }

    T value_or(T def) const { return has_value() ? value_ : std::move(def); }
    Error error() const noexcept { return err_; }

private:
    T value_{};
    Error err_;
};

template <>
class Result<void> {
public:
    Result() noexcept = default;
    Result(Error e) noexcept : err_(e) {}

    bool has_value() const noexcept { return err_ == Error::Ok; }
    explicit operator bool() const noexcept { return has_value(); }
    Error error() const noexcept { return err_; }

private:
    Error err_ = Error::Ok;
};

/* ========================================================================== */
/* Typed enums + C conversions                                                 */
/* ========================================================================== */

enum class State : u8 {
    Reset        = 0x00,
    Detected     = 0x01,
    AuxReady     = 0x02,
    LinkTrainCr  = 0x03,
    LinkTrainEq  = 0x04,
    LinkReady    = 0x05,
    StreamReady  = 0x06,
    ActiveStream = 0x07,
    Suspend      = 0x08,
    Resume       = 0x09,
    ErrorFault   = 0xFF,
};

constexpr State to_state(dp_state_machine_t s) noexcept {
    switch (s) {
    case DP_STATE_RESET:         return State::Reset;
    case DP_STATE_DETECTED:      return State::Detected;
    case DP_STATE_AUX_READY:     return State::AuxReady;
    case DP_STATE_LINK_TRAIN_CR: return State::LinkTrainCr;
    case DP_STATE_LINK_TRAIN_EQ: return State::LinkTrainEq;
    case DP_STATE_LINK_READY:    return State::LinkReady;
    case DP_STATE_STREAM_READY:  return State::StreamReady;
    case DP_STATE_ACTIVE_STREAM: return State::ActiveStream;
    case DP_STATE_SUSPEND:       return State::Suspend;
    case DP_STATE_RESUME:        return State::Resume;
    default:                     return State::ErrorFault;
    }
}

constexpr dp_state_machine_t from_state(State s) noexcept {
    switch (s) {
    case State::Reset:        return DP_STATE_RESET;
    case State::Detected:     return DP_STATE_DETECTED;
    case State::AuxReady:     return DP_STATE_AUX_READY;
    case State::LinkTrainCr:  return DP_STATE_LINK_TRAIN_CR;
    case State::LinkTrainEq:  return DP_STATE_LINK_TRAIN_EQ;
    case State::LinkReady:    return DP_STATE_LINK_READY;
    case State::StreamReady:  return DP_STATE_STREAM_READY;
    case State::ActiveStream: return DP_STATE_ACTIVE_STREAM;
    case State::Suspend:      return DP_STATE_SUSPEND;
    case State::Resume:       return DP_STATE_RESUME;
    case State::ErrorFault:   return DP_STATE_ERROR_FAULT;
    }
    return DP_STATE_ERROR_FAULT;
}

enum class Bandwidth : u8 {
    Rbr   = 0x06,
    Hbr   = 0x0A,
    Hbr2  = 0x14,
    Hbr3  = 0x1E,
    Uhbr10 = 0x01,
    Uhbr20 = 0x02,
    Uhbr135 = 0x03,
};

constexpr Bandwidth to_bw(dp_link_bandwidth_t b) noexcept {
    return static_cast<Bandwidth>(b);
}
constexpr dp_link_bandwidth_t from_bw(Bandwidth b) noexcept {
    return static_cast<dp_link_bandwidth_t>(b);
}
constexpr std::string_view to_string(Bandwidth b) noexcept {
    switch (b) {
    case Bandwidth::Rbr:    return "RBR";
    case Bandwidth::Hbr:    return "HBR";
    case Bandwidth::Hbr2:   return "HBR2";
    case Bandwidth::Hbr3:   return "HBR3";
    case Bandwidth::Uhbr10: return "UHBR10";
    case Bandwidth::Uhbr20: return "UHBR20";
    case Bandwidth::Uhbr135:return "UHBR13.5";
    }
    return "?";
}

enum class LaneCount : u8 { One = 1, Two = 2, Four = 4 };

constexpr LaneCount to_lanes(dp_lane_count_t l) noexcept {
    return static_cast<LaneCount>(l);
}
constexpr dp_lane_count_t from_lanes(LaneCount l) noexcept {
    return static_cast<dp_lane_count_t>(l);
}

enum class PixelFormat : u8 {
    Rgb    = 0,
    Yuv444 = 1,
    Yuv422 = 2,
    Yuv420 = 3,
    Raw    = 4,
};

constexpr dp_pixel_format_t from_pf(PixelFormat p) noexcept {
    return static_cast<dp_pixel_format_t>(p);
}
constexpr PixelFormat to_pf(dp_pixel_format_t p) noexcept {
    return static_cast<PixelFormat>(p);
}

enum class Bpc : u8 { B6 = 6, B8 = 8, B10 = 10, B12 = 12, B16 = 16 };

constexpr u32 bpc_value(Bpc b) noexcept { return static_cast<u32>(b); }

enum class Colorimetry : u8 {
    Rgb709      = 0,
    Rgb601      = 1,
    Bt709       = 2,
    XvYcc601    = 3,
    XvYcc709    = 4,
    SYcc601     = 5,
    AdobeYcc601 = 6,
    AdobeRgb    = 7,
    Bt2020Cycc  = 8,
    Bt2020Ycc   = 9,
    Bt2020Rgb   = 10,
};

/* ========================================================================== */
/* Timing                                                                      */
/* ========================================================================== */

struct Timing {
    u32 pixel_clock_khz = 0;
    u16 hactive = 0;
    u16 hblank = 0;
    u16 vactive = 0;
    u16 vblank = 0;
    u16 hsync_offset = 0;
    u16 hsync_pulse = 0;
    u16 vsync_offset = 0;
    u16 vsync_pulse = 0;
    u8  refresh_hz = 60;
    bool interlaced = false;
    bool hsync_positive = false;
    bool vsync_positive = false;

    /* ---- conversions ----------------------------------------------------- */

    static Timing from_c(const dp_timing_spec_t& t) noexcept {
        Timing r{};
        r.pixel_clock_khz = t.pixel_clock;
        r.hactive = t.hactive;
        r.hblank = t.hblank;
        r.vactive = t.vactive;
        r.vblank = t.vblank;
        r.hsync_offset = t.hsync_offset;
        r.hsync_pulse = t.hsync_pulse;
        r.vsync_offset = t.vsync_offset;
        r.vsync_pulse = t.vsync_pulse;
        r.refresh_hz = t.refresh_hz;
        r.interlaced = t.interlaced;
        return r;
    }

    dp_timing_spec_t to_c() const noexcept {
        dp_timing_spec_t t{};
        t.pixel_clock = pixel_clock_khz;
        t.hactive = hactive;
        t.hblank = hblank;
        t.vactive = vactive;
        t.vblank = vblank;
        t.hsync_offset = hsync_offset;
        t.hsync_pulse = hsync_pulse;
        t.vsync_offset = vsync_offset;
        t.vsync_pulse = vsync_pulse;
        t.refresh_hz = refresh_hz;
        t.interlaced = interlaced;
        return t;
    }

    static Timing from_edid(const edid_timing_t& et) noexcept {
        Timing r{};
        r.pixel_clock_khz = (u32)et.pixel_clock * 10u;
        r.hactive = et.hactive;
        r.hblank = et.hblank;
        r.vactive = et.vactive;
        r.vblank = et.vblank;
        r.hsync_offset = et.hfront_porch;
        r.hsync_pulse = et.hsync_pulse;
        r.vsync_offset = et.vfront_porch;
        r.vsync_pulse = et.vsync_pulse;
        r.interlaced = et.interlaced;
        r.hsync_positive = et.hsync_positive;
        r.vsync_positive = et.vsync_positive;
        r.refresh_hz = 60;
        return r;
    }

    static Timing from_vic(u8 vic) {
        dp_timing_spec_t t{};
        if (::dp_timing_from_vic(vic, &t) == DP_OK)
            return from_c(t);
        return Timing{};
    }

    /* ---- derived --------------------------------------------------------- */

    u32 htotal() const noexcept { return (u32)hactive + hblank; }
    u32 vtotal() const noexcept { return (u32)vactive + vblank; }

    u32 refresh_computed() const noexcept {
        u32 px = htotal() * vtotal();
        if (!px) return 0;
        u32 hz = (u32)(((u64)pixel_clock_khz * 1000ULL) / px);
        if (interlaced) hz *= 2;
        return hz;
    }

    bool valid() const noexcept {
        return pixel_clock_khz != 0 && hactive != 0 && vactive != 0 &&
               hsync_pulse != 0 && vsync_pulse != 0;
    }

    friend bool operator==(const Timing&, const Timing&) noexcept = default;
};

/* ========================================================================== */
/* Logging                                                                     */
/* ========================================================================== */

using LogFn = void(*)(int level, const char* fmt, ...);

inline void set_log(LogFn fn) noexcept { dp_log_fn = fn; }

/* ========================================================================== */
/* ControllerConfig                                                            */
/* ========================================================================== */

struct ControllerConfig {
    paddr_t    phys_addr = 0;
    vaddr_t    virt_addr = 0;
    std::size_t mapped_size = 0;
    Bandwidth  preferred_bw = Bandwidth::Hbr2;
    LaneCount  preferred_lanes = LaneCount::Four;
    bool       enable_audio = true;
    bool       enable_hdcp = false;
    bool       enable_mst  = false;
    bool       enable_psr  = false;
    bool       enable_dsc  = false;
};

/* ========================================================================== */
/* Controller                                                                  */
/* ========================================================================== */

class Controller {
public:
    /* ---- lifecycle ------------------------------------------------------- */

    explicit Controller(const ControllerConfig& cfg) noexcept {
        ::dp_controller_init(&ctrl_, cfg.phys_addr, cfg.virt_addr, cfg.mapped_size);
        ::memset(&drv_, 0, sizeof(drv_));
        drv_.ctrl = &ctrl_;
        drv_.ctrl->bandwidth = from_bw(cfg.preferred_bw);
        drv_.ctrl->lanes = from_lanes(cfg.preferred_lanes);
        cfg_ = cfg;
        valid_ = ctrl_.initialized;
    }

    ~Controller() noexcept {
        shutdown();
    }

    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;

    Controller(Controller&& o) noexcept
        : ctrl_(o.ctrl_), drv_(o.drv_), cfg_(o.cfg_), valid_(o.valid_),
          on_state_change_(std::move(o.on_state_change_)),
          on_hpd_(std::move(o.on_hpd_)),
          on_irq_(std::move(o.on_irq_)) {
        drv_.ctrl = &ctrl_;
        o.valid_ = false;
        o.ctrl_.initialized = false;
    }

    Controller& operator=(Controller&&) = delete;

    /* ---- queries --------------------------------------------------------- */

    bool      is_initialized() const noexcept { return valid_; }
    State     state() const noexcept { return to_state(ctrl_.current_state); }
    Bandwidth bandwidth() const noexcept { return to_bw(ctrl_.bandwidth); }
    LaneCount lanes() const noexcept { return to_lanes(ctrl_.lanes); }

    /* ---- escape hatch ---------------------------------------------------- */

    dp_controller_t*    native() noexcept { return &ctrl_; }
    const dp_controller_t* native() const noexcept { return &ctrl_; }
    dp_driver_state_t*  driver_state() noexcept { return &drv_; }

    /* ---- low-level register ---------------------------------------------- */

    u32  read_reg(u32 off) noexcept {
        return ::dp_core_read_reg(&ctrl_, off);
    }
    void write_reg(u32 off, u32 v) noexcept {
        ::dp_core_write_reg(&ctrl_, off, v);
    }

    /* ---- AUX / DPCD ------------------------------------------------------ */

    Result<u8> read_dpcd_u8(u32 addr) noexcept {
        u8 v = 0;
        int rc = ::dp_aux_read_dpcd(&ctrl_, addr, &v, 1);
        return rc == DP_OK ? Result<u8>(v) : Result<u8>(from_c(rc));
    }

    Error read_dpcd(u32 addr, u8* buf, std::size_t len) noexcept {
        return from_c(::dp_aux_read_dpcd(&ctrl_, addr, buf, (u32)len));
    }

    Error write_dpcd(u32 addr, const u8* buf, std::size_t len) noexcept {
        return from_c(::dp_aux_write_dpcd(&ctrl_, addr, buf, (u32)len));
    }

    bool probe_sink(u8* rev = nullptr) noexcept {
        return ::dp_aux_probe_sink(&ctrl_, rev);
    }

    /* ---- link ------------------------------------------------------------ */

    Result<dp_link_caps_t> read_link_caps() noexcept {
        dp_link_caps_t caps{};
        int rc = ::dp_link_read_caps(&ctrl_, &caps);
        return rc == DP_OK ? Result<dp_link_caps_t>(caps)
                           : Result<dp_link_caps_t>(from_c(rc));
    }

    Error set_rate(Bandwidth bw, LaneCount lanes) noexcept {
        set_state_if_needed(State::Detected);
        return from_c(::dp_link_set_rate(&ctrl_, from_bw(bw), from_lanes(lanes)));
    }

    Error train_link() noexcept {
        State before = state();
        Error e = from_c(::dp_link_train(&ctrl_));
        emit_state(before);
        return e;
    }

    Error train_cr() noexcept { return from_c(::dp_link_train_cr(&ctrl_)); }
    Error train_eq() noexcept { return from_c(::dp_link_train_eq(&ctrl_)); }

    Result<dp_link_training_state_t> verify_link() noexcept {
        dp_link_training_state_t st{};
        int rc = ::dp_link_verify_status(&ctrl_, &st);
        if (rc == DP_OK) return Result<dp_link_training_state_t>(st);
        return Result<dp_link_training_state_t>(from_c(rc));
    }

    /* ---- stream ---------------------------------------------------------- */

    Error set_timing(const Timing& t) noexcept {
        State before = state();
        dp_timing_spec_t tc = t.to_c();
        Error e = from_c(::dp_stream_set_timing(&ctrl_, &tc));
        emit_state(before);
        return e;
    }

    Error set_timing(const Timing& t, PixelFormat fmt, Bpc bpc,
                     Colorimetry cm) noexcept {
        State before = state();
        dp_timing_spec_t tc = t.to_c();
        Error e = from_c(::dp_stream_set_timing_ex(&drv_, 0, &tc,
                                                   from_pf(fmt), bpc_value(bpc),
                                                   (u8)cm));
        emit_state(before);
        return e;
    }

    Error enable_stream(u8 id) noexcept {
        State before = state();
        Error e = from_c(::dp_stream_enable_ex(&drv_, id));
        emit_state(before);
        return e;
    }

    Error disable_stream(u8 id) noexcept {
        State before = state();
        Error e = from_c(::dp_stream_disable_ex(&drv_, id));
        emit_state(before);
        return e;
    }

    /* ---- EDID ------------------------------------------------------------ */

    Result<edid_base_block_t> read_edid() noexcept {
        edid_base_block_t b{};
        cea861_extension_t cea{};
        int rc = ::dp_edid_read(&ctrl_, &b, &cea, sizeof(cea));
        return rc == DP_OK ? Result<edid_base_block_t>(b)
                           : Result<edid_base_block_t>(from_c(rc));
    }

    Result<std::vector<edid_timing_t>> parse_edid_timings() noexcept {
        edid_base_block_t b{};
        int rc = ::dp_edid_read(&ctrl_, &b, nullptr, 0);
        if (rc != DP_OK) return Result<std::vector<edid_timing_t>>(from_c(rc));

        std::vector<edid_timing_t> out;
        out.resize(32);
        int n = ::dp_edid_parse(&b, out.data(), (u32)out.size());
        if (n < 0) return Result<std::vector<edid_timing_t>>(Error::Edid);
        out.resize((std::size_t)n);
        return Result<std::vector<edid_timing_t>>(std::move(out));
    }

    /* ---- audio ----------------------------------------------------------- */

    Error enable_audio(u8 stream_id,
                       const dp_audio_infoframe_params_t& p) noexcept {
        Error e = from_c(::dp_audio_configure(&ctrl_, stream_id, &p));
        if (e != Error::Ok) return e;
        return from_c(::dp_audio_enable(&ctrl_, stream_id));
    }

    Error disable_audio(u8 stream_id) noexcept {
        return from_c(::dp_audio_disable(&ctrl_, stream_id));
    }

    /* ---- HDCP ------------------------------------------------------------ */

    Error enable_hdcp(u32 version) noexcept {
        return from_c(::dp_hdcp_enable(&ctrl_, version));
    }
    Error disable_hdcp() noexcept {
        return from_c(::dp_hdcp_disable(&ctrl_));
    }
    Error hdcp_poll() noexcept {
        return from_c(::dp_hdcp_poll(&ctrl_));
    }
    Error hdcp_frame_tick() noexcept {
        return from_c(::dp_hdcp_frame_tick(&ctrl_));
    }

    /* ---- MST ------------------------------------------------------------- */

    Error enable_mst() noexcept {
        return from_c(::dp_mst_enable(&ctrl_));
    }
    Error disable_mst() noexcept {
        return from_c(::dp_mst_disable(&ctrl_));
    }

    /* ---- PSR / DSC ------------------------------------------------------- */

    Error enable_psr() noexcept  { return from_c(::dp_psr_enable(&ctrl_)); }
    Error disable_psr() noexcept { return from_c(::dp_psr_disable(&ctrl_)); }

    Error enable_dsc(const dp_dsc_pps_t& pps) noexcept {
        return from_c(::dp_dsc_configure(&ctrl_, &pps));
    }
    Error disable_dsc() noexcept { return from_c(::dp_dsc_disable(&ctrl_)); }

    /* ---- callbacks ------------------------------------------------------- */

    using OnStateChangeFn = std::function<void(State old_state, State new_state)>;
    using OnHpdFn         = std::function<void(bool connected)>;
    using OnIrqFn         = std::function<void(u32 irq_mask)>;

    void set_on_state_change(OnStateChangeFn fn) noexcept {
        on_state_change_ = std::move(fn);
    }
    void set_on_hpd(OnHpdFn fn) noexcept { on_hpd_ = std::move(fn); }
    void set_on_irq(OnIrqFn fn) noexcept { on_irq_ = std::move(fn); }

    /* ---- polling / lifecycle -------------------------------------------- */

    void poll() noexcept {
        State before = state();
        ::dp_hdcp_poll(&ctrl_);   /* harmless if HDCP not enabled */
        ::dp_mst_poll(&ctrl_);    /* best-effort; may return errors silently */
        emit_state(before);
    }

    void frame_tick() noexcept {
        ::dp_hdcp_frame_tick(&ctrl_);
    }

    /* ---- orchestration (fills what C doesn't have) ----------------------- */

    /* Full bring-up: probe sink, read caps, train link, set timing, enable. */
    Error bring_up(const Timing& t, PixelFormat fmt = PixelFormat::Rgb,
                   Bpc bpc = Bpc::B8) noexcept {
        if (!valid_) return Error::Invalid;

        u8 rev = 0;
        if (!probe_sink(&rev)) {
            set_state(State::ErrorFault);
            return Error::NoDevice;
        }
        set_state(State::Detected);

        auto caps = read_link_caps();
        if (!caps) return caps.error();

        Bandwidth target_bw = cfg_.preferred_bw;
        if ((u32)from_bw(target_bw) > caps.value().max_link_rate)
            target_bw = to_bw((dp_link_bandwidth_t)caps.value().max_link_rate);

        LaneCount target_lanes = cfg_.preferred_lanes;
        if (caps.value().max_lane_count < (u32)from_lanes(target_lanes)) {
            if (caps.value().max_lane_count >= 4) target_lanes = LaneCount::Four;
            else if (caps.value().max_lane_count >= 2) target_lanes = LaneCount::Two;
            else target_lanes = LaneCount::One;
        }

        Error e = set_rate(target_bw, target_lanes);
        if (e != Error::Ok) return e;

        e = train_link();
        if (e != Error::Ok) { set_state(State::ErrorFault); return e; }

        e = set_timing(t, fmt, bpc, Colorimetry::Rgb709);
        if (e != Error::Ok) { set_state(State::ErrorFault); return e; }

        if (cfg_.enable_mst) { (void)enable_mst(); }
        if (cfg_.enable_psr) { (void)enable_psr(); }
        if (cfg_.enable_hdcp) (void)enable_hdcp(2);

        e = enable_stream(0);
        emit_state(state());
        return e;
    }

    /* Safe teardown. Best-effort: ignores errors from optional stages. */
    void shutdown() noexcept {
        if (!valid_) return;
        (void)::dp_stream_disable_ex(&drv_, 0);
        if (cfg_.enable_hdcp) (void)::dp_hdcp_disable(&ctrl_);
        if (cfg_.enable_mst)  (void)::dp_mst_disable(&ctrl_);
        if (cfg_.enable_audio)(void)::dp_audio_disable(&ctrl_, 0);
        ::dp_controller_shutdown(&ctrl_);
        valid_ = false;
    }

    /* Health check: returns true when sink is present and link is OK. */
    bool health_check() noexcept {
        if (!valid_) return false;
        if (!probe_sink()) return false;
        auto st = verify_link();
        return st && st.value() == DP_LINK_TRAINING_SUCCESS;
    }

    /* Retry helper: run a callable up to N times with backoff. */
    template <typename Fn>
    Error retry(Fn&& fn, unsigned attempts = 3, u32 backoff_us = 100) noexcept {
        for (unsigned i = 0; i < attempts; ++i) {
            Error e = fn();
            if (e == Error::Ok) return Error::Ok;
            if (e != Error::AuxDefer && e != Error::Timeout &&
                e != Error::AuxNack && e != Error::Busy)
                return e;
            ::dp_udelay(backoff_us << i);
        }
        return Error::Timeout;
    }

private:
    void set_state(State s) noexcept {
        State before = state();
        ctrl_.current_state = from_state(s);
        emit_state(before);
    }

    void set_state_if_needed(State s) noexcept {
        if (ctrl_.current_state == DP_STATE_RESET)
            set_state(s);
    }

    void emit_state(State before) noexcept {
        State now = state();
        if (before != now && on_state_change_)
            on_state_change_(before, now);
    }

    dp_controller_t    ctrl_{};
    dp_driver_state_t  drv_{};
    ControllerConfig   cfg_{};
    bool               valid_ = false;

    OnStateChangeFn    on_state_change_;
    OnHpdFn            on_hpd_;
    OnIrqFn            on_irq_;
};

/* ========================================================================== */
/* Register bitfield helper (compile-time offset + mask)                       */
/* ========================================================================== */

template <u32 Offset, u32 Mask, unsigned Shift = 0>
struct RegField {
    static u32 get(const Controller& c) noexcept {
        return (c.read_reg(Offset) & Mask) >> Shift;
    }
    static void set(Controller& c, u32 v) noexcept {
        u32 cur = c.read_reg(Offset);
        cur = (cur & ~Mask) | ((v << Shift) & Mask);
        c.write_reg(Offset, cur);
    }
};

/* ========================================================================== */
/* Stream RAII handle                                                          */
/* ========================================================================== */

class StreamHandle {
public:
    StreamHandle(Controller& c, u8 id) noexcept : c_(&c), id_(id) {}

    ~StreamHandle() noexcept {
        if (active_) (void)c_->disable_stream(id_);
    }

    StreamHandle(const StreamHandle&) = delete;
    StreamHandle& operator=(const StreamHandle&) = delete;
    StreamHandle(StreamHandle&& o) noexcept
        : c_(o.c_), id_(o.id_), active_(o.active_) { o.active_ = false; }

    Error enable() noexcept {
        Error e = c_->enable_stream(id_);
        if (e == Error::Ok) active_ = true;
        return e;
    }

    Error disable() noexcept {
        active_ = false;
        return c_->disable_stream(id_);
    }

    bool is_active() const noexcept { return active_; }
    u8   id() const noexcept { return id_; }

private:
    Controller* c_;
    u8          id_;
    bool        active_ = false;
};

/* ========================================================================== */
/* EDID blob wrapper                                                           */
/* ========================================================================== */

class EdidBlob {
public:
    static Result<EdidBlob> read(Controller& c) noexcept {
        EdidBlob b;
        cea861_extension_t cea{};
        int rc = ::dp_edid_read(c.native(), &b.base_, &cea, sizeof(cea));
        if (rc != DP_OK) return Result<EdidBlob>(from_c(rc));
        b.has_cea_ = true;
        b.cea_ = cea;
        return Result<EdidBlob>(std::move(b));
    }

    const edid_base_block_t& base() const noexcept { return base_; }
    bool                     has_cea() const noexcept { return has_cea_; }
    const cea861_extension_t& cea() const noexcept { return cea_; }

    std::vector<Timing> timings() const noexcept {
        std::vector<Timing> out;
        std::vector<edid_timing_t> raw(32);
        int n = ::dp_edid_parse(&base_, raw.data(), (u32)raw.size());
        if (n <= 0) return out;
        out.reserve((std::size_t)n);
        for (int i = 0; i < n; ++i)
            out.push_back(Timing::from_edid(raw[(std::size_t)i]));
        return out;
    }

    std::optional<edid_timing_t> find_dtd(u32 h, u32 v) const noexcept {
        edid_detailed_timing_t d{};
        if (::dp_edid_find_dtd(&base_, h, v, &d) == DP_OK) return d;
        return std::nullopt;
    }

private:
    edid_base_block_t base_{};
    cea861_extension_t cea_{};
    bool has_cea_ = false;
};

/* ========================================================================== */
/* Audio card wrapper                                                          */
/* ========================================================================== */

class AudioCard {
public:
    AudioCard() noexcept = default;

    Error init(Controller& c, std::string_view name = "DP-HDMI-Audio") noexcept {
        char buf[32] = {0};
        std::size_t n = name.size() < sizeof(buf) - 1 ? name.size() : sizeof(buf) - 1;
        std::memcpy(buf, name.data(), n);
        return from_c(::dp_snd_card_init(&card_, c.native(), buf));
    }

    Error set_eld(const u8* eld, std::size_t len) noexcept {
        return from_c(::dp_snd_card_set_eld(&card_, eld, (u32)len));
    }

    Error prepare(u8* dma_area, u32 dma_bytes) noexcept {
        return from_c(::dp_snd_pcm_prepare(&card_, dma_area, dma_bytes));
    }

    Error apply_params(const dp_pcm_hw_params_t& p) noexcept {
        return from_c(::dp_snd_pcm_apply_hw_params(&card_, &p));
    }

    Error start() noexcept { return from_c(::dp_snd_pcm_start(&card_)); }
    Error stop()  noexcept { return from_c(::dp_snd_pcm_stop(&card_)); }

    dp_snd_card_t* native() noexcept { return &card_; }

private:
    dp_snd_card_t card_{};
};

/* ========================================================================== */
/* VIC helpers                                                                 */
/* ========================================================================== */

constexpr std::optional<u8> vic_from_hv(u16 h, u16 v, u8 refresh,
                                        bool interlaced = false) noexcept {
    struct Entry { u8 vic; u16 h; u16 v; u8 r; bool i; };
    static constexpr Entry table[] = {
        { 1,   640,  480,  60, false },
        { 2,   720,  480,  60, false },
        { 3,   720,  480,  60, false },
        { 4,  1280,  720,  60, false },
        { 5,  1920, 1080,  60, true  },
        {16,  1920, 1080,  60, false },
        {17,   720,  576,  50, false },
        {18,   720,  576,  50, false },
        {19,  1280,  720,  50, false },
        {20,  1920, 1080,  50, true  },
        {31,  1920, 1080,  50, false },
        {32,  1920, 1080,  24, false },
        {33,  1920, 1080,  25, false },
        {34,  1920, 1080,  30, false },
        {93,  3840, 2160,  24, false },
        {94,  3840, 2160,  25, false },
        {95,  3840, 2160,  30, false },
        {96,  3840, 2160,  50, false },
        {97,  3840, 2160,  60, false },
    };
    for (auto& e : table) {
        if (e.h == h && e.v == v && e.r == refresh && e.i == interlaced)
            return e.vic;
    }
    return std::nullopt;
}

/* ========================================================================== */
/* Free helpers                                                                */
/* ========================================================================== */

inline bool bw_ok_for(const Timing& t, Bpc bpc, PixelFormat fmt,
                      Bandwidth bw, LaneCount lanes) noexcept {
    dp_timing_spec_t tc = t.to_c();
    return ::dp_check_bw_fits(&tc, bpc_value(bpc), from_pf(fmt),
                              from_bw(bw), from_lanes(lanes));
}

inline Bandwidth lowest_bw_for(const Timing& t, Bpc bpc, PixelFormat fmt,
                               LaneCount lanes) noexcept {
    dp_timing_spec_t tc = t.to_c();
    return to_bw(::dp_lowest_bw_for(&tc, bpc_value(bpc), from_pf(fmt),
                                    from_lanes(lanes)));
}

inline u32 bpp(PixelFormat fmt, Bpc bpc) noexcept {
    return ::dp_calc_bpp(bpc_value(bpc), from_pf(fmt));
}

} /* namespace dp */
