#pragma once

#include <cstdint>

namespace mod::shufflizer {

struct ShufflizerOptions {
    int32_t enemy_hp_modifier  = 100;   // Percentage multiplier for enemy HP.
    int32_t enemy_atk_modifier = 100;   // Percentage multiplier for enemy ATK.
    int32_t enemy_def_modifier = 0;     // A constant value added to enemy DEF.
    bool    shuffle_pit_floors = true;  // Enables shuffling the Pit's floors.
};

}