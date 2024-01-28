#include "global.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/region_map_sections.h"

#include "rogue_baked.h"
#include "rogue_gifts.h"

struct CustomTrainerData
{
    u8 const* name;
    u32 trainerId;
    u8 trainerColour;
};

struct CustomMonData
{
    u8 const* nickname;
    u16 const* moves;
    u16 const* abilities;
    u32 otId;
    u16 movesCount;
    u16 customTrainerId;
    u16 species;
    u16 pokeball;
    u16 heldItem;
    u16 isShiny : 1;
};

#include "data/rogue/custom_mons.h"

u16 RogueGift_GetCustomMonId(struct Pokemon* mon)
{
    return RogueGift_GetCustomMonIdBySpecies(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_OT_ID));
}

u16 RogueGift_GetCustomMonIdBySpecies(u16 species, u32 otId)
{
    u16 i;

    species = Rogue_GetEggSpecies(species);

    for(i = CUSTOM_MON_NONE + 1; i < CUSTOM_MON_COUNT; ++i)
    {
        struct CustomMonData const* monData = &sCustomPokemon[i];
        
        if(monData->otId == otId && Rogue_GetEggSpecies(monData->species) == species)
            return i;
    }

    return CUSTOM_MON_NONE;
}

u16 const* RogueGift_GetCustomMonMoves(u16 id)
{
    AGB_ASSERT(id < CUSTOM_MON_COUNT);

    if(id != CUSTOM_MON_NONE)
    {
        struct CustomMonData const* monData = &sCustomPokemon[id];
        return monData->movesCount != 0 ? monData->moves : NULL;
    }

    return NULL;
}

u16 RogueGift_GetCustomMonMoveCount(u16 id)
{
    AGB_ASSERT(id < CUSTOM_MON_COUNT);

    if(id != CUSTOM_MON_NONE)
    {
        struct CustomMonData const* monData = &sCustomPokemon[id];
        return monData->movesCount;
    }

    return 0;
}

u16 const* RogueGift_GetCustomMonAbilites(u16 id)
{
    AGB_ASSERT(id < CUSTOM_MON_COUNT);

    if(id != CUSTOM_MON_NONE)
    {
        struct CustomMonData const* monData = &sCustomPokemon[id];
        return monData->abilities;
    }

    return NULL;
}

void RogueGift_CreateMon(u16 customMonId, struct Pokemon* mon, u8 level, u8 fixedIV)
{
    struct CustomMonData const* monData = &sCustomPokemon[customMonId]; 
    struct CustomTrainerData const* trainerData = &sCustomTrainers[monData->customTrainerId];
    u32 temp = 0;

    AGB_ASSERT(customMonId < CUSTOM_MON_COUNT);

    ZeroMonData(mon);
    CreateMon(mon, monData->species, level, fixedIV, 0, 0, OT_ID_PRESET, trainerData->trainerId);

    // Met location
    temp = METLOC_FATEFUL_ENCOUNTER;
    SetMonData(mon, MON_DATA_MET_LOCATION, &temp);

    // Update nickname
    if(monData->nickname != NULL)
    {
        SetMonData(mon, MON_DATA_NICKNAME, monData->nickname);
    }

    // Update OT name
    SetMonData(mon, MON_DATA_OT_NAME, trainerData->name);

    temp = trainerData->trainerColour;
    SetMonData(mon, MON_DATA_OT_GENDER, &temp);

    // Assign pokeball
    temp = monData->pokeball;
    SetMonData(mon, MON_DATA_POKEBALL, &temp);

    // Set shiny state
    temp = monData->isShiny ? 1 : 0;
    SetMonData(mon, MON_DATA_IS_SHINY, &temp);

    // Default moveset is first 4 moves from custom moveset
    if(monData->movesCount != 0)
    {
        u8 m, j;
        u16 moves[MAX_MON_MOVES];

        for(m = 0; m < MAX_MON_MOVES && m < monData->movesCount; ++m)
        {
            moves[m] = monData->moves[m];
        }

        // Fill the rest of the moves with default moves
        for(j = 0; m < MAX_MON_MOVES; ++m, ++j)
        {
            moves[m] = GetMonData(mon, MON_DATA_MOVE1 + j);
        }

        // Give back moves
        for(m = 0; m < MAX_MON_MOVES; ++m)
        {
            temp = moves[m];
            SetMonData(mon, MON_DATA_MOVE1 + m, &temp);
            SetMonData(mon, MON_DATA_PP1 + m, &gBattleMoves[temp].pp);
        }
    }

    // Default custom ability
    if(monData->abilities != NULL)
    {
        // If we have a custom ability, always assign abilityNum 0
        temp = 0;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &temp);
    }
}