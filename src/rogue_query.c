#include "global.h"
#include "constants/abilities.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "data.h"

#include "event_data.h"
#include "item.h"
#include "item_use.h"
#include "malloc.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"

#include "rogue_adventurepaths.h"
#include "rogue_query.h"
#include "rogue_baked.h"
#include "rogue_campaign.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"

#define QUERY_BUFFER_COUNT          128
#define QUERY_NUM_SPECIES           NUM_SPECIES
#define QUERY_NUM_ITEMS             ITEMS_COUNT
#define QUERY_NUM_TRAINERS          256 // just a vague guess that needs to at least match gRogueTrainerCount
#define QUERY_NUM_ADVENTURE_PATH    ROGUE_ADVPATH_ROOM_CAPACITY

#define MAX_QUERY_BIT_COUNT (max(QUERY_NUM_ITEMS, max(QUERY_NUM_ADVENTURE_PATH, max(QUERY_NUM_TRAINERS, max(ITEMS_COUNT, QUERY_NUM_SPECIES)))))
#define MAX_QUERY_BYTE_COUNT (1 + MAX_QUERY_BIT_COUNT / 8)

// Old API
//
EWRAM_DATA u16 gRogueQueryBufferSize = 0;
EWRAM_DATA u8 gRogueQueryBits[MAX_QUERY_BYTE_COUNT];
EWRAM_DATA u16 gRogueQueryBuffer[QUERY_BUFFER_COUNT];

// 2.0 API
//
enum
{
    QUERY_TYPE_NONE,
    QUERY_TYPE_MON,
    QUERY_TYPE_ITEM,
    QUERY_TYPE_TRAINER,
    QUERY_TYPE_ADVENTURE_PATHS,
    QUERY_TYPE_CUSTOM,
};

struct RogueQueryData
{
    u8* bitFlags; // TODO - Should hard coded to be [MAX_QUERY_BYTE_COUNT], but for now just using existing memory
    u8* weightArray;
    u16* listArray;
    u16 bitCount;
    u16 totalWeight;
    u16 arrayCapacity;
    u8 queryType;
};

EWRAM_DATA static struct RogueQueryData* sRogueQueryPtr = 0;

#define ASSERT_NO_QUERY         AGB_ASSERT(sRogueQueryPtr == NULL)
#define ASSERT_ANY_QUERY        AGB_ASSERT(sRogueQueryPtr != NULL)
#define ASSERT_MON_QUERY        AGB_ASSERT(sRogueQueryPtr != NULL && sRogueQueryPtr->queryType == QUERY_TYPE_MON)
#define ASSERT_ITEM_QUERY       AGB_ASSERT(sRogueQueryPtr != NULL && sRogueQueryPtr->queryType == QUERY_TYPE_ITEM)
#define ASSERT_TRAINER_QUERY    AGB_ASSERT(sRogueQueryPtr != NULL && sRogueQueryPtr->queryType == QUERY_TYPE_TRAINER)
#define ASSERT_PATHS_QUERY      AGB_ASSERT(sRogueQueryPtr != NULL && sRogueQueryPtr->queryType == QUERY_TYPE_ADVENTURE_PATHS)
#define ASSERT_CUSTOM_QUERY     AGB_ASSERT(sRogueQueryPtr != NULL && sRogueQueryPtr->queryType == QUERY_TYPE_CUSTOM)
#define ASSERT_WEIGHT_QUERY     ASSERT_ANY_QUERY; AGB_ASSERT(sRogueQueryPtr->weightArray != NULL); AGB_ASSERT(sRogueQueryPtr->listArray == NULL)
#define ASSERT_LIST_QUERY     ASSERT_ANY_QUERY; AGB_ASSERT(sRogueQueryPtr->weightArray == NULL); AGB_ASSERT(sRogueQueryPtr->listArray != NULL)

static void SetQueryBitFlag(u16 elem, bool8 state);
static bool8 GetQueryBitFlag(u16 elem);

static u16 Query_GetEggSpecies(u16 species);
static void Query_ApplyEvolutions(u16 species, u8 level, bool8 items, bool8 removeWhenEvo);

static u16 Query_MaxBitCount();
static u16 Query_GetWeightArrayCount();

static void AllocQuery(u8 type)
{
    sRogueQueryPtr = malloc(sizeof(struct RogueQueryData));
    sRogueQueryPtr->queryType = type;
    sRogueQueryPtr->bitCount = 0;
    sRogueQueryPtr->bitFlags = &gRogueQueryBits[0];
    sRogueQueryPtr->weightArray = NULL;
    sRogueQueryPtr->listArray = NULL;
    sRogueQueryPtr->arrayCapacity = 0;
    sRogueQueryPtr->totalWeight = 0;

    memset(&sRogueQueryPtr->bitFlags[0], 0, MAX_QUERY_BYTE_COUNT);
}

static void FreeQuery()
{
    AGB_ASSERT(sRogueQueryPtr->weightArray == NULL);

    free(sRogueQueryPtr);
    sRogueQueryPtr = NULL;
}

static void SetQueryBitFlag(u16 elem, bool8 state)
{
    if(GetQueryBitFlag(elem) != state)
    {
        u16 idx = elem / 8;
        u16 bit = elem % 8;
        u8 bitMask = 1 << bit;

        ASSERT_ANY_QUERY;
        AGB_ASSERT(idx < MAX_QUERY_BYTE_COUNT);

        if(state)
        {
            sRogueQueryPtr->bitFlags[idx] |= bitMask;
            
            ++sRogueQueryPtr->bitCount;
            AGB_ASSERT(sRogueQueryPtr->bitCount < MAX_QUERY_BIT_COUNT);
        }
        else
        {
            sRogueQueryPtr->bitFlags[idx] &= ~bitMask;

            AGB_ASSERT(sRogueQueryPtr->bitCount != 0);
            --sRogueQueryPtr->bitCount;
        }
    }
}

static bool8 GetQueryBitFlag(u16 elem)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    ASSERT_ANY_QUERY;
    AGB_ASSERT(idx < MAX_QUERY_BYTE_COUNT);

    return (sRogueQueryPtr->bitFlags[idx] & bitMask) != 0;
}

// MISC QUERY
//
void RogueQuery_Init()
{
    sRogueQueryPtr = NULL;
    AGB_ASSERT(QUERY_NUM_TRAINERS >= gRogueTrainerCount);
}

void RogueMiscQuery_EditElement(u8 func, u16 elem)
{
    ASSERT_ANY_QUERY;
    SetQueryBitFlag(elem, func == QUERY_FUNC_INCLUDE);
}

void RogueMiscQuery_EditRange(u8 func, u16 fromId, u16 toId)
{
    u16 i;
    u16 maxBitCount = Query_MaxBitCount();

    ASSERT_ANY_QUERY;

    for(i = fromId; i <= toId && i < maxBitCount; ++i)
    {
        SetQueryBitFlag(i, func == QUERY_FUNC_INCLUDE);
    }
}

bool8 RogueMiscQuery_CheckState(u16 elem)
{
    return GetQueryBitFlag(elem);
}

