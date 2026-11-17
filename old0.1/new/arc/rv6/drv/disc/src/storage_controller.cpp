/*
 * ============================================================================
 *  STORAGE_CONTROLLER.CPP - glue for storage_controller.hpp
 *  Architecture: RISC-V (RV32 / RV64)
 *
 *  This file contains ONLY the extern "C" trampoline required to bridge
 *  the driver's C-linkage completion callback into a C++ caller-provided
 *  IoCompletionContext. All other C++ logic lives in the header.
 *
 *  Linkage note:
 *    stor_blk_complete_fn_t has C linkage (declared inside extern "C"
 *    block in storage_core.h). A C++ function pointer technically has
 *    undefined behaviour when assigned to it. The trampoline below is
 *    declared extern "C" so it's safe to use from the C side.
 * ============================================================================
 */

#include "storage_controller.hpp"

extern "C" void stor_cpp_io_trampoline(stor_blk_req_t *req) noexcept
{
    if (!req) return;

    auto *ctx = static_cast<stor::cpp::IoCompletionContext *>(req->cookie);
    if (!ctx || !ctx->fn) return;

    ctx->fn(ctx->user, req->status);
}
