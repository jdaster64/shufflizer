#include "shufflizer_common.h"

namespace mod::shufflizer {

namespace {

// Using TTYD's same RNG function / seed setup for laziness.
uint32_t ShufflizerRandState = 0;
    
}

void ShufflizerSeedRand(uint32_t seed) {
    ShufflizerRandState = seed;
}

int32_t ShufflizerRand(int32_t range) {
    if (range < 1) return 0;
    ShufflizerRandState = ShufflizerRandState * 0x41c64e6d + 12345;
    return ((ShufflizerRandState >> 16) & 0x7fff) % range;
}

int16_t* CreateItemArrayFromBitfield(
    const uint32_t* start, const uint32_t* end, int16_t* output,
    int32_t item_offset) {
    for (; start < end; ++start) {
        for (int32_t bit = 0; bit < 32; ++bit) {
            if (*start & (1U << bit)) {
                *output = item_offset + bit;
                ++output;
            }
        }
        item_offset += 32;
    }
    return output;
}

int16_t GetRandomItemFromBitfield(
    const uint32_t* start, const uint32_t* end, int32_t item_offset) {
    const uint32_t* cur;
    int32_t num_items_seen = 0;
    for (cur = start; cur < end; ++cur) {
        for (int32_t bit = 0; bit < 32; ++bit) {
            if (*cur & (1U << bit)) ++num_items_seen;
        }
    }
    int32_t index = ShufflizerRand(num_items_seen);
    num_items_seen = 0;
    for (cur = start; cur < end; ++cur) {
        for (int32_t bit = 0; bit < 32; ++bit) {
            if (*cur & (1U << bit)) {
                if (index == num_items_seen) return item_offset + bit;
                ++num_items_seen;
            }
        }
        item_offset += 32;
    }
    // Should not be reached if there are any bits set in the bitfield.
    return -1;
}
    
int32_t GetFieldItemIndexFromSavedFlag(
    const SavedFlagRow* start, const SavedFlagRow* end, int32_t flag) {
    int32_t num_flags_seen = 0;
    for (; start < end; ++start) {
        for (int32_t bit = 0; bit < 16; ++bit) {
            if (start->bits & (1U << bit)) {
                if (flag == start->start_offset + bit) return num_flags_seen;
                ++num_flags_seen;
            }
        }
    }
    // Flag was not found in the bitfield.
    return -1;
}
    
int32_t GetLevelFactorFromPitFloor(int32_t floor) {
    if (floor < 0 or floor >= 100) return 0;  // Shouldn't ever happen.
    int32_t set = floor / 10;
    int8_t level_factors[] = {0, 2, 4, 5, 7, 10, 12, 13, 17, 20};
    return level_factors[set];
}
    
// Returns the balanced version of a stat based on the start and end level
// factors and the stat's balance parameters.
int32_t ApplyPitBalanceParameters(
    int32_t stat, int32_t start_lvf, int32_t end_lvf,
    PitBalanceParameters params) {
    const int32_t levf = 20;
    int32_t A =
        params.levf_x_minl_diffstat + params.diff_maxl_minl_diffstat * end_lvf;
    int32_t B =
        params.levf_x_minl_minstat + params.diffl_minstat * start_lvf;
    int32_t C =
        params.levf_x_minl_diffstat + params.diff_maxl_minl_diffstat * start_lvf;
    int32_t D =
        levf * stat -
        (params.levf_x_minl_minstat + params.diffl_minstat * end_lvf);
    return (A * B + C * D) / (levf * A);
}

int32_t GetPitNpcTypeFromFloor(int32_t floor) {
    if (floor < 90) return floor % 5 + floor / 10 * 5 + 1;
    if (floor < 99) return 46 + (floor - 90) % 4;
    return 51;
}

}