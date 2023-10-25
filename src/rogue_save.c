#include "global.h"
#include "constants/items.h"
#include "constants/layouts.h"

#include "berry.h"
#include "load_save.h"
#include "item.h"
#include "malloc.h"
#include "money.h"
#include "play_time.h"
#include "random.h"

#include "rogue_adventurepaths.h"
#include "rogue_assistant.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_ridemon.h"
#include "rogue_save.h"

// The version to use for tracking/updating internal save game data
// Update "RogueSave_GetVersionIdFor" every time the save structure changes
#define ROGUE_SAVE_VERSION 1

#define ROGUE_SAVE_BLOCK_CAPACITY (sizeof(struct BoxPokemon) * IN_BOX_COUNT * LEFTOVER_BOXES_COUNT)

enum
{
    ROGUE_SAVE_FORMAT_UNKNOWN,
    ROGUE_SAVE_FORMAT_READ,
    ROGUE_SAVE_FORMAT_WRITE,
};

struct SaveBlockStream
{
    void* data;
    size_t offset;
    size_t size;
    u32 encryptionKey;
    bool8 isWriteMode;
};

// Restore states when the player returns to the hub
struct RogueRunRestoreBlock
{
    struct Pokemon playerParty[PARTY_SIZE];
    struct ItemSlot bagItems[BAG_ITEM_CAPACITY];
    struct BerryTree berryTrees[ROGUE_HUB_BERRY_TREE_COUNT];
    u32 money;
    u32 playTime;
};

STATIC_ASSERT(sizeof(struct RogueSaveBlock) < ROGUE_SAVE_BLOCK_CAPACITY, RogueSaveBlockSize);

static EWRAM_DATA struct RogueRunRestoreBlock sRunRestoreBlock = {0};

static void FlipEncryptMemory(void* ptr, size_t size, u32 encryptionKey)
{
    if(encryptionKey)
    {
        size_t i;
        u8* write;
        u8* encryptionBytes = (u8*)&encryptionKey;

        for(i = 0; i < size; ++i)
        {
            write = (u8*)(ptr) + i;
            *write = *write ^ encryptionBytes[i % 4];
        }
    }
}

static bool8 IsSerializeRangeValid(struct SaveBlockStream* block, size_t size)
{
    void* startAddr = block->data + block->offset;
    void* endAddr = startAddr + size;
    return (size_t)endAddr < (size_t)(block->data + block->size);
}

static void SerializeData(struct SaveBlockStream* block, void* ptr, size_t size)
{
    void* addr = block->data + block->offset;
    AGB_ASSERT(IsSerializeRangeValid(block, size));

    if(block->isWriteMode)
    {
        memcpy(addr, ptr, size);

        FlipEncryptMemory(addr, size, block->encryptionKey);
    }
    else
    {
        memcpy(ptr, addr, size);

        FlipEncryptMemory(ptr, size, block->encryptionKey);
    }

    block->offset += size;
}

static void SerializeArray(struct SaveBlockStream* block, void* ptr, size_t elementSize, size_t arraySize)
{
    u16 size = arraySize;
    SerializeData(block, &size, sizeof(size));

    AGB_ASSERT(size <= arraySize);
    SerializeData(block, ptr, elementSize * size);
}

void RogueSave_UpdatePointers()
{
    void* ptr = &gPokemonStoragePtr->boxes[TOTAL_BOXES_COUNT][0];
    gRogueSaveBlock = (struct RogueSaveBlock*)ptr;
}

void RogueSave_ClearData()
{
    memset(gRogueSaveBlock, 0, sizeof(struct RogueSaveBlock));
}

