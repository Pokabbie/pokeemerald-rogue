#include "constants/layouts.h"
#include "constants/rogue_hub.h"

#include "rogue_hub_strings.h"

extern const u8 gHubUpgradeDesc_Home_LowerFloor[];
extern const u8 gHubUpgradeDesc_Home_UpperFloor[];
extern const u8 gHubUpgradeDesc_Home_BerryField1[];
extern const u8 gHubUpgradeDesc_Home_BerryField2[];
extern const u8 gHubUpgradeDesc_Home_GrassField[];
extern const u8 gHubUpgradeDesc_Home_FieldShed[];
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
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        },
        .iconImage = gItemIcon_FameChecker,
        .iconPalette = gItemIconPalette_FameChecker,
    },
    [HUB_AREA_ADVENTURE_ENTRANCE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_ADVENTURE_ENTRANCE),
        .areaName = _("Town Exit"),
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_EAST] = { 4, 5 },
            [HUB_AREA_CONN_SOUTH] = { 2, 3 },
            [HUB_AREA_CONN_WEST] = { 0, 1 },
        },
        .iconImage = gItemIcon_BasementKey,
        .iconPalette = gItemIconPalette_OldKey,
    },
    [HUB_AREA_HOME] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_HOME),
        .areaName = _("Player Home"),
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .buildCost = 1,
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        },
        .iconImage = gItemIcon_BasementKey,
        .iconPalette = gItemIconPalette_OldKey,
    },
    [HUB_AREA_BERRY_FIELD] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_FARMING_FIELD),
        .areaName = _("Berry Fields"),
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_HOME_BERRY_FIELD2, HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        },
        .iconImage = gItemIcon_CheriBerry,
        .iconPalette = gItemIconPalette_CheriBerry,
    },
    [HUB_AREA_SAFARI_ZONE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_SAFARI_ZONE),
        .areaName = _("Safari Area"),
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_EAST] = { 4, 5 },
            [HUB_AREA_CONN_SOUTH] = { 2, 3 },
            [HUB_AREA_CONN_WEST] = { 0, 1 },
        },
        .iconImage = gItemIcon_FameChecker,
        .iconPalette = gItemIconPalette_FameChecker,
    },
    [HUB_AREA_RIDE_TRAINING] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_RIDE_TRAINING),
        .areaName = _("Circus"),
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        },
        .iconImage = gItemIcon_FameChecker,
        .iconPalette = gItemIconPalette_FameChecker,
    },
    [HUB_AREA_MARTS] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_MARTS),
        .areaName = _("Poké Marts"),
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        },
        .iconImage = gItemIcon_Potion,
        .iconPalette = gItemIconPalette_Potion,
    },
    [HUB_AREA_TOWN_SQUARE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_TOWN_SQUARE),
        .areaName = _("Town Square"),
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        },
        .iconImage = gItemIcon_Potion,
        .iconPalette = gItemIconPalette_Potion,
    },
};

#undef SET_UPDATE_AREA
#undef SET_AREA_PRIMARY_MAP

const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT] = 
{
    // HUB_AREA_LABS
    //
    [HUB_UPGRADE_LABS_POKE_CONNECT] = 
    {
        .upgradeName = _("Poké Connect"),
        .targetArea = HUB_AREA_LABS,
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

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

    [HUB_UPGRADE_HOME_BERRY_FIELD1] = 
    {
        .upgradeName = _("Berry Field"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_Home_BerryField1,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_BERRY_FIELD2] = 
    {
        .upgradeName = _("Berry Field 2"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_Home_BerryField2,
        .requiredUpgrades = { HUB_UPGRADE_HOME_BERRY_FIELD1, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_GRASS_FIELD] = 
    {
        .upgradeName = _("Play Garden"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_Home_GrassField,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_GRASS_FIELD_SHED] = 
    {
        .upgradeName = _("Shed"),
        .targetArea = HUB_AREA_HOME,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_Home_FieldShed,
        .requiredUpgrades = { HUB_UPGRADE_HOME_GRASS_FIELD, HUB_UPGRADE_NONE }
    },


    // HUB_AREA_BERRY_FIELD
    //
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD] = 
    {
        .upgradeName = _("Extra Field"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_ExtraField,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1] = 
    {
        .upgradeName = _("Berry Yield"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD2] = 
    {
        .upgradeName = _("Berry Yield 2"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .buildCost = 1,
        .descText = gHubUpgradeDesc_BerryField_HigherYeild,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1, HUB_UPGRADE_NONE }
    },


    // HUB_AREA_SAFARI_ZONE
    //
    [HUB_UPGRADE_SAFARI_ZONE_LEGENDS_CAVE] = 
    {
        .upgradeName = _("."),
        .targetArea = HUB_AREA_SAFARI_ZONE,
        .buildCost = 1,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
};
