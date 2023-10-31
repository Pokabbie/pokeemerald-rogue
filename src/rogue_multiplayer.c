#include "global.h"
#include "event_data.h"
#include "main.h"
#include "script.h"
#include "string.h"
#include "string_util.h"
#include "task.h"

#include "rogue_controller.h"
#include "rogue_multiplayer.h"
#include "rogue_player_customisation.h"
#include "rogue_save.h"

#define NET_STATE_NONE              0
#define NET_STATE_ACTIVE            (1 << 0)
#define NET_STATE_HOST              (2 << 0)
//#define NET_STATE_HOST              (2 << 0)

#define NET_PLAYER_FLAGS_NONE       0
#define NET_PLAYER_FLAGS_ACTIVE     (1 << 0)
#define NET_PLAYER_FLAGS_HOST       (2 << 0)

EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer = NULL;
EWRAM_DATA struct RogueNetMultiplayer gTEMPNetMultiplayer; // temporary memory holder which should be swaped out for dynamic alloc really

static void Host_HandleHandshakeRequest();
static void Host_UpdateGameState();
static void Client_SetupHandshakeRequest();
static void Client_HandleHandshakeResponse();
static void UpdateLocalPlayerState();
static void Task_WaitForConnection(u8 taskId);

STATIC_ASSERT(ARRAY_COUNT(gRogueMultiplayer->playerProfiles[0].preferredOutfitStyle) == PLAYER_OUTFIT_STYLE_COUNT, NetPlayerProfileOutfitStyleCount);


bool8 RogueMP_Init()
{
    gRogueMultiplayer = NULL;
}

bool8 RogueMP_IsActive()
{
    return gRogueMultiplayer != NULL && (gRogueMultiplayer->netCurrentState & NET_STATE_ACTIVE) != 0;
}

bool8 RogueMP_IsActiveOrConnecting()
{
    return RogueMP_IsActive() || RogueMP_IsConnecting();
}

bool8 RogueMP_IsConnecting()
{
    return gRogueMultiplayer != NULL && !RogueMP_IsHost() && gRogueMultiplayer->localPlayerId == 0;
}

bool8 RogueMP_IsHost()
{
    return RogueMP_IsActive() && (gRogueMultiplayer->netCurrentState & NET_STATE_HOST) != 0;
}

bool8 RogueMP_IsClient()
{
    return RogueMP_IsActive() && (gRogueMultiplayer->netCurrentState & NET_STATE_HOST) == 0;
}

static void CreatePlayerProfile(struct RogueNetPlayerProfile* profile)
{
    u8 i;

    // Setup profile that we want to connect using
    StringCopy_PlayerName(profile->trainerName, gSaveBlock2Ptr->playerName);
    profile->preferredOutfit = RoguePlayer_GetOutfitId();

    for(i = 0; i < PLAYER_OUTFIT_STYLE_COUNT; ++i)
        profile->preferredOutfitStyle[i] = RoguePlayer_GetOutfitStyle(i);
}

void RogueMP_OpenHost()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));

    gRogueMultiplayer = &gTEMPNetMultiplayer;

    gRogueMultiplayer->localPlayerId = 0;
    CreatePlayerProfile(&gRogueMultiplayer->playerProfiles[gRogueMultiplayer->localPlayerId]);

    gRogueMultiplayer->netRequestState = NET_STATE_ACTIVE | NET_STATE_HOST;
}

void RogueMP_OpenClient()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));

    gRogueMultiplayer = &gTEMPNetMultiplayer;

    Client_SetupHandshakeRequest();
    gRogueMultiplayer->netRequestState = NET_STATE_ACTIVE;
}

void RogueMP_Close()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    gRogueMultiplayer = NULL;
}

void RogueMP_MainCB()
{
    if(RogueMP_IsHost())
    {
        if(gRogueMultiplayer->pendingHandshake.state == NET_HANDSHAKE_STATE_SEND_TO_HOST)
        {
            // Incoming new connection request
            DebugPrint("Incoming client request...");
            Host_HandleHandshakeRequest();
        }
        else
        {
            Host_UpdateGameState();
        }
    }
    else
    {
        if(RogueMP_IsConnecting())
        {
            if(gRogueMultiplayer->pendingHandshake.state == NET_HANDSHAKE_STATE_SEND_TO_CLIENT)
            {
                // Incoming response from host
                DebugPrint("Incoming handshake response...");
                Client_HandleHandshakeResponse();
            }
        }
    }

    if(RogueMP_IsActive())
    {
        UpdateLocalPlayerState();
    }
}

