#include "constants/layouts.h"
#include "constants/rogue_hub.h"

#include "rogue_hub_strings.h"

extern const u8 gHubAreaDesc_Todo[];

extern const u8 gHubUpgradeDesc_Home_LowerFloor[];
extern const u8 gHubUpgradeDesc_Home_UpperFloor[];
extern const u8 gHubUpgradeDesc_Home_BerryField1[];
extern const u8 gHubUpgradeDesc_Home_BerryField2[];
extern const u8 gHubUpgradeDesc_Home_GrassField[];
extern const u8 gHubUpgradeDesc_Home_FieldShed[];
extern const u8 gHubUpgradeDesc_AdventureEntrance_RandomStarter[];
extern const u8 gHubUpgradeDesc_AdventureEntrance_AdventureReplay[];
extern const u8 gHubUpgradeDesc_BerryField_ExtraField[];
extern const u8 gHubUpgradeDesc_BerryField_HigherYeild[];


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
        .areaName = _("Player Home"),
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .buildCost = 1,
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
        .descText = gHubAreaDesc_Todo,
        .buildCost = 1,
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
        .descText = gHubAreaDesc_Todo,
        .buildCost = 1,
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
        .descText = gHubAreaDesc_Todo,
        .buildCost = 1,
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
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 10 },
        },
        .iconImage = gItemIcon_DevonGoods,
        .iconPalette = gItemIconPalette_DevonGoodsBrown,
    },
    [HUB_AREA_CHALLENGE_FRONTIER] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_CHALLENGE_FRONTIER),
        .areaName = _("Challenge Area"),
        .descText = gHubAreaDesc_Todo,
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
        .descText = gHubAreaDesc_Todo,
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },

            [HUB_AREA_CONN_TELEPORT] = { 9 },
        },
        .iconImage = gItemIcon_Egg,
        .iconPalette = gItemIconPalette_Egg,
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
        .upgradeName = _("House"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_Home_LowerFloor,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_UPPER_FLOOR] = 
    {
        .upgradeName = _("Upper Floor"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_Home_UpperFloor,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },

    // HUB_AREA_ADVENTURE_ENTRANCE
    //
    [HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER] = 
    {
        .upgradeName = _("Starter Bag"),
        .targetArea = HUB_AREA_ADVENTURE_ENTRANCE,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_AdventureEntrance_RandomStarter,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_ADVENTURE_ENTRANCE_ADVENTURE_REPLAY] = 
    {
        .upgradeName = _("Adventure Flag"),
        .targetArea = HUB_AREA_ADVENTURE_ENTRANCE,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_AdventureEntrance_AdventureReplay,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

    // HUB_AREA_BERRY_FIELD
    //
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD0] = 
    {
        .upgradeName = _("Berry Field (2)"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1] = 
    {
        .upgradeName = _("Berry Field (3)"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD0, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD2] = 
    {
        .upgradeName = _("Berry Field (4)"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD0] = 
    {
        .upgradeName = _("Berry Yield+"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1] = 
    {
        .upgradeName = _("Berry Yield++"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD0, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD2] = 
    {
        .upgradeName = _("Berry Yield+++"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
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
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_POKE_BALLS] = 
    {
        .upgradeName = _("PokéBall Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_POKE_BALLS_STOCK] = 
    {
        .upgradeName = _("PokéBall Stock"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_POKE_BALLS, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TMS] = 
    {
        .upgradeName = _("TM Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TMS_STOCK] = 
    {
        .upgradeName = _("TM Stock"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_TMS, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TRAVELER_BATTLE_ENCHANCERS] = 
    {
        .upgradeName = _("Battle Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_GENERAL_STOCK, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_TRAVELER_HELD_ITEMS] = 
    {
        .upgradeName = _("Held Items Shop"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_MARTS_TRAVELER_BATTLE_ENCHANCERS, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_MARTS_BANK] = 
    {
        .upgradeName = _("Bank"),
        .targetArea = HUB_AREA_MARTS,
        .buildCost = 1,
        .descText = gHubAreaDesc_Todo,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
};
