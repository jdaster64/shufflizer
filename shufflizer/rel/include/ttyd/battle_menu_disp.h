#pragma once

#include <cstdint>
#include <ttyd/common_types.h>

namespace ttyd::battle_menu_disp {

extern "C" {
    
struct WeaponSelectionWindowInfo {
    common::AttackParams* attack_params;
    uint32_t _unk_0x04;  // Enabled?
    const char* menu_text;
    uint32_t _unk_0x0c;  // Byte 0x0D = icon?
    uint32_t _unk_0x10;
    uint32_t _unk_0x14;  // Item id?
    uint32_t _unk_0x18;
}  __attribute__((__packed__));

// DrawMultiItemWin 
// BattleMenuDisp_MultiItem_End 
// BattleMenuDisp_MultiItem_Disp 
// BattleMenuDisp_MultiItem_Main 
// BattleMenuDisp_MultiItem_Init 
// DrawChangePartyWin 
// BattleMenuDisp_ChangePartySelect_End 
// BattleMenuDisp_ChangePartySelect_Disp 
// BattleMenuDisp_ChangePartySelect_Main 
// BattleMenuDisp_ChangePartySelect_Init 
// DrawOperationWin 
// BattleMenuDisp_Operation_End 
// BattleMenuDisp_Operation_Disp 
// BattleMenuDisp_Operation_Main 
// BattleMenuDisp_Operation_Init 
void DrawWeaponWin();
// BattleMenuDisp_WeaponSelect_End 
// BattleMenuDisp_WeaponSelect_Disp 
// BattleMenuDisp_WeaponSelect_Main 
// BattleMenuDisp_WeaponSelect_Init 
// DrawMainMenu 
// BattleMenuDisp_ActSelect_End 
// BattleMenuDisp_ActSelect_Disp 
// BattleMenuDisp_ActSelect_Main 
// BattleMenuDisp_ActSelect_Init 
// DrawSubMenuCommonProcess 
// DrawSubMenuCommonProcessSub1 
// InitSubMenuCommonProcess3 
// InitSubMenuCommonProcess2 
// InitSubMenuCommonProcess 
// DrawMenuCursorAndScrollArrow 
// DrawMenuHelpWin 
// DrawMenuPinchMark 
// zz_80117e48_
// zz_80118030_
// DrawMenuPartyChangeButton_Sub 
// DrawMenuPartyChangeButton 
// DrawSubIconSub 
// DrawSubIcon 
// DrawMainIconCircle 
// DrawMainIconHukidasi 
// DrawMainIcon 
// GetRingOffset 
// GetRingCenter 
// battleMenuDispEnd 
// battleMenuDispInit 
// BattleMenuKeyOKInACT 
// SelectedItemCoordinateColorUpDate 

}

}