#include "global.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "save.h"
#include "string_util.h"
#include "strings.h"
#include "rogue_expanded_flash.h"
#include "rogue_expanded_storage.h"

#define EXTRA_BOX_BYTES (EXPANDED_BOXES_COUNT * IN_BOX_COUNT * sizeof(struct BoxPokemon))
#define EXTRA_META_BYTES (EXPANDED_BOXES_COUNT * (BOX_NAME_LENGTH + 2))
#define EXTRA_PAYLOAD_BYTES (EXTRA_BOX_BYTES + EXTRA_META_BYTES)
#define EXTRA_PAYLOAD_SECTORS ((EXTRA_PAYLOAD_BYTES + ROGUE_EXPANDED_SECTOR_SIZE - 1) / ROGUE_EXPANDED_SECTOR_SIZE)
#define COMMIT_A 0x20000
#define COMMIT_B 0x60000
#define WORKING_BASE 0xA0000
#define NO_COMMIT 255
#define NO_BOX 255
#define HEADER_MAGIC 0x38534252
#define HEADER_VERSION 1
#define HEADER_COMMITTED 0x54494D43

struct ExpandedHeader
{
    u32 magic;
    u32 version;
    u32 generation;
    u32 trainerId;
    u32 length;
    u32 payloadHash;
    u32 headerHash;
    u32 committed;
};

struct ExpandedCache
{
    struct BoxPokemon mons[IN_BOX_COUNT];
    u32 originalHash;
    u8 boxId;
};

struct PACKED ExpandedMetadata
{
    u8 names[EXPANDED_BOXES_COUNT][BOX_NAME_LENGTH + 1];
    u8 wallpapers[EXPANDED_BOXES_COUNT];
};

struct ExpandedState
{
    struct ExpandedCache cache[2];
    struct ExpandedMetadata metadata;
    u8 workingSectors[(EXTRA_PAYLOAD_SECTORS + 7) / 8];
    u32 trainerId;
    u32 generation;
    u32 pendingGeneration;
    u8 previousMarker[4];
    u8 activeCommit;
    u8 protectedCommit;
    u8 pendingCommit;
    u8 nextVictim;
    bool8 initialized;
};

static EWRAM_DATA struct ExpandedState sExpanded = {0};

STATIC_ASSERT(sizeof(struct BoxPokemon) == 80, ExpandedBoxMonSize);
STATIC_ASSERT(EXTRA_PAYLOAD_BYTES + ROGUE_EXPANDED_SECTOR_SIZE < 0x40000, ExpandedCommitFits);
STATIC_ASSERT(sizeof(struct ExpandedMetadata) == EXTRA_META_BYTES, ExpandedMetadataSize);

// The existing save buffer is only borrowed while no base-sector operation is active.
static u8 *Scratch(void)
{
    return (u8 *)&gSaveDataBuffer;
}

static u32 HashBytes(u32 hash, const void *data, u32 size)
{
    const u8 *bytes = data;
    while (size--)
        hash = (hash ^ *bytes++) * 16777619u;
    return hash;
}

static u32 Hash(const void *data, u32 size)
{
    return HashBytes(2166136261u, data, size);
}

static u32 TrainerId(void)
{
    u32 id;
    memcpy(&id, gSaveBlock2Ptr->playerTrainerId, sizeof(id));
    return id;
}

static u32 CommitAddress(u8 commit)
{
    return commit == 0 ? COMMIT_A : COMMIT_B;
}

static bool8 IsMarked(const u8 *marker)
{
    return marker[0] == 'R' && marker[1] == '9' && marker[2] == '9';
}

static u8 *BaseMarker(void)
{
    return &gPokemonStoragePtr->boxWallpapers[BASE_BOXES_COUNT];
}

static bool8 ReadHeader(u8 commit, u32 generation, u32 trainerId, struct ExpandedHeader *header)
{
    if (!RogueExpandedFlash_Read(CommitAddress(commit), header, sizeof(*header)))
        return FALSE;
    if (header->magic != HEADER_MAGIC || header->version != HEADER_VERSION
     || header->generation != generation || header->trainerId != trainerId
     || header->length != EXTRA_PAYLOAD_BYTES || header->committed != HEADER_COMMITTED
     || header->headerHash != Hash(header, offsetof(struct ExpandedHeader, headerHash)))
        return FALSE;
    return TRUE;
}

static bool8 ValidateCommit(u8 commit, u32 generation, u32 trainerId)
{
    struct ExpandedHeader header;
    u32 offset, size;
    u32 hash = 2166136261u;
    if (!ReadHeader(commit, generation, trainerId, &header))
        return FALSE;
    for (offset = 0; offset < EXTRA_PAYLOAD_BYTES; offset += size)
    {
        size = min(ROGUE_EXPANDED_SECTOR_SIZE, EXTRA_PAYLOAD_BYTES - offset);
        if (!RogueExpandedFlash_Read(CommitAddress(commit) + ROGUE_EXPANDED_SECTOR_SIZE + offset, Scratch(), size))
            return FALSE;
        hash = HashBytes(hash, Scratch(), size);
    }
    return hash == header.payloadHash;
}

