#include "patch.h"

#include <ttyd/system.h>

#include <cstdint>

namespace mod::patch {

void WriteBranch(void* ptr, void* destination) {
    uint32_t delta =
        reinterpret_cast<uint32_t>(destination) -
        reinterpret_cast<uint32_t>(ptr);
    uint32_t value = 0x48000000;
    value |= (delta & 0x03FFFFFC);
    
    uint32_t* p = reinterpret_cast<uint32_t*>(ptr);
    *p = value;
    
    ttyd::OSCache::DCFlushRange(ptr, sizeof(uint32_t));
    ttyd::OSCache::ICInvalidateRange(ptr, sizeof(uint32_t));
}

void WritePatch(
    void* destination, const void* patch_start, const void* patch_end) {
    uint32_t patch_len =
        reinterpret_cast<uintptr_t>(patch_end) -
        reinterpret_cast<uintptr_t>(patch_start);
    WritePatch(destination, patch_start, patch_len);
}

void WritePatch(
    void* destination, const void* patch_start, uint32_t patch_len) {
    ttyd::system::memcpy_as4(destination, patch_start, patch_len);
    ttyd::OSCache::DCFlushRange(&destination, patch_len);
    ttyd::OSCache::ICInvalidateRange(&destination, patch_len);
}

}