void RogueMP_OverworldCB()
{
    
}

static bool8 IsExVersion()
{
#ifdef ROGUE_EXPANSION
    return TRUE;
#else
    return FALSE;
#endif
}

static void Host_HandleHandshakeRequest()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);

    if(gRogueMultiplayer->pendingHandshake.isVersionEx != IsExVersion())
    {
        // Flavour doesn't match
        gRogueMultiplayer->pendingHandshake.isVersionEx = IsExVersion();
        gRogueMultiplayer->pendingHandshake.accepted = FALSE;
        gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_SEND_TO_CLIENT;
        return;
    }

    if(gRogueMultiplayer->pendingHandshake.saveVersionId != RogueSave_GetVersionId())
    {
        // Save version doesn't match
        gRogueMultiplayer->pendingHandshake.saveVersionId = RogueSave_GetVersionId();
        gRogueMultiplayer->pendingHandshake.accepted = FALSE;
        gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_SEND_TO_CLIENT;
        return;
    }

    // Is valid so accept
    gRogueMultiplayer->pendingHandshake.accepted = TRUE;
    gRogueMultiplayer->pendingHandshake.playerId = 1; // TODO - Assign to free slot
    memcpy(&gRogueMultiplayer->playerProfiles[gRogueMultiplayer->pendingHandshake.playerId], &gRogueMultiplayer->pendingHandshake.profile, sizeof(gRogueMultiplayer->pendingHandshake.profile));


    gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_SEND_TO_CLIENT;
}

static void Client_SetupHandshakeRequest()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    CreatePlayerProfile(&gRogueMultiplayer->pendingHandshake.profile);

    // TODO - Setup versioning vars
    gRogueMultiplayer->pendingHandshake.saveVersionId = RogueSave_GetVersionId();
    gRogueMultiplayer->pendingHandshake.isVersionEx = IsExVersion();

    gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_SEND_TO_HOST;
}

static void Client_HandleHandshakeResponse()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);

    if(!gRogueMultiplayer->pendingHandshake.accepted)
    {
        DebugPrint("Handshake wasn't accepted.");
        RogueMP_Close();
        return;
    }

    gRogueMultiplayer->localPlayerId = gRogueMultiplayer->pendingHandshake.playerId;
    gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_NONE;
}

static void Host_UpdateGameState()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);

    if(!Rogue_IsRunActive())
    {
        // TODO - Only need to do this if there is a change really
        memcpy(&gRogueMultiplayer->gameState.hubMap, &gRogueSaveBlock->hubMap, sizeof(gRogueSaveBlock->hubMap));
    }
    else
    {

    }
}

static void UpdateLocalPlayerState()
{
    struct RogueNetPlayer* player = &gRogueMultiplayer->players[gRogueMultiplayer->localPlayerId];

    AGB_ASSERT(gRogueMultiplayer != NULL);

    // TEMP
    player->playerFlags = gRogueMultiplayer->localPlayerId;
}

//#define tConnectAsHost data[1]

u8 RogueMP_WaitForConnection()
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

static void Task_WaitForConnection(u8 taskId)
{
    // Wait for connections
    if (JOY_NEW(B_BUTTON))
    {
        // Cancelled
        RogueMP_Close();
        gSpecialVar_Result = FALSE;
        EnableBothScriptContexts();
        DestroyTask(taskId);
    }
    else if(RogueMP_IsActive())
    {
        if(RogueMP_IsHost())
        {
            DebugPrint("Host created successfully");
            // Has connected
            gSpecialVar_Result = TRUE;
            EnableBothScriptContexts();
            DestroyTask(taskId);
        }
        else if(!RogueMP_IsConnecting())
        {
            DebugPrint("Client connected successfully");
            // Has connected
            gSpecialVar_Result = TRUE;
            EnableBothScriptContexts();
            DestroyTask(taskId);
        }
    }
    // TODO - Error handling for if handshake wasn't accepted
    //else if(!RogueMP_IsConnecting())
    //{
    //    DebugPrint("Connection aborted...");
    //    // TODO - store some infor
    //    gSpecialVar_Result = FALSE;
    //    EnableBothScriptContexts();
    //    DestroyTask(taskId);
    //}
}