static bool8 ReadLogicalSector(u32 sector)
{
    if (sExpanded.workingSectors[sector / 8] & (1 << (sector % 8)))
        return RogueExpandedFlash_Read(WORKING_BASE + sector * ROGUE_EXPANDED_SECTOR_SIZE, Scratch(), ROGUE_EXPANDED_SECTOR_SIZE);
    if (sExpanded.activeCommit != NO_COMMIT)
        return RogueExpandedFlash_Read(CommitAddress(sExpanded.activeCommit) + (sector + 1) * ROGUE_EXPANDED_SECTOR_SIZE, Scratch(), ROGUE_EXPANDED_SECTOR_SIZE);
    memset(Scratch(), 0, ROGUE_EXPANDED_SECTOR_SIZE);
    return TRUE;
}

static bool8 FlushCache(struct ExpandedCache *cache)
{
    u32 start, end, sector, copyStart, copyEnd;
    u32 hash;
    if (cache->boxId == NO_BOX)
        return TRUE;
    hash = Hash(cache->mons, sizeof(cache->mons));
    if (hash == cache->originalHash)
        return TRUE;
    start = (cache->boxId - BASE_BOXES_COUNT) * sizeof(cache->mons);
    end = start + sizeof(cache->mons);
    for (sector = start / ROGUE_EXPANDED_SECTOR_SIZE; sector <= (end - 1) / ROGUE_EXPANDED_SECTOR_SIZE; ++sector)
    {
        if (!ReadLogicalSector(sector))
            return FALSE;
        copyStart = max(start, sector * ROGUE_EXPANDED_SECTOR_SIZE);
        copyEnd = min(end, (sector + 1) * ROGUE_EXPANDED_SECTOR_SIZE);
        memcpy(Scratch() + copyStart % ROGUE_EXPANDED_SECTOR_SIZE,
               (u8 *)cache->mons + copyStart - start, copyEnd - copyStart);
        if (!RogueExpandedFlash_WriteSector(WORKING_BASE / ROGUE_EXPANDED_SECTOR_SIZE + sector, Scratch()))
            return FALSE;
        sExpanded.workingSectors[sector / 8] |= 1 << (sector % 8);
    }
    cache->originalHash = hash;
    return TRUE;
}

