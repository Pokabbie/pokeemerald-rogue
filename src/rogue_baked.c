//
// This file is shared between the game src and the offline bake to assist in making 
// queries and other stuff which can be prepared offline a bit faster
//
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/items.h"

#ifdef ROGUE_BAKING
// Manually reinclude this if regenerating
#include "BakeHelpers.h"
#else
#include "global.h"
#include "item.h"
#include "item_use.h"

#include "rogue_controller.h"
#endif

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

    if(outEvo->targetSpecies != SPECIES_NONE && !IsGenEnabled(SpeciesToGen(outEvo->targetSpecies)))
    {
        // Invalid evo
        outEvo->targetSpecies = SPECIES_NONE;
        outEvo->method = 0;
    }

#ifdef ROGUE_EXPANSION
    if(!IsMegaEvolutionEnabled())
    {
        switch(outEvo->method)
        {
            case(EVO_MEGA_EVOLUTION):
            case(EVO_MOVE_MEGA_EVOLUTION):
            case(EVO_PRIMAL_REVERSION):
                outEvo->targetSpecies = SPECIES_NONE;
                outEvo->method = 0;
                break;
        }
    }
#endif

    if(outEvo->targetSpecies != SPECIES_NONE)
    {
#ifdef ROGUE_EXPANSION
        if(species == SPECIES_MILCERY && evoIdx == 0)
        {
            outEvo->method = EVO_LEVEL;
            outEvo->param = 20;
        }

        if(species == SPECIES_YAMASK_GALARIAN && evoIdx == 0)
        {
            outEvo->method = EVO_LEVEL;
            outEvo->param = 34;
        }
#endif

        switch(outEvo->method)
        {
            case(EVO_BEAUTY):
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

#ifdef ROGUE_EXPANSION
            case(EVO_ITEM_HOLD_DAY):
            case(EVO_ITEM_HOLD_NIGHT):
                outEvo->method = EVO_LEVEL_ITEM;
                break;

            case(EVO_LEVEL_DAY):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_SUN_STONE;
                break;
            case(EVO_LEVEL_DUSK):
            case(EVO_LEVEL_NIGHT):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_MOON_STONE;
                break;

            case(EVO_WATER_SCROLL):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_WATER_STONE;
                break;
            case(EVO_DARK_SCROLL):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_MOON_STONE;
                break;

            case(EVO_TRADE_SPECIFIC_MON):
                outEvo->method = EVO_SPECIFIC_MON_IN_PARTY;
                break;

            case(EVO_SPECIFIC_MAP):
                if(species == SPECIES_EEVEE)
                {
                    outEvo->targetSpecies = 0;
                    outEvo->method = 0;
                    outEvo->param = 0;
                }
                else
                {
                    // Crabrawler
                    outEvo->method = EVO_ITEM;
                    outEvo->param = ITEM_WATER_STONE;
                }
                break;

            case(EVO_MAPSEC):
                // All these were MAPSEC_NEW_MAUVILLE
                //outEvo->method = EVO_SPECIFIC_MAP;
                //outEvo->param = MAP_ROGUE_ROUTE_URBAN0;
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_THUNDER_STONE;
                break;
#endif
        }
    }
}

static u16 SanitizeItemId(u16 itemId)
{
    if (itemId >= ITEMS_COUNT)
        return ITEM_NONE;
    else
        return itemId;
}

#ifdef ROGUE_BAKING
// DUDs
const u8* Rogue_GetItemName(u16 itemId)
{
    return NULL;
}

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
}
#else

extern const u8 gText_ItemLinkCable[];

const u8* Rogue_GetItemName(u16 itemId)
{
    itemId = SanitizeItemId(itemId);

    switch(itemId)
    {
        case ITEM_EXP_SHARE:
            return gText_ItemLinkCable;
    }

    return gItems[itemId].name;
}

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
    itemId = SanitizeItemId(itemId);
    memcpy(outItem, &gItems[itemId], sizeof(struct Item));

    // Range edits
    if(itemId >= ITEM_HP_UP && itemId <= ITEM_PP_MAX)
    {
        outItem->price = 4000;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_X_ATTACK && itemId <= ITEM_GUARD_SPEC)
#else
    if(itemId >= ITEM_GUARD_SPEC && itemId <= ITEM_X_SPECIAL)
