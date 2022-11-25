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

#include "event_data.h"
#include "pokemon.h"

//#include "item.h"
//#include "random.h"

//#include "rogue_controller.h"

#define COMM_BUFFER_SIZE 32

EWRAM_DATA u16 gAutoCommandCounter;
EWRAM_DATA u16 gAutoCommBuffer[COMM_BUFFER_SIZE];


const struct RogueAutomationHeader gRogueAutomationHeader =
{
    .commBufferCapacity = COMM_BUFFER_SIZE,
    .commBuffer = gAutoCommBuffer,
};

static void ProcessNextAutoCmd(u16 cmd, u16* args);
static void AutoCmd_ClearPlayerParty(u16* args);
static void AutoCmd_ClearEnemyParty(u16* args);
static void AutoCmd_SetPlayerMon(u16* args);
static void AutoCmd_SetEnemyMon(u16* args);
static void AutoCmd_SetPlayerMonData(u16* args);
static void AutoCmd_SetEnemyMonData(u16* args);
static void AutoCmd_StartTrainerBattle(u16* args);


u16 Rogue_AutomationBufferSize(void)
{
    return COMM_BUFFER_SIZE;
}

u16 Rogue_ReadAutomationBuffer(u16 offset)
{
    return gAutoCommBuffer[offset];
}

void Rogue_WriteAutomationBuffer(u16 offset, u16 value)
{
    gAutoCommBuffer[offset] = value;
}

void Rogue_AutomationInit(void)
{
    gAutoCommandCounter = 0;
    gAutoCommBuffer[0] = gAutoCommandCounter;
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

bool8 Rogue_AutomationSkipTrainerPartyCreate(void)
{
    return TRUE;
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
        case 6: AutoCmd_StartTrainerBattle(args); break;
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

void DoSpecialTrainerBattle(void);

static void AutoCmd_StartTrainerBattle(u16* args)
{
    gSpecialVar_0x8004 = SPECIAL_BATTLE_AUTOMATION;
    DoSpecialTrainerBattle();
}

#endif