#ifndef ROGUE_PLAYER_CUSTOMISATION__H
#define ROGUE_PLAYER_CUSTOMISATION__H

void RoguePlayer_SetOutfitId(u16 outfit);
u16 RoguePlayer_GetOutfitId();

u16 RoguePlayer_GetOutfitCount();

u16 RoguePlayer_GetTrainerFrontPic();
u16 RoguePlayer_GetTrainerBackPic();
u16 RoguePlayer_GetObjectGfx(u8 state);

u8 RoguePlayer_GetTextVariantId();
u8 RoguePlayer_GetBagGfxVariant();

#endif