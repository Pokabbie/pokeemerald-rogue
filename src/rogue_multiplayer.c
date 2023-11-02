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

#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_multiplayer.h"
#include "rogue_player_customisation.h"
#include "rogue_ridemon.h"
#include "rogue_save.h"

#define NET_STATE_NONE              0
#define NET_STATE_ACTIVE            (1 << 0)
#define NET_STATE_HOST              (2 << 0)


#define NET_PLAYER_STATE_FLAG_NONE      0
#define NET_PLAYER_STATE_FLAG_RIDING    (1 << 0)


struct SyncedObjectEventInfo
{
    struct RogueNetPlayerMovement* movementBuffer;
    struct Coords16 pos;
    u16 gfxId;
    s8 mapNum;
    s8 mapGroup;
    u8 localId;
    u8 movementBufferHead;
    u8 movementBufferReadOffset;
    u8 facingDirection : 4;
    u8 elevation : 4;
};

EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer = NULL;
EWRAM_DATA struct RogueNetMultiplayer gTEMPNetMultiplayer; // temporary memory holder which should be swaped out for dynamic alloc really

static void Host_HandleHandshakeRequest();
static void Host_UpdateGameState();
static void Client_SetupHandshakeRequest();
static void Client_HandleHandshakeResponse();
static void UpdateLocalPlayerState(u8 playerId);
static void Task_WaitForConnection(u8 taskId);

static u8 ProcessSyncedObjectEvent(struct SyncedObjectEventInfo* syncInfo);
static void ProcessSyncedObjectMovement(struct SyncedObjectEventInfo* syncInfo, struct ObjectEvent* objectEvent);

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
}

void RogueMP_OverworldCB()
{
    if(RogueMP_IsActive())
    {
        // To split up the processing, only process 1 player per frame
        u8 playerId = gRogueMultiplayer->localCounter++ % NET_PLAYER_CAPACITY;
        UpdateLocalPlayerState(playerId);
        
        //u8 i;
//
        //for(i = 0; i < NET_PLAYER_CAPACITY; ++i)
        //    UpdateLocalPlayerState(i);
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
        // TODO - Only need to do this if there is a change really
        memcpy(&gRogueMultiplayer->gameState.hubMap, &gRogueSaveBlock->hubMap, sizeof(gRogueSaveBlock->hubMap));
    }
    else
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

        player->playerFlags = NET_PLAYER_STATE_FLAG_NONE;
        if(TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING))
        {
            player->playerFlags |= NET_PLAYER_STATE_FLAG_RIDING;
            player->partnerMon = Rogue_GetRideMonSpeciesGfx(0); // 0 is always local player
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
            isFollowerActive = TRUE;
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
        syncInfo.movementBuffer = player->movementBuffer;
        syncInfo.movementBufferHead = player->movementBufferHead;
        syncInfo.movementBufferReadOffset = 0;

        if(player->playerFlags & NET_PLAYER_STATE_FLAG_RIDING)
            syncInfo.gfxId = OBJ_EVENT_GFX_MAY_RIDING; // TODO - need to have net outfits that have riding gfx too
        else
            syncInfo.gfxId = OBJ_EVENT_GFX_BUG_CATCHER;

        objectEventId = ProcessSyncedObjectEvent(&syncInfo);

        if(objectEventId != OBJECT_EVENTS_COUNT)
        {
            if(player->playerFlags & NET_PLAYER_STATE_FLAG_RIDING)
            {
                Rogue_SetupRideObject(1 + playerId, objectEventId, player->partnerMon);
            }
        }
        else
        {
            Rogue_ClearRideObject(1 + playerId);
        }
    }
    else
    {
        EnsureObjectIsRemoved(playerObjectId);
    }

    if(isFollowerActive)
    {
        struct SyncedObjectEventInfo syncInfo = {0};

        syncInfo.localId = followerObjectId;
        syncInfo.pos.x = player->partnerPos.x;
        syncInfo.pos.y = player->partnerPos.y;
        syncInfo.elevation = player->currentElevation;
        syncInfo.facingDirection = player->partnerFacingDirection;
        syncInfo.gfxId = OBJ_EVENT_GFX_FOLLOW_MON_A + playerId;
        syncInfo.mapGroup = player->mapGroup;
        syncInfo.mapNum = player->mapNum;
        syncInfo.movementBuffer = player->movementBuffer;
        syncInfo.movementBufferHead = player->movementBufferHead;
        syncInfo.movementBufferReadOffset = 1; // skip the most recent movement, as that's where the player is

        if(player->partnerMon != FollowMon_GetGraphics(0xA + playerId))
        {
            if(player->partnerMon >= FOLLOWMON_SHINY_OFFSET)
                FollowMon_SetGraphics(0xA + playerId, player->partnerMon - FOLLOWMON_SHINY_OFFSET, TRUE);
            else
                FollowMon_SetGraphics(0xA + playerId, player->partnerMon, FALSE);

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
        WritePlayerState(&gRogueMultiplayer->playerState[playerId]);
    else
        ObservePlayerState(playerId, &gRogueMultiplayer->playerState[playerId]);
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

static bool8 ShouldSyncObjectBeVisible(struct SyncedObjectEventInfo* syncInfo)
{
    if(syncInfo->gfxId == 0)
        return FALSE;

    if(syncInfo->mapGroup == gSaveBlock1Ptr->location.mapGroup && syncInfo->mapNum == gSaveBlock1Ptr->location.mapNum)
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

static u8 ProcessSyncedObjectEvent(struct SyncedObjectEventInfo* syncInfo)
{
    u8 objectEventId = GetSpecialObjectEventIdByLocalId(syncInfo->localId);

    if(ShouldSyncObjectBeVisible(syncInfo))
    {
        if(objectEventId == OBJECT_EVENTS_COUNT)
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

        ProcessSyncedObjectMovement(syncInfo, &gObjectEvents[objectEventId]);
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
                    ObjectEventSetHeldMovement(objectEvent, syncInfo->movementBuffer[currentIdx].movementAction);
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