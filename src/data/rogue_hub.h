#include "constants/layouts.h"
#include "constants/rogue_hub.h"

#include "rogue_hub_strings.h"

extern const u8 gHubAreaDesc_Todo[];
extern const u8 gHubAreaDesc_ChallengeFrontier[];
extern const u8 gHubAreaDesc_BerryField[];
extern const u8 gHubAreaDesc_Home[];
extern const u8 gHubAreaDesc_Marts[];
extern const u8 gHubAreaDesc_RideTraining[];
extern const u8 gHubAreaDesc_DayCare[];
extern const u8 gHubAreaDesc_Market[];

extern const u8 gHubUpgradeDesc_Home_LowerFloor[];
extern const u8 gHubUpgradeDesc_Home_UpperFloor[];
extern const u8 gHubUpgradeDesc_Home_TownStyle[];
extern const u8 gHubUpgradeDesc_Home_BerryField1[];
extern const u8 gHubUpgradeDesc_Home_BerryField2[];
extern const u8 gHubUpgradeDesc_Home_GrassField[];
extern const u8 gHubUpgradeDesc_Home_FieldShed[];
extern const u8 gHubUpgradeDesc_Lab_UniqueMonSlots0[];
extern const u8 gHubUpgradeDesc_Lab_UniqueMonSlots1[];
extern const u8 gHubUpgradeDesc_Lab_UniqueMonRarity0[];
extern const u8 gHubUpgradeDesc_Lab_UniqueMonRarity1[];
extern const u8 gHubUpgradeDesc_Lab_UniqueMonRarity2[];
extern const u8 gHubUpgradeDesc_Lab_UniqueMonRarity3[];
extern const u8 gHubUpgradeDesc_AdventureEntrance_RandomStarter[];
extern const u8 gHubUpgradeDesc_AdventureEntrance_AdventureReplay[];
extern const u8 gHubUpgradeDesc_BerryField_Brewing[];
extern const u8 gHubUpgradeDesc_BerryField_ExtraField[];
extern const u8 gHubUpgradeDesc_BerryField_HigherYeild[];
extern const u8 gHubUpgradeDesc_Mart_GeneralStock[];
extern const u8 gHubUpgradeDesc_Mart_PokeBalls[];
extern const u8 gHubUpgradeDesc_Mart_PokeBallsStock[];
extern const u8 gHubUpgradeDesc_Mart_TMs[];
extern const u8 gHubUpgradeDesc_Mart_TMsStock[];
extern const u8 gHubUpgradeDesc_Mart_TravelerBattleEnhancers[];
extern const u8 gHubUpgradeDesc_Mart_TravelerHeldItems[];
extern const u8 gHubUpgradeDesc_Mart_Bank[];
extern const u8 gHubUpgradeDesc_TownSquare_TrainerSchool[];
extern const u8 gHubUpgradeDesc_TownSquare_MoveTutors[];
extern const u8 gHubUpgradeDesc_DayCare_DayCareSlots[];
extern const u8 gHubUpgradeDesc_DayCare_Breeder[];
extern const u8 gHubUpgradeDesc_DayCare_BreederSeenDex[];
extern const u8 gHubUpgradeDesc_DayCare_Phone[];
extern const u8 gHubUpgradeDesc_DayCare_Bakery[];
extern const u8 gHubUpgradeDesc_DayCare_PokeblockBlenders[];
extern const u8 gHubUpgradeDesc_DayCare_TreatShop[];
extern const u8 gHubUpgradeDesc_DayCare_TreatShopStock[];
extern const u8 gHubUpgradeDesc_DayCare_TreatShop_Vanilla[];
extern const u8 gHubUpgradeDesc_DayCare_TeaShop[];
extern const u8 gHubUpgradeDesc_DayCare_ExpShare0[];
extern const u8 gHubUpgradeDesc_DayCare_ExpShare1[];
extern const u8 gHubUpgradeDesc_DayCare_ExpShare2[];


#define SET_UPDATE_AREA(area) \
    .primaryMapGroup = MAP_GROUP(map), \
    .primaryMapNum = MAP_NUM(map), \
    .primaryMapLayout = LAYOUT_ ## map

#define SET_AREA_PRIMARY_MAP(map) \
    .primaryMapGroup = MAP_GROUP(map), \
    .primaryMapNum = MAP_NUM(map), \
    .primaryMapLayout = LAYOUT_ ## map


