#pragma once

#include <cstdint>
#include <ttyd/common_types.h>

namespace ttyd::battle_unit {

extern "C" {
    
// Base status ailment vulnerability (in %).
struct StatusVulnerability {
    uint8_t     sleep;
    uint8_t     stop;
    uint8_t     dizzy;
    uint8_t     poison;
    uint8_t     confuse;
    uint8_t     electric;
    uint8_t     burn;
    uint8_t     freeze;

    uint8_t     huge;
    uint8_t     tiny;
    uint8_t     attack_up;
    uint8_t     attack_down;
    uint8_t     defense_up;
    uint8_t     defense_down;
    uint8_t     allergic;
    uint8_t     fright;

    uint8_t     gale_force;
    uint8_t     fast;
    uint8_t     slow;
    uint8_t     dodgy;
    uint8_t     invisible;
    uint8_t     ohko;
} __attribute__((__packed__));

static_assert(sizeof(StatusVulnerability) == 0x16);
    
// Describes a class of battle unit.
struct BattleUnitParams {
    uint32_t    type_id;        // e.g. 0xab = Bonetail
    const char* name_lookup;    // e.g. "btl_un_kuribo" for Goomba
    
    int16_t     max_hp;
    int16_t     max_fp;
    int8_t      danger_hp;
    int8_t      peril_hp;
    int8_t      level;
    int8_t      bonus_exp;
    int8_t      bonus_coin;
    int8_t      bonus_coin_rate;
    int8_t      base_coin;
    int8_t      run_rate;
    int16_t     pb_min_cap;     // Power Bounce cap; usually 9,999 for enemies
    int16_t     _unk_0x16;
    int16_t     _unk_0x18;
    int16_t     _unk_0x1a;
    int16_t     _unk_0x1c;
    uint16_t    _unk_0x1e;
    
    // Bunch of unknown stuff...
    float       _unk_0x20;
    float       _unk_0x24;
    float       _unk_0x28;
    float       _unk_0x2c;
    float       _unk_0x30;
    float       _unk_0x34;
    float       _unk_0x38;
    float       _unk_0x3c;
    float       _unk_0x40;
    float       _unk_0x44;
    float       _unk_0x48;
    float       _unk_0x4c;
    float       _unk_0x50;
    float       _unk_0x54;
    float       _unk_0x58;
    float       _unk_0x5c;
    float       _unk_0x60;
    float       _unk_0x64;
    float       _unk_0x68;
    float       _unk_0x6c;
    float       _unk_0x70;
    float       _unk_0x74;
    float       _unk_0x78;
    float       _unk_0x7c;
    float       _unk_0x80;
    float       _unk_0x84;
    
    uint32_t    _unk_0x88;
    uint32_t    _unk_0x8c;  // May be multiple values / bits.
    uint32_t    _unk_0x90;
    uint32_t    _unk_0x94;
    uint32_t    _unk_0x98;
    
    const char* damaged_sfx_name;
    const char* fire_damage_sfx_name;
    const char* ice_damage_sfx_name;
    const char* explosion_damage_sfx_name;
    void*       _unk_0xac;
    StatusVulnerability*    default_status_vulnerability;
    int8_t      num_parts;
    uint8_t     _unk_0xb5[3];  // Probably unused / padding.
    void*       parts;
    void*       init_script;
    void*       data_table;
} __attribute__((__packed__));

static_assert(sizeof(BattleUnitParams) == 0xC4);

struct BattleUnitBadgesEquipped {
    int8_t close_call;
    int8_t pretty_lucky;
    int8_t lucky_day;
    int8_t _unk_0x03;  // Possibly unused
    int8_t power_plus;
    int8_t p_up_d_down;
    int8_t all_or_nothing;
    int8_t mega_rush;
    
    int8_t power_rush;
    int8_t p_down_d_up;
    int8_t double_pain;
    int8_t last_stand;
    int8_t defend_plus;
    int8_t damage_dodge;
    int8_t happy_heart;
    int8_t happy_flower;
    
    int8_t return_postage;
    int8_t hp_plus;
    int8_t fp_plus;
    int8_t double_dip;
    int8_t triple_dip;
    int8_t flower_saver;
    int8_t feeling_fine;
    int8_t zap_tap;
    
    int8_t pity_flower;
    int8_t hp_drain;
    int8_t fp_drain;
    int8_t refund;
    int8_t charge;
    int8_t super_charge;
    int8_t unused_square_diamond_badge;
    int8_t jumpman;
    
    int8_t hammerman;
    int8_t ice_power;
    int8_t spike_shield;
    int8_t super_appeal;
    int8_t lucky_start;
    int8_t simplifier;
    int8_t unsimplifier;
    int8_t _unk_0x27;  // Probably unused / padding
} __attribute__((__packed__));

// Describes an instance of a battle unit, or "actor".
struct BattleUnitInstance {
    uint32_t            _unk_0x000;
    uint32_t            _unk_0x004;
    uint32_t            type_id;
    uint32_t            _unk_0x00c;
    BattleUnitParams*   unit_class_params;
    void*               parts_params;
    
    char                _unk_0x018[0xf0];
    int16_t             max_hp;
    int16_t             base_max_hp;                  // i.e., w/no badges
    int16_t             current_hp;
    int16_t             max_fp;
    int16_t             base_max_fp;
    int16_t             current_fp;
    float               _unk_0x114;
    char                current_status_params[0x1e];  // TODO: sep. struct
    char                _unk_0x136[0xe];
    StatusVulnerability*    base_status_vulnerability;
    char                _unk_0x148[0x144];
    void*               wait_script;
    uint32_t            wait_script_thread;
    uint32_t            _unk_0x294;
    void*               phase_script;
    uint32_t            phase_script_thread;
    void*               attack_script;
    void*               confuse_script;
    uint32_t            attack_script_thread;
    uint32_t            _unk_0x2ac;
    void*               damage_script;
    uint32_t            damage_script_thread;
    char                _unk_0x2b8[0x28];
    BattleUnitBadgesEquipped    badges_equipped;
    char                _unk_0x308[0x82c];
} __attribute__((__packed__));

static_assert(sizeof(BattleUnitInstance) == 0xB34);
    
// Describes one unit in a battle party.
struct BattlePartySlotInfo {
    BattleUnitParams*   unit_class_params;
    uint32_t            _unk_0x04;
    uint32_t            _unk_0x08;
    float               _unk_0x0c;
    uint32_t            _unk_0x10;
    float               _unk_0x14;
    uint32_t            _unk_0x18;
    uint32_t            _unk_0x1c;
    uint32_t            _unk_0x20;
    uint32_t            _unk_0x24;
    uint32_t            _unk_0x28;
    void*               item_drop_table;
} __attribute__((__packed__));

// BtlUnit_CheckShadowGuard
// BtlUnit_EnemyItemCanUseCheck
// BtlUnit_HpGaugeMain
// BtlUnit_HpGaugeInit
// BtlUnit_snd_se_pos
// BtlUnit_snd_se
// BtlUnit_ControlPoseSoundMain
// BtlUnit_PoseSoundInit
// BtlUnit_SetCommandAnimPose
// BtlUnit_SetSeMode
// BtlUnit_LoadSeMode
// BtlUnit_ResetMoveStatus
// BtlUnit_GetGuardKouraPtr
// BtlUnit_PayWeaponCost
// BtlUnit_CheckWeaponCost
int32_t BtlUnit_GetWeaponCost(
    BattleUnitInstance* battle_unit, common::AttackParams* attack_params);
// BtlUnit_SetMaxFp
// BtlUnit_GetMaxFp
// BtlUnit_RecoverFp
// BtlUnit_RecoverHp
// BtlUnit_SetFp
// BtlUnit_GetFp
// BtlUnit_GetCoin
// BtlUnit_GetExp
// BtlUnit_CheckPinchStatus
// BtlUnit_SetParamToPouch
// BtlUnit_ReviseHpFp
// BtlUnit_SetParamFromPouch
// BtlUnit_CanActStatus
// BtlUnit_CanGuardStatus
// BtlUnit_CheckData
// BtlUnit_GetData
// BtlUnit_GetACPossibility
// BtlUnit_SetTotalHitDamage
// BtlUnit_GetTotalHitDamage
// BtlUnit_GetHitDamage
// BtlUnit_GetEnemyBelong
// BtlUnit_GetTalkTogePos
// BtlUnit_ChangeStayAnim
// BtlUnit_ChangeTalkAnim
// BtlUnit_SetBodyAnim
// BtlUnit_SetBodyAnimType
// BtlUnit_SetAnim
// BtlUnit_SetAnimType
// BtlUnit_GetPoseNameFromType
// BtlUnit_OffUnitFlag
// BtlUnit_OnUnitFlag
// BtlUnit_CheckUnitFlag
// BtlUnit_OffStatusFlag
// BtlUnit_OnStatusFlag
// BtlUnit_CheckStatusFlag
// BtlUnit_CheckStatus
// BtlUnit_CheckRecoveryStatus
// BtlUnit_ClearStatus
// BtlUnit_SetStatus
// BtlUnit_GetStatus
// _CheckMoveCount
// BtlUnit_GetBelong
// BtlUnit_SetJumpSpeed
// BtlUnit_SetPartsMoveSpeed
// BtlUnit_SetMoveSpeed
// BtlUnit_SetPartsFallAccel
// BtlUnit_SetFallAccel
// BtlUnit_SetPartsMoveTargetPos
// BtlUnit_SetMoveTargetPos
// BtlUnit_SetPartsMoveCurrentPos
// BtlUnit_SetMoveCurrentPos
// BtlUnit_SetPartsMoveStartPos
// BtlUnit_SetMoveStartPos
// BtlUnit_AddPartsDispOffset
// BtlUnit_SetPartsDispOffset
// BtlUnit_SetDispOffset
// BtlUnit_AddPartsOffsetPos
// BtlUnit_SetPartsOffsetPos
// BtlUnit_GetPartsOffsetPos
// BtlUnit_SetOffsetPos
// BtlUnit_SetHeight
// BtlUnit_GetHeight
// BtlUnit_GetWidth
// BtlUnit_AddPartsScale
// BtlUnit_SetPartsScale
// BtlUnit_SetPartsBaseScale
// BtlUnit_AddScale
// BtlUnit_SetScale
// BtlUnit_GetScale
// BtlUnit_SetBaseScale
// BtlUnit_AddPartsRotateOffset
// BtlUnit_SetPartsRotateOffset
// BtlUnit_SetRotateOffset
// BtlUnit_GetPartsBaseRotate
// BtlUnit_SetPartsBaseRotate
// BtlUnit_GetBaseRotate
// BtlUnit_SetBaseRotate
// BtlUnit_AddPartsRotate
// BtlUnit_GetPartsRotate
// BtlUnit_SetPartsRotate
// BtlUnit_AddRotate
// BtlUnit_GetRotate
// BtlUnit_SetRotate
// BtlUnit_SetPartsHomePos
// BtlUnit_AddHomePos
// BtlUnit_SetHomePos
// BtlUnit_GetHomePos
// BtlUnit_SetHitCursorOffset
// BtlUnit_SetHitOffset
// BtlUnit_GetHitPos
// BtlUnit_GetPartsWorldPos
// BtlUnit_AddPartsPos
// BtlUnit_SetPartsPos
// BtlUnit_GetPartsPos
// BtlUnit_AddPos
// BtlUnit_SetPos
// BtlUnit_GetPos
// BtlUnit_GetBodyPartsId
// BtlUnit_GetPartsPtr
// BtlUnit_GetUnitId
// BtlUnit_Spawn
// BtlUnit_Delete
void* BtlUnit_Entry(
    BattlePartySlotInfo* slot_info, float unk0, float unk1);
// BtlUnit_Init

}

}