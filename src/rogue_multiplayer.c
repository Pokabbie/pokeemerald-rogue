#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "event_data.h"
#include "main.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "follow_me.h"
#include "script.h"
#include "string.h"
#include "string_util.h"
#include "task.h"

#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_followmon.h"
#include "rogue_multiplayer.h"
#include "rogue_player_customisation.h"
#include "rogue_ridemon.h"
#include "rogue_save.h"
#include "rogue_timeofday.h"

#define NET_STATE_NONE              0
#define NET_STATE_ACTIVE            (1 << 0)
#define NET_STATE_HOST              (1 << 1)


#define NET_PLAYER_STATE_FLAG_NONE      0
#define NET_PLAYER_STATE_FLAG_RIDING    (1 << 0)
#define NET_PLAYER_STATE_FLAG_FLYING    (1 << 1)

// Enable logging of MP
#if 1
#define MpLog(pBuf) DebugPrint("[MP LOG] " pBuf)
#define MpLogf(pBuf, ...) DebugPrintf("[MP LOG] " pBuf, __VA_ARGS__)
#else
#define MpLog(pBuf)
#define MpLogf(pBuf, ...)
#endif

struct SyncedObjectEventInfo
{
    struct RogueNetPlayerMovement* movementBuffer;
    struct Coords16 pos;
    u16 gfxId;
    s8 mapNum;
    s8 mapGroup;
    u8 adventureTileNum;
    u8 adventureDifficulty;
    u8 localId;
    u8 movementBufferHead;
    u8 movementBufferReadOffset;
    u8 facingDirection : 4;
    u8 elevation : 4;
};

struct RogueLocalMP
{
    u8 lastProcessedRemoteSendCmd;
    u8 recentSendCmd;
    u8 recentResult;
    u8 hasPendingPlayerInteraction : 1;
};

// Temporary data, that isn't remembered
EWRAM_DATA struct RogueLocalMP gRogueLocalMP = {0};

EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer = NULL;
EWRAM_DATA struct RogueNetMultiplayer gTEMPNetMultiplayer; // temporary memory holder which should be swaped out for dynamic alloc really

static void Host_HandleHandshakeRequest();
static void Client_SetupHandshakeRequest();
static void Client_HandleHandshakeResponse();
static void Host_UpdateGameState();
static void Client_UpdateGameState();
static void UpdateLocalPlayerState(u8 playerId);
static void ProcessPlayerCommands();
static void UpdateLocalPlayerStatus();
static void Task_WaitPlayerStatusSync(u8 taskId);
static void Task_WaitForConnection(u8 taskId);
static void Task_WaitForSendFinish(u8 taskId);

static u8 ProcessSyncedObjectEvent(struct SyncedObjectEventInfo* syncInfo);
static void ProcessSyncedObjectMovement(struct SyncedObjectEventInfo* syncInfo, struct ObjectEvent* objectEvent);

STATIC_ASSERT(ARRAY_COUNT(gRogueMultiplayer->playerProfiles[0].preferredOutfitStyle) == PLAYER_OUTFIT_STYLE_COUNT, NetPlayerProfileOutfitStyleCount);


void RogueMP_Init()
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

ROGUE_STATIC_ASSERT(NET_PLAYER_CAPACITY == 2, PlayerIdAssume2Players);

u8 RogueMP_GetLocalPlayerId()
{
    if(gRogueMultiplayer != NULL)
        return gRogueMultiplayer->localPlayerId;
    else
        return 0;
}

u8 RogueMP_GetRemotePlayerId()
{
    if(gRogueMultiplayer != NULL)
        return gRogueMultiplayer->localPlayerId ^ 1;
    else
        return 0;
}

static struct RogueNetPlayer* GetLocalPlayer()
{
    return &gRogueMultiplayer->playerState[RogueMP_GetLocalPlayerId()];
}

static struct RogueNetPlayer* GetRemotePlayer()
{
    return &gRogueMultiplayer->playerState[RogueMP_GetRemotePlayerId()];
}

bool8 RogueMP_IsRemotePlayerActive()
{ 
    if(gRogueMultiplayer != NULL)
        return gRogueMultiplayer->playerProfiles[RogueMP_GetRemotePlayerId()].isActive;
    else
        return FALSE;
}

static void CreatePlayerProfile(struct RogueNetPlayerProfile* profile)
{
    u8 i;

    // Setup profile that we want to connect using
    StringCopy_PlayerName(profile->trainerName, gSaveBlock2Ptr->playerName);
    StringCopyN(profile->pokemonHubName, gSaveBlock2Ptr->pokemonHubName, POKEMON_HUB_NAME_LENGTH + 1);
    memcpy(profile->playerTrainerId, gSaveBlock2Ptr->playerTrainerId, sizeof(gSaveBlock2Ptr->playerTrainerId));

    profile->isActive = TRUE;
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
    RogueMP_RemoveObjectEvents();
    gRogueMultiplayer = NULL;
}

u8 RogueMP_GetPlayerOutfitId(u8 playerId)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    return gRogueMultiplayer->playerProfiles[playerId].preferredOutfit;
}

u16 RogueMP_GetPlayerOutfitStyle(u8 playerId, u8 outfitStyle)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    return gRogueMultiplayer->playerProfiles[playerId].preferredOutfitStyle[outfitStyle];
}

u8 const* RogueMP_GetPlayerName(u8 playerId)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    return gRogueMultiplayer->playerProfiles[playerId].trainerName;
}

u8 const* RogueMP_GetPlayerHubName(u8 playerId)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    return gRogueMultiplayer->playerProfiles[playerId].pokemonHubName;
}

