#include "loomcos_noise_refresh.h"
#include <vector>
#include <algorithm>
#include <cstring>
#cmath>

struct TileInfo {
    int32_t x, y, w, h;
    uint32_t activity_counter; // Counts frames since last modification
    bool is_active;
};

struct LoomNoiseScheduler {
    int32_t screen_w;
    int32_t screen_h;
    int32_t tile_size;
    int32_t cols;
    int32_t rows;
    std::vector<TileInfo> tiles;
    uint32_t lfsr_state; // Fast pseudo-random noise seed for non-linear scheduling
};

extern "C" {

LoomNoiseSchedulerHandle loom_noise_scheduler_create(int32_t screen_w, int32_t screen_h, int32_t tile_size) {
    if (screen_w <= 0 || screen_h <= 0 || tile_size <= 0) return nullptr;

    auto scheduler = new LoomNoiseScheduler();
    scheduler->screen_w = screen_w;
    scheduler->screen_h = screen_h;
    scheduler->tile_size = tile_size;
    scheduler->cols = (screen_w + tile_size - 1) / tile_size;
    scheduler->rows = (screen_h + tile_size - 1) / tile_size;
    scheduler->lfsr_state = 0xACE1u; // Initial non-zero LFSR seed

    // Generate grid tiles
    for (int r = 0; r < scheduler->rows; ++r) {
        for (int c = 0; c < scheduler->cols; ++c) {
            TileInfo t;
            t.x = c * tile_size;
            t.y = r * tile_size;
            t.w = std::min(tile_size, screen_w - t.x);
            t.h = std::min(tile_size, screen_h - t.y);
            t.activity_counter = 0;
            t.is_active = true;
            scheduler->tiles.push_back(t);
        }
    }

    return reinterpret_cast<LoomNoiseSchedulerHandle>(scheduler);
}

void loom_noise_scheduler_destroy(LoomNoiseSchedulerHandle scheduler) {
    if (scheduler) {
        delete reinterpret_cast<LoomNoiseScheduler*>(scheduler);
    }
}

void loom_noise_update_activity_map(
    LoomNoiseSchedulerHandle scheduler,
    const uint32_t* current_fb,
    const uint32_t* previous_fb
) {
    if (!scheduler || !current_fb || !previous_fb) return;
    auto s = reinterpret_cast<LoomNoiseScheduler*>(scheduler);

    // Check each tile for pixel changes using fast row checks
    for (auto& tile : s->tiles) {
        bool tile_changed = false;

        for (int ty = 0; ty < tile.h; ++ty) {
            int screen_y = tile.y + ty;
            if (screen_y >= s->screen_h) break;

            const uint32_t* cur_row = current_fb + (screen_y * s->screen_w + tile.x);
            const uint32_t* prev_row = previous_fb + (screen_y * s->screen_w + tile.x);

            size_t row_bytes = tile.w * sizeof(uint32_t);
            if (std::memcmp(cur_row, prev_row, row_bytes) != 0) {
                tile_changed = true;
                break;
            }
        }

        if (tile_changed) {
            tile.is_active = true;
            tile.activity_counter = 15; // Keep active for 15 frames after change settles
        } else {
            if (tile.activity_counter > 0) {
                tile.activity_counter--;
                tile.is_active = true; // Still settling down
            } else {
                tile.is_active = false; // Fully dormant, eligible for noise-based sparse refresh
            }
        }
    }
}

void loom_noise_sparse_blit_pass(
    LoomNoiseSchedulerHandle scheduler,
    uint32_t* dest_fb,
    const uint32_t* source_fb,
    uint32_t frame_counter
) {
    if (!scheduler || !dest_fb || !source_fb) return;
    auto s = reinterpret_cast<LoomNoiseScheduler*>(scheduler);

    // Simple fast Galois LFSR step to generate noise sample index
    // x^16 + x^14 + x^13 + x^11 + 1 polynomial shift
    uint32_t bit = ((s->lfsr_state >> 0) ^ (s->lfsr_state >> 2) ^ (s->lfsr_state >> 3) ^ (s->lfsr_state >> 5)) & 1;
    s->lfsr_state = (s->lfsr_state >> 1) | (bit << 15);
    size_t noise_sample_target = s->lfsr_state % s->tiles.size();

    // Iterate through all tiles with non-linear noise scheduling
    for (size_t i = 0; i < s->tiles.size(); ++i) {
        const auto& tile = s->tiles[i];

        // Decision rule: Refresh if tile is active OR if it matches our non-linear noise sample for this frame
        bool should_refresh = tile.is_active || (i == noise_sample_target) || ((i + frame_counter) % 17 == 0);

        if (!should_refresh) {
            continue; // Skip tile completely to save CPU cycles!
        }

        // Blit tile scanlines from source to destination framebuffer
        for (int ty = 0; ty < tile.h; ++ty) {
            int screen_y = tile.y + ty;
            if (screen_y >= s->screen_h) break;

            uint32_t* dest_row = dest_fb + (screen_y * s->screen_w + tile.x);
            const uint32_t* src_row = source_fb + (screen_y * s->screen_w + tile.x);

            std::memcpy(dest_row, src_row, tile.w * sizeof(uint32_t));
        }
    }
}

}