const struct RogueHubArea gRogueHubAreas[HUB_AREA_COUNT] = 
{
    [HUB_AREA_LABS] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_LABS),
        .areaName = _("Labs"),
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 13 },
        },
        .iconImage = gItemIcon_Pokedex,
        .iconPalette = gItemIconPalette_Pokedex,
    },
    [HUB_AREA_ADVENTURE_ENTRANCE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_ADVENTURE_ENTRANCE),
        .areaName = _("Town Exit"),
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_EAST] = { 4, 5 },
            [HUB_AREA_CONN_SOUTH] = { 2, 3 },
            [HUB_AREA_CONN_WEST] = { 0, 1 },

            [HUB_AREA_CONN_TELEPORT] = { 6 },
        },
        .iconImage = gItemIcon_PokeBall,
        .iconPalette = gItemIconPalette_PokeBall,
    },
    [HUB_AREA_HOME] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_HOME),
        .areaName = _("Home Base"),
        .descText = gHubAreaDesc_Home,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .buildCost = 15,
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 9 },
        },
        .iconImage = gItemIcon_BasementKey,
        .iconPalette = gItemIconPalette_OldKey,
    },
    [HUB_AREA_BERRY_FIELD] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_FARMING_FIELD),
        .areaName = _("Berry Fields"),
        .descText = gHubAreaDesc_BerryField,
        .buildCost = 10,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 8 },
        },
        .iconImage = gItemIcon_CheriBerry,
        .iconPalette = gItemIconPalette_CheriBerry,
    },
    [HUB_AREA_SAFARI_ZONE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_SAFARI_ZONE),
        .areaName = _("Safari Area"),
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_EAST] = { 4, 5 },
            [HUB_AREA_CONN_SOUTH] = { 2, 3 },
            [HUB_AREA_CONN_WEST] = { 0, 1 },

            [HUB_AREA_CONN_TELEPORT] = { 7 },
        },
        .iconImage = gItemIcon_SafariBall,
        .iconPalette = gItemIconPalette_SafariBall,
    },
    [HUB_AREA_RIDE_TRAINING] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_RIDE_TRAINING),
        .areaName = _("Park"),
        .descText = gHubAreaDesc_RideTraining,
        .buildCost = 15,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 8 },
        },
        .iconImage = gItemIcon_FieldMoves,
        .iconPalette = gItemIconPalette_FieldMoves,
    },
    [HUB_AREA_MARTS] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_MARTS),
        .areaName = _("Poké Marts"),
        .descText = gHubAreaDesc_Marts,
        .buildCost = 25,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 8 },
        },
        .iconImage = gItemIcon_Potion,
        .iconPalette = gItemIconPalette_Potion,
    },
    [HUB_AREA_TOWN_SQUARE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_TOWN_SQUARE),
        .areaName = _("Town Square"),
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 10 },
        },
#ifdef ROGUE_EXPANSION
        .iconImage = gItemIcon_DevonParts,
#else
        .iconImage = gItemIcon_DevonGoods,
#endif
        .iconPalette = gItemIconPalette_DevonGoodsBrown,
    },
    [HUB_AREA_CHALLENGE_FRONTIER] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_CHALLENGE_FRONTIER),
        .areaName = _("Challenge Area"),
        .descText = gHubAreaDesc_ChallengeFrontier,
        .buildCost = 0,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 10 },
        },
        .iconImage = gItemIcon_MasterBall,
        .iconPalette = gItemIconPalette_MasterBall,
    },
    [HUB_AREA_DAY_CARE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_DAY_CARE),
        .areaName = _("Day Care"),
        .descText = gHubAreaDesc_DayCare,
        .buildCost = 30,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 8 },
        },
        .iconImage = gItemIcon_Egg,
        .iconPalette = gItemIconPalette_Egg,
    },
    [HUB_AREA_MARKET] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_MARKET),
        .areaName = _("Market"),
        .descText = gHubAreaDesc_Market,
        .buildCost = 25,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 9 },
        },
        .iconImage = gItemIcon_PokeblockShiny,
        .iconPalette = gItemIconPalette_PokeblockShiny,
    },
};

#undef SET_UPDATE_AREA
#undef SET_AREA_PRIMARY_MAP

