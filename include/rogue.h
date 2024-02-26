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

struct RogueRoamerMon
{
    u32 species     : 16;
    u32 hpIV        : 5;
    u32 attackIV    : 5;
    u32 defenseIV   : 5;
    u32 shinyFlag   : 1;

    u32 speedIV     : 5;
    u32 spAttackIV  : 5;
    u32 spDefenseIV : 5;

    u32 abilityNum  : 2; // technically only needs to be 1 for Vanilla
    u32 genderFlag  : 1;
    u32 hpPerc      : 7; // stored as a percentage
    u32 encounerCount : 4;
    u32 unused0     : 3; 

    u32 status      : 16;
    u32 unused1     : 16; 
};

STATIC_ASSERT(sizeof(struct RogueRoamerMon) == 12, SizeOfRogueRoamerMon);

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
            u16 species;
            bool8 shinyState;
        } honeyTree;
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
    u16 routeHistoryBuffer[18];
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

struct OLDRogueQuestState
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

struct RogueQuestStateNEW
{
    u32 stateFlags : 16;
    u32 highestCompleteDifficulty : 3;
    u32 highestCollectedRewardDifficulty : 3;
    u32 unused : 12;
};

struct RogueCampaignState
{
    u8 isUnlocked : 1;
    u8 isCompleted : 1;
    u16 bestScore;
};

struct RogueHubDecoration
{
    u8 x;
    u8 y;
    u8 objectId;
    u8 unused;
};

struct RogueHubMap
{
    struct Coords8 areaCoords[HUB_AREA_COUNT];
    struct RogueHubDecoration homeDecorations[HOME_DECOR_TOTAL_COUNT];
    u8 homeRegionStyles[HOME_REGION_COUNT];
    u8 areaBuiltFlags[1 + HUB_AREA_COUNT / 8];
    u8 upgradeFlags[1 + HUB_UPGRADE_COUNT / 8];
    u16 weatherState;
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
    struct RogueRoamerMon roamer;
    u16 species[WILD_ENCOUNTER_TOTAL_CAPACITY];
    u8 catchCounts[WILD_ENCOUNTER_TOTAL_CAPACITY];
    u8 roamerActiveThisPath : 1;
};

// We just want this to be the same size as box pokemon so we can reserve the memory and cast later
struct RogueBoxPokemonFacade
{
    u8 data[80];
};

struct RogueRunData
{
    struct RogueWildEncounters wildEncounters;
    struct RogueBoxPokemonFacade daycarePokemon[DAYCARE_SLOT_COUNT];
    u16 bossTrainerNums[ROGUE_MAX_BOSS_COUNT];
    u16 rivalSpecies[ROGUE_RIVAL_TOTAL_MON_COUNT];
    u16 legendarySpecies[ADVPATH_LEGEND_COUNT];
    u16 teamEncounterRooms[ADVPATH_TEAM_ENCOUNTER_COUNT];
    u16 dynamicTRMoves[NUM_TECHNICAL_RECORDS];
    u16 partyHeldItems[PARTY_SIZE];
    u16 dynamicTrainerNums[ROGUE_MAX_ACTIVE_TRAINER_COUNT];
    u8 legendaryDifficulties[ADVPATH_LEGEND_COUNT];
    u8 teamEncounterDifficulties[ADVPATH_TEAM_ENCOUNTER_COUNT];
    u8 rivalEncounterDifficulties[ROGUE_RIVAL_MAX_ROUTE_ENCOUNTERS];
    u8 completedBadges[ROGUE_MAX_BOSS_COUNT];
    u8 honeyTreePokeblock[POKEBLOCK_ITEM_COUNT];
    u8 activeEvoItemFlags[8];
    u8 activeFormItemFlags[16]; // technically this isn't needed for Vanilla
    union
    {
        struct RogueCampaignData_Generic generic;
        struct RogueCampaignData_LowBst lowBst;
    } campaignData;
    u16 baseSeed;
    u16 rivalTrainerNum;
    u16 teamEncounterNum;
    u16 enteredRoomCounter;
    u16 currentDifficulty;
    u8 shrineSpawnDifficulty;
    u8 adventureRoomId;
    u8 currentRouteIndex;
    u8 currentLevelOffset;
#ifdef ROGUE_EXPANSION
    u8 megasEnabled : 1;
    u8 zMovesEnabled : 1;
    u8 dynamaxEnabled : 1;
#endif
    bool8 isQuickSaveValid : 1;
    bool8 hasPendingRivalBattle : 1;
    bool8 rivalHasShiny : 1;
};