void RogueMiscQuery_FilterByChance(u16 rngSeed, u8 func, u8 chance)
{
    u16 elem;
    u16 count = Query_MaxBitCount();
    u32 startSeed = gRngRogueValue;

    SeedRogueRng(rngSeed);

    for(elem = 1; elem < count; ++elem)
    {
        if(GetQueryBitFlag(elem))
        {
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!RogueRandomChance(chance, 0))
                {
                    SetQueryBitFlag(elem, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(RogueRandomChance(chance, 0))
                {
                    SetQueryBitFlag(elem, FALSE);
                }
            }
        }
    }

    gRngRogueValue = startSeed;
}

void RogueCustomQuery_Begin()
{
    ASSERT_NO_QUERY;
    AllocQuery(QUERY_TYPE_CUSTOM);
}

void RogueCustomQuery_End()
{
    ASSERT_CUSTOM_QUERY;
    FreeQuery();
}

// MON QUERY
//

void RogueMonQuery_Begin()
{
    ASSERT_NO_QUERY;
    AllocQuery(QUERY_TYPE_MON);
}

void RogueMonQuery_End()
{
    ASSERT_MON_QUERY;
    FreeQuery();
}

void RogueMonQuery_Reset(u8 func)
{
    u16 species;
    ASSERT_MON_QUERY;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(func == QUERY_FUNC_INCLUDE)
        {
            SetQueryBitFlag(species, TRUE);
        }
        else if(func == QUERY_FUNC_EXCLUDE)
        {
            SetQueryBitFlag(species, FALSE);
        }
    }
}

void RogueMonQuery_IsSpeciesActive()
{
    u16 species;
    ASSERT_MON_QUERY;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        SetQueryBitFlag(species, Query_IsSpeciesEnabled(species));
    }
}

void RogueMonQuery_TransformIntoEggSpecies()
{
    u16 species;
    u16 eggSpecies;
    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species))
        {
            eggSpecies = Query_GetEggSpecies(species);
            if(eggSpecies != species)
            {
                SetQueryBitFlag(eggSpecies, TRUE);
                SetQueryBitFlag(species, FALSE);
            }
        }
    }
}

void RogueMonQuery_TransformIntoEvos(u8 levelLimit, bool8 includeItemEvos, bool8 keepSourceSpecies)
{
    u16 species;

    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(Rogue_GetEvolutionCount(species) != 0 && GetQueryBitFlag(species))
        {
            Query_ApplyEvolutions(species, levelLimit, includeItemEvos, !keepSourceSpecies);
        }
    }
}

static void Query_ApplyEvolutions(u16 species, u8 level, bool8 items, bool8 removeWhenEvo)
{
    u8 i;
    bool8 hasEvolved;
    struct Evolution evo;

    hasEvolved = FALSE;

    ASSERT_MON_QUERY;
    
    for(i = 0; i < EVOS_PER_MON; ++i)
    {
        Rogue_ModifyEvolution(species, i, &evo);

        if(evo.method == 0 || evo.targetSpecies == SPECIES_NONE)
            continue;

        switch(evo.method)
        {
            // Leve evos
            case EVO_LEVEL:
            case EVO_LEVEL_ATK_GT_DEF:
            case EVO_LEVEL_ATK_EQ_DEF:
            case EVO_LEVEL_ATK_LT_DEF:
            case EVO_LEVEL_SILCOON:
            case EVO_LEVEL_CASCOON:
            case EVO_LEVEL_NINJASK:
            case EVO_LEVEL_SHEDINJA:
#ifdef ROGUE_EXPANSION
            case EVO_LEVEL_FEMALE:
            case EVO_LEVEL_MALE:
            case EVO_LEVEL_DAY:
            case EVO_LEVEL_DUSK:
            case EVO_LEVEL_NATURE_AMPED:
            case EVO_LEVEL_NATURE_LOW_KEY:
            case EVO_CRITICAL_HITS:
#endif
            if (evo.param > level)
                continue; // not the correct level to evolve
            break;
                
            // Item evos
            case EVO_ITEM:
            case EVO_LEVEL_ITEM:
#ifdef ROGUE_EXPANSION
            case EVO_ITEM_HOLD_DAY:
            case EVO_ITEM_HOLD_NIGHT:
            case EVO_MOVE:
            case EVO_MOVE_TYPE:
            case EVO_MAPSEC:
            case EVO_ITEM_MALE:
            case EVO_ITEM_FEMALE:
            case EVO_LEVEL_RAIN:
            case EVO_SPECIFIC_MON_IN_PARTY:
            case EVO_LEVEL_DARK_TYPE_MON_IN_PARTY:
            case EVO_SPECIFIC_MAP:
            case EVO_SCRIPT_TRIGGER_DMG:
            case EVO_DARK_SCROLL:
            case EVO_WATER_SCROLL:
#endif
            if (!items)
                continue; // not accepting item evos
            break;
        }

        // If we reach here we're allowed to evolve
        hasEvolved = TRUE;
        SetQueryBitFlag(evo.targetSpecies, TRUE);

        if(removeWhenEvo)
        {
            SetQueryBitFlag(species, FALSE);
        }

        if(evo.targetSpecies < species)
        {
            // We've already considered this species so we must reconsider it e.g. if a baby mon was introduced in later gen 
            // (Azuril is a good example as it will miss out on full evo chain)
            Query_ApplyEvolutions(evo.targetSpecies, level, items, removeWhenEvo);
        }
    }
}

