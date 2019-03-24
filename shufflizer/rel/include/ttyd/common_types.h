#pragma once

#include <cstdint>

// Speculated TTYD enums / types that aren't tied to any particular known file.
// Might be moved to somewhere more fitting in a later revision.
namespace ttyd::common {

namespace ButtonId {
    enum e {
        DPAD_LEFT   = 0x0001,
        DPAD_RIGHT  = 0x0002,
        DPAD_DOWN   = 0x0004,
        DPAD_UP     = 0x0008,
        Z           = 0x0010,
        R           = 0x0020,
        L           = 0x0040,
        //          = 0x0080,
        A           = 0x0100,
        B           = 0x0200,
        X           = 0x0400,
        Y           = 0x0800,
        START       = 0x1000,
    };
}
    
namespace ModuleId {
    enum e {
        INVALID_MODULE = 0,
        AAA,
        AJI,  // x-naut fortress
        BOM,  // fahr outpost
        DMO,
        DOU,  // pirate's grotto
        EKI,  // riverside station
        END,  // credits?
        GON,  // hooktail's castle
        GOR,  // rogueport
        GRA,  // twilight trail
        HEI,  // petal meadows
        HOM,  // riverside station facade / train cutscenes
        JIN,  // creepy steeple
        JON,  // pit of 100 trials
        KPA,  // bowser's castle intermissions
        LAS,  // palace of shadow
        MOO,  // moon
        MRI,  // great tree
        MUJ,  // keelhaul key
        NOK,  // petalburg
        PIK,  // poshley heights / sanctum
        RSH,  // excess express
        SYS,
        TIK,  // rogueport underground
        TOU,  // outer glitz pit
        TOU2, // glitz pit arena
        USU,  // twilight town
        WIN,  // boggly woods
        YUU,
        MAX_MODULE_ID,
    };
}

namespace ActorTypeId {
    enum e {
        INVALID_ACTOR = 0,
        GOOMBA,
        PARAGOOMBA,
        SPIKY_GOOMBA,
        SPINIA,
        SPANIA,
        LORD_CRUMP_PROLOGUE,
        GUS,
        BLOOPER,
        LEFT_TENTACLE,
        RIGHT_TENTACLE,
        KOOPATROL,
        MAGIKOOPA,
        MAGIKOOPA_CLONE,
        KOOPA_TROOPA,
        PARATROOPA,
        FUZZY,
        DULL_BONES,
        BALD_CLEFT,
        BRISTLE,
        GOLD_FUZZY,
        FUZZY_HORDE,
        RED_BONES,
        HOOKTAIL,
        DARK_PUFF,
        PALE_PIRANHA,
        CLEFT,
        PIDER,
        X_NAUT,
        YUX,
        MINI_YUX,
        BELDAM_CH_2,
        MARILYN_CH_2,
        VIVIAN_CH_2,
        MAGNUS,
        X_FIST,
        GOOMBA_GLITZVILLE,
        KP_KOOPA,
        KP_PARATROOPA,
        POKEY,
        LAKITU,
        SPINY,
        HYPER_BALD_CLEFT,
        BOB_OMB,
        BANDIT,
        BIG_BANDIT,
        RED_SPIKY_BUZZY,
        SHADY_KOOPA,
        SHADY_PARATROOPA,
        RED_MAGIKOOPA,
        RED_MAGIKOOPA_CLONE,
        WHITE_MAGIKOOPA,
        WHITE_MAGIKOOPA_CLONE,
        GREEN_MAGIKOOPA,
        GREEN_MAGIKOOPA_CLONE,
        DARK_CRAW,
        HAMMER_BRO,
        BOOMERANG_BRO,
        FIRE_BRO,
        RED_CHOMP,
        DARK_KOOPATROL,
        IRON_CLEFT_RED,
        IRON_CLEFT_GREEN,
        BOWSER_CH_3,
        RAWK_HAWK,
        MACHO_GRUBBA,
        HYPER_GOOMBA,
        HYPER_PARAGOOMBA,
        HYPER_SPIKY_GOOMBA,
        CRAZEE_DAYZEE,
        AMAZY_DAYZEE,
        HYPER_CLEFT,
        BUZZY_BEETLE,
        SPIKE_TOP,
        SWOOPER,
        BOO,
        ATOMIC_BOO,
        DOOPLISS_CH_4_FIGHT_1,
        DOOPLISS_CH_4_INVINCIBLE,
        DOOPLISS_CH_4_FIGHT_2,
        GOOMBELLA_CH_4,
        KOOPS_CH_4,
        YOSHI_CH_4,
        FLURRIE_CH_4,
        EMBER,
        LAVA_BUBBLE,
        GREEN_FUZZY,
        FLOWER_FUZZY,
        PUTRID_PIRANHA,
        PARABUZZY,
        BILL_BLASTER,
        BULLET_BILL,
        BULKY_BOB_OMB,
        CORTEZ,
        CORTEZ_BONE_PILE,
        CORTEZ_SWORD,
        CORTEZ_HOOK,
        CORTEZ_RAPIER,
        CORTEZ_SABER,
        LORD_CRUMP_CH_5,
        X_NAUTS_CRUMP_FORMATION_1,
        X_NAUTS_CRUMP_FORMATION_2,
        X_NAUTS_CRUMP_FORMATION_3,
        RUFF_PUFF,
        POISON_POKEY,
        SPIKY_PARABUZZY,
        DARK_BOO,
        SMORG,
        SMORG_MIASMA_TENTACLE_A,
        SMORG_MIASMA_TENTACLE_B,
        SMORG_MIASMA_TENTACLE_C,
        SMORG_MIASMA_CLAW,
        ICE_PUFF,
        FROST_PIRANHA,
        MOON_CLEFT,
        Z_YUX,
        MINI_Z_YUX,
        X_YUX,
        MINI_X_YUX,
        X_NAUT_PHD,
        ELITE_X_NAUT,
        MAGNUS_2_0,
        X_PUNCH,
        SWOOPULA,
        PHANTOM_EMBER,
        BOMBSHELL_BILL_BLASTER,
        BOMBSHELL_BILL,
        CHAIN_CHOMP,
        DARK_WIZZERD,
        DARK_WIZZERD_CLONE,
        DRY_BONES,
        DARK_BONES,
        GLOOMTAIL,
        BELDAM_CH_8,
        MARILYN_CH_8,
        DOOPLISS_CH_8,
        DOOPLISS_CH_8_FORM_1,       // TODO: Identify forms
        DOOPLISS_CH_8_FORM_2,
        DOOPLISS_CH_8_FORM_3,
        DOOPLISS_CH_8_FORM_4,
        DOOPLISS_CH_8_FORM_5,
        DOOPLISS_CH_8_FORM_6,
        DOOPLISS_CH_8_FORM_7,
        DOOPLISS_CH_8_FORM_8,
        BOWSER_CH_8,
        KAMMY_KOOPA,
        GRODUS,
        GRODUS_X,
        SHADOW_QUEEN_PHASE_1,
        SHADOW_QUEEN_INVINCIBLE,
        SHADOW_QUEEN_PHASE_2,
        LEFT_RIGHT_HAND,
        DEAD_HANDS,
        GLOOMBA,
        PARAGLOOMBA,
        SPIKY_GLOOMBA,
        DARK_KOOPA,
        DARK_PARATROOPA,
        BADGE_BANDIT,
        DARK_LAKITU,
        SKY_BLUE_SPINY,
        WIZZERD,
        PIRANHA_PLANT,
        SPUNIA,                     // Spunia
        ARANTULA,
        DARK_BRISTLE,
        POISON_PUFF,
        SWAMPIRE,
        BOB_ULK,
        ELITE_WIZZERD,
        ELITE_WIZZERD_CLONE,
        BONETAIL,
        // Unknown slots 0xAC ~ 0xDA
        BOMB_SQUAD_BOMB = 0xDB,
        SYSTEM          = 0xDC,
        MARIO           = 0xDE,
        SHELL_SHIELD    = 0xDF,
        GOOMBELLA       = 0xE0,
        KOOPS           = 0xE1,
        YOSHI           = 0xE2,
        FLURRIE         = 0xE3,
        VIVIAN          = 0xE4,
        BOBBERY         = 0xE5,
        MS_MOWZ         = 0xE6,
    };
}

namespace ItemId {
    enum e {
        INVALID_ITEM = 0,
        STRANGE_SACK,
        INVALID_ITEM_PAPER_MODE_ICON,
        INVALID_ITEM_TUBE_MODE_ICON,
        INVALID_ITEM_PLANE_MODE_ICON,
        INVALID_ITEM_BOAT_MODE_ICON,
        BOOTS,
        SUPER_BOOTS,
        ULTRA_BOOTS,
        HAMMER,
        SUPER_HAMMER,
        ULTRA_HAMMER,
        CASTLE_KEY_000C,
        CASTLE_KEY_000D,
        CASTLE_KEY_000E,
        CASTLE_KEY_000F,
        RED_KEY_0010,
        BLUE_KEY_0011,
        STORAGE_KEY_0012,
        STORAGE_KEY_0013,
        GROTTO_KEY_0014,
        SHOP_KEY_0015,
        STEEPLE_KEY_0016,
        STEEPLE_KEY_0017,
        STATION_KEY_0018,
        STATION_KEY_0019,
        CARD_KEY_001A,
        ELEVATOR_KEY_001B,
        ELEVATOR_KEY_001C,
        CARD_KEY_001D,
        CARD_KEY_001E,
        CARD_KEY_001F,
        CARD_KEY_0020,
        BLACK_KEY_0021,
        BLACK_KEY_0022,
        BLACK_KEY_0023,
        BLACK_KEY_0024,
        STAR_KEY_0025,
        PALACE_KEY_0026,
        PALACE_KEY_0027,
        PALACE_KEY_0028,
        PALACE_KEY_0029,
        PALACE_KEY_002A,
        PALACE_KEY_002B,
        PALACE_KEY_002C,
        PALACE_KEY_002D,
        PALACE_KEY_002E,
        PALACE_KEY_002F,
        PALACE_KEY_0030,
        HOUSE_KEY_0031,
        MAGICAL_MAP,
        CONTACT_LENS,
        BLIMP_TICKET,
        TRAIN_TICKET,
        MAILBOX_SP,
        SUPER_LUIGI,
        SUPER_LUIGI_2,
        SUPER_LUIGI_3,
        SUPER_LUIGI_4,
        SUPER_LUIGI_5,
        COOKBOOK,
        MOON_STONE,
        SUN_STONE,
        NECKLACE,
        PUNI_ORB,
        CHAMPS_BELT,
        POISONED_CAKE,
        SUPERBOMBOMB,
        THE_LETTER_P,
        OLD_LETTER,
        CHUCKOLA_COLA,
        SKULL_GEM,
        GATE_HANDLE,
        WEDDING_RING,
        GALLEY_POT,
        GOLD_RING,
        SHELL_EARRINGS,
        AUTOGRAPH,
        RAGGED_DIARY,
        BLANKET,
        VITAL_PAPER,
        BRIEFCASE,
        GOLDBOB_GUIDE,
        INVALID_ITEM_PAPER_0053,
        INVALID_ITEM_PAPER_0054,
        COG,
        DATA_DISK,
        SHINE_SPRITE,       // 0x0057
        ULTRA_STONE,
        INVALID_ITEM_BOWSER_MEAT_0059,
        INVALID_ITEM_MARIO_POSTER_005A,
        SPECIAL_CARD,
        PLATINUM_CARD,
        GOLD_CARD,
        SILVER_CARD,
        BOX,
        MAGICAL_MAP_LARGE,
        DUBIOUS_PAPER,
        ROUTING_SLIP,
        WRESTLING_MAG,
        PRESENT,
        BLUE_POTION,
        RED_POTION,
        ORANGE_POTION,
        GREEN_POTION,
        INVALID_ITEM_STAR_FN0OW_0069,
        LOTTERY_PICK,
        BATTLE_TRUNKS,
        UP_ARROW,
        PACKAGE,
        ATTACK_FX_B_KEY_ITEM,
        INVALID_ITEM_006F,
        INVALID_ITEM_0070,
        INVALID_ITEM_0071,
        DIAMOND_STAR,
        EMERALD_STAR,
        GOLD_STAR,
        RUBY_STAR,
        SAPPHIRE_STAR,
        GARNET_STAR,
        CRYSTAL_STAR,
        COIN,               // 0x0079
        PIANTA,
        HEART_PICKUP,
        FLOWER_PICKUP,
        STAR_PIECE,         // 0x007d
        GOLD_BAR,           // 0x007e
        GOLD_BAR_X3,
        THUNDER_BOLT,       // 0x0080
        THUNDER_RAGE,
        SHOOTING_STAR,
        ICE_STORM,
        FIRE_FLOWER,
        EARTH_QUAKE,
        BOOS_SHEET,
        VOLT_SHROOM,
        REPEL_CAPE,
        RUIN_POWDER,
        SLEEPY_SHEEP,
        POW_BLOCK,
        STOPWATCH,
        DIZZY_DIAL,
        POWER_PUNCH,
        COURAGE_SHELL,
        HP_DRAIN_ITEM,
        TRADE_OFF,          // 0x0091    
        MINI_MR_MINI,
        MR_SOFTENER,
        MUSHROOM,
        SUPER_SHROOM,
        ULTRA_SHROOM,
        LIFE_SHROOM,
        DRIED_SHROOM,
        TASTY_TONIC,
        HONEY_SYRUP,
        MAPLE_SYRUP,
        JAMMIN_JELLY,
        SLOW_SHROOM,
        GRADUAL_SYRUP,
        HOT_DOG,
        CAKE,
        POINT_SWAP,         // 0x00a1    
        FRIGHT_MASK,
        MYSTERY,
        INN_COUPON,
        WHACKA_BUMP,        // 0x00a5
        COCONUT,
        DRIED_BOUQUET,
        MYSTIC_EGG,
        GOLDEN_LEAF,
        KEEL_MANGO,
        FRESH_PASTA,
        CAKE_MIX,
        HOT_SAUCE,
        TURTLEY_LEAF,
        HORSETAIL,
        PEACHY_PEACH,
        SPITE_POUCH,        // 0x00b1
        KOOPA_CURSE,
        SHROOM_FRY,         // 0x00b3
        SHROOM_ROAST,
        SHROOM_STEAK,
        MISTAKE,
        HONEY_SHROOM,
        MAPLE_SHROOM,
        JELLY_SHROOM,
        HONEY_SUPER,
        MAPLE_SUPER,
        JELLY_SUPER,
        HONEY_ULTRA,
        MAPLE_ULTRA,
        JELLY_ULTRA,
        SPICY_SOUP,
        ZESS_DINNER,
        ZESS_SPECIAL,
        ZESS_DELUXE,
        ZESS_DYNAMITE,
        ZESS_TEA,
        SPACE_FOOD,
        ICICLE_POP,
        ZESS_FRAPPE,
        SNOW_BUNNY,
        COCONUT_BOMB,
        COURAGE_MEAL,
        SHROOM_CAKE,
        SHROOM_CREPE,
        MOUSSE_CAKE,
        FRIED_EGG,
        FRUIT_PARFAIT,
        EGG_BOMB,
        INK_PASTA,
        SPAGHETTI,
        SHROOM_BROTH,
        POISON_SHROOM,
        CHOCO_CAKE,
        MANGO_DELIGHT,
        LOVE_PUDDING,
        METEOR_MEAL,
        TRIAL_STEW,
        COUPLES_CAKE,
        INKY_SAUCE,
        OMELETTE_MEAL,
        KOOPA_TEA,
        KOOPASTA,
        SPICY_PASTA,
        HEARTFUL_CAKE,
        PEACH_TART,
        ELECTRO_POP,
        FIRE_POP,
        HONEY_CANDY,
        COCO_CANDY,
        JELLY_CANDY,
        ZESS_COOKIE,
        HEALTHY_SALAD,
        KOOPA_BUN,
        FRESH_JUICE,
        INVALID_ITEM_CAN,   // 0x00ec  
        INVALID_ITEM_ROCK,
        INVALID_ITEM_BONE,
        INVALID_ITEM_HAMMER,
        POWER_JUMP,         // 0x00f0
        MULTIBOUNCE,
        POWER_BOUNCE,
        TORNADO_JUMP,
        SHRINK_STOMP,
        SLEEPY_STOMP,
        SOFT_STOMP,
        POWER_SMASH,
        QUAKE_HAMMER,
        HAMMER_THROW,
        PIERCING_BLOW,
        HEAD_RATTLE,
        FIRE_DRIVE,
        ICE_SMASH,
        DOUBLE_DIP,
        DOUBLE_DIP_P,
        CHARGE,
        CHARGE_P,
        SUPER_APPEAL,
        SUPER_APPEAL_P,
        POWER_PLUS,
        POWER_PLUS_P,
        P_UP_D_DOWN,
        P_UP_D_DOWN_P,
        ALL_OR_NOTHING,
        ALL_OR_NOTHING_P,
        MEGA_RUSH,
        MEGA_RUSH_P,
        POWER_RUSH,
        POWER_RUSH_P,
        P_DOWN_D_UP,
        P_DOWN_D_UP_P,
        LAST_STAND,
        LAST_STAND_P,
        DEFEND_PLUS,
        DEFEND_PLUS_P,
        DAMAGE_DODGE,
        DAMAGE_DODGE_P,
        HP_PLUS,
        HP_PLUS_P,
        FP_PLUS,
        FLOWER_SAVER,
        FLOWER_SAVER_P,
        ICE_POWER,
        SPIKE_SHIELD,
        FEELING_FINE,
        FEELING_FINE_P,
        ZAP_TAP,
        DOUBLE_PAIN,
        JUMPMAN,
        HAMMERMAN,
        RETURN_POSTAGE,
        HAPPY_HEART,
        HAPPY_HEART_P,
        HAPPY_FLOWER,
        HP_DRAIN,
        HP_DRAIN_P,
        FP_DRAIN,
        FP_DRAIN_P,
        CLOSE_CALL,
        CLOSE_CALL_P,
        PRETTY_LUCKY,
        PRETTY_LUCKY_P,
        LUCKY_DAY,
        LUCKY_DAY_P,
        REFUND,
        PITY_FLOWER,
        PITY_FLOWER_P,
        QUICK_CHANGE,
        PEEKABOO,
        TIMING_TUTOR,
        HEART_FINDER,
        FLOWER_FINDER,
        MONEY_MONEY,
        ITEM_HOG,
        ATTACK_FX_R,
        ATTACK_FX_B,
        ATTACK_FX_G,
        ATTACK_FX_Y,
        ATTACK_FX_P,
        CHILL_OUT,
        FIRST_ATTACK,
        BUMP_ATTACK,
        SLOW_GO,
        SIMPLIFIER,
        UNSIMPLIFIER,
        LUCKY_START,
        L_EMBLEM,
        W_EMBLEM,
        TRIPLE_DIP,         // 0x0149
        LUCKY_START_P,
        AUTO_COMMAND_BADGE,
        MEGA_JUMP,
        MEGA_SMASH,
        MEGA_QUAKE,
        SQUARE_DIAMOND_BADGE,
        SQUARE_DIAMOND_BADGE_P,
        SUPER_CHARGE,
        SUPER_CHARGE_P,
        MAX_ITEM_ID,
    };
}
    
struct AttackParams {
    char*           _unk_0x00;
    uint16_t        _unk_0x04;
    uint16_t        _unk_0x06;
    uint16_t        _unk_0x08;
    int16_t         item_id;
    char*           _unk_0x0c;
    uint8_t         _unk_0x10;  // Always 0x64?
    int8_t          base_fp_cost;
    int8_t          base_sp_cost;
    int8_t          guard_types_allowed;  // 0 = none, 2 = both; ? = guard only
    float           _unk_0x14;
    uint32_t        _unk_0x18;  // Possibly separate 8-bit values, or flags?
    
