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
#include "intro.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"

#include "rogue_assistant.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"

// Constants
//

#define IN_COMM_BUFFER_SIZE 16
#define OUT_COMM_BUFFER_SIZE 8

#define NET_PLAYER_CAPACITY 4

enum 
{
    GAME_CONSTANT_SAVE_BLOCK1_PTR_ADDRESS,
    GAME_CONSTANT_SAVE_BLOCK2_PTR_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_CAPACITY,
    GAME_CONSTANT_NET_PLAYER_PROFILE_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_PROFILE_SIZE,
    GAME_CONSTANT_NET_PLAYER_STATE_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_STATE_SIZE,
};

#define NETPLAYER_FLAGS_NONE        0
#define NETPLAYER_FLAGS_ACTIVE      (1 << 0)

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
    s8 mapGroup;
    s8 mapNum;
};

struct RogueAssistantState
{
    u8 inCommBuffer[16];
    u8 outCommBuffer[16];
    struct NetPlayerProfile netPlayerProfile[NET_PLAYER_CAPACITY + 1];
    struct NetPlayerState netPlayerState[NET_PLAYER_CAPACITY + 1];
};

EWRAM_DATA struct RogueAssistantState gRogueAssistantState;

const struct RogueAssistantHeader gRogueAssistantHeader =
{
    .inCommCapacity = sizeof(gRogueAssistantState.inCommBuffer),
    .outCommCapacity = sizeof(gRogueAssistantState.outCommBuffer),
    .inCommBuffer = gRogueAssistantState.inCommBuffer,
    .outCommBuffer = gRogueAssistantState.outCommBuffer
};


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

static const CommandCallback sCommCommands[] = 
{
    CommCmd_Echo,
    CommCmd_ReadConstant,
};


// Methods
//

void Rogue_AssistantInit()
{

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
    // Populate current player state
    {
        if(!IsNetPlayerActive(0))
        {
            gRogueAssistantState.netPlayerProfile[0].flags = NETPLAYER_FLAGS_ACTIVE;
            memcpy(gRogueAssistantState.netPlayerProfile->trainerName, gSaveBlock2Ptr->playerName, sizeof(gRogueAssistantState.netPlayerProfile->trainerName));
        }

        gRogueAssistantState.netPlayerState[0].pos = gSaveBlock1Ptr->pos;
        gRogueAssistantState.netPlayerState[0].pos.x -= 3;
        gRogueAssistantState.netPlayerState[0].mapGroup = gSaveBlock1Ptr->location.mapGroup;
        gRogueAssistantState.netPlayerState[0].mapNum = gSaveBlock1Ptr->location.mapNum;
    }

    // Do external net update, if needed
    {
        u8 i;
        for(i = 1; i < NET_PLAYER_CAPACITY; ++i)
        {
            if(IsNetPlayerActive(i))
            {
                NetPlayerUpdate(i, &gRogueAssistantState.netPlayerProfile[i], &gRogueAssistantState.netPlayerState[i]);
            }
        }
    }
}


// Multiplayer
//

static void NetPlayerUpdate(u8 playerId, struct NetPlayerProfile* playerProfile, struct NetPlayerState* playerState)
{
    bool8 shouldBeVisible = FALSE;
    u8 localId = OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 0;
    u8 localFollowId = OBJ_EVENT_ID_MULTIPLAYER_FIRST + playerId * 2 + 1;

    if(playerState->mapGroup == gSaveBlock1Ptr->location.mapGroup && playerState->mapNum == gSaveBlock1Ptr->location.mapNum)
    {
        s16 xDist = abs(playerState->pos.x - gSaveBlock1Ptr->pos.x);
        s16 yDist = abs(playerState->pos.y - gSaveBlock1Ptr->pos.y);
        if(xDist <= 8 && yDist <= 5)
        {
            shouldBeVisible = TRUE;
        }

        if(shouldBeVisible)
        {
            u8 objectId = GetObjectEventIdByLocalIdAndMap(localId, playerState->mapNum, playerState->mapGroup);
            s16 mapX = playerState->pos.x + MAP_OFFSET;
            s16 mapY = playerState->pos.y + MAP_OFFSET;

            if(objectId == OBJECT_EVENTS_COUNT)
            {
                objectId = SpawnSpecialObjectEventParameterized(
                    OBJ_EVENT_GFX_ANABEL,
                    MOVEMENT_TYPE_NONE,
                    localId,
                    mapX,
                    mapY,
                    MapGridGetElevationAt(mapX, mapY)
                );

                //gObjectEvents[objectEventId].rangeX = 8;
                //gObjectEvents[objectEventId].rangeY = 8;
            }

            if(objectId != OBJECT_EVENTS_COUNT)
            {
                s16 totalDist;
                u8 heldMovement = MOVEMENT_ACTION_NONE;
                struct ObjectEvent* object = &gObjectEvents[objectId];
                s16 xDiff = mapX - object->currentCoords.x;
                s16 yDiff = mapY - object->currentCoords.y;

                xDist = abs(xDiff);
                yDist = abs(yDiff);
                totalDist = xDist + yDist;

                // Close enough to animate
                if(totalDist < 12)
                {
                    u8 heldMovement = MOVEMENT_ACTION_NONE;

                    // Try to move on smallest axis distance first
                    if(xDist > 0 && (yDist == 0 || xDist > yDist))
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
                    }
                }
                else
                {
                    // Teleport, as too far
                    MoveObjectEventToMapCoords(object, mapX, mapY);
                }

                
    //if (newState == MOVEMENT_INVALID)
    //ObjectEventClearHeldMovementIfActive(follower);
    //ObjectEventSetHeldMovement(follower, newState);
    //ObjectEventClearHeldMovementIfFinished(follower);

                //SetObjEventTemplateMovementType

                //gObjectEvents[objectId].previousCoords.x = netPlayer->pos.x;
                //gObjectEvents[objectId].previousCoords.y = netPlayer->pos.y;
//
                //gObjectEvents[objectId].currentCoords.x = netPlayer->pos.x;
                //gObjectEvents[objectId].currentCoords.y = netPlayer->pos.y;
            }
        }
    }
}


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

    default: // Error
        value = (u32)-1;
        break;
    }

    WRITE_OUTPUT_32(value);
}