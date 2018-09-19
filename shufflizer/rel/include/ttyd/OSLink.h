#pragma once

#include <cstdint>

namespace ttyd::oslink {

extern "C" {
    
struct OSModuleLink {
    void* next;
    void* prev;
};
    
struct OSModuleInfo {
    uint32_t        id;                     // unique identifier for the module
    OSModuleLink    link;                   // doubly linked list of modules
    uint32_t        num_sections;           // # of sections
    uint32_t        section_info_offset;    // offset to section info table
    uint32_t        name_offset;            // offset to module name
    uint32_t        name_size;              // size of module name
    uint32_t        version;                // version number
    uint32_t        bss_size;               // size of .bss section
    uint32_t        rel_offset;
    uint32_t        imp_offset;
    int8_t          prolog_section;
    int8_t          epilog_section;
    int8_t          unresolved_section;
    int8_t          bss_section;
    uint32_t        prolog;
    uint32_t        epilog;
    uint32_t        unresolved;
    uint32_t        align;
    uint32_t        bss_align;
    uint32_t        fix_size;               // only present in v3 rels, unknown
};

// OSNotifyLink
// OSNotifyUnlink
bool Relocate(OSModuleInfo* new_module, void* bss);
bool Link(OSModuleInfo* new_module, void* bss);
bool OSLink(OSModuleInfo* new_module, void* bss);
// Undo
// OSUnlink
// __OSModuleInit

}

}