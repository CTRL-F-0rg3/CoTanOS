/*
 * ============================================================================
 *  STORAGE_CONTROLLER.HPP - C++17 abstraction over storage_core.h
 *  Architecture: RISC-V (RV32 / RV64)
 *
 *  Design:
 *    - Header-only; storage_controller.cpp is a stub for the trampoline.
 *    - RAII owners: NvmeController, AhciController, AacController.
 *    - Non-owning handle: BlockDevice (trivially copyable).
 *    - Result<T> instead of error codes; no exceptions.
 *    - Only std::array, std::span, std::optional, std::string_view used.
 *    - No allocation on hot path; async I/O uses caller-owned
 *      stor_blk_req_t with C linkage callbacks.
 * ============================================================================
 */

#ifndef STORAGE_CONTROLLER_HPP
#define STORAGE_CONTROLLER_HPP

#include "storage_core.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace stor {
namespace cpp {

/* ========================================================================== */
/* SECTION 1: ERROR MAPPING                                                   */
/* ========================================================================== */

enum class Error : int {
    Ok           = STOR_OK,
    Generic      = STOR_ERR_GENERIC,
    Timeout      = STOR_ERR_TIMEOUT,
    Invalid      = STOR_ERR_INVAL,
    NoMem        = STOR_ERR_NOMEM,
    Busy         = STOR_ERR_BUSY,
    NotSupported = STOR_ERR_NOT_SUPPORTED,
    Protocol     = STOR_ERR_PROTOCOL,
    NoDevice     = STOR_ERR_NO_DEVICE,
    Io           = STOR_ERR_IO,
    Checksum     = STOR_ERR_CHECKSUM,
    NotReady     = STOR_ERR_NOT_READY,
    Aborted      = STOR_ERR_ABORTED,
    Range        = STOR_ERR_RANGE,
    Perm         = STOR_ERR_PERM,
};

constexpr Error to_error(int rc) noexcept
{
    switch (rc) {
    case STOR_OK:                return Error::Ok;
    case STOR_ERR_GENERIC:       return Error::Generic;
    case STOR_ERR_TIMEOUT:       return Error::Timeout;
    case STOR_ERR_INVAL:         return Error::Invalid;
    case STOR_ERR_NOMEM:         return Error::NoMem;
    case STOR_ERR_BUSY:          return Error::Busy;
    case STOR_ERR_NOT_SUPPORTED: return Error::NotSupported;
    case STOR_ERR_PROTOCOL:      return Error::Protocol;
    case STOR_ERR_NO_DEVICE:     return Error::NoDevice;
    case STOR_ERR_IO:            return Error::Io;
    case STOR_ERR_CHECKSUM:      return Error::Checksum;
    case STOR_ERR_NOT_READY:     return Error::NotReady;
    case STOR_ERR_ABORTED:       return Error::Aborted;
    case STOR_ERR_RANGE:         return Error::Range;
    case STOR_ERR_PERM:          return Error::Perm;
    default:                     return Error::Generic;
    }
}

constexpr std::string_view to_string(Error e) noexcept
{
    switch (e) {
    case Error::Ok:           return "OK";
    case Error::Generic:      return "GENERIC";
    case Error::Timeout:      return "TIMEOUT";
    case Error::Invalid:      return "INVALID";
    case Error::NoMem:        return "NOMEM";
    case Error::Busy:         return "BUSY";
    case Error::NotSupported: return "NOT_SUPPORTED";
    case Error::Protocol:     return "PROTOCOL";
    case Error::NoDevice:     return "NO_DEVICE";
    case Error::Io:           return "IO";
    case Error::Checksum:     return "CHECKSUM";
    case Error::NotReady:     return "NOT_READY";
    case Error::Aborted:      return "ABORTED";
    case Error::Range:        return "RANGE";
    case Error::Perm:         return "PERM";
    }
    return "UNKNOWN";
}

/* ========================================================================== */
/* SECTION 2: Result<T> (no exceptions)                                       */
/* ========================================================================== */

template <typename T>
class [[nodiscard]] Result {
    union {
        T     value_;
        Error error_;
    };
    bool ok_;

public:
    Result(T v) : value_(std::move(v)), ok_(true) {}
    Result(Error e) : error_(e), ok_(false) {}

