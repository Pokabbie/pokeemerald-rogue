#ifndef ROGUE_GIFTS_H
#define ROGUE_GIFTS_H

#include "constants/generated/custom_mons.h"

u16 RogueGift_GetCustomMonId(struct Pokemon* mon);
u16 RogueGift_GetCustomMonIdBySpecies(u16 species, u32 otId);

u16 const* RogueGift_GetCustomMonMoves(u16 id);
u16 RogueGift_GetCustomMonMoveCount(u16 id);

u16 const* RogueGift_GetCustomMonAbilites(u16 id);

void RogueGift_CreateMon(u16 customMonId, struct Pokemon* mon, u8 level, u8 fixedIV);

#endif // ROGUE_GIFTS_H