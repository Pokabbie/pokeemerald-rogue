#include "global.h"
#include "constants/abilities.h"
//#include "constants/heal_locations.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
//#include "constants/layouts.h"
//#include "constants/rogue.h"
#include "data.h"
//
//#include "battle.h"
//#include "battle_setup.h"
//#include "event_data.h"
#include "item.h"
//#include "money.h"
//#include "overworld.h"
#include "pokemon.h"
//#include "random.h"
//#include "strings.h"
//#include "string_util.h"
//#include "text.h"

#include "rogue_query.h"

#define QUERY_BUFFER_COUNT 64
#define MAX_QUERY_BIT_COUNT (max(ITEMS_COUNT, NUM_SPECIES))

EWRAM_DATA u16 gRogueQueryBufferSize = 0;
EWRAM_DATA u8 gRogueQueryBits[1 + MAX_QUERY_BIT_COUNT / 8];
EWRAM_DATA u16 gRogueQueryBuffer[QUERY_BUFFER_COUNT];

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];

static void SetQueryState(u16 elem, bool8 state)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;

    u8 bitMask = 1 << bit;
    if(state)
    {
        gRogueQueryBits[idx] |= bitMask;
    }
    else
    {
        gRogueQueryBits[idx] &= ~bitMask;
    }
}

static bool8 GetQueryState(u16 elem)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;

    u8 bitMask = 1 << bit;
    return gRogueQueryBits[idx] & bitMask;
}

void RogueQuery_Clear(void)
{
    gRogueQueryBufferSize = 0;
    memset(&gRogueQueryBits[0], 255, sizeof(bool8) * ARRAY_COUNT(gRogueQueryBits));
}

void RogueQuery_CollapseSpeciesBuffer(void)
{
    u16 species;
    gRogueQueryBufferSize = 0;
    
    for(species = SPECIES_NONE + 1; species < NUM_SPECIES && gRogueQueryBufferSize < (QUERY_BUFFER_COUNT - 1); ++species)
    {
        if(GetQueryState(species))
        {
            gRogueQueryBuffer[gRogueQueryBufferSize++] = species;
        }
    }
}

