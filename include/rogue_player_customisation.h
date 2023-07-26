#ifndef ROGUE_PLAYER_CUSTOMISATION__H
#define ROGUE_PLAYER_CUSTOMISATION__H

void RoguePlayer_SetOutfitId(u16 outfit);
u16 RoguePlayer_GetOutfitId();

u16 RoguePlayer_GetOutfitCount();

u16 RoguePlayer_GetTrainerFrontSprite();
u16 RoguePlayer_GetTrainerBackSprite();

u16 RoguePlayer_GetObjectGfx(u8 state);

#endif