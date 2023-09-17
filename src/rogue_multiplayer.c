#include "global.h"
#include "event_data.h"
#include "main.h"
#include "script.h"
#include "task.h"

#include "rogue_multiplayer.h"

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

bool8 RogueMP_Init()
{
    gRogueMultiplayer = NULL;
}

bool8 RogueMP_IsActive()
{
    return gRogueMultiplayer != NULL && (gRogueMultiplayer->netCurrentState & NET_STATE_ACTIVE) != 0;
}

bool8 RogueMP_IsHost()
{
    return RogueMP_IsActive() && (gRogueMultiplayer->netCurrentState & NET_STATE_HOST) != 0;
}

bool8 RogueMP_IsClient()
{
    return RogueMP_IsActive() && (gRogueMultiplayer->netCurrentState & NET_STATE_HOST) == 0;
}

void RogueMP_OpenHost()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));

    gRogueMultiplayer = &gTEMPNetMultiplayer;
    gRogueMultiplayer->netRequestState = NET_STATE_ACTIVE | NET_STATE_HOST;
}

void RogueMP_OpenClient()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));

    gRogueMultiplayer = &gTEMPNetMultiplayer;
    gRogueMultiplayer->netRequestState = NET_STATE_ACTIVE;
}

void RogueMP_Close()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    gRogueMultiplayer = NULL;
}

void RogueMP_Update()
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
        // Has connected
        gSpecialVar_Result = TRUE;
        EnableBothScriptContexts();
        DestroyTask(taskId);
    }
}