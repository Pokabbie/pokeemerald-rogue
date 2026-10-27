#include "global.h"
#include "gba/flash_internal.h"
#include "rogue_expanded_flash.h"

static EWRAM_DATA bool8 sExpandedFlashAvailable = FALSE;

void RogueExpandedFlash_Init(void)
{
    sExpandedFlashAvailable = ReadFlashId() == ROGUE_EXPANDED_FLASH_ID;
}

bool8 RogueExpandedFlash_IsAvailable(void)
{
    return sExpandedFlashAvailable;
}

bool8 RogueExpandedFlash_Read(u32 address, void *dest, u32 size)
{
    u8 *out = dest;
    if (!sExpandedFlashAvailable || address > ROGUE_EXPANDED_FLASH_SIZE || size > ROGUE_EXPANDED_FLASH_SIZE - address)
        return FALSE;
    while (size)
    {
        u32 count = min(size, 0x10000 - (address & 0xFFFF));
        u32 i;
        const vu8 *src;
        SwitchFlashBank(address >> 16);
        src = (const vu8 *)(FLASH_BASE + (address & 0xFFFF));
        for (i = 0; i < count; ++i)
            out[i] = src[i];
        address += count;
        out += count;
        size -= count;
    }
    SwitchFlashBank(0);
    return TRUE;
}

static bool8 WaitByte(u32 offset, u8 expected)
{
    u32 attempts;
    // The prototype chip returns 0x5F while busy. Delay this ambiguous value.
    if (expected == 0x5F)
    {
        vu32 delay;
        for (delay = 0; delay < 1000; ++delay)
            ;
    }
    for (attempts = 0; attempts < 0x200000; ++attempts)
        if (*(vu8 *)(FLASH_BASE + offset) == expected)
            return TRUE;
    return FALSE;
}

bool8 RogueExpandedFlash_WriteSector(u16 sector, const u8 *data)
{
    u32 i;
    u32 offset = (sector & 15) * ROGUE_EXPANDED_SECTOR_SIZE;
    bool8 success = FALSE;
    // This driver is never allowed to write the legacy 128 KiB prefix.
    if (!sExpandedFlashAvailable || sector < 32 || sector >= 256)
        return FALSE;
    SwitchFlashBank(sector >> 4);
    // The inactive save bank often already contains this sector's contents.
    for (i = 0; i < ROGUE_EXPANDED_SECTOR_SIZE; ++i)
        if (*(vu8 *)(FLASH_BASE + offset + i) != data[i])
            break;
    if (i == ROGUE_EXPANDED_SECTOR_SIZE)
    {
        success = TRUE;
        goto done;
    }
    FLASH_WRITE(0x5555, 0xAA);
    FLASH_WRITE(0x2AAA, 0x55);
    FLASH_WRITE(0x5555, 0x80);
    FLASH_WRITE(0x5555, 0xAA);
    FLASH_WRITE(0x2AAA, 0x55);
    FLASH_WRITE(offset, 0x30);
    if (!WaitByte(offset, 0xFF))
        goto done;
    for (i = 0; i < ROGUE_EXPANDED_SECTOR_SIZE; ++i)
    {
        if (data[i] == 0xFF)
            continue;
        FLASH_WRITE(0x5555, 0xAA);
        FLASH_WRITE(0x2AAA, 0x55);
        FLASH_WRITE(0x5555, 0xA0);
        FLASH_WRITE(offset + i, data[i]);
        if (!WaitByte(offset + i, data[i]))
            goto done;
    }
    for (i = 0; i < ROGUE_EXPANDED_SECTOR_SIZE; ++i)
        if (*(vu8 *)(FLASH_BASE + offset + i) != data[i])
            goto done;
    success = TRUE;
done:
    SwitchFlashBank(0);
    return success;
}

static const u8 sErrorFont[26][7] =
{
    {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
    {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
    {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
    {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
    {14,4,4,4,4,4,14}, {7,2,2,2,2,18,12},
    {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
    {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17},
    {14,17,17,17,17,17,14}, {30,17,17,30,16,16,16},
    {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
    {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
    {17,17,17,17,17,17,14}, {17,17,17,17,17,10,4},
    {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
    {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31},
};

static void ErrorText(const char *text, u16 y)
{
    vu16 *pixels = (vu16 *)0x06000000;
    u16 x = 12;
    while (*text && x < 234)
    {
        if (*text >= 'A' && *text <= 'Z')
        {
            u8 row, column;
            for (row = 0; row < 7; ++row)
                for (column = 0; column < 5; ++column)
                    if (sErrorFont[*text - 'A'][row] & (16 >> column))
                        pixels[(y + row) * 240 + x + column] = 0x7FFF;
        }
        ++text;
        x += 6;
    }
}

void RogueExpandedFlash_Fatal(u8 reason)
{
    vu16 *pixels = (vu16 *)0x06000000;
    u32 i;
    REG_IME = 0;
    REG_DMA0CNT_H = 0;
    REG_DMA1CNT_H = 0;
    REG_DMA2CNT_H = 0;
    REG_DMA3CNT_H = 0;
    REG_SOUNDCNT_X = 0;
    REG_BG2PA = 0x100;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 0x100;
    REG_BG2X = 0;
    REG_BG2Y = 0;
    REG_BLDCNT = 0;
    REG_DISPCNT = 0x0403; // Mode 3 with BG2; independent of the game heap and tasks.
    for (i = 0; i < 240 * 160; ++i)
        pixels[i] = 0x0008;
    ErrorText("EXPANDED STORAGE STOPPED", 24);
    switch (reason)
    {
    case EXPANDED_ERROR_BACKEND:
        ErrorText("REQUIRES PROTOTYPE MGBA", 48);
        break;
    case EXPANDED_ERROR_LINK:
        ErrorText("LINK SAVES ARE NOT SUPPORTED", 48);
        break;
    case EXPANDED_ERROR_WRITE:
        ErrorText("FLASH WRITE FAILED", 48);
        break;
    case EXPANDED_ERROR_READ:
        ErrorText("FLASH READ FAILED", 48);
        break;
    default:
        ErrorText("SAVE BANK COULD NOT BE VERIFIED", 48);
        break;
    }
    ErrorText("STOPPED TO PROTECT YOUR SAVE", 80);
    ErrorText("RESTART FROM YOUR LAST GOOD SAVE", 104);
    for (;;)
        asm volatile("nop");
}