struct RogueHubArea
{
    const u32* iconImage;
    const u32* iconPalette;
    const u8* descText;
    const u8 areaName[ITEM_NAME_LENGTH];
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
    const u8 upgradeName[ITEM_NAME_LENGTH];
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
    u32 includedGenMask;
    u32 excludedGenMask;
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
    u32 classFlags;
    u32 trainerFlags;
    u16 objectEventGfx;
    u16 typeAssignment;
    u16 typeAssignmentGroup;
    u16 preferredPokeballItem;
    u16 potentialShinySpecies;
    u8 levelOverride;
    u8 trainerClass;
    u16 trainerPic;
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

struct RogueDifficultyConfig
{
    u8 toggleBits[CONFIG_TOGGLE_BYTE_COUNT];
    u8 rangeValues[CONFIG_RANGE_COUNT];
};

struct RogueDebugConfig
{
    u8 toggleBits[DEBUG_TOGGLE_BYTE_COUNT];
    u8 rangeValues[DEBUG_RANGE_COUNT];
};

// Rogue Multiplayer
//
struct RogueNetHubState
{
    struct RogueHubMap hubMap;
    struct RogueDifficultyConfig difficultyConfig;
    u16 timeOfDay;
    u8 season;
};

struct RogueNetAdventureState
{
    u16 baseSeed;
    u8 isRunActive : 1;
};

struct RogueNetGameState
{
    struct RogueNetHubState hub;
    struct RogueNetAdventureState adventure;
};

struct RogueNetPlayerProfile
{
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
    u16 preferredOutfitStyle[3]; // PLAYER_OUTFIT_STYLE_COUNT
    u8 networkId; // assigned by host
    u8 preferredOutfit;
    u8 fallbackOutfit;
    u8 isActive : 1;
};

struct RogueNetPlayerMovement
{
    struct Coords16 pos;
    u8 movementAction;
};

struct RogueNetPlayer
{
    struct RogueNetPlayerMovement movementBuffer[NET_PLAYER_MOVEMENT_BUFFER_SIZE];
    struct Coords16 playerPos;
    struct Coords8 partnerPos;
    u16 partnerMon;
    s8 mapGroup;
    s8 mapNum;
    u8 adventureTileNum;
    u8 adventureDifficulty;
    u8 playerFlags;
    u8 movementBufferHead;
    u8 currentElevation : 4;
    u8 facingDirection : 4;
    u8 partnerFacingDirection : 4;
};

struct RogueNetHandshake
{
    struct RogueNetPlayerProfile profile;
    u16 saveVersionId;
    u8 state;
    u8 playerId;
    u8 accepted : 1;
    u8 isVersionEx : 1;
};

struct RogueNetMultiplayer
{
    struct RogueNetPlayerProfile playerProfiles[NET_PLAYER_CAPACITY];
    struct RogueNetPlayer playerState[NET_PLAYER_CAPACITY];
    struct RogueNetGameState gameState;
    struct RogueNetHandshake pendingHandshake;
    u8 netRequestState;
    u8 netCurrentState;
    u8 localPlayerId;
    u8 localCounter;
};

// Rogue Assistant
//

struct RogueAssistantHeader
{
    u8 rogueVersion;
    u8 rogueDebug;
    u32 assistantConfirmSize;
    u32 assistantConfirmOffset;
    u32 netMultiplayerSize;
    u32 netHandshakeOffset;
    u32 netHandshakeSize;
    u32 netHandshakeStateOffset;
    u32 netHandshakePlayerIdOffset;
    u32 netGameStateOffset;
    u32 netGameStateSize;
    u32 netPlayerProfileOffset;
    u32 netPlayerProfileSize;
    u32 netPlayerStateOffset;
    u32 netPlayerStateSize;
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

struct RoguePokemonCompetitiveSet
{
    u16 moves[MAX_MON_MOVES];
    u16 flags;
    u16 heldItem;
    u16 ability;
    u8 hiddenPowerType;
    u8 nature;
};

struct RoguePokemonCompetitiveSetRules
{
    bool8 skipMoves : 1;
    bool8 skipHeldItem : 1;
    bool8 skipAbility : 1;
    bool8 skipHiddenPowerType : 1;
    bool8 skipNature : 1;
    bool8 allowMissingMoves : 1;
};

struct RoguePokemonProfile
{
    struct RoguePokemonCompetitiveSet const* competitiveSets;
    struct LevelUpMove const* levelUpMoves;
    u16 const* tutorMoves;
    u16 monFlags;
    u16 competitiveSetCount;
};

struct RogueRideMonState
{
    u16 monGfx;
    u16 desiredRideSpecies;
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
    struct OLDRogueQuestState questStates[QUEST_CAPACITY];
    struct RogueQuestStateNEW questStatesNEW[QUEST_SAVE_COUNT];
    struct RogueCampaignState campaignData[ROGUE_CAMPAIGN_COUNT];
    struct RogueSafariMon safariMons[45];
    struct RogueSafariMon safariLegends[15];
    struct RogueHubMap hubMap;
    struct RogueDifficultyConfig difficultyConfig;
    u16 timeOfDayMinutes;
    u8 seasonCounter;
};

struct RogueSpeciesBakedData
{
    u32 evolutionChainTypeFlags : 18;
    u32 eggSpecies : 11;
    u32 unused1 : 3;