static u16 SerializeRogueBlockInternal(struct SaveBlockStream* stream, struct RogueSaveBlock* saveBlock)
{
    u16 secretId;

    // Serialize header
    //
    SerializeData(stream, &saveBlock->saveVersion, sizeof(saveBlock->saveVersion));

    // Block read/write mode
    SerializeData(stream, &saveBlock->currentBlockFormat, sizeof(saveBlock->currentBlockFormat));

    // Secret ID to ensure this is a valid NEW Rogue save
    secretId = 27615;
    SerializeData(stream, &secretId, sizeof(secretId));

    if(secretId != 27615)
    {
        DebugPrintf("Couldn't load save as missing secret ID (expected 27615, found %d)", secretId);
        return;
    }

    // Serialize data
    //

    // Quests
    SerializeArray(stream, saveBlock->questStates, sizeof(saveBlock->questStates[0]), ARRAY_COUNT(saveBlock->questStates));

    // Campaigns
    SerializeArray(stream, saveBlock->campaignData, sizeof(saveBlock->campaignData[0]), ARRAY_COUNT(saveBlock->campaignData));
    
    // Hub progression
    SerializeArray(stream, saveBlock->hubMap.areaBuiltFlags, sizeof(saveBlock->hubMap.areaBuiltFlags[0]), ARRAY_COUNT(saveBlock->hubMap.areaBuiltFlags));
    SerializeArray(stream, saveBlock->hubMap.areaCoords, sizeof(saveBlock->hubMap.areaCoords[0]), ARRAY_COUNT(saveBlock->hubMap.areaCoords));
    SerializeArray(stream, saveBlock->hubMap.upgradeFlags, sizeof(saveBlock->hubMap.upgradeFlags[0]), ARRAY_COUNT(saveBlock->hubMap.upgradeFlags));

    // Time/Seasons
    SerializeData(stream, &saveBlock->timeOfDayMinutes, sizeof(saveBlock->timeOfDayMinutes));
    SerializeData(stream, &saveBlock->seasonCounter, sizeof(saveBlock->seasonCounter));

    // Safari
    SerializeArray(stream, saveBlock->safariMons, sizeof(saveBlock->safariMons[0]), ARRAY_COUNT(saveBlock->safariMons));
    SerializeArray(stream, saveBlock->safariLegends, sizeof(saveBlock->safariLegends[0]), ARRAY_COUNT(saveBlock->safariLegends));

    // Difficulty/Adventure Settings
    SerializeArray(stream, saveBlock->difficultyConfig.toggleBits, sizeof(saveBlock->difficultyConfig.toggleBits[0]), ARRAY_COUNT(saveBlock->difficultyConfig.toggleBits));
    SerializeArray(stream, saveBlock->difficultyConfig.rangeValues, sizeof(saveBlock->difficultyConfig.rangeValues[0]), ARRAY_COUNT(saveBlock->difficultyConfig.rangeValues));

    // Serialize debug data
    {
        bool8 isDebug = FALSE;
#ifdef ROGUE_DEBUG
        isDebug = TRUE;
#endif
        SerializeData(stream, &isDebug, sizeof(isDebug));

        if(isDebug)
        {
#ifdef ROGUE_DEBUG
            SerializeArray(stream, gRogueDebug.toggleBits, sizeof(gRogueDebug.toggleBits[0]), ARRAY_COUNT(gRogueDebug.toggleBits));
            SerializeArray(stream, gRogueDebug.rangeValues, sizeof(gRogueDebug.rangeValues[0]), ARRAY_COUNT(gRogueDebug.rangeValues));
#else
            struct RogueDebugConfig throwaway;

            SerializeArray(stream, throwaway.toggleBits, sizeof(throwaway.toggleBits[0]), ARRAY_COUNT(throwaway.toggleBits));
            SerializeArray(stream, throwaway.rangeValues, sizeof(throwaway.rangeValues[0]), ARRAY_COUNT(throwaway.rangeValues));
#endif
        }
    }

    {
        struct RogueRideMonState* rideState = Rogue_GetPlayerRideMonStatePtr();
        SerializeData(stream, rideState, sizeof(struct RogueRideMonState));
    }

    // Run Data
    // Don't need to worry about versioning these, as it's not valid to patch midway through a run
    SerializeData(stream, &gRngRogueValue, sizeof(gRngRogueValue));

    // For development it's easier to keep the restore block first, as it's not as likely to change as the run data
    SerializeData(stream, &sRunRestoreBlock, sizeof(sRunRestoreBlock));
    SerializeData(stream, &gRogueRun, sizeof(gRogueRun));

    SerializeData(stream, &gRogueAdvPath.currentRoomParams, sizeof(gRogueAdvPath.currentRoomParams));
    SerializeData(stream, &gRogueAdvPath.currentRoomType, sizeof(gRogueAdvPath.currentRoomType));
}

