#ifndef ROGUE_PLAYER_CUSTOMISATION__H
#define ROGUE_PLAYER_CUSTOMISATION__H

enum 
{
    PLAYER_OUTFIT_STYLE_APPEARANCE,
    PLAYER_OUTFIT_STYLE_PRIMARY,
    PLAYER_OUTFIT_STYLE_SECONDARY,
    PLAYER_OUTFIT_STYLE_COUNT,
};

enum
{
    PLAYER_COLOUR_MODE_HUE,
    PLAYER_COLOUR_MODE_GREYSCALE,
    PLAYER_COLOUR_MODE_COUNT,
};

void RoguePlayer_SetNewGameOutfit();

void RoguePlayer_SetOutfitId(u16 outfit);
u16 RoguePlayer_GetOutfitId();
u16 RoguePlayer_GetOutfitCount();

void RoguePlayer_SetOutfitStyle(u8 styleId, u16 value);
u16 RoguePlayer_GetOutfitStyle(u8 styleId);

bool8 RoguePlayer_HasSpritingAnim();

u16 RoguePlayer_GetTrainerFrontPic();
u16 RoguePlayer_GetTrainerBackPic();
const u16* RoguePlayer_GetTrainerFrontPalette();
const u16* RoguePlayer_GetTrainerBackPalette();

const struct ObjectEventGraphicsInfo* RoguePlayer_GetObjectEventGraphicsInfo(u8 state);
const u16* RoguePlayer_GetOverworldPalette();

u8 RoguePlayer_GetTextVariantId();
u8 RoguePlayer_GetBagGfxVariant();

#endif