    char*           base_damage_fn;
    int32_t         base_damage;
    int32_t         base_damage_2;
    int32_t         base_damage_3;
    int32_t         base_damage_4;
    int32_t         base_damage_5;
    int32_t         base_damage_6;
    uint32_t        _unk_0x38;
    uint32_t        _unk_0x3c;
    
    char*           base_fp_damage_fn;
    int32_t         base_fp_damage;
    uint32_t        _unk_0x48;
    uint32_t        _unk_0x4c;
    uint32_t        _unk_0x50;
    uint32_t        _unk_0x54;
    uint32_t        _unk_0x58;
    uint32_t        _unk_0x5c;
    uint32_t        _unk_0x60;
    
    int8_t          num_targets;  // 1 for single-target, 2 for multi-target
    uint8_t         _unk_0x65;
    uint8_t         _unk_0x66;
    uint8_t         _unk_0x67;
    uint8_t         allowed_target_types;
    uint8_t         _unk_0x69;  // Used for enemy-knockback attacks (like Gulp)?
    uint8_t         _unk_0x6a;  // Seems to correlate with allowed targets?
    uint8_t         _unk_0x6b;  // Seems to indicate effects like flip, quake...
    int8_t          element;
    uint8_t         _unk_0x6d;
    uint8_t         _unk_0x6e;  // Always 0x03?
    uint8_t         _unk_0x6f;  // Always 0x02?
    char*           _unk_0x70;
    uint32_t        _unk_0x74_flags;
    uint32_t        _unk_0x78_flags;
    uint32_t        _unk_0x7c_flags;  // May be flags; generally similar values.
    