void RogueMonQuery_IsOfType(u8 func, u32 typeFlags)
{
    u8 i;
    u16 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(typeFlags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species))
        {
            speciesFlags = 0;
            Rogue_AppendSpeciesTypeFlags(species, &speciesFlags);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if((typeFlags & speciesFlags) == 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if((typeFlags & speciesFlags) != 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_EvosContainType(u8 func, u32 typeFlags)
{
    u8 i;
    bool8 containsAnyType;
    u16 species;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(typeFlags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species))
        {
            containsAnyType = (typeFlags & Rogue_GetSpeciesEvolutionChainTypeFlags(species)) != 0;

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!containsAnyType)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(containsAnyType)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_ContainsPresetFlags(u8 func, u32 presetflags)
{
    u8 i;
    u16 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(presetflags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species))
        {
            speciesFlags = Rogue_GetMonFlags(species);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if((presetflags & speciesFlags) == 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if((presetflags & speciesFlags) != 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_IsLegendary(u8 func)
{
    u16 species;
    const bool8 checkState = (func == QUERY_FUNC_INCLUDE);
    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species) && RoguePokedex_IsSpeciesLegendary(species) != checkState)
        {
            SetQueryBitFlag(species, FALSE);
        }
    }
}

void RogueMonQuery_IsLegendaryWithPresetFlags(u8 func, u32 presetflags)
{
    u8 i;
    u16 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(presetflags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(RoguePokedex_IsSpeciesLegendary(species) && GetQueryBitFlag(species))
        {
            speciesFlags = Rogue_GetMonFlags(species);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if((presetflags & speciesFlags) == 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if((presetflags & speciesFlags) != 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_AnyActiveEvos(u8 func, bool8 includeMegas)
{
    bool8 hasValidEvo;
    u16 species, i;
    struct Evolution evo;

    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species))
        {
            hasValidEvo = FALSE;

            for (i = 0; i < EVOS_PER_MON; i++)
            {
                Rogue_ModifyEvolution(species, i, &evo);

                if (evo.targetSpecies != SPECIES_NONE && evo.method != 0)
                {
                    if(!includeMegas)
                    {
                        switch (evo.method)
                        {
                            case EVO_MEGA_EVOLUTION:
                            case EVO_MOVE_MEGA_EVOLUTION:
                            case EVO_PRIMAL_REVERSION:
                                continue;
                        }
                    }

                    hasValidEvo = TRUE;
                    break;
                }
            }
            
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!hasValidEvo)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else // if(func == QUERY_FUNC_EXCLUDE)
            {
                if(hasValidEvo)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_CustomFilter(QueryFilterCallback filterFunc, void* usrData)
{
    u16 species;
    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species) && !filterFunc(species, usrData))
        {
            SetQueryBitFlag(species, FALSE);
        }
    }
}

static u16 Query_GetEggSpecies(u16 inSpecies)
{
    // Edge case handling for specific pre evos added in later gens
    u8 genLimit = RoguePokedex_GetDexGenLimit();
    u16 species = Rogue_GetEggSpecies(inSpecies);

    if(genLimit == 1)
    {
        // Check egg species
        switch (species)
        {
        case SPECIES_PICHU:
            species = SPECIES_PIKACHU;
            break;
        
        case SPECIES_CLEFFA:
            species = SPECIES_CLEFAIRY;
            break;
        
        case SPECIES_IGGLYBUFF:
            species = SPECIES_JIGGLYPUFF;
            break;

        case SPECIES_TYROGUE:
            species = inSpecies;
            break;

        case SPECIES_SMOOCHUM:
            species = SPECIES_JYNX;
            break;

        case SPECIES_ELEKID:
            species = SPECIES_ELECTABUZZ;
            break;

        case SPECIES_MAGBY:
            species = SPECIES_MAGMAR;
            break;
        }
    }

    if(genLimit < 3)
    {
        // Check egg species
        switch (species)
        {
        case SPECIES_AZURILL:
            species = SPECIES_MARILL;
            break;

        case SPECIES_WYNAUT:
            species = SPECIES_WOBBUFFET;
            break;
        }
    }

#ifdef ROGUE_EXPANSION
    if(genLimit < 4)
    {
        // Check egg species
        switch (species)
        {
        case SPECIES_HAPPINY:
            species = SPECIES_CHANSEY;
            break;

        case SPECIES_MIME_JR:
            species = SPECIES_MR_MIME;
            break;

        case SPECIES_MUNCHLAX:
            species = SPECIES_SNORLAX;
            break;

        case SPECIES_BONSLY:
            species = SPECIES_SUDOWOODO;
            break;

        case SPECIES_MANTYKE:
            species = SPECIES_MANTINE;
            break;

        case SPECIES_BUDEW:
            species = SPECIES_ROSELIA;
            break;

        case SPECIES_CHINGLING:
            species = SPECIES_CHIMECHO;
            break;
        }
    }
#endif

    return species;
}

bool8 Query_IsSpeciesEnabledInternal(u16 species)
{
    if(Rogue_IsRunActive())
        return RoguePokedex_IsSpeciesEnabled(species);

    // Force species active in queries for hub activities
    return TRUE;
}

bool8 Query_IsSpeciesEnabled(u16 species)
{
    // Check if mon has valid data
    if(gBaseStats[species].abilities[0] != ABILITY_NONE && gBaseStats[species].catchRate != 0)
    {
#ifdef ROGUE_EXPANSION
        // Include specific forms in these queries
        if(species > FORMS_START)
        {
            // Regional forms
            if(species >= SPECIES_RATTATA_ALOLAN && species <= SPECIES_STUNFISK_GALARIAN)
                return Query_IsSpeciesEnabledInternal(species);

            // Alt forms
            // Gen4
            if(species >= SPECIES_BURMY_SANDY_CLOAK && species <= SPECIES_SHAYMIN_SKY)
                return Query_IsSpeciesEnabledInternal(species);

            // Gen5
            if(species == SPECIES_BASCULIN_BLUE_STRIPED || species == SPECIES_BASCULIN_WHITE_STRIPED)
                return Query_IsSpeciesEnabledInternal(species);

            if(species >= SPECIES_DEERLING_SUMMER && species <= SPECIES_KYUREM_BLACK)
                return Query_IsSpeciesEnabledInternal(species);

            // Gen6
            if(species == SPECIES_MEOWSTIC_FEMALE)
                return Query_IsSpeciesEnabledInternal(species);
    
            // Gen7
            if(species >= SPECIES_ORICORIO_POM_POM && species <= SPECIES_LYCANROC_DUSK)
                return Query_IsSpeciesEnabledInternal(species);

            if(species >= SPECIES_NECROZMA_DUSK_MANE && species <= SPECIES_NECROZMA_DAWN_WINGS)
                return Query_IsSpeciesEnabledInternal(species);

            if(species == SPECIES_MAGEARNA_ORIGINAL_COLOR)
                return Query_IsSpeciesEnabledInternal(species);

            // Gen8
            if(species >= SPECIES_TOXTRICITY_LOW_KEY && species <= SPECIES_POLTEAGEIST_ANTIQUE)
                return Query_IsSpeciesEnabledInternal(species);

            if(species == SPECIES_INDEEDEE_FEMALE)
                return Query_IsSpeciesEnabledInternal(species);

            if(species >= SPECIES_ZACIAN_CROWNED_SWORD && species <= SPECIES_ZAMAZENTA_CROWNED_SHIELD)
                return Query_IsSpeciesEnabledInternal(species);

            if(species >= SPECIES_CALYREX_ICE_RIDER && species <= SPECIES_CALYREX_SHADOW_RIDER)
                return Query_IsSpeciesEnabledInternal(species);

            // If we've gotten here then we're not interested in this form
            return FALSE;
        }
#endif

        return Query_IsSpeciesEnabledInternal(species);
    }

    return FALSE;
}

// ITEM QUERY
//

void RogueItemQuery_Begin()
{
    ASSERT_NO_QUERY;
    AllocQuery(QUERY_TYPE_ITEM);
}

void RogueItemQuery_End()
{
    ASSERT_ITEM_QUERY;
    FreeQuery();
}

void RogueItemQuery_Reset(u8 func)
{
    u16 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(func == QUERY_FUNC_INCLUDE)
        {
            SetQueryBitFlag(itemId, TRUE);
        }
        else if(func == QUERY_FUNC_EXCLUDE)
        {
            SetQueryBitFlag(itemId, FALSE);
        }
    }
}

bool8 Query_IsItemEnabled(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);

    if(item.itemId == itemId)
    {
        u8 gen = ItemToGen(itemId); // TODO - Rework this
        if(!IsGenEnabled(gen))
            return FALSE;

        // Query excluded items
        switch (itemId)
        {
        case ITEM_SACRED_ASH:
        case ITEM_REVIVAL_HERB:
        case ITEM_REVIVE:
        case ITEM_MAX_REVIVE:
        case ITEM_RARE_CANDY:
        case ITEM_HEART_SCALE:
        case ITEM_LUCKY_EGG:
        case ITEM_EXP_SHARE:
        case ITEM_SHOAL_SALT:
        case ITEM_SHOAL_SHELL:
        case ITEM_FLUFFY_TAIL:
        case ITEM_SOOTHE_BELL:

        case ITEM_DURIN_BERRY:
        case ITEM_PAMTRE_BERRY:
        case ITEM_NOMEL_BERRY:
        case ITEM_PINAP_BERRY:
        case ITEM_NANAB_BERRY:
        case ITEM_RAZZ_BERRY:
        case ITEM_ENIGMA_BERRY:
        case ITEM_BELUE_BERRY:
        case ITEM_WATMEL_BERRY:
        case ITEM_SPELON_BERRY:
        case ITEM_RABUTA_BERRY:
        case ITEM_CORNN_BERRY:
        case ITEM_WEPEAR_BERRY:
        case ITEM_BLUK_BERRY:

        // Berries that may confuse
        case ITEM_AGUAV_BERRY:
        case ITEM_WIKI_BERRY:
        case ITEM_PERSIM_BERRY:
        case ITEM_IAPAPA_BERRY:
        case ITEM_MAGO_BERRY:
        case ITEM_FIGY_BERRY:
        case ITEM_MAGOST_BERRY:
#ifdef ROGUE_EXPANSION

        // Not implemented/needed
        case ITEM_MAX_HONEY:
        case ITEM_LURE:
        case ITEM_SUPER_LURE:
        case ITEM_MAX_LURE:
        case ITEM_MAX_MUSHROOMS:
        case ITEM_WISHING_PIECE:
        case ITEM_ARMORITE_ORE:
        case ITEM_DYNITE_ORE:
        case ITEM_GALARICA_TWIG:
        case ITEM_SWEET_HEART:
        case ITEM_POKE_TOY:

        // Link cable is Rogue's item
        case ITEM_LINKING_CORD:

        // Not needed as is not a lvl up evo
        case ITEM_PRISM_SCALE:

        // Exclude all treasures then turn on the ones we want to use
        case ITEM_NUGGET:
        case ITEM_PEARL:
        case ITEM_BIG_PEARL:
        case ITEM_STARDUST:
        case ITEM_STAR_PIECE:

        // Ignore these, as mons/form swaps currently not enabled
        case ITEM_PIKASHUNIUM_Z:
        case ITEM_ULTRANECROZIUM_Z:
#endif
            return FALSE;
        }

        if(itemId >= FIRST_MAIL_INDEX && itemId <= LAST_MAIL_INDEX)
            return FALSE;

        if(itemId >= ITEM_RED_SCARF && itemId <= ITEM_YELLOW_SCARF)
            return FALSE;

        if(itemId >= ITEM_RED_SHARD && itemId <= ITEM_GREEN_SHARD)
            return FALSE;

        if(itemId >= ITEM_BLUE_FLUTE && itemId <= ITEM_WHITE_FLUTE)
            return FALSE;

#if !defined(ROGUE_EXPANSION)
        if(((itemId >= ITEM_HP_UP && itemId <= ITEM_CALCIUM) || itemId == ITEM_ZINC) && !Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
            return FALSE;
#endif
    
#ifdef ROGUE_EXPANSION
        if(itemId >= ITEM_GROWTH_MULCH && itemId <= ITEM_BLACK_APRICORN)
            return FALSE;

        // Exclude all treasures then turn on the ones we want to use
        if(itemId >= ITEM_BOTTLE_CAP && itemId <= ITEM_STRANGE_SOUVENIR)
            return FALSE;

        // These TMs aren't setup
        if(itemId >= ITEM_TM51 && itemId <= ITEM_TM100)
            return FALSE;

        // Regional treat (Avoid spawning in multiple)
        if(itemId >= ITEM_PEWTER_CRUNCHIES && itemId <= ITEM_BIG_MALASADA)
        {
            switch(gen)
            {
                case 1:
                    if(itemId != ITEM_PEWTER_CRUNCHIES)
                        return FALSE;
                    break;
                case 2:
                    if(itemId != ITEM_RAGE_CANDY_BAR)
                        return FALSE;
                    break;
                case 3:
                    if(itemId != ITEM_LAVA_COOKIE)
                        return FALSE;
                    break;
                case 4:
                    if(itemId != ITEM_OLD_GATEAU)
                        return FALSE;
                    break;
                case 5:
                    if(itemId != ITEM_CASTELIACONE)
                        return FALSE;
                    break;
                case 6:
                    if(itemId != ITEM_LUMIOSE_GALETTE)
                        return FALSE;
                    break;
                case 7:
                    if(itemId != ITEM_SHALOUR_SABLE)
                        return FALSE;
                    break;
                //case 8:
                default:
                    if(itemId != ITEM_BIG_MALASADA)
                        return FALSE;
                    break;
            }
        }

        // Ignore fossils for now
        if(itemId >= ITEM_HELIX_FOSSIL && itemId <= ITEM_FOSSILIZED_DINO)
            return FALSE;

        // Ignore sweets, as they are not used
        if(itemId >= ITEM_STRAWBERRY_SWEET && itemId <= ITEM_RIBBON_SWEET)
            return FALSE;

        // Exclude everything but plates
        if(itemId >= ITEM_DOUSE_DRIVE && itemId <= ITEM_CHILL_DRIVE)
            return FALSE;
        if(itemId >= ITEM_FIRE_MEMORY && itemId <= ITEM_FAIRY_MEMORY)
            return FALSE;

        if(itemId >= ITEM_RED_ORB && itemId <= ITEM_DIANCITE && !IsMegaEvolutionEnabled())
            return FALSE;

        if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z && !IsZMovesEnabled())
            return FALSE;
    
        if(itemId >= ITEM_EXP_CANDY_XS && itemId <= ITEM_DYNAMAX_CANDY && !IsDynamaxEnabled())
            return FALSE;

        // Disable EV items when setting not active
        if(itemId >= ITEM_HEALTH_FEATHER && itemId <= ITEM_SWIFT_FEATHER && !Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
            return FALSE;
    
        if(itemId >= ITEM_HP_UP && itemId <= ITEM_CARBOS && !Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
            return FALSE;
    
        if(itemId >= ITEM_MACHO_BRACE && itemId <= ITEM_POWER_ANKLET && !Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
            return FALSE;

#endif
        // If we managed to get here, we must be valid
        return TRUE;
    }

    return FALSE;
}

void RogueItemQuery_IsItemActive()
{
    u16 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        SetQueryBitFlag(itemId, Query_IsItemEnabled(itemId));
    }
}

void RogueItemQuery_IsStoredInPocket(u8 func, u8 pocket)
{
    u16 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(ItemId_GetPocket(itemId) != pocket)
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(ItemId_GetPocket(itemId) == pocket)
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
        }
    }
}

static bool8 IsMedicine(struct Item* item);

void RogueItemQuery_IsMedicine(u8 func)
{
    u16 itemId;
    struct Item item;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!IsMedicine(&item))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(IsMedicine(&item))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
        }
    }
}

