#ifndef GUARD_ROGUE_ENHANCED_MEMORY_H
#define GUARD_ROGUE_ENHANCED_MEMORY_H

#define ENHANCED_IWRAM_END 0x03010000

void RogueEnhancedMemory_Init(void);
extern u32 gEnhancedMemoryChecks;
extern u8 gEnhancedFastScratch[4096];
extern const u32 gEnhancedUpperRomProbe[4];
extern const u32 gEnhancedRomEndProbe;

#endif
