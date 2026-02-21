#ifndef ROGUE_POKEDEX__H
#define ROGUE_POKEDEX__H

// New UI/UX for Rogue pokedex
void Rogue_ShowPokedexFromMenu(void);
void Rogue_ShowPokedexFromScript(void);
void Rogue_ShowPokedexForPartySlot(u8 slot);
void Rogue_SelectPokemonInPokedexFromDex(bool8 requireSeen, bool8 requireCaught);
void Rogue_SelectPokemonInPokedexFromDexVariant(u8 variant, bool8 requireSeen, bool8 requireCaught);
void Rogue_SelectPokemonInSafari();

u8 RoguePokedex_GetDexRegion();
void RoguePokedex_SetDexRegion(u8 region);

u8 RoguePokedex_GetDexVariant();
void RoguePokedex_SetDexVariant(u8 variant);

u8 RoguePokedex_GetDexGenLimit();
u16 RoguePokedex_GetCurrentDexLimit();

bool8 RoguePokedex_IsVariantEditUnlocked();
bool8 RoguePokedex_IsVariantEditEnabled();

bool8 RoguePokedex_IsSpeciesEnabled(u16 species);
bool8 RoguePokedex_IsBaseSpeciesEnabled(u16 baseSpecies);
u16 RoguePokedex_GetSpeciesCurrentNum(u16 species);
u16 RoguePokedex_RedirectSpeciesGetSetFlag(u16 species);

u16 RoguePokedex_CountCaughtMonsForVariant(u16 variant, u8 caseID);
u16 RoguePokedex_CountCurrentCaughtMons(u8 caseID);
u16 RoguePokedex_CountNationalCaughtMons(u8 caseID);

bool8 RoguePokedex_IsSpeciesLegendary(u16 species);
bool8 RoguePokedex_IsSpeciesValidBoxLegendary(u16 species);
bool8 RoguePokedex_IsSpeciesValidRoamerLegendary(u16 species);

bool8 RoguePokedex_IsSpeciesParadox(u16 species);

u8 const* RoguePokedex_GetSpeciesName(u16 species);
u8 RoguePokedex_GetSpeciesType(u16 species, u8 typeIndex);
u16 RoguePokedex_GetSpeciesBST(u16 species);
u8 RoguePokedex_GetSpeciesBestStat(u16 species);
u8 RoguePokedex_GetSpeciesWorstStat(u16 species);
void RoguePokedex_GetSpeciesStatArray(u16 species, u8* stats, u8 bufferSize);

#endif