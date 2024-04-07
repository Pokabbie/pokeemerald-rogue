#include "global.h"
#include "pokemon.h"

#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"
#include "rogue_query_script.h"

#define DEFAULT_FAVOUR 100

typedef void (*ScriptCallback)(struct QueryScriptContext*);

static void Condition_Always(struct QueryScriptContext* context);
static void Condition_Never(struct QueryScriptContext* context);
static void Condition_Not(struct QueryScriptContext* context);
static void Condition_GreaterThan(struct QueryScriptContext* context);
static void Condition_LessThan(struct QueryScriptContext* context);
static void Condition_EqualTo(struct QueryScriptContext* context);
static void Condition_NotEqualTo(struct QueryScriptContext* context);
static void Condition_HasType(struct QueryScriptContext* context);
static void Condition_IsMonoType(struct QueryScriptContext* context);
static void Condition_IsLegendary(struct QueryScriptContext* context);
static void Condition_IsBoxLegendary(struct QueryScriptContext* context);
static void Condition_IsNonBoxLegendary(struct QueryScriptContext* context);
static void Condition_IsBannedSpecies(struct QueryScriptContext* context);
static void Condition_HasUniqueTypeInTeam(struct QueryScriptContext* context);
static void Condition_AlreadyHasTypeInTeam(struct QueryScriptContext* context);

static void Action_IncludeTypes(struct QueryScriptContext* context);
static void Action_ExcludeTypes(struct QueryScriptContext* context);
static void Action_ForceIncludeSpecies(struct QueryScriptContext* context);
static void Action_ForceIncludeSpeciesList(struct QueryScriptContext* context);
static void Action_TryIncludeSpecies(struct QueryScriptContext* context);
static void Action_TryIncludeSpeciesList(struct QueryScriptContext* context);
static void Action_IncFavour(struct QueryScriptContext* context);
static void Action_DecFavour(struct QueryScriptContext* context);
static void Action_ImpossibleFavour(struct QueryScriptContext* context);

static ScriptCallback const gScriptTable[] = 
{
    [QUERY_SCRIPT_ALWAYS] = Condition_Always,
    [QUERY_SCRIPT_NEVER] = Condition_Never,
    [QUERY_SCRIPT_NOT] = Condition_Not,
    [QUERY_SCRIPT_GREATER_THAN] = Condition_GreaterThan,
    [QUERY_SCRIPT_LESS_THAN] = Condition_LessThan,
    [QUERY_SCRIPT_EQUAL_TO] = Condition_EqualTo,
    [QUERY_SCRIPT_NOT_EQUAL_TO] = Condition_NotEqualTo,
    [QUERY_SCRIPT_HAS_TYPE] = Condition_HasType,
    [QUERY_SCRIPT_IS_MONO_TYPE] = Condition_IsMonoType,
    [QUERY_SCRIPT_IS_LEGENDARY] = Condition_IsLegendary,
    [QUERY_SCRIPT_IS_BOX_LEGENDARY] = Condition_IsBoxLegendary,
    [QUERY_SCRIPT_IS_NON_BOX_LEGENDARY] = Condition_IsNonBoxLegendary,
    [QUERY_SCRIPT_IS_BANNED_SPECIES] = Condition_IsBannedSpecies,
    [QUERY_SCRIPT_HAS_UNIQUE_TYPE_IN_TEAM] = Condition_HasUniqueTypeInTeam,
    [QUERY_SCRIPT_ALREADY_HAS_TYPE_IN_TEAM] = Condition_AlreadyHasTypeInTeam,

    [QUERY_SCRIPT_INCLUDE_TYPES] = Action_IncludeTypes,
    [QUERY_SCRIPT_EXCLUDE_TYPES] = Action_ExcludeTypes,
    [QUERY_SCRIPT_FORCE_INCLUDE_SPECIES] = Action_ForceIncludeSpecies,
    [QUERY_SCRIPT_FORCE_INCLUDE_SPECIES_LIST] = Action_ForceIncludeSpeciesList,
    [QUERY_SCRIPT_TRY_INCLUDE_SPECIES] = Action_TryIncludeSpecies,
    [QUERY_SCRIPT_TRY_INCLUDE_SPECIES_LIST] = Action_TryIncludeSpeciesList,

    [QUERY_SCRIPT_INC_FAVOUR] = Action_IncFavour,
    [QUERY_SCRIPT_DEC_FAVOUR] = Action_DecFavour,
    [QUERY_SCRIPT_IMPOSSIBLE_FAVOUR] = Action_ImpossibleFavour,
};