void RogueItemQuery_IsEvolutionItem(u8 func)
{
    u16 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!Rogue_IsEvolutionItem(itemId))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(Rogue_IsEvolutionItem(itemId))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
        }
    }
}

void RogueItemQuery_InPriceRange(u8 func, u16 minPrice, u16 maxPrice)
{
    u16 itemId;
    struct Item item;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!(item.price >= minPrice && item.price <= maxPrice))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(item.price >= minPrice && item.price <= maxPrice)
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
        }
    }
}

static bool8 Query_IsGeneralShopItem(u16 itemId)
{
    if(Rogue_IsEvolutionItem(itemId))
        return FALSE;

    switch (itemId)
    {
    case ITEM_HP_UP:
    case ITEM_PROTEIN:
    case ITEM_IRON:
    case ITEM_CARBOS:
    case ITEM_CALCIUM:
    case ITEM_ZINC:
#ifdef ROGUE_EXPANSION
    case ITEM_ABILITY_CAPSULE:
    case ITEM_ABILITY_PATCH:
#endif

        return FALSE;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_RED_NECTAR && itemId <= ITEM_PURPLE_NECTAR)
        return FALSE;

    if(itemId >= ITEM_LONELY_MINT && itemId <= ITEM_SERIOUS_MINT)
        return FALSE;

    if(itemId >= ITEM_HEALTH_FEATHER && itemId <= ITEM_SWIFT_FEATHER)
        return FALSE;
#endif

    return TRUE;
}

