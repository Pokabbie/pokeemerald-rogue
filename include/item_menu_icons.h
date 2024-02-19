#ifndef GUARD_ITEM_MENU_ICONS_H
#define GUARD_ITEM_MENU_ICONS_H

enum
{
    BAG_GFX_VARIANT_BRENDAN,
    BAG_GFX_VARIANT_MAY,
    BAG_GFX_VARIANT_RED,
    BAG_GFX_VARIANT_LEAF,
    BAG_GFX_VARIANT_BRENDAN_SILVER,
    BAG_GFX_VARIANT_MAY_SILVER,
    BAG_GFX_VARIANT_BRENDAN_BLACK,
    BAG_GFX_VARIANT_MAY_BLACK,
    BAG_GFX_VARIANT_RED_SILVER,
    BAG_GFX_VARIANT_LEAF_SILVER,
    BAG_GFX_VARIANT_RED_BLACK,
    BAG_GFX_VARIANT_LEAF_BLACK,
    BAG_GFX_VARIANT_RED_PINK,
    BAG_GFX_VARIANT_LEAF_PINK,

    BAG_GFX_VARIANT_COUNT,
};

extern const struct CompressedSpriteSheet gBagSpriteSheet[BAG_GFX_VARIANT_COUNT];
extern const struct CompressedSpritePalette gBagPaletteTable[BAG_GFX_VARIANT_COUNT];

extern const struct CompressedSpriteSheet gBerryCheckCircleSpriteSheet;
extern const struct CompressedSpritePalette gBerryCheckCirclePaletteTable;

void RemoveBagSprite(u8 id);
void AddBagVisualSprite(u8 bagPocketId);
void SetBagVisualPocketId(u8 bagPocketId, bool8 isSwitchingPockets);
void ShakeBagSprite(void);
void SetBagSpriteVisible(bool8 state);
void AddSwitchPocketRotatingBallSprite(s16 rotationDirection);
void AddBagItemIconSprite(u16 itemId, u8 id);
void RemoveBagItemIconSprite(u8 id);
void CreateItemMenuSwapLine(void);
void SetItemMenuSwapLineInvisibility(bool8 invisible);
void UpdateItemMenuSwapLinePos(u8 y);
u8 CreateBerryTagSprite(u8 id, s16 x, s16 y);
void FreeBerryTagSpritePalette(void);
u8 CreateSpinningBerrySprite(u8 berryId, u8 x, u8 y, bool8 startAffine);
u8 CreateBerryFlavorCircleSprite(s16 x);

#endif // GUARD_ITEM_MENU_ICONS_H
