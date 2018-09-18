#pragma once

#include <cstdint>

// Types and enums used for the shuffled state of a particular Shufflizer seed.
namespace mod::shufflizer {

namespace ShopType {
    enum e {
        TOAD_BROS_BAZAAR = 0,
        WESTSIDE_GOODS,
        DEEPDOWN_DEPOT,
        NIFF_T_SHOP,
        PUNGENT_SHOP,
        SOUVENIR_SHOP,
        TWILIGHT_SHOP,
        KEELHAUL_GALLERIA,
        SALES_STALL,
        NORTHWINDS_MART,
        SHOP_MAX_ID,
    };
}

struct ItemPrice {
    int16_t item;
    int16_t price;
};

struct ShopInfo {
    ItemPrice buy_items[6];
    ItemPrice sell_items[5];
};

struct SeedState {
    int16_t field_items[286];
    ShopInfo shops[ShopType::SHOP_MAX_ID];
    int8_t pit_floors[100];
};

}