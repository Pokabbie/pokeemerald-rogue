#ifndef GUARD_ROGUE_H
#define GUARD_ROGUE_H

enum RogueAdvPathRoomType
{
    ADVPATH_ROOM_NONE,
    ADVPATH_ROOM_ROUTE,
    ADVPATH_ROOM_RESTSTOP,
    ADVPATH_ROOM_LEGENDARY,
    ADVPATH_ROOM_MINIBOSS,
    ADVPATH_ROOM_WILD_DEN,
    ADVPATH_ROOM_GAMESHOW,
    ADVPATH_ROOM_GRAVEYARD,
    ADVPATH_ROOM_LAB,
    ADVPATH_ROOM_COUNT,

    // Special cases are excluded from count
    ADVPATH_ROOM_BOSS,
};

struct RogueAdvPathRoomParams
{
    u8 roomIdx;
    union
    {
        struct
        {
            u8 difficulty;
        } route;
        struct
        {
            u16 species;
        } wildDen;
    } perType;
};

struct RogueAdvPathNode
{
    u8 roomType;
    u8 isBridgeActive : 1;
    u8 isLadderActive : 1;
    struct RogueAdvPathRoomParams roomParams;
};

struct RogueAdvPath
{
    u8 currentNodeX;
    u8 currentNodeY;
    u8 currentColumnCount;
    u8 currentRoomType;
    u8 isOverviewActive : 1;
    u8 justGenerated : 1;
    struct RogueAdvPathRoomParams currentRoomParams;
    struct RogueAdvPathNode nodes[ROGUE_MAX_ADVPATH_ROWS * ROGUE_MAX_ADVPATH_COLUMNS];
};

struct RogueQuestReward
{
    u8 type;
    u16 params[3];
    const u8* previewText;
    const u8* giveText;
};

struct RogueQuestConstants
{
    const u8 title[QUEST_TITLE_LENGTH];
    const u8 desc[QUEST_DESC_LENGTH];
    const u8 sortIndex;
    const u16 flags;
    struct RogueQuestReward rewards[QUEST_MAX_REWARD_COUNT];
    const u16 unlockedQuests[QUEST_MAX_FOLLOWING_QUESTS];
    const u16 unlockedShopRewards[QUEST_MAX_ITEM_SHOP_REWARD_COUNT]; // Specifically into the ROGUE_SHOP_QUEST_REWARDS shop
};

struct RogueQuestState
{
    union
    {
        u8 byte[2];
        u16 half;
    } data;
    u8 isUnlocked : 1;
    u8 isCompleted : 1;
    u8 isValid : 1;
    u8 isPinned : 1;
    u8 hasPendingRewards : 1;
    u8 hasNewMarker : 1;
};

struct RogueCampaignState
{
    u8 isUnlocked : 1;
    u8 isCompleted : 1;
    u16 bestScore;
};

struct RogueGlobalData
{
    u8 safairShinyBufferHead;
    u16 safariShinyBuffer[6];
    u32 safariShinyPersonality;
    struct RogueQuestState questStates[QUEST_CAPACITY];
    struct RogueCampaignState campaignData[ROGUE_CAMPAIGN_COUNT];
};

//ROGUE_STATIC_ASSERT(sizeof(struct RogueQuestState) <= sizeof(u8), RogueQuestState);


struct RogueCampaignData_LowBst
{
    u16 scoreSpecies;
};

struct RogueCampaignData_Generic
{
    u16 score;
};

struct RogueRunData
{
    u16 currentRoomIdx;
    u16 currentDifficulty;
    u8 currentRouteIndex;
    u8 currentLevelOffset;
#ifdef ROGUE_EXPANSION
    u8 megasEnabled : 1;
    u8 zMovesEnabled : 1;
#endif
    u16 wildEncounters[9];
    u16 fishingEncounters[2];
    u16 routeHistoryBuffer[12];
    u16 legendaryHistoryBuffer[6];
    u16 miniBossHistoryBuffer[6];
    u16 bossHistoryBuffer[15];
    u16 wildEncounterHistoryBuffer[3];
    union
    {
        struct RogueCampaignData_Generic generic;
        struct RogueCampaignData_LowBst lowBst;
    } campaignData;
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
    u32 rngSeed;
    struct RogueRunData runData;
    struct RogueHubData hubData;
};

ROGUE_STATIC_ASSERT(sizeof(struct RogueSaveData) <= 384, RogueSaveDataSize);

struct RogueRouteMap
{
    u16 layout;
    u16 group;
    u16 num;
};

struct RogueRouteEncounter
{
    u8 dropRarity;
    u16 mapFlags;
    struct RogueRouteMap map;
    const u8 wildTypeTable[3];
};

struct RogueRouteData
{
    u8 routeCount;
    const struct RogueRouteEncounter* routes;
};

struct RogueEncounterMap
{
    u16 encounterId;
    u16 layout;
    u16 group;
    u16 num;
};

struct RogueEncounterData
{
    u8 mapCount;
    const struct RogueEncounterMap* mapTable;
};

struct RogueTrainerEncounter
{
    u16 gfxId;
    u16 trainerId;
    u16 victorySetFlag;
    u16 trainerFlags;
    u16 partyFlags;
    u16 querySpeciesCount;
    const u16* querySpecies;
    u16 incTypes[3];
    u16 excTypes[3];
};

struct RogueTrainerData
{
    u8 count;
    const struct RogueTrainerEncounter* trainers;
};

struct SpeciesTable
{
    u8 wildSpeciesCount;
    const u16* wildSpecies;
    u8 trainerSpeciesCount;
    const u16* trainerSpecies;
};

struct RogueMonPreset
{
    bool8 allowMissingMoves;
    u16 heldItem;
    u16 abilityNum;
    u16 hiddenPowerType;
    u16 flags;
    u16 moves[MAX_MON_MOVES];
};

struct RogueMonPresetCollection
{
    u16 flags;
    u16 presetCount;
    u16 movesCount;
    const struct RogueMonPreset* presets;
    const u16* moves;
};

#ifdef ROGUE_FEATURE_AUTOMATION
struct RogueAutomationHeader
{
    u32 commBufferCapacity;
    u16* commBuffer;
};

extern const struct RogueAutomationHeader gRogueAutomationHeader;
#endif

extern const struct SpeciesTable gRogueSpeciesTable[];
extern const struct RogueRouteData gRogueRouteTable;
extern const struct RogueEncounterData gRogueLegendaryEncounterInfo;
extern const struct RogueEncounterData gRogueRestStopEncounterInfo;
extern const struct RogueTrainerData gRogueBossEncounters;
extern const struct RogueTrainerData gRogueMiniBossEncounters;
extern const struct RogueMonPresetCollection gPresetMonTable[NUM_SPECIES];
extern const struct RogueQuestConstants gRogueQuests[QUEST_CAPACITY + 1];
extern const u8 gRogueTypeWeatherTable[];
extern const struct RogueEncounterMap gRogueTypeToEliteRoom[];


#endif  // GUARD_ROGUE_H