void RogueItemQuery_IsGeneralShopItem(u8 func)
{
    u16 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!Query_IsGeneralShopItem(itemId))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(Query_IsGeneralShopItem(itemId))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
        }
    }
}

// TRAINER QUERY
//

void RogueTrainerQuery_Begin()
{
    ASSERT_NO_QUERY;
    AllocQuery(QUERY_TYPE_TRAINER);
}

void RogueTrainerQuery_End()
{
    ASSERT_TRAINER_QUERY;
    FreeQuery();
}

void RogueTrainerQuery_Reset(u8 func)
{
    u16 trainerNum;

    ASSERT_TRAINER_QUERY;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        if(func == QUERY_FUNC_INCLUDE)
        {
            SetQueryBitFlag(trainerNum, TRUE);
        }
        else if(func == QUERY_FUNC_EXCLUDE)
        {
            SetQueryBitFlag(trainerNum, FALSE);
        }
    }
}

void RogueTrainerQuery_ContainsTrainerFlag(u8 func, u32 trainerFlags)
{
    u16 trainerNum;
    bool8 containsAnyFlags;

    ASSERT_TRAINER_QUERY;

    // Skip and accept all if empty
    if(trainerFlags == 0)
        return;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        if(GetQueryBitFlag(trainerNum))
        {
            containsAnyFlags = (Rogue_GetTrainer(trainerNum)->trainerFlags & trainerFlags) != 0;

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!containsAnyFlags)
                {
                    SetQueryBitFlag(trainerNum, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(containsAnyFlags)
                {
                    SetQueryBitFlag(trainerNum, FALSE);
                }
            }
        }
    }
}

void RogueTrainerQuery_IsOfTypeGroup(u8 func, u16 typeGroup)
{
    u16 trainerNum;
    bool8 containsAnyFlags;

    ASSERT_TRAINER_QUERY;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        if(GetQueryBitFlag(trainerNum))
        {
            containsAnyFlags = Rogue_GetTrainerTypeGroupId(trainerNum) == typeGroup;

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!containsAnyFlags)
                {
                    SetQueryBitFlag(trainerNum, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(containsAnyFlags)
                {
                    SetQueryBitFlag(trainerNum, FALSE);
                }
            }
        }
    }
}


// ADVENTURE PATH QUERY
//
void RoguePathsQuery_Begin()
{
    ASSERT_NO_QUERY;
    AllocQuery(QUERY_TYPE_ADVENTURE_PATHS);
}

void RoguePathsQuery_End()
{
    ASSERT_PATHS_QUERY;
    FreeQuery();
}

void RoguePathsQuery_Reset(u8 func)
{
    u8 i;

    ASSERT_PATHS_QUERY;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(func == QUERY_FUNC_INCLUDE)
        {
            SetQueryBitFlag(i, TRUE);
        }
        else if(func == QUERY_FUNC_EXCLUDE)
        {
            SetQueryBitFlag(i, FALSE);
        }
    }
}

void RoguePathsQuery_IsOfType(u8 func, u8 roomType)
{
    u8 i;

    ASSERT_PATHS_QUERY;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(GetQueryBitFlag(i))
        {
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(gRogueAdvPath.rooms[i].roomType != roomType)
                {
                    SetQueryBitFlag(i, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                {
                    SetQueryBitFlag(i, FALSE);
                }
            }
        }
    }
}


//QUERY_NUM_ADVENTURE_PATH


// WEIGHT QUERY
//
static u16 Query_MaxBitCount()
{
    ASSERT_ANY_QUERY;
    if(sRogueQueryPtr->queryType == QUERY_TYPE_MON)
        return QUERY_NUM_SPECIES;
    else if(sRogueQueryPtr->queryType == QUERY_TYPE_ITEM)
        return QUERY_NUM_ITEMS;
    else if(sRogueQueryPtr->queryType == QUERY_TYPE_TRAINER)
        return gRogueTrainerCount;
    else if(sRogueQueryPtr->queryType == QUERY_TYPE_ADVENTURE_PATHS)
        return QUERY_NUM_ADVENTURE_PATH;
    else // QUERY_TYPE_CUSTOM
        return MAX_QUERY_BIT_COUNT;
}

static u16 Query_GetWeightArrayCount()
{
    ASSERT_WEIGHT_QUERY;

    if(sRogueQueryPtr->bitCount <= sRogueQueryPtr->arrayCapacity)
    {
        return sRogueQueryPtr->bitCount;
    }
    else
    {
        DebugPrintf("QueryWeight: Clamping as active bits too large (active_bits:%d capacity:%d)", sRogueQueryPtr->bitCount, sRogueQueryPtr->arrayCapacity);
        return sRogueQueryPtr->arrayCapacity;
    }
}

void RogueWeightQuery_Begin()
{
    ASSERT_ANY_QUERY;
    sRogueQueryPtr->weightArray = (u8*)((void*)&gRogueQueryBuffer[0]); // TODO - Dynamic alloc
    sRogueQueryPtr->arrayCapacity = ARRAY_COUNT(gRogueQueryBuffer) * sizeof(u16);
}

void RogueWeightQuery_End()
{
    ASSERT_WEIGHT_QUERY;
    sRogueQueryPtr->weightArray = NULL;
    sRogueQueryPtr->arrayCapacity = 0;
}

bool8 RogueWeightQuery_HasAnyWeights()
{
    ASSERT_WEIGHT_QUERY;
    return sRogueQueryPtr->bitCount != 0 && sRogueQueryPtr->totalWeight != 0;
}

bool8 RogueWeightQuery_HasMultipleWeights()
{
    ASSERT_WEIGHT_QUERY;
    return RogueWeightQuery_HasAnyWeights() && sRogueQueryPtr->bitCount > 1;
}

void RogueWeightQuery_CalculateWeights(WeightCallback callback, void* data)
{
    u8 weight;
    u16 elem;
    u16 index;
    u16 counter = 0;
    u16 maxBitCount = Query_MaxBitCount();
    u16 weightCount = Query_GetWeightArrayCount();

    ASSERT_WEIGHT_QUERY;

    sRogueQueryPtr->totalWeight = 0;

    for(elem = 0; elem < maxBitCount; ++elem)
    {
        if(GetQueryBitFlag(elem))
        {
            index = counter++;

            if(index < weightCount)
            {
                weight = callback(index, elem, data);

                sRogueQueryPtr->weightArray[index] = weight;
                sRogueQueryPtr->totalWeight += weight;
            }
        }
    }
}

void RogueWeightQuery_FillWeights(u8 weight)
{
    u16 i;
    u16 weightCount = Query_GetWeightArrayCount();

    ASSERT_WEIGHT_QUERY;

    sRogueQueryPtr->totalWeight = 0;

    for(i = 0; i < weightCount; ++i)
    {
        sRogueQueryPtr->weightArray[i] = weight;
        sRogueQueryPtr->totalWeight += weight;
    }
}