    u32 evolutionCount : 8;
    u32 unused2 : 24;
};

struct RogueFollowMonGraphicsInfo
{
    struct ObjectEventGraphicsInfo const* objectEventGfxInfo;
    u16 const* normalPal;
    u16 const* shinyPal;
};

#ifndef ROGUE_EXPANSION
// Dud structs not defined in vanilla
//
struct FormChange
{
    u32 dud;
};

#endif

STATIC_ASSERT(sizeof(struct RogueSpeciesBakedData) == 8, SizeOfRogueSpeciesBakedData);

extern const struct RogueRouteData gRogueRouteTable;
extern const struct RogueEncounterData gRogueLegendaryEncounterInfo;
extern const struct RogueEncounterData gRogueTeamEncounterInfo;
extern const struct RogueEncounterData gRogueRestStopEncounterInfo;

extern const struct RogueTrainer gRogueTrainers[];
extern const u16 gRogueTrainerCount;
extern const struct RogueBattleMusic gRogueTrainerMusic[];

extern const struct RoguePokemonProfile gRoguePokemonProfiles[NUM_SPECIES];
extern u16 const gRoguePokemonHeldItemUsages[ITEMS_COUNT];
extern u16 const gRoguePokemonMoveUsages[MOVES_COUNT];

extern const struct RoguePokedexVariant gPokedexVariants[POKEDEX_VARIANT_COUNT];
extern const struct RoguePokedexRegion gPokedexRegions[POKEDEX_REGION_COUNT];

extern const struct RogueQuestConstants gRogueQuests[QUEST_CAPACITY + 1];
extern const struct RogueHubArea gRogueHubAreas[HUB_AREA_COUNT];
extern const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT];
extern const u8 gRogueTypeWeatherTable[];
extern const struct RogueEncounterMap gRogueTypeToEliteRoom[];

extern const struct RogueAdventureSettings gRogueAdventureSettings[];


#endif  // GUARD_ROGUE_H
