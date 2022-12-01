#include "global.h"

#ifdef ROGUE_FEATURE_AUTOMATION
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_frontier.h"
#include "constants/event_objects.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/weather.h"
#include "data.h"

#include "battle_main.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "intro.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"

#include "rogue_automation.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"

#define COMM_BUFFER_SIZE 32

EWRAM_DATA u16 gAutoCommandCounter;
EWRAM_DATA u16 gAutoInputState;
EWRAM_DATA u16 gAutoCommBuffer[COMM_BUFFER_SIZE];
EWRAM_DATA u8 gAutoFlagBits[1 + AUTO_FLAG_COUNT / 8];


const struct RogueAutomationHeader gRogueAutomationHeader =
{
    .commBufferCapacity = COMM_BUFFER_SIZE,
    .commBuffer = gAutoCommBuffer,
};

void DoSpecialTrainerBattle(void);
void ApplyMonPreset(struct Pokemon* mon, u8 level, const struct RogueMonPreset* preset);
bool8 SelectNextPreset(u16 species, u16 trainerNum, u8 monIdx, u16 randFlag, struct RogueMonPreset* outPreset);

static void ProcessNextAutoCmd(u16 cmd, u16* args);
static void AutoCmd_ClearPlayerParty(u16* args);
static void AutoCmd_ClearEnemyParty(u16* args);
static void AutoCmd_SetPlayerMon(u16* args);
static void AutoCmd_SetEnemyMon(u16* args);
static void AutoCmd_SetPlayerMonData(u16* args);
static void AutoCmd_SetEnemyMonData(u16* args);
static void AutoCmd_GetPlayerMonData(u16* args);
static void AutoCmd_GetEnemyMonData(u16* args);
static void AutoCmd_StartTrainerBattle(u16* args);
static void AutoCmd_GetInputState(u16* args);
static void AutoCmd_GetNumSpecies(u16* args);
static void AutoCmd_ApplyRandomPlayerMonPreset(u16* args);
static void AutoCmd_ApplyRandomEnemyMonPreset(u16* args);
static void AutoCmd_GeneratePlayerParty(u16* args);
static void AutoCmd_GenerateEnemyParty(u16* args);
static void AutoCmd_SetRunDifficulty(u16* args);
static void AutoCmd_SetWeather(u16* args);
static void AutoCmd_SetRogueSeed(u16* args);
static void AutoCmd_SetFlag(u16* args);
static void AutoCmd_GetFlag(u16* args);
static void AutoCmd_SetVar(u16* args);
static void AutoCmd_GetVar(u16* args);
static void AutoCmd_GetMapLayoutID(u16* args);
static void AutoCmd_Warp(u16* args);
static void AutoCmd_WarpNextAdventureEncounter(u16* args);
static void AutoCmd_SetAutomationFlag(u16* args);
static void AutoCmd_GetAutomationFlag(u16* args);


u16 Rogue_AutomationBufferSize(void)
{
    return COMM_BUFFER_SIZE;
}

u16 Rogue_ReadAutomationBuffer(u16 offset)
{
    return gAutoCommBuffer[offset];
}

bool8 Rogue_AutomationGetFlag(u16 flag)
{
    u16 idx = flag / 8;
    u16 bit = flag % 8;

    u8 bitMask = 1 << bit;
    return gAutoFlagBits[idx] & bitMask;
}

void Rogue_AutomationSetFlag(u16 flag, bool8 state)
{
    u16 idx = flag / 8;
    u16 bit = flag % 8;

    u8 bitMask = 1 << bit;
    if(state)
    {
        gAutoFlagBits[idx] |= bitMask;
    }
    else
    {
        gAutoFlagBits[idx] &= ~bitMask;
    }
}

void Rogue_WriteAutomationBuffer(u16 offset, u16 value)
{
    gAutoCommBuffer[offset] = value;
}

void Rogue_AutomationInit(void)
{
    gAutoCommandCounter = 0;
    gAutoCommBuffer[0] = gAutoCommandCounter;

    gAutoInputState = AUTO_INPUT_STATE_TITLE_MENU;

    Rogue_AutomationSetFlag(AUTO_FLAG_TRAINER_FORCE_COMP_MOVESETS, FALSE);
    Rogue_AutomationSetFlag(AUTO_FLAG_TRAINER_DISABLE_PARTY_GENERATION, FALSE);
    Rogue_AutomationSetFlag(AUTO_FLAG_TRAINER_RANDOM_AI, FALSE);
    Rogue_AutomationSetFlag(AUTO_FLAG_PLAYER_AUTO_PICK_MOVES, TRUE);
    Rogue_AutomationSetFlag(AUTO_FLAG_TRAINER_LVL_5, FALSE);
}

