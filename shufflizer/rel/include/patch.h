#pragma once

#include <ttyd/OSCache.h>

#include <cstdint>

namespace mod::patch {

void WriteBranch(void* ptr, void* destination);

void WritePatch(
    void* destination, const void* patch_start, const void* patch_end);
void WritePatch(
    void* destination, const void* patch_start, uint32_t patch_len);

template<typename Func, typename Dest>
Func HookFunction(Func function, Dest destination) {
	uint32_t* instructions = reinterpret_cast<uint32_t*>(function);
	
	uint32_t* trampoline = new uint32_t[2];
	// Original instruction
	trampoline[0] = instructions[0];
    ttyd::OSCache::DCFlushRange(&trampoline[0], sizeof(uint32_t));
    ttyd::OSCache::ICInvalidateRange(&trampoline[0], sizeof(uint32_t));
  
	// Branch to original function past hook
	WriteBranch(&trampoline[1], &instructions[1]);
	
	// Write actual hook
	WriteBranch(&instructions[0],
                reinterpret_cast<void*>(static_cast<Func>(destination)));
	
	return reinterpret_cast<Func>(trampoline);
}

}