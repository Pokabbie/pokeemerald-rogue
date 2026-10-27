#include "global.h"
#include "rogue_enhanced_memory.h"
#include "rogue_expanded_flash.h"

EWRAM_DATA u32 gEnhancedMemoryChecks = 0;
u8 gEnhancedFastScratch[4096] __attribute__((section("iwram_ext_data"), aligned(4)));
const u32 gEnhancedUpperRomProbe[4] __attribute__((section("enhanced_rom_probe"), used)) =
{
    0x314D4152, 0x524F4D36, 0x34495241, 0x4D36344B
};
const u32 gEnhancedRomEndProbe __attribute__((section("enhanced_rom_end"), used)) = 0x454E4436;

void RogueEnhancedMemory_Init(void)
{
    vu32 *low = (vu32 *)0x02000000;
    vu32 *extra = (vu32 *)0x02040000;
    vu32 *ptr;
    u32 i;
    u32 checks = 0;
    // Run before game initialization; old mirrored RAM must fail explicitly.
    *low = 0x13579BDF;
    *extra = 0x2468ACE0;
    if (*low != 0x13579BDF || *extra != 0x2468ACE0)
        RogueExpandedFlash_Fatal(EXPANDED_ERROR_BACKEND);
    *low = 0;
    checks |= 1;
    low = (vu32 *)0x03000000;
    extra = (vu32 *)0x03008000;
    *low = 0x13579BDF;
    *extra = 0x2468ACE0;
    if (*low != 0x13579BDF || *extra != 0x2468ACE0)
        RogueExpandedFlash_Fatal(EXPANDED_ERROR_BACKEND);
    *low = 0;
    checks |= 2;
    // Stock BIOS RAM reset only knows the original hardware sizes.
    for (ptr = (vu32 *)0x02040000; ptr < (vu32 *)0x02100000; ++ptr)
        *ptr = 0;
    for (ptr = (vu32 *)0x03008000; ptr < (vu32 *)0x03010000; ++ptr)
        *ptr = 0;
    for (i = 0; i < sizeof(gEnhancedFastScratch); ++i)
        gEnhancedFastScratch[i] = (i * 29 + 7) & 255;
    for (i = 0; i < sizeof(gEnhancedFastScratch); ++i)
        if (gEnhancedFastScratch[i] != ((i * 29 + 7) & 255))
            RogueExpandedFlash_Fatal(EXPANDED_ERROR_BACKEND);
    checks |= 4;
    if (((const vu32 *)gEnhancedUpperRomProbe)[0] != 0x314D4152
     || ((const vu32 *)gEnhancedUpperRomProbe)[1] != 0x524F4D36
     || ((const vu32 *)gEnhancedUpperRomProbe)[2] != 0x34495241
     || ((const vu32 *)gEnhancedUpperRomProbe)[3] != 0x4D36344B
     || *(const vu32 *)&gEnhancedRomEndProbe != 0x454E4436)
        RogueExpandedFlash_Fatal(EXPANDED_ERROR_BACKEND);
    checks |= 8;
    gEnhancedMemoryChecks = checks;
}
