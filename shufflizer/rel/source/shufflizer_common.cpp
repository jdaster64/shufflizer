#include "shufflizer_common.h"

#include <cstdint>
#include <cstdio>
#include <ttyd/common_types.h>
#include <ttyd/msgdrv.h>
#include <ttyd/string.h>
#include <common.h>
#include <shufflizer_state.h>

namespace mod::shufflizer {

namespace {
   
namespace ItemId = ::ttyd::common::ItemId;

// Using TTYD's same RNG function / seed setup for laziness.
uint32_t ShufflizerRandState = 0;

const char* kUltraHammerDialogueSegments[31] = {
    // NOTE: This should never appear, but is a failsafe.
    "Oh, this is awkward...\n<wait 250>"
    "There doesn't seem to be\n"
    "one of those in this seed.\n<k><p>"
    "Might want to report this\n"
    "as a bug on Jdaster64's\n"
    "GitHub page with the seed",
    // Locations based on the flag index.
    "in this very town",
    "under this very town",
    "in Hooktail's castle",
    "in Petalburg",
    "in Petal Meadows",
    "in Twilight Town",
    "on Twilight Trail",
    "around Creepy Steeple",
    "in Glitzville",
    "in Glitzville",
    "in the Boggly Woods",
    "in the Great Tree",
    "in Pirate's Grotto",
    "on Keelhaul Key",
    "in Poshley Heights",
    "on the Excess Express",
    "at Riverside Station",  // hom
    "at Riverside Station",  // eki
    "around Fahr Outpost",
    // These really shouldn't happen, but might be useful later.
    nullptr,  // "on the moon",
    nullptr,  // "in the X-Nauts' fortress",
    nullptr,  // "in the Palace of Shadow",
    // Various placeholders.
    nullptr,  // battle flags part 1
    nullptr,  // battle flags part 2
    nullptr,  // Bowser section
    nullptr,  // ?
    "in the Pit of 100 Trials",
    nullptr,  // ?
    nullptr,  // Trouble Center flags
    
    // The last set of flags is for Shine Sprites / Star Pieces,
    // so if it's one of those, the player can find out via Merluvlee.
    "I don't know where you can\n"
    "find it, but Merluvlee can\n"
    "probably help you out"
};

const char* GetThwompQuizOptions(const SeedState& state, char* buf) {
    // Selects a random item from a late-game shop.
    const ShopInfo& item_1_info =
        state.shops[ShufflizerRand(6) + ShopType::PUNGENT_SHOP];
    const int16_t item_1 = item_1_info.buy_items[ShufflizerRand(6)].item;
    
    const int16_t item_2 =
        state.shops[ShopType::TOAD_BROS_BAZAAR].buy_items[ShufflizerRand(6)].item;
    const int16_t item_3 =
        state.shops[ShopType::NIFF_T_SHOP].buy_items[ShufflizerRand(6)].item;
        
    // Ha ha funny
    const char* kFourthOptions[7] = {
        "Super Slow Go",
        "Debug Badge",
        "Badge 0154 (not real)",
        "Tomato Jump",
        "Berserker",
        "Another Badge",
        "Bowser Jewel"
    };
    sprintf(buf, "<select 0 -1  0 80>\n%s\n%s\n%s\n%s",
            ttyd::msgdrv::msgSearch(common::kItemDataArr[item_1].item_name_msg),
            ttyd::msgdrv::msgSearch(common::kItemDataArr[item_2].item_name_msg),
            ttyd::msgdrv::msgSearch(common::kItemDataArr[item_3].item_name_msg),
            kFourthOptions[ShufflizerRand(7)]);
    return buf;
}

const char* GetFranklyUltraHammerDialogue(int32_t flag_index, char* buf) {
    const char* introductory_text = 
        "Yes, apparently this Ultra\nHammer can be found someplace\n";
    const char* location_text = "";
    
    if (flag_index == -1) {
        introductory_text =
            "Oh, you're in luck! I think\n"
            "the traveling merchant in\n"
            "town has one of those";
    } else if (flag_index < 1024 || flag_index >= 5674) {
        introductory_text = kUltraHammerDialogueSegments[0];
    } else {
        location_text = 
            kUltraHammerDialogueSegments[(flag_index - 1024) / 150];
        if (!location_text) {
            introductory_text = kUltraHammerDialogueSegments[0];
            location_text = "";
        } else if ((flag_index - 1024) / 150 % 30 == 0) {
            // Dummy / Merluvlee text; clear the two lines before it.
            introductory_text = "";
        }
    }
    
    sprintf(buf,
            "%s%s.\n<k><p>"
            "You'll have to figure out how\n"
            "you're going to find it,\n"
            "though, because I don't know.\n"
            "<k><p>"
            "Hmph! <wait 100>Good luck with that.\n"
            "<wait 250>Once you find it, the path to\n"
            "Fahr Outpost will open.\n<k>", introductory_text, location_text);
            
    return buf;
}
    
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
    int32_t item_offset, int32_t bits_per_item) {
    uint32_t shift_amount = 0;
    switch (bits_per_item) {
        case 1: break;
        case 2: shift_amount = 1; break;
        case 4: shift_amount = 2; break;
        default: bits_per_item = 1;
    }
    const uint32_t bitmask = (1U << bits_per_item) - 1;
    for (; start < end; ++start) {
        for (int32_t bit = 0; bit < 32; bit += bits_per_item) {
            for (int32_t num = (*start >> bit) & bitmask; num > 0; --num) {
                *output = item_offset + (bit >> shift_amount);
                ++output;
            }
        }
        item_offset += (32 >> shift_amount);
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

int32_t GetFieldItemSavedFlagIdFromIndex(
    const SavedFlagRow* start, const SavedFlagRow* end, int32_t index) {
    if (index < 0) return -1;
    int32_t num_flags_seen = 0;
    for (; start < end; ++start) {
        for (int32_t bit = 0; bit < 16; ++bit) {
            if (start->bits & (1U << bit)) {
                if (num_flags_seen == index) return start->start_offset + bit;
                ++num_flags_seen;
            }
        }
    }
    // Not enough flags in bitfield to reach index.
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

const char* GetReplacementMessage(const char* msg_key, const SeedState& state) {
    // Do not use for multiple messages that can be displayed at once!
    static char buf[512];
    
    // TODO: Consider replacing these compares with a single hash and
    // comparison if many more strings are added.
    if (!ttyd::string::strcmp(msg_key, "in_cake")) {
        return "Strawberry Cake";
    } else if (!ttyd::string::strcmp(msg_key, "msg_kame_no_noroi")) {
        return "Has a chance of inducing Slow \n"
               "status on all foes.";
    } else if (!ttyd::string::strcmp(msg_key, "msg_cake")) {
        return "Scrumptious strawberry cake \n"
               "that heals 15 HP and 15 FP.";
    } else if (!ttyd::string::strcmp(msg_key, "msg_teki_kyouka")) {
        return "Boosts foes' level by 10, but \n"
               "temporarily gives them +3 ATK!";
    } else if (!ttyd::string::strcmp(msg_key, "msg_ice_candy")) {
        return "A dessert made by Zess T.\n"
               "Gives 15 FP, but might freeze!";
    } else if (!ttyd::string::strcmp(msg_key, "list_ice_candy")) {
        return "A dessert made by Zess T.\n"
               "Gives 15 FP, but might freeze!\n"
               "Made by mixing Honey Syrup \n"
               "with an Ice Storm.";
    } else if (!ttyd::string::strcmp(msg_key, "msg_nancy_frappe")) {
        return "A dessert made by Zess T.\n"
               "Gives 20 FP, but might freeze!";
    } else if (!ttyd::string::strcmp(msg_key, "list_nancy_frappe")) {
        return "A dessert made by Zess T.\n"
               "Gives 20 FP, but might freeze!\n"
               "Made by mixing Maple Syrup \n"
               "with an Ice Storm.";
    } else if (!ttyd::string::strcmp(msg_key, "in_toughen_up")) {
        return "Toughen Up";
    } else if (!ttyd::string::strcmp(msg_key, "in_toughen_up_p")) {
        return "Toughen Up P";
    } else if (!ttyd::string::strcmp(msg_key, "in_mini_hp_up")) {
        return "Mini-HP Plus";
    } else if (!ttyd::string::strcmp(msg_key, "in_mini_fp_up")) {
        return "Mini-FP Plus";
    } else if (!ttyd::string::strcmp(msg_key, "msg_toughen_up")) {
        return "Wear this to add Toughen Up\n"
               "to Mario's Tactics menu.\n"
               "This uses 1 FP to raise DEF\n"
               "by 2 points for a turn.\n"
               "Wearing more copies raises\n"
               "the effect and FP cost.";
    } else if (!ttyd::string::strcmp(msg_key, "msg_toughen_up_p")) {
        return "Wear this to add Toughen Up\n"
               "to partners' Tactics menu.\n"
               "This uses 1 FP to raise DEF\n"
               "by 2 points for a turn.\n"
               "Wearing more copies raises\n"
               "the effect and FP cost.";
    } else if (!ttyd::string::strcmp(msg_key, "msg_mini_hp_up")) {
        return "Increase maximum HP by 1.";
    } else if (!ttyd::string::strcmp(msg_key, "msg_mini_fp_up")) {
        return "Increase maximum FP by 1.";
    } else if (!ttyd::string::strcmp(
        msg_key, "btl_hlp_cmd_operation_super_charge")) {
        return "Briefly increases DEF by\n"
               "more than Defending.";
    } else if (!ttyd::string::strcmp(msg_key, "stg1_hei_24")) {
        return "<boss><se 2>\n"
               "On to question two!\n"
               "<k>\n<p>\n"
               "Now then,<wait 50> which of the\n"
               "following can be bought at\n"
               "the shop in Petalburg?\n<o>";
    } else if (!ttyd::string::strcmp(msg_key, "stg1_hei_q_02")) {
        return GetThwompQuizOptions(state, buf);
    } else if (!ttyd::string::strcmp(msg_key, "mac_6_013") ||
               !ttyd::string::strcmp(msg_key, "gor_02_026_01")) {
        if (common::HasKeyItemInInventory(ItemId::ULTRA_HAMMER)) return nullptr;
        
        // Change the text describing the location of the Ultra Hammer
        // based on its shuffled location if the player hasn't found it yet.
        const int32_t flag_index = state.ultra_hammer_flag;
        return GetFranklyUltraHammerDialogue(flag_index, buf);
    }
           
    // No string for the given message.
    return nullptr;
}

}
