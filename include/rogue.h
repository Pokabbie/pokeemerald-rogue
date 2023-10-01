#ifndef GUARD_ROGUE_H
#define GUARD_ROGUE_H

// Extra data for pokemon in party
struct RoguePartyMon
{
    bool8 hasPendingEvo : 1;
    u8 lastPopupLevel : 7;

    bool8 isSafariIllegal : 1;
    u8 pad0 : 7;

    u8 pad1[2];
};

STATIC_ASSERT(sizeof(struct RoguePartyMon) == 4, SizeOfRoguePartyMon);

// Minimal version of mon info to allow easy tracking for safari area
// Split into 32bit blocks
struct RogueSafariMon
{
    u32 species     : 16;
    u32 hpIV        : 5;
    u32 attackIV    : 5;
    u32 defenseIV   : 5;
    u32 shinyFlag   : 1;

    u32 speedIV     : 5;
    u32 spAttackIV  : 5;
    u32 spDefenseIV : 5;
    u32 pokeball    : 5; // 31 balls for EX
    u32 nature      : 5; // 24 natures
    u32 abilityNum  : 2; // technically only needs to be 1 for Vanilla
    u32 genderFlag  : 1;
    u32 unused0     : 4;
    
    // Adding this makes it jump from 8 bytes per mon to 20
    u8 nickname[POKEMON_NAME_LENGTH];
    u8 priorityCounter;
    u8 unused2;
};

//STATIC_ASSERT(sizeof(struct RogueSafariMon) == 8, SizeOfRogueSafariMon);
STATIC_ASSERT(sizeof(struct RogueSafariMon) == 20, SizeOfRogueSafariMon);

// Adventure Path settings
//
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

struct RogueAdvPathRoom
{
    struct Coords8 coords;
    struct RogueAdvPathRoomParams roomParams;
    u16 rngSeed;
    u8 roomType;
    u8 connectionMask;
};

struct RogueAdvPath
{
    struct RogueAdvPathRoomParams currentRoomParams;
    struct RogueAdvPathRoom rooms[ROGUE_ADVPATH_ROOM_CAPACITY];
    u16 routeHistoryBuffer[12]; // TODO - Remove these
    u16 legendaryHistoryBuffer[6];
    u16 miniBossHistoryBuffer[6];
    u8 currentRoomId;
    u8 currentRoomType;
    u8 roomCount;
    u8 pathLength;
    s8 pathMinY;
    s8 pathMaxY;
    u8 isOverviewActive : 1;
    u8 justGenerated : 1;
};


// Adventure Path generation
//
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

struct RogueHubMap
{
    struct Coords8 areaCoords[HUB_AREA_COUNT];
    u8 areaBuiltFlags[1 + HUB_AREA_COUNT / 8];
    u8 upgradeFlags[1 + HUB_UPGRADE_COUNT / 8];
};

struct RogueCampaignData_LowBst
{
    u16 scoreSpecies;
};

struct RogueCampaignData_Generic
{
    u16 score;
};

struct RogueWildEncounters
{
    u16 species[WILD_ENCOUNTER_TOTAL_CAPACITY];
    u8 catchCounts[WILD_ENCOUNTER_TOTAL_CAPACITY];
};

struct RogueRunData
{
    struct RogueWildEncounters wildEncounters;
    u16 bossTrainerNums[ROGUE_MAX_BOSS_COUNT];
    u8 completedBadges[ROGUE_MAX_BOSS_COUNT];
    union
    {
        struct RogueCampaignData_Generic generic;
        struct RogueCampaignData_LowBst lowBst;
    } campaignData;
    u16 baseSeed;
    u8 adventureRoomId;
    u16 enteredRoomCounter;
    u16 currentDifficulty;
    u8 currentRouteIndex;
    u8 currentLevelOffset;
#ifdef ROGUE_EXPANSION
    u8 megasEnabled : 1;
    u8 zMovesEnabled : 1;
#endif
    bool8 isQuickSaveValid : 1;
};

struct RogueHubArea
{
    const u32* iconImage;
    const u32* iconPalette;
    const u8* descText;
    const u8 areaName[16];
    u8 connectionWarps[4][2];
    u8 requiredUpgrades[HUB_UPGRADE_MAX_REQUIREMENTS];
    u16 primaryMapNum;
    u16 primaryMapLayout;
    u8 primaryMapGroup;
    u8 buildCost;
};

struct RogueAreaUpgrade
{
    const u32* iconImage;
    const u32* iconPalette;
    const u8* descText;
    const u8 upgradeName[24];
    u8 requiredUpgrades[HUB_UPGRADE_MAX_REQUIREMENTS];
    u8 targetArea;
    u8 buildCost;
};

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

struct RogueBattleMusicRedirect
{
    u16 redirectParam;
    u8 redirectType;
    u8 musicPlayer;
};

struct RogueBattleMusic
{
    struct RogueBattleMusicRedirect const* redirects;
    u16 redirectCount;
    u16 encounterMusic;
    u16 battleMusic;
    u16 victoryMusic;
};

