#include "global.h"
//#include "constants/abilities.h"
//#include "constants/heal_locations.h"
//#include "constants/items.h"
//#include "constants/layouts.h"
//#include "constants/rogue.h"
#include "data.h"
//
//#include "battle.h"
//#include "battle_setup.h"
//#include "event_data.h"
//#include "item.h"
//#include "money.h"
//#include "overworld.h"
#include "pokemon.h"
//#include "random.h"
//#include "strings.h"
//#include "string_util.h"
//#include "text.h"

#include "rogue_query.h"

EWRAM_DATA u16 gRogueQueryBufferSize = 0;
EWRAM_DATA u8 gRogueQuerySpeciesBits[1 + NUM_SPECIES / 8];
EWRAM_DATA u16 gRogueQueryBuffer[64];

static void SetSpeciesState(u16 species, bool8 state)
{
    u16 idx = species / 8;
    u16 bit = species % 8;

    u8 bitMask = 1 << bit;
    if(state)
    {
        gRogueQuerySpeciesBits[idx] |= bitMask;
    }
    else
    {
        gRogueQuerySpeciesBits[idx] &= ~bitMask;
    }
}

static bool8 GetSpeciesState(u16 species)
{
    u16 idx = species / 8;
    u16 bit = species % 8;

    u8 bitMask = 1 << bit;
    return gRogueQuerySpeciesBits[idx] & bitMask;
}

//void RogueQuery_Clear(void) {}
//void RogueQuery_CollapseBuffer(void){}
//u16* RogueQuery_BufferPtr(void){ return NULL;}
//u16 RogueQuery_BufferSize(void) { return 0;}
//
//
//void RogueQuery_SpeciesOfType(u8 type){}
//void RogueQuery_SpeciesOfTypes(u8 type1, u8 type2){}
//void RogueQuery_EggSpeciesOnly(void){}

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];


void RogueQuery_Clear(void)
{
    gRogueQueryBufferSize = 0;
    memset(&gRogueQuerySpeciesBits[0], 255, sizeof(bool8) * ARRAY_COUNT(gRogueQuerySpeciesBits));
}

void RogueQuery_CollapseBuffer(void)
{
    u16 species;
    gRogueQueryBufferSize = 0;
    
    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetSpeciesState(species))
        {
            gRogueQueryBuffer[gRogueQueryBufferSize++] = species;
        }
    }
}

u16* RogueQuery_BufferPtr(void)
{
    return &gRogueQueryBuffer[0];
}

u16 RogueQuery_BufferSize(void)
{
    return gRogueQueryBufferSize;
}

static bool8 CanEvolveByLevel(u16 species, u8 level)
{
   int s, e;
   bool8 found;

   // Working backwards up to 5 times seems arbitrary, since the maximum number
   // of times would only be 3 for 3-stage evolutions.
   for (s = 0; s < NUM_SPECIES; s++)
   {
       for (e = 0; e < EVOS_PER_MON; e++)
       {
           if (gEvolutionTable[s][e].targetSpecies == species)
           {
               switch(gEvolutionTable[s][e].method)
               {
               case EVO_LEVEL:
               case EVO_LEVEL_ATK_GT_DEF:
               case EVO_LEVEL_ATK_EQ_DEF:
               case EVO_LEVEL_ATK_LT_DEF:
               case EVO_LEVEL_SILCOON:
               case EVO_LEVEL_CASCOON:
               case EVO_LEVEL_NINJASK:
               if (gEvolutionTable[s][e].param > level)
               {
                   // Haven't reached level up yet
                   return FALSE;
               }
               else
               {
                   // Level is above evolve level so we're good!
                   return TRUE;
               }
               break;
               };

               if(!CanEvolveByLevel(s, level))
               {
                   // If the species we evolve from can't exist yet then neither can we (Probably)
                   return FALSE;
               }
           }
       }
   }

   // We must be a baby of all evolutions are valid
   return TRUE;
}

// Taken straight from daycare
static u16 GetEggSpecies(u16 species)
{
    int i, j, k;
    bool8 found;

    // Working backwards up to 5 times seems arbitrary, since the maximum number
    // of times would only be 3 for 3-stage evolutions.
    for (i = 0; i < EVOS_PER_MON; i++)
    {
        found = FALSE;
        for (j = 1; j < NUM_SPECIES; j++)
        {
            for (k = 0; k < EVOS_PER_MON; k++)
            {
                if (gEvolutionTable[j][k].targetSpecies == species)
                {
                    species = j;
                    found = TRUE;
                    break;
                }
            }

            if (found)
                break;
        }

        if (j == NUM_SPECIES)
            break;
    }

    return species;
}

void RogueQuery_SpeciesOfType(u8 type)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetSpeciesState(species))
        {
            if(gBaseStats[species].type1 != type && gBaseStats[species].type2 != type)
            {
                SetSpeciesState(species, FALSE);
            }
        }
    }
}

void RogueQuery_SpeciesOfTypes(u8 type1, u8 type2)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetSpeciesState(species))
        {
            if(gBaseStats[species].type1 != type1 && gBaseStats[species].type2 != type1 && gBaseStats[species].type1 != type2 && gBaseStats[species].type2 != type2)
            {
                SetSpeciesState(species, FALSE);
            }
        }
    }
}

void RogueQuery_EggSpeciesOnly(void)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetSpeciesState(species))
        {
            if(GetEggSpecies(species) != species)
            {
                SetSpeciesState(species, FALSE);
            }
        }
    }
}
