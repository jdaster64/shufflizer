#pragma once

#include <cstdint>

namespace ttyd::mario_pouch {

extern "C" {

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
// pouchReviseMarioParam
// pouchRemoveKeepItem
// pouchAddKeepItem
// pouchGetPartyAttackLv
// pouchGetHammerLv
// pouchGetJumpLv
// pouchSetAudienceNum
// pouchGetAudienceNum
// pouchGetMaxAP
// pouchSetAP
void pouchAddAP(int32_t star_power);
int32_t pouchGetAP();
// pouchSetMaxFP
// pouchSetFP
// pouchGetMaxFP
// pouchGetFP
// pouchSetPartyHP
// pouchGetPartyHP
// pouchSetMaxHP
// pouchSetHP
// pouchGetMaxHP
// pouchGetHP
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
// pouchGetPtr

}

}