void RogueQueryScript_SetupScript(struct QueryScriptContext* context, u16 const* script)
{
    context->script = script;
    context->ptr = NULL;
}

void RogueQueryScript_SetupVarsForSpecies(struct QueryScriptContext* context, u16 species)
{
    context->currentSpecies = species;
}

void RogueQueryScript_SetupVarsForParty(struct QueryScriptContext* context, struct Pokemon* party, u8 count, bool8 includeTypeCoverage, u8 maxBoxLegends, u8 maxNonBoxLegends)
{
    u8 i;
    u16 species;
    u8 boxLegendaryCount = 0;
    u8 nonBoxLegendaryCount = 0;

    context->partyTypeFlags = 0;

    for(i = 0; i < count; ++i)
    {
        species = GetMonData(&party[i], MON_DATA_SPECIES);
        if(species != SPECIES_NONE)
        {
            if(includeTypeCoverage)
                Rogue_AppendSpeciesTypeFlags(species, &context->partyTypeFlags);

            if(RoguePokedex_IsSpeciesLegendary(species))
            {
                if(RoguePokedex_IsSpeciesValidBoxLegendary(species))
                    ++boxLegendaryCount;
                else
                    ++nonBoxLegendaryCount;
            }
        }
    }

    context->allowBoxLegendaries = (boxLegendaryCount < maxBoxLegends);
    context->allowNonBoxLegendaries = (nonBoxLegendaryCount < maxNonBoxLegends);
}

void RogueQueryScript_Execute(struct QueryScriptContext* context)
{
    u16 cmd;

    context->ptr = context->script;
    
    // Default vars
    context->conditionState = TRUE;

    while(*context->ptr != QUERY_SCRIPT_END)
    {
        cmd = *context->ptr; 
        ++context->ptr;

        AGB_ASSERT(gScriptTable[cmd] != NULL);
        gScriptTable[cmd](context);
    }
}

u8 RogueQueryScript_CalculateWeightsCallback(u16 index, u16 species, void* data)
{
    struct QueryScriptContext* context = (struct QueryScriptContext*)data;

    RogueQueryScript_SetupVarsForSpecies(context, species);
    context->currentFavour = DEFAULT_FAVOUR;

    RogueQueryScript_Execute(context);

    if(context->currentFavour == 0)
    {
        return 0;
    }
    else
    {
        // Favour is measuring powers of 2
        u8 weight;
        s16 signedValue;

        // Start with neutral weight as 8, so can decrease weight by 3 or increase by 4
        signedValue = (s16)context->currentFavour - DEFAULT_FAVOUR + 3;
        signedValue = min(8, max(0, signedValue));

        weight = (1 << (u8)(signedValue));
        return weight;
    }
}

