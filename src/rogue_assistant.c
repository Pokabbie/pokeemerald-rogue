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
#include "rogue_query.h"

// Constants
//

#define IN_COMM_BUFFER_SIZE 16
#define OUT_COMM_BUFFER_SIZE 8

#define NET_PLAYER_CAPACITY 4

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

// Global states
//

struct NetPlayerProfile
{
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
    u8 flags;
};

struct NetPlayerState
{
    struct Coords16 pos;
    struct Coords16 partnerPos;
    u16 facingDirection;
    u16 partnerFacingDirection;
    u16 partnerMon;
    s8 mapGroup;
    s8 mapNum;
};

struct RogueAssistantState
{
    u16 assistantState;
    u16 assistantSubstate;
    u16 requestState;
    u8 inCommBuffer[16];
    u8 outCommBuffer[32];
    struct NetPlayerProfile netPlayerProfile[NET_PLAYER_CAPACITY];
    struct NetPlayerState netPlayerState[NET_PLAYER_CAPACITY];
};

EWRAM_DATA struct RogueAssistantState gRogueAssistantState;

const struct RogueAssistantHeader gRogueAssistantHeader =
{
    .inCommCapacity = sizeof(gRogueAssistantState.inCommBuffer),
    .outCommCapacity = sizeof(gRogueAssistantState.outCommBuffer),
    .inCommBuffer = gRogueAssistantState.inCommBuffer,
    .outCommBuffer = gRogueAssistantState.outCommBuffer
};


static void Task_ConnectMultiplayer(u8 taskId);


// Read/Write utils
//
typedef u8 buffer_offset_t;

static u8 Read8(buffer_offset_t* offset, u8* buffer, size_t capacity);
static u16 Read16(buffer_offset_t* offset, u8* buffer, size_t capacity);
static u32 Read32(buffer_offset_t* offset, u8* buffer, size_t capacity);

static void Write8(buffer_offset_t* offset, u8 value, u8* buffer, size_t capacity);
static void Write16(buffer_offset_t* offset, u16 value, u8* buffer, size_t capacity);
static void Write32(buffer_offset_t* offset, u32 value, u8* buffer, size_t capacity);

#define READ_INPUT_8() Read8(&inputPos, gRogueAssistantHeader.inCommBuffer, gRogueAssistantHeader.inCommCapacity)
#define READ_INPUT_16() Read16(&inputPos, gRogueAssistantHeader.inCommBuffer, gRogueAssistantHeader.inCommCapacity)
#define READ_INPUT_32() Read32(&inputPos, gRogueAssistantHeader.inCommBuffer, gRogueAssistantHeader.inCommCapacity)

#define READ_OUTPUT_8() Read8(&outputPos, gRogueAssistantHeader.outCommBuffer, gRogueAssistantHeader.outCommCapacity)
#define READ_OUTPUT_16() Read16(&outputPos, gRogueAssistantHeader.outCommBuffer, gRogueAssistantHeader.outCommCapacity)
#define READ_OUTPUT_32() Read32(&outputPos, gRogueAssistantHeader.outCommBuffer, gRogueAssistantHeader.outCommCapacity)

#define WRITE_OUTPUT_8(value) Write8(&outputPos, value, gRogueAssistantHeader.outCommBuffer, gRogueAssistantHeader.outCommCapacity)
#define WRITE_OUTPUT_16(value) Write16(&outputPos, value, gRogueAssistantHeader.outCommBuffer, gRogueAssistantHeader.outCommCapacity)
#define WRITE_OUTPUT_32(value) Write32(&outputPos, value, gRogueAssistantHeader.outCommBuffer, gRogueAssistantHeader.outCommCapacity)



// Declerations
//

typedef void (*CommandCallback)(buffer_offset_t, buffer_offset_t);

static void NetPlayerUpdate(u8 playerId, struct NetPlayerProfile* playerProfile, struct NetPlayerState* playerState);

