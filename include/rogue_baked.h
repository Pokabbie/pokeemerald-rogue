//
// This file is shared between the game src and the offline bake to assist in making 
// queries and other stuff which can be prepared offline a bit faster
//
#ifndef ROGUE_BAKED_H
#define ROGUE_BAKED_H

void Rogue_ModifyEvolution(u16 species, u8 evoIdx, struct Evolution* outEvo);
u16 Rogue_GetEggSpecies(u16 species);
u8 Rogue_GetEvolutionCount(u16 species);

#endif