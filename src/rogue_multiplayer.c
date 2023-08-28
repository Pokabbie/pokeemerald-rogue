#include "global.h"

#include "rogue_multiplayer.h"

#define NET_PLAYER_FLAGS_NONE        0
#define NET_PLAYER_FLAGS_ACTIVE      (1 << 0)
#define NET_PLAYER_FLAGS_HOST        (2 << 0)

EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer = NULL;
EWRAM_DATA struct RogueNetMultiplayer gTEMPNetMultiplayer; // temporary memory holder which should be swaped out for dynamic alloc really

bool8 RogueMP_Init()
{
    gRogueMultiplayer = NULL;
    memset(&gTEMPNetMultiplayer, 0, sizeof(gTEMPNetMultiplayer));
}

bool8 RogueMP_IsActive()
{
    return gRogueMultiplayer != NULL;
}

bool8 RogueMP_IsHost()
{
    return gRogueMultiplayer != NULL && (gRogueMultiplayer->players[0].playerFlags & NET_PLAYER_FLAGS_HOST) != 0;
}

bool8 RogueMP_IsClient()
{
    return gRogueMultiplayer != NULL && (gRogueMultiplayer->players[0].playerFlags & NET_PLAYER_FLAGS_HOST) == 0;
}

void RogueMP_OpenHost()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    gRogueMultiplayer = &gTEMPNetMultiplayer;
}

void RogueMP_OpenClient()
{
    AGB_ASSERT(gRogueMultiplayer == NULL);
    gRogueMultiplayer = &gTEMPNetMultiplayer;
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

void RogueMP_WaitForConnection()
{
    //if (FindTaskIdByFunc(Task_ConnectMultiplayer) == TASK_NONE)
    //{
    //    u8 taskId1;
//
    //    taskId1 = CreateTask(Task_ConnectMultiplayer, 80);
    //    gTasks[taskId1].tConnectAsHost = asHost;
//
    //    if(asHost)
    //        Rogue_UpdateAssistantRequestState(REQUEST_STATE_MULTIPLAYER_HOST);
    //    else
    //        Rogue_UpdateAssistantRequestState(REQUEST_STATE_MULTIPLAYER_JOIN);
    //}
}