void Rogue_AutomationCallback(void)
{
    u16 counterId = gAutoCommBuffer[0];

    // Received a command if the counter missmatches
    if(gAutoCommandCounter != counterId)
    {
        u16 cmd = gAutoCommBuffer[1];
        u16* args = &gAutoCommBuffer[2];
        ProcessNextAutoCmd(cmd, args);

        // Indicate that we've finished this command
        gAutoCommandCounter = counterId + 1;
        gAutoCommBuffer[0] = gAutoCommandCounter;
    }
}

void Rogue_PushAutomationInputState(u16 state)
{
    gAutoInputState = state;
}

// Auto Commands
//
static void ProcessNextAutoCmd(u16 cmd, u16* args)
{
    switch(cmd)
    {
        case 0: AutoCmd_ClearPlayerParty(args); break;
        case 1: AutoCmd_ClearEnemyParty(args); break;
        case 2: AutoCmd_SetPlayerMon(args); break;
        case 3: AutoCmd_SetEnemyMon(args); break;
        case 4: AutoCmd_SetPlayerMonData(args); break;
        case 5: AutoCmd_SetEnemyMonData(args); break;
        case 6: AutoCmd_GetPlayerMonData(args); break;
        case 7: AutoCmd_GetEnemyMonData(args); break;
        case 8: AutoCmd_StartTrainerBattle(args); break;
        case 9: AutoCmd_GetInputState(args); break;
        case 10: AutoCmd_GetNumSpecies(args); break;
        case 11: AutoCmd_ApplyRandomPlayerMonPreset(args); break;
        case 12: AutoCmd_ApplyRandomEnemyMonPreset(args); break;
        case 13: AutoCmd_GeneratePlayerParty(args); break;
        case 14: AutoCmd_GenerateEnemyParty(args); break;
        case 15: AutoCmd_SetRunDifficulty(args); break;
        case 16: AutoCmd_SetWeather(args); break;
        case 17: AutoCmd_SetRogueSeed(args); break;
        case 18: AutoCmd_SetFlag(args); break;
        case 19: AutoCmd_GetFlag(args); break;
        case 20: AutoCmd_SetVar(args); break;
        case 21: AutoCmd_GetVar(args); break;
        case 22: AutoCmd_GetMapLayoutID(args); break;
        case 23: AutoCmd_Warp(args); break;
        case 24: AutoCmd_WarpNextAdventureEncounter(args); break;
        case 25: AutoCmd_SetAutomationFlag(args); break;
        case 26: AutoCmd_GetAutomationFlag(args); break;
    }
}

static void AutoCmd_ClearPlayerParty(u16* args)
{
    u16 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        ZeroMonData(&gPlayerParty[i]);
    }

    CalculatePlayerPartyCount();
}

static void AutoCmd_ClearEnemyParty(u16* args)
{
    u16 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        ZeroMonData(&gEnemyParty[i]);
    }

    CalculateEnemyPartyCount();
}