static bool8 CommCmd_ProcessNext();
static void CommCmd_Echo(buffer_offset_t inputPos, buffer_offset_t outputPos);
static void CommCmd_ReadConstant(buffer_offset_t inputPos, buffer_offset_t outputPos);
static void CommCmd_BeginMultiplayerHost(buffer_offset_t inputPos, buffer_offset_t outputPos);
static void CommCmd_BeginMultiplayerClient(buffer_offset_t inputPos, buffer_offset_t outputPos);
static void CommCmd_EndMultiplayer(buffer_offset_t inputPos, buffer_offset_t outputPos);
static void CommCmd_GetSpeciesName(buffer_offset_t inputPos, buffer_offset_t outputPos);
static void CommCmd_GetItemName(buffer_offset_t inputPos, buffer_offset_t outputPos);

static const CommandCallback sCommCommands[] = 
{
    CommCmd_Echo,
    CommCmd_ReadConstant,
    CommCmd_BeginMultiplayerHost,
    CommCmd_BeginMultiplayerClient,
    CommCmd_EndMultiplayer,
    CommCmd_GetSpeciesName,
    CommCmd_GetItemName,
};


// Methods
//

void Rogue_AssistantInit()
{
    PUSH_ASSISTANT_STATE(NONE);
    Rogue_UpdateAssistantRequestState(REQUEST_STATE_NONE);
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

void Rogue_AssistantMainCB()
{
    CommCmd_ProcessNext();
}

static bool8 IsNetPlayerActive(u8 id)
{
    return (gRogueAssistantState.netPlayerProfile[id].flags & NETPLAYER_FLAGS_ACTIVE) != 0;
}

void Rogue_AssistantOverworldCB()
{
    if(Rogue_IsNetMultiplayerActive())
    {
        // Populate current player state
        {
            struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];

            gRogueAssistantState.netPlayerState[0].pos = gSaveBlock1Ptr->pos;
            gRogueAssistantState.netPlayerState[0].facingDirection = player->facingDirection;
            gRogueAssistantState.netPlayerState[0].mapGroup = gSaveBlock1Ptr->location.mapGroup;
            gRogueAssistantState.netPlayerState[0].mapNum = gSaveBlock1Ptr->location.mapNum;

            if(FollowMon_IsPartnerMonActive())
            {
                struct ObjectEvent* follower = &gObjectEvents[gSaveBlock2Ptr->follower.objId];

                gRogueAssistantState.netPlayerState[0].partnerMon = FollowMon_GetPartnerFollowSpecies();
                gRogueAssistantState.netPlayerState[0].partnerPos = follower->currentCoords;
                gRogueAssistantState.netPlayerState[0].partnerFacingDirection = follower->facingDirection;
            }
            else
            {
                gRogueAssistantState.netPlayerState[0].partnerMon = 0;
            }
        }

        // Do external net update, if needed
        {
            u8 i;
            for(i = 1; i < NET_PLAYER_CAPACITY; ++i)
            {
                NetPlayerUpdate(i, &gRogueAssistantState.netPlayerProfile[i], &gRogueAssistantState.netPlayerState[i]);
            }
        }
    }
}


// Multiplayer
//

struct SyncObjectEventInfo
{
    u8 localId;
    u16 gfxId;
    u16 facingDirection;
    s16 mapX;
    s16 mapY;
    s8 mapNum;
    s8 mapGroup;
};

