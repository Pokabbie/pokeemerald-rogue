static const u8 gText_PreviewUnlocksUpgrade[] = _("New HUB Upgrade");
static const u8 gText_PreviewUnlocksAdventureSetting[] = _("New Adventure Options"); 
static const u8 gText_GiveUnlocksShops[] = _("New HUB Unlocks:\nMart & Clothes Shop!");
static const u8 gText_GiveUnlocksSafari[] = _("New HUB Unlocks:\nSafari Zone & Name Rater!");
static const u8 gText_GiveUnlocksTravellingMart[] = _("New HUB Unlocks:\nTravelling Mart!");
static const u8 gText_GiveUnlocksTravellingBank[] = _("New HUB Unlocks:\nBank!");
static const u8 gText_GiveUnlocksBikeShop[] = _("New HUB Unlocks:\nBike Shop!");
static const u8 gText_GiveUnlocksTutors[] = _("New HUB Unlocks:\nMove Tutors!");
static const u8 gText_GiveUnlocksDaycare[] = _("New HUB Unlocks:\nDay Care!");
static const u8 gText_GiveUnlocksBerries[] = _("New HUB Unlocks:\nBerry Patch!");
static const u8 gText_GiveUnlocksBakery[] = _("New HUB Unlocks:\nBakery!");
static const u8 gText_GiveUnlocksBallSwitch[] = _("New HUB Unlocks:\nPOKé BALL swapper!");
static const u8 gText_PreviewUnlocksCurseStart[] = _("New HUB Unlocks:\nCurses now avaliable in Config Lab!");

static const u8 gText_PreviewUnlocksLegendarySafari[] = _("Safari Upgrade");
static const u8 gText_GiveLegendarySafari[] = _("New Safari Upgrade:\nLegendary POKéMON can now appear!");
static const u8 gText_GiveSafariLimiter[] = _("New Safari Upgrade:\nYou can now filter by GENERATION!");
static const u8 gText_PreviewUnlocksBikeShop[] = _("Bike Shop Upgrade");
static const u8 gText_GiveLegendaryBikeShop[] = _("New Bike Shop Upgrade:\nYou can now carry both Bikes at once!");
static const u8 gText_PreviewUnlocksMrRandoStart[] = _("Mr. Randoman will now offer a free\ntrade at the start of Adventures!");
static const u8 gText_PreviewUnlocksTutorUpgrade[] = _("Tutors Upgrade");
static const u8 gText_GiveTutorUpgrade[] = _("HUB Tutors will now offer more moves!");
static const u8 gText_PreviewUnlocksTravelShopUpgrade[] = _("Travelling Mart Upgrade");
static const u8 gText_GiveTravelShopMegas[] = _("Travelling Mart will now stock\nMega stones!");
static const u8 gText_GiveTravelShopZMoves[] = _("Travelling Mart will now stock\nZ Crystals!");
static const u8 gText_GiveTravelShopRustedItems[] = _("Travelling Mart will now stock\nRusted Sword and Shield!");
static const u8 gText_GiveTravelShopOrbs[] = _("Travelling Mart will now stock\nRed and Blue Orbs!");
static const u8 gText_GiveTravelShopPlates[] = _("Travelling Mart will now stock\nAdditional Plates and Memories!");

