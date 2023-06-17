

// Make sure to bump ROGUE_COMPAT_VERSION in rogue_controller.c when changing this
const u8 gText_RogueVersion[] = _("v1.3.2a");

#ifdef ROGUE_EXPANSION
const u8 gText_RogueVersionPrefix[] = _("EX");
#else
const u8 gText_RogueVersionPrefix[] = _("Vanilla");
#endif

const u8 gText_RogueVersionSpacer[] = _(" - ");

#if defined(ROGUE_FEATURE_AUTOMATION)
const u8 gText_RogueVersionSuffix[] = _("(AUTOMATION)");
#elif defined(ROGUE_DEBUG)
const u8 gText_RogueVersionSuffix[] = _("(DEBUG)");
#else
const u8 gText_RogueVersionSuffix[] = _("The Regional Update");
#endif

// UI String
//
const u8 gText_StatusRoute[] = _("ROUTE: {STR_VAR_1}\n");
const u8 gText_StatusBadges[] = _("BADGES: {STR_VAR_1}\n");
const u8 gText_StatusScore[] = _("SCORE: {STR_VAR_1}\n");
const u8 gText_StatusTimer[] = _("TIMER: {STR_VAR_1}:{STR_VAR_2}\n");
const u8 gText_StatusClock[] = _("{STR_VAR_1}:{STR_VAR_2}\n");
const u8 gText_StatusSeasonSpring[] = _("·Spring·");
const u8 gText_StatusSeasonSummer[] = _("·Summer·");
const u8 gText_StatusSeasonAutumn[] = _("·Autumn·");
const u8 gText_StatusSeasonWinter[] = _("·Winter·");

// Colours aren't exact as they'll be used in battle UI
const u8 gText_MoveEffective[] = _("{COLOR RED}Neutral");
const u8 gText_MoveNoEffect[] = _("{COLOR TRANSPARENT}No Effect...");
const u8 gText_MoveSuperEffective[] = _("{COLOR LIGHT_GRAY}Effective!");
const u8 gText_MoveNotVeryEffective[] = _("{COLOR GREEN}Not Effective");


// Trainers
//
const u8 gText_TrainerName_Brendan[] = _("BRENDAN");
const u8 gText_TrainerName_May[] = _("MAY");
const u8 gText_TrainerName_Red[] = _("RED");
const u8 gText_TrainerName_Leaf[] = _("LEAF");
const u8 gText_TrainerName_Ethan[] = _("ETHAN");
const u8 gText_TrainerName_Lyra[] = _("LYRA");

// Items
//
const u8 gText_EscapeRopeDesc[] = _(
    "Use to escape\n"
    "instantly from\n"
    "most encounters.");

// Difficulty/Config lab settings
//

//{COLOR DARK_GRAY}{SHADOW DARK_GRAY} <default> orange tint
//{COLOR GREEN}{SHADOW LIGHT_GREEN} no tint
//{COLOR RED}{SHADOW LIGHT_RED} red tint
//{COLOR LIGHT_BLUE}{SHADOW BLUE} green tint
const u8 gText_16Spaces[] = _("                ");

const u8 gText_DifficultySettings[] = _("DIFFICULTY SETTINGS");
const u8 gText_DifficultyArrowLeft[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}{LEFT_ARROW}");
const u8 gText_DifficultyArrowRight[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}{RIGHT_ARROW}");

const u8 gText_DifficultyDoesntAffectReward[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}(rewards unaffected)");
const u8 gText_DifficultyRewardLevel[] = _("{FONT_SMALL}{COLOR GREEN}{SHADOW LIGHT_GREEN}REWARD LEVEL: ");

const u8 gText_DifficultyPreset[] = _("PRESET");
const u8 gText_DifficultyPresetEasy[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}EASY");
const u8 gText_DifficultyPresetMedium[] = _("{COLOR DARK_GRAY}{SHADOW LIGHT_GREEN}AVERAGE");
const u8 gText_DifficultyPresetHard[] = _("{COLOR RED}{SHADOW LIGHT_GREEN}HARD");
const u8 gText_DifficultyPresetBrutal[] = _("{COLOR LIGHT_RED}{SHADOW LIGHT_GREEN}BRUTAL");
const u8 gText_DifficultyPresetCustom[] = _("{COLOR RED}{SHADOW LIGHT_RED}CUSTOM");

