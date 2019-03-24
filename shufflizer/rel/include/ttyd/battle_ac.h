#pragma once

#include <cstdint>
#include <ttyd/battle_unit.h>
#include <ttyd/common_types.h>

namespace ttyd::battle_ac {

extern "C" {
    
// BattleAcGaugeSeDelete battle_ac.o 
// BattleAcGaugeSeUpdate battle_ac.o 
// BattleAcGaugeSeInit battle_ac.o 
// BattleAcDrawGauge battle_ac.o 
// BattleACGetButtonIcon battle_ac.o 
// BattleActionCommandGetPrizeLv battle_ac.o 
// BattleActionCommandSetDifficulty battle_ac.o 
// BattleActionCommandGetDifficulty battle_ac.o 
// BattleActionCommandResetDefenceResult battle_ac.o 
// BattleActionCommandGetDefenceResult battle_ac.o 
int32_t BattleACPadCheckRecordTrigger(int32_t frames, int32_t buttons);
int32_t BattleActionCommandCheckDefence(
    battle_unit::BattleUnitInstance* battle_unit,
    common::AttackParams* attack_params);
// BattleActionCommandStop battle_ac.o 
// BattleActionCommandStart battle_ac.o 
// BattleActionCommandSetup battle_ac.o 
// BattleActionCommandDeclareACResult battle_ac.o 
// BattleActionCommandResult battle_ac.o 
// BattleActionCommandManager battle_ac.o 
// BattleActionCommandManagerInit battle_ac.o 

}

}