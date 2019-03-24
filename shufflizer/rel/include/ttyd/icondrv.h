#pragma once

#include <cstdint>

namespace ttyd::icondrv {

extern "C" {
    
// iconNumberDispGx3D
void iconNumberDispGx(
    float* matrix, int32_t value, int32_t is_small, int32_t* unk);
// iconSetAlpha
// iconSetScale
// iconFlagOff
// iconFlagOn
// iconSetPos
// iconNameToPtr
// iconGX
// iconGetWidthHight
// iconGetTexObj
// iconDispGxCol
// iconDispGx2
// iconDispGx
// iconDispGxAlpha
// iconDisp
// iconChange
// iconDelete
// iconEntry2D
// iconEntry
// iconMain
// iconReInit
// iconTexSetup
// iconInit
// _callback_bin
// _callback_tpl

}

}