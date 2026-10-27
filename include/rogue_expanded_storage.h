#ifndef GUARD_ROGUE_EXPANDED_STORAGE_H
#define GUARD_ROGUE_EXPANDED_STORAGE_H

#define BASE_BOXES_COUNT 10
#define EXPANDED_BOXES_COUNT 89
#define EXPANDED_TOTAL_BOXES_COUNT 99

struct BoxPokemon;
bool8 RogueExpanded_Load(u32 generation);
bool8 RogueExpanded_ValidateBaseSlot(u8 slot, u32 generation);
bool8 RogueExpanded_PrepareSave(u32 generation);
void RogueExpanded_CompleteSave(bool8 success);
void RogueExpanded_Reset(void);
struct BoxPokemon *RogueExpanded_GetBox(u8 boxId);
u8 *RogueExpanded_GetName(u8 boxId);
u8 RogueExpanded_GetWallpaper(u8 boxId);
void RogueExpanded_SetWallpaper(u8 boxId, u8 wallpaper);

#endif
