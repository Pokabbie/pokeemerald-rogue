#include "constants/layouts.h"
#include "constants/rogue_hub.h"

#include "rogue_hub_strings.h"

#define SET_AREA_PRIMARY_MAP(map) \
    .primaryMapGroup = MAP_GROUP(map), \
    .primaryMapNum = MAP_NUM(map), \
    .primaryMapLayout = LAYOUT_ ## map


const struct RogueHubArea gRogueHubAreas[HUB_AREA_COUNT] = 
{
    [HUB_AREA_TOWN_SQUARE] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_TOWN_SQUARE),
        .areaName = _("Town Square"),
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        }
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
        }
    },
    [HUB_AREA_HOME] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_HOME),
        .areaName = _("{PLAYER}'s house"),
        .requiredUpgrades = { HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_EAST] = { 1, 2 },
            [HUB_AREA_CONN_SOUTH] = { 3, 4 },
            [HUB_AREA_CONN_WEST] = { 5, 6 },
        }
    },
    [HUB_AREA_BERRY_FIELD] = 
    {
        SET_AREA_PRIMARY_MAP(ROGUE_AREA_FARMING_FIELD),
        .areaName = _("Farming field"),
        .requiredUpgrades = { HUB_UPGRADE_HOME_BERRY_FIELD2, HUB_UPGRADE_NONE },
        .connectionWarps = 
        {
            [HUB_AREA_CONN_NORTH] = { 0, 1 },
            [HUB_AREA_CONN_EAST] = { 2, 3 },
            [HUB_AREA_CONN_SOUTH] = { 4, 5 },
            [HUB_AREA_CONN_WEST] = { 6, 7 },
        }
    },
};


const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT] = 
{
    // HUB_AREA_TOWN_SQUARE Upgrades
    //
    [HUB_UPGRADE_TOWN_SQUARE_POKE_CONNECT] = 
    {
        .upgradeName = _("Poké Connect"),
        .targetArea = HUB_AREA_TOWN_SQUARE,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

    // HUB_AREA_HOME Upgrades
    //
    [HUB_UPGRADE_HOME_LOWER_FLOOR] = 
    {
        .upgradeName = _("House"),
        .targetArea = HUB_AREA_HOME,
        .descText = sText_Desc_Home_LowerFloor,
        .completeText = sText_Desc_Home_LowerFloor,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_UPPER_FLOOR] = 
    {
        .upgradeName = _("Upper Floor"),
        .targetArea = HUB_AREA_HOME,
        .descText = sText_Desc_Home_UpperFloor,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_BERRY_FIELD1] = 
    {
        .upgradeName = _("Berry Field"),
        .targetArea = HUB_AREA_HOME,
        .descText = sText_Desc_Home_BerryField1,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_BERRY_FIELD2] = 
    {
        .upgradeName = _("Berry Field (2)"),
        .targetArea = HUB_AREA_HOME,
        .descText = sText_Desc_Home_BerryField2,
        .completeText = sText_Complete_Home_BerryField2,
        .requiredUpgrades = { HUB_UPGRADE_HOME_BERRY_FIELD1, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_GRASS_FIELD] = 
    {
        .upgradeName = _("Open Garden"),
        .targetArea = HUB_AREA_HOME,
        .descText = sText_Desc_Home_OpenField,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_GRASS_FIELD_SHED] = 
    {
        .upgradeName = _("Shed"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_HOME_GRASS_FIELD, HUB_UPGRADE_NONE }
    },


    // HUB_AREA_BERRY_FIELD Upgrades
    //
    [HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD] = 
    {
        .upgradeName = _("Extra Field"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1] = 
    {
        .upgradeName = _("Increase yield"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD2] = 
    {
        .upgradeName = _("Increased yield (2)"),
        .targetArea = HUB_AREA_BERRY_FIELD,
        .requiredUpgrades = { HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1, HUB_UPGRADE_NONE }
    },
};
