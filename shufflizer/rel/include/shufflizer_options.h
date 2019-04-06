#pragma once

#include <cstdint>

namespace mod::shufflizer {

struct ShufflizerOptions {
    int32_t enemy_hp_modifier  = 100;   // Percentage multiplier for enemy HP.
    int32_t enemy_atk_modifier = 100;   // Percentage multiplier for enemy ATK.
    int32_t enemy_def_modifier = 0;     // A constant value added to enemy DEF.
    int32_t superguard_cost    = 0;     // Amount of SP to deduct on Superguard.
    bool    shuffle_misc_items = true;  // Shuffle Turtley Leaf, Hot Sauce, etc.
    bool    shuffle_pit_floors = true;  // Enables shuffling the Pit's floors.
    bool    charlieton_fixed   = true;  // Make Pit Charlieton's inventory fixed.
    bool    select_move_power  = true;  // Select power of stacked move badges.
};

}