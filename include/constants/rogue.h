#ifndef GUARD_ROGUE_H
#define GUARD_ROGUE_H

#define ROGUE_DEBUG

#define ROGUE_ROUTE_FIELD           0
#define ROGUE_ROUTE_FOREST          1
#define ROGUE_ROUTE_CAVE            2
#define ROGUE_ROUTE_MOUNTAIN        3
#define ROGUE_ROUTE_WATERFRONT      4
#define ROGUE_ROUTE_URBAN           5

#define ROGUE_ROUTE_START   ROGUE_ROUTE_FIELD
#define ROGUE_ROUTE_END     ROGUE_ROUTE_URBAN
#define ROGUE_ROUTE_COUNT (ROGUE_ROUTE_END - ROGUE_ROUTE_START + 1)

struct RogueRunData
{
    u16 currentRoomIdx;
    u16 nextRestStopRoomIdx;
    u8 currentRouteType;
    u16 wildEncounters[5];
    u16 fishingEncounters[2];
};

struct RogueHubData
{
    u32 money;
    u16 registeredItem;
};

struct RogueRouteMap
{
    u16 layout;
    u16 group;
    u16 num;
};

struct RogueRouteData
{
    u8 wildTypeTableCount;
    u8 mapCount;
    const u8* wildTypeTable;
    const struct RogueRouteMap* mapTable;
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
extern const struct RogueRouteData gRogueSpecialEncounterInfo;
extern const struct RogueMonPresetCollection gPresetMonTable[NUM_SPECIES];

#endif  // GUARD_ROGUE_H