const u8 gText_DifficultyEnabled[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}ENABLED  ");
const u8 gText_DifficultyDisabled[] = _("{COLOR LIGHT_RED}{SHADOW LIGHT_GREEN}DISABLED");

const u8 gText_DifficultyToggles[] = _("TOGGLES");
const u8 gText_DifficultySliders[] = _("SLIDERS");

const u8 gText_DifficultyExpAll[] = _("EXP. ALL");
const u8 gText_DifficultyOverLvl[] = _("OVER LEVEL");
const u8 gText_DifficultyEVGain[] = _("EV GAIN");
const u8 gText_DifficultyOverworldMons[] = _("VISIBLE WILD {PKMN}");
const u8 gText_DifficultyBagWipe[] = _("BASIC BAG");
const u8 gText_DifficultySwitchMode[] = _("SWITCH MODE");

const u8 gText_DifficultyTrainers[] = _("TRAINERS");
const u8 gText_DifficultyItems[] = _("ITEMS");
const u8 gText_DifficultyLegendaries[] = _("LEGENDARIES");

const u8 gText_DifficultyPresetDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Developer tuned experience for every\n"
    "skill level."
);

const u8 gText_DifficultyCustomDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Edit individual controls and tune the\n"
    "difficulty to your liking."
);

const u8 gText_DifficultyExpAllDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "All {PKMN} in the party will be awarded EXP\n"
    "even if they didn't enter the battle."
);

const u8 gText_DifficultyOverLvlDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Allows {PKMN} to continue gaining LVLs\n"
    "past the current LVL CAP."
);

const u8 gText_DifficultyEVGainDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "{PKMN} gain EVs from TRAINER battles based\n"
    "on their nature (Trainers never have EVs)"
);

const u8 gText_DifficultyOverworldMonsDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Wild {PKMN} can be encounted and interacted\n"
    "with in the overworld."
);

const u8 gText_DifficultyBagWipeDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "When embarking, you will be given basic\n"
    "items instead of your bag's contents."
);

const u8 gText_DifficultySwitchModeDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "After fainting an opposing {PKMN} you will be\n"
    "given a chance to switch out your {PKMN}."
);

const u8 gText_DifficultyTrainersDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Increases quality, quantity and strength\n"
    "of all TRAINER's {PKMN}."
);

const u8 gText_DifficultyItemsDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "TODO. This needs a rework..."
);

const u8 gText_DifficultyLegendariesDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Decreases quality and quantity of\n"
    "LEGENDARY {PKMN} encounters."
);

// Quest Log
//
const u8 gText_QuestLogTitleOverview[] = _("QUESTS");
const u8 gText_QuestLogTitleDesc[] = _("DESCRIPTION");
const u8 gText_QuestLogTitleRewards[] = _("REWARDS");
const u8 gText_QuestLogTitlePinned[] = _("Pinned");
const u8 gText_QuestLogTitleActive[] = _("In progress");
const u8 gText_QuestLogTitleInactive[] = _("Inactive");
const u8 gText_QuestLogTitleComplete[] = _("Complete");
const u8 gText_QuestLogTitleTodo[] = _("To do");
const u8 gText_QuestLogTitleRepeatable[] = _("Repeatable");
const u8 gText_QuestLogTitleNew[] = _("New!");
const u8 gText_QuestLogTitleStatus[] = _("Status:");
const u8 gText_QuestLogMarkerRepeatable[] = _("REPEATABLE");
const u8 gText_QuestLogMarkerInactive[] = _("{COLOR RED}{SHADOW LIGHT_RED}INACTIVE");
const u8 gText_QuestLogStatusIncomplete[] = _("{COLOR RED}{SHADOW LIGHT_RED}INCOMPLETE");
const u8 gText_QuestLogStatusComplete[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}COMPLETE");
const u8 gText_QuestLogStatusCollection[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}READY TO COLLECT");
const u8 gText_QuestLogStatusCollected[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}ALREADY COLLECTED");
const u8 gText_QuestLogTitleRewardMoney[] = _("¥{STR_VAR_1}");
const u8 gText_QuestLogTitleRewardPokemon[] = _("{STR_VAR_1}");
const u8 gText_QuestLogTitleRewardShinyPokemon[] = _("Shiny {STR_VAR_1}");
const u8 gText_QuestLogTitleQuestUnlocks[] = _("Unlocks new Quests");
const u8 gText_QuestLogPromptOverview[] = _("Select a Category");
const u8 gText_QuestLogPromptCategory[] = _("Select a Quest");
const u8 gText_QuestLogPromptPinQuest[] = _("Pin this Quest?");
const u8 gText_QuestLogPromptUnpinQuest[] = _("Unpin this Quest?");
const u8 gText_QuestLogOverviewCompleted[] = _("Completed:");
const u8 gText_QuestLogOverviewUnlocked[] = _("Unlocked:");
const u8 gText_QuestLogOverviewRewardsToCollect[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}PENDING REWARDS");
const u8 gText_QuestLogBack[] = _("Back");

