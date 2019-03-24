#pragma once

#include <cstdint>

namespace ttyd::mtx {

extern "C" {

// PSMTXIdentity
// PSMTXCopy
// PSMTXConcat
// PSMTXInverse
// PSMTXInvXpose
// PSMTXRotRad
// PSMTXRotTrig
// __PSMTXRotAxisRadInternal
// PSMTXRotAxisRad
void PSMTXTrans(float* matrix, float x, float y, float z);
// PSMTXTransApply
// PSMTXScale
// PSMTXScaleApply
// C_MTXLookAt
// C_MTXLightFrustum
// C_MTXLightPerspective
// C_MTXLightOrtho

}

}