static void SyncObjectEvent(struct SyncObjectEventInfo objectInfo)
{
    bool8 shouldBeVisible = (objectInfo.gfxId != 0) && (objectInfo.mapGroup == gSaveBlock1Ptr->location.mapGroup && objectInfo.mapNum == gSaveBlock1Ptr->location.mapNum);
    u8 objectId = GetSpecialObjectEventIdByLocalId(objectInfo.localId);
    s16 xDist = abs(objectInfo.mapX - gSaveBlock1Ptr->pos.x - MAP_OFFSET);
    s16 yDist = abs(objectInfo.mapY - gSaveBlock1Ptr->pos.y - MAP_OFFSET);

    if(shouldBeVisible && xDist <= 8 && yDist <= 5)
    {
        shouldBeVisible = TRUE;
    }

    if(shouldBeVisible)
    {
        if(objectId == OBJECT_EVENTS_COUNT && IsSafeToSpawnObjectEvents())
        {
            objectId = SpawnSpecialObjectEventParameterized(
                objectInfo.gfxId,
                MOVEMENT_TYPE_NONE,
                objectInfo.localId,
                objectInfo.mapX,
                objectInfo.mapY,
                MapGridGetElevationAt(objectInfo.mapX, objectInfo.mapY)
            );
        }

        if(objectId != OBJECT_EVENTS_COUNT)
        {
            s16 totalDist;
            u8 heldMovement = MOVEMENT_ACTION_NONE;
            struct ObjectEvent* object = &gObjectEvents[objectId];
            s16 xDiff = objectInfo.mapX - object->currentCoords.x;
            s16 yDiff = objectInfo.mapY - object->currentCoords.y;

            xDist = abs(xDiff);
            yDist = abs(yDiff);
            totalDist = xDist + yDist;

            // Close enough to animate
            if(totalDist < 12)
            {
                u8 heldMovement = MOVEMENT_ACTION_NONE;
                u8 idealMovement = GetWalkNormalMovementAction(objectInfo.facingDirection);

                // If we're facing a direction we need to go, do that preferably
                if((idealMovement == MOVEMENT_ACTION_WALK_NORMAL_LEFT && xDiff < 0)
                    || (idealMovement == MOVEMENT_ACTION_WALK_NORMAL_RIGHT && xDiff > 0)
                    || (idealMovement == MOVEMENT_ACTION_WALK_NORMAL_UP && yDiff < 0)
                    || (idealMovement == MOVEMENT_ACTION_WALK_NORMAL_DOWN && yDiff > 0)
                )
                {
                    heldMovement = idealMovement;
                }
                // Otherwise try to move on smallest axis distance first
                else if(xDist > 0 && (yDist == 0 || xDist > yDist))
                {
                    heldMovement = xDiff < 0 ? MOVEMENT_ACTION_WALK_NORMAL_LEFT : MOVEMENT_ACTION_WALK_NORMAL_RIGHT;
                }
                else if(yDist > 0)
                {
                    heldMovement = yDiff < 0 ? MOVEMENT_ACTION_WALK_NORMAL_UP : MOVEMENT_ACTION_WALK_NORMAL_DOWN;
                }

                // Speed up movement action if far away
                if(totalDist >= 5)
                    heldMovement += MOVEMENT_ACTION_WALK_FASTER_DOWN - MOVEMENT_ACTION_WALK_NORMAL_DOWN;
                else if(totalDist >= 2)
                    heldMovement += MOVEMENT_ACTION_WALK_FAST_DOWN - MOVEMENT_ACTION_WALK_NORMAL_DOWN;

                if(ObjectEventClearHeldMovementIfFinished(object))
                {
                    if(heldMovement != MOVEMENT_ACTION_NONE)
                    {
                        // Keep queuing up the correct movement
                        ObjectEventSetHeldMovement(object, heldMovement);
                    }
                    else if(object->facingDirection != objectInfo.facingDirection)
                    {
                        // Finished movement, so sync up facing direction
                        ObjectEventSetHeldMovement(object, GetFaceDirectionMovementAction(objectInfo.facingDirection));
                    }
                }
            }
            else
            {
                // Teleport, as too far
                MoveObjectEventToMapCoords(object, objectInfo.mapX, objectInfo.mapY);
            }
        }
    }
    else
    {
        // Remove object if currently exists
        if(objectId != OBJECT_EVENTS_COUNT)
        {
            RemoveObjectEvent(&gObjectEvents[objectId]);
        }
    }
}

bool8 Rogue_IsNetMultiplayerActive()
{
    return IsNetPlayerActive(0);
}

bool8 Rogue_IsNetMultiplayerHost()
{
    return IsNetPlayerActive(0) && (gRogueAssistantState.netPlayerProfile[0].flags & NETPLAYER_FLAGS_HOST) != 0;
}

