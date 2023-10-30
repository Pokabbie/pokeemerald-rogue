#include "global.h"
#include "event_data.h"
#include "main.h"
#include "script.h"
#include "string.h"
#include "string_util.h"
#include "task.h"

#include "rogue_multiplayer.h"
#include "rogue_player_customisation.h"

#define NET_STATE_NONE              0
#define NET_STATE_ACTIVE            (1 << 0)
#define NET_STATE_HOST              (2 << 0)
//#define NET_STATE_HOST              (2 << 0)

#define NET_PLAYER_FLAGS_NONE       0
#define NET_PLAYER_FLAGS_ACTIVE     (1 << 0)
#define NET_PLAYER_FLAGS_HOST       (2 << 0)

EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer = NULL;
EWRAM_DATA struct RogueNetMultiplayer gTEMPNetMultiplayer; // temporary memory holder which should be swaped out for dynamic alloc really

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
    return gRogueMultiplayer != NULL && (gRogueMultiplayer->netRequestState & NET_STATE_ACTIVE) != 0;
}

bool8 RogueMP_IsConnecting()
{
    return !RogueMP_IsActive() && RogueMP_IsActiveOrConnecting();
}

bool8 RogueMP_IsHost()
{
    return RogueMP_IsActive() && (gRogueMultiplayer->netCurrentState & NET_STATE_HOST) != 0;
}

bool8 RogueMP_IsClient()
{
    return RogueMP_IsActive() && (gRogueMultiplayer->netCurrentState & NET_STATE_HOST) == 0;
}

static void CreatePlayerProfile(bool8 isHost)
{
    u8 i;
    AGB_ASSERT(gRogueMultiplayer != NULL);
    memset(&gRogueMultiplayer->playerProfiles[0], 0, sizeof(gRogueMultiplayer->playerProfiles[0]));
    memset(&gRogueMultiplayer->players[0], 0, sizeof(gRogueMultiplayer->players[0]));

    StringCopy_PlayerName(gRogueMultiplayer->playerProfiles[0].trainerName, gSaveBlock2Ptr->playerName);
    gRogueMultiplayer->playerProfiles[0].preferredOutfit = RoguePlayer_GetOutfitId();

    for(i = 0; i < PLAYER_OUTFIT_STYLE_COUNT; ++i)
        gRogueMultiplayer->playerProfiles[0].preferredOutfitStyle[i] = RoguePlayer_GetOutfitStyle(i);


    // Initialise handshake
    if(!isHost)
    {
        memset(&gRogueMultiplayer->pendingHandshake, 0, sizeof(gRogueMultiplayer->pendingHandshake));
        memcpy(&gRogueMultiplayer->pendingHandshake.profile, &gRogueMultiplayer->players[0], sizeof(&gRogueMultiplayer->players[0]));
        gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_SEND_TO_HOST;
    }
}

void RogueMP_OpenHost()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));

    gRogueMultiplayer = &gTEMPNetMultiplayer;
    CreatePlayerProfile(TRUE);
    gRogueMultiplayer->netRequestState = NET_STATE_ACTIVE | NET_STATE_HOST;
}

void RogueMP_OpenClient()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));

    gRogueMultiplayer = &gTEMPNetMultiplayer;
    CreatePlayerProfile(FALSE);
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
            gRogueMultiplayer->pendingHandshake.accepted = TRUE;
            gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_SEND_TO_CLIENT;
        }
    }
    else
    {
        if(RogueMP_IsConnecting())
        {
            if(gRogueMultiplayer->pendingHandshake.state == NET_HANDSHAKE_STATE_SEND_TO_CLIENT)
            {
                DebugPrint("Incoming handshake response...");

                // Incoming response from host
                if(!gRogueMultiplayer->pendingHandshake.accepted)
                {
                    RogueMP_Close();
                    return;
                }

                gRogueMultiplayer->pendingHandshake.state = NET_HANDSHAKE_STATE_NONE;
            }
        }
    }
}

void RogueMP_OverworldCB()
{
    
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
        else
        {
            DebugPrint("Client connected successfully");
            // Has connected
            gSpecialVar_Result = TRUE;
            EnableBothScriptContexts();
            DestroyTask(taskId);
        }
    }
}