#ifndef ROGUE_GIFTS_H
#define ROGUE_GIFTS_H

#include "constants/generated/custom_mons.h"

enum
{
    UNIQUE_RARITY_COMMON,   // 2 moves
    UNIQUE_RARITY_RARE,     // 4 moves
    UNIQUE_RARITY_EPIC,     // 4 moves & ability
    UNIQUE_RARITY_EXOTIC,   // bespoke made mons for Quest rewards
};

u32 RogueGift_GetCustomMonId(struct Pokemon* mon);
u32 RogueGift_GetCustomMonIdBySpecies(u16 species, u32 otId);

u16 RogueGift_GetCustomMonMove(u32 id, u8 i);
u16 RogueGift_GetCustomMonMoveCount(u32 id);

u16 RogueGift_GetCustomMonAbility(u32 id, u8 i);
u16 RogueGift_GetCustomMonAbilityCount(u32 id);

bool8 RogueGift_DisplayCustomMonRarity(u32 id);
u8 RogueGift_GetCustomMonRarity(u32 id);

void RogueGift_CreateMon(u32 customMonId, struct Pokemon* mon, u16 species, u8 level, u8 fixedIV);
u32 RogueGift_CreateDynamicMonId(u8 rarity, u16 species);

#endif // ROGUE_GIFTS_H