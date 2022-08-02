//
// This file is shared between the game src and the offline bake to assist in making 
// queries and other stuff which can be prepared offline a bit faster
//
#ifdef ROGUE_BAKING
// Manually reinclude this if regenerating
#include "BakeHelpers.h"
#else
#include "global.h"
#endif

#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/items.h"

#include "rogue_baked.h"

#ifdef ROGUE_BAKING
#define ROGUE_BAKE_INVALID
#else
// Swap to force runtime resolution
//#define ROGUE_BAKE_INVALID
#define ROGUE_BAKE_VALID
#endif

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];

#ifdef ROGUE_BAKE_VALID
extern const u16 gRogueBake_EggSpecies[NUM_SPECIES];
extern const u8 gRogueBake_EvolutionCount[NUM_SPECIES];
#endif

void Rogue_ModifyEvolution(u16 species, u8 evoIdx, struct Evolution* outEvo)
{
    memcpy(outEvo, &gEvolutionTable[species][evoIdx], sizeof(struct Evolution));

    if(outEvo->targetSpecies != SPECIES_NONE)
    {
        switch(outEvo->method)
        {
            case(EVO_FRIENDSHIP):
                outEvo->method = EVO_LEVEL;
                outEvo->param = 20;
                break;

            case(EVO_TRADE):
                outEvo->method = EVO_LEVEL_ITEM;
                outEvo->param = ITEM_EXP_SHARE; // Link cable
                break;
            case(EVO_TRADE_ITEM):
                outEvo->method = EVO_LEVEL_ITEM;
                break;

            case(EVO_FRIENDSHIP_DAY):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_SUN_STONE;
                break;
            case(EVO_FRIENDSHIP_NIGHT):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_MOON_STONE;
                break;

            case(EVO_BEAUTY):
                outEvo->method = EVO_LEVEL_ITEM;
                outEvo->param = ITEM_DRAGON_SCALE;
                break;
        }
    }
}

u32 Rogue_ModifyExperienceTables(u8 growthRate, u8 level)
{
    // Originallu from const u32 gExperienceTables[][MAX_LEVEL + 1]
    // But want to ideally fit all EXP within u16 since we earn it differently in Rogue anyway
    return level * 3000;//MAX_LEVEL;
}


// Taken straight from daycare
u16 Rogue_GetEggSpecies(u16 species)
{
#ifdef ROGUE_BAKE_VALID
    return gRogueBake_EggSpecies[species];

#else
    u16 e, s, evo, spe;
    bool8 found;
    struct Evolution evolution;

    // Working backwards up to 5 times seems arbitrary, since the maximum number
    // of times would only be 3 for 3-stage evolutions.
    for (e = 0; e < 2; ++e)//EVOS_PER_MON; i++)
    {
        found = FALSE;
        for (s = 1; s < NUM_SPECIES; s++)
        {
            if (s < species)
                // Work downwards, as the evolution is most likely just before this
                spe = species - s;
            else
                // Start counting upwards now, as we've exhausted all of the before species
                spe = s;

            for (evo = 0; evo < EVOS_PER_MON; evo++)
            {
                Rogue_ModifyEvolution(spe, evo, &evolution);

                if (evolution.targetSpecies == species)
                {
                    species = spe;
                    found = TRUE;
                    break;
                }
            }

            if (found)
                break;
        }

        if (s == NUM_SPECIES)
            break;
    }

    return species;
#endif
}

u8 Rogue_GetEvolutionCount(u16 species)
{
#ifdef ROGUE_BAKE_VALID
    return gRogueBake_EvolutionCount[species];
    
#else
    u16 s, e;
    struct Evolution evo;

    for (e = 0; e < EVOS_PER_MON; e++)
    {
        Rogue_ModifyEvolution(species, e, &evo);

        s = evo.targetSpecies;
        if (s != SPECIES_NONE)
        {
            return 1 + Rogue_GetEvolutionCount(s);
        }
    }

    return 0;
#endif
}