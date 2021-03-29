#include "shufflizer.h"

#include <cinttypes>
#include <cstdio>
#include <ttyd/battle_ac.h>
#include <ttyd/battle_damage.h>
#include <ttyd/battle_menu_disp.h>
#include <ttyd/battle_unit.h>
#include <ttyd/cardmgr.h>
#include <ttyd/common_types.h>
#include <ttyd/event.h>
#include <ttyd/evt_mobj.h>
#include <ttyd/icondrv.h>
#include <ttyd/itemdrv.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/mariost.h>
#include <ttyd/mtx.h>
#include <ttyd/mobjdrv.h>
#include <ttyd/msgdrv.h>
#include <ttyd/OSCache.h>
#include <ttyd/OSLink.h>
#include <ttyd/seqdrv.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/sound.h>
#include <ttyd/statuswindow.h>
#include <ttyd/string.h>
#include <ttyd/system.h>

#include "common.h"
#include "patch.h"
#include "shufflizer_common.h"

// Assembly patch functions.
extern "C" {
    void CharlietonPitPriceListPatchStart();
    void CharlietonPitPriceItemPatchStart();
    void CharlietonRogueportPriceListPatchStart();
    void CharlietonRogueportPriceItemPatchStart();
    void CharlietonPitPriceListPatchEnd();
    void CharlietonPitPriceItemPatchEnd();
    void CharlietonRogueportPriceListPatchEnd();
    void CharlietonRogueportPriceItemPatchEnd();

    void StartPreventBlooperCrash1();
    void StartPreventBlooperCrash2();
    void BranchBackPreventBlooperCrash1();
    void BranchBackPreventBlooperCrash2();
}

// Code referenced in assembly patches.
extern "C" {
    
bool checkBattleUnitPointer(void* battle_unit_ptr) {
    const uintptr_t address = reinterpret_cast<uint32_t>(battle_unit_ptr);
    return address >= 0x80000000 && address < 0x81800000;
}

uint32_t preventBlooperCrash1(uint32_t unknown_val, void *battle_unit_ptr) { 
    if (checkBattleUnitPointer(battle_unit_ptr)) {
        // 0x214 in JP
        uint32_t offset = 0x218;
        const uintptr_t address = reinterpret_cast<uint32_t>(battle_unit_ptr);
        *reinterpret_cast<uint32_t*>(address + offset) = unknown_val;
    }
    return 2;
}

}

