/*
 * CEA-861 Extension Block Parser (HDMI/DisplayPort extensions)
 */

#include "base/cotan_shim.h"
#include "base/display.h"

#define CEA_AUDIO_DB  1
#define CEA_VIDEO_DB  2
#define CEA_VENDOR_DB 3

int edid_parse_cea_extension(const uint8_t *edid_data, size_t total_len) {
    uint8_t checksum = 0;
    uint8_t dtd_offset;
    size_t i, idx;

    if (total_len < 256) return 0;

    const uint8_t *cea_block = &edid_data[128];

    if (cea_block[0] != 0x02) {
        printf("[edid] Not a CEA-861 extension block (tag=%02x)\n", cea_block[0]);
        return 0;
    }

    for (i = 128; i < 256; i++) checksum += edid_data[i];
    if (checksum != 0) {
        printf("[edid] CEA extension checksum mismatch\n");
        return -1;
    }

    dtd_offset = cea_block[2];
    if (dtd_offset == 0) return 0;

    printf("[edid] CEA-861 extension found. DTD offset: %u\n", dtd_offset);

    idx = 4;
    while (idx < dtd_offset && idx < 128) {
        uint8_t header = cea_block[idx];
        uint8_t tag = (header >> 5) & 0x07;
        uint8_t len = header & 0x1F;

        if (idx + 1 + len > dtd_offset) break;

        if (tag == CEA_VIDEO_DB) {
            printf("[edid]   -> Video Data Block found (len=%u)\n", len);
        } else if (tag == CEA_AUDIO_DB) {
            printf("[edid]   -> Audio Data Block found (len=%u)\n", len);
        } else if (tag == CEA_VENDOR_DB && len >= 5) {
            if (cea_block[idx+1] == 0x03 && cea_block[idx+2] == 0x0c && cea_block[idx+3] == 0x00) {
                printf("[edid]   -> HDMI Vendor Specific Data Block detected!\n");
            }
        }
        idx += 1 + len;
    }
    return 0;
}