const u8 gText_QuestRewardGive[] = _("Received {STR_VAR_1}!");
const u8 gText_QuestRewardGiveMon[] = _("Received a {STR_VAR_1}!");
const u8 gText_QuestRewardGiveShinyMon[] = _("Received a Shiny {STR_VAR_1}!");
const u8 gText_QuestRewardGiveMoney[] = _("Received ¥{STR_VAR_1}!");

// Campaigns
//
const u8 gText_CampaignHofTitle[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}{STR_VAR_1}");

const u8 gText_Campaign_None[] = _("???");
const u8 gText_Campaign_LowBST[] = _("BST CUP");
const u8 gText_Campaign_Classic[] = _("Classic Mode");
const u8 gText_Campaign_MiniBossBattler[] = _("Battle Addict");
const u8 gText_Campaign_AutoBattler[] = _("Auto Battler");
const u8 gText_Campaign_LaterManner[] = _("LaterManner Mode");
const u8 gText_Campaign_PokeballLimit[] = _("Limited Capture");
const u8 gText_Campaign_OneHp[] = _("One Hit Mode");

// Popups
//
const u8 gText_Popup_QuestComplete[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Quest Completed!");
const u8 gText_Popup_QuestFail[] = _("{COLOR RED}{SHADOW LIGHT_RED}Quest Failed");
const u8 gText_Popup_LegendaryClause[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Clause Activated!");
const u8 gText_Popup_None[] = _("");

const u8 gPopupText_WeakLegendaryClause[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Basic Legendary");
const u8 gPopupText_StrongLegendaryClause[] = _("{COLOR RED}{SHADOW LIGHT_RED}Strong Legendary");

const u8 gPopupText_CampaignNoneScore[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Campaign Active!");
const u8 gPopupText_CampaignHighScore[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Aim for High Score!");
const u8 gPopupText_CampaignLowScore[] = _("{COLOR RED}{SHADOW LIGHT_RED}Aim for Low Score!");

const u8 gPopupText_SafariArea[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Safari Area");

const u8 gPopupText_StarterWarning[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Unable to evolve\npartner in gen");

const u8 gPopupText_EncounterChain[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Encounter Chain");
const u8 gPopupText_EncounterChainEnd[] = _("{COLOR RED}{SHADOW LIGHT_RED}Chain Lost");

// Debug
//
#ifdef ROGUE_DEBUG
const u8 gText_RogueDebug_Header[] = _("ROGUE DEBUG");
const u8 gText_RogueDebug_Save[] = _("\nSave: ");
const u8 gText_RogueDebug_Room[] = _("\nRoom: ");
const u8 gText_RogueDebug_BossRoom[] = _("\nBossRoom: ");
const u8 gText_RogueDebug_Difficulty[] = _("\nDfcy: ");
const u8 gText_RogueDebug_PlayerLvl[] = _("\nPly lvl: ");
const u8 gText_RogueDebug_WildLvl[] = _("\nWld lvl: ");
const u8 gText_RogueDebug_WildCount[] = _("\nWld Opt: ");
const u8 gText_RogueDebug_ItemCount[] = _("\nItm Opt: ");
const u8 gText_RogueDebug_TrainerCount[] = _("\nTrn Opt: ");
const u8 gText_RogueDebug_Seed[] = _("\nSeed: ");

const u8 gText_RogueDebug_AdvHeader[] = _("ROGUE ADVPATH");
const u8 gText_RogueDebug_AdvCount[] = _("\nCount: ");
const u8 gText_RogueDebug_X[] = _("\nX: ");
const u8 gText_RogueDebug_Y[] = _("\nY: ");
#endif