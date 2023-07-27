#ifndef ROGUE_PLAYER_CUSTOMISATION__H
#define ROGUE_PLAYER_CUSTOMISATION__H

enum 
{
    PLAYER_LAYER_APPEARANCE,
    PLAYER_LAYER_PRIMARY_COLOUR,
    PLAYER_LAYER_SECONDARY_COLOUR,
    PLAYER_LAYER_COUNT,
};

void RoguePlayer_SetOutfitId(u16 outfit);
u16 RoguePlayer_GetOutfitId();
u16 RoguePlayer_GetOutfitCount();

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