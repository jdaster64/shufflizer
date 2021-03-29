#pragma once

#include <cstdint>
#include <ttyd/battle_damage.h>
#include <ttyd/battle_unit.h>
#include <ttyd/OSLink.h>

#include "shufflizer_common.h"
#include "shufflizer_menu.h"
#include "shufflizer_options.h"
#include "shufflizer_state.h"

namespace mod::shufflizer {

class Shufflizer {
public:
    Shufflizer();
    
    // Sets up necessary hooks for Shufflizer code to run.
    void Init();
    // Code that runs every frame; updates RNG and mod UI / input.
    void Update();
    // Code that runs drawing-related code every frame.
    void Draw();
    
private:
    // Initializes the item locations, shop tables, and pit floor order used,
    // seeded by the file's name; called on file load.
    void InitializeShuffleSeed();
    // Runs initialization code specific to individual modules; e.g.
    // setting up shop data, randomizing Pit Charlieton's inventory, etc.
    void OnModuleLoaded(ttyd::oslink::OSModuleInfo* module_info);
    // Runs custom code immediately after the main initialization event
    // has finished executing when loading a room. Should be able to run
    // synchronously in a single frame.
    void AfterRoomInitEvent();
    // Overrides field / chest / block item spawns with shuffled replacements.
    int16_t ReplaceGeneralItem(int16_t id, int32_t collection_expr);
    void ReplaceChestItem(void* object);
    // Balances enemy HP, ATK, DEF, and level based on where they spawn in the
    // Pit, as well as incorporating any modifiers the player applies.
    int32_t AlterDamageCalculation(
        void* attacking_actor_params, void* defending_actor_params,
        void* additional_defender_params,
        ttyd::common::AttackParams* attack_params, void* unk0, uint32_t unk1);
    int32_t AlterFpDamageCalculation(
        void* attacking_actor_params, void* defending_actor_params,
        void* additional_defender_params,
        ttyd::common::AttackParams* attack_params, void* unk0, uint32_t unk1);
    void AlterHpAndLevel(
        ttyd::battle_unit::BattlePartySlotInfo* slot_info);
    // Handles move custom power level selection when in Mario's battle menus.
    void HandleMovePowerLevelSelection();
    
    // Holds the information required to look up the field items, shop items,
    // and Pit floor order specific to the current shuffle seed / save file.
    SeedState seed_state_;
    // Controls togglable options like enemy stat modifiers.
    ShufflizerOptions options_;
    // Handles the UI at startup & UI / input for changing options on the fly.
    ShufflizerMenu menu_;
};

}