u8 const* RogueMP_GetPlayerTrainerId(u8 playerId)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    return gRogueMultiplayer->playerProfiles[playerId].playerTrainerId;
}

u8 RogueMP_GetPlayerStatus(u8 playerId)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);
    return gRogueMultiplayer->playerState[playerId].playerStatus;
}

void RogueMP_MainCB()
{
    // Bump counter each frame so we can divide up MP ops
    ++gRogueMultiplayer->localCounter;

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
        else
        {
            Client_UpdateGameState();
        }
    }

    if(RogueMP_IsActive())
    {
        START_TIMER(ROGUE_MP_PROCESS_PLAYER_COMMANDS);
        ProcessPlayerCommands();
        STOP_TIMER(ROGUE_MP_PROCESS_PLAYER_COMMANDS);
    }
}

void RogueMP_OverworldCB()
{
    START_TIMER(ROGUE_MP_UPDATE);

    if(RogueMP_IsActive())
    {
        // To split up the processing, only process 1 player per frame
        //u8 playerId = gRogueMultiplayer->localCounter % NET_PLAYER_CAPACITY;
        //UpdateLocalPlayerState(playerId);
        
        u8 i;
        for(i = 0; i < NET_PLAYER_CAPACITY; ++i)
            UpdateLocalPlayerState(i);

    }

    STOP_TIMER(ROGUE_MP_UPDATE);
}

void RogueMP_RemoveObjectEvents()
{
    u8 i;
    u8 objectId;

    for(i = OBJ_EVENT_ID_MULTIPLAYER_FIRST; i <= OBJ_EVENT_ID_MULTIPLAYER_LAST; ++i)
    {
        objectId = GetSpecialObjectEventIdByLocalId(i);
        if(objectId != OBJECT_EVENTS_COUNT)
        {
            RemoveObjectEvent(&gObjectEvents[objectId]);
        }
    }
}

#define canCancel       data[0]
#define awaitUpdate     data[1]
#define startedStatusCounterPtr ((u16*)&gTasks[taskId].data[2])

u8 RogueMP_WaitPlayerStatusSync(bool8 allowCancel)
{
    u8 taskId = FindTaskIdByFunc(Task_WaitPlayerStatusSync);
    if (taskId == TASK_NONE)
    {
        struct RogueNetPlayer* localPlayer = GetLocalPlayer();
        struct RogueNetPlayer* remotePlayer = GetRemotePlayer();

        taskId = CreateTask(Task_WaitPlayerStatusSync, 80);
        gTasks[taskId].canCancel = allowCancel;
        gTasks[taskId].awaitUpdate = FALSE;
    }

    return taskId;
}

u8 RogueMP_WaitUpdatedPlayerStatus(bool8 allowCancel)
{
    u8 taskId = FindTaskIdByFunc(Task_WaitPlayerStatusSync);
    if (taskId == TASK_NONE)
    {
        struct RogueNetPlayer* localPlayer = GetLocalPlayer();

        taskId = CreateTask(Task_WaitPlayerStatusSync, 80);
        gTasks[taskId].canCancel = allowCancel;
        gTasks[taskId].awaitUpdate = TRUE;
        *startedStatusCounterPtr = localPlayer->playerStatusCounter;
    }

    return taskId;
}

