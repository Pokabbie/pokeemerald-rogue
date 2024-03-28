//
// This file is shared between the game src and the offline bake to assist in making 
// queries and other stuff which can be prepared offline a bit faster
//
#ifndef ROGUE_BAKED_H
#define ROGUE_BAKED_H

struct Evolution;
struct Item;
struct Trainer;

#ifdef ROGUE_EXPANSION
#define gRogueSpeciesInfo  gSpeciesInfo
#else
#define gRogueSpeciesInfo  gBaseStats
#endif

// Misc utils (TODO - should really break these out into separate file)
void HistoryBufferPush(u16* buffer, u16 capacity, u16 value);
bool8 HistoryBufferContains(u16* buffer, u16 capacity, u16 value);

u16 SelectIndexFromWeights(u16* weights, u16 count, u16 rngValue);

// Semi-baked methods
bool8 Rogue_CheckPokedexVariantFlag(u8 dexVariant, u16 species, bool8* result);

void Rogue_ModifyEvolution(u16 species, u8 evoIdx, struct Evolution* outEvo);
void Rogue_ModifyEvolution_ApplyCurses(u16 species, u8 evoIdx, struct Evolution* outEvo);
void Rogue_ModifyFormChange(u16 species, u8 changeIdx, struct FormChange* outFormChange);
const u8* Rogue_GetItemName(u16 itemIdx);
const u8* Rogue_GetItemDesc(u16 itemIdx);
const void* Rogue_GetItemIconPicOrPalette(u16 itemId, u8 which);
u16 Rogue_GetPrice(u16 itemId);
void Rogue_ModifyItem(u16 itemIdx, struct Item* outItem);
bool8 Rogue_IsEvolutionItem(u16 itemId);
u16 Rogue_GetEvolutionItemIndex(u16 itemId);
bool8 Rogue_IsFormItem(u16 itemId);
u16 Rogue_GetFormItemIndex(u16 itemId);
u32 Rogue_CalculateMovePrice(u16 move);

const u8* Rogue_GetTrainerName(u16 trainerNum);
void Rogue_ModifyTrainer(u16 trainerNum, struct Trainer* outTrainer);
u32 Rogue_ModifyExperienceTables(u8 growthRate, u8 level);

void Rogue_ModifyBattleMusic(u16 musicType, u16 trainerSpecies, struct RogueBattleMusic* outMusic);

u16 Rogue_GetEggSpecies(u16 species);
u8 Rogue_GetMaxEvolutionCount(u16 species);
u8 Rogue_GetActiveEvolutionCount(u16 species);
u8 Rogue_GetActiveFormChangeCount(u16 species);
bool8 Rogue_DoesEvolveInto(u16 fromSpecies, u16 toSpecies);

void Rogue_AppendSpeciesTypeFlags(u16 species, u32* outFlags);
u32 Rogue_GetSpeciesEvolutionChainTypeFlags(u16 species);
u32 Rogue_GetTypeFlagsFromArray(const u8* types, u8 count);

u32 Rogue_GetMonFlags(u16 species);
bool8 Rogue_CheckMonFlags(u16 species, u32 flag);

u16 Rogue_BerryToPokeblock(u16 berryItem);

#endif