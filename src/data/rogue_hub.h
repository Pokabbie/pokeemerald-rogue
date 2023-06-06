
//static const u8 gText_GiveTravelShopPlates[] = _("Travelling Mart will now stock\nAdditional Plates and Memories!");

const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT] = 
{
    [HUB_UPGRADE_HOME_LOWER_FLOOR] = 
    {
        .upgradeName = _("House"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_UPPER_FLOOR] = 
    {
        .upgradeName = _("House (2)"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_HOME_LOWER_FLOOR, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_BERRY_FIELD1] = 
    {
        .upgradeName = _("Berry Field"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },
    [HUB_UPGRADE_HOME_BERRY_FIELD2] = 
    {
        .upgradeName = _("Berry Field (2)"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_HOME_BERRY_FIELD1, HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_GRASS_FIELD] = 
    {
        .upgradeName = _("Open Field"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_NONE }
    },

    [HUB_UPGRADE_HOME_GRASS_FIELD_SHED] = 
    {
        .upgradeName = _("Shed"),
        .targetArea = HUB_AREA_HOME,
        .requiredUpgrades = { HUB_UPGRADE_HOME_GRASS_FIELD, HUB_UPGRADE_NONE }
    },
};
