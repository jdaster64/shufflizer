#include "mod.h"

#include <ttyd/OSArena.h>
#include <ttyd/dispdrv.h>
#include <ttyd/fontmgr.h>
#include <ttyd/seqdrv.h>
#include <ttyd/seq_logo.h>
#include <ttyd/system.h>

#include "patch.h"

namespace mod {

Mod* gMod = nullptr;

uint32_t initMod() {
    Mod* mod = new Mod();
    mod->Init();
    
    return 0;
}

void main() {
    // Check if using v3 of the REL Loader
    if (ttyd::OSArena::__OSArenaLo == ttyd::OSArena::__OSArenaHi) {
        // Not using v3 of the REL Loader, so init stuff now
        initMod();
    } else {
        // Using v3 of the REL Loader, so hook initMod at the address
        // where REL Loader V1/V2 would normally run
#ifdef TTYD_US
        constexpr uint32_t marioStInitAddress = 0x8006FE38;
#elif defined TTYD_JP
        constexpr uint32_t marioStInitAddress = 0x8006EBD8;
#elif defined TTYD_EU
        constexpr uint32_t marioStInitAddress = 0x800710F4;
#endif
        patch::WriteBranchBL(
            reinterpret_cast<void*>(marioStInitAddress),
            reinterpret_cast<void*>(initMod));
    }
}

Mod::Mod() {}

void Mod::Init() {
    gMod = this;
    
    makeKey_trampoline_ = patch::HookFunction(ttyd::system::makeKey, [](){
        gMod->UpdateEarly();
    });

    // Initialize typesetting early
    ttyd::fontmgr::fontmgrTexSetup();
    patch::HookFunction(ttyd::fontmgr::fontmgrTexSetup, [](){});

    // Skip to the title on startup; for development only, isn't init'd properly.
    /*
    patch::HookFunction(
        ttyd::seq_logo::seq_logoMain,
        [](ttyd::seqdrv::SeqInfo*) {
            ttyd::seqdrv::seqSetSeq(
                ttyd::seqdrv::SeqIndex::kTitle, nullptr, nullptr);
        });
    */
        
    // Main mod initialization logic.
    shufflizer_mod_.Init();
}

void Mod::UpdateEarly() {
    // Perform the main mod's update logic.
    shufflizer_mod_.Update();
    
    // Check for font load, then call the main mod's drawing logic.
    ttyd::dispdrv::dispEntry(
        ttyd::dispdrv::DisplayLayer::kDebug3d, 0,
        [](ttyd::dispdrv::DisplayLayer layerId, void* user) {
            reinterpret_cast<shufflizer::Shufflizer*>(user)->Draw();
        }, &shufflizer_mod_);
    
    // Call original function
    makeKey_trampoline_();
}

}