#pragma once

#include <cstdint>

namespace ttyd::oslink {

extern "C" {
    
struct OSModuleInfo {
    uint32_t        id;                     // unique identifier for the module
    void*           link;                   // doubly linked list of modules
    uint32_t        num_sections;           // # of sections
    uint32_t        section_info_offset;    // offset to section info table
    uint32_t        name_offset;            // offset to module name
    uint32_t        name_size;              // size of module name
    uint32_t        version;                // version number
};

// OSNotifyLink
// OSNotifyUnlink
bool Relocate(OSModuleInfo* new_module, void* bss);
bool Link(OSModuleInfo* new_module, void* bss);
// OSLink
// Undo
// OSUnlink
// __OSModuleInit

}

}