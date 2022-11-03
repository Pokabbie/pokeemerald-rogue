enum
{
    PAGE_TITLE,
    PAGE_DIRECTOR,
    PAGE_GAME_FREAK,
    PAGE_POSSIBLE,
    PAGE_RH_HIDEOUT,
    PAGE_SPRITES,
    PAGE_SONGS,
    PAGE_DISCORD_MODS,
    PAGE_SPECIAL_THANKS,
    PAGE_SPECIAL_THANKS2,
    PAGE_SPECIAL_THANKS3,
    PAGE_SPECIAL_THANKS4,
    PAGE_SPECIAL_THANKS5,
    PAGE_COUNT
};

#define ENTRIES_PER_PAGE 5

static const u8 sCreditsText_EmptyString[]                                  = _("");
static const struct CreditsEntry sCreditsEntry_EmptyString[]                = { 0, FALSE, sCreditsText_EmptyString};

static const u8 sCreditsText_PkmnEmeraldRogueVersion[]                      = _("POKéMON EMERALD ROGUE");
static const u8 sCreditsText_Credits[]                                      = _("Credits");
static const struct CreditsEntry sCreditsEntry_PkmnEmeraldRogueVersion[]    = { 7,  TRUE, sCreditsText_PkmnEmeraldRogueVersion};
static const struct CreditsEntry sCreditsEntry_Credits[]                    = {11,  TRUE, sCreditsText_Credits};

static const u8 sCreditsText_Creator[]                                      = _("Hack Creator");
static const u8 sCreditsText_Pokabbie[]                                     = _("Pokabbie");
static const struct CreditsEntry sCreditsEntry_Creator[]                    = {12,  TRUE, sCreditsText_Creator};
static const struct CreditsEntry sCreditsEntry_Pokabbie[]                   = {11,  FALSE, sCreditsText_Pokabbie};

static const u8 sCreditsText_Emerald[]                                      = _("POKéMON EMERALD");
static const u8 sCreditsText_GameFreak[]                                    = _("Game Freak");
static const struct CreditsEntry sCreditsEntry_Emerald[]                    = {12,  TRUE, sCreditsText_Emerald};
static const struct CreditsEntry sCreditsEntry_GameFreak[]                  = {11,  FALSE, sCreditsText_GameFreak};

static const u8 sCreditsText_MadePossibleBy[]                               = _("Made possible by...");
static const struct CreditsEntry sCreditsEntry_MadePossibleBy[]             = {12,  TRUE, sCreditsText_MadePossibleBy};

static const u8 sCreditsText_Expansion[]                                    = _("Emerald Expansion");
static const u8 sCreditsText_RhHidout[]                                     = _("RH-Hideout Community");
static const struct CreditsEntry sCreditsEntry_Expansion[]                  = {12,  TRUE, sCreditsText_Expansion};
static const struct CreditsEntry sCreditsEntry_RhHidout[]                   = {11,  FALSE, sCreditsText_RhHidout};

static const u8 sCreditsText_Sprites[]                                      = _("Additional Sprites");
static const u8 sCreditsText_AveonTrainer[]                                 = _("AveonTrainer");
static const u8 sCreditsText_PurpleZaffre[]                                 = _("PurpleZaffre");
static const struct CreditsEntry sCreditsEntry_Sprites[]                    = {12,  TRUE, sCreditsText_Sprites};
static const struct CreditsEntry sCreditsEntry_AveonTrainer[]               = {11,  FALSE, sCreditsText_AveonTrainer};
static const struct CreditsEntry sCreditsEntry_PurpleZaffre[]               = {11,  FALSE, sCreditsText_PurpleZaffre};

static const u8 sCreditsText_Songs[]                                        = _("Additional Songs");
static const u8 sCreditsText_CyanSMP64[]                                    = _("CyanSMP64");
static const struct CreditsEntry sCreditsEntry_Songs[]                      = {12,  TRUE, sCreditsText_Songs};
static const struct CreditsEntry sCreditsEntry_CyanSMP64[]                  = {11,  FALSE, sCreditsText_CyanSMP64};

static const u8 sCreditsText_DiscordMods[]                                  = _("Discord Mods");
static const u8 sCreditsText_Erma[]                                         = _("Erma");
static const u8 sCreditsText_Raven[]                                        = _("Raven");
static const u8 sCreditsText_TailsMK4[]                                     = _("TailsMK4");
static const struct CreditsEntry sCreditsEntry_DiscordMods[]                = {12,  TRUE, sCreditsText_DiscordMods};
static const struct CreditsEntry sCreditsEntry_Erma[]                       = {11,  FALSE, sCreditsText_Erma};
static const struct CreditsEntry sCreditsEntry_Raven[]                      = {11,  FALSE, sCreditsText_Raven};
static const struct CreditsEntry sCreditsEntry_TailsMK4[]                   = {11,  FALSE, sCreditsText_TailsMK4};

