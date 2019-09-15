#pragma once

#include <cstdint>

namespace ttyd::mario_pouch {

extern "C" {
    
struct PouchWork {
    char        _unk_0x000[0x070];  // Party-related data
    int16_t     current_hp;
    int16_t     max_hp;
    int16_t     current_fp;
    int16_t     max_fp;
    int16_t     coins;
    int16_t     current_sp;
    int16_t     max_sp;
    char        _unk_0x07e[0x006];  // Unused?
    float       audience_level;
    int16_t     rank;
    int16_t     level;
    uint16_t    star_powers_obtained;  // Bitfield
    int16_t     base_max_hp;
    int16_t     base_max_fp;
    char        _unk_0x092[0x542];  // Most of this is known but not needed
} __attribute__((__packed__));

static_assert(sizeof(PouchWork) == 0x5D4);

// pouchGetYoshiName
// pouchSetYoshiName
// pouchSetPartyColor
// pouchGetPartyColor
// pouchCheckMail
// pouchOpenMail
// pouchReceiveMail
// pouchReceiveMailCount
// pouchGetStarStone
// pouchAddKpaScore
// pouchGetKpaScore
// pouchAddKpaCoin
// pouchGetKpaCoin
// pouchMajinaiInit
// pouchArriveBadge
// unk050[US|JP] zz_800d3234_
// pouchEquipBadgeID
// pouchEquipCheckBadgeIndex
int32_t pouchEquipCheckBadge(int16_t badge_id);
// pouchUnEquipBadgeIndex
// pouchEquipBadgeIndex
// pouchGetStarPoint
// pouchRevisePartyParam
void pouchReviseMarioParam();
// pouchRemoveKeepItem
// pouchAddKeepItem
// pouchGetPartyAttackLv
// pouchGetHammerLv
// pouchGetJumpLv
// pouchSetAudienceNum
// pouchGetAudienceNum
int32_t pouchGetMaxAP();
// pouchSetAP
void pouchAddAP(int32_t star_power);
int32_t pouchGetAP();
void pouchSetMaxFP(int16_t max_fp);
void pouchSetFP(int16_t fp);
int16_t pouchGetMaxFP();
int16_t pouchGetFP();
// pouchSetPartyHP
// pouchGetPartyHP
void pouchSetMaxHP(int16_t max_hp);
void pouchSetHP(int16_t hp);
int16_t pouchGetMaxHP();
int16_t pouchGetHP();
// pouchAddHP
// pouchAddStarPiece
// pouchGetStarPiece
// pouchSetSuperCoin
// pouchGetSuperCoin
// pouchSetCoin
// pouchAddCoin
// pouchGetCoin
// pouchSortItem
// comp_kind_r
// comp_aiueo_r
// comp_kind
// comp_aiueo
// unk051[US|JP] zz_800d48b0_
// pouchRemoveItemIndex
// pouchRemoveItem
// pouchCheckItem
// pouchGetItem
// pouchGetEmptyKeepItemCnt
// pouchGetEmptyHaveItemCnt
// pouchGetEquipBadgeCnt
// pouchGetHaveBadgeCnt
// pouchGetKeepItemCnt
// pouchGetHaveItemCnt
// ?pouchEquipBadge
// pouchHaveBadge
// pouchKeepItem
// pouchHaveItem
// pouchKeyItem
// pouchInit
PouchWork* pouchGetPtr();

}

}