static u16 ParseScriptValue(struct QueryScriptContext* context)
{
    u16 value = *context->ptr;
    ++context->ptr;

    // If we have the final bit set, it means we want to do a var lookup
    if((value & QUERY_MASK_VAR_LOOKUP) != 0)
    {
        value = value & ~QUERY_MASK_VAR_LOOKUP;

        switch (value)
        {
        case QUERY_VAR_MON_SPECIES:
            return context->currentSpecies;
        case QUERY_VAR_MON_EGG_SPECIES:
            return Rogue_GetEggSpecies(context->currentSpecies);

        case QUERY_VAR_MON_GENERATION:
            return SpeciesToGen(context->currentSpecies);

        case QUERY_VAR_MON_TYPE1:
            return RoguePokedex_GetSpeciesType(context->currentSpecies, 0);
        case QUERY_VAR_MON_TYPE2:
            return RoguePokedex_GetSpeciesType(context->currentSpecies, 1);

        case QUERY_VAR_MON_BST:
            return RoguePokedex_GetSpeciesBST(context->currentSpecies);

        case QUERY_VAR_MON_HP:
            return gRogueSpeciesInfo[context->currentSpecies].baseHP;

        case QUERY_VAR_MON_ATK:
            return gRogueSpeciesInfo[context->currentSpecies].baseAttack;
        case QUERY_VAR_MON_DEF:
            return gRogueSpeciesInfo[context->currentSpecies].baseDefense;

        case QUERY_VAR_MON_SPATK:
            return gRogueSpeciesInfo[context->currentSpecies].baseSpAttack;
        case QUERY_VAR_MON_SPDEF:
            return gRogueSpeciesInfo[context->currentSpecies].baseSpDefense;

        case QUERY_VAR_MON_SPEED:
            return gRogueSpeciesInfo[context->currentSpecies].baseSpeed;

        case QUERY_VAR_MON_BEST_STAT:
            return RoguePokedex_GetSpeciesBestStat(context->currentSpecies);
        case QUERY_VAR_MON_WORST_STAT:
            return RoguePokedex_GetSpeciesWorstStat(context->currentSpecies);
        
        default:
            // Invalid variable
            AGB_ASSERT(FALSE);
            return 0;
        }
    }

    return value;
}

static u32 ParseMonTypeFlags(struct QueryScriptContext* context)
{
    u32 typeFlags = 0;

    while(*context->ptr != TYPE_NONE)
    {
        typeFlags |= MON_TYPE_VAL_TO_FLAGS(*context->ptr);
        ++context->ptr;
    }

    ++context->ptr;
    return typeFlags;
}

static void Condition_Always(struct QueryScriptContext* context)
{
    context->conditionState = TRUE;
}

static void Condition_Never(struct QueryScriptContext* context)
{
    context->conditionState = FALSE;
}

static void Condition_Not(struct QueryScriptContext* context)
{
    context->conditionState = !context->conditionState;
}

static void Condition_GreaterThan(struct QueryScriptContext* context)
{
    u16 valueA = ParseScriptValue(context);
    u16 valueB = ParseScriptValue(context);

    context->conditionState = valueA > valueB;
}

static void Condition_LessThan(struct QueryScriptContext* context)
{
    u16 valueA = ParseScriptValue(context);
    u16 valueB = ParseScriptValue(context);

    context->conditionState = valueA < valueB;
}

static void Condition_EqualTo(struct QueryScriptContext* context)
{
    u16 valueA = ParseScriptValue(context);
    u16 valueB = ParseScriptValue(context);

    context->conditionState = valueA == valueB;
}

static void Condition_NotEqualTo(struct QueryScriptContext* context)
{
    u16 valueA = ParseScriptValue(context);
    u16 valueB = ParseScriptValue(context);

    context->conditionState = valueA != valueB;
}

static void Condition_HasType(struct QueryScriptContext* context)
{
    u16 typeToCheck = ParseScriptValue(context);

    context->conditionState = ( RoguePokedex_GetSpeciesType(context->currentSpecies, 0) == typeToCheck || RoguePokedex_GetSpeciesType(context->currentSpecies, 1) == typeToCheck);
}

static void Condition_IsMonoType(struct QueryScriptContext* context)
{
    context->conditionState = (RoguePokedex_GetSpeciesType(context->currentSpecies, 0) == RoguePokedex_GetSpeciesType(context->currentSpecies, 1));
}

static void Condition_IsLegendary(struct QueryScriptContext* context)
{
    context->conditionState = RoguePokedex_IsSpeciesLegendary(context->currentSpecies);
}

