#include "global.h"
#include "constants/abilities.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "data.h"

#include "event_data.h"
#include "item.h"
#include "item_use.h"
#include "malloc.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"

#include "rogue_adventurepaths.h"
#include "rogue_query.h"
#include "rogue_baked.h"
#include "rogue_campaign.h"
#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_pokedex.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"

#ifdef ROGUE_EXPANSION
#define QUERY_NUM_SPECIES           PLACEHOLDER_START
#else
#define QUERY_NUM_SPECIES           NUM_SPECIES
#endif

#define QUERY_BUFFER_COUNT          128
#define QUERY_NUM_ITEMS             ITEMS_COUNT
#define QUERY_NUM_TRAINERS          320 // just a vague guess that needs to at least match gRogueTrainerCount
#define QUERY_NUM_ADVENTURE_PATH    ROGUE_ADVPATH_ROOM_CAPACITY
#define QUERY_NUM_MOVES             MOVES_COUNT

#define MAX_QUERY_BIT_COUNT (max(QUERY_NUM_MOVES, max(QUERY_NUM_ITEMS, max(QUERY_NUM_ADVENTURE_PATH, max(QUERY_NUM_TRAINERS, max(ITEMS_COUNT, QUERY_NUM_SPECIES))))))
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
    QUERY_TYPE_MOVES,
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
    u16 debugDumpCounter;
    u8 queryType;
    bool8 dynamicAllocListArray;
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 queryStartClock;
#endif
};

EWRAM_DATA static struct RogueQueryData sRogueQuery = {0};

#define ASSERT_NO_QUERY         AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_NONE)
#define ASSERT_ANY_QUERY        AGB_ASSERT(sRogueQuery.queryType != QUERY_TYPE_NONE)
#define ASSERT_MON_QUERY        AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_MON)
#define ASSERT_ITEM_QUERY       AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_ITEM)
#define ASSERT_TRAINER_QUERY    AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_TRAINER)
#define ASSERT_PATHS_QUERY      AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_ADVENTURE_PATHS)
#define ASSERT_CUSTOM_QUERY     AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_CUSTOM)
#define ASSERT_MOVES_QUERY      AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_MOVES)
#define ASSERT_WEIGHT_QUERY     ASSERT_ANY_QUERY; AGB_ASSERT(sRogueQuery.weightArray != NULL); AGB_ASSERT(sRogueQuery.listArray == NULL)
#define ASSERT_LIST_QUERY     ASSERT_ANY_QUERY; AGB_ASSERT(sRogueQuery.weightArray == NULL); AGB_ASSERT(sRogueQuery.listArray != NULL)

static void SetQueryBitFlag(u16 elem, bool8 state);
static bool8 GetQueryBitFlag(u16 elem);

static u16 Query_GetEggSpecies(u16 species);
static void Query_ApplyEvolutions(u16 species, u8 level, bool8 items, bool8 removeWhenEvo);

static u16 Query_MaxBitCount();
static u16 Query_GetWeightArrayCount();

static void AllocQuery(u8 type)
{
    memset(&sRogueQuery, 0, sizeof(sRogueQuery));
    sRogueQuery.queryType = type;
    sRogueQuery.debugDumpCounter = 0;
    sRogueQuery.bitCount = 0;
    sRogueQuery.bitFlags = &gRogueQueryBits[0];
    sRogueQuery.weightArray = NULL;
    sRogueQuery.listArray = NULL;
    sRogueQuery.dynamicAllocListArray = FALSE;
    sRogueQuery.arrayCapacity = 0;
    sRogueQuery.totalWeight = 0;

    memset(&sRogueQuery.bitFlags[0], 0, MAX_QUERY_BYTE_COUNT);
    
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    sRogueQuery.queryStartClock = RogueDebug_SampleClock();
    DebugPrintf("[Query] Main Start %d", sRogueQuery.queryType);
#endif
}

static void FreeQuery()
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    {
        u32 queryDuration = RogueDebug_SampleClock() - sRogueQuery.queryStartClock;
        DebugPrintf("[Query] End %d (duration: %d us)", sRogueQuery.queryType, RogueDebug_ClockToDisplayUnits(queryDuration));
    }
#endif
    AGB_ASSERT(sRogueQuery.weightArray == NULL);
    sRogueQuery.queryType = QUERY_TYPE_NONE;
}

// Disable optimisation by disabling this
#if 1