void RogueWeightQuery_UpdateIndividualWeight(u16 checkElem, u8 weight)
{
    u16 elem;
    u16 index;
    u16 counter = 0;
    u16 maxBitCount = Query_MaxBitCount();
    u16 weightCount = Query_GetWeightArrayCount();

    ASSERT_WEIGHT_QUERY;

    for(elem = 0; elem < maxBitCount; ++elem)
    {
        if(GetQueryBitFlag(elem))
        {
            index = counter++;

            if(index < weightCount)
            {
                if(elem == checkElem)
                {
                    AGB_ASSERT(sRogueQueryPtr->totalWeight > sRogueQueryPtr->weightArray[index]);

                    // Remove old weight and replace with new one
                    sRogueQueryPtr->totalWeight -= sRogueQueryPtr->weightArray[index];
                    sRogueQueryPtr->weightArray[index] = weight;
                    sRogueQueryPtr->totalWeight += weight;
                    return;
                }
            }
        }
    }

    // If we've gotten here we've tried to update the weight for an elem that doesn't exist
    AGB_ASSERT(FALSE);
}

static u16 RogueWeightQuery_SelectRandomFromWeightsInternal(u16 randValue, bool8 updateWeight, u8 newWeight)
{
    u8 weight;
    u16 elem;
    u16 index;
    u16 counter = 0;
    u16 maxBitCount = Query_MaxBitCount();
    u16 weightCount = Query_GetWeightArrayCount();
    u16 targetWeight = randValue % sRogueQueryPtr->totalWeight;

    ASSERT_WEIGHT_QUERY;
    AGB_ASSERT(sRogueQueryPtr->totalWeight != 0);

    for(elem = 0; elem < maxBitCount; ++elem)
    {
        if(GetQueryBitFlag(elem))
        {
            index = counter++;

            if(index < weightCount)
            {
                weight = sRogueQueryPtr->weightArray[index];

                if(weight != 0)
                {
                    if(targetWeight < weight)
                    {
                        // We've found the target!
                        if(updateWeight)
                        {
                            // Remove old weight and replace with new one
                            sRogueQueryPtr->totalWeight -= weight;
                            sRogueQueryPtr->weightArray[index] = newWeight;
                            sRogueQueryPtr->totalWeight += newWeight;
                        }

                        return elem;
                    }
                    else
                    {
                        // Still not reached target
                        targetWeight -= weight;
                    }
                }
            }
        }
    }

    // Shouldn't have gotten here
    AGB_ASSERT(FALSE);
    return 0;
}

u16 RogueWeightQuery_SelectRandomFromWeights(u16 randValue)
{
    return RogueWeightQuery_SelectRandomFromWeightsInternal(randValue, FALSE, 0);
}

u16 RogueWeightQuery_SelectRandomFromWeightsWithUpdate(u16 randValue, u8 updatedWeight)
{
    return RogueWeightQuery_SelectRandomFromWeightsInternal(randValue, TRUE, updatedWeight);
}

// LIST QUERY
//
void RogueListQuery_Begin()
{
    ASSERT_ANY_QUERY;
    AGB_ASSERT(sRogueQueryPtr->arrayCapacity == 0);

    // Attempt to dynamically allocate memory for us to use, as we can't fit into the preallocation size
    if(sRogueQueryPtr->bitCount + 1 >= ARRAY_COUNT(gRogueQueryBuffer))
    {
        sRogueQueryPtr->arrayCapacity = sRogueQueryPtr->bitCount + 1;
        sRogueQueryPtr->listArray = Alloc(sizeof(u16) * sRogueQueryPtr->arrayCapacity);

        if(sRogueQueryPtr->listArray == NULL)
        {
            // Failed to allocate, so fallback and just have a smaller list
            AGB_ASSERT(FALSE);
            sRogueQueryPtr->arrayCapacity = 0;
        }
    }

    // Use the preallocated array
    if(sRogueQueryPtr->arrayCapacity == 0)
    {
        sRogueQueryPtr->listArray = &gRogueQueryBuffer[0];
        sRogueQueryPtr->arrayCapacity = ARRAY_COUNT(gRogueQueryBuffer);
    }
}

void RogueListQuery_End()
{
    ASSERT_LIST_QUERY;
    AGB_ASSERT(sRogueQueryPtr->arrayCapacity != 0);

    // Free dynamically alloced array
    if(sRogueQueryPtr->arrayCapacity >= ARRAY_COUNT(gRogueQueryBuffer))
    {
        Free(sRogueQueryPtr->listArray);
    }

    sRogueQueryPtr->listArray = NULL;
    sRogueQueryPtr->arrayCapacity = 0;
}

bool8 SortItemPlaceBefore(u8 sortMode, u16 itemIdA, u16 itemIdB, u16 quantityA, u16 quantityB);

u16 const* RogueListQuery_CollapseItems(u8 sortMode, bool8 flipSort)
{
    u16 itemId;
    u16 index;
    ASSERT_ITEM_QUERY;
    ASSERT_LIST_QUERY;

    index = 0;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            sRogueQueryPtr->listArray[index++] = itemId;
    
            if(index >= sRogueQueryPtr->arrayCapacity - 1)
            {
                // Hit capacity (Probably means putting too much into 1 shop)
                break;
            }
        }
    }

    // Terminate
    sRogueQueryPtr->listArray[index] = ITEM_NONE;

    if(sortMode < ITEM_SORT_MODE_COUNT)
    {
        u16 i, j, temp;
        bool8 anySorts = FALSE;

        for(j = 0; j < index; ++j)
        {
            anySorts = FALSE;

            for(i = 1; i < index; ++i)
            {
                if(i == j)
                    continue;

                if(SortItemPlaceBefore(sortMode, sRogueQueryPtr->listArray[i], sRogueQueryPtr->listArray[i - 1], 1, 1) != flipSort)
                {
                    SWAP(sRogueQueryPtr->listArray[i], sRogueQueryPtr->listArray[i - 1], temp);
                    anySorts = TRUE;
                }
            }

            if(!anySorts)
                break;
        }
    }

    return sRogueQueryPtr->listArray;
}

// Old API
//
extern const u16* const gRegionalDexSpecies[];
extern u16 gRegionalDexSpeciesCount[];
//extern struct Evolution gEvolutionTable[][EVOS_PER_MON];

static void SetQueryState(u16 elem, bool8 state)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;

    u8 bitMask = 1 << bit;
    
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueQueryBits));
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

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueQueryBits));
    return gRogueQueryBits[idx] & bitMask;
}


void RogueQuery_Clear(void)
{
    gRogueQueryBufferSize = 0;
    memset(&gRogueQueryBits[0], 255, sizeof(u8) * ARRAY_COUNT(gRogueQueryBits));
}

void RogueQuery_CollapseSpeciesBuffer(void)
{
    u16 species;
    gRogueQueryBufferSize = 0;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES && gRogueQueryBufferSize < (QUERY_BUFFER_COUNT - 1); ++species)
    {
        if(GetQueryState(species))
        {
            gRogueQueryBuffer[gRogueQueryBufferSize++] = species;
        }
    }

    if(gRogueQueryBufferSize < QUERY_BUFFER_COUNT)
        gRogueQueryBuffer[gRogueQueryBufferSize] = 0;
}

