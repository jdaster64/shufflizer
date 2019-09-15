#include "common.h"

// TODO: Add conditional compilation once other regions are supported.
namespace mod::common {
    
using ::ttyd::common::AttackParams;
using ::ttyd::common::ItemData;

constexpr uint32_t r13 = 0x8041CF20;

// Header-accessible
const char* kNextMap            = reinterpret_cast<const char*>(r13 + 0x16A8);
ItemData* kItemDataArr          = reinterpret_cast<ItemData*>(0x803108A8);
AttackParams* 
    kSuperHammerRecoilParams    = reinterpret_cast<AttackParams*>(0x80353F68);
AttackParams*
    kUltraHammerRecoilParams    = reinterpret_cast<AttackParams*>(0x803540E8);
AttackParams* kGulpRecoilParams = reinterpret_cast<AttackParams*>(0x8037CE24);
AttackParams* kTradeOffParams   = reinterpret_cast<AttackParams*>(0x80369B60);
AttackParams* kKoopaCurseParams = reinterpret_cast<AttackParams*>(0x8036AC3C);
AttackParams* kHammerThrowParams
    = reinterpret_cast<AttackParams*>(0x80354C28);
AttackParams* kPiercingBlowParams
    = reinterpret_cast<AttackParams*>(0x80354CE8);
AttackParams* kSuperChargeParams 
    = reinterpret_cast<AttackParams*>(0x80355228);
AttackParams* kSuperChargePartnerParams
    = reinterpret_cast<AttackParams*>(0x803552E8);
int8_t* kGuardFramesArr         = reinterpret_cast<int8_t*>(0x802EE018);
int8_t* kSuperguardFramesArr    = reinterpret_cast<int8_t*>(0x802EE020);
int32_t* kDazzleItemArr         = reinterpret_cast<int32_t*>(0x803AD038);
int32_t* kCharlietonItemArr     = reinterpret_cast<int32_t*>(0x803AD078);
int32_t* kHowzItemArr           = reinterpret_cast<int32_t*>(0x80419D90);
int32_t* kPitCharlietonItemArr  = reinterpret_cast<int32_t*>(0x80419E18);
int32_t* kPiantaParlorItemArr   = reinterpret_cast<int32_t*>(0x80419E34);

const int32_t kSavedWordFlagBaseValue = -130000000;
const int32_t kPitModuleNpcArrOffset  = 0xEF90;
const int32_t kPitModuleCharlietonChanceOffset = 0x11EA4;
const int32_t kMujModuleFlavioDesiredItemOffset = 0x1FA04;
const int32_t kObjectChestScriptPtrOffset = 0x1D8;
const int32_t kObjectCollectionExprOffset = 0x1E4;
const int32_t kUltraHammerChestDespawnIdOffset = 0x3C8E4;

// Save data related offsets.
char** kSaveDataPtr = reinterpret_cast<char**>(r13 - 0x6F50);
const uint32_t kCurrentMapOffset    = 0x012C;
const uint32_t kCurrentAreaOffset   = 0x013C;
const uint32_t kLoadedRelPtrOffset  = 0x0160;
const uint32_t kStorySequenceOffset = 0x0174;
const uint32_t kSavedFlagsOffset    = 0x0178;
const uint32_t kPitSequenceOffset   = 0x0AA1;
const uint32_t kFilenameOffset      = 0x11BC;

// Pouch (Player stats / inventory) related offsets.
char** kPouchPtr = reinterpret_cast<char**>(r13 + 0x1BE0);
const uint32_t kKeyItemsOffset      = 0x00A0;

// Battle related offsets.
char** kBattlePtr = reinterpret_cast<char**>(r13 + 0x1C70);
const uint32_t kBattleMenuOffset    = 0x1C78;

// Status window offsets.
// TODO: Figure out what this structure actually is.
char** kUnknownStatusWindowStructPtr = reinterpret_cast<char**>(r13 + 0x1CCC);
const uint32_t kStatusWindowHeightOffset = 0x24;

const char* GetCurrentArea() {
    if (!*kSaveDataPtr) return nullptr;
    return reinterpret_cast<const char*>(
        reinterpret_cast<uintptr_t>(*kSaveDataPtr + kCurrentAreaOffset));
}

const char* GetCurrentMap() {
    if (!*kSaveDataPtr) return nullptr;
    return reinterpret_cast<const char*>(
        reinterpret_cast<uintptr_t>(*kSaveDataPtr + kCurrentMapOffset));
}

const char* GetFilename() {
    if (!*kSaveDataPtr) return nullptr;
    return reinterpret_cast<const char*>(
        reinterpret_cast<uintptr_t>(*kSaveDataPtr + kFilenameOffset));
}

int32_t GetStorySequence() {
    if (!*kSaveDataPtr) return 0;
    return *reinterpret_cast<int32_t*>(*kSaveDataPtr + kStorySequenceOffset);
}

int8_t GetPitSequence() {
    if (!*kSaveDataPtr) return 0;
    return *reinterpret_cast<int8_t*>(*kSaveDataPtr + kPitSequenceOffset);
}

bool GetSavedFlagValue(int32_t flag) {
    if (!*kSaveDataPtr) return false;
    int32_t flag_word =
        *reinterpret_cast<int32_t*>(
            *kSaveDataPtr + kSavedFlagsOffset + (flag >> 5) * sizeof(uint32_t));
    return flag_word & (1U << (flag & 31));
}

bool HasKeyItemInInventory(int16_t item_id) {
    int16_t* key_items_arr =
        reinterpret_cast<int16_t*>(*kPouchPtr + kKeyItemsOffset);
    for (int i = 0; i < 121; ++i) {
        if (*key_items_arr++ == item_id) return true;
    }
    return false;
}

void* GetBattleWindowDataOffset() {
    if (!*kBattlePtr) return nullptr;
    void** wrapper = reinterpret_cast<void**>(*kBattlePtr + kBattleMenuOffset);
    if (!*wrapper) return nullptr;
    return *wrapper;
}

float GetStatusWindowHeight() {
    if (!*kUnknownStatusWindowStructPtr) return 0.0f;
    float* height =
        reinterpret_cast<float*>(
            *kUnknownStatusWindowStructPtr + kStatusWindowHeightOffset);
    return *height;
}

}