static void DefaultMetadata(void)
{
    u32 i;
    memset(&sExpanded.metadata, 0, sizeof(sExpanded.metadata));
    for (i = 0; i < EXPANDED_BOXES_COUNT; ++i)
    {
        u8 *dest = StringCopy(sExpanded.metadata.names[i], gText_Box);
        ConvertIntToDecimalStringN(dest, BASE_BOXES_COUNT + i + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        sExpanded.metadata.wallpapers[i] = (BASE_BOXES_COUNT + i) % 16;
    }
}

static void ResetState(void)
{
    memset(&sExpanded, 0, sizeof(sExpanded));
    sExpanded.cache[0].boxId = NO_BOX;
    sExpanded.cache[1].boxId = NO_BOX;
    sExpanded.activeCommit = NO_COMMIT;
    sExpanded.protectedCommit = NO_COMMIT;
    sExpanded.pendingCommit = NO_COMMIT;
    sExpanded.trainerId = TrainerId();
    sExpanded.initialized = TRUE;
    DefaultMetadata();
}

void RogueExpanded_Reset(void)
{
    u8 protectedCommit = NO_COMMIT;
    if (sExpanded.initialized)
        protectedCommit = sExpanded.activeCommit != NO_COMMIT ? sExpanded.activeCommit : sExpanded.protectedCommit;
    if (!RogueExpandedFlash_IsAvailable())
        RogueExpandedFlash_Fatal(EXPANDED_ERROR_BACKEND);
    ResetState();
    // A new profile/clear starts empty, while its previous save remains recoverable.
    sExpanded.protectedCommit = protectedCommit;
    memset(BaseMarker(), 0, 4);
}

bool8 RogueExpanded_Load(u32 generation)
{
    u32 address;
    u8 i;
    if (!RogueExpandedFlash_IsAvailable())
        return FALSE;
    ResetState();
    sExpanded.generation = generation;
    if (!IsMarked(BaseMarker()))
        return TRUE; // A legacy save starts with 89 empty boxes.
    if (BaseMarker()[3] != HEADER_VERSION)
        return FALSE;
    for (i = 0; i < 2; ++i)
    {
        if (ValidateCommit(i, generation, sExpanded.trainerId))
        {
            sExpanded.activeCommit = i;
            address = CommitAddress(i) + ROGUE_EXPANDED_SECTOR_SIZE + EXTRA_BOX_BYTES;
            return RogueExpandedFlash_Read(address, &sExpanded.metadata, sizeof(sExpanded.metadata));
        }
    }
    return FALSE;
}

// Validate the base generation as a complete set before accepting its box bank.
// This also rejects interrupted base writes containing a mixture of generations.
bool8 RogueExpanded_ValidateBaseSlot(u8 slot, u32 generation)
{
    u32 seen = 0, trainerId = 0, i;
    u8 marker[4] = {0};
    const u32 markerOffset = offsetof(struct PokemonStorage, boxWallpapers) + BASE_BOXES_COUNT;
    const u32 markerSector = SECTOR_ID_PKMN_STORAGE_START + markerOffset / SECTOR_DATA_SIZE;
    for (i = 0; i < NUM_SECTORS_PER_SLOT; ++i)
    {
        struct SaveSector *sector = &gSaveDataBuffer;
        if (!RogueExpandedFlash_Read((slot * NUM_SECTORS_PER_SLOT + i) * ROGUE_EXPANDED_SECTOR_SIZE,
                                     sector, sizeof(*sector)))
            return FALSE;
        if (sector->signature != SECTOR_SIGNATURE || sector->id >= NUM_SECTORS_PER_SLOT
         || sector->counter != generation || (seen & (1 << sector->id)))
            return FALSE;
        seen |= 1 << sector->id;
        if (sector->id == SECTOR_ID_SAVEBLOCK2)
            memcpy(&trainerId, sector->data + offsetof(struct SaveBlock2, playerTrainerId), sizeof(trainerId));
        if (sector->id == markerSector)
            memcpy(marker, sector->data + markerOffset % SECTOR_DATA_SIZE, sizeof(marker));
    }
    if (seen != (1 << NUM_SECTORS_PER_SLOT) - 1)
        return FALSE;
    if (!IsMarked(marker))
        return TRUE;
    return marker[3] == HEADER_VERSION && (ValidateCommit(0, generation, trainerId) || ValidateCommit(1, generation, trainerId));
}

struct BoxPokemon *RogueExpanded_GetBox(u8 boxId)
{
    struct ExpandedCache *cache;
    u32 start, end, sector, copyStart, copyEnd;
    u8 i;
    if (boxId < BASE_BOXES_COUNT)
        return gPokemonStoragePtr->boxes[boxId];
    if (boxId >= EXPANDED_TOTAL_BOXES_COUNT)
        return NULL;
    if (!sExpanded.initialized || !RogueExpandedFlash_IsAvailable())
        RogueExpandedFlash_Fatal(EXPANDED_ERROR_BACKEND);
    for (i = 0; i < 2; ++i)
        if (sExpanded.cache[i].boxId == boxId)
            return sExpanded.cache[i].mons;
    // Keep the box used by the PC/summary screen resident while scanning other boxes.
    if (sExpanded.cache[0].boxId == NO_BOX)
        i = 0;
    else if (sExpanded.cache[1].boxId == NO_BOX)
        i = 1;
    else if (sExpanded.cache[0].boxId == gPokemonStoragePtr->currentBox)
        i = 1;
    else if (sExpanded.cache[1].boxId == gPokemonStoragePtr->currentBox)
        i = 0;
    else
        i = sExpanded.nextVictim;
    cache = &sExpanded.cache[i];
    if (!FlushCache(cache))
        RogueExpandedFlash_Fatal(EXPANDED_ERROR_WRITE);
    start = (boxId - BASE_BOXES_COUNT) * sizeof(cache->mons);
    end = start + sizeof(cache->mons);
    for (sector = start / ROGUE_EXPANDED_SECTOR_SIZE; sector <= (end - 1) / ROGUE_EXPANDED_SECTOR_SIZE; ++sector)
    {
        if (!ReadLogicalSector(sector))
            RogueExpandedFlash_Fatal(EXPANDED_ERROR_READ);
        copyStart = max(start, sector * ROGUE_EXPANDED_SECTOR_SIZE);
        copyEnd = min(end, (sector + 1) * ROGUE_EXPANDED_SECTOR_SIZE);
        memcpy((u8 *)cache->mons + copyStart - start, Scratch() + copyStart % ROGUE_EXPANDED_SECTOR_SIZE, copyEnd - copyStart);
    }
    cache->boxId = boxId;
    cache->originalHash = Hash(cache->mons, sizeof(cache->mons));
    sExpanded.nextVictim = i ^ 1;
    return cache->mons;
}

u8 *RogueExpanded_GetName(u8 boxId)
{
    if (boxId < BASE_BOXES_COUNT)
        return gPokemonStoragePtr->boxNames[boxId];
    if (boxId < EXPANDED_TOTAL_BOXES_COUNT)
        return sExpanded.metadata.names[boxId - BASE_BOXES_COUNT];
    return NULL;
}

u8 RogueExpanded_GetWallpaper(u8 boxId)
{
    if (boxId < BASE_BOXES_COUNT)
        return gPokemonStoragePtr->boxWallpapers[boxId];
    return boxId < EXPANDED_TOTAL_BOXES_COUNT ? sExpanded.metadata.wallpapers[boxId - BASE_BOXES_COUNT] : 0;
}

void RogueExpanded_SetWallpaper(u8 boxId, u8 wallpaper)
{
    if (boxId < BASE_BOXES_COUNT)
        gPokemonStoragePtr->boxWallpapers[boxId] = wallpaper;
    else if (boxId < EXPANDED_TOTAL_BOXES_COUNT)
        sExpanded.metadata.wallpapers[boxId - BASE_BOXES_COUNT] = wallpaper;
}

bool8 RogueExpanded_PrepareSave(u32 generation)
{
    struct ExpandedHeader header;
    u32 sector, remaining, payloadOffset, hash = 2166136261u;
    u8 target;
    if (!sExpanded.initialized || !RogueExpandedFlash_IsAvailable())
        return FALSE;
    if (!FlushCache(&sExpanded.cache[0]) || !FlushCache(&sExpanded.cache[1]))
        return FALSE;
    target = sExpanded.activeCommit == 0 || (sExpanded.activeCommit == NO_COMMIT && sExpanded.protectedCommit == 0) ? 1 : 0;
    // Invalidate the destination header before touching its payload.
    memset(Scratch(), 0xFF, ROGUE_EXPANDED_SECTOR_SIZE);
    if (!RogueExpandedFlash_WriteSector(CommitAddress(target) / ROGUE_EXPANDED_SECTOR_SIZE, Scratch()))
        return FALSE;
    for (sector = 0; sector < EXTRA_PAYLOAD_SECTORS; ++sector)
    {
        u32 metaStart, metaEnd;
        if (!ReadLogicalSector(sector))
            return FALSE;
        payloadOffset = sector * ROGUE_EXPANDED_SECTOR_SIZE;
        remaining = min(ROGUE_EXPANDED_SECTOR_SIZE, EXTRA_PAYLOAD_BYTES - payloadOffset);
        metaStart = max(payloadOffset, EXTRA_BOX_BYTES);
        metaEnd = min(payloadOffset + remaining, EXTRA_PAYLOAD_BYTES);
        if (metaStart < metaEnd)
            memcpy(Scratch() + metaStart - payloadOffset, (u8 *)&sExpanded.metadata + metaStart - EXTRA_BOX_BYTES, metaEnd - metaStart);
        hash = HashBytes(hash, Scratch(), remaining);
        if (!RogueExpandedFlash_WriteSector(CommitAddress(target) / ROGUE_EXPANDED_SECTOR_SIZE + 1 + sector, Scratch()))
            return FALSE;
    }
    memset(&header, 0, sizeof(header));
    header.magic = HEADER_MAGIC;
    header.version = HEADER_VERSION;
    header.generation = generation;
    header.trainerId = TrainerId();
    header.length = EXTRA_PAYLOAD_BYTES;
    header.payloadHash = hash;
    header.headerHash = Hash(&header, offsetof(struct ExpandedHeader, headerHash));
    header.committed = HEADER_COMMITTED;
    memset(Scratch(), 0xFF, ROGUE_EXPANDED_SECTOR_SIZE);
    memcpy(Scratch(), &header, sizeof(header));
    if (!RogueExpandedFlash_WriteSector(CommitAddress(target) / ROGUE_EXPANDED_SECTOR_SIZE, Scratch())
     || !ValidateCommit(target, generation, header.trainerId))
        return FALSE;
    sExpanded.pendingCommit = target;
    sExpanded.pendingGeneration = generation;
    memcpy(sExpanded.previousMarker, BaseMarker(), 4);
    BaseMarker()[0] = 'R';
    BaseMarker()[1] = '9';
    BaseMarker()[2] = '9';
    BaseMarker()[3] = HEADER_VERSION;
    return TRUE;
}

void RogueExpanded_CompleteSave(bool8 success)
{
    if (sExpanded.pendingCommit == NO_COMMIT)
        return;
    if (success)
    {
        sExpanded.activeCommit = sExpanded.pendingCommit;
        sExpanded.generation = sExpanded.pendingGeneration;
        sExpanded.trainerId = TrainerId();
        memset(sExpanded.workingSectors, 0, sizeof(sExpanded.workingSectors));
    }
    else
    {
        memcpy(BaseMarker(), sExpanded.previousMarker, 4);
    }
    sExpanded.pendingCommit = NO_COMMIT;
}