static u32 IncrementActiveIteratorInternal(u32 i)
{
    // Attempt to avoid uncessary reads by working in steps of 32
    u32* data32 = ((u32*)sRogueQuery.bitFlags);
    u32 value, mask, offset, remainder;

    ++i; // always offset by 1

    offset = i / 32;
    remainder = i % 32;
    value = data32[offset];

    while(offset < sizeof(gRogueQueryBits) && i < MAX_QUERY_BYTE_COUNT * 8)
    {
        if(value == 0)
        {
            // Can optimise and do a big step here
            // Skip the entire 32 bit value
            remainder = 0;
            ++offset;
            value = data32[offset];

            i = offset * 32;
            continue;
        }

        mask = (1 << remainder);

        // Found a bit that is active
        if(value & mask)
            break;

        ++i;
        remainder = i % 32;

        if(remainder == 0)
        {
            // Entered a new 32 range to check
            offset = i / 32;
            value = data32[offset];
        }
    }

    // Clamp
    if(i >= MAX_QUERY_BYTE_COUNT * 8)
        i = MAX_QUERY_BYTE_COUNT * 8 - 1;

    return i;
}

static u32 IncrementActiveIterator(u32 startIndex)
{
    // Validation checks, this is only really needed for checking
#if defined(ROGUE_DEBUG) && 0
    u32 i;
    u32 endIndex = IncrementActiveIteratorInternal(startIndex);

    for(i = startIndex + 1; i < endIndex; ++i)
    {
        AGB_ASSERT(GetQueryBitFlag(i) == FALSE);
    }

    return endIndex;
#else
    return IncrementActiveIteratorInternal(startIndex);
#endif
}

#define ITERATOR_INC(i) (i = IncrementActiveIterator(i))
#else

#define ITERATOR_INC(i) (++i)
#endif

static void SetQueryBitFlag(u16 elem, bool8 state)
{
    if(GetQueryBitFlag(elem) != state)
    {
        u32 idx = elem / 8;
        u8 bit = elem % 8;
        u8 bitMask = 1 << bit;

        ASSERT_ANY_QUERY;
        AGB_ASSERT(idx < MAX_QUERY_BYTE_COUNT);

        if(state)
        {
            sRogueQuery.bitFlags[idx] |= bitMask;
            
            ++sRogueQuery.bitCount;
            AGB_ASSERT(sRogueQuery.bitCount < MAX_QUERY_BIT_COUNT);
        }
        else
        {
            sRogueQuery.bitFlags[idx] &= ~bitMask;

            AGB_ASSERT(sRogueQuery.bitCount != 0);
            --sRogueQuery.bitCount;
        }
    }
}

static bool8 GetQueryBitFlag(u16 elem)
{
    u32 idx = elem / 8;
    u8 bit = elem % 8;
    u8 bitMask = 1 << bit;

    ASSERT_ANY_QUERY;
    AGB_ASSERT(idx < MAX_QUERY_BYTE_COUNT);

    return (sRogueQuery.bitFlags[idx] & bitMask) != 0;
}

// MISC QUERY
//
void RogueQuery_Init()
{
    memset(&sRogueQuery, 0, sizeof(sRogueQuery));
    AGB_ASSERT(QUERY_NUM_TRAINERS >= gRogueTrainerCount);
}

void RogueMiscQuery_EditElement(u8 func, u16 elem)
{
    ASSERT_ANY_QUERY;
    SetQueryBitFlag(elem, func == QUERY_FUNC_INCLUDE);
}

void RogueMiscQuery_EditRange(u8 func, u16 fromId, u16 toId)
{
    u32 i;
    u32 maxBitCount = Query_MaxBitCount();

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

bool8 RogueMiscQuery_AnyActiveStates(u16 fromId, u16 toId)
{
    u32 i;

    for(i = fromId; i <= toId; ++i)
    {
        if(RogueMiscQuery_CheckState(i))
            return TRUE;
    }

    return FALSE;
}

void RogueMiscQuery_FilterByChance(u16 rngSeed, u8 func, u8 chance, u8 minCount)
{
    u32 elem;
    u32 count = Query_MaxBitCount();
    RAND_TYPE startSeed = gRngRogueValue;

    ASSERT_ANY_QUERY;

    SeedRogueRng(rngSeed);

    for(elem = 1; elem < count && sRogueQuery.bitCount > minCount; ITERATOR_INC(elem))
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

bool8 RogueMiscQuery_AnyActiveElements()
{
    ASSERT_ANY_QUERY;
    return sRogueQuery.bitCount != 0;
}

u16 RogueMiscQuery_SelectRandomElement(u16 rngValue)
{
    u32 elem;
    u32 currIndex;
    u32 count = Query_MaxBitCount();
    u32 desiredIndex = rngValue % sRogueQuery.bitCount;
    ASSERT_ANY_QUERY;
    AGB_ASSERT(RogueMiscQuery_AnyActiveElements());

    currIndex = 0;

    for(elem = 0; elem < count; ITERATOR_INC(elem))
    {
        if(GetQueryBitFlag(elem))
        {
            if(currIndex++ == desiredIndex)
                return elem;
        }
    }

    // Should never reach here
    AGB_ASSERT(FALSE);
    return elem - 1;
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

    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_MON_QUERY))
    {
        RogueDebugQuery_FillPC(FALSE);
    }
}