const struct RogueQuestConstants gRogueQuests[QUEST_CAPACITY + 1] = 
{
    [QUEST_NONE] = 
    {
        .title = _("-"),
        .desc = _("-"),
        .flags = QUEST_FLAGS_NONE,
        .sortIndex = 0,
    },

    [QUEST_FirstAdventure] = 
    {
        .title = _("To adventure!"),
        .desc = _(
                    "Embark on your first\n"
                    "adventure!"
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 3 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 1000 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksShops },
        },
        .unlockedQuests = { 
            QUEST_WobFate
        }
    },

    [QUEST_Gym1] = 
    {
        .title = _("BADGE 1"),
        .desc = _(
                    "Defeat the 1st GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_PECHA_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM01_CUT, 1 } },
        }
    },
    [QUEST_Gym2] = 
    {
        .title = _("BADGE 2"),
        .desc = _(
                    "Defeat the 2nd GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_PERSIM_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM05_FLASH, 1 } },
        }
    },
    [QUEST_Gym3] = 
    {
        .title = _("BADGE 3"),
        .desc = _(
                    "Defeat the 3rd GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RAWST_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM06_ROCK_SMASH, 1 } },
        }
    },
    [QUEST_Gym4] = 
    {
        .title = _("BADGE 4"),
        .desc = _(
                    "Defeat the 4th GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_CHERI_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM04_STRENGTH, 1 } },
        }
    },
    [QUEST_Gym5] = 
    {
        .title = _("BADGE 5"),
        .desc = _(
                    "Defeat the 5th GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ASPEAR_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM08_DIVE, 1 } },
        }
    },
    [QUEST_Gym6] = 
    {
        .title = _("BADGE 6"),
        .desc = _(
                    "Defeat the 6th GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_CHESTO_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM02_FLY, 1 } },
        }
    },
    [QUEST_Gym7] = 
    {
        .title = _("BADGE 7"),
        .desc = _(
                    "Defeat the 7th GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_LEPPA_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM07_WATERFALL, 1 } },
        }
    },
    [QUEST_Gym8] = 
    {
        .title = _("BADGE 8"),
        .desc = _(
                    "Defeat the 8th GYM."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SITRUS_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HM03_SURF, 1 } },
        }
    },

    [QUEST_GymChallenge] = 
    {
        .title = _("Gym Challenge"),
        .desc = _(
                    "Defeat the first 4\n"
                    "GYM leaders."
                ),
        .flags = QUEST_FLAGS_REPEATABLE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 2 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 2500 } } 
        }
    },
    [QUEST_GymMaster] = 
    {
        .title = _("Gym Master"),
        .desc = _(
                    "Defeat all 8\n"
                    "GYM leaders."
                ),
        .flags = QUEST_FLAGS_REPEATABLE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 2 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 2500 } } 
        }
    },
    [QUEST_EliteMaster] = 
    {
        .title = _("Elite Master"),
        .desc = _(
                    "Defeat all of the\n"
                    "Elite 4."
                ),
        .flags = QUEST_FLAGS_REPEATABLE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 2 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } } 
        }
    },
    [QUEST_Champion] = 
    {
        .title = _("The Champion"),
        .desc = _(
                    "Defeat the final\n"
                    "champion."
                ),
        .flags = QUEST_FLAGS_REPEATABLE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 3 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } } 
        },
        .unlockedQuests = { 
            QUEST_NORMAL_Champion, 
            QUEST_FIGHTING_Champion, 
            QUEST_FLYING_Champion, 
            QUEST_POISON_Champion, 
            QUEST_GROUND_Champion, 
            QUEST_ROCK_Champion, 
            QUEST_BUG_Champion, 
            QUEST_GHOST_Champion, 
            QUEST_STEEL_Champion, 
            QUEST_FIRE_Champion, 
            QUEST_WATER_Champion, 
            QUEST_GRASS_Champion, 
            QUEST_ELECTRIC_Champion, 
            QUEST_PSYCHIC_Champion, 
            QUEST_ICE_Champion, 
            QUEST_DRAGON_Champion, 
            QUEST_DARK_Champion, 
#ifdef ROGUE_EXPANSION
            QUEST_FAIRY_Champion,
#endif
        }
    },
    
    [QUEST_MeetPokabbie] = 
    {
        .title = _("Post Game"),
        .desc = _(
                    "Meet ??? in ??? to activate\n"
                    "post-game content."
                ),
        .flags = QUEST_FLAGS_ACTIVE_IN_HUB,
        .rewards = {
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_STARF_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SALAC_BERRY, 1 } },
#else
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SALAC_BERRY, 1 } },
#endif
        },
        .unlockedQuests = { 
            QUEST_KantoMode,
            QUEST_OrreMode,
            QUEST_JohtoMode,
            QUEST_HoennMode,
#ifdef ROGUE_EXPANSION
            QUEST_SinnohMode,
            QUEST_UnovaMode,
            QUEST_KalosMode,
            QUEST_AlolaMode,
            QUEST_GalarMode,

            QUEST_MegaEvo,
            QUEST_ZMove,
            QUEST_ShayminItem,
            QUEST_HoopaItem,
            QUEST_NatureItem,
#endif
        }
    },

    [QUEST_Collector1] = 
    {
        .title = _("Collector"),
        .desc = _(
                    "Fill out 15 POKéDEX\n"
                    "entries."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksSafari },
        },
        .unlockedQuests = { 
            QUEST_Collector2,
            QUEST_CollectorLegend,
        }
    },
    [QUEST_Collector2] = 
    {
        .title = _("Collector +"),
        .desc = _(
                    "Fill out 100 POKéDEX\n"
                    "entries."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_MEW, 7, FALSE } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksLegendarySafari, .giveText=gText_GiveSafariLimiter },
        }
    },
    [QUEST_CollectorLegend] = 
    {
        .title = _("Collector X"),
        .desc = _(
                    "Defeat the Elite 4\n"
                    "with a Legendary POKéMON\n"
                    "in your party."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksLegendarySafari, .giveText=gText_GiveLegendarySafari },
        },
        .unlockedQuests = { 
            QUEST_LegendOnly
        }
    },

    [QUEST_ShoppingSpree] = 
    {
        .title = _("Shop Spree"),
        .desc = _(
                    "Spend at least ¥20000\n"
                    "In a single visit to any\n"
                    "REST STOP."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_STAR_PIECE, 2 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksTravellingMart },
        },
        .unlockedQuests = { 
            QUEST_BigSaver
        }
    },
    [QUEST_BigSaver] = 
    {
        .title = _("Big Saver"),
        .desc = _(
                    "Exit a REST STOP with\n"
                    "¥50000 in your pocket."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_AMULET_COIN, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_STAR_PIECE, 3 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksTravellingBank },
        }
    },
    
    [QUEST_Bike1] = 
    {
        .title = _("Can't Stop!"),
        .desc = _(
                    "Exit a Tough Route within\n"
                    "2 minutes of entering."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_TAMATO_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksBikeShop },
        },
        .unlockedQuests = { 
            QUEST_Bike2, 
        }
    },
    [QUEST_Bike2] = 
    {
        .title = _("Won't Stop!"),
        .desc = _(
                    "After collecting the 8th\n"
                    "GYM badge, exit a Tough\n"
                    "Route within 1 minute."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksBikeShop, .giveText=gText_GiveLegendaryBikeShop },
        }
    },

    [QUEST_NoFainting1] = 
    {
        .title = _("Care Tactics"),
        .desc = _(
                    "Defeat 4 GYM leaders\n"
                    "without your PARTNER\n"
                    "POKéMON fainting."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SUPER_ROD, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksTutors },
        },
        .unlockedQuests = { 
            QUEST_NoFainting2,
            QUEST_NoFainting3,
        }
    },
    [QUEST_NoFainting2] = 
    {
        .title = _("Smart Tactics"),
        .desc = _(
                    "Defeat all 8 GYM\n"
                    "leaders without any\n"
                    "POKéMON fainting."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_KELPSY_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HONDEW_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTutorUpgrade, .giveText=gText_GiveTutorUpgrade },
        }
    },
    [QUEST_NoFainting3] = 
    {
        .title = _("True Tactics"),
        .desc = _(
                    "Win a full Run\n"
                    "without any POKéMON\n"
                    "ever fainting."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 20000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_LUM_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTutorUpgrade, .giveText=gText_GiveTutorUpgrade },
        }
    },

    [QUEST_MrRandoman] = 
    {
        .title = _("Mr. Randoman"),
        .desc = _(
                    "Trade a POKéMON with\n"
                    "Mr. Randoman."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksAdventureSetting, .giveText=gText_PreviewUnlocksMrRandoStart },
        },
        .unlockedQuests = { 
            QUEST_ChaosChampion,
        }
    },
    [QUEST_ChaosChampion] = 
    {
        .title = _("Chaos Master"),
        .desc = _(
                    "Win a full Run, always\n"
                    "doing a Random Party\n"
                    "Trade whenever possible."
                ),
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ABILITY_CAPSULE, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ABILITY_PATCH, 1 } },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 20000 } },
#endif
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_SMEARGLE, 7, TRUE } },
        }
    },
    [QUEST_DevilDeal] = 
    {
        .title = _("Devil Deal"),
        .desc = _(
                    "Make a deal with...\n"
                    "a devil?"
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_PreviewUnlocksCurseStart },
        },
        .unlockedQuests = { 
            QUEST_Hardcore,
            QUEST_Hardcore2,
            QUEST_Hardcore3,
            QUEST_CursedBody,
            QUEST_Nuzlocke,
        }
    },

    [QUEST_CursedBody] = 
    {
        .title = _("Cursed Body"),
        .desc = _(
                    "Accept 1 of every\n"
                    "Curse and complete a\n"
                    "full run."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_DUSKULL, 7, TRUE } },
        },
    },
    [QUEST_Nuzlocke] = 
    {
        .title = _("Roguelocke"),
        .desc = _(
                    "Full run with: Bag Wipe\n"
                    "Random starter; Embargo,\n"
                    "Species & 10 Wild curses."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ESCAPE_ROPE, 1 } },
        },
    },

    [QUEST_IronMono1] = 
    {
        .title = _("True Mono"),
        .desc = _(
                    "Accept 5 Capacity\n"
                    "Curses and complete a\n"
                    "full run."
                ),
        .flags = QUEST_FLAGS_REPEATABLE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 2 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ULTRA_BALL, 99 } },
        },
    },
    [QUEST_IronMono2] = 
    {
        .title = _("Iron Kaizo"),
        .desc = _(
                    "Full run with: Bag Wipe\n"
                    "& Random starter;\n"
                    "Embargo, 5 Capacity, 5 Wild,\n"
                    "99 Discount curses."
                ),
        .flags = QUEST_FLAGS_REPEATABLE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 2 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_MASTER_BALL, 1 } },
        },
    },
    
    [QUEST_LegendOnly] = 
    {
        .title = _("Apotheosis"),
        .desc = _(
                    "Win a full Run, only\n"
                    "ever using legendary\n"
                    "POKéMON."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 1000 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksBallSwitch },
        }
    },

    [QUEST_WobFate] = 
    {
        .title = _("Fate"),
        .desc = _(
                    "Have a POKéMON faint whilst\n"
                    "fighting a wild Wobbuffet."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 2500 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_WYNAUT, 7, TRUE } },
        }
    },

    [QUEST_Hardcore] = 
    {
        .title = _("Extreme Mode"),
        .sortIndex = 5,
        .desc = _(
                    "Win a full Run, with\n"
                    "Embargo Curse."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_MAGIKARP, 7, TRUE } },
        }
    },
    [QUEST_Hardcore2] = 
    {
        .title = _("Hardcore Mode"),
        .sortIndex = 5,
        .desc = _(
                    "Extreme Mode rules +\n"
                    "Hard Trainers enabled."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_DITTO, 7, TRUE } },
        }
    },
    [QUEST_Hardcore3] = 
    {
        .title = _("Insane Mode"),
        .sortIndex = 5,
        .desc = _(
                    "Hardcore Mode rules +\n"
                    "Species Curse & no\n"
                    "catching legendaries."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_CELEBI, 7, TRUE } },
        },
        .unlockedQuests = {
            QUEST_IronMono1,
            QUEST_IronMono2,
            QUEST_Hardcore4,
        },
    },
    [QUEST_Hardcore4] = 
    {
        .title = _("Please Stop..."),
        .sortIndex = 5,
        .desc = _(
                    "Insane Mode rules + Hard\n"
                    "Items, Bag Wipe, EVs off\n"
                    "Overlvl off, Random Starter."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_RAYQUAZA, 7, TRUE } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ESCAPE_ROPE, 1 } },
        }
    },

    [QUEST_KantoMode] = 
    {
        .title = _("Kanto Style"),
        .sortIndex = 10,
        .desc = _(
                    "Enable only KANTO BOSSES\n"
                    "and REGIONAL DEX and\n"
                    "win a full standard run."
                ),
        .unlockedQuests = { 
            QUEST_GlitchMode,
        },
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_PIKACHU, 7, TRUE } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_CLEFAIRY, 7, TRUE } },
        }
    },
    [QUEST_OrreMode] = 
    {
        .title = _("Orre Style"),
        .sortIndex = 10,
        .desc = _(
                    "Reach the final champion\n"
                    "in DOUBLES with an ESPEON\n"
                    "& UMBREON in your party."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_EEVEE, 7, TRUE } },
        }
    },

    [QUEST_JohtoMode] = 
    {
        .title = _("Johto Style"),
        .sortIndex = 10,
        .desc = _(
                    "Enable only JOHTO BOSSES\n"
                    "and REGIONAL DEX and\n"
                    "win a full standard run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_MARILL, 7, TRUE } },
        }
    },
    [QUEST_HoennMode] = 
    {
        .title = _("Hoenn Rainbow"),
        .sortIndex = 10,
        .desc = _(
                    "Enable only HOENN BOSSES\n"
                    "and REGIONAL DEX and win\n"
                    "a full RAINBOW run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_RALTS, 7, TRUE } },
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopOrbs },
#endif
        }
    },

    [QUEST_GlitchMode] = 
    {
        .title = _("Glitch Style"),
        .sortIndex = 10,
        .desc = _(
                    "Enable only NONE BOSSES\n"
                    "and FULL NATIONAL DEX and\n"
                    "win a full standard run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_MAREEP, 7, TRUE } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ESCAPE_ROPE, 1 } },
        }
    },
