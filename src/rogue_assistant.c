#include "global.h"

#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_frontier.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/weather.h"
#include "gba/isagbprint.h"
#include "data.h"

#include "battle_main.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "follow_me.h"
#include "intro.h"
#include "item.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "script.h"
#include "task.h"

#include "rogue_assistant.h"
#include "rogue_adventurepaths.h"
#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_multiplayer.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_query.h"
#include "rogue_settings.h"

enum 
{
    GAME_CONSTANT_ASSISTANT_STATE_NUM_ADDRESS,
    GAME_CONSTANT_ASSISTANT_SUBSTATE_NUM_ADDRESS,
    GAME_CONSTANT_REQUEST_STATE_NUM_ADDRESS,

    GAME_CONSTANT_SAVE_BLOCK1_PTR_ADDRESS,
    GAME_CONSTANT_SAVE_BLOCK2_PTR_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_CAPACITY,
    GAME_CONSTANT_NET_PLAYER_PROFILE_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_PROFILE_SIZE,
    GAME_CONSTANT_NET_PLAYER_STATE_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_STATE_SIZE,

    GAME_CONSTANT_DEBUG_MAIN_ADDRESS,
    GAME_CONSTANT_DEBUG_QUERY_UNCOLLAPSE_BUFFER_PTR,
    GAME_CONSTANT_DEBUG_QUERY_UNCOLLAPSE_CAPACITY,
    GAME_CONSTANT_DEBUG_QUERY_COLLAPSED_BUFFER_PTR,
    GAME_CONSTANT_DEBUG_QUERY_COLLAPSED_SIZE_PTR,
};

#define NETPLAYER_FLAGS_NONE        0
#define NETPLAYER_FLAGS_ACTIVE      (1 << 0)
#define NETPLAYER_FLAGS_HOST        (2 << 0)

#define ASSISTANT_CONFIRM_THRESHOLD  (10 * 60) // 10 seconds at 60fps

// Global states
//

// Player states controlled by the player who it is assigned too
// OLD TO REMOVE
struct NetPlayerState
{
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
    struct Coords16 pos;
    struct Coords8 partnerPos; // relative to player pos
    u16 networkId;
    u16 partnerMon;
    u8 facingDirection;
    u8 partnerFacingDirection;
    u8 playerFlags;
    s8 mapGroup;
    s8 mapNum;
};

struct RogueAssistantState
{
    u8 inCommBuffer[16];
    u8 outCommBuffer[32];
    u16 assistantState;
    u16 assistantSubstate;
    u16 requestState;
    u16 externalConfirmCounter;
    u8 isAssistantConnected : 1;
};

// TODO - Should really just use gBlockRecvBuffer and other similar vars for communication

EWRAM_DATA struct RogueAssistantState gRogueAssistantState;

const struct RogueAssistantHeader gRogueAssistantHeader =
{
    // TODO - Include RogueAssistant compat version
#ifdef ROGUE_EXPANSION
    .rogueVersion = 1,
#else
    .rogueVersion = 0,
#endif
#ifdef ROGUE_DEBUG
    .rogueDebug = 1,
#else
    .rogueDebug = 0,
#endif
    .multiplayerPtr = &gRogueMultiplayer,
    .netMultiplayerSize = sizeof(struct RogueNetMultiplayer),

    .netHandshakeOffset = offsetof(struct RogueNetMultiplayer, pendingHandshake),
    .netHandshakeSize = sizeof(struct RogueNetHandshake),
    .netHandshakeStateOffset = offsetof(struct RogueNetHandshake, state),
    .netHandshakePlayerIdOffset = offsetof(struct RogueNetHandshake, playerId),

    .netGameStateOffset = offsetof(struct RogueNetMultiplayer, gameState),
    .netGameStateSize = sizeof(struct RogueNetGameState),
    .netPlayerProfileOffset  = offsetof(struct RogueNetMultiplayer, playerProfiles),
    .netPlayerProfileSize = sizeof(struct RogueNetPlayerProfile),
    .netPlayerStateOffset  = offsetof(struct RogueNetMultiplayer, playerState),
    .netPlayerStateSize = sizeof(struct RogueNetPlayer),
    .netRequestStateOffset = offsetof(struct RogueNetMultiplayer, netRequestState),
    .netCurrentStateOffset = offsetof(struct RogueNetMultiplayer, netCurrentState),
    .netPlayerCount = NET_PLAYER_CAPACITY,

    .saveBlock1Ptr = &gSaveBlock1Ptr,
    .saveBlock2Ptr = &gSaveBlock2Ptr,
    .rogueBlockPtr = &gRogueSaveBlock,

    .assistantState = &gRogueAssistantState,
    .assistantConfirmSize = sizeof(gRogueAssistantState.externalConfirmCounter),
    .assistantConfirmOffset = offsetof(struct RogueAssistantState, externalConfirmCounter),

    //.inCommCapacity = sizeof(gRogueAssistantState.inCommBuffer),
    //.outCommCapacity = sizeof(gRogueAssistantState.outCommBuffer),
    //.inCommBuffer = gRogueAssistantState.inCommBuffer,
    //.outCommBuffer = gRogueAssistantState.outCommBuffer
};


