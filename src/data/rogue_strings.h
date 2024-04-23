

// When changing this, make sure to:
// -bump ROGUE_SAVE_VERSION in rogue_save.c
// -update SAVE_VER_ID_X_Y_Z enum in rogue_save.h
// -update RogueSave_GetVersionIdFor()
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
const u8 gText_MoveEffective[] = _("{COLOR BLUE}{SHADOW DYNAMIC_COLOR6}Neutral");
const u8 gText_MoveNoEffect[] = _("{COLOR TRANSPARENT}{SHADOW BLUE}No Effect…");
const u8 gText_MoveSuperEffective[] = _("{COLOR GREEN}{SHADOW DYNAMIC_COLOR6}Effective!");
const u8 gText_MoveNotVeryEffective[] = _("{COLOR WHITE}{SHADOW DYNAMIC_COLOR6}Not Effective");


// Trainers
//
#ifdef ROGUE_EXPANSION
const u8 gText_TrainerName_Default[] = _("Rogue");
#else
const u8 gText_TrainerName_Default[] = _("ROGUE");
#endif

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
const u8 gText_32Spaces[] = _("                                ");

const u8 gText_DifficultySettings[] = _("Difficulty Settings");
const u8 gText_DifficultyArrowLeft[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}{LEFT_ARROW}");
const u8 gText_DifficultyArrowRight[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}{RIGHT_ARROW}");

const u8 gText_DifficultyDoesntAffectReward[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}(rewards unaffected)");
const u8 gText_DifficultyRewardLevel[] = _("{FONT_SMALL}{COLOR GREEN}{SHADOW LIGHT_GREEN}Reward Level: ");

const u8 gText_DifficultyPreset[] = _("Difficulty");
const u8 gText_DifficultyPresetEasy[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Easy");
const u8 gText_DifficultyPresetMedium[] = _("{COLOR DARK_GRAY}{SHADOW LIGHT_GREEN}Average");
const u8 gText_DifficultyPresetHard[] = _("{COLOR RED}{SHADOW LIGHT_GREEN}Hard");
const u8 gText_DifficultyPresetBrutal[] = _("{COLOR LIGHT_RED}{SHADOW LIGHT_GREEN}Brutal");
const u8 gText_DifficultyPresetCustom[] = _("{COLOR RED}{SHADOW LIGHT_RED}Custom");

const u8 gText_DifficultyEnabled[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Enabled  ");
const u8 gText_DifficultyDisabled[] = _("{COLOR LIGHT_RED}{SHADOW LIGHT_GREEN}Disabled");
const u8 gText_DifficultyModeActive[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Active!");

const u8 gText_DifficultyExpAll[] = _("Exp. All");
const u8 gText_DifficultyOverLvl[] = _("Over Level");
const u8 gText_DifficultyEVGain[] = _("EV Gain");
const u8 gText_DifficultyOverworldMons[] = _("Visible Wild {PKMN}");
const u8 gText_DifficultyBagWipe[] = _("True Roguelike");
const u8 gText_DifficultySwitchMode[] = _("Switch Mode");

const u8 gText_DifficultyTrainers[] = _("Trainers");
const u8 gText_DifficultyItems[] = _("Items");
const u8 gText_DifficultyLegendaries[] = _("Legendaries");

const u8 gText_DifficultyCustomDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Edit individual controls and tune the\n"
    "difficulty to your liking."
);

const u8 gText_AdventureCustomDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Edit individual controls and tune the\n"
    "adventure to your liking."
);

const u8 gText_DifficultyExpAllDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "All {PKMN} in the party will be awarded Exp.\n"
    "even if they didn't enter the battle."
);

const u8 gText_DifficultyOverLvlDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Allows {PKMN} to continue gaining lvls\n"
    "past the current lvl cap."
);

const u8 gText_DifficultyEVGainDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "{PKMN} gain EVs from Trainer battles based\n"
    "on their nature (Trainers never have EVs)"
);

const u8 gText_DifficultyOverworldMonsDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Wild {PKMN} can be encounted and interacted\n"
    "with in the overworld."
);

const u8 gText_DifficultyBagWipeDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Only your Partner {PKMN} and Key Items will\n"
    "be taken into runs."
);

const u8 gText_DifficultySwitchModeDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "After fainting an opposing {PKMN} you will be\n"
    "given a chance to switch out your {PKMN}."
);

const u8 gText_DifficultyTrainersDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Increases quality, quantity and strength\n"
    "of all Trainer's {PKMN}."
);

const u8 gText_DifficultyItemsDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "TODO. This needs a rework..."
);

const u8 gText_DifficultyLegendariesDesc[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Decreases quality and quantity of\n"
    "Legendary {PKMN} encounters."
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