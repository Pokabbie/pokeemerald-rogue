#include "global.h"

#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_frontier.h"
#include "constants/event_objects.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/weather.h"
#include "gba/isagbprint.h"
#include "data.h"

#include "battle_main.h"
#include "event_data.h"
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
    GAME_CONSTANT_NET_PLAYER_ADDRESS,
    GAME_CONSTANT_NET_PLAYER_CAPACITY,
};

// Global states
//

struct NetPlayerState
{
    struct Coords16 pos;
    struct WarpData location;
};

struct RogueAssistantState
{
    u8 inCommBuffer[16];
    u8 outCommBuffer[16];
    struct NetPlayerState netPlayers[NET_PLAYER_CAPACITY];
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



// Command declerations
//

typedef void (*CommandCallback)(buffer_offset_t, buffer_offset_t);

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

void Rogue_AssistantCallback()
{
    CommCmd_ProcessNext();
}


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
    case GAME_CONSTANT_NET_PLAYER_ADDRESS:
        value = (u32)&gRogueAssistantState.netPlayers[0];
        break;

    case GAME_CONSTANT_NET_PLAYER_CAPACITY:
        value = NET_PLAYER_CAPACITY;
        break;
        
    default: // Error
        value = (u32)-1;
        break;
    }

    WRITE_OUTPUT_32(value);
}