static u16 SerializeRogueBlock(bool8 inWriteMode)
{
    u16 count, saveVersion;
    struct RogueSaveBlock blockCopy;
    struct SaveBlockStream stream;

    //blockCopy = Alloc(sizeof(struct RogueSaveBlock));
    //AGB_ASSERT(blockCopy != NULL);

    if(inWriteMode)
    {
        // Write directly from the copy into the save ptr
        memcpy(&blockCopy, gRogueSaveBlock, sizeof(struct RogueSaveBlock));

        stream.data = gRogueSaveBlock;
        stream.size = ROGUE_SAVE_BLOCK_CAPACITY;
        stream.encryptionKey = 0;
        stream.offset = 0;
        stream.isWriteMode = TRUE; // write to above

        saveVersion = SerializeRogueBlockInternal(&stream, &blockCopy);
        DebugPrintf("RogueBlock WRITE (offset: %d, free: %d, size: %d)", stream.offset, (stream.size - stream.offset), stream.size);
    }
    else
    {
        // Write directly into the copy into from save ptr
        // then stomp back ontop of the save ptr to be safely used elsewhere
        memset(&blockCopy, 0, sizeof(struct RogueSaveBlock));

        stream.data = gRogueSaveBlock;
        stream.size = ROGUE_SAVE_BLOCK_CAPACITY;
        stream.encryptionKey = 0;
        stream.offset = 0;
        stream.isWriteMode = FALSE; // read from above

        saveVersion = SerializeRogueBlockInternal(&stream, &blockCopy);
        DebugPrintf("RogueBlock READ (offset: %d, free: %d, size: %d)", stream.offset, (stream.size - stream.offset), stream.size);

        memcpy(gRogueSaveBlock, &blockCopy, sizeof(struct RogueSaveBlock));
    }

    //Free(blockCopy);
    return saveVersion;
}

void RogueSave_FormatForWriting()
{
    if(gRogueSaveBlock->currentBlockFormat != ROGUE_SAVE_FORMAT_WRITE)
    {
        gRogueSaveBlock->saveVersion = ROGUE_SAVE_VERSION;
        gRogueSaveBlock->currentBlockFormat = ROGUE_SAVE_FORMAT_WRITE;

        SerializeRogueBlock(TRUE);
    }
}

void RogueSave_FormatForReading()
{
    if(gRogueSaveBlock->currentBlockFormat != ROGUE_SAVE_FORMAT_READ)
    {
        SerializeRogueBlock(FALSE);

        gRogueSaveBlock->currentBlockFormat = ROGUE_SAVE_FORMAT_READ;
    }
}

u16 RogueSave_GetVersionIdFor(u16 saveVersion)
{
    // TODO - lookup
    return SAVE_VER_ID_2_0;
}

u16 RogueSave_GetVersionId()
{
    return RogueSave_GetVersionIdFor(gRogueSaveBlock->saveVersion);
}

void RogueSave_OnSaveLoaded()
{
    Rogue_NotifySaveLoaded();

    if(gRogueSaveBlock->saveVersion != ROGUE_SAVE_VERSION)
    {
        Rogue_NotifySaveVersionUpdated(gRogueSaveBlock->saveVersion, ROGUE_SAVE_VERSION);
    }

    if(Rogue_IsRunActive() && Rogue_GetCurrentDifficulty() < ROGUE_MAX_BOSS_COUNT)
    {
        // We need to regenerate here, as we need to know which
        RogueAdv_GenerateAdventurePathsIfRequired();

        gRogueAdvPath.isOverviewActive = FALSE;

        if(gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_ADVENTURE_PATHS) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(ROGUE_ADVENTURE_PATHS))
        {
            u8 i;
            gRogueAdvPath.isOverviewActive = TRUE;
        }
    }

    // We cache these values for faster lookup
    RecalcCharmCurseValues();
    
    // Fixes bugs with follow mon permnamently stealing a random NPC from a map
    ResetFollowParterMonObjectEvent();

    // Remove any net objects which we shouldn't have saved anyway
    Rogue_RemoveNetObjectEvents();
}

static void AppendItemsFromPocket(u8 pocket, struct ItemSlot* dst, u16* index)
{
    u16 i;
    u16 writeIdx;
    u16 itemId;
    u16 count = gBagPockets[pocket].capacity;

    // Use getters to avoid encryption
    for (i = 0; i < count; i++)
    {
        writeIdx = *index;
        itemId = gBagPockets[pocket].itemSlots[i].itemId;

        if(itemId != ITEM_NONE)
        {
            dst[writeIdx].itemId = itemId;
            dst[writeIdx].quantity = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);

            *index = writeIdx + 1;
        }
    }
}

