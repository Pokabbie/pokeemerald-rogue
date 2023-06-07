
static const u8 sText_Desc_Home_LowerFloor[] = _(
    "The home can be used to store items and\n"
    "change your outfits.\p"
    "It has access to further upgrades\n"
    "which provide extra convenience."
);

static const u8 sText_Desc_Home_UpperFloor[] = _(
    "The upper floor of the house will grant\n"
    "access to the bed, which will allow you\l"
    "to sleep so you may choose the time,\l"
    "weather and season."
);

static const u8 sText_Desc_Home_BerryField1[] = _(
    "The berry field will grant you a small\n"
    "patch where you may plant berries.\p"
    "They will grow whilst you are out\n"
    "adventuring."
);

static const u8 sText_Desc_Home_BerryField2[] = _(
    "This will grant an additional berry\n"
    "field."
);

static const u8 sText_Desc_Home_OpenField[] = _(
    "This field is an area which will allow\n"
    "POKéMON from your PC to roam around\l"
    "freely."
);

// TODO - Is this what the shed should do??
static const u8 sText_Desc_Home_Shed[] = _(
    "The shed is an area which will allow you\n"
    "to pose your party of POKéMON for\l"
    "photos."
);

const struct RogueAreaUpgrade gRogueHubUpgrades[HUB_UPGRADE_COUNT] = 
{
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
};
