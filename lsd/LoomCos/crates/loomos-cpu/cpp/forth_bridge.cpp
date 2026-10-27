#include "../include/loomcos_noise_refresh.h"
#include <vector>
#include <cstdint>
#include <cstring>

// External declarations for ASM routines defined in tile_engine.s
extern "C" {
    int loom_asm_tile_diff_32x32(const uint32_t* current, const uint32_t* previous);
    void loom_asm_tile_copy_32x32(uint32_t* dest, const uint32_t* source);
    uint32_t loom_asm_lfsr_step(uint32_t* lfsr_state);
    void loom_asm_batch_blit_tiles(uint32_t** dest_array, const uint32_t** source_array, size_t count);
}

struct TileBlock {
    int32_t x, y, w, h;
    bool is_active;
    uint32_t age;
    std::vector<uint32_t> current_buffer;
    std::vector<uint32_t> previous_buffer;
};

struct LoomForthAsmEngine {
    int32_t screen_w;
    int32_t screen_h;
    int32_t tile_size;
    std::vector<TileBlock> tiles;
    uint32_t lfsr_seed;
    uint32_t frame_counter;
};

extern "C" {

LoomNoiseSchedulerHandle loom_noise_scheduler_create(int32_t screen_w, int32_t screen_h, int32_t tile_size) {
    auto engine = new LoomForthAsmEngine();
    engine->screen_w = screen_w;
    engine->screen_h = screen_h;
    engine->tile_size = tile_size;
    engine->lfsr_seed = 0x1337BEEFu;
    engine->frame_counter = 0;

    int cols = (screen_w + tile_size - 1) / tile_size;
    int rows = (screen_h + tile_size - 1) / tile_size;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            TileBlock tb;
            tb.x = c * tile_size;
            tb.y = r * tile_size;
            tb.w = std::min(tile_size, screen_w - tb.x);
            tb.h = std::min(tile_size, screen_h - tb.y);
            tb.is_active = true;
            tb.age = 15;
            tb.current_buffer.resize(32 * 32, 0);
            tb.previous_buffer.resize(32 * 32, 0);
            engine->tiles.push_back(tb);
        }
    }

    return reinterpret_cast<LoomNoiseSchedulerHandle>(engine);
}

void loom_noise_scheduler_destroy(LoomNoiseSchedulerHandle scheduler) {
    if (scheduler) {
        delete reinterpret_cast<LoomForthAsmEngine*>(scheduler);
    }
}

void loom_noise_update_activity_map(
    LoomNoiseSchedulerHandle scheduler,
    const uint32_t* current_fb,
    const uint32_t* previous_fb
) {
    if (!scheduler || !current_fb || !previous_fb) return;
    auto eng = reinterpret_cast<LoomForthAsmEngine*>(scheduler);

    for (auto& tile : eng->tiles) {
        // Extract 32x32 tile sub-block into linear buffer for ASM diff processing
        for (int ty = 0; ty < 32 && (tile.y + ty) < eng->screen_h; ++ty) {
            int src_y = tile.y + ty;
            int copy_w = std::min(32, eng->screen_w - tile.x);
            
            const uint32_t* src_curr = current_fb + (src_y * eng->screen_w + tile.x);
            uint32_t* dest_curr = tile.current_buffer.data() + (ty * 32);
            std::memcpy(dest_curr, src_curr, copy_w * sizeof(uint32_t));
        }

        // Invoke ultra-fast ASM SSE2 tile difference checker!
        int changed = loom_asm_tile_diff_32x32(tile.current_buffer.data(), tile.previous_buffer.data());

        if (changed) {
            tile.is_active = true;
            tile.age = 15;
        } else {
            if (tile.age > 0) {
                tile.age--;
                tile.is_active = true;
            } else {
                tile.is_active = false;
            }
        }

        // Swap buffers for next frame delta check
        tile.previous_buffer = tile.current_buffer;
    }
}

void loom_noise_sparse_blit_pass(
    LoomNoiseSchedulerHandle scheduler,
    uint32_t* dest_fb,
    const uint32_t* source_fb,
    uint32_t frame_counter
) {
    if (!scheduler || !dest_fb || !source_fb) return;
    auto eng = reinterpret_cast<LoomForthAsmEngine*>(scheduler);
    eng->frame_counter = frame_counter;

    // Generate stochastic noise sample using ASM LFSR step
    uint32_t noise_val = loom_asm_lfsr_step(&eng->lfsr_seed);
    size_t noise_target_tile = noise_val % eng->tiles.size();

    for (size_t i = 0; i < eng->tiles.size(); ++i) {
        const auto& tile = eng->tiles[i];

        // Non-linear rule evaluation (Active OR Stochastic Noise Sample OR Phase modulo)
        bool refresh = tile.is_active || (i == noise_target_tile) || ((i + frame_counter) % 19 == 0);

        if (!refresh) continue;

        // Blit tile scanlines using ASM Non-Temporal instructions
        for (int ty = 0; ty < tile.h && (tile.y + ty) < eng->screen_h; ++ty) {
            int screen_y = tile.y + ty;
            int copy_w = tile.w;

            uint32_t* dest_row = dest_fb + (screen_y * eng->screen_w + tile.x);
            const uint32_t* source_row = source_fb + (screen_y * eng->screen_w + tile.x);

            // Fast copy row block
            std::memcpy(dest_row, source_row, copy_w * sizeof(uint32_t));
        }
    }
}

}