void RogueMonQuery_End()
{
    ASSERT_MON_QUERY;
    FreeQuery();
    
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_MON_QUERY))
    {
        RogueDebugQuery_FillPC(TRUE);
    }
}

void RogueMonQuery_Reset(u8 func)
{
    u32 species;
    ASSERT_MON_QUERY;

    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_MON_QUERY))
    {
        RogueDebugQuery_FillPC(TRUE);
    }

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
    u32 species;
    ASSERT_MON_QUERY;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        SetQueryBitFlag(species, Query_IsSpeciesEnabled(species));
    }
}

void RogueMonQuery_IsBaseSpeciesInCurrentDex(u8 func)
{
    u32 species;
    ASSERT_MON_QUERY;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species))
        {
            bool8 inDex = RoguePokedex_IsBaseSpeciesEnabled(species);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!inDex)
                    SetQueryBitFlag(species, FALSE);
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(inDex)
                    SetQueryBitFlag(species, FALSE);
            }
        }
    }
}

void RogueMonQuery_IsSeenInPokedex(u8 func)
{
    u32 species;
    ASSERT_MON_QUERY;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species))
        {
            bool8 inDex = GetSetPokedexSpeciesFlag(species, FLAG_GET_SEEN);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!inDex)
                    SetQueryBitFlag(species, FALSE);
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(inDex)
                    SetQueryBitFlag(species, FALSE);
            }
        }
    }
}

void RogueMonQuery_TransformIntoEggSpecies()
{
    u32 species;
    u32 eggSpecies;
    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
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
    u32 species;

    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(Rogue_GetMaxEvolutionCount(species) != 0 && GetQueryBitFlag(species))
        {
            Query_ApplyEvolutions(species, levelLimit, includeItemEvos, !keepSourceSpecies);
        }
    }
}