const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT] = 
{
    // HUB_AREA_HOME
    //
    [HUB_UPGRADE_HOME_LOWER_FLOOR] = 
    {
        .upgradeName = _("Interior Base"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Home_LowerFloor,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_UPPER_FLOOR] = 
    {
        .upgradeName = _("Upper Floor"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 0,
        .descText = gHubUpgradeDesc_Home_UpperFloor,
        .requiredUpgrades = { HUB_UPGRADE_HOME_UPPER_FLOOR, HUB_UPGRADE_NONE } // reserved
    },
    [HUB_UPGRADE_HOME_EDIT_STYLE] = 
    {
        .upgradeName = _("Town Style"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Home_TownStyle,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },

    // HUB_AREA_LABS
    //
    [HUB_UPGRADE_LAB_UNIQUE_MON_LAB] = 
    {
        .upgradeName = _("Unique Lab"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 0,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .isHidden = TRUE
    },

    [HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS0] = 
    {
        .upgradeName = _("Unique{PKMN} Slots"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_Lab_UniqueMonSlots0,
        .requiredUpgrades = { HUB_UPGRADE_LAB_UNIQUE_MON_LAB, HUB_UPGRADE_NONE },
    },
    [HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS1] = 
    {
        .upgradeName = _("Unique{PKMN} Slots+"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_Lab_UniqueMonSlots1,
        .requiredUpgrades = { HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS0, HUB_UPGRADE_NONE },
    },

    [HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_RARE] = 
    {
        .upgradeName = _("Rare Unique{PKMN}"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Lab_UniqueMonRarity0,
        .requiredUpgrades = { HUB_UPGRADE_LAB_UNIQUE_MON_LAB, HUB_UPGRADE_NONE },
    },
    [HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EPIC] = 
    {
        .upgradeName = _("Epic Unique{PKMN}"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_Lab_UniqueMonRarity1,
        .requiredUpgrades = { HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_RARE, HUB_UPGRADE_NONE },
    },
    [HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EXOTIC] = 
    {
        .upgradeName = _("Exotic Unique{PKMN}"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_Lab_UniqueMonRarity2,
        .requiredUpgrades = { HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EPIC, HUB_UPGRADE_NONE },
    },
    [HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY] = // placeholder for supporting legendaries
    {
        .upgradeName = _("Legend Unique{PKMN}"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 0, // 9
        .descText = gHubUpgradeDesc_Lab_UniqueMonRarity3,
        .requiredUpgrades = { HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY, HUB_UPGRADE_NONE }, // requiring self will mean it's never built
    },

    // HUB_AREA_ADVENTURE_ENTRANCE
    //
    [HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER] = 
    {
        .upgradeName = _("Starter Bag"),
        .targetArea = HUB_AREA_ADVENTURE_ENTRANCE,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_AdventureEntrance_RandomStarter,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_ADVENTURE_ENTRANCE_ADVENTURE_REPLAY] = 
    {
        .upgradeName = _("Adventure Flag"),
        .targetArea = HUB_AREA_ADVENTURE_ENTRANCE,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_AdventureEntrance_AdventureReplay,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

    // HUB_AREA_BERRY_FIELD
    //
    [HUB_UPGRADE_BERRY_FIELD_BREWING] = 
    {
        .upgradeName = _("Potion Cauldron"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_BerryField_Brewing,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_BREWING_UPGRADE] = 
    {
        .upgradeName = _("PotionCauldron+"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 0,
        .descText = gHubUpgradeDesc_BerryField_Brewing,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_BREWING_UPGRADE, HUB_UPGRADE_NONE } // reserved
    },
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD0] = 
    {
        .upgradeName = _("Berry Field+"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1] = 
    {
        .upgradeName = _("Berry Field++"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD0, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD2] = 
    {
        .upgradeName = _("Berry Field+++"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD0] = 
    {
        .upgradeName = _("Berry Yield+"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1] = 
    {
        .upgradeName = _("Berry Yield++"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD0, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD2] = 
    {
        .upgradeName = _("Berry Yield+++"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1, HUB_UPGRADE_NONE }
    },


    // HUB_AREA_SAFARI_ZONE
    //
    [HUB_UPGRADE_SAFARI_ZONE_LEGENDS_CAVE] = 
    {
        .upgradeName = _("Legend Cave"),
        .targetArea = HUB_AREA_SAFARI_ZONE,
        .buildCost = 0,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .isHidden = TRUE
    },

    // HUB_AREA_MARTS
    //
    [HUB_UPGRADE_MARTS_GENERAL_STOCK] = 
    {
        .upgradeName = _("General Stock"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Mart_GeneralStock,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_POKE_BALLS] = 
    {
        .upgradeName = _("PokéBall Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Mart_PokeBalls,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_POKE_BALLS_STOCK] = 
    {
        .upgradeName = _("PokéBall Stock"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_Mart_PokeBallsStock,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_POKE_BALLS, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TMS] = 
    {
        .upgradeName = _("TM Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Mart_TMs,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TMS_STOCK] = 
    {
        .upgradeName = _("TM Stock"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_Mart_TMsStock,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_TMS, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TRAVELER_BATTLE_ENCHANCERS] = 
    {
        .upgradeName = _("Battle Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Mart_TravelerBattleEnhancers,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TRAVELER_HELD_ITEMS] = 
    {
        .upgradeName = _("Held Items Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_Mart_TravelerHeldItems,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_TRAVELER_BATTLE_ENCHANCERS, HUB_UPGRADE_NONE }
    },

    // HUB_AREA_TOWN_SQUARE
    //
    [HUB_UPGRADE_TOWN_SQUARE_SCHOOL] = 
    {
        .upgradeName = _("Trainer School"),
        .targetArea = HUB_AREA_TOWN_SQUARE,
        .buildCost = 0,
        .descText = gHubUpgradeDesc_TownSquare_TrainerSchool,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_TOWN_SQUARE_TUTORS] = 
    {
        .upgradeName = _("Move Tutor"),
        .targetArea = HUB_AREA_TOWN_SQUARE,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_TownSquare_MoveTutors,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

    // HUB_UPGRADE_DAY_CARE_BREEDER
    //
    [HUB_UPGRADE_DAY_CARE_CAPACITY0] = 
    {
        .upgradeName = _("DayCare Slots+"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_DayCare_DayCareSlots,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_CAPACITY1] = 
    {
        .upgradeName = _("DayCare Slots++"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 9,
        .descText = gHubUpgradeDesc_DayCare_DayCareSlots,
        .requiredUpgrades = { HUB_UPGRADE_DAY_CARE_CAPACITY0, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_BREEDER] = 
    {
        .upgradeName = _("{PKMN} Breeder"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_DayCare_Breeder,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_BREEDER_SEEN_DEX] = 
    {
        .upgradeName = _("{PKMN} Breeder+"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 12,
        .descText = gHubUpgradeDesc_DayCare_BreederSeenDex,
        .requiredUpgrades = { HUB_UPGRADE_DAY_CARE_BREEDER, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_BREEDER_LEGENDS] = 
    {
        .upgradeName = _("{PKMN} Breeder++"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 0, // 9
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_DAY_CARE_BREEDER_LEGENDS, HUB_UPGRADE_NONE }, // reserved
        .isHidden = TRUE,
    },
    [HUB_UPGRADE_DAY_CARE_PHONE] = 
    {
        .upgradeName = _("Day Care Phone"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_DayCare_Phone,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_TEA_SHOP] = 
    {
        .upgradeName = _("Tea Room"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_DayCare_TeaShop,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_EXP_SHARE0] = 
    {
        .upgradeName = _("Day Care Exp"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_DayCare_ExpShare0,
        .requiredUpgrades = { HUB_UPGRADE_DAY_CARE_CAPACITY1, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_EXP_SHARE1] = 
    {
        .upgradeName = _("Day Care Exp+"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 9,
        .descText = gHubUpgradeDesc_DayCare_ExpShare1,
        .requiredUpgrades = { HUB_UPGRADE_DAY_CARE_EXP_SHARE0, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_DAY_CARE_EXP_SHARE2] = 
    {
        .upgradeName = _("Day Care Exp++"),
        .targetArea = HUB_AREA_DAY_CARE,
        .buildCost = 12,
        .descText = gHubUpgradeDesc_DayCare_ExpShare2,
        .requiredUpgrades = { HUB_UPGRADE_DAY_CARE_EXP_SHARE1, HUB_UPGRADE_NONE }
    },


    [HUB_UPGRADE_MARKET_POKEBLOCK_BLENDERS] = 
    {
        .upgradeName = _("{POKEBLOCK} Blender+"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_DayCare_PokeblockBlenders,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARKET_BAKERY] = 
    {
        .upgradeName = _("Bakery"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 7,
        .descText = gHubUpgradeDesc_DayCare_Bakery,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

#ifdef ROGUE_EXPANSION
    [HUB_UPGRADE_MARKET_TREAT_SHOP] = 
    {
        .upgradeName = _("Treat Shop"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_DayCare_TreatShop,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARKET_TREAT_SHOP_STOCK] = 
    {
        .upgradeName = _("{POKEBLOCK} Shop"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_DayCare_TreatShopStock,
        .requiredUpgrades = { HUB_UPGRADE_MARKET_TREAT_SHOP, HUB_UPGRADE_NONE }
    },
#else
    // In Vanilla we only have 1 upgrade
    [HUB_UPGRADE_MARKET_TREAT_SHOP] = 
    {
        .upgradeName = _("Treat Shop"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 5,
        .descText = gHubUpgradeDesc_DayCare_TreatShop_Vanilla,
        .requiredUpgrades = { HUB_UPGRADE_MARKET_POKEBLOCK_BLENDERS, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARKET_TREAT_SHOP_STOCK] = 
    {
        .upgradeName = _("{POKEBLOCK} Shop"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 0,
        .descText = gHubUpgradeDesc_DayCare_TreatShopStock,
        .requiredUpgrades = { HUB_UPGRADE_MARKET_TREAT_SHOP_STOCK, HUB_UPGRADE_NONE } // unobtainable in Vanilla
    },
#endif
    [HUB_UPGRADE_MARKET_BANK] = 
    {
        .upgradeName = _("Bank"),
        .targetArea = HUB_AREA_MARKET,
        .buildCost = 3,
        .descText = gHubUpgradeDesc_Mart_Bank,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
};
