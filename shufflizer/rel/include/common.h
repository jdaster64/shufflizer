#pragma once

#include <cstdint>
#include <ttyd/common_types.h>

// TTYD symbols and wrapper functions commonly referenced in mod code
// (for instance, player save data, item data, next room to be loaded).
//
// TODO: Add conditional compilation support for other versions.
namespace mod::common {

extern const char* kNextMap;
extern ttyd::common::ItemData* kItemDataArr;
extern ttyd::common::AttackParams* kSuperHammerRecoilParams;
extern ttyd::common::AttackParams* kUltraHammerRecoilParams;
extern ttyd::common::AttackParams* kGulpRecoilParams;
extern ttyd::common::AttackParams* kTradeOffParams;
extern ttyd::common::AttackParams* kKoopaCurseParams;
extern ttyd::common::AttackParams* kHammerThrowParams;
extern ttyd::common::AttackParams* kPiercingBlowParams;
extern ttyd::common::AttackParams* kSuperChargeParams;
extern ttyd::common::AttackParams* kSuperChargePartnerParams;
extern int8_t* kGuardFramesArr;
extern int8_t* kSuperguardFramesArr;

// Arrays of item IDs used to populate shops.
extern int32_t* kDazzleItemArr;
extern int32_t* kCharlietonItemArr;
extern int32_t* kHowzItemArr;
extern int32_t* kPitCharlietonItemArr;
extern int32_t* kPiantaParlorItemArr;

// Miscellaneous offsets.
extern const int32_t kSavedWordFlagBaseValue;
extern const int32_t kPitModuleNpcArrOffset;
extern const int32_t kPitModuleCharlietonChanceOffset;
extern const int32_t kJinModuleAtomicBooItemObjectFlagOffset;
extern const int32_t kJinModuleAtomicBooItemGswfSetOffset;
extern const int32_t kMujModuleFlavioDesiredItemOffset;
extern const int32_t kObjectChestScriptPtrOffset;
extern const int32_t kObjectCollectionExprOffset;
extern const int32_t kUltraHammerChestDespawnIdOffset;

const char* GetCurrentArea();
const char* GetCurrentMap();
const char* GetFilename();
int32_t GetStorySequence();
int8_t GetPitSequence();
bool GetSavedFlagValue(int32_t flag);
bool HasKeyItemInInventory(int16_t item_id);
void* GetBattleWindowDataOffset();
float GetStatusWindowHeight();

}