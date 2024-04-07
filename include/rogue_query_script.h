#ifndef ROGUE_QUERY_SCRIPTS_H
#define ROGUE_QUERY_SCRIPTS_H

enum
{
    // Global
    QUERY_SCRIPT_END,

    // Conditions
    //

    // Global
    QUERY_SCRIPT_ALWAYS,
    QUERY_SCRIPT_NEVER,
    QUERY_SCRIPT_NOT,
    QUERY_SCRIPT_ELSE = QUERY_SCRIPT_NOT, // functionally identical
    
    // Weight methods
    //
    QUERY_SCRIPT_GREATER_THAN,
    QUERY_SCRIPT_LESS_THAN,
    QUERY_SCRIPT_EQUAL_TO,
    QUERY_SCRIPT_NOT_EQUAL_TO,

    QUERY_SCRIPT_HAS_TYPE,
    QUERY_SCRIPT_IS_MONO_TYPE,
    QUERY_SCRIPT_IS_LEGENDARY,
    QUERY_SCRIPT_IS_BOX_LEGENDARY,
    QUERY_SCRIPT_IS_NON_BOX_LEGENDARY,
    QUERY_SCRIPT_IS_BANNED_SPECIES,
    QUERY_SCRIPT_HAS_UNIQUE_TYPE_IN_TEAM,
    QUERY_SCRIPT_ALREADY_HAS_TYPE_IN_TEAM,

    // Actions
    //

    // Query methods
    //
    QUERY_SCRIPT_INCLUDE_TYPES,
    QUERY_SCRIPT_EXCLUDE_TYPES,
    QUERY_SCRIPT_FORCE_INCLUDE_SPECIES,
    QUERY_SCRIPT_FORCE_INCLUDE_SPECIES_LIST,
    QUERY_SCRIPT_TRY_INCLUDE_SPECIES,
    QUERY_SCRIPT_TRY_INCLUDE_SPECIES_LIST,

    // Weight methods
    //
    QUERY_SCRIPT_INC_FAVOUR,
    QUERY_SCRIPT_DEC_FAVOUR,
    QUERY_SCRIPT_IMPOSSIBLE_FAVOUR,
    // TODO - Support custom inc/dec if needed
};


// Indicate that this is variable lookup, otherwise it is assumed to be a value
#define QUERY_MASK_VAR_LOOKUP 0x8000

enum
{
    QUERY_VAR_MON_SPECIES,
    QUERY_VAR_MON_EGG_SPECIES,
    QUERY_VAR_MON_GENERATION,
    QUERY_VAR_MON_TYPE1,
    QUERY_VAR_MON_TYPE2,
    
    QUERY_VAR_MON_BST,
    QUERY_VAR_MON_HP,
    QUERY_VAR_MON_ATK,
    QUERY_VAR_MON_DEF,
    QUERY_VAR_MON_SPATK,
    QUERY_VAR_MON_SPDEF,
    QUERY_VAR_MON_SPEED,

    QUERY_VAR_MON_BEST_STAT,
    QUERY_VAR_MON_WORST_STAT,
    
    QUERY_VAR_COUNT
};

struct QueryScriptContext
{
    u16 const* script;
    u16 const* ptr;
    u32 partyTypeFlags;
    u16 currentSpecies;
    bool8 allowBoxLegendaries;
    bool8 allowNonBoxLegendaries;
    bool8 conditionState;
    u8 currentFavour;
};

void RogueQueryScript_SetupScript(struct QueryScriptContext* context, u16 const* script);
void RogueQueryScript_SetupVarsForSpecies(struct QueryScriptContext* context, u16 species);
void RogueQueryScript_SetupVarsForParty(struct QueryScriptContext* context, struct Pokemon* party, u8 count, bool8 includeTypeCoverage, u8 maxBoxLegends, u8 maxNonBoxLegends);

void RogueQueryScript_Execute(struct QueryScriptContext* context);

u8 RogueQueryScript_CalculateWeightsCallback(u16 index, u16 species, void* data);

#endif