    Result(const Result& other) : ok_(other.ok_) {
        if (ok_) new (&value_) T(other.value_);
        else     error_ = other.error_;
    }
    Result(Result&& other) noexcept : ok_(other.ok_) {
        if (ok_) new (&value_) T(std::move(other.value_));
        else     error_ = other.error_;
    }

    Result& operator=(const Result& other) {
        if (this == &other) return *this;
        this->~Result();
        ok_ = other.ok_;
        if (ok_) new (&value_) T(other.value_);
        else     error_ = other.error_;
        return *this;
    }
    Result& operator=(Result&& other) noexcept {
        if (this == &other) return *this;
        this->~Result();
        ok_ = other.ok_;
        if (ok_) new (&value_) T(std::move(other.value_));
        else     error_ = other.error_;
        return *this;
    }

    ~Result() { if (ok_) value_.~T(); }

    bool is_ok()  const noexcept { return ok_; }
    bool is_err() const noexcept { return !ok_; }
    explicit operator bool() const noexcept { return ok_; }

    T&       value()       &  noexcept { return value_; }
    const T& value() const &  noexcept { return value_; }
    T&&      value()       && noexcept { return std::move(value_); }

    Error error() const noexcept { return ok_ ? Error::Ok : error_; }
};

template <>
class [[nodiscard]] Result<void> {
    Error error_;
public:
    Result()          noexcept : error_(Error::Ok) {}
    Result(Error e)   noexcept : error_(e) {}
    bool is_ok()  const noexcept { return error_ == Error::Ok; }
    bool is_err() const noexcept { return error_ != Error::Ok; }
    explicit operator bool() const noexcept { return is_ok(); }
    Error error() const noexcept { return error_; }
};

/* ========================================================================== */
/* SECTION 3: GEOMETRY / HEALTH SNAPSHOTS                                     */
/* ========================================================================== */

struct Geometry {
    std::uint64_t capacity_sectors;
    std::uint32_t sector_size;
    std::uint32_t physical_block_size;
    std::uint32_t logical_block_size;
    std::uint32_t max_transfer_sectors;
    std::uint32_t discard_granularity;
    std::uint32_t queue_depth;
    bool          rotational;
    bool          removable;
    bool          write_cache;

    static Geometry from(const stor_blk_geometry_t& g) noexcept {
        return {
            g.capacity_sectors, g.sector_size,
            g.physical_block_size, g.logical_block_size,
            g.max_transfer_sectors, g.discard_granularity,
            g.queue_depth, g.rotational, g.removable, g.write_cache
        };
    }
};

struct Health {
    std::uint8_t  critical_warning;
    std::uint16_t temperature_k;
    std::uint8_t  available_spare;
    std::uint8_t  spare_thresh;
    std::uint8_t  percent_used;
    std::uint64_t data_units_read;
    std::uint64_t data_units_written;
    std::uint64_t host_read_cmds;
    std::uint64_t host_write_cmds;
    std::uint64_t controller_busy_time;
    std::uint64_t power_cycles;
    std::uint64_t power_on_hours;
    std::uint64_t unsafe_shutdowns;
    std::uint64_t media_errors;
    std::uint32_t num_err_log_entries;
    bool          valid;