static void Query_ApplyEvolutions(u16 species, u8 level, bool8 items, bool8 removeWhenEvo)
{
    u32 i;
    struct Evolution evo;
    u32 evoCount = Rogue_GetMaxEvolutionCount(species);

    ASSERT_MON_QUERY;
    
    for(i = 0; i < evoCount; ++i)
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
            case EVO_LEVEL_DAY:
            case EVO_LEVEL_NIGHT:
#ifdef ROGUE_EXPANSION
            case EVO_LEVEL_FEMALE:
            case EVO_LEVEL_MALE:
            case EVO_LEVEL_DUSK:
            case EVO_LEVEL_NATURE_AMPED:
            case EVO_LEVEL_NATURE_LOW_KEY:

            case EVO_LEVEL_FAMILY_OF_THREE:
            case EVO_LEVEL_FAMILY_OF_FOUR:
            case EVO_LEVEL_TWO_SEGMENT:
            case EVO_LEVEL_THREE_SEGMENT:
#endif
                if (evo.param > level)
                    continue; // not the correct level to evolve
            break;
                
#ifdef ROGUE_EXPANSION
            case EVO_MOVE_TYPE: // assume we have a move of this type by this level
            case EVO_LEVEL_30_NATURE:
            if (30 >= level)
                continue;
            break;

            case EVOLUTIONS_END:
            case EVO_NONE:
                // Ignore these
                continue;
            break;
#endif

            // Item evos
            case EVO_ITEM:
#ifdef ROGUE_EXPANSION
            //case EVO_ITEM_DAY:
            //case EVO_ITEM_NIGHT:
            case EVO_ITEM_MALE:
            case EVO_ITEM_FEMALE:
#endif
                if (!items)
                    continue; // not accepting item evos
            break;

            default:
                // If we get here it means we've not considered this evo so make sure to add it to this switch statement or fix the source data
                AGB_ASSERT(FALSE);
                break;
        }

        // If we reach here we're allowed to evolve
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
    u32 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(typeFlags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
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

void RogueMonQuery_IsOfGeneration(u8 func, u32 generationFlags)
{
    u32 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(generationFlags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species))
        {
            speciesFlags = MON_GEN_TO_FLAGS(SpeciesToGen(species));

            if(func == QUERY_FUNC_INCLUDE)
            {
                if((generationFlags & speciesFlags) == 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if((generationFlags & speciesFlags) != 0)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_EvosContainType(u8 func, u32 typeFlags)
{
    bool32 containsAnyType;
    u32 species;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(typeFlags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
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
    u32 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(presetflags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
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
    u32 species;
    const bool32 checkState = (func == QUERY_FUNC_INCLUDE);
    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species) && RoguePokedex_IsSpeciesLegendary(species) != checkState)
        {
            SetQueryBitFlag(species, FALSE);
        }
    }
}

void RogueMonQuery_IsLegendaryWithPresetFlags(u8 func, u32 presetflags)
{
    u32 species;
    u32 speciesFlags;

    ASSERT_MON_QUERY;

    // Skip and accept all if empty
    if(presetflags == 0)
        return;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species) && RoguePokedex_IsSpeciesLegendary(species))
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

void RogueMonQuery_IsBoxLegendary(u8 func)
{
    bool32 valid;
    u32 species;

    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species) && RoguePokedex_IsSpeciesLegendary(species))
        {
            valid = RoguePokedex_IsSpeciesValidBoxLegendary(species);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!valid)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(valid)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_IsRoamerLegendary(u8 func)
{
    bool32 valid;
    u32 species;

    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species) && RoguePokedex_IsSpeciesLegendary(species))
        {
            valid = RoguePokedex_IsSpeciesValidRoamerLegendary(species);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!valid)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(valid)
                {
                    SetQueryBitFlag(species, FALSE);
                }
            }
        }
    }
}