struct RogueTeamGeneratorSubset
{
    u32 includedTypeMask;
    u32 excludedTypeMask;
    u8 maxSamples;
};

struct RogueTeamGenerator
{
    u16 const* queryScriptOverride;
    u16 const* queryScriptPost;
    u16 const* weightScript;
    struct RogueTeamGeneratorSubset const* subsets;
    u8 subsetCount;
    u8 preferredGender;
};

struct RogueTrainer
{
    u8 const* trainerName;
    u8 const* const* encounterText; // TRAINER_STRING_COUNT * N
    u32 trainerFlags;
    u16 objectEventGfx;
    u16 typeAssignment;
    u16 typeAssignmentGroup;
    u8 trainerClass;
    u8 trainerPic;
    u8 preferredWeather;
    u8 musicPlayer;
    u8 encounterTextCount;
    struct RogueTeamGenerator teamGenerator;
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

// Rogue Multiplayer
//
struct RogueNetHandshake
{
    u32 token;
    u8 request;
};

struct RogueNetGameState
{
    u8 temp1;
    u8 temp2;
};

struct RogueNetPlayer
{
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
    struct Coords16 playerPos;
    struct Coords8 partnerPos;
    u16 networkId;
    u16 partnerMon;
    u8 facingDirection;
    u8 partnerFacingDirection;
    u8 playerFlags;
    s8 mapGroup;
    s8 mapNum;
};

struct RogueNetMultiplayer
{
    struct RogueNetPlayer players[NET_PLAYER_CAPACITY];
    struct RogueNetGameState gameState;
    struct RogueNetHandshake pendingHandshake;
    u8 netRequestState;
    u8 netCurrentState;
};

// Rogue Assistant
//

struct RogueAssistantHeader
{
    u8 rogueVersion;
    u8 rogueDebug;
    u32 netMultiplayerSize;
    u32 netHandshakeOffset;
    u32 netHandshakeSize;
    u32 netGameStateOffset;
    u32 netGameStateSize;
    u32 netPlayerOffset;
    u32 netPlayerSize;
    u32 netPlayerCount;
    u32 netRequestStateOffset;
    u32 netCurrentStateOffset;
    void const* saveBlock1Ptr;
    void const* saveBlock2Ptr;
    void const* rogueBlockPtr;
    void const* assistantState;
    void const* multiplayerPtr;
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

struct RoguePokedexVariant
{
    const u8* displayName;
    const u16* speciesList;
    u16 speciesCount;
    u8 genLimit;
};

struct RoguePokedexRegion
{
    const u8* displayName;
    const u16* variantList;
    u16 variantCount;
};

struct RogueDifficultyConfig
{
    u8 toggleBits[1 + (DIFFICULTY_TOGGLE_COUNT) / 8];
    u8 rangeValues[DIFFICULTY_RANGE_COUNT];
};

struct RogueDebugConfig
{
    u8 toggleBits[1 + (DEBUG_TOGGLE_COUNT) / 8];
    u8 rangeValues[DEBUG_RANGE_COUNT];
};

struct RogueRideMonState
{
    u16 monGfx;
    u8 flyingHeight : 6;
    u8 whistleType : 1;
    bool8 flyingState : 1;
};

struct RogueSaveBlock
{
    u16 saveVersion;
    u8 currentBlockFormat;

    // Everything past this point is not safe to read until the block format
    // has been adjusted
    struct RogueQuestState questStates[QUEST_CAPACITY];
    struct RogueCampaignState campaignData[ROGUE_CAMPAIGN_COUNT];
    struct RogueSafariMon safariMons[45];
    struct RogueSafariMon safariLegends[15];
    struct RogueHubMap hubMap;
    struct RogueDifficultyConfig difficultyConfig;
    u16 timeOfDayMinutes;
    u8 seasonCounter;
};

extern const struct RogueRouteData gRogueRouteTable;
extern const struct RogueEncounterData gRogueLegendaryEncounterInfo;
extern const struct RogueEncounterData gRogueRestStopEncounterInfo;

extern const struct RogueTrainer gRogueTrainers[];
extern const u16 gRogueTrainerCount;
extern const struct RogueBattleMusic gRogueTrainerMusic[];

extern const struct RogueMonPresetCollection gPresetMonTable[NUM_SPECIES];


extern const struct RoguePokedexVariant gPokedexVariants[POKEDEX_VARIANT_COUNT];
extern const struct RoguePokedexRegion gPokedexRegions[POKEDEX_REGION_COUNT];

extern const struct RogueQuestConstants gRogueQuests[QUEST_CAPACITY + 1];
extern const struct RogueHubArea gRogueHubAreas[HUB_AREA_COUNT];
extern const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT];
extern const u8 gRogueTypeWeatherTable[];
extern const struct RogueEncounterMap gRogueTypeToEliteRoom[];

extern const struct RogueAdventureSettings gRogueAdventureSettings[];


#endif  // GUARD_ROGUE_H