    int8_t          sleep_chance;
    int8_t          sleep_time;
    int8_t          stop_chance;
    int8_t          stop_time;
    
    int8_t          dizzy_chance;
    int8_t          dizzy_time;
    int8_t          poison_chance;
    int8_t          poison_time;
    
    int8_t          poison_strength;
    int8_t          confuse_chance;
    int8_t          confuse_time;
    int8_t          electric_chance;
    
    int8_t          electric_time;
    int8_t          dodgy_chance;
    int8_t          dodgy_time;
    int8_t          burn_chance;
    
    int8_t          burn_time;
    int8_t          freeze_chance;
    int8_t          freeze_time;
    int8_t          size_change_chance;
    
    int8_t          size_change_time;
    int8_t          size_change_strength;
    int8_t          atk_change_chance;
    int8_t          atk_change_time;
    
    int8_t          atk_chance_strength;
    int8_t          def_change_chance;
    int8_t          def_change_time;
    int8_t          def_change_strength;
    
    int8_t          allergic_chance;
    int8_t          allergic_time;
    int8_t          ohko_chance;
    int8_t          charge_strength;
    
    int8_t          fast_chance;
    int8_t          fast_time;
    int8_t          slow_chance;
    int8_t          slow_time;
    
    int8_t          fright_chance;
    int8_t          gale_force_chance;
    int8_t          payback_time;
    int8_t          hold_fast_time;
    