void Rogue_RemoveNetObjectEvents()
{
    u8 i, j;
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

static bool8 AllowNetPartnerMons()
{
    return !Rogue_IsRunActive();
}

static void NetPlayerUpdate(u8 playerId, struct NetPlayerProfile* playerProfile, struct NetPlayerState* playerState)
{
    struct SyncObjectEventInfo syncObject;

    if(!IsNetPlayerActive(playerId))
    {
        // Remove player object
        u8 objectId = GetSpecialObjectEventIdByLocalId(OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 0);

        if(objectId != OBJECT_EVENTS_COUNT)
        {
            RemoveObjectEvent(&gObjectEvents[objectId]);
        }

        // Remove partner object
        if(AllowNetPartnerMons())
        {
            objectId = GetSpecialObjectEventIdByLocalId(OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 1);

            if(objectId != OBJECT_EVENTS_COUNT)
            {
                RemoveObjectEvent(&gObjectEvents[objectId]);
            }
        }
        return;
    }

    syncObject.localId = OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 0;
    syncObject.gfxId = OBJ_EVENT_GFX_ANABEL;
    syncObject.facingDirection = playerState->facingDirection;
    syncObject.mapX = playerState->pos.x + MAP_OFFSET;
    syncObject.mapY = playerState->pos.y + MAP_OFFSET;
    syncObject.mapNum = playerState->mapNum;
    syncObject.mapGroup = playerState->mapGroup;

    SyncObjectEvent(syncObject);

    // Follower
    if(AllowNetPartnerMons())
    {
        syncObject.localId = OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 1;
        syncObject.facingDirection = playerState->partnerFacingDirection;
        syncObject.mapX = playerState->partnerPos.x;
        syncObject.mapY = playerState->partnerPos.y;
        syncObject.mapNum = playerState->mapNum;
        syncObject.mapGroup = playerState->mapGroup;

        if(playerState->partnerMon != 0)
        {
            if(VarGet(VAR_FOLLOW_MON_4 + playerId) != playerState->partnerMon)
            {
                // Remove the object for one frame to reset the gfx
                VarSet(VAR_FOLLOW_MON_4 + playerId, playerState->partnerMon);
                syncObject.gfxId = 0;
            }
            else
            {
                syncObject.gfxId = OBJ_EVENT_GFX_FOLLOW_MON_4 + playerId;
            }
        }
        else
        {
            syncObject.gfxId = 0;
        }

        SyncObjectEvent(syncObject);
    }
}

#define tConnectAsHost data[1]

void Rogue_CreateMultiplayerConnectTask(bool8 asHost)
{
    if (FindTaskIdByFunc(Task_ConnectMultiplayer) == TASK_NONE)
    {
        u8 taskId1;

        taskId1 = CreateTask(Task_ConnectMultiplayer, 80);
        gTasks[taskId1].tConnectAsHost = asHost;

        if(asHost)
            Rogue_UpdateAssistantRequestState(REQUEST_STATE_MULTIPLAYER_HOST);
        else
            Rogue_UpdateAssistantRequestState(REQUEST_STATE_MULTIPLAYER_JOIN);
    }
}

static void Task_ConnectMultiplayer(u8 taskId)
{
    // Wait for connections
    if (JOY_NEW(B_BUTTON))
    {
        // Cancelled
        Rogue_UpdateAssistantRequestState(REQUEST_STATE_NONE);
        gSpecialVar_Result = FALSE;
        EnableBothScriptContexts();
        DestroyTask(taskId);
    }
    else if(Rogue_IsNetMultiplayerActive())
    {
        // Has connected
        Rogue_UpdateAssistantRequestState(REQUEST_STATE_NONE);
        gSpecialVar_Result = TRUE;
        EnableBothScriptContexts();
        DestroyTask(taskId);
    }
}

#undef tConnectAsHost

// Commands
//

static bool8 CommCmd_ProcessNext()
{
    u16 cmdToken, prevCmdToken, cmdIdx;
    buffer_offset_t inputPos = 0;
    buffer_offset_t outputPos = 0;

    cmdToken = READ_INPUT_16();
    prevCmdToken = READ_OUTPUT_16();

    // If we have a new valid cmd, execute it
    if(cmdToken != 0 && cmdToken != prevCmdToken)
    {
        cmdIdx = READ_INPUT_16();
        AGB_ASSERT(cmdIdx < ARRAY_COUNT(sCommCommands));

        if(cmdIdx < ARRAY_COUNT(sCommCommands))
        {
            // Clear previous output token
            outputPos = 0;
            WRITE_OUTPUT_16(0);

            sCommCommands[cmdIdx](inputPos, outputPos);

            // Command has finished, so put the correct token into the out buffer now, to signal that it is safe to read
            outputPos = 0;
            WRITE_OUTPUT_16(cmdToken);

            return TRUE;
        }
    }

    return FALSE;
}

static u8 Read8(buffer_offset_t* offset, u8* buffer, size_t capacity)
{
    u8* ptr = &buffer[*offset];
    u8 value;
    
    memcpy(&value, ptr, sizeof(u8));

    *offset += sizeof(u8);
    AGB_ASSERT(*offset < capacity);

    return value;
}

static u16 Read16(buffer_offset_t* offset, u8* buffer, size_t capacity)
{
    u8* ptr = &buffer[*offset];
    u16 value;
    
    memcpy(&value, ptr, sizeof(u16));

    *offset += sizeof(u16);
    AGB_ASSERT(*offset < capacity);

    return value;
}

static u32 Read32(buffer_offset_t* offset, u8* buffer, size_t capacity)
{
    u8* ptr = &buffer[*offset];
    u32 value;
    
    memcpy(&value, ptr, sizeof(u32));

    *offset += sizeof(u32);
    AGB_ASSERT(*offset < capacity);

    return value;
}

static void Write8(buffer_offset_t* offset, u8 value, u8* buffer, size_t capacity)
{
    u8* ptr = &buffer[*offset];
    
    memcpy(ptr, &value, sizeof(u8));

    *ptr = value;

    *offset += sizeof(u8);
    AGB_ASSERT(*offset < capacity);
}

static void Write16(buffer_offset_t* offset, u16 value, u8* buffer, size_t capacity)
{
    u8* ptr = &buffer[*offset];
    
    memcpy(ptr, &value, sizeof(u16));

    *offset += sizeof(u16);
    AGB_ASSERT(*offset < capacity);
}

static void Write32(buffer_offset_t* offset, u32 value, u8* buffer, size_t capacity)
{
    u8* ptr = &buffer[*offset];
    
    memcpy(ptr, &value, sizeof(u32));

    *offset += sizeof(u32);
    AGB_ASSERT(*offset < capacity);
}

// Commands
//

static void CommCmd_Echo(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    u16 value = READ_INPUT_16();

    WRITE_OUTPUT_16(value);
}

static void CommCmd_ReadConstant(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    u32 value;
    u16 constant = READ_INPUT_16();

    switch (constant)
    {
    case GAME_CONSTANT_ASSISTANT_STATE_NUM_ADDRESS:
        value = (u32)&gRogueAssistantState.assistantState;
        break;
    case GAME_CONSTANT_ASSISTANT_SUBSTATE_NUM_ADDRESS:
        value = (u32)&gRogueAssistantState.assistantSubstate;
        break;

    case GAME_CONSTANT_REQUEST_STATE_NUM_ADDRESS:
        value = (u32)&gRogueAssistantState.requestState;
        break;

    case GAME_CONSTANT_SAVE_BLOCK1_PTR_ADDRESS:
        value = (u32)&gSaveBlock1Ptr;
        break;
    case GAME_CONSTANT_SAVE_BLOCK2_PTR_ADDRESS:
        value = (u32)&gSaveBlock2Ptr;
        break;

    case GAME_CONSTANT_NET_PLAYER_CAPACITY:
        value = NET_PLAYER_CAPACITY;
        break;
    case GAME_CONSTANT_NET_PLAYER_PROFILE_ADDRESS:
        value = (u32)&gRogueAssistantState.netPlayerProfile[0];
        break;
    case GAME_CONSTANT_NET_PLAYER_PROFILE_SIZE:
        value = sizeof(gRogueAssistantState.netPlayerProfile[0]);
        break;
    case GAME_CONSTANT_NET_PLAYER_STATE_ADDRESS:
        value = (u32)&gRogueAssistantState.netPlayerState[0];
        break;
    case GAME_CONSTANT_NET_PLAYER_STATE_SIZE:
        value = sizeof(gRogueAssistantState.netPlayerState[0]);
        break;

    case GAME_CONSTANT_DEBUG_MAIN_ADDRESS:
        value = (u32)&gMain;
        break;

    case GAME_CONSTANT_DEBUG_QUERY_UNCOLLAPSE_BUFFER_PTR:
        {
            struct RogueQueryDebug debugData = RogueQuery_GetDebugData();
            value = (u32)debugData.uncollapsedBufferPtr;
        }
        break;

    case GAME_CONSTANT_DEBUG_QUERY_UNCOLLAPSE_CAPACITY:
        {
            struct RogueQueryDebug debugData = RogueQuery_GetDebugData();
            value = (u32)debugData.uncollapsedBufferCapacity;
        }
        break;

    case GAME_CONSTANT_DEBUG_QUERY_COLLAPSED_BUFFER_PTR:
        {
            struct RogueQueryDebug debugData = RogueQuery_GetDebugData();
            value = (u32)debugData.collapseBufferPtr;
        }
        break;

    case GAME_CONSTANT_DEBUG_QUERY_COLLAPSED_SIZE_PTR:
        {
            struct RogueQueryDebug debugData = RogueQuery_GetDebugData();
            value = (u32)debugData.collapseSizePtr;
        }
        break;

    default: // Error
        value = (u32)-1;
        break;
    }

    WRITE_OUTPUT_32(value);
}

static void CommCmd_BeginMultiplayerHost(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    gRogueAssistantState.netPlayerProfile[0].flags = NETPLAYER_FLAGS_ACTIVE | NETPLAYER_FLAGS_HOST;
    memcpy(gRogueAssistantState.netPlayerProfile->trainerName, gSaveBlock2Ptr->playerName, sizeof(gRogueAssistantState.netPlayerProfile->trainerName));
}

static void CommCmd_BeginMultiplayerClient(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    gRogueAssistantState.netPlayerProfile[0].flags = NETPLAYER_FLAGS_ACTIVE;
    memcpy(gRogueAssistantState.netPlayerProfile->trainerName, gSaveBlock2Ptr->playerName, sizeof(gRogueAssistantState.netPlayerProfile->trainerName));
}

static void CommCmd_EndMultiplayer(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    gRogueAssistantState.netPlayerProfile[0].flags = NETPLAYER_FLAGS_NONE;
    Rogue_RemoveNetObjectEvents();
}

static void CommCmd_GetSpeciesName(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    u8 i;
    u16 value = READ_INPUT_16();

    for(i = 0; i < POKEMON_NAME_LENGTH + 1; ++i)
    {
        if(value < NUM_SPECIES)
        {
            WRITE_OUTPUT_8(gSpeciesNames[value][i]);
        }
        else
        {
            WRITE_OUTPUT_8(0xAC); // '?'
        }
    }
}

static void CommCmd_GetItemName(buffer_offset_t inputPos, buffer_offset_t outputPos)
{
    u8 i;
    u16 value = READ_INPUT_16();

    for(i = 0; i < ITEM_NAME_LENGTH; ++i)
    {
        if(value < ITEMS_COUNT)
        {
            WRITE_OUTPUT_8(Rogue_GetItemName(value)[i]);
        }
        else
        {
            WRITE_OUTPUT_8(0xAC); // '?'
        }
    }
}