static const u8 sCreditsText_SpecialThanks[]                                = _("Special Thanks to");
static const u8 sCreditsText_Kate[]                                         = _("Kate");
static const u8 sCreditsText_You[]                                          = _("And to you, the community");
static const u8 sCreditsText_You2[]                                         = _("Thank you!");
static const u8 sCreditsText_You3[]                                         = _("For making Rogue larger than");
static const u8 sCreditsText_You4[]                                         = _("I could ever have imagined!");
static const u8 sCreditsText_You5[]                                         = _("Good luck on your future runs!");
static const u8 sCreditsText_You6[]                                         = _("{EMOJI_HEART}");
static const struct CreditsEntry sCreditsEntry_SpecialThanks[]              = {12,  TRUE, sCreditsText_SpecialThanks};
static const struct CreditsEntry sCreditsEntry_Kate[]                       = {11,  FALSE, sCreditsText_Kate};
static const struct CreditsEntry sCreditsEntry_Community[]                  = {11,  FALSE, sCreditsText_You};
static const struct CreditsEntry sCreditsEntry_Community2[]                 = {11,  FALSE, sCreditsText_You2};
static const struct CreditsEntry sCreditsEntry_Community3[]                 = {11,  FALSE, sCreditsText_You3};
static const struct CreditsEntry sCreditsEntry_Community4[]                 = {11,  FALSE, sCreditsText_You4};
static const struct CreditsEntry sCreditsEntry_Community5[]                 = {11,  FALSE, sCreditsText_You5};
static const struct CreditsEntry sCreditsEntry_Community6[]                 = {11,  FALSE, sCreditsText_You6};

#define _ sCreditsEntry_EmptyString

static const struct CreditsEntry *const sCreditsEntryPointerTable[PAGE_COUNT][ENTRIES_PER_PAGE] =
{
    [PAGE_TITLE] = {
        _,
        sCreditsEntry_PkmnEmeraldRogueVersion,
        sCreditsEntry_Credits,
        _,
        _
    },
    [PAGE_DIRECTOR] = {
        _,
        sCreditsEntry_Creator,
        sCreditsEntry_Pokabbie,
        _,
        _,
    },
    [PAGE_GAME_FREAK] = {
        _,
        sCreditsEntry_Emerald,
        sCreditsEntry_GameFreak,
        _,
        _,
    },
    [PAGE_POSSIBLE] = {
        _,
        sCreditsEntry_MadePossibleBy,
        _,
        _,
        _,
    },
    [PAGE_RH_HIDEOUT] = {
        _,
        sCreditsEntry_Expansion,
        sCreditsEntry_RhHidout,
        _,
        _,
    },
    [PAGE_SPRITES] = {
        _,
        sCreditsEntry_Sprites,
        sCreditsEntry_AveonTrainer,
        sCreditsEntry_PurpleZaffre,
        _,
    },
    [PAGE_SONGS] = {
        _,
        sCreditsEntry_Songs,
        sCreditsEntry_CyanSMP64,
        _,
        _,
    },
    [PAGE_DISCORD_MODS] = {
        sCreditsEntry_DiscordMods,
        sCreditsEntry_Erma,
        sCreditsEntry_Raven,
        sCreditsEntry_TailsMK4,
        _,
    },
    [PAGE_SPECIAL_THANKS] = {
        _,
        sCreditsEntry_SpecialThanks,
        sCreditsEntry_Kate,
        _,
        _,
    },
    [PAGE_SPECIAL_THANKS2] = {
        _,
        sCreditsEntry_Community,
        _,
        _,
        _,
    },
    [PAGE_SPECIAL_THANKS3] = {
        _,
        sCreditsEntry_Community2,
        _,
        _,
        _,
    },
    [PAGE_SPECIAL_THANKS4] = {
        _,
        sCreditsEntry_Community3,
        sCreditsEntry_Community4,
        _,
        _,
    },
    [PAGE_SPECIAL_THANKS5] = {
        _,
        sCreditsEntry_Community5,
        sCreditsEntry_Community6,
        _,
        _,
    },
};
#undef _