#ifdef ROGUE_EXPANSION
    [QUEST_SinnohMode] = 
    {
        .title = _("Sinnoh Rainbow"),
        .sortIndex = 10,
        .desc = _(
                    "Enable the SINNOH\n"
                    "REGIONAL DEX and win\n"
                    "a full RAINBOW run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_DRIFLOON, 7, TRUE } },
        }
    },
    [QUEST_UnovaMode] = 
    {
        .title = _("Unova Rainbow"),
        .sortIndex = 10,
        .desc = _(
                    "Enable the UNOVA\n"
                    "REGIONAL DEX and win\n"
                    "a full RAINBOW run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_YAMASK, 7, TRUE } },
        }
    },
    [QUEST_KalosMode] = 
    {
        .title = _("Kalos Rainbow"),
        .sortIndex = 10,
        .desc = _(
                    "Enable the KALOS\n"
                    "REGIONAL DEX and win\n"
                    "a full RAINBOW run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_ESPURR, 7, TRUE } },
        }
    },
    [QUEST_AlolaMode] = 
    {
        .title = _("Alola Rainbow"),
        .sortIndex = 10,
        .desc = _(
                    "Enable the ALOLA\n"
                    "REGIONAL DEX and win\n"
                    "a full RAINBOW run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_ROCKRUFF, 7, TRUE } },
        }
    },
    [QUEST_GalarMode] = 
    {
        .title = _("Galar Rainbow"),
        .sortIndex = 10,
        .desc = _(
                    "Enable the GALAR\n"
                    "REGIONAL DEX and win\n"
                    "a full RAINBOW run."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_POKEMON, .params={ SPECIES_HATENNA, 7, TRUE } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopRustedItems },
        }
    },
