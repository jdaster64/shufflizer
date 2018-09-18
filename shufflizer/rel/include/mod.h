#pragma once

#include <cstdint>

#include "shufflizer.h"

namespace mod {

class Mod {
public:
	Mod();
	void Init();
	
private:
	void UpdateEarly();
	
    // Encapsulates all the main mod logic.
    shufflizer::Shufflizer shufflizer_mod_;
    
    // Main trampoline to call once-a-frame update logic from.
	void (*makeKey_trampoline_)() = nullptr;
};

}