static void Task_ConnectMultiplayer(u8 taskId);


// Methods
//

void Rogue_AssistantInit()
{
    PUSH_ASSISTANT_STATE(NONE);
    gRogueAssistantState.isAssistantConnected = FALSE;
    gRogueAssistantState.externalConfirmCounter = ASSISTANT_CONFIRM_THRESHOLD;

    Rogue_UpdateAssistantRequestState(REQUEST_STATE_NONE);
    RogueMP_Init();
}

bool8 Rogue_IsAssistantConnected()
{
    return gRogueAssistantState.isAssistantConnected;
}

void Rogue_UpdateAssistantState(u16 state, u16 substate)
{
    gRogueAssistantState.assistantState = state;
    gRogueAssistantState.assistantSubstate = substate;
}

void Rogue_UpdateAssistantRequestState(u16 state)
{
    gRogueAssistantState.requestState = state;
}

static void OnAssistantConnect()
{
    Rogue_PushPopup_AssistantConnected();
}

static void OnAssistantDisconnect()
{
    Rogue_PushPopup_AssistantDisconnected();

    // Close multiplayer if active
    if(RogueMP_IsActive())
        RogueMP_Close();
}

void Rogue_AssistantMainCB()
{
    // Expect the external sevice to keep stomping this counter to 0, so if it goes over threshold, we've lost connection
    if(gRogueAssistantState.isAssistantConnected)
    {
        if(!RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT))
        {
            if(++gRogueAssistantState.externalConfirmCounter >= ASSISTANT_CONFIRM_THRESHOLD)
            {
                gRogueAssistantState.isAssistantConnected = FALSE;
                OnAssistantDisconnect();
            }
        }
    }
    else if(gRogueAssistantState.externalConfirmCounter == 0)
    {
        gRogueAssistantState.isAssistantConnected = TRUE;
        OnAssistantConnect();
    }

    if(RogueMP_IsActiveOrConnecting())
        RogueMP_MainCB();
}

static void Task_WaitForConnection(u8 taskId)
{
    // Wait for connections
    if (JOY_NEW(B_BUTTON))
    {
        // Cancelled
        gSpecialVar_Result = FALSE;
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else if(Rogue_IsAssistantConnected())
    {
        // Has connected
        gSpecialVar_Result = TRUE;
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

u8 Rogue_WaitForRogueAssistant()
{
    u8 taskId = FindTaskIdByFunc(Task_WaitForConnection);
    if (taskId == TASK_NONE)
    {
        taskId = CreateTask(Task_WaitForConnection, 80);

        //gTasks[taskId1].tConnectAsHost = asHost;
        //if(asHost)
        //    Rogue_UpdateAssistantRequestState(REQUEST_STATE_MULTIPLAYER_HOST);
        //else
        //    Rogue_UpdateAssistantRequestState(REQUEST_STATE_MULTIPLAYER_JOIN);
    }

    return taskId;
}

void Rogue_AssistantOverworldCB()
{
    if(RogueMP_IsActiveOrConnecting())
        RogueMP_OverworldCB();
}