static void Task_WaitPlayerStatusSync(u8 taskId)
{
    if(RogueMP_IsRemotePlayerActive())
    {
        struct RogueNetPlayer* localPlayer = GetLocalPlayer();
        struct RogueNetPlayer* remotePlayer = GetRemotePlayer();

        if(gTasks[taskId].canCancel && JOY_NEW(B_BUTTON))
        {
            gSpecialVar_Result = FALSE;
            //gSpecialVar_0x8000 = FALSE;
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
        else if(remotePlayer->playerStatusCounter == localPlayer->playerStatusCounter)
        {
            if(gTasks[taskId].awaitUpdate && localPlayer->playerStatusCounter == *startedStatusCounterPtr)
                return;

            // If we aborted, this will missmatch
            gSpecialVar_Result = (remotePlayer->playerStatus == localPlayer->playerStatus);
            gSpecialVar_0x8000 = remotePlayer->playerStatusParam;
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
    }
    else
    {
        gSpecialVar_Result = FALSE;
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

#undef canCancel
#undef awaitUpdate
#undef startedStatusCounterPtr

static void UpdateLocalPlayerStatus()
{
    struct RogueNetPlayer* localPlayer = GetLocalPlayer();
    struct RogueNetPlayer* remotePlayer = GetRemotePlayer();

    if(localPlayer->playerStatus == MP_PLAYER_STATUS_NONE && remotePlayer->playerStatus == MP_PLAYER_STATUS_NONE && remotePlayer->playerStatusCounter == localPlayer->playerStatusCounter)
    {
        // Reset counter here to avoid overflow situations if possible (This still isn't ideal)
        localPlayer->playerStatusCounter = 0;
    }
    // If positive, the remote is ahead of us, so we locally need to sync up
    else if(remotePlayer->playerStatusCounter > localPlayer->playerStatusCounter)
    {
        MpLogf("remote status:%d counter:%d param:%d", remotePlayer->playerStatus, remotePlayer->playerStatusCounter, remotePlayer->playerStatusParam);

        switch(remotePlayer->playerStatus)
        {
            case MP_PLAYER_STATUS_NONE:
            {
                // Just sync up, don't need to do anything else
                localPlayer->playerStatus = remotePlayer->playerStatus;
                localPlayer->playerStatusCounter = remotePlayer->playerStatusCounter;
            }
            break;

            case MP_PLAYER_STATUS_TALK_TO_PLAYER:
            {
                // Begin interaction from our side
                if(localPlayer->playerStatus != MP_PLAYER_STATUS_TALK_TO_PLAYER)
                    gRogueLocalMP.hasPendingPlayerInteraction = TRUE;

                localPlayer->playerStatus = remotePlayer->playerStatus;
                localPlayer->playerStatusCounter = remotePlayer->playerStatusCounter;
                localPlayer->playerStatusParam = remotePlayer->playerStatusParam;
            }
            break;
        }
    }
}

extern const u8 Rogue_RemoteInteractMultiplayerPlayer[];

bool8 RogueMP_TryExecuteScripts()
{
    if(gRogueLocalMP.hasPendingPlayerInteraction)
    {
        gRogueLocalMP.hasPendingPlayerInteraction = FALSE;
        ScriptContext_SetupScript(Rogue_RemoteInteractMultiplayerPlayer);
        return FALSE;
    }

    return FALSE;
}

void RogueMP_PushLocalPlayerStatus(u8 status, u16 param)
{
    if(RogueMP_IsActive())
    {
        struct RogueNetPlayer* localPlayer = GetLocalPlayer();
        localPlayer->playerStatus = status;
        localPlayer->playerStatusParam = param;
        localPlayer->playerStatusCounter = (RogueMP_IsRemotePlayerActive() ? GetRemotePlayer() : localPlayer)->playerStatusCounter + 1;

        MpLogf("local status:%d counter:%d param:%d", localPlayer->playerStatus, localPlayer->playerStatusCounter, localPlayer->playerStatusParam);
    }
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
    gRogueMultiplayer->pendingHandshake.profile.isActive = TRUE;
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
        // Only do 1 large copy per frame
        switch (gRogueMultiplayer->localCounter % 2)
        {
        case 0:
            memcpy(&gRogueMultiplayer->gameState.hub.hubMap, &gRogueSaveBlock->hubMap, sizeof(gRogueSaveBlock->hubMap));
            break;
        
        case 1:
            memcpy(&gRogueMultiplayer->gameState.hub.difficultyConfig, &gRogueSaveBlock->difficultyConfig, sizeof(gRogueSaveBlock->difficultyConfig));
            break;
        }

        gRogueMultiplayer->gameState.hub.timeOfDay = RogueToD_GetTime();
        gRogueMultiplayer->gameState.hub.season = RogueToD_GetSeason();
        gRogueMultiplayer->gameState.adventure.isRunActive = FALSE;
    }
    else
    {
        gRogueMultiplayer->gameState.adventure.baseSeed = gRogueRun.baseSeed;
        gRogueMultiplayer->gameState.adventure.isRunActive = TRUE;
    }
}

static void Client_UpdateGameState()
{
    AGB_ASSERT(gRogueMultiplayer != NULL);

    if(!Rogue_IsRunActive())
    {
    }
}

static bool8 IsObjectCurrentlyMoving(struct ObjectEvent* objectEvent)
{
    // Require coords to change
    if(objectEvent->currentCoords.x == objectEvent->previousCoords.x && objectEvent->currentCoords.y == objectEvent->previousCoords.y)
        return FALSE;

    // Certain actions are illegal as they will cause the position to get lost be observing client
    if(objectEvent->movementActionId <= MOVEMENT_ACTION_FACE_RIGHT) // objectEvent->movementActionId >= MOVEMENT_ACTION_FACE_DOWN implied
        return FALSE;

    if(objectEvent->movementActionId >= MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_DOWN && objectEvent->movementActionId <= MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT)
        return FALSE;

    if(objectEvent->movementActionId >= MOVEMENT_ACTION_DELAY_1 && objectEvent->movementActionId <= MOVEMENT_ACTION_DELAY_16)
        return FALSE;

    if(objectEvent->movementActionId >= MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_DOWN && objectEvent->movementActionId <= MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT)
        return FALSE;

    return TRUE;
}

static bool8 HasMovementUpdated(struct RogueNetPlayerMovement* oldMovement, struct RogueNetPlayerMovement* newMovement)
{
    return oldMovement->pos.x != newMovement->pos.x || oldMovement->pos.y != newMovement->pos.y;
}

static u8 GetLocalAdventureTileNum()
{
    if(Rogue_IsRunActive())
    {
        return RogueAdv_GetTileNum();
    }

    return 0;
}

static u8 GetLocalAdventureDifficulty()
{
    if(Rogue_IsRunActive())
    {
        return Rogue_GetCurrentDifficulty();
    }

    return ROGUE_MAX_BOSS_COUNT;
}

static void WritePlayerState(struct RogueNetPlayer* player)
{
    if(gPlayerAvatar.objectEventId != OBJECT_EVENTS_COUNT)
    {
        if(IsObjectCurrentlyMoving(&gObjectEvents[gPlayerAvatar.objectEventId]))
        {
            struct RogueNetPlayerMovement newMovement;
            newMovement.pos.x = gObjectEvents[gPlayerAvatar.objectEventId].previousCoords.x;
            newMovement.pos.y = gObjectEvents[gPlayerAvatar.objectEventId].previousCoords.y;
            newMovement.movementAction = gObjectEvents[gPlayerAvatar.objectEventId].movementActionId;

            if(HasMovementUpdated(&player->movementBuffer[player->movementBufferHead], &newMovement))
            {
                u8 newHead = (player->movementBufferHead + 1) % ARRAY_COUNT(player->movementBuffer);

                memcpy(&player->movementBuffer[newHead], &newMovement, sizeof(newMovement));
                player->movementBufferHead = newHead;
            }
        }

        player->playerPos.x = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.x;
        player->playerPos.y = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y;
        player->currentElevation = gObjectEvents[gPlayerAvatar.objectEventId].currentElevation;
        player->facingDirection = gObjectEvents[gPlayerAvatar.objectEventId].facingDirection;
        player->mapGroup = gSaveBlock1Ptr->location.mapGroup;
        player->mapNum = gSaveBlock1Ptr->location.mapNum;
        player->adventureTileNum = GetLocalAdventureTileNum();
        player->adventureDifficulty = GetLocalAdventureDifficulty();

        player->playerFlags = NET_PLAYER_STATE_FLAG_NONE;
        if(TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING))
        {
            player->playerFlags |= NET_PLAYER_STATE_FLAG_RIDING;
            player->partnerMon = Rogue_GetRideMonSpeciesGfx(0); // 0 is always local player

            if(Rogue_IsRideMonFlying())
                player->playerFlags |= NET_PLAYER_STATE_FLAG_FLYING;
        }
        else if(FollowMon_IsPartnerMonActive())
        {
            u8 followerObjectEventId = GetFollowerObjectId();
            player->partnerMon = FollowMon_GetPartnerFollowSpecies(TRUE);
            player->partnerPos.x = gObjectEvents[followerObjectEventId].currentCoords.x;
            player->partnerPos.y = gObjectEvents[followerObjectEventId].currentCoords.y;
            player->partnerFacingDirection = gObjectEvents[followerObjectEventId].facingDirection;
        }
        else
        {
            player->partnerMon = SPECIES_NONE;
        }
    }
}

static bool8 ArePlayerFollowMonsAllowed()
{
    if(Rogue_AreWildMonEnabled())
    {
        return FALSE;
    }

    return TRUE;
}

static void EnsureObjectIsRemoved(u8 localObjectId)
{
    struct SyncedObjectEventInfo syncInfo = {0};
    syncInfo.localId = localObjectId;
    ProcessSyncedObjectEvent(&syncInfo);
}

static void ObservePlayerState(u8 playerId, struct RogueNetPlayer* player)
{
    bool8 isPlayerActive = FALSE;
    bool8 isFollowerActive = FALSE;
    u8 playerObjectId = OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 0;
    u8 followerObjectId = OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 1;

    if(gRogueMultiplayer->playerProfiles[playerId].isActive)
    {
        isPlayerActive = TRUE;

        if(player->partnerMon != SPECIES_NONE && !(player->playerFlags & NET_PLAYER_STATE_FLAG_RIDING) && ArePlayerFollowMonsAllowed())
        {
            // Only display follower if not sat on top of it
            isFollowerActive = !(player->playerPos.x == player->partnerPos.x && player->playerPos.y == player->partnerPos.y);
        }
    }

    // Don't display followers on adventure paths screetn
    if(isFollowerActive && Rogue_IsRunActive())
    {
        // Don't display followers on adventure paths screen or on screen with wild mons
        if(RogueAdv_IsViewingPath() || Rogue_AreWildMonEnabled())
        {
            isFollowerActive = FALSE;
        }
    }

    if(isPlayerActive)
    {
        u8 objectEventId;
        struct SyncedObjectEventInfo syncInfo = {0};

        syncInfo.localId = playerObjectId;
        syncInfo.pos.x = player->playerPos.x;
        syncInfo.pos.y = player->playerPos.y;
        syncInfo.elevation = player->currentElevation;
        syncInfo.facingDirection = player->facingDirection;
        syncInfo.mapGroup = player->mapGroup;
        syncInfo.mapNum = player->mapNum;
        syncInfo.adventureTileNum = player->adventureTileNum;
        syncInfo.adventureDifficulty = player->adventureDifficulty;
        syncInfo.movementBuffer = player->movementBuffer;
        syncInfo.movementBufferHead = player->movementBufferHead;
        syncInfo.movementBufferReadOffset = 0;

        if(player->playerFlags & NET_PLAYER_STATE_FLAG_RIDING)
            syncInfo.gfxId = OBJ_EVENT_GFX_NET_PLAYER_RIDING;
        else
            syncInfo.gfxId = OBJ_EVENT_GFX_NET_PLAYER_NORMAL;

        objectEventId = ProcessSyncedObjectEvent(&syncInfo);

        if(objectEventId != OBJECT_EVENTS_COUNT && (player->playerFlags & NET_PLAYER_STATE_FLAG_RIDING))
        {
            Rogue_SetupRideObject(1 + playerId, objectEventId, player->partnerMon, (player->playerFlags & NET_PLAYER_STATE_FLAG_FLYING) != 0);
        }
        else
        {
            Rogue_ClearRideObject(1 + playerId);
        }
    }
    else
    {
        EnsureObjectIsRemoved(playerObjectId);
        Rogue_ClearRideObject(1 + playerId);
    }

    if(isFollowerActive)
    {
        struct SyncedObjectEventInfo syncInfo = {0};

        syncInfo.localId = followerObjectId;
        syncInfo.pos.x = player->partnerPos.x;
        syncInfo.pos.y = player->partnerPos.y;
        syncInfo.elevation = player->currentElevation;
        syncInfo.facingDirection = player->partnerFacingDirection;
        syncInfo.gfxId = OBJ_EVENT_GFX_MP_FOLLOW_MON;
        syncInfo.mapGroup = player->mapGroup;
        syncInfo.mapNum = player->mapNum;
        syncInfo.adventureTileNum = player->adventureTileNum;
        syncInfo.adventureDifficulty = player->adventureDifficulty;
        syncInfo.movementBuffer = player->movementBuffer;
        syncInfo.movementBufferHead = player->movementBufferHead;
        syncInfo.movementBufferReadOffset = 1; // skip the most recent movement, as that's where the player is

        if(player->partnerMon != FollowMon_GetGraphics(OBJ_EVENT_GFX_MP_FOLLOW_MON - OBJ_EVENT_GFX_FOLLOW_MON_0))
        {
            FollowMon_SetGraphicsRaw(OBJ_EVENT_GFX_MP_FOLLOW_MON - OBJ_EVENT_GFX_FOLLOW_MON_0, player->partnerMon);

            // Delete object and recreate
            EnsureObjectIsRemoved(followerObjectId);
        }

        ProcessSyncedObjectEvent(&syncInfo);
    }
    else
    {
        EnsureObjectIsRemoved(followerObjectId);
    }
}

static void UpdateLocalPlayerState(u8 playerId)
{
    AGB_ASSERT(gRogueMultiplayer != NULL);

    if(playerId == gRogueMultiplayer->localPlayerId)
    {
        START_TIMER(ROGUE_MP_UPDATE_LOCAL_PLAYER);
        WritePlayerState(&gRogueMultiplayer->playerState[playerId]);
        STOP_TIMER(ROGUE_MP_UPDATE_LOCAL_PLAYER);
    }
    else
    {
        START_TIMER(ROGUE_MP_UPDATE_REMOTE_PLAYER);
        ObservePlayerState(playerId, &gRogueMultiplayer->playerState[playerId]);
        STOP_TIMER(ROGUE_MP_UPDATE_REMOTE_PLAYER);
    }
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
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else if(RogueMP_IsActive())
    {
        if(RogueMP_IsHost())
        {
            DebugPrint("Host created successfully");
            // Has connected
            gSpecialVar_Result = TRUE;
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
        else if(!RogueMP_IsConnecting())
        {
            DebugPrint("Client connected successfully");
            // Has connected
            gSpecialVar_Result = TRUE;
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
    }
    // TODO - Error handling for if handshake wasn't accepted
    //else if(!RogueMP_IsConnecting())
    //{
    //    DebugPrint("Connection aborted...");
    //    // TODO - store some infor
    //    gSpecialVar_Result = FALSE;
    //    ScriptContext_Enable();
    //    DestroyTask(taskId);
    //}
}

static bool8 ShouldSyncObjectBeVisible(struct SyncedObjectEventInfo* syncInfo)
{
    if(syncInfo->gfxId == 0)
        return FALSE;

    if(
        syncInfo->mapGroup == gSaveBlock1Ptr->location.mapGroup && 
        syncInfo->mapNum == gSaveBlock1Ptr->location.mapNum &&
        syncInfo->adventureTileNum == GetLocalAdventureTileNum() &&
        syncInfo->adventureDifficulty == GetLocalAdventureDifficulty()
    )
    {
        s16 playerX, playerY, xDist, yDist;
        PlayerGetDestCoords(&playerX, &playerY);

        xDist = abs(syncInfo->pos.x - playerX);
        yDist = abs(syncInfo->pos.y - playerY);

        if(xDist <= 10 && yDist <= 7)
            return TRUE;
    }

    return FALSE;
}

bool8 IsSafeToSpawnObjectEvents();

static u8 ProcessSyncedObjectEvent(struct SyncedObjectEventInfo* syncInfo)
{
    u8 objectEventId = GetSpecialObjectEventIdByLocalId(syncInfo->localId);

    if(ShouldSyncObjectBeVisible(syncInfo))
    {
        if(objectEventId == OBJECT_EVENTS_COUNT && IsSafeToSpawnObjectEvents())
        {
            objectEventId = SpawnSpecialObjectEventParameterized(
                syncInfo->gfxId,
                MOVEMENT_TYPE_NONE,
                syncInfo->localId,
                syncInfo->pos.x,
                syncInfo->pos.y,
                syncInfo->elevation
            );
        }

        if(objectEventId != OBJECT_EVENTS_COUNT)
        {
            ProcessSyncedObjectMovement(syncInfo, &gObjectEvents[objectEventId]);
        }
    }
    else
    {
        if(objectEventId != OBJECT_EVENTS_COUNT)
        {
            RemoveObjectEvent(&gObjectEvents[objectEventId]);
            objectEventId = OBJECT_EVENTS_COUNT;
        }
    }

    return objectEventId;
}

static void ProcessSyncedObjectMovement(struct SyncedObjectEventInfo* syncInfo, struct ObjectEvent* objectEvent)
{
    if(syncInfo->gfxId != objectEvent->graphicsId)
    {
        // This doesn't work too well :/
        //ObjectEventSetGraphicsId(objectEvent, syncInfo->gfxId);

        // Delete object and reprocess on path to make sure there is no delay
        EnsureObjectIsRemoved(syncInfo->localId);
        ProcessSyncedObjectEvent(syncInfo);
        return;
    }

    // Wait for current movement to finish
    if(ObjectEventCheckHeldMovementStatus(objectEvent))
    {
        if(objectEvent->currentCoords.x == syncInfo->pos.x && objectEvent->currentCoords.y == syncInfo->pos.y)
        {
            if(objectEvent->facingDirection != syncInfo->facingDirection)
                ObjectEventTurnByLocalIdAndMap(syncInfo->localId, syncInfo->mapNum, syncInfo->mapGroup, syncInfo->facingDirection);
        }
        else
        {
            u8 i, currentIdx;
            ObjectEventClearHeldMovement(objectEvent);

            // Iterate backwards
            for(i = syncInfo->movementBufferReadOffset; i < NET_PLAYER_MOVEMENT_BUFFER_SIZE; ++i)
            {
                // Count backward from the head until we find the correct index
                currentIdx = (NET_PLAYER_MOVEMENT_BUFFER_SIZE + syncInfo->movementBufferHead - i) % NET_PLAYER_MOVEMENT_BUFFER_SIZE;

                if(syncInfo->movementBuffer[currentIdx].pos.x == objectEvent->currentCoords.x && syncInfo->movementBuffer[currentIdx].pos.y == objectEvent->currentCoords.y)
                {
                    u8 movementAction = syncInfo->movementBuffer[currentIdx].movementAction;

                    // If we're not the player, we can't do any sprinting movement actions
                    //if(syncInfo->gfxId != OBJ_EVENT_GFX_NET_PLAYER_NORMAL) // note: we're just gonna not show any sprinting anims for now as it freezes on the final frame which is yucky
                    {
                        if(movementAction >= MOVEMENT_ACTION_PLAYER_RUN_DOWN && movementAction <= MOVEMENT_ACTION_PLAYER_RUN_RIGHT)
                        {
                            // Convert to walk fast actions
                            movementAction = MOVEMENT_ACTION_WALK_FAST_DOWN + (movementAction - MOVEMENT_ACTION_PLAYER_RUN_DOWN);
                        }
                    }

                    ObjectEventSetHeldMovement(objectEvent, movementAction);
                    return;
                }
            }

            // If we got here, it means our position lies outside of the movement buffer, so TP to the correct location
            MoveObjectEventToMapCoords(
                objectEvent, 
                syncInfo->pos.x, 
                syncInfo->pos.y
            );
        }
    }
}

// Remote Commands
//

// Utils
//

// Modified helpers taken from Emerald Expansion
#define MEMBERS(...) VARARG_8(MEMBERS_, __VA_ARGS__)
#define MEMBERS_0()
#define MEMBERS_1(a) a;
#define MEMBERS_2(a, b) a; b;
#define MEMBERS_3(a, b, c) a; b; c;
#define MEMBERS_4(a, b, c, d) a; b; c; d;
#define MEMBERS_5(a, b, c, d, e) a; b; c; d; e;
#define MEMBERS_6(a, b, c, d, e, f) a; b; c; d; e; f;
#define MEMBERS_7(a, b, c, d, e, f, g) a; b; c; d; e; f; g;
#define MEMBERS_8(a, b, c, d, e, f, g, h) a; b; c; d; e; f; g; h;

#define MP_CMD_ARGS_(buffer, varToken, ...) struct PACKED { u16 cmdId; MEMBERS(__VA_ARGS__) } varToken = (void *)buffer
#define LOCAL_SEND_ARGS(...) MP_CMD_ARGS_(GetLocalPlayer()->cmdSendBuffer, * localCmdArgs, __VA_ARGS__)
#define LOCAL_RESP_ARGS(...) MP_CMD_ARGS_(GetLocalPlayer()->cmdRespBuffer, * localCmdArgs, __VA_ARGS__)
#define REMOTE_SEND_ARGS(...) MP_CMD_ARGS_(GetRemotePlayer()->cmdSendBuffer, const * const remoteCmdArgs UNUSED, __VA_ARGS__)
#define REMOTE_RESP_ARGS(...) MP_CMD_ARGS_(GetRemotePlayer()->cmdRespBuffer, const * const remoteCmdArgs UNUSED, __VA_ARGS__)

//
// Local client populate's it's own send cmd
// Remote client then processes that; there may be multiple rounds of back and forth per each cmd
// 
// The remote is expected to populate resp cmd and buffer and then in response to that local can choose to populate send cmd and buffer to queue another round
// e.g. this is needed to send large structs
//

typedef void (*MpCmdCallback)();

static void Send_Cmd_RequestMon();
static void Resp_Cmd_RequestMon();
static void Send_Cmd_RequestTalkPlayer();
static void Resp_Cmd_RequestTalkPlayer();

static const MpCmdCallback sMpSendCmdCallbacks[MP_CMD_COUNT] = 
{
    [MP_CMD_REQUEST_MON] = Send_Cmd_RequestMon,
    [MP_CMD_REQUEST_TALK_TO_PLAYER] = Send_Cmd_RequestTalkPlayer,
};

static const MpCmdCallback sMpRespCmdCallbacks[MP_CMD_COUNT] = 
{
    [MP_CMD_REQUEST_MON] = Resp_Cmd_RequestMon,
    [MP_CMD_REQUEST_TALK_TO_PLAYER] = Resp_Cmd_RequestTalkPlayer,
};

// Helpers
//
static void ClearSendCmd(u8 result)
{
    LOCAL_SEND_ARGS();
    gRogueLocalMP.recentSendCmd = localCmdArgs->cmdId;
    gRogueLocalMP.recentResult = result;

    localCmdArgs->cmdId = MP_CMD_NONE;
}

// Main logic
//

static void ProcessPlayerCommands()
{
    // Setup to assume 2 players
    struct RogueNetPlayer* localPlayer = GetLocalPlayer();
    struct RogueNetPlayer* remotePlayer = GetRemotePlayer();
    AGB_ASSERT(gRogueMultiplayer != NULL);

    // Can't do anything if the other player isn't active
    // TODO - Need to gracefully fail any queued interactions
    if(!RogueMP_IsRemotePlayerActive())
        return;

    UpdateLocalPlayerStatus();

    // Process incoming request
    {
        REMOTE_SEND_ARGS();

        if(gRogueLocalMP.lastProcessedRemoteSendCmd != remoteCmdArgs->cmdId)
        {
            // Zero the response buffer
            gRogueLocalMP.lastProcessedRemoteSendCmd = remoteCmdArgs->cmdId;
            memset(localPlayer->cmdRespBuffer, 0, sizeof(localPlayer->cmdRespBuffer));
        }

        // Run send callback
        if(remoteCmdArgs->cmdId != MP_CMD_NONE)
        {
            AGB_ASSERT(sMpSendCmdCallbacks[remoteCmdArgs->cmdId] != NULL);

            if(sMpSendCmdCallbacks[remoteCmdArgs->cmdId])
                sMpSendCmdCallbacks[remoteCmdArgs->cmdId]();
        }
    }

    // Process response to our request
    {
        LOCAL_SEND_ARGS();
        REMOTE_RESP_ARGS();

        if(localCmdArgs->cmdId != MP_CMD_NONE && localCmdArgs->cmdId == remoteCmdArgs->cmdId)
        {
            AGB_ASSERT(sMpRespCmdCallbacks[localCmdArgs->cmdId] != NULL);

            // Run resp callback
            if(sMpRespCmdCallbacks[localCmdArgs->cmdId])
                sMpRespCmdCallbacks[localCmdArgs->cmdId]();
        }
    }
}

#define isIncoming data[0]
#define canCancel data[1]

u8 RogueMP_WaitForOutgoingCommand(bool8 allowCancel)
{
    u8 taskId = FindTaskIdByFunc(Task_WaitForSendFinish);
    if (taskId == TASK_NONE)
    {
        taskId = CreateTask(Task_WaitForSendFinish, 80);
        gTasks[taskId].isIncoming = FALSE;
        gTasks[taskId].canCancel = allowCancel;
    }

    return taskId;
}

u8 RogueMP_WaitForIncomingCommand(bool8 allowCancel)
{
    u8 taskId = FindTaskIdByFunc(Task_WaitForSendFinish);
    if (taskId == TASK_NONE)
    {
        taskId = CreateTask(Task_WaitForSendFinish, 80);
        gTasks[taskId].isIncoming = TRUE;
        gTasks[taskId].canCancel = allowCancel;
    }

    return taskId;
}

static void Task_WaitForSendFinish(u8 taskId)
{
    if(RogueMP_IsRemotePlayerActive())
    {
        if(gTasks[taskId].isIncoming)
        {
            REMOTE_SEND_ARGS();
            if(remoteCmdArgs->cmdId != MP_CMD_NONE)
            {
                gSpecialVar_Result = TRUE;
                ScriptContext_Enable();
                DestroyTask(taskId);
            }
            else if(gTasks[taskId].canCancel && JOY_NEW(B_BUTTON))
            {
                ClearSendCmd(0);
                gSpecialVar_Result = FALSE;
                ScriptContext_Enable();
                DestroyTask(taskId);
            }
        }
        else
        {
            LOCAL_SEND_ARGS();
            if(localCmdArgs->cmdId == MP_CMD_NONE)
            {
                gSpecialVar_Result = gRogueLocalMP.recentResult;
                ScriptContext_Enable();
                DestroyTask(taskId);
            }
            else if(gTasks[taskId].canCancel && JOY_NEW(B_BUTTON))
            {
                ClearSendCmd(0);
                gSpecialVar_Result = FALSE;
                ScriptContext_Enable();
                DestroyTask(taskId);
            }
        }
    }
    else
    {
        gSpecialVar_Result = FALSE;
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

#undef isIncoming
#undef canCancel

bool8 RogueMP_IsWaitingForCommandToFinish()
{
    if(RogueMP_IsRemotePlayerActive())
    {
        LOCAL_SEND_ARGS();
        if(localCmdArgs->cmdId == MP_CMD_NONE)
        {
            return FALSE;
        }

        return TRUE;
    }
    else
        return FALSE;
}

// MP_CMD_REQUEST_MON
//

void RogueMP_Cmd_RequestPartyMon(u8 fromSlot, u8 toSlot)
{
    LOCAL_SEND_ARGS(u8 fromSlot : 4, u8 toSlot : 4, u8 offset, u8 data[NET_CMD_UNRESERVED_BUFFER_SIZE - 2]);

    AGB_ASSERT(localCmdArgs->cmdId == MP_CMD_NONE);

    // Create the initial request
    localCmdArgs->cmdId = MP_CMD_REQUEST_MON;
    localCmdArgs->fromSlot = fromSlot;
    localCmdArgs->toSlot = toSlot;
    localCmdArgs->offset = 0;

    MpLogf("RequestMon start from:%d to:%d size:%d", fromSlot, toSlot, sizeof(struct Pokemon) * (toSlot - fromSlot + 1));
}

static void Send_Cmd_RequestMon()
{
    REMOTE_SEND_ARGS(u8 fromSlot : 4, u8 toSlot : 4, u8 offset, u8 data[NET_CMD_UNRESERVED_BUFFER_SIZE - 2]);
    LOCAL_RESP_ARGS(u8 fromSlot : 4, u8 toSlot : 4, u8 offset, u8 data[NET_CMD_UNRESERVED_BUFFER_SIZE - 2]);

    if(
        localCmdArgs->cmdId != remoteCmdArgs->cmdId ||
        localCmdArgs->fromSlot != remoteCmdArgs->fromSlot ||
        localCmdArgs->toSlot != remoteCmdArgs->toSlot ||
        localCmdArgs->offset != remoteCmdArgs->offset
    )
    {
        // Requesting new chunk of the struct
        u8 fromSlot = min(remoteCmdArgs->fromSlot, PARTY_SIZE - 1);
        u8 toSlot = min(remoteCmdArgs->toSlot, PARTY_SIZE - 1);
        size_t totalSize = sizeof(struct Pokemon) * (toSlot - fromSlot + 1);

        size_t offset = min((size_t)remoteCmdArgs->offset * sizeof(localCmdArgs->data), totalSize);
        u8* src = (u8*)&gPlayerParty[fromSlot];
        size_t copySize = min(totalSize - offset, sizeof(localCmdArgs->data));

        MpLogf("RequestMon send from:%d to:%d offset:%d, size:%d", fromSlot, toSlot, offset, copySize);

        localCmdArgs->cmdId = remoteCmdArgs->cmdId;
        localCmdArgs->fromSlot = fromSlot;
        localCmdArgs->toSlot = toSlot;
        localCmdArgs->offset = remoteCmdArgs->offset;
        memcpy(localCmdArgs->data, &src[offset], copySize);
    }
}

static void Resp_Cmd_RequestMon()
{
    LOCAL_SEND_ARGS(u8 fromSlot : 4, u8 toSlot : 4, u8 offset, u8 data[NET_CMD_UNRESERVED_BUFFER_SIZE - 2]);
    REMOTE_RESP_ARGS(u8 fromSlot : 4, u8 toSlot : 4, u8 offset, u8 data[NET_CMD_UNRESERVED_BUFFER_SIZE - 2]);

    if(
        localCmdArgs->cmdId == remoteCmdArgs->cmdId &&
        localCmdArgs->fromSlot == remoteCmdArgs->fromSlot &&
        localCmdArgs->toSlot == remoteCmdArgs->toSlot &&
        localCmdArgs->offset == remoteCmdArgs->offset
    )
    {
        // We have recieved the response so queue up the next amount
        u8 fromSlot = min(remoteCmdArgs->fromSlot, PARTY_SIZE - 1);
        u8 toSlot = min(remoteCmdArgs->toSlot, PARTY_SIZE - 1);
        size_t totalSize = sizeof(struct Pokemon) * (toSlot - fromSlot + 1);

        size_t offset = min((size_t)localCmdArgs->offset * sizeof(localCmdArgs->data), totalSize);
        u8* dest = (u8*)&gEnemyParty[fromSlot];
        size_t copySize = min(totalSize - offset, sizeof(localCmdArgs->data));

        MpLogf("RequestMon resp from:%d to:%d offset:%d, size:%d", fromSlot, toSlot, offset, copySize);

        if(copySize != 0)
            memcpy(&dest[offset], remoteCmdArgs->data, copySize);

        if(copySize < sizeof(localCmdArgs->data))
        {
            // Finished
            MpLog("RequestMon complete!");
            ClearSendCmd(TRUE);

            // TEMP DEBUG
            //{
            //    u8 i;
//
            //    for(i = fromSlot; i <= toSlot; ++i)
            //    {
            //        CopyMon(&gPlayerParty[i], &gEnemyParty[i], sizeof(struct Pokemon));
            //    }
            //    CalculatePlayerPartyCount();
            //}
            //GiveTradedMonToPlayer(&gEnemyParty[slot]);
        }
        else
        {
            // Request next chunk
            localCmdArgs->offset++;
        }
    }
}

// MP_CMD_REQUEST_TALK_TO_PLAYER
//

void RogueMP_Cmd_RequestTalkToPlayer()
{
    LOCAL_SEND_ARGS();

    AGB_ASSERT(localCmdArgs->cmdId == MP_CMD_NONE);

    // Create the initial request
    localCmdArgs->cmdId = MP_CMD_REQUEST_TALK_TO_PLAYER;

    MpLog("RequestTalkToPlayer start");
}

static void Send_Cmd_RequestTalkPlayer()
{
    LOCAL_RESP_ARGS();
    REMOTE_SEND_ARGS();

    if(localCmdArgs->cmdId == remoteCmdArgs->cmdId)
    {
        // We've begun talking, so now we can coordinate using other commands
        MpLog("RequestTalkToPlayer end");
        ClearSendCmd(TRUE);
    }
}

static void Resp_Cmd_RequestTalkPlayer()
{
    LOCAL_SEND_ARGS();
    REMOTE_RESP_ARGS();

    if(localCmdArgs->cmdId == remoteCmdArgs->cmdId)
    {
        // We've begun talking, so now we can coordinate using other commands
        MpLog("RequestTalkToPlayer end");
        ClearSendCmd(TRUE);
    }
}

bool8 RogueMP_HasTalkRequestPending()
{
    if(RogueMP_IsRemotePlayerActive())
    {
        LOCAL_RESP_ARGS();
        REMOTE_SEND_ARGS();
        return remoteCmdArgs->cmdId == MP_CMD_REQUEST_TALK_TO_PLAYER && localCmdArgs->cmdId != MP_CMD_REQUEST_TALK_TO_PLAYER;
    }

    return FALSE;
}

void RogueMP_NotifyAcceptTalkRequest()
{
    LOCAL_RESP_ARGS();

    localCmdArgs->cmdId = MP_CMD_REQUEST_TALK_TO_PLAYER;

    MpLog("RequestTalkToPlayer accepted");
}