void RogueMonQuery_AnyActiveEvos(u8 func)
{
    bool32 hasValidEvo;
    u32 species, i;
    struct Evolution evo;
    u32 evoCount;

    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
    {
        if(GetQueryBitFlag(species))
        {
            hasValidEvo = FALSE;
            evoCount = Rogue_GetMaxEvolutionCount(species);

            for (i = 0; i < evoCount; i++)
            {
                Rogue_ModifyEvolution(species, i, &evo);

                if (evo.targetSpecies != SPECIES_NONE && evo.method != 0)
                {
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
    u32 species;
    ASSERT_MON_QUERY;
    
    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ITERATOR_INC(species))
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
    u32 genLimit = RoguePokedex_GetDexGenLimit();
    u32 species = Rogue_GetEggSpecies(inSpecies);

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
    if(gRogueSpeciesInfo[species].baseHP != 0)
    {
#ifdef ROGUE_EXPANSION
        if(species > GEN9_START && species <= PLACEHOLDER_START)
        {
            // Gen 9 section is after the forms start
            // Illegal species for either wild or trainers
            switch (species)
            {
            //case SPECIES_MAUSHOLD_FAMILY_OF_FOUR:
            case SPECIES_PALAFIN_HERO:
            //case SPECIES_DUDUNSPARCE_THREE_SEGMENT:
            case SPECIES_GIMMIGHOUL_ROAMING:
            case SPECIES_OGERPON_TEAL_MASK_TERA:
            case SPECIES_OGERPON_WELLSPRING_MASK_TERA:
            case SPECIES_OGERPON_HEARTHFLAME_MASK_TERA:
            case SPECIES_OGERPON_CORNERSTONE_MASK_TERA:
            case SPECIES_TERAPAGOS_TERASTAL:
            case SPECIES_TERAPAGOS_STELLAR:
                return FALSE;
            
            }
        }

        // Include specific forms in these queries
        else if(species > FORMS_START)
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
#else
        if(species >= SPECIES_OLD_UNOWN_B && species <= SPECIES_OLD_UNOWN_Z)
            return FALSE;

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
    
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_ITEM_QUERY))
    {
        RogueDebugQuery_FillBag();
    }
}

void RogueItemQuery_Reset(u8 func)
{
    u32 itemId;
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

void RogueItemQuery_IsItemActive()
{
    u32 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        SetQueryBitFlag(itemId, Rogue_IsItemEnabled(itemId));
    }
}

void RogueItemQuery_IsStoredInPocket(u8 func, u8 pocket)
{
    u32 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ITERATOR_INC(itemId))
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

void RogueItemQuery_IsEvolutionItem(u8 func)
{
    u32 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ITERATOR_INC(itemId))
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
    u32 itemId;
    u32 price;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ITERATOR_INC(itemId))
    {
        if(GetQueryBitFlag(itemId))
        {
            price = ItemId_GetPrice(itemId);

            if(func == QUERY_FUNC_INCLUDE)
            {
                if(!(price >= minPrice && price <= maxPrice))
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(price >= minPrice && price <= maxPrice)
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
    u32 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ITERATOR_INC(itemId))
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

void RogueItemQuery_IsHeldItem(u8 func)
{
    u32 itemId;
    ASSERT_ITEM_QUERY;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ITERATOR_INC(itemId))
    {
        if(GetQueryBitFlag(itemId))
        {
            if(func == QUERY_FUNC_INCLUDE)
            {
                if(ItemId_GetHoldEffect(itemId) == HOLD_EFFECT_NONE)
                {
                    SetQueryBitFlag(itemId, FALSE);
                }
            }
            else if(func == QUERY_FUNC_EXCLUDE)
            {
                if(ItemId_GetHoldEffect(itemId) != HOLD_EFFECT_NONE)
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
    u32 trainerNum;

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

void RogueTrainerQuery_ContainsClassFlag(u8 func, u32 classFlags)
{
    u32 trainerNum;
    bool32 containsAnyFlags;

    ASSERT_TRAINER_QUERY;

    // Skip and accept all if empty
    if(classFlags == 0)
        return;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ITERATOR_INC(trainerNum))
    {
        if(GetQueryBitFlag(trainerNum))
        {
            containsAnyFlags = (Rogue_GetTrainer(trainerNum)->classFlags & classFlags) != 0;

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

void RogueTrainerQuery_ContainsTrainerFlag(u8 func, u32 trainerFlags)
{
    u32 trainerNum;
    bool32 containsAnyFlags;

    ASSERT_TRAINER_QUERY;

    // Skip and accept all if empty
    if(trainerFlags == 0)
        return;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ITERATOR_INC(trainerNum))
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
    u32 trainerNum;
    bool32 containsAnyFlags;

    ASSERT_TRAINER_QUERY;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ITERATOR_INC(trainerNum))
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
    u32 i;

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
    u32 i;

    ASSERT_PATHS_QUERY;

    for(i = 0; i < gRogueAdvPath.roomCount; ITERATOR_INC(i))
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


// MOVES QUERY
//
void RogueMoveQuery_Begin()
{
    ASSERT_NO_QUERY;
    AllocQuery(QUERY_TYPE_MOVES);
}

void RogueMoveQuery_End()
{
    ASSERT_MOVES_QUERY;
    FreeQuery();
}

void RogueMoveQuery_Reset(u8 func)
{
    u32 i;

    ASSERT_MOVES_QUERY;

    for(i = MOVE_NONE + 1; i < QUERY_NUM_MOVES; ++i) // ITERATOR_INC(i) ?
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

void RogueMoveQuery_IsTM(u8 func)
{
    u32 i, move;

    ASSERT_MOVES_QUERY;

    if(func == QUERY_FUNC_INCLUDE)
    {
        // TODO
        AGB_ASSERT(FALSE);
    }
    else if(func == QUERY_FUNC_EXCLUDE)
    {
        for(i = 0; i < NUM_TECHNICAL_MACHINES; ++i) // ITERATOR_INC(i) ?
        {
            move = ItemIdToBattleMoveId(ITEM_TM01 + i);
            if(GetQueryBitFlag(move))
            {
                SetQueryBitFlag(move, FALSE);
            }
        }
    }
}

void RogueMoveQuery_IsHM(u8 func)
{
    ASSERT_MOVES_QUERY;

    if(func == QUERY_FUNC_INCLUDE)
    {
        // TODO
        AGB_ASSERT(FALSE);
    }
    else if(func == QUERY_FUNC_EXCLUDE)
    {
#ifndef ROGUE_FEATURE_REMOVE_HIDDEN_MACHINES
        u16 move;
        u16 i;

        for(i = 0; i < NUM_HIDDEN_MACHINES; ++i) // ITERATOR_INC(i) ?
        {
            move = ItemIdToBattleMoveId(ITEM_HM01 + i);
            if(GetQueryBitFlag(move))
            {
                SetQueryBitFlag(move, FALSE);
            }
        }
#endif
    }
}



// WEIGHT QUERY
//
static u16 Query_MaxBitCount()
{
    ASSERT_ANY_QUERY;

    switch (sRogueQuery.queryType)
    {
    case QUERY_TYPE_MON:
        return QUERY_NUM_SPECIES;
    
    case QUERY_TYPE_ITEM:
        return QUERY_NUM_ITEMS;
    
    case QUERY_TYPE_TRAINER:
        return gRogueTrainerCount;
    
    case QUERY_TYPE_ADVENTURE_PATHS:
        return QUERY_NUM_ADVENTURE_PATH;
    
    case QUERY_TYPE_MOVES:
        return QUERY_NUM_MOVES;
    
    default: // QUERY_TYPE_CUSTOM
        return MAX_QUERY_BIT_COUNT;
    }
}

static u16 Query_GetWeightArrayCount()
{
    ASSERT_WEIGHT_QUERY;

    if(sRogueQuery.bitCount <= sRogueQuery.arrayCapacity)
    {
        return sRogueQuery.bitCount;
    }
    else
    {
        DebugPrintf("QueryWeight: Clamping as active bits too large (active_bits:%d capacity:%d)", sRogueQuery.bitCount, sRogueQuery.arrayCapacity);
        return sRogueQuery.arrayCapacity;
    }
}

bool8 RogueWeightQuery_IsOverSafeCapacity()
{
    ASSERT_ANY_QUERY;
    return sRogueQuery.bitCount > ARRAY_COUNT(gRogueQueryBuffer) * sizeof(u16);
}

void RogueWeightQuery_Begin()
{
    ASSERT_ANY_QUERY;
    sRogueQuery.weightArray = (u8*)((void*)&gRogueQueryBuffer[0]); // TODO - Dynamic alloc
    sRogueQuery.arrayCapacity = ARRAY_COUNT(gRogueQueryBuffer) * sizeof(u16);
    
    // Remove random entries until we can safely calcualte weights without going over
    while(RogueWeightQuery_IsOverSafeCapacity())
    {
        RogueMiscQuery_FilterByChance(RogueRandom(), QUERY_FUNC_INCLUDE, 66, 1);
    }

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    {
        u32 queryDuration = RogueDebug_SampleClock() - sRogueQuery.queryStartClock;
        DebugPrintf("[Query] End %d (duration: %d us)", sRogueQuery.queryType, RogueDebug_ClockToDisplayUnits(queryDuration));
        
        sRogueQuery.queryStartClock = RogueDebug_SampleClock();
        DebugPrintf("[Query] Weight Start %d", sRogueQuery.queryType);
    }
#endif
}

void RogueWeightQuery_End()
{
    ASSERT_WEIGHT_QUERY;
    sRogueQuery.weightArray = NULL;
    sRogueQuery.arrayCapacity = 0;
}

bool8 RogueWeightQuery_HasAnyWeights()
{
    ASSERT_WEIGHT_QUERY;
    return sRogueQuery.bitCount != 0 && sRogueQuery.totalWeight != 0;
}

bool8 RogueWeightQuery_HasMultipleWeights()
{
    ASSERT_WEIGHT_QUERY;
    return RogueWeightQuery_HasAnyWeights() && sRogueQuery.bitCount > 1;
}

void RogueWeightQuery_CalculateWeights(WeightCallback callback, void* data)
{
    u32 weight;
    u32 elem;
    u32 index;
    u32 counter = 0;
    u32 maxBitCount = Query_MaxBitCount();
    u32 weightCount = Query_GetWeightArrayCount();

    ASSERT_WEIGHT_QUERY;

    sRogueQuery.totalWeight = 0;

    for(elem = 0; elem < maxBitCount; ITERATOR_INC(elem))
    {
        if(GetQueryBitFlag(elem))
        {
            index = counter++;

            if(index < weightCount)
            {
                weight = callback(index, elem, data);

                sRogueQuery.weightArray[index] = weight;
                sRogueQuery.totalWeight += weight;
            }
        }
    }
}

void RogueWeightQuery_FillWeights(u8 weight)
{
    u32 weightCount = Query_GetWeightArrayCount();

    ASSERT_WEIGHT_QUERY;

    memset(sRogueQuery.weightArray, weight, weightCount);
    sRogueQuery.totalWeight = weight * weightCount;
}

static u16 RogueWeightQuery_SelectRandomFromWeightsInternal(u16 randValue, bool8 updateWeight, u8 newWeight)
{
    u32 weight;
    u32 elem;
    u32 index;
    u32 counter = 0;
    u32 maxBitCount = Query_MaxBitCount();
    u32 weightCount = Query_GetWeightArrayCount();
    u32 targetWeight = randValue % sRogueQuery.totalWeight;

    ASSERT_WEIGHT_QUERY;
    AGB_ASSERT(sRogueQuery.totalWeight != 0);

    for(elem = 0; elem < maxBitCount; ITERATOR_INC(elem))
    {
        if(GetQueryBitFlag(elem))
        {
            index = counter++;

            if(index < weightCount)
            {
                weight = sRogueQuery.weightArray[index];

                if(weight != 0)
                {
                    if(targetWeight < weight)
                    {
                        // We've found the target!
                        if(updateWeight)
                        {
                            // Remove old weight and replace with new one
                            sRogueQuery.totalWeight -= weight;
                            sRogueQuery.weightArray[index] = newWeight;
                            sRogueQuery.totalWeight += newWeight;
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
    AGB_ASSERT(sRogueQuery.arrayCapacity == 0);

    // Attempt to dynamically allocate memory for us to use, as we can't fit into the preallocation size
    if(sRogueQuery.bitCount + 1 >= ARRAY_COUNT(gRogueQueryBuffer))
    {
        sRogueQuery.arrayCapacity = sRogueQuery.bitCount + 1;
        sRogueQuery.listArray = Alloc(sizeof(u16) * sRogueQuery.arrayCapacity);
        sRogueQuery.dynamicAllocListArray = TRUE;

        if(sRogueQuery.listArray == NULL)
        {
            // Failed to allocate, so fallback and just have a smaller list
            AGB_ASSERT(FALSE);
            sRogueQuery.arrayCapacity = 0;
        }
    }

    // Use the preallocated array
    if(sRogueQuery.arrayCapacity == 0)
    {
        sRogueQuery.listArray = &gRogueQueryBuffer[0];
        sRogueQuery.arrayCapacity = ARRAY_COUNT(gRogueQueryBuffer);
        sRogueQuery.dynamicAllocListArray = FALSE;
    }
}

void RogueListQuery_End()
{
    ASSERT_LIST_QUERY;
    AGB_ASSERT(sRogueQuery.arrayCapacity != 0);

    // Free dynamically alloced array
    if(sRogueQuery.dynamicAllocListArray)
    {
        Free(sRogueQuery.listArray);
    }

    sRogueQuery.listArray = NULL;
    sRogueQuery.arrayCapacity = 0;
    sRogueQuery.dynamicAllocListArray = FALSE;
}

bool8 SortItemPlaceBefore(u8 sortMode, u16 itemIdA, u16 itemIdB, u16 quantityA, u16 quantityB);

static void SortInsertItem(u16 itemId, u16* buffer, u16 currBufferCount, u8 sortMode, bool8 flipSort)
{
    if(currBufferCount == 0)
    {
        // Insert remaining item at the end
        buffer[currBufferCount] = itemId;
    }
    else if(currBufferCount == 1)
    {
        if(SortItemPlaceBefore(sortMode, itemId, buffer[0], 1, 1) != flipSort)
        {
            buffer[currBufferCount] = buffer[0];
            buffer[0] = itemId;
        }
        else
        {
            buffer[currBufferCount] = itemId;
        }
    }
    else
    {
        u16 index = 0;
        u16 minIndex = 0;
        u16 maxIndex = currBufferCount - 1;

        // Insert sort, find the index to insert at
        while(minIndex != maxIndex)
        {
            AGB_ASSERT(minIndex < maxIndex);

            index = (maxIndex + minIndex) / 2;

            if(SortItemPlaceBefore(sortMode, itemId, buffer[index], 1, 1) != flipSort)
            {
                if(maxIndex == index)
                    --maxIndex;
                else
                    maxIndex = index;
            }
            else
            {
                if(minIndex == index)
                    ++minIndex;
                else
                    minIndex = index;
            }
        }

        AGB_ASSERT(minIndex == maxIndex);

        // Special case to sort the end of the list
        if(minIndex == currBufferCount - 1)
        {
            if(SortItemPlaceBefore(sortMode, itemId, buffer[currBufferCount - 1], 1, 1) != flipSort)
            {
                buffer[currBufferCount] = buffer[currBufferCount - 1];
                buffer[currBufferCount - 1] = itemId;
            }
            else
            {
                buffer[currBufferCount] = itemId;
            }
        }
        else
        {

            // Shift everything up
            for(index = currBufferCount; TRUE; --index)
            {
                buffer[index] = buffer[index - 1];

                if(index == minIndex + 1)
                    break;
            }

            buffer[minIndex] = itemId;
        }
    }
}

u16 const* RogueListQuery_CollapseItems(u8 sortMode, bool8 flipSort)
{
    u32 itemId;
    u32 index;
    ASSERT_ITEM_QUERY;
    ASSERT_LIST_QUERY;

    index = 0;

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ITERATOR_INC(itemId))
    {
        if(GetQueryBitFlag(itemId))
        {
            SortInsertItem(itemId, sRogueQuery.listArray, index, sortMode, flipSort);
            ++index;

            if(index >= sRogueQuery.arrayCapacity - 1)
            {
                // Hit capacity (Probably means putting too much into 1 shop)
                break;
            }
        }
    }

    // Terminate
    sRogueQuery.listArray[index] = ITEM_NONE;

    //if(sortMode < ITEM_SORT_MODE_COUNT)
    //{
    //    u16 i, j, temp;
    //    bool8 anySorts = FALSE;
    //
    //    for(j = 0; j < index; ++j)
    //    {
    //        anySorts = FALSE;
    //
    //        for(i = 1; i < index; ++i)
    //        {
    //            if(i == j)
    //                continue;
    //
    //            if(SortItemPlaceBefore(sortMode, sRogueQuery.listArray[i], sRogueQuery.listArray[i - 1], 1, 1) != flipSort)
    //            {
    //                SWAP(sRogueQuery.listArray[i], sRogueQuery.listArray[i - 1], temp);
    //                anySorts = TRUE;
    //            }
    //        }
    //
    //        if(!anySorts)
    //            break;
    //    }
    //}

    return sRogueQuery.listArray;
}

void RogueDebugQuery_FillPC(bool8 append)
{
#ifdef ROGUE_DEBUG
    u16 species;
    u16 writeIdx = append ? sRogueQuery.debugDumpCounter : 0;
    u16 queryTypeToRestore = sRogueQuery.queryType;

    AGB_ASSERT(sRogueQuery.queryType == QUERY_TYPE_MON || sRogueQuery.queryType == QUERY_TYPE_NONE);
    sRogueQuery.queryType = QUERY_TYPE_MON;

    for(species = SPECIES_NONE + 1; species < QUERY_NUM_SPECIES; ++species)
    {
        if(GetQueryBitFlag(species))
        {
            struct Pokemon mon;
            u16 currIdx = writeIdx++;
            u16 targetBox = currIdx / IN_BOX_COUNT;
            u16 boxIndex = currIdx % IN_BOX_COUNT;

            GetSetPokedexSpeciesFlag(species, FLAG_SET_SEEN);
            GetSetPokedexSpeciesFlag(species, FLAG_SET_CAUGHT);

            CreateMon(&mon, species, 1, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

            SetBoxMonAt(targetBox, boxIndex, &mon.box);
        }
    }

    // Leave 1 space between query results
    sRogueQuery.debugDumpCounter = writeIdx + 1;

    // Clear a row of spaces after so there is a clear indication of the end
    for(species = 0; species < 6; ++species)
    {
        u16 currIdx = writeIdx++;
        u16 targetBox = currIdx / IN_BOX_COUNT;
        u16 boxIndex = currIdx % IN_BOX_COUNT;
        ZeroBoxMonAt(targetBox, boxIndex);
    }

    sRogueQuery.queryType = queryTypeToRestore;
#endif
}

void RogueDebugQuery_FillBag()
{
#ifdef ROGUE_DEBUG
    u16 itemId;

    ASSERT_NO_QUERY;
    sRogueQuery.queryType = QUERY_TYPE_ITEM;

    ClearBag();

    for(itemId = ITEM_NONE + 1; itemId < QUERY_NUM_ITEMS; ++itemId)
    {
        if(GetQueryBitFlag(itemId))
        {
            if(!AddBagItem(itemId, 1))
                break;
        }
    }

    sRogueQuery.queryType = QUERY_TYPE_NONE;
#endif
}