#pragma once

#include <cstdint>
#include <ttyd/common_types.h>

namespace ttyd::battle_damage {

extern "C" {

// BattleInitCounterPreCheckWork
// BattleAttackDeclareAll
// __declare
// BattleAttackDeclare
// BattleCheckDamage
// _checkDamageCode_EmergencyRevival
// BattlePreCheckDamage
// BattleSetStatusDamageFromWeapon
// BattleSetStatusDamage
// _getRegistStatus
// _getSickStatusParam
// _getSickStatusRate
int32_t BattleCalculateFpDamage(
    void* attacking_actor_params, void* defending_actor_params,
    void* additional_defender_params, ttyd::common::AttackParams* attack_params,
    void* unk0, uint32_t unk1);
int32_t BattleCalculateDamage(
    void* attacking_actor_params, void* defending_actor_params,
    void* additional_defender_params, ttyd::common::AttackParams* attack_params,
    void* unk0, uint32_t unk1);
// BattleCheckPikkyoro
// BattleDamageDirect

}

}