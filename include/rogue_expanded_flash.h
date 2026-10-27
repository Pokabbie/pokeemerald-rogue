#ifndef GUARD_ROGUE_EXPANDED_FLASH_H
#define GUARD_ROGUE_EXPANDED_FLASH_H

#define ROGUE_EXPANDED_FLASH_ID 0x8862
#define ROGUE_EXPANDED_FLASH_SIZE 0x100000
#define ROGUE_EXPANDED_SECTOR_SIZE 4096

enum RogueExpandedError
{
    EXPANDED_ERROR_BACKEND,
    EXPANDED_ERROR_READ,
    EXPANDED_ERROR_WRITE,
    EXPANDED_ERROR_SAVE,
    EXPANDED_ERROR_LINK,
};

void RogueExpandedFlash_Init(void);
bool8 RogueExpandedFlash_IsAvailable(void);
bool8 RogueExpandedFlash_Read(u32 address, void *dest, u32 size);
bool8 RogueExpandedFlash_WriteSector(u16 sector, const u8 *data);
void RogueExpandedFlash_Fatal(u8 reason) __attribute__((noreturn));

#endif