void RogueQuery_CollapseItemBuffer(void)
{
    u16 itemId;
    gRogueQueryBufferSize = 0;
    
    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && gRogueQueryBufferSize < (QUERY_BUFFER_COUNT - 1); ++itemId)
    {
        if(GetQueryState(itemId))
        {
            gRogueQueryBuffer[gRogueQueryBufferSize++] = itemId;
        }
    }

    if(gRogueQueryBufferSize < QUERY_BUFFER_COUNT)
        gRogueQueryBuffer[gRogueQueryBufferSize] = 0;
}

u16* RogueQuery_BufferPtr(void)
{
    return &gRogueQueryBuffer[0];
}

u16 RogueQuery_BufferSize(void)
{
    return gRogueQueryBufferSize;
}

struct RogueQueryDebug RogueQuery_GetDebugData(void)
{
    struct RogueQueryDebug data;
    data.uncollapsedBufferPtr = &gRogueQueryBits[0];
    data.uncollapsedBufferCapacity = ARRAY_COUNT(gRogueQueryBits);
    data.collapseBufferPtr = &gRogueQueryBuffer[0];
    data.collapseSizePtr = &gRogueQueryBufferSize;
    return data;
}

bool8 RogueQuery_CheckIncluded(u16 id)
{
    return GetQueryState(id);
}

u16 RogueQuery_UncollapsedSpeciesSize(void)
{
    u16 species;
    u16 count = 0;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            ++count;
        }
    }

    return count;
}

u16 RogueQuery_UncollapsedItemSize(void)
{
    u16 itemId;
    u16 count = 0;
    
    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            ++count;
        }
    }

    return count;
}

u16 RogueQuery_AtUncollapsedIndex(u16 idx)
{
    u16 i;
    u16 counter = 0;
    
    for(i = 1; i < MAX_QUERY_BIT_COUNT; ++i)
    {
        if(GetQueryState(i))
        {
            if(idx == counter++)
                return i;
        }
    }

    return 0;
}

u16 RogueQuery_PopCollapsedIndex(u16 idx)
{
    u16 i;
    u16 value = gRogueQueryBuffer[idx];

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueQueryBuffer));

    SetQueryState(value, FALSE);
    --gRogueQueryBufferSize;

    for(i = idx; i < gRogueQueryBufferSize; ++i)
    {
        AGB_ASSERT(i + 1 < ARRAY_COUNT(gRogueQueryBuffer));
        gRogueQueryBuffer[i] = gRogueQueryBuffer[i + 1];
    }

    gRogueQueryBuffer[gRogueQueryBufferSize] = 0;
    return value;
}

u16 RogueQuery_PopUncollapsedIndex(u16 idx)
{
    u16 i;
    u16 counter = 0;
    
    for(i = 1; i < MAX_QUERY_BIT_COUNT; ++i)
    {
        if(GetQueryState(i))
        {
            if(idx == counter++)
            {
                SetQueryState(i, FALSE);
                return i;
            }
        }
    }

    return 0;
}

void RogueQuery_Include(u16 idx)
{
    SetQueryState(idx, TRUE);
}

void RogueQuery_IncludeRange(u16 fromId, u16 toId)
{
    u16 i;
    for(i = fromId; i <= toId; ++i)
    {
        SetQueryState(i, TRUE);
    }
}

void RogueQuery_Exclude(u16 idx)
{
    SetQueryState(idx, FALSE);
}

void RogueQuery_ExcludeAll(void)
{
    memset(&gRogueQueryBits[0], 0, sizeof(u8) * ARRAY_COUNT(gRogueQueryBits));
}

void RogueQuery_CustomSpecies(QueryCallback query, u16 usrData)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryState(species))
        {
            if(query(species, usrData) == FALSE)
            {
                SetQueryState(species, FALSE);
            }
        }
    }
}

