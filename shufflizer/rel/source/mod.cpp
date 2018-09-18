#include "mod.h"

#include <ttyd/dispdrv.h>
#include <ttyd/fontmgr.h>
#include <ttyd/seqdrv.h>
#include <ttyd/seq_logo.h>
#include <ttyd/system.h>

#include "patch.h"

namespace mod {

Mod* gMod = nullptr;

void main() {
	Mod* mod = new Mod();
	mod->Init();
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

	// Skip the logo
	patch::HookFunction(
        ttyd::seq_logo::seq_logoMain,
        [](ttyd::seqdrv::SeqInfo*) {
            ttyd::seqdrv::seqSetSeq(
                ttyd::seqdrv::SeqIndex::kTitle, nullptr, nullptr);
        });
        
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