namespace mod::shufflizer {

namespace {
    
using ::ttyd::battle_menu_disp::WeaponSelectionWindowInfo;
using ::ttyd::battle_unit::BattleUnitInstance;
using ::ttyd::battle_unit::BattleUnitParams;
using ::ttyd::battle_unit::BattlePartySlotInfo;
using ::ttyd::common::AttackParams;
using ::ttyd::common::ItemData;
using ::ttyd::common::ShopItemData;
using ::ttyd::oslink::OSModuleInfo;
using ::ttyd::seqdrv::SeqIndex;
namespace ActorTypeId   = ::ttyd::common::ActorTypeId;
namespace ButtonId      = ::ttyd::common::ButtonId;
namespace ItemId        = ::ttyd::common::ItemId;
namespace ModuleId      = ::ttyd::common::ModuleId;

// Number of badges of each type to include in the shuffle pool,
// expressed in 2-bit form and offset by POWER_JUMP.
// If a badge appears in a shop and would normally appear more than once,
// in the pool, one copy will be removed from the pool.
const uint32_t kBadgePoolCounts[] = {
    0xaaaa6aaaU, 0x5a555aaaU, 0x556bfaaaU, 0x6a956a56U, 0x55555555U,
    0x00015f55U, 0x00000028U
};
// Bitfield for items that can appear on the field, offset by THUNDER_BOLT.
const uint32_t kFieldItems[]    = { 0xffffffffU, 0x0006001fU };
// Bitfield for items, recipes, and stackable badges that can appear in shops,
// offset by THUNDER_BOLT.  Gold Bar and Gold Bar x3 are not included.
const uint32_t kShopGuaranteedItems[]     = {
    0xffffffffU, 0x0006001fU, 0x00000000U, 0x00000000U, 0x01400000U,
    0x00000000U, 0x00018000U
};
const uint32_t kShopFillerItems[]     = { 
    0x00000000U, 0xfff80000U, 0xffffffffU, 0x3fff0fffU, 0x0ebfffffU, 
    0x078ffff7U, 0x00060000U
};
const uint32_t kShopAllItems[]     = { 
    0xffffffffU, 0xfffe001fU, 0xffffffffU, 0x3fff0fffU, 0x0fffffffU, 
    0x078ffff7U, 0x00078000U
};
// Rebalanced price tiers for badges.
const uint32_t kBadgePriceTiers[] = {
    0x32222633U, 0x66262224U, 0x77662244U, 0x55778888U, 0x33555555U, 
    0x44444773U, 0x32224581U, 0x63355553U, 0x41182216U, 0x11111564U,
    0x04111411U, 0x10000000U, 0x00000221U
};
// Offset from the start of shops' modules to their shop info, following the
// order of the ShopType enum.
const uint32_t kShopOffsets[]   = {
    0x36c5c, 0x57b70,  0xdd98,  0x3c14, 0x40fa8,
    0x1aed4, 0x15be8, 0x21c18, 0x23770, 0x11174
};
// Array of save data flags that represent collection state of shufflable items.
const SavedFlagRow kSavedFlags[] = {
    // Ultra Hammer allowed.
    { 0x04A0, 0x0800 }, { 0x04D0, 0x8000 }, { 0x04E0, 0x0001 },
    { 0x0540, 0x8800 }, { 0x0550, 0x0180 }, { 0x0780, 0xE000 },
    { 0x0790, 0x000B }, { 0x0820, 0x00D0 }, { 0x08B0, 0xE000 },
    { 0x08C0, 0x0007 }, { 0x09D0, 0x0A00 }, { 0x0BA0, 0x4600 },
    { 0x0C40, 0x5C00 }, { 0x0C50, 0x007E }, { 0x0CC0, 0xB000 },
    { 0x0EA0, 0x040F }, { 0x1590, 0x8DC0 }, { 0x15A0, 0xF803 },
    { 0x15B0, 0x9EFF }, { 0x15D0, 0x1000 }, { 0x15F0, 0xFE00 },
    { 0x1600, 0xFFFF }, { 0x1610, 0x03F3 },
    // Ultra Hammer disallowed. (ends with Pit rewards)
    { 0x0540, 0x7400 }, { 0x0550, 0x007C }, { 0x05D0, 0xE000 },
    { 0x05E0, 0x0033 }, { 0x0650, 0x1000 }, { 0x06F0, 0xCF14 },
    { 0x0700, 0x477C }, { 0x09D0, 0x1180 }, { 0x09E0, 0x0020 },
    { 0x0A70, 0xF8B0 }, { 0x0B00, 0x1200 }, { 0x0B10, 0x3060 },
    { 0x0B20, 0x5020 }, { 0x0B30, 0x0A20 }, { 0x0B40, 0x0020 },
    { 0x0F20, 0x8000 }, { 0x0F30, 0x0018 }, { 0x0FC0, 0x00F8 },
    { 0x1060, 0x0040 }, { 0x1070, 0x01D8 }, { 0x1110, 0x8FD8 },
    { 0x1120, 0x07BF }, { 0x1590, 0x7220 }, { 0x15A0, 0x07FC },
    { 0x15B0, 0x6000 }, { 0x15C0, 0xFFFF }, { 0x15D0, 0xEFFF },
    { 0x15E0, 0xFFFF }, { 0x15F0, 0x01FF }, { 0x1610, 0xFC00 },
    { 0x1620, 0x0007 }, { 0x13D0, 0x1FF8 }
};
// Pit of 100 Trials stat balancing parameters.
const PitBalanceParameters kHpBalanceParams = {
     40,    // Max level factor * min stat at min level
      8,    // Min stat at max level - min stat at min level
     80,    // Max level factor * (max - min stat at min level)
      6     // (Max - min stat at max level) - (Max - min stat at min level)
};
const PitBalanceParameters kLevelBalanceParams = {
    160,    // Max level factor * min stat at min level
     28,    // Min stat at max level - min stat at min level
     80,    // Max level factor * (max - min stat at min level)
      0     // (Max - min stat at max level) - (Max - min stat at min level)
};
const PitBalanceParameters kAtkBalanceParams = {
     20,    // Max level factor * min stat at min level
      5,    // Min stat at max level - min stat at min level
     40,    // Max level factor * (max - min stat at min level)
      2     // (Max - min stat at max level) - (Max - min stat at min level)
};
const PitBalanceParameters kDefBalanceParams = {
      2,    // Max level factor * min stat at min level
      0,    // Min stat at max level - min stat at min level
     28,    // Max level factor * (max - min stat at min level)
      4     // (Max - min stat at max level) - (Max - min stat at min level)
};

const char* kMoveBadgeAbbreviations[14] = {
    "P.J.", "M.B.", "P.B.", "T.J.", "Sh.S.", "Sl.S.", "So.S.",
    "P.S.", "Q.H.", "H.T.", "P.B.", "H.R.", "F.D.", "I.S."
};

// Addresses of various patch locations specific to the Shufflizer mod.
// TODO: All of these would probably need to be changed for other regions.
    
// Address to change to show crash handler.
uint32_t* kEnableHandlerOpcode  = reinterpret_cast<uint32_t*>(0x80009b2c);
// Address to insert branch to skip Ultra Hammer, etc. cutscenes.
uint32_t* kSkipUHCutsceneOpcode = reinterpret_cast<uint32_t*>(0x800abcd8);
// Address to change Trade Off's rank up effect strength.
uint32_t* kTradeOffRankUpOpcode = reinterpret_cast<uint32_t*>(0x8012eb3c);

// Address to insert new script code to give Trade Off an +ATK effect.
uint32_t* kTradeOffScriptHook   = reinterpret_cast<uint32_t*>(0x80369b34);

// Addresses to write patches to Charlieton code (to put maxes on item prices)
void* kCharlietonGorListHook    = reinterpret_cast<uint32_t*>(0x8023c0e4);
void* kCharlietonPitListHook    = reinterpret_cast<uint32_t*>(0x8023c120);
void* kCharlietonGorItemHook    = reinterpret_cast<uint32_t*>(0x8023d26c);
void* kCharlietonPitItemHook    = reinterpret_cast<uint32_t*>(0x8023d2e0);
// Addresses to write patches to fix Blooper crash.
const uintptr_t kBlooperCrash1HookAddress   = 0x8010f810;
const uintptr_t kBlooperCrash2HookAddress   = 0x8010f888;

// Miscellaneous constants.
const int16_t kHpPlusIconId             =   9;
const int16_t kFpPlusIconId             =   5;
const int16_t kSquareDiamondIconId      =  44;
const int16_t kSquareDiamondPartnerId   =  87;
const int16_t kKoopaCurseIconId         = 390;
const int16_t kChapter3PeachIntermissionSeq = 173;

void InitializeItemDataChanges() {
    ItemData* item_db = common::kItemDataArr;
    
    // Individual balance / visual changes.
    item_db[ItemId::TRADE_OFF].buy_price = 50;
    item_db[ItemId::TRADE_OFF].sell_price = 25;
    item_db[ItemId::TRIAL_STEW].buy_price = 50;
    item_db[ItemId::TRIAL_STEW].sell_price = 25;
    item_db[ItemId::STRANGE_SACK].buy_price = 250;
    item_db[ItemId::ULTRA_HAMMER].buy_price = 250;
    item_db[ItemId::CAKE].buy_price = 30;
    item_db[ItemId::CAKE].sell_price = 15;
    item_db[ItemId::CAKE].hp_restored = 15;
    item_db[ItemId::CAKE].fp_restored = 15;
    item_db[ItemId::FRESH_PASTA].buy_price = 15;
    item_db[ItemId::FRESH_PASTA].sell_price = 10;
    item_db[ItemId::KOOPASTA].buy_price = 20;
    item_db[ItemId::KOOPASTA].sell_price = 15;
    item_db[ItemId::SPICY_PASTA].buy_price = 30;
    item_db[ItemId::SPICY_PASTA].sell_price = 20;
    item_db[ItemId::INK_PASTA].buy_price = 30;
    item_db[ItemId::INK_PASTA].sell_price = 20;
    item_db[ItemId::KOOPA_CURSE].icon_id = kKoopaCurseIconId;
    item_db[ItemId::FP_DRAIN_P].bp_cost = 1;
    // Because, let's be honest.
    item_db[ItemId::TORNADO_JUMP].bp_cost = 1;
    
    // New badges (Toughen Up, Toughen Up P, Mini-HP Plus, Mini-FP Plus).
    item_db[ItemId::SUPER_CHARGE].bp_cost = 1;
    item_db[ItemId::SUPER_CHARGE].icon_id = kSquareDiamondIconId;
    item_db[ItemId::SUPER_CHARGE].item_name_msg = "in_toughen_up";
    item_db[ItemId::SUPER_CHARGE].item_desc_msg = "msg_toughen_up";
    item_db[ItemId::SUPER_CHARGE].item_desc_menu_msg = "msg_toughen_up";
    item_db[ItemId::SUPER_CHARGE_P].bp_cost = 1;
    item_db[ItemId::SUPER_CHARGE_P].icon_id = kSquareDiamondPartnerId;
    item_db[ItemId::SUPER_CHARGE_P].item_name_msg = "in_toughen_up_p";
    item_db[ItemId::SUPER_CHARGE_P].item_desc_msg = "msg_toughen_up";
    item_db[ItemId::SUPER_CHARGE_P].item_desc_menu_msg = "msg_toughen_up_p";
    item_db[ItemId::SQUARE_DIAMOND_BADGE].bp_cost = 1;
    item_db[ItemId::SQUARE_DIAMOND_BADGE].icon_id = kHpPlusIconId;
    item_db[ItemId::SQUARE_DIAMOND_BADGE].item_name_msg = "in_mini_hp_up";
    item_db[ItemId::SQUARE_DIAMOND_BADGE].item_desc_msg = "msg_mini_hp_up";
    item_db[ItemId::SQUARE_DIAMOND_BADGE].item_desc_menu_msg = "msg_mini_hp_up";
    item_db[ItemId::SQUARE_DIAMOND_BADGE_P].bp_cost = 1;
    item_db[ItemId::SQUARE_DIAMOND_BADGE_P].icon_id = kFpPlusIconId;
    item_db[ItemId::SQUARE_DIAMOND_BADGE_P].item_name_msg = "in_mini_fp_up";
    item_db[ItemId::SQUARE_DIAMOND_BADGE_P].item_desc_msg = "msg_mini_fp_up";
    item_db[ItemId::SQUARE_DIAMOND_BADGE_P].item_desc_menu_msg = "msg_mini_fp_up";
    
    // Set coin buy / discount / sell prices for badges to rebalanced values,
    // badge Star Piece costs on BP cost, recipe prices based on sell price,
    // and fix unused items' and badges' sort order.
    for (int32_t i = 0; i < ItemId::MAX_ITEM_ID; ++i) {
        ItemData& item = item_db[i];
        if (i >= ItemId::GOLD_BAR && i <= ItemId::FRESH_JUICE) {
            if (item.buy_price == 10 && item.sell_price > 8) {
                item.buy_price = item.sell_price * 5 / 4;
            }
            
            if (item.type_sort_order > 0x31) {
                item.type_sort_order += 1;
            }
        } else if (i >= ItemId::POWER_JUMP) {
            const int32_t word_index = (i - ItemId::POWER_JUMP) >> 3;
            const int32_t nybble_index = (i - ItemId::POWER_JUMP) & 7;
            const int32_t tier = 
                (kBadgePriceTiers[word_index] >> (nybble_index << 2)) & 15;
            if (tier > 0) {
                item.buy_price = tier > 4 ? tier * 50 - 100 : tier * 25;
            }
            item.star_piece_price = tier > 0 ? tier : 1;
            
            // higher discounted price, since most prices in general are lower
            item.discount_price = item.buy_price * 4 / 5;
            item.sell_price = item.buy_price >> 1;
            
            if (item.type_sort_order > 0x49) ++item.type_sort_order;
            if (item.type_sort_order > 0x43) ++item.type_sort_order;
            if (item.type_sort_order > 0x3b) ++item.type_sort_order;
            if (item.type_sort_order > 0x24) ++item.type_sort_order;
            if (item.type_sort_order > 0x21) ++item.type_sort_order;
            if (item.type_sort_order > 0x1f) ++item.type_sort_order;
            if (item.type_sort_order > 0x16) item.type_sort_order += 2;
        }
    }
    
    // Fixed sort order for Koopa Curse, new badges, and unused 'P' badges.
    item_db[ItemId::KOOPA_CURSE].type_sort_order        = 0x31 + 1;
    
    item_db[ItemId::SUPER_CHARGE].type_sort_order       = 0x16 + 1;
    item_db[ItemId::SUPER_CHARGE_P].type_sort_order     = 0x16 + 2;
    item_db[ItemId::SQUARE_DIAMOND_BADGE].type_sort_order = 0x1f + 3;
    item_db[ItemId::SQUARE_DIAMOND_BADGE_P].type_sort_order = 0x21 + 4;
    
    item_db[ItemId::ALL_OR_NOTHING_P].type_sort_order   = 0x24 + 5;
    item_db[ItemId::LUCKY_DAY_P].type_sort_order        = 0x3b + 6;
    item_db[ItemId::PITY_FLOWER_P].type_sort_order      = 0x43 + 7;
    item_db[ItemId::FP_DRAIN_P].type_sort_order         = 0x49 + 8;
    
}

void FillItemPriceInfo(ItemPrice* item_price, int16_t item_id, bool is_sell) {
    int32_t price;
    if (is_sell) {
        int32_t base_price = common::kItemDataArr[item_id].sell_price;
        // Between 100% and 200%, with a bit greater weight to multiples of 25%.
        int32_t modifier = 100 + ShufflizerRand(26) * 5 / 6 * 5;
        price = base_price * modifier / 100;
        if (price < 1) price = 1;
    } else {
        int32_t base_price = common::kItemDataArr[item_id].buy_price;
        // Between 75% and 150%, with a bit greater weight to multiples of 25%.
        int32_t modifier = 75 + ShufflizerRand(20) * 5 / 6 * 5;
        price = base_price * modifier / 100;
        if (price < 2) price = 2;
    }
    if (price > 999) price = 999;
    item_price->item = item_id;
    item_price->price = price;
}

int32_t ApplyPitBalanceParametersForCurrentFloor(
    int32_t stat, PitBalanceParameters params, const SeedState& seed_state) {
    // Get the actual Pit floor and shuffled Pit floor, bail if out of range.
    int32_t start_floor = common::GetPitSequence();
    if (start_floor < 0 or start_floor >= 100) return stat;
    int32_t end_floor = seed_state.pit_floors[start_floor];
    if (end_floor < 0 or end_floor >= 100) return stat;
    
    int32_t start_lvf = GetLevelFactorFromPitFloor(start_floor);
    int32_t end_lvf = GetLevelFactorFromPitFloor(end_floor);
    
    return ApplyPitBalanceParameters(stat, start_lvf, end_lvf, params);
}

int32_t GetActorIdFromActorParams(void* actor_data_ptr) {
    return *reinterpret_cast<int32_t*>(
        reinterpret_cast<uintptr_t>(actor_data_ptr) + 0x4);
}

int32_t IsRecoilAttack(AttackParams* attack_params) {
    return attack_params == common::kSuperHammerRecoilParams ||
           attack_params == common::kUltraHammerRecoilParams ||
           attack_params == common::kGulpRecoilParams;
}

// Trampoline hooks.
Shufflizer* gSelf = nullptr;
void (*g_stg0_00_init_trampoline)() = nullptr;
void (*g_cardCopy2Main_trampoline)(uint32_t) = nullptr;
bool (*g_OSLink_trampoline)(OSModuleInfo*, void*) = nullptr;
void (*g_seq_mapChangeMain_trampoline)(void*) = nullptr;
void* (*g_itemEntry_trampoline)(
    const char*, uint32_t, uint32_t, int32_t, void*, float, float, float) = nullptr;
int32_t (*g_mobj_powerupblk_trampoline)(void*) = nullptr;
int32_t (*g_mobj_itembox_trampoline)(void*) = nullptr;
void (*g_mobjRunEvent_trampoline)(void*, void*) = nullptr;
void (*g__getSickStatusParam_trampoline)(
    BattleUnitInstance*, AttackParams*, int32_t, int8_t*, int8_t*) = nullptr;
int32_t (*g_BattleCalculateDamage_trampoline)(
    void*, void*, void*, AttackParams*, void*, uint32_t) = nullptr;
int32_t (*g_BattleCalculateFpDamage_trampoline)(
    void*, void*, void*, AttackParams*, void*, uint32_t) = nullptr;
void* (*g_BtlUnit_Entry_trampoline)(
    BattlePartySlotInfo*, float, float) = nullptr;
const char* (*g_msgSearch_trampoline)(const char*) = nullptr;
void (*g_seqSetSeq_trampoline)(SeqIndex, const char*, const char*) = nullptr;
int32_t (*g_pouchEquipCheckBadge_trampoline)(int16_t) = nullptr;
void (*g_pouchReviseMarioParam_trampoline)() = nullptr;
int32_t (*g_BtlUnit_GetWeaponCost_trampoline)(
    BattleUnitInstance*, AttackParams*) = nullptr;
void (*g_BtlUnit_ReviseHpFp_trampoline)(BattleUnitInstance*) = nullptr;
void (*g_DrawWeaponWin_trampoline)() = nullptr;
int32_t (*g_BattleActionCommandCheckDefence_trampoline)(
    BattleUnitInstance*, AttackParams*) = nullptr;
void (*g_statusWinDisp_trampoline)() = nullptr;
    
// Global state sentinels for item replacement functions.
int32_t gShineBlockFlag = -1;
bool gInItemBoxRoutine = false;

// Global state variables for changing move badge powers in battle.
bool gInBattle = false;
bool gChangeMovePowersEnabled = true;
int8_t gMaxMoveBadgeCounts[14];
int8_t gCurMoveBadgeCounts[14];
char gMoveBadgeTextBuffers[14][24];
// To prevent repeated menu movements.
bool gLWasPressed = false;
bool gRWasPressed = false;
    
}

Shufflizer::Shufflizer() : seed_state_(), options_(), menu_(&options_) {}

void Shufflizer::InitializeShuffleSeed() {
    // 1. Seed the Shufflizer RNG, using a simple hash of the filename.
    uint32_t filename_hash = 0;
    for (const char* c = common::GetFilename(); *c != 0; ++c) {
        filename_hash = 37 * filename_hash + *c;
    }
    ShufflizerSeedRand(filename_hash);
    
    // 2. Change necessary buy/sell prices, etc. in the ItemData table.
    InitializeItemDataChanges();
    
    // 3. Put together list of possible shop items.
    // - 39 normal, 57 recipes, 69 stackable badge types.
    int16_t shop_items[165];
    int16_t *pos;
    int32_t *assign_pos;
    
    pos = CreateItemArrayFromBitfield(
        kShopGuaranteedItems, kShopGuaranteedItems + 7, shop_items, 
        ItemId::THUNDER_BOLT);
    CreateItemArrayFromBitfield(
        kShopFillerItems, kShopFillerItems + 7, pos, ItemId::THUNDER_BOLT);
    // Dried Shroom and Dizzy Dial are not randomized, but Gold Bars should be
    // included in the normal shops.
    shop_items[ItemId::DRIED_SHROOM - ItemId::THUNDER_BOLT] = ItemId::GOLD_BAR;
    shop_items[ItemId::DIZZY_DIAL - ItemId::THUNDER_BOLT] = ItemId::GOLD_BAR_X3;
    
    // 4. Shuffle filler items (all but normal items + HP/FP Pluses),
    // then shuffle first 77 items.
    // (60 shop + 20 Pianta slots - Dried Shroom, Dizzy Dial, and Cake Mix)
    ShuffleRange(pos, shop_items + 165);
    ShuffleRange(shop_items, shop_items + 77);
    
    // 5. Fill out normal shop item data.
    pos = shop_items;
    for (int32_t i = 0; i < 60; ++i) {
        // Slot Dried Shroom and Dizzy Dial in the expected places.
        if (i == ShopType::WESTSIDE_GOODS * 6 + 2) {
            ItemPrice& item_price = 
                seed_state_.shops[ShopType::WESTSIDE_GOODS].buy_items[2];
            item_price.item = ItemId::DRIED_SHROOM;
            item_price.price = 2;
        } else if (i == ShopType::WESTSIDE_GOODS * 6 + 4) {
            ItemPrice& item_price = 
                seed_state_.shops[ShopType::WESTSIDE_GOODS].buy_items[4];
            item_price.item = ItemId::DIZZY_DIAL;
            item_price.price = 12;
        } else {
            FillItemPriceInfo(
                &seed_state_.shops[i/6].buy_items[i%6],
                *pos++, /* is_sell = */ false);
        }
    }
    // Fill shop sell items with 5 random items each between 0x80 and 0xeb.
    for (int32_t i = 0; i < 50; ++i) {
        FillItemPriceInfo(
            &seed_state_.shops[i/5].sell_items[i%5],
            ItemId::THUNDER_BOLT +
                ShufflizerRand(ItemId::INVALID_ITEM_CAN - ItemId::THUNDER_BOLT),
            /* is_sell = */ true);
    }
    
    // 6. Fill out Pianta Parlor tables.
    assign_pos = common::kPiantaParlorItemArr;
    // Hardcode Cake Mix to first slot.
    *assign_pos++ = ItemId::CAKE_MIX;
    for (int32_t i = 1; i < 20; ++i) {
        // Leave breaks between every card's items.
        if (!(i & 3)) ++assign_pos;
        *assign_pos++ = *pos++;
    }
    
    // 7. Initialize the shuffled one-time item pool.
    // Items are distributed as follows:
    // - 27 Howz badges, 15 Dazzle badges, 16 Charlieton items,
    //   286 field items (103 of which are eligible for Ultra Hammer).
    // - Includes up to 138 badges, some items / Star Pieces, 20 coins,
    //   41 Shine Sprites, the Strange Sack and the Ultra Hammer.
    //   (The exact counts of badges / items / Star Pieces depend on the
    //    item shops' badges and the Star Piece cost of Dazzle's badges.)
    int16_t items[344 + 10];  // The last 10 are padding for moving Pit rewards.
    int16_t *badges_end, *items_end, *shines_end;
    
    uint32_t badge_pool_counts[7];
    ttyd::system::memcpy_as4(
        badge_pool_counts, kBadgePoolCounts, 7 * sizeof(uint32_t));
    
    // Remove a copy of all badges included in the standard item shops or Parlor
    // that would otherwise have more than one copy.
    for (int32_t i = 0; i < 77; ++i) {
        int16_t item_id = shop_items[i];
        if (item_id >= ItemId::POWER_JUMP) {
            uint32_t word_index = (item_id - ItemId::POWER_JUMP) >> 4;
            uint32_t bits_index = (item_id - ItemId::POWER_JUMP) & 15;
            int32_t count =
                (badge_pool_counts[word_index] >> (bits_index << 1)) & 3;
            if (count > 1) {
                badge_pool_counts[word_index] -= 1 << (bits_index << 1);
            }
        }
    }
    // Initialize remaining badge pool.
    pos = CreateItemArrayFromBitfield(
        badge_pool_counts, badge_pool_counts + 7, items, ItemId::POWER_JUMP, 
        /* bits_per_item = */ 2);
    badges_end = pos;
    
    *pos++ = ItemId::STRANGE_SACK;
    
    // Fill forward with the 39 field-spawnable items x5 copies; some will be
    // overwritten later, but this is enough to guarantee there will be
    // enough given the lowest possible number of badges / Star Pieces, but not 
    // enough to write past the end of the item buffer.
    for (int32_t i = 0; i < 5; ++i) {
        pos = CreateItemArrayFromBitfield(
            kFieldItems, kFieldItems + 2, pos, ItemId::THUNDER_BOLT);
    }
    
    // Fill Shines and coins backwards from the next-to-last slot;
    // the last slot is saved for the Ultra Hammer.
    pos = items + 343;
    shines_end = pos;
    for (int32_t i = 0; i < 41; ++i) {
        *--pos = ItemId::SHINE_SPRITE;
    }
    for (int32_t i = 0; i < 20; ++i) {
        *--pos = ItemId::COIN;
    }
    items_end = pos;
    
    // 8. Shuffle badges, and assign Howz and Dazzle's badge stock.
    ShuffleRange(items, badges_end);
    
    assign_pos = common::kHowzItemArr;
    pos = items;
    for (int32_t i = 0; i < 27; ++i) {
        // Leave breaks after each chapter's group of badges.
        if (i > 6 && !(i % 3)) ++assign_pos;
        *assign_pos++ = *pos++;
    }
    
    assign_pos = common::kDazzleItemArr;
    // Keep track of how many Star Pieces we need (-2 for Excess NPC gifts).
    int32_t star_pieces = -2;
    for (int32_t i = 0; i < 15; ++i) {
        int16_t item_id = *pos++;
        *assign_pos++ = item_id;
        star_pieces += common::kItemDataArr[item_id].star_piece_price;
    }
    
    // Replace the last X-2 items with Star Pieces.
    for (int32_t i = 0; i < star_pieces; ++i) {
        *--items_end = ItemId::STAR_PIECE;
    }
    
    // 9. Shuffle the remaining badges and Strange Sack for Pit rewards;
    // these will be moved to the correct location later.
    ShuffleRange(pos, badges_end + 1);
    pos += 10;
    
    // 10. Perform shuffles to uniformly distribute valid items to Charlieton,
    // and UH-allowed / disallowed field item slots.
    
    // Shuffle remaining badges / Strange Sack and items.
    ShuffleRange(pos, items_end);
    // Shuffle all but the first 16 of these, and everything else except the UH.
    ShuffleRange(pos + 16, shines_end);
    // Swap the Ultra Hammer with a random Charlieton or UH-allowed field item.
    int16_t* swap_pos = pos + ShufflizerRand(16 + 103);
    *shines_end = *swap_pos;
    *swap_pos = ItemId::ULTRA_HAMMER;
    
    // 11. Assign Rogueport Charlieton's stock of items.
    assign_pos = common::kCharlietonItemArr;
    for (int32_t i = 0; i < 16; ++i) {
        *assign_pos++ = *pos++;
    }
    
    // 12. Copy the slots with the Pit rewards chosen earlier to the end,
    // then copy remaining items to the field item seed state.
    ttyd::system::memcpy_as4(items + 344, items + 42, 10 * sizeof(int16_t));
    ttyd::system::memcpy_as4(seed_state_.field_items, pos, 286 * sizeof(int16_t));
    
    // 13. Set up shuffled Pit floors (include the 10's in their normal places).
    int8_t pit_floors[90];
    for (int32_t i = 0; i < 90; ++i) {
        pit_floors[i] = i * 10 / 9;
    }
    ShuffleRange(pit_floors, pit_floors + 90);
    for (int32_t i = 0; i < 100; ++i) {
        if (i % 10 == 9) {
            seed_state_.pit_floors[i] = i;
        } else {
            seed_state_.pit_floors[i] = pit_floors[(i+1) * 9 / 10];
        }
    }
    
    // 14. Set up Charlieton Pit items (uses the same pool as item shops).
    ShuffleRange(shop_items, shop_items + 165);
    ttyd::system::memcpy_as4(
        seed_state_.charlieton_pit_items, shop_items, 54 * sizeof(int16_t));
        
    // 15. Set up shuffled miscellaneous items.
    pos = shop_items;
    for (int32_t i = ItemId::COCONUT; i < ItemId::PEACHY_PEACH; ++i) {
        *pos++ = i;
    }
    // Cake Mix should remain in the same place, shuffle the rest.
    pos = shop_items + (ItemId::CAKE_MIX - ItemId::COCONUT);
    swap_pos = shop_items + (ItemId::PEACHY_PEACH - ItemId::COCONUT);
    *pos = ItemId::PEACHY_PEACH;
    ShuffleRange(shop_items, swap_pos);
    *swap_pos = *pos;
    *pos = ItemId::CAKE_MIX;
    
    ttyd::system::memcpy_as4(
        seed_state_.special_field_items, shop_items, 12 * sizeof(int16_t));
        
    // 16. Save the final id of the Ultra Hammer flag for convenience.
    // (If it ended up in Charlieton's shop, the index remains -1.)
    int32_t ultra_hammer_index = -1;
    for (int32_t i = 0; i < 286; ++i) {
        if (seed_state_.field_items[i] == ItemId::ULTRA_HAMMER) {
            ultra_hammer_index = i;
            break;
        }
    }
    seed_state_.ultra_hammer_flag = GetFieldItemSavedFlagIdFromIndex(
        kSavedFlags, kSavedFlags + sizeof(kSavedFlags) / sizeof(kSavedFlags[0]),
        ultra_hammer_index);
}

void Shufflizer::OnModuleLoaded(ttyd::oslink::OSModuleInfo* module_info) {
    if (module_info == nullptr) return;
    int32_t module_id = module_info->id;
    uintptr_t module_ptr = reinterpret_cast<uintptr_t>(module_info);
    
    // Pit of 100 Trials.
    if (module_id == ModuleId::JON) {
        int8_t pit_floor = common::GetPitSequence();
        int32_t* item_pos = common::kPitCharlietonItemArr;
        
        if (options_.charlieton_fixed) {
            // Read from the floor's predetermined set of items / badges.
            int32_t offset = pit_floor / 10 * 6;
            if (offset < 0 || offset > 48) offset = 0;
            for (int32_t i = 0; i < 6; ++i) {
                *item_pos++ = seed_state_.charlieton_pit_items[offset + i];
            }
        } else {
            // Assign Charlieton a set of six random items.
            for (int32_t i = 0; i < 6; ++i) {
                *item_pos++ = GetRandomItemFromBitfield(
                    kShopAllItems, kShopAllItems + 7, ItemId::THUNDER_BOLT);
            }
        }
        
        // Make Charlieton always appear.
        *reinterpret_cast<int32_t*>(
            module_ptr + common::kPitModuleCharlietonChanceOffset) = 1000;
            
        // Change the enemy type that spawns on the floor, if enabled.
        if (options_.shuffle_pit_floors) {
            int8_t pit_floor = common::GetPitSequence();
            int8_t* pit_floor_npc_id_ptr =
                reinterpret_cast<int8_t*>(
                    module_ptr + common::kPitModuleNpcArrOffset + pit_floor);
            *pit_floor_npc_id_ptr =
                GetPitNpcTypeFromFloor(seed_state_.pit_floors[pit_floor]);
        }
    }
    
    // Remove code that automatically saves badges gotten from the Atomic Boo
    // fight to the badge shop, in favor of adding an event that spawns it on
    // the floor if you don't pick it up initially, and re-enter the room later.
    if (module_id == ModuleId::JIN) {
        // Disable the flag that makes the item go to the badge shop if left.
        *reinterpret_cast<int32_t*>(
            module_ptr + common::kJinModuleAtomicBooItemObjectFlagOffset) = 0;
        // Don't automatically set the GSWF for having picked up the item.
        *reinterpret_cast<int32_t*>(
            module_ptr + common::kJinModuleAtomicBooItemGswfSetOffset) = 0;
    }
    
    // If shuffling misc. items is allowed and module loaded is Keelhaul Key,
    // replace Flavio's desired item with its shuffled replacement.
    if (options_.shuffle_misc_items && module_id == ModuleId::MUJ) {
        *reinterpret_cast<int32_t*>(
            module_ptr + common::kMujModuleFlavioDesiredItemOffset) = 
                seed_state_.special_field_items[0];
    }
    
    // Handle shop info for most shops, based on module loaded.
    int32_t shop = -1;
    switch (module_id) {
        case ModuleId::GOR:
            shop = ShopType::WESTSIDE_GOODS;
            break;
        case ModuleId::TIK:
            shop = ShopType::DEEPDOWN_DEPOT;
            break;
        case ModuleId::NOK:
            shop = ShopType::NIFF_T_SHOP;
            break;
        case ModuleId::MRI:
            shop = ShopType::PUNGENT_SHOP;
            break;
        case ModuleId::TOU:
            shop = ShopType::SOUVENIR_SHOP;
            break;
        case ModuleId::USU:
            shop = ShopType::TWILIGHT_SHOP;
            break;
        case ModuleId::MUJ:
            shop = ShopType::KEELHAUL_GALLERIA;
            break;
        case ModuleId::RSH:
            shop = ShopType::SALES_STALL;
            break;
        case ModuleId::BOM:
            shop = ShopType::NORTHWINDS_MART;
            break;
        default:
            break;
    }
    
    // No shops in this area; nothing more to do.
    if (shop == -1 || shop >= ShopType::SHOP_MAX_ID) return;
    
    // Copy shop data from Shufflizer state to the shop's data.
    ShopItemData* item_data =
        reinterpret_cast<ShopItemData*>(module_ptr + kShopOffsets[shop]);
    for (int32_t i = 0; i < 6; ++i) {
        const ItemPrice& item_price = seed_state_.shops[shop].buy_items[i];
        item_data->item_id = item_price.item;
        item_data->buy_price = item_price.price;
        ++item_data;
    }
    // Items available to buy here should sell for half their going rate.
    for (int32_t i = 0; i < 6; ++i) {
        const ItemPrice& item_price = seed_state_.shops[shop].buy_items[i];
        item_data->item_id = item_price.item;
        item_data->sell_price = item_price.price / 2;
        ++item_data;
    }
    for (int32_t i = 0; i < 5; ++i) {
        const ItemPrice& item_price = seed_state_.shops[shop].sell_items[i];
        item_data->item_id = item_price.item;
        item_data->sell_price = item_price.price;
        ++item_data;
    }
    // Null-terminate the array, since some shops have more than five
    // specialty prices in the vanilla game.
    item_data->item_id = 0;
    item_data->sell_price = 0;
    
    if (module_id != ModuleId::GOR) return;
    
    // Initialize the Toad Bros. Bazaar items; the buy items have six additional
    // copies of the buy items with the 5 Super Luigi volumes / contact lens.
    shop = ShopType::TOAD_BROS_BAZAAR;
    item_data =
        reinterpret_cast<ShopItemData*>(module_ptr + kShopOffsets[shop]);
    for (int32_t copy = 0; copy < 7; ++copy) {
        // Skip first item slot on additional copies.
        if (copy > 0) ++item_data;
        for (int32_t i = copy > 0 ? 1 : 0; i < 6; ++i) {
            const ItemPrice& item_price = seed_state_.shops[shop].buy_items[i];
            item_data->item_id = item_price.item;
            item_data->buy_price = item_price.price;
            ++item_data;
        }
    }
    // Items available to buy here should sell for half their going rate.
    for (int32_t i = 0; i < 6; ++i) {
        const ItemPrice& item_price = seed_state_.shops[shop].buy_items[i];
        item_data->item_id = item_price.item;
        item_data->sell_price = item_price.price / 2;
        ++item_data;
    }
    for (int32_t i = 0; i < 5; ++i) {
        const ItemPrice& item_price = seed_state_.shops[shop].sell_items[i];
        item_data->item_id = item_price.item;
        item_data->sell_price = item_price.price;
        ++item_data;
    }
    // Null-terminate the array, since some shops have more than five
    // specialty prices in the vanilla game.
    item_data->item_id = 0;
    item_data->sell_price = 0;
    
    // Disable Ultra Hammer chest despawning unless the chest has been opened.
    int32_t* chest_despawn_item_ptr = 
        reinterpret_cast<int32_t*>(
            module_ptr + common::kUltraHammerChestDespawnIdOffset);
    if (common::GetSavedFlagValue(0x4ab)) {
        // If chest has been opened, make it despawn if you have the Hammer
        // (which is guaranteed.)
        *chest_despawn_item_ptr = ItemId::HAMMER;
    } else {
        // Make it despawn if you have an impossible item (which never happens).
        *chest_despawn_item_ptr = ItemId::INVALID_ITEM_CAN;
    }
    
    // Make Ultra Hammer unpurchaseable before finishing Chapter 3.
    ItemData* item_db = common::kItemDataArr;
    if (common::GetStorySequence() < kChapter3PeachIntermissionSeq) {
        item_db[ItemId::ULTRA_HAMMER].buy_price = 834;
    } else {
        item_db[ItemId::ULTRA_HAMMER].buy_price = 250;
    }
}

void Shufflizer::AfterRoomInitEvent() {
    // Creepy Steeple entrance; beat Atomic Boo without picking up its item.
    if (!ttyd::string::strcmp(common::GetCurrentMap(), "jin_00") &&
        common::GetSavedFlagValue(0x8b2) && !common::GetSavedFlagValue(0x8c2)) {
        // Respawn the item (the type doesn't matter, since it will get
        // replaced with the proper item by ReplaceGeneralItem).
        ttyd::itemdrv::itemEntry(
            "item_999", /* type */ 0x79, /* mode */ 0, 
            mod::common::kSavedWordFlagBaseValue + 0x8c2, nullptr, 
            -64.0, 0.0, 0.0);
    }
}

int16_t Shufflizer::ReplaceGeneralItem(int16_t id, int32_t collection_expr) {
    // If in a battle or in the pause menu (i.e. using an item on the field),
    // don't replace the item.
    if (gInBattle || ttyd::mariost::marioStGetSystemLevel() == 15) return -1;
    
    // Check for miscellaneous items, if shuffling them is enabled.
    if (options_.shuffle_misc_items && 
        id >= ItemId::COCONUT && id <= ItemId::PEACHY_PEACH) {
        // Lazy fixes for Mario showing certain items to an NPC in a script.
        if (!ttyd::string::strcmp(common::GetCurrentMap(), "muj_01") ||
            !ttyd::string::strcmp(common::GetCurrentArea(), "rsh")) return id;
        return seed_state_.special_field_items[id - ItemId::COCONUT];
    }
    
    // Special cases.
    int32_t flag = collection_expr - common::kSavedWordFlagBaseValue;
    if (collection_expr == -1) {
        if (gShineBlockFlag > 0) {
            flag = gShineBlockFlag;
        } else if (!ttyd::string::strcmp(common::GetCurrentMap(), "aji_03")) {
            switch (id) {
                case ItemId::FEELING_FINE:
                    flag = 0x1066;
                    break;
                case ItemId::FEELING_FINE_P:
                    flag = 0x1078;
                    break;
                case ItemId::STAR_PIECE:
                    flag = 0x1621;
                    break;
                default:
                    return -1;
            }
        } else if (!ttyd::string::strcmp(common::GetCurrentMap(), "tou_05")) {
            if (id == ItemId::STAR_PIECE) {
                flag = 0x15f9;
            } else {
                return -1;
            }
        } else if (!ttyd::string::strncmp(common::GetCurrentMap(), "rsh_04", 6)) {
            if (id == ItemId::STAR_PIECE) {
                flag = 0x1614;
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
    int32_t field_item_index = GetFieldItemIndexFromSavedFlag(
        kSavedFlags, kSavedFlags + sizeof(kSavedFlags) / sizeof(kSavedFlags[0]),
        flag);
    
    if (field_item_index < 0) return -1;
    
    int16_t result_item = seed_state_.field_items[field_item_index];
    if (result_item == ItemId::COIN && gShineBlockFlag > 0) {
        return ItemId::GOLD_BAR;
    }
    return result_item;
}

void Shufflizer::ReplaceChestItem(void* object) {
    // Get the flag number associated with this chest.
    uintptr_t object_ptr = reinterpret_cast<uintptr_t>(object);
    int32_t flag = 
        *reinterpret_cast<int32_t*>(
            object_ptr + common::kObjectCollectionExprOffset) - 
        common::kSavedWordFlagBaseValue;
    int32_t field_item_index = GetFieldItemIndexFromSavedFlag(
        kSavedFlags, kSavedFlags + sizeof(kSavedFlags) / sizeof(kSavedFlags[0]),
        flag);
    if (field_item_index < 0) return;
    
    // Hacky relative addresses, since I couldn't figure out how else to
    // find the item from the object pointer.
    int32_t offset = 0x30;
    if (!ttyd::string::strcmp(common::GetCurrentArea(), "jon")) {
        offset = (flag - 0x13d3) * sizeof(uint32_t) - 0x28;
    } else if (!ttyd::string::strcmp(common::GetCurrentArea(), "dou")) {
        offset = 0x18;
    } else if (!ttyd::string::strcmp(common::GetCurrentArea(), "tou")) {
        offset = 0x38;
    } else if (flag == 0x4ab) {
        offset = 0x3c;
    }
    
    // Replace the item in the chest.
    uintptr_t chest_script_ptr =
        *reinterpret_cast<uintptr_t*>(
            object_ptr + common::kObjectChestScriptPtrOffset);
    int32_t* chest_item_ptr =
        reinterpret_cast<int32_t*>(chest_script_ptr + offset);
    *chest_item_ptr = seed_state_.field_items[field_item_index];
}

int32_t Shufflizer::AlterDamageCalculation(
    void* a_actor_params, void* d_actor_params, void* addl_d_params,
    AttackParams* attack_params, void* unk0, uint32_t unk1) {
    // Get base attack / defense powers.
    int32_t base_atk = attack_params->base_damage;
    int8_t* defense_ptr = *reinterpret_cast<int8_t**>(
        reinterpret_cast<uintptr_t>(addl_d_params) + 0x1B4);
    int32_t base_def = defense_ptr[attack_params->element];
    
    int32_t altered_atk = base_atk, altered_def = base_def;
    // Alter ATK power for enemy attacks.
    if (GetActorIdFromActorParams(a_actor_params) <= ActorTypeId::BONETAIL
        && !IsRecoilAttack(attack_params) && !attack_params->item_id
        && base_atk > 0) {
        // If in the Pit and floors are shuffled, apply balance.
        if (!ttyd::string::strcmp(common::GetCurrentArea(), "jon") &&
            options_.shuffle_pit_floors) {
            altered_atk = ApplyPitBalanceParametersForCurrentFloor(
                base_atk, kAtkBalanceParams, seed_state_);
            if (altered_atk < 1) altered_atk = 1;
        }
        
        // Apply configurable ATK modifier.
        altered_atk = (altered_atk * options_.enemy_atk_modifier + 50) / 100;
        if (altered_atk < 1) altered_atk = 1;
        if (altered_atk > 99) altered_atk = 99;
        attack_params->base_damage = altered_atk;
    }
    // Alter DEF power for enemies on defense.
    if (GetActorIdFromActorParams(d_actor_params) <= ActorTypeId::BONETAIL
        && base_def >= 0 && base_def < 99) {
        // Balance for the Pit (don't change DEF if it's 0 by default.)
        if (!ttyd::string::strcmp(common::GetCurrentArea(), "jon")
            && base_def > 0 && options_.shuffle_pit_floors) {
            altered_def = ApplyPitBalanceParametersForCurrentFloor(
                base_def, kDefBalanceParams, seed_state_);
            if (altered_def < 0) altered_def = 0;
        }
        
        // Apply configurable DEF modifier.
        altered_def = altered_def + options_.enemy_def_modifier;
        if (altered_def > 99) altered_def = 99;
        defense_ptr[attack_params->element] = altered_def;
    }
    
    // Run vanilla damage calculation.
    const int32_t damage = g_BattleCalculateDamage_trampoline(
        a_actor_params, d_actor_params, addl_d_params, attack_params, unk0, unk1);
    
    // Change ATK and DEF back, and return calculated damage.
    attack_params->base_damage = base_atk;
    defense_ptr[attack_params->element] = base_def;
    return damage;
}

int32_t Shufflizer::AlterFpDamageCalculation(
    void* a_actor_params, void* d_actor_params, void* addl_d_params,
    AttackParams* attack_params, void* unk0, uint32_t unk1) {
    // Get base FP damage.
    int32_t base_fp_damage = attack_params->base_fp_damage;
    
    int32_t altered_fp_damage = base_fp_damage;
    // Alter FP damage for enemy attacks.
    if (GetActorIdFromActorParams(a_actor_params) <= ActorTypeId::BONETAIL
        && !attack_params->item_id && base_fp_damage > 0) {
        // If in the Pit and floors are shuffled, apply balance.
        if (!ttyd::string::strcmp(common::GetCurrentArea(), "jon") &&
            options_.shuffle_pit_floors) {
            altered_fp_damage = ApplyPitBalanceParametersForCurrentFloor(
                base_fp_damage, kAtkBalanceParams, seed_state_);
            if (altered_fp_damage < 1) altered_fp_damage = 1;
        }
        
        // Apply configurable ATK modifier.
        altered_fp_damage =
            (altered_fp_damage * options_.enemy_atk_modifier + 50) / 100;
        if (altered_fp_damage < 1) altered_fp_damage = 1;
        if (altered_fp_damage > 99) altered_fp_damage = 99;
        attack_params->base_fp_damage = altered_fp_damage;
    }
    
    // Run vanilla damage calculation.
    const int32_t damage = g_BattleCalculateFpDamage_trampoline(
        a_actor_params, d_actor_params, addl_d_params, attack_params, unk0, unk1);
    
    // Change value back, and return calculated FP loss.
    attack_params->base_fp_damage = base_fp_damage;
    return damage;
}

void Shufflizer::AlterHpAndLevel(
    ttyd::battle_unit::BattlePartySlotInfo* slot_info) {
    BattleUnitParams* unit_params = slot_info->unit_class_params;
    // If not an enemy type, nothing to change.
    if (unit_params->type_id > ActorTypeId::BONETAIL) return;
    // Used as sentinel to see if stats have already been changed for this type.
    if (unit_params->run_rate & 1) return;
    
    int32_t hp_stat = unit_params->max_hp;
    
    // If in the Pit and floors are shuffled, balance HP & Level.
    if (!ttyd::string::strcmp(common::GetCurrentArea(), "jon") &&
        options_.shuffle_pit_floors) {
        // Replace bonus EXP with fairly equivalent level increase.
        int32_t level_stat =
            unit_params->level + unit_params->bonus_exp * 2;
        level_stat = ApplyPitBalanceParametersForCurrentFloor(
            level_stat, kLevelBalanceParams, seed_state_);
        if (level_stat < 1) level_stat = 1;
        unit_params->level = level_stat;
        unit_params->bonus_exp = 0;
        
        // Balance HP.
        hp_stat = ApplyPitBalanceParametersForCurrentFloor(
            hp_stat, kHpBalanceParams, seed_state_);
        if (hp_stat < 1) hp_stat = 1;
    }
    // Regardless of whether floors are shuffled, guarantee a level for Bonetail.
    if (unit_params->type_id == ActorTypeId::BONETAIL) {
        unit_params->level = 100;
        unit_params->bonus_exp = 100;
    }
    
    // Apply configurable HP modifier.
    hp_stat = (hp_stat * options_.enemy_hp_modifier + 50) / 100;
    if (hp_stat < 1) hp_stat = 1;
    unit_params->max_hp = hp_stat;
    
    // Set sentinel bit so enemy's stats aren't changed multiple times.
    unit_params->run_rate = unit_params->run_rate | 1;
}

void Shufflizer::HandleMovePowerLevelSelection() {
    if (!gChangeMovePowersEnabled) return;
    
    // Handle button presses.
    uint16_t buttons = ttyd::system::keyGetButton(0);
    bool l_pressed = (buttons & ButtonId::L) && !gLWasPressed;
    bool r_pressed = (buttons & ButtonId::R) && !gRWasPressed;
    gLWasPressed = !!(buttons & ButtonId::L);
    gRWasPressed = !!(buttons & ButtonId::R);

    // Check to see if in menu.
    void** win_data = reinterpret_cast<void**>(
        common::GetBattleWindowDataOffset());
    if (!win_data) return;
    if (!win_data[0] || !win_data[2]) return;

    const int32_t win_selection_max = reinterpret_cast<int32_t*>(win_data[0])[2];
    const int32_t win_selection_cur = reinterpret_cast<int32_t*>(win_data[0])[0];
    WeaponSelectionWindowInfo* moves_win_data =
        reinterpret_cast<WeaponSelectionWindowInfo*>(win_data[2]);
        
    for (int32_t i = 0; i < win_selection_max; ++i) {
        WeaponSelectionWindowInfo& wwi = moves_win_data[i];
        if (!wwi.attack_params) continue;
        const int32_t badge_move_idx =
            wwi.attack_params->item_id - ItemId::POWER_JUMP;
        if (badge_move_idx < 0 || badge_move_idx >= 14 ||
            gMaxMoveBadgeCounts[badge_move_idx] <= 1) continue;
               
        // If current selection, and L / R was pressed, change level.
        if (i == win_selection_cur) {
            if (l_pressed &&
                gCurMoveBadgeCounts[badge_move_idx] > 1) {
                gCurMoveBadgeCounts[badge_move_idx] -= 1;
                ttyd::sound::SoundEfxPlayEx(0x478, 0, 0x64, 0x40);
            } else if (r_pressed &&
                gCurMoveBadgeCounts[badge_move_idx] < 
                gMaxMoveBadgeCounts[badge_move_idx]) {
                gCurMoveBadgeCounts[badge_move_idx] += 1;
                ttyd::sound::SoundEfxPlayEx(0x478, 0, 0x64, 0x40);
            }
        }
        
        // Overwrite default text based on current power level for all options.
        sprintf(gMoveBadgeTextBuffers[badge_move_idx], "%s Lv. %" PRId8 " (L/R)",
                kMoveBadgeAbbreviations[badge_move_idx],
                gCurMoveBadgeCounts[badge_move_idx]);
        wwi.menu_text = gMoveBadgeTextBuffers[badge_move_idx];
    }
}      

void Shufflizer::Init() {
    gSelf = this;
    
    // FILE / SEED INITIALIZATION:
    // For loading a new file.
    g_stg0_00_init_trampoline = patch::HookFunction(
        ttyd::event::stg0_00_init, []() {
            gSelf->InitializeShuffleSeed();
            g_stg0_00_init_trampoline();
        });
    // For loading an existing file.
    g_cardCopy2Main_trampoline = patch::HookFunction(
        ttyd::cardmgr::cardCopy2Main, [](uint32_t size) {
            g_cardCopy2Main_trampoline(size);
            gSelf->InitializeShuffleSeed();
        });
        
    // ROOM INITIALIZATION:
    // Run code on loading a new module.
    g_OSLink_trampoline = patch::HookFunction(
        ttyd::oslink::OSLink, [](OSModuleInfo* new_module, void* bss) {
            bool result = g_OSLink_trampoline(new_module, bss);
            if (new_module != nullptr && result) {
                gSelf->OnModuleLoaded(new_module);
            }
            return result;
        });
    // Run custom events after the main initialization event for a room has
    // finished executing.
    g_seq_mapChangeMain_trampoline = patch::HookFunction(
        ttyd::seq_mapchange::seq_mapChangeMain, [](void* work) {
            g_seq_mapChangeMain_trampoline(work);
            if (reinterpret_cast<int32_t*>(work)[1] == 5) {
                gSelf->AfterRoomInitEvent();
            }
        });
    
    // ITEM REPLACEMENT:
    // Run code before spawning an item.
    g_itemEntry_trampoline = patch::HookFunction(
        ttyd::itemdrv::itemEntry, [](
            const char* name, uint32_t id, uint32_t mode,
            int32_t collection_expr, void* script, float x, float y , float z) {
            int16_t replacement = gSelf->ReplaceGeneralItem(id, collection_expr);
            if (replacement >= 0) id = replacement;
            return g_itemEntry_trampoline(
                name, id, mode, collection_expr, script, x, y, z);
        });
    // Run code on entering/exiting the Shine Block object logic.
    g_mobj_powerupblk_trampoline = patch::HookFunction(
        ttyd::evt_mobj::mobj_powerupblk, [](void* object) {
            // Set sentinel value to the flag associated with this block.
            gShineBlockFlag =
                *reinterpret_cast<int32_t*>(
                    reinterpret_cast<uintptr_t>(object) + 
                    common::kObjectCollectionExprOffset)
                - common::kSavedWordFlagBaseValue;
            int32_t result = g_mobj_powerupblk_trampoline(object);
            gShineBlockFlag = -1;
            return result;
        });
    // Run code on entering/exiting the item block object logic (for chests).
    g_mobj_itembox_trampoline = patch::HookFunction(
        ttyd::evt_mobj::mobj_itembox, [](void* object) {
            // Set sentinel value to true while in item box routine.
            gInItemBoxRoutine = true;
            int32_t result = g_mobj_itembox_trampoline(object);
            gInItemBoxRoutine = false;
            return result;
        });
    // Hook into mobjRunEvent to alter chest items.
    g_mobjRunEvent_trampoline = patch::HookFunction(
        ttyd::mobjdrv::mobjRunEvent, [](void* object, void* script) {
            if (gInItemBoxRoutine) {
                if (script ==
                    *reinterpret_cast<void**>(
                        reinterpret_cast<uintptr_t>(object) +
                        common::kObjectChestScriptPtrOffset)) {
                    gSelf->ReplaceChestItem(object);
                }
            }
            g_mobjRunEvent_trampoline(object, script);
        });
        
    // STATS BALANCING:
    // Wrap functions for calculating damage & FP-damage.
    g_BattleCalculateDamage_trampoline = patch::HookFunction(
        ttyd::battle_damage::BattleCalculateDamage, [](
            void* a_actor_params, void* d_actor_params, void* addl_d_params,
            AttackParams* attack_params, void* unk0, uint32_t unk1) {
            return gSelf->AlterDamageCalculation(
                a_actor_params, d_actor_params, addl_d_params,
                attack_params, unk0, unk1);
        });
    g_BattleCalculateFpDamage_trampoline = patch::HookFunction(
        ttyd::battle_damage::BattleCalculateFpDamage, [](
            void* a_actor_params, void* d_actor_params, void* addl_d_params,
            AttackParams* attack_params, void* unk0, uint32_t unk1) {
            return gSelf->AlterFpDamageCalculation(
                a_actor_params, d_actor_params, addl_d_params,
                attack_params, unk0, unk1);
        });
    // Alter enemy HP and level / Star Points before loading the enemy's info.
    g_BtlUnit_Entry_trampoline = patch::HookFunction(
        ttyd::battle_unit::BtlUnit_Entry, [](
            BattlePartySlotInfo* slot_info, float unk0, float unk1) {
            gSelf->AlterHpAndLevel(slot_info);
            return g_BtlUnit_Entry_trampoline(slot_info, unk0, unk1);
        });
    
    // TEXT REPLACEMENT:
    // Patch in fixed text for Trade Off, Cake, and Koopa Curse, etc.
    g_msgSearch_trampoline = patch::HookFunction(
        ttyd::msgdrv::msgSearch, [](const char* msg_key) {
            const char* replacement = 
                GetReplacementMessage(msg_key, gSelf->seed_state_);
            if (replacement) return replacement;
            return g_msgSearch_trampoline(msg_key);
        });
        
    // SELECT MOVE BADGE POWER:
    // Patch sequence start function to set variables on battle start.
    g_seqSetSeq_trampoline = patch::HookFunction(
        ttyd::seqdrv::seqSetSeq, [](
            SeqIndex seq, const char *mapName, const char *beroName) {
                if (seq == SeqIndex::kBattle) {
                    if (gSelf->options_.select_move_power) {
                        for (int32_t i = 0; i < 14; ++i) {
                            int8_t badge_count =
                                ttyd::mario_pouch::pouchEquipCheckBadge(
                                    ItemId::POWER_JUMP + i);
                            gMaxMoveBadgeCounts[i] = badge_count;
                            gCurMoveBadgeCounts[i] = badge_count;
                        }
                    }
                    gInBattle = true;
                    gChangeMovePowersEnabled = gSelf->options_.select_move_power;
                } else {
                    gInBattle = false;
                }
                g_seqSetSeq_trampoline(seq, mapName, beroName);
            });
    // Patch pouch badge count function to use user-selected values.
    g_pouchEquipCheckBadge_trampoline = patch::HookFunction(
        ttyd::mario_pouch::pouchEquipCheckBadge, [](int16_t badge_id) {
            // If badge id is move badge, and in battle with power selection
            // enabled, return the current setting instead of the actual count.
            if (gInBattle && gChangeMovePowersEnabled &&
                badge_id >= ItemId::POWER_JUMP && badge_id <= ItemId::ICE_SMASH) {
                const int32_t cur_count =
                    gCurMoveBadgeCounts[badge_id - ItemId::POWER_JUMP];
                return cur_count;
            }
            return g_pouchEquipCheckBadge_trampoline(badge_id);
        });
    // Patch move cost function to use user-selected move badge levels.
    g_BtlUnit_GetWeaponCost_trampoline = patch::HookFunction(
        ttyd::battle_unit::BtlUnit_GetWeaponCost, [](
            BattleUnitInstance* battle_unit, AttackParams* attack_params) {
                if (attack_params && gInBattle && gChangeMovePowersEnabled &&
                    attack_params->item_id >= ItemId::POWER_JUMP &&
                    attack_params->item_id <= ItemId::ICE_SMASH) {
                    int32_t base_cost = attack_params->base_fp_cost;
                    int32_t cur_count =
                        gCurMoveBadgeCounts[attack_params->item_id -
                                            ItemId::POWER_JUMP];
                        
                    if (cur_count > 1 &&
                        (attack_params->item_id == ItemId::POWER_BOUNCE ||
                         attack_params->item_id == ItemId::FIRE_DRIVE)) {
                        // Better base for balance, but leave 1-badge cost alone.
                        base_cost = 4;
                    } else if (attack_params->item_id == ItemId::TORNADO_JUMP) {
                        // It needs the help in any case.
                        base_cost = 2;
                    }
                    // Linear scaling!  Stack 'em to the heavens!
                    int32_t fp_cost = base_cost * cur_count -
                        battle_unit->badges_equipped.flower_saver;
                    return fp_cost < 1 ? 1 : fp_cost;
                } else if (attack_params && gInBattle &&
                           attack_params->item_id == ItemId::PIERCING_BLOW) {
                    // Even without user-selected levels, change Piercing Blow
                    // to use standard exponential stacking.
                    int32_t fp_cost = 1;
                    const int32_t num_piercing_blows =
                        ttyd::mario_pouch::pouchEquipCheckBadge(
                            ItemId::PIERCING_BLOW);
                    for (int32_t i = 0; i < num_piercing_blows; ++i) {
                        fp_cost <<= 1;
                    }
                    fp_cost = fp_cost -
                        battle_unit->badges_equipped.flower_saver;
                    return fp_cost < 1 ? 1 : fp_cost;
                }
                return g_BtlUnit_GetWeaponCost_trampoline(
                    battle_unit, attack_params);
            });
    // Patch move window function to allow user input.
    g_DrawWeaponWin_trampoline = patch::HookFunction(
        ttyd::battle_menu_disp::DrawWeaponWin, []() {
            gSelf->HandleMovePowerLevelSelection();
            g_DrawWeaponWin_trampoline();
        });
        
    // SUPERGUARD COST:
    // Deduct SP on Superguard success, and disallow them if there isn't enough.
    g_BattleActionCommandCheckDefence_trampoline = patch::HookFunction(
        ttyd::battle_ac::BattleActionCommandCheckDefence, [](
            BattleUnitInstance* battle_unit, AttackParams* attack_params) {
            int8_t superguard_frames[7];
            bool restore_superguard_frames = false;
            // Temporarily disable Superguards if Star Power is too low.
            if (ttyd::mario_pouch::pouchGetAP() <
                gSelf->options_.superguard_cost) {
                restore_superguard_frames = true;
                ttyd::system::memcpy_as4(
                    superguard_frames, common::kSuperguardFramesArr, 7);
                for (int32_t i = 0; i < 7; ++i) {
                    common::kSuperguardFramesArr[i] = 0;
                }
            }
            const int32_t defense_result =
                g_BattleActionCommandCheckDefence_trampoline(
                    battle_unit, attack_params);
            // Successful Superguard, subtract SP.
            if (defense_result == 5) {
                ttyd::mario_pouch::pouchAddAP(
                    -gSelf->options_.superguard_cost);
            }
            if (restore_superguard_frames) {
                ttyd::system::memcpy_as4(
                    common::kSuperguardFramesArr, superguard_frames, 7);
            }
            return defense_result;
        });
    // Show the current Star Power in numeric form underneath the status bar.
    g_statusWinDisp_trampoline = patch::HookFunction(
        ttyd::statuswindow::statusWinDisp, []() {
            // Draw the normal menu stuff.
            g_statusWinDisp_trampoline();
            
            // Don't display SP if Mario hasn't gotten any Star Powers yet.
            if (ttyd::mario_pouch::pouchGetMaxAP() < 100) return;
            
            // Don't try to display SP if the status bar is not on-screen.
            float menu_height = common::GetStatusWindowHeight();
            if (menu_height < 100.f || menu_height > 330.f) return;
            
            float matrix[16];
            int32_t unknown_param = -1;
            int32_t current_AP = ttyd::mario_pouch::pouchGetAP();
            ttyd::mtx::PSMTXTrans(matrix, 192.f, menu_height - 100.f, 0.f);
            ttyd::icondrv::iconNumberDispGx(
                matrix, current_AP, 1 /* is_small */, &unknown_param);
        });
        
    // NEW 2.0 PATCH BADGES:
    // Patch _getSickStatusParam so "Toughen Up" gets the correct strength.
    g__getSickStatusParam_trampoline = patch::HookFunction(
        ttyd::battle_damage::_getSickStatusParam, [](
            BattleUnitInstance* battle_unit, AttackParams* attack_params,
            int32_t status_type, int8_t* turn_count, int8_t* strength) {
            // Run vanilla logic.
            g__getSickStatusParam_trampoline(
                battle_unit, attack_params, status_type, turn_count, strength);
              
            // If badge type and status type (DEF-Up) are correct, 
            // change the effect strength based on the number of badges equipped.
            if (status_type == 14 && (
                attack_params->item_id == ItemId::SUPER_CHARGE ||
                attack_params->item_id == ItemId::SUPER_CHARGE_P)) {
                bool is_mario = battle_unit->type_id == ActorTypeId::MARIO;
                int8_t badges = ttyd::mario_pouch::pouchEquipCheckBadge(
                    is_mario ? ItemId::SUPER_CHARGE : ItemId::SUPER_CHARGE_P);
                *strength = badges + 1;
            }
        });
    // Patch pouch HP/FP revision to account for Mini Plus badges.
    g_pouchReviseMarioParam_trampoline = patch::HookFunction(
        ttyd::mario_pouch::pouchReviseMarioParam, []() {
            // Save pre-revised HP and FP values.
            const int16_t previous_hp = ttyd::mario_pouch::pouchGetHP();
            const int16_t previous_fp = ttyd::mario_pouch::pouchGetFP();
            
            // Run vanilla logic.
            g_pouchReviseMarioParam_trampoline();
            
            // Alter HP and FP values as required.
            const int8_t new_max_hp =
                ttyd::mario_pouch::pouchGetPtr()->max_hp +
                ttyd::mario_pouch::pouchEquipCheckBadge(
                    ItemId::SQUARE_DIAMOND_BADGE);
            const int8_t new_max_fp =
                ttyd::mario_pouch::pouchGetPtr()->max_fp +
                ttyd::mario_pouch::pouchEquipCheckBadge(
                    ItemId::SQUARE_DIAMOND_BADGE_P);
            ttyd::mario_pouch::pouchGetPtr()->max_hp = new_max_hp;
            ttyd::mario_pouch::pouchGetPtr()->max_fp = new_max_fp;
            ttyd::mario_pouch::pouchSetHP(
                previous_hp < new_max_hp ? previous_hp : new_max_hp);
            ttyd::mario_pouch::pouchSetFP(
                previous_fp < new_max_fp ? previous_fp : new_max_fp);
        });
    // Patch in-battle HP/FP revision to account for Mini Plus badges.
    g_BtlUnit_ReviseHpFp_trampoline = patch::HookFunction(
        ttyd::battle_unit::BtlUnit_ReviseHpFp, [](
            BattleUnitInstance* battle_unit) {
            // Save pre-revised HP and FP values.
            const int16_t previous_hp = battle_unit->current_hp;
            const int16_t previous_fp = battle_unit->current_fp;
            
            // Run vanilla logic.
            g_BtlUnit_ReviseHpFp_trampoline(battle_unit);
            
            // Alter HP and FP values as required.
            // (Still interacts weirdly w/Badge Bandits, largely not my fault.)
            if (battle_unit->type_id == ActorTypeId::MARIO) {
                const int8_t new_max_hp =
                    battle_unit->max_hp +
                    ttyd::mario_pouch::pouchEquipCheckBadge(
                        ItemId::SQUARE_DIAMOND_BADGE);
                battle_unit->max_hp = new_max_hp;
                battle_unit->current_hp =
                    previous_hp < new_max_hp ? previous_hp : new_max_hp;
            }
            const int8_t new_max_fp =
                battle_unit->max_fp +
                ttyd::mario_pouch::pouchEquipCheckBadge(
                    ItemId::SQUARE_DIAMOND_BADGE_P);
            battle_unit->max_fp = new_max_fp;
            battle_unit->current_fp =
                previous_fp < new_max_fp ? previous_fp : new_max_fp;
        });
    
    // MISC. PATCHES:
    
    // Patch Charlieton's sell price scripts, making items half price in the Pit
    // and capping prices at 999 coins.
    mod::patch::WritePatch(
        kCharlietonGorListHook,
        reinterpret_cast<void*>(CharlietonRogueportPriceListPatchStart),
        reinterpret_cast<void*>(CharlietonRogueportPriceListPatchEnd));
    mod::patch::WritePatch(
        kCharlietonPitListHook,
        reinterpret_cast<void*>(CharlietonPitPriceListPatchStart),
        reinterpret_cast<void*>(CharlietonPitPriceListPatchEnd));
    mod::patch::WritePatch(
        kCharlietonGorItemHook,
        reinterpret_cast<void*>(CharlietonRogueportPriceItemPatchStart),
        reinterpret_cast<void*>(CharlietonRogueportPriceItemPatchEnd));
    mod::patch::WritePatch(
        kCharlietonPitItemHook,
        reinterpret_cast<void*>(CharlietonPitPriceItemPatchStart),
        reinterpret_cast<void*>(CharlietonPitPriceItemPatchEnd));
    
    // Remove the cutscenes for picking up Ultra Hammer and similar upgrades.
    const uint32_t skip_cutscene_opcode = 0x48000030;     // b 0x0030
    mod::patch::WritePatch(
        kSkipUHCutsceneOpcode, &skip_cutscene_opcode, sizeof(uint32_t));
    
    // Change Trade Off's rank up amount to +10 levels instead of 5.
    const uint32_t trade_off_rankup_opcode = 0x3804000A;  // addi r0, r4, 10
    mod::patch::WritePatch(
        kTradeOffRankUpOpcode, &trade_off_rankup_opcode, sizeof(uint32_t));
    
    // Change Trade Off to have a 100% chance of inflicting +3 ATK for 9 turns.
    common::kTradeOffParams->atk_change_chance      = 100;
    common::kTradeOffParams->atk_change_time        = 9;
    common::kTradeOffParams->atk_change_strength    = 3;
    
    // Patch Trade Off's script code to inflict the newly added +ATK status.
    uint32_t script_opcodes[] = {
        0x00020032, 0xFE363C8C, 0x80369B60, 0x0002005B, 0x801054E8,
        0xFE363C8C, 0x0001005E, 0x803652B8, 0x00000002, 0x00000001
    };
    mod::patch::WritePatch(
        kTradeOffScriptHook, script_opcodes, sizeof(script_opcodes));
    
    // Change Koopa Curse to affect all enemies.
    common::kKoopaCurseParams->num_targets = 2;
    
    // Change Piercing Blow to have Hammer Throw's damage scaling when stacked.
    ttyd::system::memcpy_as4(
        &common::kPiercingBlowParams->base_damage_fn,
        &common::kHammerThrowParams->base_damage_fn, 9 * sizeof(int32_t));
    // In this case, determines which equipped badges to count / power to add.
    common::kPiercingBlowParams->_unk_0x38 = ItemId::PIERCING_BLOW;
    
    // Change Super Charge (P) to have a single-turn defense buff instead.
    common::kSuperChargeParams->base_fp_cost = 1;
    common::kSuperChargeParams->charge_strength = 0;
    common::kSuperChargeParams->def_change_chance = 100;
    common::kSuperChargeParams->def_change_time = 1;
    common::kSuperChargeParams->def_change_strength = 2;
    common::kSuperChargeParams->icon_id = kSquareDiamondIconId;
    common::kSuperChargeParams->name_msg = "in_toughen_up";
    common::kSuperChargePartnerParams->base_fp_cost = 1;
    common::kSuperChargePartnerParams->charge_strength = 0;
    common::kSuperChargePartnerParams->def_change_chance = 100;
    common::kSuperChargePartnerParams->def_change_time = 1;
    common::kSuperChargePartnerParams->def_change_strength = 2;
    common::kSuperChargePartnerParams->icon_id = kSquareDiamondIconId;
    common::kSuperChargePartnerParams->name_msg = "in_toughen_up";
    
    // Enable crash handler printout.
    const uint32_t enable_handler_opcode = 0x3800FFFF;  // li r0, -1
    mod::patch::WritePatch(
        kEnableHandlerOpcode, &enable_handler_opcode, sizeof(uint32_t));
        
    // Fix crash when defeating Blooper and tentacles simultaneously.
    mod::patch::WriteBranch(
        reinterpret_cast<void*>(kBlooperCrash1HookAddress),
        reinterpret_cast<void*>(StartPreventBlooperCrash1));
    mod::patch::WriteBranch(
        reinterpret_cast<void*>(BranchBackPreventBlooperCrash1),
        reinterpret_cast<void*>(kBlooperCrash1HookAddress + 0x4));
    mod::patch::WriteBranch(
        reinterpret_cast<void*>(kBlooperCrash2HookAddress),
        reinterpret_cast<void*>(StartPreventBlooperCrash2));
    mod::patch::WriteBranch(
        reinterpret_cast<void*>(BranchBackPreventBlooperCrash2),
        reinterpret_cast<void*>(kBlooperCrash2HookAddress + 0x4));
}

void Shufflizer::Update() {
    // Tick the Shufflizer random number generator so random calls made after
    // the initialization finishes are not obviously deterministic.
    ShufflizerRand(417);
    // Update input, etc. for the menu controller.
    menu_.Update();
}

void Shufflizer::Draw() {
    // Update graphics for the menu.
    menu_.Draw();
}

}