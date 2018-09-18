#pragma once

#include <cstdint>

// Commonly used internal types and helper functions for the Shufflizer mod.
namespace mod::shufflizer {
    
// Represents a set of 16 consecutive bits from TTYD's "GSWF" bits.
struct SavedFlagRow {
    uint16_t start_offset;
    uint16_t bits;
};

// Parameters for mapping between vanilla enemy stats and balanced Pit stats.
// See the related functions for a further explanation.
struct PitBalanceParameters {
    // Max level factor * min stat at min level
    uint8_t levf_x_minl_minstat;
    // Min stat at max level - min stat at min level
    uint8_t diffl_minstat;
    // Max level factor * (max - min stat at min level)
    uint8_t levf_x_minl_diffstat;
    // (Max - min stat at max level) - (Max - min stat at min level)
    uint8_t diff_maxl_minl_diffstat;
};

// Internal randomization function, so Shufflizer setup doesn't interfere 
// with vanilla TTYD's RNG.
void ShufflizerSeedRand(uint32_t seed);
int32_t ShufflizerRand(int32_t range);

// Shuffles a range of objects with uniform probability.
template <typename T>
void ShuffleRange(T* start, T* end);

// Converts an array of bits from `start` to `end` to an array of item IDs
// starting at `output`, and returns the end of the array.
// (It is assumed that you own sufficient memory to store the entire array).
// The Nth bit in the bitfield corresponds to item ID `item_offset` + N.
int16_t* CreateItemArrayFromBitfield(
    const uint32_t* start, const uint32_t* end, int16_t* output,
    int32_t item_offset);
    
// Returns the item represented by a randomly selected bit in a bitfield
// plus the given ID offset.
int16_t GetRandomItemFromBitfield(
    const uint32_t* start, const uint32_t* end, int32_t item_offset);
    
// Returns the ordered index of the given `flag` in an array of SavedFlagRows
// (which should represent the flag indices of all shuffled 'field items').
// If the flag is not present in the array, returns -1.
int32_t GetFieldItemIndexFromSavedFlag(
    const SavedFlagRow* start, const SavedFlagRow* end, int32_t flag);
    
// Returns the "level factor", i.e. an indicator of relative nonlinear strength,
// given the (zero-indexed) floor a Pit enemy appears on / should appear on.
// The minimum level factor is 0, and the maximum (for the 90's) is 20.
int32_t GetLevelFactorFromPitFloor(int32_t floor);
    
// Returns the balanced version of a stat based on the start and end level
// factors and the stat's balance parameters.
int32_t ApplyPitBalanceParameters(
    int32_t stat, int32_t start_lvf, int32_t end_lvf,
    PitBalanceParameters params);
    
// Get NPC type to spawn based on the (zero-indexed) floor it would spawn on.
// Return value unspecified for treasure floors or floors out of range.
int32_t GetPitNpcTypeFromFloor(int32_t floor);
    
//// IMPLEMENTATION DETAILS

template <typename T>
void ShuffleRange(T* start, T* end) {
    for (T* first = start; end - first > 1; ++first) {
        int32_t offset = ShufflizerRand(end - first);
        T tmp = *first;
        *first = *(first + offset);
        *(first + offset) = tmp;
    }
}
    
}