static void Condition_IsBoxLegendary(struct QueryScriptContext* context)
{
    context->conditionState = RoguePokedex_IsSpeciesValidBoxLegendary(context->currentSpecies);
}

static void Condition_IsNonBoxLegendary(struct QueryScriptContext* context)
{
    context->conditionState = RoguePokedex_IsSpeciesLegendary(context->currentSpecies) && !RoguePokedex_IsSpeciesValidBoxLegendary(context->currentSpecies);
}

static void Condition_IsBannedSpecies(struct QueryScriptContext* context)
{
    context->conditionState = FALSE;

    if(RoguePokedex_IsSpeciesLegendary(context->currentSpecies))
    {
        if(RoguePokedex_IsSpeciesValidBoxLegendary(context->currentSpecies))
            context->conditionState = !context->allowBoxLegendaries;
        else
            context->conditionState = !context->allowNonBoxLegendaries;
    }
}

static void Condition_HasUniqueTypeInTeam(struct QueryScriptContext* context)
{
    context->conditionState = 
        ((context->partyTypeFlags & MON_TYPE_VAL_TO_FLAGS(RoguePokedex_GetSpeciesType(context->currentSpecies, 0))) == 0) ||
        ((context->partyTypeFlags & MON_TYPE_VAL_TO_FLAGS(RoguePokedex_GetSpeciesType(context->currentSpecies, 1))) == 0);
}

static void Condition_AlreadyHasTypeInTeam(struct QueryScriptContext* context)
{
    context->conditionState = 
        ((context->partyTypeFlags & MON_TYPE_VAL_TO_FLAGS(RoguePokedex_GetSpeciesType(context->currentSpecies, 0))) != 0) ||
        ((context->partyTypeFlags & MON_TYPE_VAL_TO_FLAGS(RoguePokedex_GetSpeciesType(context->currentSpecies, 1))) != 0);
}

static void Action_IncludeTypes(struct QueryScriptContext* context)
{
    u32 typeFlags = ParseMonTypeFlags(context);

    if(context->conditionState)
    {
        RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, typeFlags);
    }
}

static void Action_ExcludeTypes(struct QueryScriptContext* context)
{
    u32 typeFlags = ParseMonTypeFlags(context);

    if(context->conditionState)
    {
        RogueMonQuery_IsOfType(QUERY_FUNC_EXCLUDE, typeFlags);
    }
}

static void Action_ForceIncludeSpecies(struct QueryScriptContext* context)
{
    u16 species = ParseScriptValue(context);

    if(context->conditionState)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, species);
    }
}

static void Action_ForceIncludeSpeciesList(struct QueryScriptContext* context)
{
    u16 species;

    do
    {
        species = ParseScriptValue(context);

        if(context->conditionState && species != SPECIES_NONE)
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, species);
        }
    } 
    while (species != SPECIES_NONE);
}


static void Action_TryIncludeSpecies(struct QueryScriptContext* context)
{
    u16 species = ParseScriptValue(context);

    if(context->conditionState && Query_IsSpeciesEnabled(species))
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, species);
    }
}

static void Action_TryIncludeSpeciesList(struct QueryScriptContext* context)
{
    u16 species;

    do
    {
        species = ParseScriptValue(context);

        if(context->conditionState && species != SPECIES_NONE && Query_IsSpeciesEnabled(species))
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, species);
        }
    } 
    while (species != SPECIES_NONE);
}

static void Action_IncFavour(struct QueryScriptContext* context)
{
    if(context->conditionState && context->currentFavour != 0)
    {
        ++context->currentFavour;
    }
}

static void Action_DecFavour(struct QueryScriptContext* context)
{
    if(context->conditionState && context->currentFavour != 0)
    {
        --context->currentFavour;
    }
}

static void Action_ImpossibleFavour(struct QueryScriptContext* context)
{
    if(context->conditionState && context->currentFavour != 0)
    {
        context->currentFavour = 0;
    }
}