static void AutoCmd_SetPlayerMon(u16* args)
{
    u16 index = args[0];
    u16 species = args[1];
    u16 level = args[2];
    u16 fixedIV = args[3];

    CreateMon(&gPlayerParty[index], species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    CalculatePlayerPartyCount();
}

static void AutoCmd_SetEnemyMon(u16* args)
{
    u16 index = args[0];
    u16 species = args[1];
    u16 level = args[2];
    u16 fixedIV = args[3];

    CreateMon(&gEnemyParty[index], species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    CalculateEnemyPartyCount();
}

static void AutoCmd_SetPlayerMonData(u16* args)
{
    u16 index = args[0];
    u16 data = args[1];
    u32 value = args[2];

    SetMonData(&gPlayerParty[index], data, &value);
}

static void AutoCmd_SetEnemyMonData(u16* args)
{
    u16 index = args[0];
    u16 data = args[1];
    u32 value = args[2];

    SetMonData(&gEnemyParty[index], data, &value);
}

static void AutoCmd_GetPlayerMonData(u16* args)
{
    u16 index = args[0];
    u16 data = args[1];

    args[0] = GetMonData(&gPlayerParty[index], data);
}

static void AutoCmd_GetEnemyMonData(u16* args)
{
    u16 index = args[0];
    u16 data = args[1];

    args[0] = GetMonData(&gEnemyParty[index], data);
}

static void AutoCmd_StartTrainerBattle(u16* args)
{
    gSpecialVar_0x8004 = SPECIAL_BATTLE_AUTOMATION;
    DoSpecialTrainerBattle();
}

static void AutoCmd_GetInputState(u16* args)
{
    args[0] = gAutoInputState;
}

static void AutoCmd_GetNumSpecies(u16* args)
{
    args[0] = NUM_SPECIES;
}

static void ApplyRandomMonPreset(struct Pokemon* party, u8 monIdx)
{
    struct RogueMonPreset preset;
    u16 species = GetMonData(&party[monIdx], MON_DATA_SPECIES);
    u16 level = GetMonData(&party[monIdx], MON_DATA_LEVEL);

    if(SelectNextPreset(species, 0, monIdx, 0, &preset))
        ApplyMonPreset(&party[monIdx], level, &preset);
}

static void AutoCmd_ApplyRandomPlayerMonPreset(u16* args)
{
    ApplyRandomMonPreset(&gPlayerParty[0], args[0]);
}

static void AutoCmd_ApplyRandomEnemyMonPreset(u16* args)
{
    ApplyRandomMonPreset(&gEnemyParty[0], args[0]);
}

static void GenerateTrainerParty(u16* args, struct Pokemon * party)
{
    u16 trainerNum = args[0];
    bool8 success;
    u8 monCount;
    Rogue_PreCreateTrainerParty(trainerNum, &success, &monCount);

    if(success)
    {
        u16 i;
        monCount = args[1]; // Override mon coount

        for(i = 0; i < monCount; ++i)
            Rogue_CreateTrainerMon(trainerNum, party, i, monCount);

        Rogue_PostCreateTrainerParty(trainerNum, party, monCount);
    }
}

static void AutoCmd_GeneratePlayerParty(u16* args)
{
    GenerateTrainerParty(args, &gPlayerParty[0]);
    CalculatePlayerPartyCount();
}

static void AutoCmd_GenerateEnemyParty(u16* args)
{
    GenerateTrainerParty(args, &gEnemyParty[0]);
    CalculateEnemyPartyCount();
}

static void AutoCmd_SetRunDifficulty(u16* args)
{
    gRogueRun.currentDifficulty = args[0];
}

static void AutoCmd_SetWeather(u16* args)
{
    u16 weather = args[0];
    
    SetSavedWeather(weather);
    DoCurrentWeather();
}

static void AutoCmd_SetRogueSeed(u16* args)
{
    gSaveBlock1Ptr->dewfordTrends[0].words[0] = args[0];
    gSaveBlock1Ptr->dewfordTrends[0].words[1] = args[1];
}

static void AutoCmd_SetFlag(u16* args)
{
    u16 flag = args[0];
    u16 state = args[1];

    if(state)
        FlagSet(flag);
    else
        FlagClear(flag);
}

static void AutoCmd_GetFlag(u16* args)
{
    u16 flag = args[0];
    args[0] = FlagGet(flag);
}

static void AutoCmd_SetVar(u16* args)
{
    u16 var = args[0];
    u16 value = args[1];

    VarSet(var, value);
}

static void AutoCmd_GetVar(u16* args)
{
    u16 var = args[0];
    args[0] = VarGet(var);
}

static void AutoCmd_GetMapLayoutID(u16* args)
{
    args[0] = gMapHeader.mapLayoutId;
}

static void AutoCmd_Warp(u16* args)
{
    struct WarpData warp;
    warp.mapGroup = args[0];
    warp.mapNum = args[1];
    warp.warpId = args[2];
    warp.x = args[3];
    warp.y = args[4];

    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
}

static void AutoCmd_WarpNextAdventureEncounter(u16* args)
{
    RogueAdv_DebugExecuteRandomNextNode();
}

static void AutoCmd_SetAutomationFlag(u16* args)
{
    u16 flag = args[0];
    u16 state = args[1];

    Rogue_AutomationSetFlag(flag, state ? 1 : 0);
}

static void AutoCmd_GetAutomationFlag(u16* args)
{
    u16 flag = args[0];
    args[0] = Rogue_AutomationGetFlag(flag);
}

#endif