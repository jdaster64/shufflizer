#pragma once

#include <cstdint>
#include <ttyd/battle_unit.h>
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
void _getSickStatusParam(
    ttyd::battle_unit::BattleUnitInstance* target_actor_params,
    ttyd::common::AttackParams* attack_params, int32_t status_type,
    int8_t* turn_count, int8_t* strength);
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