void RogueQuery_CustomItems(QueryCallback query, u16 usrData)
{
    u16 itemId;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            if(query(itemId, usrData) == FALSE)
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

// Species
//

static bool8 IsFinalEvolution(u16 species)
{
    u16 s, e;
    struct Evolution evo;

    for (e = 0; e < EVOS_PER_MON; e++)
    {
        Rogue_ModifyEvolution(species, e, &evo);

        if (evo.targetSpecies != SPECIES_NONE)
        {
            return FALSE;
        }
    }

    return TRUE;
}

// Items
//

void RogueQuery_ItemsIsValid(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(item.itemId != itemId)
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsExcludeCommon(void)
{
    u16 itemId;
    u16 maxGen = VarGet(VAR_ROGUE_ENABLED_GEN_LIMIT);

    RogueQuery_Exclude(ITEM_SACRED_ASH);
    RogueQuery_Exclude(ITEM_REVIVAL_HERB);
    RogueQuery_Exclude(ITEM_REVIVE);
    RogueQuery_Exclude(ITEM_MAX_REVIVE);
    RogueQuery_Exclude(ITEM_RARE_CANDY);
    RogueQuery_Exclude(ITEM_HEART_SCALE);
    RogueQuery_Exclude(ITEM_LUCKY_EGG);

    RogueQuery_ItemsExcludeRange(FIRST_MAIL_INDEX, LAST_MAIL_INDEX);
    RogueQuery_ItemsExcludeRange(ITEM_RED_SCARF, ITEM_YELLOW_SCARF);
    RogueQuery_ItemsExcludeRange(ITEM_RED_SHARD, ITEM_GREEN_SHARD);
    RogueQuery_ItemsExcludeRange(ITEM_BLUE_FLUTE, ITEM_WHITE_FLUTE);
    
#ifdef ROGUE_EXPANSION
    // Not implemented
    RogueQuery_Exclude(ITEM_MAX_HONEY);
    RogueQuery_Exclude(ITEM_LURE);
    RogueQuery_Exclude(ITEM_SUPER_LURE);
    RogueQuery_Exclude(ITEM_MAX_LURE);

    RogueQuery_Exclude(ITEM_PRISM_SCALE); // Not needed as is not a lvl up evo
    RogueQuery_ItemsExcludeRange(ITEM_GROWTH_MULCH, ITEM_BLACK_APRICORN);

    // Exclude all treasures then turn on the ones we want to use
    RogueQuery_ItemsExcludeRange(ITEM_BOTTLE_CAP, ITEM_STRANGE_SOUVENIR);
    RogueQuery_Include(ITEM_NUGGET);
    RogueQuery_Include(ITEM_PEARL);
    RogueQuery_Include(ITEM_BIG_PEARL);
    RogueQuery_Include(ITEM_STARDUST);
    RogueQuery_Include(ITEM_STAR_PIECE);

    // These TMs aren't setup
    RogueQuery_ItemsExcludeRange(ITEM_TM51, ITEM_TM100);

    // Regional treat (Avoid spawning in multiple)
    RogueQuery_ItemsExcludeRange(ITEM_PEWTER_CRUNCHIES, ITEM_BIG_MALASADA);

    // Ignore fossils for now
    RogueQuery_ItemsExcludeRange(ITEM_HELIX_FOSSIL, ITEM_FOSSILIZED_DINO);

    // Ignore sweets, as they are not used
    RogueQuery_ItemsExcludeRange(ITEM_STRAWBERRY_SWEET, ITEM_RIBBON_SWEET);
    
    // Ignroe these, as mons/form swaps currently not enabled
    RogueQuery_Exclude(ITEM_PIKASHUNIUM_Z);
    RogueQuery_Exclude(ITEM_ULTRANECROZIUM_Z);

    // Exclude everything but plates
    //RogueQuery_ItemsExcludeRange(ITEM_FLAME_PLATE, ITEM_FAIRY_MEMORY);
    RogueQuery_ItemsExcludeRange(ITEM_DOUSE_DRIVE, ITEM_CHILL_DRIVE);
    RogueQuery_ItemsExcludeRange(ITEM_FIRE_MEMORY, ITEM_FAIRY_MEMORY);

    switch(maxGen)
    {
        case 1:
            RogueQuery_Include(ITEM_PEWTER_CRUNCHIES);
            break;
        case 2:
            RogueQuery_Include(ITEM_RAGE_CANDY_BAR);
            break;
        case 3:
            RogueQuery_Include(ITEM_LAVA_COOKIE);
            break;
        case 4:
            RogueQuery_Include(ITEM_OLD_GATEAU);
            break;
        case 5:
            RogueQuery_Include(ITEM_CASTELIACONE);
            break;
        case 6:
            RogueQuery_Include(ITEM_LUMIOSE_GALETTE);
            break;
        case 7:
            RogueQuery_Include(ITEM_SHALOUR_SABLE);
            break;
        //case 8:
        default:
            RogueQuery_Include(ITEM_BIG_MALASADA);
            break;
    }

    if(!IsMegaEvolutionEnabled())
    {
        RogueQuery_ItemsExcludeRange(ITEM_RED_ORB, ITEM_DIANCITE);
    }

    if(!IsZMovesEnabled())
    {
        RogueQuery_ItemsExcludeRange(ITEM_NORMALIUM_Z, ITEM_ULTRANECROZIUM_Z);
    }

    if(!IsDynamaxEnabled())
    {
        RogueQuery_ItemsExcludeRange(ITEM_EXP_CANDY_XS, ITEM_DYNAMAX_CANDY);
        RogueQuery_Exclude(ITEM_MAX_MUSHROOMS);
    }

    if(!Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
    {
        RogueQuery_ItemsExcludeRange(ITEM_HEALTH_FEATHER, ITEM_SWIFT_FEATHER);
        RogueQuery_ItemsExcludeRange(ITEM_HP_UP, ITEM_CARBOS);
        RogueQuery_ItemsExcludeRange(ITEM_MACHO_BRACE, ITEM_POWER_ANKLET);
    }
#else
    if(!Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
    {
        // These items aren't next to each other in vanilla
        RogueQuery_ItemsExcludeRange(ITEM_HP_UP, ITEM_CALCIUM);
        RogueQuery_Exclude(ITEM_ZINC);
    }
#endif

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            if(!IsGenEnabled(ItemToGen(itemId)) || Rogue_CheckCampaignBansItem(itemId))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsInPocket(u8 pocket)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(item.pocket != pocket)
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotInPocket(u8 pocket)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(item.pocket == pocket)
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsInPriceRange(u16 minPrice, u16 maxPrice)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(item.price < minPrice || item.price > maxPrice)
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

static bool8 IsExtraEvolutionItem(struct Item* item)
{
    switch(item->itemId)
    {
        case ITEM_LINK_CABLE:
        case ITEM_KINGS_ROCK:
        case ITEM_METAL_COAT:
        case ITEM_DRAGON_SCALE:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_DEEP_SEA_SCALE:
#ifdef ROGUE_EXPANSION
        case ITEM_UPGRADE:
        case ITEM_RAZOR_FANG:
        case ITEM_RAZOR_CLAW:
        case ITEM_BLACK_AUGURITE:
        case ITEM_PEAT_BLOCK:
#else
        case ITEM_UP_GRADE:
#endif
            return TRUE;
    }

#ifdef ROGUE_EXPANSION
    if(item->itemId >= ITEM_FIRE_STONE && item->itemId <= ITEM_RIBBON_SWEET)
    {
        return TRUE;
    }

    if(item->itemId >= ITEM_RED_ORB && item->itemId <= ITEM_DIANCITE)
    {
        return TRUE;
    }

    if(item->itemId >= ITEM_RED_NECTAR && item->itemId <= ITEM_PURPLE_NECTAR)
    {
        return TRUE;
    }
#endif

    return FALSE;
}

static bool8 IsRareHeldItem(struct Item* item);

static bool8 IsBattleEnchancer(struct Item* item)
{
    return (item->itemId >= ITEM_HP_UP && item->itemId <= ITEM_PP_MAX) ||
        item->fieldUseFunc == ItemUseOutOfBattle_EvolutionStone ||
        item->battleUseFunc == ItemUseInBattle_StatIncrease ||
#ifdef ROGUE_EXPANSION
        (item->itemId >= ITEM_HEALTH_FEATHER && item->itemId <= ITEM_SERIOUS_MINT) ||
#endif
        IsExtraEvolutionItem(item);
}

static bool8 IsMedicine(struct Item* item)
{
    return (item->fieldUseFunc == ItemUseOutOfBattle_Medicine || 
        item->fieldUseFunc == ItemUseOutOfBattle_PPRecovery ||
        item->fieldUseFunc == ItemUseOutOfBattle_Repel) 
        && !IsBattleEnchancer(item);
}

static bool8 IsHeldItem(struct Item* item)
{
    return item->holdEffect != HOLD_EFFECT_NONE && !IsBattleEnchancer(item);
}

static bool8 IsRareHeldItem(struct Item* item)
{
#ifdef ROGUE_EXPANSION
    return (item->itemId >= ITEM_RED_ORB && item->itemId <= ITEM_DIANCITE) ||
        (item->itemId >= ITEM_NORMALIUM_Z && item->itemId <= ITEM_ULTRANECROZIUM_Z) ||
        (item->itemId == ITEM_RUSTED_SWORD || item->itemId == ITEM_RUSTED_SHIELD) ||
        (item->itemId == ITEM_ADAMANT_ORB || item->itemId == ITEM_LUSTROUS_ORB || item->itemId == ITEM_GRISEOUS_ORB) ||
        (item->itemId == ITEM_ADAMANT_CRYSTAL || item->itemId == ITEM_LUSTROUS_GLOBE || item->itemId == ITEM_GRISEOUS_CORE) ||
        item->itemId == ITEM_SOUL_DEW ||
        item->itemId == ITEM_DYNAMAX_CANDY ||
        item->itemId == ITEM_MAX_MUSHROOMS;
#else
    return FALSE;
#endif
}

void RogueQuery_ItemsHeldItem(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(!IsHeldItem(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotHeldItem(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(IsHeldItem(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsRareHeldItem(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(!IsRareHeldItem(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotRareHeldItem(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(IsRareHeldItem(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsMedicine(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(!IsMedicine(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotMedicine(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(IsMedicine(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsBattleEnchancer(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(!IsBattleEnchancer(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsNotBattleEnchancer(void)
{
    u16 itemId;
    struct Item item;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            Rogue_ModifyItem(itemId, &item);

            if(IsBattleEnchancer(&item))
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}

void RogueQuery_ItemsExcludeRange(u16 fromId, u16 toId)
{
    u16 itemId;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(GetQueryState(itemId))
        {
            if(itemId >= fromId && itemId <= toId)
            {
                SetQueryState(itemId, FALSE);
            }
        }
    }
}
