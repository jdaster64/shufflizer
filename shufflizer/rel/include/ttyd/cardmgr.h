#pragma once

#include <cstdint>

namespace ttyd::cardmgr {

extern "C" {

// memcard_open 
// statusCallback 
// readCallback 
// writeCallback 
// createCallback 
// formatCallback 
// checkCallback 
// mountAttachCallback 
// mountDetachCallback 
// cardFormat 
// create_main 
// cardCreate 
// read_all_main 
// cardReadAll 
// read_main 
// write_header_main 
// cardWriteHeader 
// write_main 
// cardWrite 
// cardCopy 
// cardErase 
void cardCopy2Main(uint32_t size);
// cardMain 
// cardGetCode
// cardIsExec 
// unk025[US|JP] zz_800b2bdc_
// unk026[US|JP] zz_800b2c08_
// unk027[US|JP] zz_800b2c2c_
// cardInit 
// cardBufReset 
// cardGetFilePtr 

}

}