#ifndef GUARD_ITEM_ICON_H
#define GUARD_ITEM_ICON_H

extern u8 *gItemIconDecompressionBuffer;
extern u8 *gItemIcon4x4Buffer;

extern const struct SpriteTemplate gItemIconSpriteTemplate;

bool8 AllocItemIconTemporaryBuffers(void);
void FreeItemIconTemporaryBuffers(void);
void CopyItemIconPicTo4x4Buffer(const void *src, void *dest);
u8 AddItemIconSprite(u16 tilesTag, u16 paletteTag, u16 itemId);
u8 AddIconSprite(u16 tilesTag, u16 paletteTag, const u32* image, const u32* palette);
u8 BlitItemIconToWindow(u16 itemId, u8 windowId, u16 x, u16 y, void * paletteDest);
u8 BlitPokemonIconToWindow(u16 species, u8 windowId, u16 x, u16 y, void * paletteDest);
u8 BlitCustomItemIconToWindow(u8 windowId, u16 x, u16 y, void * paletteDest, u32 const* icon, u32 const* palette);
u8 AddCustomItemIconSprite(const struct SpriteTemplate *customSpriteTemplate, u16 tilesTag, u16 paletteTag, u16 itemId);
const void *GetItemIconPicOrPalette(u16 itemId, u8 which);

#endif //GUARD_ITEM_ICON_H