#endif

#ifdef ROGUE_EXPANSION
    [QUEST_MegaEvo] = 
    {
        .title = _("Mega Evolution"),
        .sortIndex = 200,
        .desc = _(
                    "In the final champion\n"
                    "battle Mega Evolve a\n"
                    "POKéMON."
                ),
        .rewards = {
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopMegas },
        }
    },
    [QUEST_ZMove] = 
    {
        .title = _("Z Power"),
        .sortIndex = 200,
        .desc = _(
                    "In the final champion\n"
                    "battle use a Z-Move."
                ),
        .rewards = {
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopZMoves },
        }
    },

    [QUEST_ShayminItem] = 
    {
        .title = _("Gratitude"),
        .sortIndex = 200,
        .desc = _(
                    "Reach the final champion\n"
                    "with a Shaymin in your\n"
                    "party."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_GRACIDEA, 1 } },
        }
    },
    [QUEST_HoopaItem] = 
    {
        .title = _("Mischief"),
        .sortIndex = 200,
        .desc = _(
                    "Reach the final champion\n"
                    "with a Hoopa in your\n"
                    "party."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_PRISON_BOTTLE, 1 } },
        }
    },
    [QUEST_NatureItem] = 
    {
        .title = _("'Of Nature"),
        .sortIndex = 200,
        .desc = _(
                    "Reach the final champion\n"
                    "with any of the 'Forces of\n"
                    "Nature' in your party."
                ),
        .rewards = {
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_REVEAL_GLASS, 1 } },
        }
    },
