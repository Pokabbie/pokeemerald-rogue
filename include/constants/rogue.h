#ifndef GUARD_ROGUE_H
#define GUARD_ROGUE_H

//#define ROGUE_DEBUG

//#define ROGUE_EXPANSION

#define ROGUE_SUPPORT_QUICK_SAVE

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define ROGUE_ROUTE_COUNT 7

struct RogueRunData
{
    u16 currentRoomIdx;
    u16 nextRestStopRoomIdx;
    u16 specialEncounterCounter;
    u8 currentRouteIndex;
    u16 wildEncounters[6];
    u16 fishingEncounters[2];
    u16 routeHistoryBuffer[4];
    u16 wildEncounterHistoryBuffer[2];
};

struct RogueHubData
{
    u32 money;
    u16 registeredItem;
    u16 playTimeHours;
    u8 playTimeMinutes;
    u8 playTimeSeconds;
    u8 playTimeVBlanks;
    //struct Pokemon playerParty[PARTY_SIZE];
    //struct ItemSlot bagPocket_Items[BAG_ITEMS_COUNT];
    //struct ItemSlot bagPocket_KeyItems[BAG_KEYITEMS_COUNT];
    //struct ItemSlot bagPocket_PokeBalls[BAG_POKEBALLS_COUNT];
    //struct ItemSlot bagPocket_TMHM[BAG_TMHM_COUNT];
    //struct ItemSlot bagPocket_Berries[BAG_BERRIES_COUNT];
};

// Can at most be 384 bytes
struct RogueSaveData // 27 Bytes
{
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    u32 rngSeed;
    struct RogueRunData runData;
    struct RogueHubData hubData;
#endif
};

ROGUE_STATIC_ASSERT(sizeof(struct RogueSaveData) <= 384, RogueSaveDataSize);

struct RogueRouteMap
{
    u16 layout;
    u16 group;
    u16 num;
};

struct RogueRouteData
{
    u8 dropRarity;
    struct RogueRouteMap map;
    const u8 wildTypeTable[3];
};

struct RogueEncounterMap
{
    u16 encounterSpecies;
    u16 layout;
    u16 group;
    u16 num;
};

struct RogueEncounterData
{
    u8 mapCount;
    const struct RogueEncounterMap* mapTable;
};

struct SpeciesTable
{
    u8 wildSpeciesCount;
    const u16* wildSpecies;
    u8 trainerSpeciesCount;
    const u16* trainerSpecies;
    ///*0x00*/ u8 partyFlags;
    ///*0x01*/ u8 trainerClass;
    ///*0x02*/ u8 encounterMusic_gender; // last bit is gender
    ///*0x03*/ u8 trainerPic;
    ///*0x04*/ u8 trainerName[12];
    ///*0x10*/ u16 items[MAX_TRAINER_ITEMS];
    ///*0x18*/ bool8 doubleBattle;
    ///*0x1C*/ u32 aiFlags;
    ///*0x20*/ u8 partySize;
    ///*0x24*/ union TrainerMonPtr party;
};

struct RogueMonPreset
{
    u16 heldItem;
    u16 abilityNum;
    u16 moves[MAX_MON_MOVES];
};

struct RogueMonPresetCollection
{
    u16 presetCount;
    const struct RogueMonPreset* presets;
};

extern const struct SpeciesTable gRogueSpeciesTable[];
extern const struct RogueRouteData gRogueRouteTable[ROGUE_ROUTE_COUNT];
extern const struct RogueEncounterData gRogueSpecialEncounterInfo;
extern const struct RogueMonPresetCollection gPresetMonTable[NUM_SPECIES];

#endif  // GUARD_ROGUE_H