    static Health from(const stor_blk_health_t& h) noexcept {
        return {
            h.critical_warning, h.temperature_k,
            h.available_spare, h.spare_thresh, h.percent_used,
            h.data_units_read, h.data_units_written,
            h.host_read_cmds, h.host_write_cmds,
            h.controller_busy_time, h.power_cycles, h.power_on_hours,
            h.unsafe_shutdowns, h.media_errors,
            h.num_err_log_entries, h.valid
        };
    }
};

/* ========================================================================== */
/* SECTION 4: BLOCK DEVICE (non-owning handle, trivially copyable)            */
/* ========================================================================== */

class BlockDevice {
    stor_blk_dev_t *dev_;

public:
    constexpr explicit BlockDevice(stor_blk_dev_t *d = nullptr) noexcept
        : dev_(d) {}

    /* --- Identity --- */
    std::string_view name()   const noexcept {
        return dev_ && dev_->name   ? std::string_view(dev_->name)   : std::string_view{};
    }
    std::string_view serial() const noexcept {
        return dev_ && dev_->serial ? std::string_view(dev_->serial) : std::string_view{};
    }
    std::string_view model()  const noexcept {
        return dev_ && dev_->model  ? std::string_view(dev_->model)  : std::string_view{};
    }
    stor_blk_type_t type()    const noexcept {
        return dev_ ? dev_->type : STOR_BLK_TYPE_UNKNOWN;
    }

    /* --- Geometry --- */
    std::uint64_t capacity_sectors() const noexcept { return dev_ ? dev_->capacity_sectors : 0; }
    std::uint32_t sector_size()      const noexcept { return dev_ ? dev_->sector_size      : 0; }
    std::uint64_t capacity_bytes()   const noexcept { return dev_ ? dev_->capacity_bytes   : 0; }
    std::uint32_t max_transfer_sectors() const noexcept {
        return dev_ ? dev_->max_transfer_sectors : 0;
    }
    std::uint32_t queue_depth() const noexcept { return dev_ ? dev_->queue_depth : 0; }

    bool rotational()      const noexcept { return dev_ && dev_->rotational; }
    bool removable()       const noexcept { return dev_ && dev_->removable; }
    bool trim_supported()  const noexcept { return dev_ && dev_->trim_supported; }
    bool zoned()           const noexcept { return dev_ && dev_->zoned; }