#endif
    {
        outItem->price = 1500;
    }

    if(outItem->fieldUseFunc == ItemUseOutOfBattle_EvolutionStone)
    {
        outItem->price = 2100;
    }

    // Hold items set price (Ignore berries)
    if(outItem->holdEffect != 0 && !(itemId >= FIRST_BERRY_INDEX && itemId <= LAST_BERRY_INDEX))
    {
        outItem->price = 500;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_LEVEL_BALL && itemId <= ITEM_CHERISH_BALL)
    {
        outItem->price = 2500;
    }

    if(itemId >= ITEM_FIRE_STONE && itemId <= ITEM_RIBBON_SWEET)
    {
        outItem->price = 2100;
    }

    if(itemId >= ITEM_RED_NECTAR && itemId <= ITEM_PURPLE_NECTAR)
    {
        outItem->price = 2100;
    }

    if(itemId >= ITEM_RED_ORB && itemId <= ITEM_DIANCITE)
    {
        outItem->price = 5000;
    }

    if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
    {
        outItem->price = 5000;
    }

    if(itemId >= ITEM_DOUSE_DRIVE && itemId <= ITEM_CHILL_DRIVE)
    {
        outItem->price = 1000;
    }

    if(itemId >= ITEM_ROTOM_CATALOG && itemId <= ITEM_REINS_OF_UNITY)
    {
        outItem->price = 5000;
    }
#endif

    // Individual items
    switch(itemId)
    {
        // Evo item prices
        case ITEM_EXP_SHARE:
            outItem->price = 2100;
            outItem->holdEffect = 0;//HOLD_EFFECT_NONE;
            break;

        case ITEM_NUGGET:
            outItem->price = 1000;
            outItem->holdEffect = 0;
            break;

        case ITEM_PEARL:
            outItem->price = 1500;
            outItem->holdEffect = 0;
            break;

        case ITEM_BIG_PEARL:
            outItem->price = 2000;
            outItem->holdEffect = 0;
            break;

        case ITEM_STARDUST:
            outItem->price = 3000;
            outItem->holdEffect = 0;
            break;

        case ITEM_STAR_PIECE:
            outItem->price = 4000;
            outItem->holdEffect = 0;
            break;
    
#ifdef ROGUE_EXPANSION
        case ITEM_ABILITY_PATCH:
            outItem->price = 10000;
            break;

        case ITEM_RUSTED_SWORD:
        case ITEM_RUSTED_SHIELD:
            outItem->price = 5000;
            break;
#endif

        case ITEM_KINGS_ROCK:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_DEEP_SEA_SCALE:
        case ITEM_METAL_COAT:
        case ITEM_DRAGON_SCALE:
#ifdef ROGUE_EXPANSION
        case ITEM_UPGRADE:
#else
        case ITEM_UP_GRADE:
#endif
            outItem->price = 2100;
            break;

        case ITEM_RARE_CANDY:
            outItem->price = 1000;
            break;

#ifdef ROGUE_EXPANSION
        case ITEM_SPORT_BALL:
        case ITEM_PARK_BALL:
#endif
        case ITEM_SAFARI_BALL:
            outItem->price = 0;
            break;

        case ITEM_MASTER_BALL:
            outItem->price = 50000;
            break;
    }
}
#endif


u32 Rogue_ModifyExperienceTables(u8 growthRate, u8 level)
{
    // Originallu from const u32 gExperienceTables[][MAX_LEVEL + 1]
    // But want to ideally fit all EXP within u16 since we earn it differently in Rogue anyway
    return level * 300;//MAX_LEVEL;
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

#ifdef ROGUE_EXPANSION
                if(evolution.method != EVO_MEGA_EVOLUTION &&
                    evolution.method != EVO_MOVE_MEGA_EVOLUTION &&
                    evolution.method != EVO_PRIMAL_REVERSION
                )
#endif
                {
                    if (evolution.targetSpecies == species)
                    {
                        species = spe;
                        found = TRUE;
                        break;
                    }
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

#ifdef ROGUE_EXPANSION
        if(evo.method != EVO_MEGA_EVOLUTION &&
            evo.method != EVO_MOVE_MEGA_EVOLUTION &&
            evo.method != EVO_PRIMAL_REVERSION
        )
#endif
        {
            if (s != SPECIES_NONE)
            {
                return 1 + Rogue_GetEvolutionCount(s);
            }
        }
    }

    return 0;
#endif
}