void RogueQuery_CollapseItemBuffer(void)
{
    u16 item;
    gRogueQueryBufferSize = 0;
    
    for(item = ITEM_NONE + 1; item < ITEMS_COUNT && gRogueQueryBufferSize < (QUERY_BUFFER_COUNT - 1); ++item)
    {
        if(GetQueryState(item))
        {
            gRogueQueryBuffer[gRogueQueryBufferSize++] = item;
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

void RogueQuery_Exclude(u16 idx)
{
    SetQueryState(idx, FALSE);
}

// Species
//

static bool8 IsSpeciesType(u16 species, u8 type)
{
    return gBaseStats[species].type1 == type || gBaseStats[species].type2 == type;
}

// Taken straight from daycare
static u16 GetEggSpecies(u16 species)
{
    u16 e, s, evo, spe;
    bool8 found;

    // Working backwards up to 5 times seems arbitrary, since the maximum number
    // of times would only be 3 for 3-stage evolutions.
    for (e = 0; e < 2; ++e)//EVOS_PER_MON; i++)
    {
        found = FALSE;
        for (s = 1; s < NUM_SPECIES; s++)
        {
            if(s < species)
                // Work downwards, as the evolution is most likely just before this
                spe = species - s;
            else
                // Start counting upwards now, as we've exhausted all of the before species
                spe = s;

            for (evo = 0; evo < EVOS_PER_MON; evo++)
            {
                if (gEvolutionTable[spe][evo].targetSpecies == species)
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
}

static bool8 IsFinalEvolution(u16 species)
{
    u16 s, e;

    for (e = 0; e < EVOS_PER_MON; e++)
    {
        if (gEvolutionTable[species][e].targetSpecies != SPECIES_NONE)
        {
            return FALSE;
        }
    }

    return TRUE;
}

static bool8 IsSpeciesIsLegendary(u16 species)
{
    switch(species)
    {
        case SPECIES_ARTICUNO:
        case SPECIES_ZAPDOS:
        case SPECIES_MOLTRES:
        case SPECIES_MEWTWO:
        case SPECIES_MEW:
        case SPECIES_RAIKOU:
        case SPECIES_ENTEI:
        case SPECIES_SUICUNE:
        case SPECIES_LUGIA:
        case SPECIES_HO_OH:
        case SPECIES_CELEBI:
        case SPECIES_REGIROCK:
        case SPECIES_REGICE:
        case SPECIES_REGISTEEL:
        case SPECIES_KYOGRE:
        case SPECIES_GROUDON:
        case SPECIES_RAYQUAZA:
        case SPECIES_LATIAS:
        case SPECIES_LATIOS:
        case SPECIES_JIRACHI:
        case SPECIES_DEOXYS:
            return TRUE;
    };

    return FALSE;
}

void RogueQuery_SpeciesIsValid(void)
{
    // Handle for ?? species mainly
    // Just going to base this off ability 1 being none as that seems safest whilst allowing new mons
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            if(gBaseStats[species].abilities[0] == ABILITY_NONE)
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_SpeciesOfType(u8 type)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            if(!IsSpeciesType(species, type))
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_SpeciesOfTypes(const u8* types, u8 count)
{
    u8 t;
    bool8 isValid;
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            isValid = FALSE;
            for(t = 0; t < count; ++t)
            {
                if(IsSpeciesType(species, types[t]))
                {
                    isValid = TRUE;
                    break;
                }
            }

            if(!isValid)
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_SpeciesIsFinalEvolution(void)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            if(!IsFinalEvolution(species))
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_TransformToEggSpecies(void)
{
    u16 species;
    u16 eggSpecies;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            eggSpecies = GetEggSpecies(species);
            if(eggSpecies != species)
            {
                SetQueryState(eggSpecies, TRUE);
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_EvolveSpeciesToLevel(u8 level)
{
    u8 evo;
    u16 species;
    bool8 removeChild = TRUE;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            for(evo = 0; evo < EVOS_PER_MON; ++evo)
            {
                    switch(gEvolutionTable[species][evo].method)
                    {
                    case EVO_LEVEL:
                    case EVO_LEVEL_ATK_GT_DEF:
                    case EVO_LEVEL_ATK_EQ_DEF:
                    case EVO_LEVEL_ATK_LT_DEF:
                    case EVO_LEVEL_SILCOON:
                    case EVO_LEVEL_CASCOON:
                    case EVO_LEVEL_NINJASK:
                    if (gEvolutionTable[species][evo].param <= level)
                    {
                        SetQueryState(gEvolutionTable[species][evo].targetSpecies, TRUE);
                        if(removeChild)
                        {
                            SetQueryState(species, FALSE);
                        }
                    }
                    break;
                    };
            }
        }
    }
}

void RogueQuery_EvolveSpeciesByItem()
{
    u8 evo;
    u16 species;
    bool8 removeChild = TRUE;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            for(evo = 0; evo < EVOS_PER_MON; ++evo)
            {
                    switch(gEvolutionTable[species][evo].method)
                    {
                    case EVO_ITEM:
                    case EVO_TRADE_ITEM:
                    {
                        SetQueryState(gEvolutionTable[species][evo].targetSpecies, TRUE);
                        if(removeChild)
                        {
                            SetQueryState(species, FALSE);
                        }
                    }
                    break;
                    };
            }
        }
    }
}

void RogueQuery_SpeciesIsLegendary(void)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            if(!IsSpeciesIsLegendary(species))
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_SpeciesIsNotLegendary(void)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            if(IsSpeciesIsLegendary(species))
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

// Items
//

void RogueQuery_ItemsIsValid(void)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(gItems[item].itemId != item)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsInPocket(u8 pocket)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(gItems[item].pocket != pocket)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotInPocket(u8 pocket)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(gItems[item].pocket == pocket)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsInPriceRange(u16 minPrice, u16 maxPrice)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(gItems[item].price < minPrice || gItems[item].price > maxPrice)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsHeldItem(void)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(gItems[item].holdEffect == HOLD_EFFECT_NONE)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotHeldItem(void)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(gItems[item].holdEffect != HOLD_EFFECT_NONE)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsExcludeRange(u16 fromId, u16 toId)
{
    u16 item;

    for(item = ITEM_NONE + 1; item < ITEMS_COUNT; ++item)
    {
        if(GetQueryState(item))
        {
            if(item >= fromId && item <= toId)
            {
                SetQueryState(item, FALSE);
            }
        }
    }
}
