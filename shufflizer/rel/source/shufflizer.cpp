#include "shufflizer.h"

#include <cstdint>
#include <ttyd/battle_damage.h>
#include <ttyd/battle_unit.h>
#include <ttyd/cardmgr.h>
#include <ttyd/common_types.h>
#include <ttyd/event.h>
#include <ttyd/evt_mobj.h>
#include <ttyd/itemdrv.h>
#include <ttyd/mobjdrv.h>
#include <ttyd/msgdrv.h>
#include <ttyd/OSCache.h>
#include <ttyd/OSLink.h>
#include <ttyd/string.h>
#include <ttyd/system.h>

#include "common.h"
#include "patch.h"
#include "shufflizer_common.h"

extern "C" {
    void CharlietonPitPriceListPatchStart();
    void CharlietonPitPriceItemPatchStart();
    void CharlietonRogueportPriceListPatchStart();
    void CharlietonRogueportPriceItemPatchStart();
    void CharlietonPitPriceListPatchEnd();
    void CharlietonPitPriceItemPatchEnd();
    void CharlietonRogueportPriceListPatchEnd();
    void CharlietonRogueportPriceItemPatchEnd();
}

namespace mod::shufflizer {

namespace {
    
using ::ttyd::battle_unit::BattleUnitParams;
using ::ttyd::battle_unit::BattlePartySlotInfo;
using ::ttyd::common::AttackParams;
using ::ttyd::common::ItemData;
using ::ttyd::common::ShopItemData;
using ::ttyd::oslink::OSModuleInfo;
namespace ActorTypeId   = ::ttyd::common::ActorTypeId;
namespace ItemId        = ::ttyd::common::ItemId;
namespace ModuleId      = ::ttyd::common::ModuleId;

// Bitfields for badges of which there should be one-or-more, and two-or-more
// copies in the shuffle pool, offset by the first badge ID (POWER_JUMP).
const uint32_t kOneCopyBadges[] = { 0xffffffffU, 0xffffffffU, 0x01ffefffU };
const uint32_t kTwoCopyBadges[] = { 0x303ffb7fU, 0x787107ffU, 0x00300000U };
// Bitfield for items that can appear on the field, offset by THUNDER_BOLT.
const uint32_t kFieldItems[]    = { 0xffffffffU, 0x0006001fU };
// Bitfield for items, recipes, and stackable badges that can appear in shops,
// offset by THUNDER_BOLT.  Gold Bar and Gold Bar x3 are not included.
const uint32_t kShopItems[]     = { 
    0xffffffffU, 0xfffe001fU, 0xffffffffU, 0x3bff0fffU, 0x0fffffffU, 0x078ffff7U
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
    // Ultra Hammer disallowed.
    { 0x0540, 0x7400 }, { 0x0550, 0x007C }, { 0x05D0, 0xE000 },
    { 0x05E0, 0x0033 }, { 0x0650, 0x1000 }, { 0x06F0, 0xCF14 },
    { 0x0700, 0x477C }, { 0x09D0, 0x1180 }, { 0x09E0, 0x0020 },
    { 0x0A70, 0xF8B0 }, { 0x0B00, 0x1200 }, { 0x0B10, 0x3060 },
    { 0x0B20, 0x5020 }, { 0x0B30, 0x0620 }, { 0x0B40, 0x0020 },
    { 0x0F20, 0x8000 }, { 0x0F30, 0x0018 }, { 0x0FC0, 0x00F8 },
    { 0x1060, 0x0040 }, { 0x1070, 0x01D8 }, { 0x1110, 0x8FD8 },
    { 0x1120, 0x07BF }, { 0x13D0, 0x1FF8 }, { 0x1590, 0x7220 },
    { 0x15A0, 0x07FC }, { 0x15B0, 0x6000 }, { 0x15C0, 0xFFFF },
    { 0x15D0, 0xEFFF }, { 0x15E0, 0xFFFF }, { 0x15F0, 0x01FF },
    { 0x1610, 0xFC00 }, { 0x1620, 0x0007 }
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

// Addresses to write patches to Charlieton code (to avoid > 999 prices).
void* kCharlietonGorListHook    = reinterpret_cast<uint32_t*>(0x8023c0e4);
void* kCharlietonPitListHook    = reinterpret_cast<uint32_t*>(0x8023c124);
void* kCharlietonGorItemHook    = reinterpret_cast<uint32_t*>(0x8023d26c);
void* kCharlietonPitItemHook    = reinterpret_cast<uint32_t*>(0x8023d2e0);

// Miscellaneous constants.
int16_t kKoopaCurseIconId       = 390;

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
    item_db[ItemId::KOOPA_CURSE].icon_id = kKoopaCurseIconId;
    item_db[ItemId::FP_DRAIN_P].bp_cost = 1;
    
    // Set recipe prices based on sell price, badge Star Piece costs on BP cost,
    // and fix unused badges' sort order.
    for (int32_t i = 0; i < ItemId::MAX_ITEM_ID; ++i) {
        ItemData& item = item_db[i];
        if (item.buy_price == 10 && item.sell_price > 8) {
            item.buy_price = item.sell_price * 5 / 4;
        }
        if (i >= ItemId::POWER_JUMP) {
            item.star_piece_price = item.bp_cost + 1;
            
            if (item.type_sort_order > 0x49) {
                item.type_sort_order += 4;
            } else if (item.type_sort_order > 0x43) {
                item.type_sort_order += 3;
            } else if (item.type_sort_order > 0x3b) {
                item.type_sort_order += 2;
            } else if (item.type_sort_order > 0x24) {
                item.type_sort_order += 1;
            }
        }
    }
    
    // Fixed sort order for unused 'P' badges.
    item_db[ItemId::ALL_OR_NOTHING_P].type_sort_order   = 0x24 + 1;
    item_db[ItemId::LUCKY_DAY_P].type_sort_order        = 0x3b + 2;
    item_db[ItemId::PITY_FLOWER_P].type_sort_order      = 0x43 + 3;
    item_db[ItemId::FP_DRAIN_P].type_sort_order         = 0x49 + 4;
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
void* (*g_itemEntry_trampoline)(
    const char*, uint32_t, uint32_t, int32_t, void*, float, float, float) = nullptr;
int32_t (*g_mobj_powerupblk_trampoline)(void*) = nullptr;
int32_t (*g_mobj_itembox_trampoline)(void*) = nullptr;
void (*g_mobjRunEvent_trampoline)(void*, void*) = nullptr;
int32_t (*g_BattleCalculateDamage_trampoline)(
    void*, void*, void*, AttackParams*, void*, uint32_t) = nullptr;
int32_t (*g_BattleCalculateFpDamage_trampoline)(
    void*, void*, void*, AttackParams*, void*, uint32_t) = nullptr;
void* (*g_BtlUnit_Entry_trampoline)(
    BattlePartySlotInfo*, float, float) = nullptr;
const char* (*g_msgSearch_trampoline)(const char*) = nullptr;
    
// Global state sentinels for item replacement functions.
int32_t gShineBlockFlag = -1;
bool gInItemBoxRoutine = false;
    
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
    
    // 3. Initialize the shuffled one-time item pool.
    // Items are distributed as follows:
    // - 27 Howz badges, 15 Dazzle badges, 16 Charlieton items,
    //   286 field items (103 of which are eligible for Ultra Hammer).
    // - 133 badges, 143 items, 25 coins, 41 Shines, Strange Sack, Ultra Hammer.
    //   (Some of the items are replaced by Star Pieces, per Dazzle's badges.)
    int16_t items[344];
    int16_t *pos, *items_end, *badges_end, *shines_end;
    int32_t* assign_pos;
    
    // Add all badges with one+ and two+ copies.
    pos = CreateItemArrayFromBitfield(
        kOneCopyBadges, kOneCopyBadges + 3, items, ItemId::POWER_JUMP);
    pos = CreateItemArrayFromBitfield(
        kTwoCopyBadges, kTwoCopyBadges + 3, pos, ItemId::POWER_JUMP);
    // Add third copies of these badges, since that's their maximum effect.
    *pos++ = ItemId::SIMPLIFIER;
    *pos++ = ItemId::UNSIMPLIFIER;
    badges_end = pos;
    
    *pos++ = ItemId::STRANGE_SACK;
    
    // 39 items x4 copies, set the ptr back 13 spots for a total of 143.
    for (int32_t i = 0; i < 4; ++i) {
        pos = CreateItemArrayFromBitfield(
            kFieldItems, kFieldItems + 2, pos, ItemId::THUNDER_BOLT);
    }
    pos -= 13;
    items_end = pos;
    
    for (int32_t i = 0; i < 25; ++i) {
        *pos++ = ItemId::COIN;
    }
    for (int32_t i = 0; i < 41; ++i) {
        *pos++ = ItemId::SHINE_SPRITE;
    }
    shines_end = pos;
    // One empty space left at end for the Ultra Hammer.
    
    // 4. Shuffle badges, and assign Howz and Dazzle's badge stock.
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
    
    // 5. Replace the last X-2 items with Star Pieces.
    for (int32_t i = 0; i < star_pieces; ++i) {
        *--items_end = ItemId::STAR_PIECE;
    }
    
    // 6. Perform shuffles to uniformly distribute valid items to Charlieton,
    // and UH-allowed / disallowed field item slots.
    
    // Shuffle remaining badges, items, and the Strange Sack.
    ShuffleRange(pos, items_end);
    // Shuffle all but the first 16 of these, and everything else except the UH.
    ShuffleRange(pos + 16, shines_end);
    // Swap the Ultra Hammer with a random Charlieton or UH-allowed field item.
    int16_t* swap_pos = pos + ShufflizerRand(16 + 103);
    *shines_end = *swap_pos;
    *swap_pos = ItemId::ULTRA_HAMMER;
    
    // 7. Assign Rogueport Charlieton's stock of items.
    assign_pos = common::kCharlietonItemArr;
    for (int32_t i = 0; i < 16; ++i) {
        *assign_pos++ = *pos++;
    }
    
    // 8. Copy remaining items to the field item seed state.
    ttyd::system::memcpy_as4(seed_state_.field_items, pos, 286 * sizeof(int16_t));
    
    // 9. Put together list of possible shop items.
    // - 39 normal, 57 recipes, 64 stackable badge types.
    int16_t shop_items[160];
    pos = CreateItemArrayFromBitfield(
        kShopItems, kShopItems + 6, shop_items, ItemId::THUNDER_BOLT);
    // Dried Shroom and Dizzy Dial are not randomized, but Gold Bars should be
    // included in the normal shops.
    shop_items[ItemId::DRIED_SHROOM - ItemId::THUNDER_BOLT] = ItemId::GOLD_BAR;
    shop_items[ItemId::DIZZY_DIAL - ItemId::THUNDER_BOLT] = ItemId::GOLD_BAR_X3;
    
    // 10. Shuffle recipes and badges, then shuffle first 77 items.
    // (60 shop + 20 Pianta slots - Dried Shroom, Dizzy Dial, and Cake Mix)
    ShuffleRange(shop_items + 39, pos);
    ShuffleRange(shop_items, shop_items + 77);
    
    // 11. Fill out normal shop item data.
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
    
    // 12. Fill out Pianta Parlor tables.
    assign_pos = common::kPiantaParlorItemArr;
    // Hardcode Cake Mix to first slot.
    *assign_pos++ = ItemId::CAKE_MIX;
    for (int32_t i = 1; i < 20; ++i) {
        // Leave breaks between every card's items.
        if (!(i & 3)) ++assign_pos;
        *assign_pos++ = *pos++;
    }
    
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
}

void Shufflizer::OnModuleLoaded(ttyd::oslink::OSModuleInfo* module_info) {
    if (module_info == nullptr) return;
    int32_t module_id = module_info->id;
    uintptr_t module_ptr = reinterpret_cast<uintptr_t>(module_info);
    
    // Pit of 100 Trials.
    if (module_id == ModuleId::JON) {
        // Assign Charlieton a set of six random items.
        int32_t* item_pos = common::kPitCharlietonItemArr;
        for (int32_t i = 0; i < 6; ++i) {
            *item_pos++ = GetRandomItemFromBitfield(
                kShopItems, kShopItems + 6, ItemId::THUNDER_BOLT);
        }
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
}

int16_t Shufflizer::ReplaceGeneralItem(int16_t id, int32_t collection_expr) {
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
    unit_params->max_hp = hp_stat;
    
    // Set sentinel bit so enemy's stats aren't changed multiple times.
    unit_params->run_rate = unit_params->run_rate | 1;
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
    
    // Patch in fixed text for (the altered) Trade Off, Cake, and Koopa Curse.
    g_msgSearch_trampoline = patch::HookFunction(
        ttyd::msgdrv::msgSearch, [](const char* msg_key) {
            // TODO: Consider replacing these compares with a single hash and
            // comparison if many more strings are added.
            if (!ttyd::string::strcmp(msg_key, "in_cake")) {
                return "Strawberry Cake";
            } else if (!ttyd::string::strcmp(msg_key, "msg_kame_no_noroi")) {
                return "Has a chance of inducing Slow \n"
                       "status on all foes.";
            } else if (!ttyd::string::strcmp(msg_key, "msg_cake")) {
                return "Scrumptious strawberry cake \n"
                       "that heals 15 HP and FP.";
            } else if (!ttyd::string::strcmp(msg_key, "msg_teki_kyouka")) {
                return "Boosts foes' level by 10, but \n"
                       "temporarily gives them +3 ATK!";
            }
            return g_msgSearch_trampoline(msg_key);
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
    common::kTradeOffParams->atk_chance_strength    = 3;
    
    // Patch Trade Off's script code to inflict the newly added +ATK status.
    uint32_t script_opcodes[] = {
        0x00020032, 0xFE363C8C, 0x80369B60, 0x0002005B, 0x801054E8,
        0xFE363C8C, 0x0001005E, 0x803652B8, 0x00000002, 0x00000001
    };
    mod::patch::WritePatch(
        kTradeOffScriptHook, script_opcodes, sizeof(script_opcodes));
    
    // Change Koopa Curse to affect all enemies.
    common::kKoopaCurseParams->num_targets = 2;
    
    // Enable crash handler printout.
    const uint32_t enable_handler_opcode = 0x3800FFFF;  // li r0, -1
    mod::patch::WritePatch(
        kEnableHandlerOpcode, &enable_handler_opcode, sizeof(uint32_t));
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