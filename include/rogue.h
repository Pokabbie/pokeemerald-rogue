#ifndef GUARD_ROGUE_H
#define GUARD_ROGUE_H

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
            bool8 shinyState;
        } wildDen;
        struct
        {
            bool8 shinyState;
        } legendary;
        struct 
        {
            u16 trainerNum;
        } boss;
        struct 
        {
            u16 trainerNum;
        } miniboss;
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

struct RogueAdvPathGenerator // Attach to mode Difficult Option???
{
   u8 minLength;
   u8 maxLength;
   u16 roomWeights[ADVPATH_ROOM_WEIGHT_COUNT];
   u16 maxRoomCount[ADVPATH_ROOM_WEIGHT_COUNT];
   u16 subRoomWeights[ADVPATH_SUBROOM_WEIGHT_COUNT];
};


struct RogueAdventurePhase
{
    u8 levelCap;
    u8 levelStep;
    u16 bossTrainerFlagsInclude;
    u16 bossTrainerFlagsExclude;
    struct RogueAdvPathGenerator pathGenerator;
};

struct RogueAdventureSettings
{
    const struct RogueAdventurePhase* phases;
    u8 phaseCount;
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
    u8 completedBadges[ROGUE_MAX_BOSS_COUNT];
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
    u16 trainerFlags;
    u16 partyFlags;
    u16 querySpeciesCount;
    const u16* querySpecies;
    u16 incTypes[3];
    u16 excTypes[3];
};

struct RogueTrainerData
{
    // old
    u8 count;
    const struct RogueTrainerEncounter* trainers;
};


struct RogueTrainerMonGenerator
{
    u8 monCount;
    u8 targetLevel;
    u8 incTypes[2];
    u8 excTypes[2];
    u16 generatorFlags;
    u16 customSpeciesCount;
    const u16* customSpecies;
};

struct RogueTrainer
{
    u8 trainerClass;
    u8 encounterMusic_gender; // last bit is gender
    u8 trainerPic;
    u8 preferredWeather;
    u8 trainerName[12];
    u16 objectEventGfx;
    u16 trainerFlags;
    struct RogueTrainerMonGenerator monGenerators[3];
    struct RogueTrainerMonGenerator aceMonGenerators[1];
};

struct RogueTrainerCollection
{
    u16 bossCount;
    u16 minibossCount;
    const struct RogueTrainer* boss;
    const struct RogueTrainer* miniboss;
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
    u16 abilityNum; // not actually abilityNum, should be the abilityId
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

struct RogueAssistantHeader
{
    u32 inCommCapacity;
    u32 outCommCapacity;
    u8* inCommBuffer;
    u8* outCommBuffer;
};

extern const struct RogueAssistantHeader gRogueAssistantHeader;

#ifdef ROGUE_FEATURE_AUTOMATION
struct RogueAutomationHeader
{
    u32 commBufferCapacity;
    u16* commBuffer;
};

extern const struct RogueAutomationHeader gRogueAutomationHeader;
#endif

struct PokemonObjectEventInfo
{
    const u32* defaultPic;
    const u32* shinyPic;
    u8 width;
    u8 height;
    u8 defaultPaletteOffset;
    u8 shinyPaletteOffset;
};

extern const struct RogueRouteData gRogueRouteTable;
extern const struct RogueEncounterData gRogueLegendaryEncounterInfo;
extern const struct RogueEncounterData gRogueRestStopEncounterInfo;
extern const struct RogueTrainerCollection gRogueTrainers;

extern const struct RogueMonPresetCollection gPresetMonTable[NUM_SPECIES];
extern const struct RogueQuestConstants gRogueQuests[QUEST_CAPACITY + 1];
extern const u8 gRogueTypeWeatherTable[];
extern const struct RogueEncounterMap gRogueTypeToEliteRoom[];

extern const struct RogueAdventureSettings gRogueAdventureSettings[];


#endif  // GUARD_ROGUE_H