    int8_t          invisible_chance;
    int8_t          invisible_time;
    int8_t          hp_regen_time;
    int8_t          hp_regen_strength;
    
    int8_t          fp_regen_time;
    int8_t          fp_regen_strength;
    uint8_t         _unk_0xae;
    uint8_t         _unk_0xaf;
    
    char*           item_script_ptr;
    
    int8_t          background_a1_a2_fall_weight;
    int8_t          background_a1_fall_weight;
    int8_t          background_a2_fall_weight;
    int8_t          background_no_fall_weight;
    int8_t          background_b_fall_chance;
    int8_t          nozzle_change_direction_chance;
    int8_t          nozzle_fire_chance;
    int8_t          iron_frame_fall_chance;
    int8_t          object_fall_chance;
    uint8_t         _unk_0xbd;
    uint8_t         _unk_0xbe;
    uint8_t         _unk_0xbf;
} __attribute__((__packed__));
    
struct ItemData {
    const char*     item_string_id;     // Used for recipe lookups, etc.
    const char*     item_name_msg;
    const char*     item_desc_msg;
    const char*     item_desc_menu_msg;
    uint16_t        _unk_0x10;
    int16_t         type_sort_order;    // For "By Type" ordering
    int16_t         buy_price;
    int16_t         discount_price;
    int16_t         star_piece_price;
    int16_t         sell_price;
    int8_t          bp_cost;
    int8_t          hp_restored;
    int8_t          fp_restored;
    uint8_t         _unk_0x1f;
    int16_t         icon_id;
    uint16_t        _unk_0x22;
    AttackParams*   attack_params;
} __attribute__((__packed__));

struct ShopItemData {
    int32_t         item_id;
    // Only one of the following is set; sell items follow buy items.
    int16_t         sell_price;
    int16_t         buy_price;
} __attribute__((__packed__));
    
}