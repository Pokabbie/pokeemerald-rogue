#ifndef ROGUE_QUERY_H
#define ROGUE_QUERY_H

void RogueQuery_Clear(void);
void RogueQuery_CollapseSpeciesBuffer(void);
void RogueQuery_CollapseItemBuffer(void);
u16* RogueQuery_BufferPtr(void);
u16 RogueQuery_BufferSize(void);

void RogueQuery_Exclude(u16 idx);

// Species
void RogueQuery_SpeciesIsValid(void);
void RogueQuery_SpeciesOfType(u8 type);
void RogueQuery_SpeciesOfTypes(const u8* types, u8 count);
void RogueQuery_SpeciesIsFinalEvolution(void);
void RogueQuery_TransformToEggSpecies(void);
void RogueQuery_EvolveSpeciesToLevel(u8 level);
void RogueQuery_EvolveSpeciesByItem();
void RogueQuery_SpeciesIsLegendary(void);
void RogueQuery_SpeciesIsNotLegendary(void);

// Items
void RogueQuery_ItemsIsValid(void);
void RogueQuery_ItemsInPocket(u8 pocket);
void RogueQuery_ItemsNotInPocket(u8 pocket);
void RogueQuery_ItemsHeldItem(void);
void RogueQuery_ItemsNotHeldItem(void);
void RogueQuery_ItemsExcludeRange(u16 fromId, u16 toId);
void RogueQuery_ItemsInPriceRange(u16 minPrice, u16 maxPrice);

#endif