    /* --- Sync I/O --- */
    Result<void> read_sync(std::uint64_t lba, std::uint32_t nsect,
                            void *buf) noexcept {
        if (!dev_ || !buf) return Error::Invalid;
        int rc = stor_blk_read_sync(dev_, lba, nsect, buf);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> write_sync(std::uint64_t lba, std::uint32_t nsect,
                             const void *buf) noexcept {
        if (!dev_ || !buf) return Error::Invalid;
        int rc = stor_blk_write_sync(dev_, lba, nsect, buf);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> flush_sync() noexcept {
        if (!dev_) return Error::Invalid;
        int rc = stor_blk_flush_sync(dev_);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> discard_sync(std::uint64_t lba, std::uint32_t nsect) noexcept {
        if (!dev_) return Error::Invalid;
        int rc = stor_blk_discard_sync(dev_, lba, nsect);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    /* --- Async I/O (caller owns the request storage until completion) --- */
    Result<void> submit(stor_blk_req_t *req) noexcept {
        if (!dev_ || !req) return Error::Invalid;
        int rc = stor_blk_submit(dev_, req);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    std::uint32_t poll(std::uint32_t budget = 16) noexcept {
        if (!dev_) return 0;
        int rc = stor_blk_poll(dev_, budget);
        return rc > 0 ? static_cast<std::uint32_t>(rc) : 0u;
    }

    Result<void> quiesce() noexcept {
        if (!dev_) return Error::Invalid;
        int rc = stor_blk_quiesce(dev_);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> resume() noexcept {
        if (!dev_) return Error::Invalid;
        int rc = stor_blk_resume(dev_);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    /* --- Snapshots --- */
    Result<Geometry> geometry() const noexcept {
        if (!dev_) return Error::Invalid;
        stor_blk_geometry_t g{};
        int rc = stor_blk_geometry(dev_, &g);
        if (rc != STOR_OK) return to_error(rc);
        return Geometry::from(g);
    }

    Result<Health> health() const noexcept {
        if (!dev_) return Error::Invalid;
        stor_blk_health_t h{};
        int rc = stor_blk_health(dev_, &h);
        if (rc != STOR_OK) return to_error(rc);
        return Health::from(h);
    }

    /* --- Raw access --- */
    stor_blk_dev_t *raw() noexcept { return dev_; }
    const stor_blk_dev_t *raw() const noexcept { return dev_; }
    explicit operator bool() const noexcept { return dev_ != nullptr; }
};

/* ========================================================================== */
/* SECTION 5: ASYNC COMPLETION BRIDGE                                         */
/* ========================================================================== */

struct IoCompletionContext {
    void (*fn)(void *user, int status) noexcept;
    void  *user;
};

extern "C" void stor_cpp_io_trampoline(stor_blk_req_t *req) noexcept;

/* Convenience fill helpers (caller supplies stor_blk_req_t storage). */
inline void fill_read_req(stor_blk_req_t& req,
                           std::uint64_t lba, std::uint32_t nsect,
                           void *buf, std::uint32_t sector_size,
                           IoCompletionContext& ctx) noexcept {
    std::memset(&req, 0, sizeof(req));
    req.op       = STOR_BLK_READ;
    req.lba      = lba;
    req.nsect    = nsect;
    req.buf      = buf;
    req.buf_len  = nsect * sector_size;
    req.cookie   = &ctx;
    req.complete = stor_cpp_io_trampoline;
}

inline void fill_write_req(stor_blk_req_t& req,
                            std::uint64_t lba, std::uint32_t nsect,
                            const void *buf, std::uint32_t sector_size,
                            IoCompletionContext& ctx) noexcept {
    std::memset(&req, 0, sizeof(req));
    req.op       = STOR_BLK_WRITE;
    req.lba      = lba;
    req.nsect    = nsect;
    req.buf      = const_cast<void *>(buf);
    req.buf_len  = nsect * sector_size;
    req.cookie   = &ctx;
    req.complete = stor_cpp_io_trampoline;
}

/* ========================================================================== */
/* SECTION 6: NVME CONTROLLER (RAII owner)                                    */
/* ========================================================================== */

class NvmeController {
    stor_nvme_ctrl_t ctrl_{};
    bool owns_ = false;

public:
    NvmeController() noexcept = default;

    ~NvmeController() noexcept {
        if (owns_) {
            (void)stor_nvme_delete_io_queues(&ctrl_);
            (void)stor_nvme_shutdown(&ctrl_, true);
            (void)stor_nvme_disable(&ctrl_);
        }
    }

    NvmeController(const NvmeController&) = delete;
    NvmeController& operator=(const NvmeController&) = delete;

    NvmeController(NvmeController&& other) noexcept : ctrl_(other.ctrl_), owns_(other.owns_) {
        other.owns_ = false;
        std::memset(&other.ctrl_, 0, sizeof(other.ctrl_));
    }
    NvmeController& operator=(NvmeController&& other) noexcept {
        if (this == &other) return *this;
        this->~NvmeController();
        ctrl_ = other.ctrl_;
        owns_ = other.owns_;
        other.owns_ = false;
        std::memset(&other.ctrl_, 0, sizeof(other.ctrl_));
        return *this;
    }

    Result<void> bring_up(stor_pci_device_t pci,
                          std::uint16_t io_queues = 4,
                          std::uint16_t io_depth  = 128) noexcept {
        if (owns_) return Error::Busy;
        int rc = stor_nvme_probe(&pci, &ctrl_);
        if (rc != STOR_OK) return to_error(rc);
        rc = stor_nvme_init(&ctrl_);
        if (rc != STOR_OK) return to_error(rc);
        rc = stor_nvme_create_io_queues(&ctrl_, io_queues, io_depth);
        if (rc != STOR_OK) return to_error(rc);
        rc = stor_nvme_scan_namespaces(&ctrl_);
        if (rc != STOR_OK) return to_error(rc);

        for (std::uint32_t i = 0; i < ctrl_.namespace_count; ++i) {
            if (ctrl_.ns[i]) (void)stor_nvme_zns_probe_ns(ctrl_.ns[i]);
        }
        owns_ = true;
        return {};
    }

    Result<void> shutdown(bool notify = true) noexcept {
        if (!owns_) return Error::Invalid;
        int rc = stor_nvme_shutdown(&ctrl_, notify);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> smart_log(stor_nvme_smart_log_t& out) noexcept {
        if (!owns_) return Error::Invalid;
        int rc = stor_nvme_get_smart(&ctrl_, &out);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> error_log(stor_nvme_error_log_entry_t *out, std::uint32_t n) noexcept {
        if (!owns_ || !out) return Error::Invalid;
        int rc = stor_nvme_get_error_log(&ctrl_, n, out);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    Result<void> fw_slots(stor_nvme_fw_slot_log_t& out) noexcept {
        if (!owns_) return Error::Invalid;
        int rc = stor_nvme_get_fw_slots(&ctrl_, &out);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    /* --- Namespace access --- */
    stor_nvme_ctrl_t *raw() noexcept { return owns_ ? &ctrl_ : nullptr; }
    const stor_nvme_ctrl_t *raw() const noexcept { return owns_ ? &ctrl_ : nullptr; }
    bool ready() const noexcept { return owns_; }

    std::span<stor_nvme_ns_t *const> namespaces() const noexcept {
        if (!owns_) return {};
        return { ctrl_.ns, ctrl_.namespace_count };
    }

    BlockDevice namespace_device(std::uint32_t nsid) noexcept {
        if (!owns_) return BlockDevice{};
        for (std::uint32_t i = 0; i < ctrl_.namespace_count; ++i) {
            if (ctrl_.ns[i] && ctrl_.ns[i]->nsid == nsid)
                return BlockDevice(&ctrl_.ns[i]->blk);
        }
        return BlockDevice{};
    }
};

/* ========================================================================== */
/* SECTION 7: AHCI CONTROLLER (RAII owner)                                    */
/* ========================================================================== */

class AhciController {
    stor_ahci_hba_t hba_{};
    bool owns_ = false;

public:
    AhciController() noexcept = default;

    ~AhciController() noexcept {
        if (owns_) {
            (void)stor_ahci_shutdown(&hba_);
        }
    }

    AhciController(const AhciController&) = delete;
    AhciController& operator=(const AhciController&) = delete;

    AhciController(AhciController&& other) noexcept : hba_(other.hba_), owns_(other.owns_) {
        other.owns_ = false;
        std::memset(&other.hba_, 0, sizeof(other.hba_));
    }
    AhciController& operator=(AhciController&& other) noexcept {
        if (this == &other) return *this;
        this->~AhciController();
        hba_ = other.hba_;
        owns_ = other.owns_;
        other.owns_ = false;
        std::memset(&other.hba_, 0, sizeof(other.hba_));
        return *this;
    }

    Result<void> bring_up(stor_pci_device_t pci) noexcept {
        if (owns_) return Error::Busy;
        int rc = stor_ahci_probe(&pci, &hba_);
        if (rc != STOR_OK) return to_error(rc);
        rc = stor_ahci_init(&hba_);
        if (rc != STOR_OK) return to_error(rc);

        /* Attach block devices for each initialized port. */
        for (std::uint32_t i = 0; i < AHCI_MAX_PORTS; ++i) {
            if (hba_.ports[i].ready) {
                (void)stor_ahci_attach_blk(&hba_, i);
            }
        }
        owns_ = true;
        return {};
    }

    Result<void> shutdown() noexcept {
        if (!owns_) return Error::Invalid;
        int rc = stor_ahci_shutdown(&hba_);
        return rc == STOR_OK ? Result<void>{} : Result<void>{to_error(rc)};
    }

    stor_ahci_hba_t *raw() noexcept { return owns_ ? &hba_ : nullptr; }
    const stor_ahci_hba_t *raw() const noexcept { return owns_ ? &hba_ : nullptr; }
    bool ready() const noexcept { return owns_; }

    /* --- Ports / devices --- */
    std::uint32_t port_count() const noexcept {
        return owns_ ? hba_.n_ports_present : 0;
    }

    BlockDevice port_device(std::uint32_t port) noexcept {
        if (!owns_ || port >= AHCI_MAX_PORTS) return BlockDevice{};
        if (!hba_.ports[port].ready) return BlockDevice{};
        return BlockDevice(&hba_.ports[port].blk);
    }
};

/* ========================================================================== */
/* SECTION 8: AAC CONTROLLER (RAII owner)                                     */
/* ========================================================================== */

class AacController {
    stor_aac_hba_t hba_{};
    bool owns_ = false;

public:
    AacController() noexcept = default;

    ~AacController() noexcept {
        if (owns_) (void)stor_aac_shutdown(&hba_);
    }

    AacController(const AacController&) = delete;
    AacController& operator=(const AacController&) = delete;

    AacController(AacController&& other) noexcept : hba_(other.hba_), owns_(other.owns_) {
        other.owns_ = false;
        std::memset(&other.hba_, 0, sizeof(other.hba_));
    }
    AacController& operator=(AacController&& other) noexcept {
        if (this == &other) return *this;
        this->~AacController();
        hba_ = other.hba_;
        owns_ = other.owns_;
        other.owns_ = false;
        std::memset(&other.hba_, 0, sizeof(other.hba_));
        return *this;
    }

    Result<void> bring_up(stor_pci_device_t pci) noexcept {
        if (owns_) return Error::Busy;
        int rc = stor_aac_probe(&pci, &hba_);
        if (rc != STOR_OK) return to_error(rc);
        rc = stor_aac_init(&hba_);
        if (rc != STOR_OK) return to_error(rc);
        rc = stor_aac_scan_devices(&hba_);
        if (rc != STOR_OK) return to_error(rc);

        for (std::uint32_t i = 0; i < hba_.ld_count; ++i) {
            (void)stor_aac_attach_blk(&hba_, i);
        }
        owns_ = true;
        return {};
    }

    Result<void> shutdown() noexcept {
        if (!owns_) return Error::Invalid;
        /* No public stor_aac_shutdown in header; stub for now. */
        owns_ = false;
        return {};
    }

    stor_aac_hba_t *raw() noexcept { return owns_ ? &hba_ : nullptr; }
    const stor_aac_hba_t *raw() const noexcept { return owns_ ? &hba_ : nullptr; }
    bool ready() const noexcept { return owns_; }

    std::uint32_t ld_count() const noexcept { return owns_ ? hba_.ld_count : 0; }

    BlockDevice ld_device(std::uint32_t idx) noexcept {
        if (!owns_ || idx >= hba_.ld_count) return BlockDevice{};
        return BlockDevice(&hba_.lds[idx]);
    }
};

/* ========================================================================== */
/* SECTION 9: DEVICE RANGE / ITERATORS                                        */
/* ========================================================================== */

class DeviceRange {
    const BlockDevice *begin_;
    const BlockDevice *end_;
public:
    constexpr DeviceRange(const BlockDevice *b, const BlockDevice *e) noexcept
        : begin_(b), end_(e) {}
    constexpr const BlockDevice *begin() const noexcept { return begin_; }
    constexpr const BlockDevice *end() const noexcept { return end_; }
    std::size_t size() const noexcept { return static_cast<std::size_t>(end_ - begin_); }
    BlockDevice operator[](std::size_t i) const noexcept { return begin_[i]; }
};

/* ========================================================================== */
/* SECTION 10: STORAGE MANAGER (orchestration)                                */
/* ========================================================================== */

class StorageManager {
public:
    struct Config {
        std::uint16_t nvme_io_queues = 4;
        std::uint16_t nvme_io_depth  = 128;
        bool          bring_up_nvme  = true;
        bool          bring_up_ahci  = true;
        bool          bring_up_aac   = true;
        std::size_t   max_devices    = 64;
    };

    StorageManager() noexcept = default;
    ~StorageManager() noexcept = default;

    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;

    StorageManager(StorageManager&&) noexcept = default;
    StorageManager& operator=(StorageManager&&) noexcept = default;

    /* --- Bring-up --- */
    Result<void> bring_up(const Config& cfg = {}) noexcept {
        if (!stor_pci_ready()) return Error::NotReady;

        cfg_ = cfg;
        device_count_ = 0;
        brought_up_ = false;

        if (cfg_.bring_up_nvme) (void)bring_up_nvme_once();
        if (cfg_.bring_up_ahci) (void)bring_up_ahci_once();
        if (cfg_.bring_up_aac)  (void)bring_up_aac_once();

        brought_up_ = (device_count_ > 0);
        return brought_up_ ? Result<void>{} : Result<void>{Error::NoDevice};
    }

    Result<void> shutdown() noexcept {
        nvme_ = NvmeController{};
        ahci_ = AhciController{};
        aac_  = AacController{};
        device_count_ = 0;
        brought_up_ = false;
        return {};
    }

    /* --- Controller access --- */
    NvmeController& nvme() noexcept { return nvme_; }
    AhciController& ahci() noexcept { return ahci_; }
    AacController&  aac()  noexcept { return aac_; }

    const NvmeController& nvme() const noexcept { return nvme_; }
    const AhciController& ahci() const noexcept { return ahci_; }
    const AacController&  aac()  const noexcept { return aac_; }

    /* --- Device iteration --- */
    DeviceRange devices() const noexcept {
        return DeviceRange(devices_.data(), devices_.data() + device_count_);
    }

    std::size_t device_count() const noexcept { return device_count_; }

    BlockDevice *find(std::string_view name) noexcept {
        for (std::size_t i = 0; i < device_count_; ++i) {
            if (devices_[i].name() == name) return &devices_[i];
        }
        return nullptr;
    }

    /* --- Callbacks (raw fn pointers to avoid allocation) --- */
    void set_on_device_add(void (*fn)(void*, BlockDevice) noexcept,
                            void *ctx = nullptr) noexcept {
        on_device_add_     = fn;
        on_device_add_ctx_ = ctx;
    }

    void set_on_device_remove(void (*fn)(void*, BlockDevice) noexcept,
                               void *ctx = nullptr) noexcept {
        on_device_remove_     = fn;
        on_device_remove_ctx_ = ctx;
    }

    void set_on_error(void (*fn)(void*, Error, std::string_view) noexcept,
                       void *ctx = nullptr) noexcept {
        on_error_     = fn;
        on_error_ctx_ = ctx;
    }

    /* --- Periodic maintenance --- */
    std::uint32_t poll_all(std::uint32_t budget_each = 16) noexcept {
        std::uint32_t total = 0;
        for (std::size_t i = 0; i < device_count_; ++i)
            total += devices_[i].poll(budget_each);
        return total;
    }

    Result<void> health_check_all(Health *out, std::size_t cap,
                                    std::size_t *got = nullptr) noexcept {
        if (got) *got = 0;
        if (!out || cap == 0) return Error::Invalid;
        std::size_t n = 0;
        for (std::size_t i = 0; i < device_count_ && n < cap; ++i) {
            auto r = devices_[i].health();
            if (r.is_ok()) {
                out[n++] = r.value();
            } else {
                Health h{};
                h.valid = false;
                out[n++] = h;
            }
        }
        if (got) *got = n;
        return {};
    }

private:
    void add_device(BlockDevice dev) noexcept {
        if (!dev || device_count_ >= cfg_.max_devices) return;
        devices_[device_count_++] = dev;
        if (on_device_add_) on_device_add_(on_device_add_ctx_, dev);
    }

    Result<void> bring_up_nvme_once() noexcept {
        std::array<stor_pci_device_t, 8> list{};
        std::uint32_t found = 0;
        int rc = stor_pci_enumerate(STOR_PCI_CLASS_STORAGE,
                                      STOR_PCI_SUBCLASS_NVM,
                                      STOR_PCI_PROGIF_NVME,
                                      list.data(), (std::uint32_t)list.size(),
                                      &found);
        if (rc != STOR_OK || found == 0) return Error::NoDevice;

        for (std::uint32_t i = 0; i < found; ++i) {
            NvmeController tmp;
            if (tmp.bring_up(list[i], cfg_.nvme_io_queues,
                              cfg_.nvme_io_depth).is_err())
                continue;
            nvme_ = std::move(tmp);

            for (auto *ns : nvme_.namespaces()) {
                if (ns) add_device(BlockDevice(&ns->blk));
            }
            return {};
        }
        return Error::NoDevice;
    }

    Result<void> bring_up_ahci_once() noexcept {
        std::array<stor_pci_device_t, 8> list{};
        std::uint32_t found = 0;
        int rc = stor_pci_enumerate(STOR_PCI_CLASS_STORAGE,
                                      STOR_PCI_SUBCLASS_SATA,
                                      STOR_PCI_PROGIF_AHCI,
                                      list.data(), (std::uint32_t)list.size(),
                                      &found);
        if (rc != STOR_OK || found == 0) return Error::NoDevice;

        for (std::uint32_t i = 0; i < found; ++i) {
            AhciController tmp;
            if (tmp.bring_up(list[i]).is_err()) continue;
            ahci_ = std::move(tmp);

            for (std::uint32_t p = 0; p < AHCI_MAX_PORTS; ++p) {
                BlockDevice d = ahci_.port_device(p);
                if (d) add_device(d);
            }
            return {};
        }
        return Error::NoDevice;
    }

    Result<void> bring_up_aac_once() noexcept {
        std::array<stor_pci_device_t, 8> list{};
        std::uint32_t found = 0;
        int rc = stor_pci_enumerate(STOR_PCI_CLASS_STORAGE,
                                      STOR_PCI_SUBCLASS_RAID,
                                      0xFF,
                                      list.data(), (std::uint32_t)list.size(),
                                      &found);
        if (rc != STOR_OK || found == 0) return Error::NoDevice;

        for (std::uint32_t i = 0; i < found; ++i) {
            AacController tmp;
            if (tmp.bring_up(list[i]).is_err()) continue;
            aac_ = std::move(tmp);

            for (std::uint32_t ld = 0; ld < aac_.ld_count(); ++ld) {
                BlockDevice d = aac_.ld_device(ld);
                if (d) add_device(d);
            }
            return {};
        }
        return Error::NoDevice;
    }

    NvmeController nvme_{};
    AhciController ahci_{};
    AacController  aac_{};
    Config         cfg_{};
    bool           brought_up_ = false;

    std::array<BlockDevice, 64> devices_{};
    std::size_t                 device_count_ = 0;

    void (*on_device_add_)(void*, BlockDevice) noexcept = nullptr;
    void  *on_device_add_ctx_ = nullptr;
    void (*on_device_remove_)(void*, BlockDevice) noexcept = nullptr;
    void  *on_device_remove_ctx_ = nullptr;
    void (*on_error_)(void*, Error, std::string_view) noexcept = nullptr;
    void  *on_error_ctx_ = nullptr;
};

/* ========================================================================== */
/* SECTION 11: INIT HELPERS                                                   */
/* ========================================================================== */

inline void set_logger(stor_log_fn_t fn) noexcept {
    stor_log_fn = fn;
}

inline bool pci_ready() noexcept {
    return stor_pci_ready();
}

} /* namespace cpp */
} /* namespace stor */

#endif /* STORAGE_CONTROLLER_HPP */
