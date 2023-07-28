#ifndef ROGUE_PLAYER_CUSTOMISATION__H
#define ROGUE_PLAYER_CUSTOMISATION__H

enum 
{
    PLAYER_OUTFIT_STYLE_APPEARANCE,
    PLAYER_OUTFIT_STYLE_PRIMARY_HUE,
    PLAYER_OUTFIT_STYLE_PRIMARY_SAT,
    PLAYER_OUTFIT_STYLE_PRIMARY_LUM,
    PLAYER_OUTFIT_STYLE_SECONDARY_HUE,
    PLAYER_OUTFIT_STYLE_SECONDARY_SAT,
    PLAYER_OUTFIT_STYLE_SECONDARY_LUM,
    PLAYER_OUTFIT_STYLE_COUNT,
};

enum
{
    PLAYER_COLOUR_MODE_HUE,
    PLAYER_COLOUR_MODE_GREYSCALE,
    PLAYER_COLOUR_MODE_COUNT,
};

enum 
{
    PLAYER_LAYER_APPEARANCE,
    PLAYER_LAYER_PRIMARY_COLOUR,
    PLAYER_LAYER_SECONDARY_COLOUR,
    PLAYER_LAYER_COUNT,
};

void RoguePlayer_SetNewGameOutfit();

void RoguePlayer_SetOutfitId(u16 outfit);
u16 RoguePlayer_GetOutfitId();
u16 RoguePlayer_GetOutfitCount();

void RoguePlayer_SetOutfitStyle(u8 styleId, s8 value);
s8 RoguePlayer_GetOutfitStyle(u8 styleId);
s8 RoguePlayer_GetOutfitStylePerc(u8 styleId);

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