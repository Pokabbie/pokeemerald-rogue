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
#include "malloc.h"
#include "new_game.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "string_util.h"
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

#define ASSISTANT_CONFIRM_THRESHOLD         (10 * 60) // 10 seconds at 60fps
#define ASSISTANT_CONFIRM_THRESHOLD_BOX     (3 * 60) // Used a harsher threshold when in theb ox

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

//TOTAL_BOXES_COUNT

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

// Lightweight box data
struct RoguePerBoxData
{
    u8 name[BOX_NAME_LENGTH + 1];
    u8 monCount;
    u8 wallpaper;
};

struct RogueBoxGlobalData
{
    struct RoguePerBoxData boxData[ASSISTANT_HOME_TOTAL_BOXES];
    struct BoxPokemon* destBoxMons;
    u8 boxRemoteIndexOrder[ASSISTANT_HOME_TOTAL_BOXES];
    u32 trainerId;
};

static EWRAM_DATA struct RogueBoxGlobalData* sRogueAssistantBoxData = NULL;

EWRAM_DATA struct RogueAssistantState gRogueAssistantState;

STATIC_ASSERT(TOTAL_BOXES_COUNT == ASSISTANT_HOME_LOCAL_BOXES, SizeOfAssistantLocalBoxes);

// Make sure to change the same value in RogueAssistant if this ever changes
// (These numbers must match in order to connect)
#define ROGUE_ASSISTANT_COMPAT_VERSION 1

const struct RogueAssistantHeader gRogueAssistantHeader =
{
    .rogueVersion = ROGUE_VERSION,
#ifdef ROGUE_DEBUG
    .rogueDebug = 1,
#else
    .rogueDebug = 0,
#endif
    .rogueAssistantCompatVersion = ROGUE_ASSISTANT_COMPAT_VERSION,

    // MP Data
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

    // Assistant Data
    .assistantState = &gRogueAssistantState,
    .assistantConfirmSize = sizeof(gRogueAssistantState.externalConfirmCounter),
    .assistantConfirmOffset = offsetof(struct RogueAssistantState, externalConfirmCounter),

    // Box Data
    .homeBoxPtr = &sRogueAssistantBoxData,
    .homeBoxSize = sizeof(struct RogueBoxGlobalData),
    .homeLocalBoxCount = ASSISTANT_HOME_LOCAL_BOXES,
    .homeTotalBoxCount = ASSISTANT_HOME_TOTAL_BOXES,
    .homeMinimalBoxOffset = offsetof(struct RogueBoxGlobalData, boxData),
    .homeMinimalBoxSize = sizeof(struct RoguePerBoxData),
    .homeDestMonOffset = offsetof(struct RogueBoxGlobalData, destBoxMons),
    .homeDestMonSize = sizeof(struct BoxPokemon) * IN_BOX_COUNT,
    .homeTrainerIdOffset = offsetof(struct RogueBoxGlobalData, trainerId),
    .homeRemoteIndexOrderOffset = offsetof(struct RogueBoxGlobalData, boxRemoteIndexOrder),

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
            u16 failThreshold = (sRogueAssistantBoxData != NULL) ? ASSISTANT_CONFIRM_THRESHOLD_BOX : ASSISTANT_CONFIRM_THRESHOLD;

            if(++gRogueAssistantState.externalConfirmCounter >= failThreshold)
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

void RogueBox_OpenConnection()
{
    AGB_ASSERT(sRogueAssistantBoxData == NULL);
    if(sRogueAssistantBoxData == NULL)
    {
        u32 i;
        sRogueAssistantBoxData = AllocZeroed(sizeof(struct RogueBoxGlobalData));
        sRogueAssistantBoxData->destBoxMons = &gPokemonStoragePtr->boxes[0][0];
        sRogueAssistantBoxData->trainerId = GetTrainerId(gSaveBlock2Ptr->playerTrainerId);

        memset(sRogueAssistantBoxData->boxRemoteIndexOrder, 255, sizeof(sRogueAssistantBoxData->boxRemoteIndexOrder));

        for(i = 0; i < ASSISTANT_HOME_LOCAL_BOXES; ++i)
        {
            StringCopyN(sRogueAssistantBoxData->boxData[i].name, gPokemonStoragePtr->boxNames[i], BOX_NAME_LENGTH);
            sRogueAssistantBoxData->boxData[i].monCount = CountMonsInBox(i);
            sRogueAssistantBoxData->boxData[i].wallpaper = gPokemonStoragePtr->boxWallpapers[i];

            sRogueAssistantBoxData->boxRemoteIndexOrder[i] = i;
        }
    }
}

void RogueBox_CloseConnection()
{
    AGB_ASSERT(sRogueAssistantBoxData != NULL);
    if(sRogueAssistantBoxData != NULL)
    {
        // Copy name and wallpapers back
        u32 i;

        for(i = 0; i < ASSISTANT_HOME_LOCAL_BOXES; ++i)
        {
            StringCopyN(gPokemonStoragePtr->boxNames[i], RogueBox_GetName(i), BOX_NAME_LENGTH);
            gPokemonStoragePtr->boxWallpapers[i] = sRogueAssistantBoxData->boxData[i].wallpaper;
        }

        Free(sRogueAssistantBoxData);
        sRogueAssistantBoxData = NULL;
    }
}

bool32 RogueBox_IsConnectedAndReady()
{
    return Rogue_IsAssistantConnected() && sRogueAssistantBoxData != NULL && sRogueAssistantBoxData->boxRemoteIndexOrder[ASSISTANT_HOME_TOTAL_BOXES - 1] != 255;
}

u8 RogueBox_GetCountInBox(u8 i)
{
    AGB_ASSERT(sRogueAssistantBoxData != NULL);
    //AGB_ASSERT(sRogueAssistantBoxData->boxData[i].isValid);
    return sRogueAssistantBoxData->boxData[i].monCount;
}

static u8 const sText_DefaultBoxName[BOX_NAME_LENGTH + 1] = _("UNNAMED");

u8 const* RogueBox_GetName(u8 boxId)
{
    u32 i;
    u8* srcStr = sRogueAssistantBoxData->boxData[boxId].name;
    AGB_ASSERT(sRogueAssistantBoxData != NULL);

    // External box names come through as all 0's so handle those here
    for(i = 0; i < BOX_NAME_LENGTH + 1; ++i)
    {
        // Found non 0 char so assume this is valid
        if(srcStr[i] != 0)
            return srcStr;
    }

    return sText_DefaultBoxName;
}

bool32 RogueBox_IsLocalBox(u8 i)
{
    return i < ASSISTANT_HOME_LOCAL_BOXES;
}

void RogueBox_SwapBoxes(u8 a, u8 b)
{
    AGB_ASSERT(sRogueAssistantBoxData != NULL);
    if(a != b && a < ASSISTANT_HOME_TOTAL_BOXES && b < ASSISTANT_HOME_TOTAL_BOXES)
    {
        struct RoguePerBoxData tempBoxData;
        u8 temp8;
        SWAP(sRogueAssistantBoxData->boxData[a], sRogueAssistantBoxData->boxData[b], tempBoxData);
        SWAP(sRogueAssistantBoxData->boxRemoteIndexOrder[a], sRogueAssistantBoxData->boxRemoteIndexOrder[b], temp8);
    }
}