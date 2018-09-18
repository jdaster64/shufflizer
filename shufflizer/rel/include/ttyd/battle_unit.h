#pragma once

#include <cstdint>

namespace ttyd::battle_unit {

extern "C" {
    
struct BattleUnitParams {
    uint32_t    type_id;        // e.g. 0xab = Bonetail
    const char* name_lookup;    // e.g. "btl_un_kuribo" for Goomba
    
    int16_t     max_hp;
    int16_t     max_fp;
    uint8_t     _unk_0x0c;      // always 0x01?
    uint8_t     _unk_0x0d;      // always 0x01?
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
    
    // Probably mostly script pointers?
    void*       _unk_0x9c;
    void*       _unk_0xa0;
    void*       _unk_0xa4;
    void*       _unk_0xa8;
    void*       _unk_0xac;
    void*       _unk_0xb0;
    uint32_t    _unk_0xb4;  // Could be boolean, etc.
    void*       _unk_0xb8;
    void*       _unk_0xbc;
    void*       _unk_0xc0;
} __attribute__((__packed__));
    
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
// BtlUnit_GetWeaponCost
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