#ifndef ROGUE_POKEDEX__H
#define ROGUE_POKEDEX__H

// New UI/UX for Rogue pokedex
void CB2_Rogue_ShowPokedex(void);

u8 RoguePokedex_GetDexRegion();
void RoguePokedex_SetDexRegion(u8 region);

u8 RoguePokedex_GetDexVariant();
void RoguePokedex_SetDexVariant(u8 variant);

u8 RoguePokedex_GetDexGenLimit();
void RoguePokedex_SetDexGenLimit(u8 gen);

bool8 RoguePokedex_IsNationalDexActive();
u16 RoguePokedex_GetNationalDexLimit();

bool8 RoguePokedex_IsVariantEditUnlocked();
bool8 RoguePokedex_IsVariantEditEnabled();

bool8 RoguePokedex_IsSpeciesEnabled(u16 species);

#endif