void RogueSave_SaveHubStates()
{
    u8 i;
    u16 bagItemIdx;
    u16 pocketId;

    sRunRestoreBlock.money = GetMoney(&gSaveBlock1Ptr->money);
    sRunRestoreBlock.playTime = 
        (u32)gSaveBlock2Ptr->playTimeSeconds +
        (u32)gSaveBlock2Ptr->playTimeMinutes * 60 +
        (u32)gSaveBlock2Ptr->playTimeHours * 60 * 60;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        CopyMon(&sRunRestoreBlock.playerParty[i], &gPlayerParty[i], sizeof(gPlayerParty[i]));
    }
    for(; i < PARTY_SIZE; ++i)
    {
        ZeroMonData(&sRunRestoreBlock.playerParty[i]);
    }

    memcpy(&sRunRestoreBlock.berryTrees[0], GetBerryTreeInfo(1), sizeof(struct BerryTree) * ROGUE_HUB_BERRY_TREE_COUNT);
    
    // Put all items into a single big list
    bagItemIdx = 0;

    for(pocketId = 0; pocketId < POCKETS_COUNT; ++pocketId)
        AppendItemsFromPocket(pocketId, &sRunRestoreBlock.bagItems[0], &bagItemIdx);

    for(; bagItemIdx < BAG_ITEM_CAPACITY; ++bagItemIdx)
    {
        sRunRestoreBlock.bagItems[bagItemIdx].itemId = ITEM_NONE;
        sRunRestoreBlock.bagItems[bagItemIdx].quantity = 0;
    }
}

void RogueSave_LoadHubStates()
{
    u8 i;
    u16 bagItemIdx;
    u32 totalTime;

    SetMoney(&gSaveBlock1Ptr->money, sRunRestoreBlock.money);

    // Add previous run time to total time
    //
    totalTime = 
        (u32)gSaveBlock2Ptr->playTimeSeconds +
        (u32)gSaveBlock2Ptr->playTimeMinutes * 60 +
        (u32)gSaveBlock2Ptr->playTimeHours * 60 * 60;

    totalTime += sRunRestoreBlock.playTime;


    gSaveBlock2Ptr->playTimeSeconds = totalTime % 60;
    totalTime /= 60;

    gSaveBlock2Ptr->playTimeMinutes = totalTime % 60;
    totalTime /= 60;

    gSaveBlock2Ptr->playTimeHours = totalTime % 60;
    totalTime /= 60;

    if(gSaveBlock2Ptr->playTimeHours > 999)
    {
        PlayTimeCounter_SetToMax();
    }


    for(i = 0; i < PARTY_SIZE; ++i)
    {
        CopyMon(&gPlayerParty[i], &sRunRestoreBlock.playerParty[i], sizeof(gPlayerParty[i]));
    }

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE)
            break;
    }
    gPlayerPartyCount = i;

    memcpy(GetBerryTreeInfo(1), &sRunRestoreBlock.berryTrees[0], sizeof(struct BerryTree) * ROGUE_HUB_BERRY_TREE_COUNT);

    // Restore the bag by just clearing and adding everything back to it
    ClearBag();

    for(bagItemIdx = 0; bagItemIdx < BAG_ITEM_CAPACITY; ++bagItemIdx)
    {
        const u16 itemId = sRunRestoreBlock.bagItems[bagItemIdx].itemId;
        const u16 quantity = sRunRestoreBlock.bagItems[bagItemIdx].quantity;

        if(itemId != ITEM_NONE && quantity != 0)
        {
            AddBagItem(itemId, quantity);
        }
    }
}

u16 RogueSave_GetHubBagItemIdAt(u16 index)
{
    if(index < ARRAY_COUNT(sRunRestoreBlock.bagItems))
    {
        return sRunRestoreBlock.bagItems[index].itemId;
    }

    return ITEM_NONE;
}

u16 RogueSave_GetHubBagItemQuantityAt(u16 index)
{
    if(index < ARRAY_COUNT(sRunRestoreBlock.bagItems))
    {
        return sRunRestoreBlock.bagItems[index].quantity;
    }

    return 0;
}