#endif

    [QUEST_BerryCollector] = 
    {
        .title = _("Forager"),
        .desc = _(
                    "Collect 10 different types\n"
                    "of Berry whilst on an\n"
                    "Adventure."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 500 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ORAN_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksBerries },
        },
        .unlockedQuests = { 
            QUEST_DenExplorer
        }
    },

    [QUEST_DenExplorer] = 
    {
        .title = _("Wild Den"),
        .desc = _(
                    "Capture a POKéMON from\n"
                    "a Wild Den."
                ),
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_QUALOT_BERRY, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_GREPA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksUpgrade, .giveText=gText_GiveUnlocksBakery },
        }
    },

    [QUEST_NORMAL_Champion] = 
    {
        .title = _("NORMAL Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "NORMAL POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SILK_SCARF, 1 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_CHILAN_BERRY, 1 } },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SILK_SCARF, 1 } },
#endif
        }
    },
    [QUEST_FIGHTING_Champion] = 
    {
        .title = _("FIGHT Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "FIGHTING POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_CHOPLE_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_BLACK_BELT, 1 } },
#endif
        }
    },
    [QUEST_FLYING_Champion] = 
    {
        .title = _("FLYING Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "FLYING POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_COBA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SHARP_BEAK, 1 } },
#endif
        }
    },
    [QUEST_POISON_Champion] = 
    {
        .title = _("POISON Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "POISON POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_KEBIA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_POISON_BARB, 1 } },
#endif
        }
    },
    [QUEST_GROUND_Champion] = 
    {
        .title = _("GROUND Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "GROUND POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SHUCA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SOFT_SAND, 1 } },
#endif
        }
    },
    [QUEST_ROCK_Champion] = 
    {
        .title = _("ROCK Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "ROCK POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_CHARTI_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HARD_STONE, 1 } },
#endif
        }
    },
    [QUEST_BUG_Champion] = 
    {
        .title = _("BUG Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "BUG POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_TANGA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
#endif
        }
    },
    [QUEST_GHOST_Champion] = 
    {
        .title = _("GHOST Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "GHOST POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_KASIB_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_SPELL_TAG, 1 } },
#endif
        }
    },
    [QUEST_STEEL_Champion] = 
    {
        .title = _("STEEL Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "STEEL POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_BABIRI_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_METAL_COAT, 1 } },
#endif
        }
    },
    [QUEST_FIRE_Champion] = 
    {
        .title = _("FIRE Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "FIRE POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_OCCA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_CHARCOAL, 1 } },
#endif
        }
    },
    [QUEST_WATER_Champion] = 
    {
        .title = _("WATER Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "WATER POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_PASSHO_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_MYSTIC_WATER, 1 } },
#endif
        }
    },
    [QUEST_GRASS_Champion] = 
    {
        .title = _("GRASS Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "GRASS POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RINDO_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_MIRACLE_SEED, 1 } },
#endif
        }
    },
    [QUEST_ELECTRIC_Champion] = 
    {
        .title = _("ELECTR Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "ELECTRIC POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_WACAN_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_MAGNET, 1 } },
#endif
        }
    },
    [QUEST_PSYCHIC_Champion] = 
    {
        .title = _("PSYCHC Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "PSYCHIC POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_PAYAPA_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_TWISTED_SPOON, 1 } },
#endif
        }
    },
    [QUEST_ICE_Champion] = 
    {
        .title = _("ICE Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "ICE POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_YACHE_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_NEVER_MELT_ICE, 1 } },
#endif
        }
    },
    [QUEST_DRAGON_Champion] = 
    {
        .title = _("DRAGON Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "DRAGON POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_HABAN_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_DRAGON_FANG, 1 } },
#endif
        }
    },
    [QUEST_DARK_Champion] = 
    {
        .title = _("DARK Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "DARK POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
#ifdef ROGUE_EXPANSION
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_COLBUR_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
#else
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_BLACK_GLASSES, 1 } },
#endif
        }
    },
#ifdef ROGUE_EXPANSION
    [QUEST_FAIRY_Champion] = 
    {
        .title = _("FAIRY Master"),
        .sortIndex = 255,
        .desc = _(
                    "Win a full Run, only\n"
                    "ever catching and using\n"
                    "FAIRY POKéMON."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 5000 } },
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_ROSELI_BERRY, 1 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksTravelShopUpgrade, .giveText=gText_GiveTravelShopPlates },
        }
    },
#endif
};