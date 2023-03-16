

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
const u8 gText_StatusClock[] = _("CLOCK: {STR_VAR_1}:{STR_VAR_2}\n");

// Colours aren't exact as they'll be used in battle UI
const u8 gText_MoveEffective[] = _("{COLOR RED}Neutral");
const u8 gText_MoveNoEffect[] = _("{COLOR TRANSPARENT}No Effect...");
const u8 gText_MoveSuperEffective[] = _("{COLOR LIGHT_GRAY}Effective!");
const u8 gText_MoveNotVeryEffective[] = _("{COLOR GREEN}Not Effective");


// Trainers
//
const u8 gText_TrainerNameChallenger[] = _("CHALLENGER");
const u8 gText_TrainerNameGrunt[] = _("GRUNT");

const u8 gText_TrainerName_TateLiza[] = _("LIZA");
const u8 gText_TrainerName_Anabel[] = _("ANABEL");

const u8 gText_TrainerName_Brock[] = _("BROCK");
const u8 gText_TrainerName_Misty[] = _("MISTY");
const u8 gText_TrainerName_LtSurge[] = _("LT. SURGE");
const u8 gText_TrainerName_Erika[] = _("ERIKA");
const u8 gText_TrainerName_Koga[] = _("KOGA");
const u8 gText_TrainerName_Sabrina[] = _("SABRINA");
const u8 gText_TrainerName_Blaine[] = _("BLAINE");
const u8 gText_TrainerName_Giovanni[] = _("GIOVANNI");

const u8 gText_TrainerName_Lorelei[] = _("LORELEI");
const u8 gText_TrainerName_Bruno[] = _("BRUNO");
const u8 gText_TrainerName_Agatha[] = _("AGATHA");
const u8 gText_TrainerName_Lance[] = _("LANCE");

const u8 gText_TrainerName_Blue[] = _("BLUE");
const u8 gText_TrainerName_ProfOak[] = _("PROF. OAK");

const u8 gText_TrainerName_Brendan[] = _("BRENDAN");
const u8 gText_TrainerName_May[] = _("MAY");
const u8 gText_TrainerName_Red[] = _("RED");
const u8 gText_TrainerName_Leaf[] = _("LEAF");
const u8 gText_TrainerName_Ethan[] = _("ETHAN");
const u8 gText_TrainerName_Lyra[] = _("LYRA");

const u8 gText_TrainerName_Falkner[] = _("FALKNER");
const u8 gText_TrainerName_Bugsy[] = _("BUGSY");
const u8 gText_TrainerName_Whitney[] = _("WHITNEY");
const u8 gText_TrainerName_Morty[] = _("MORTY");
const u8 gText_TrainerName_Chuck[] = _("CHUCK");
const u8 gText_TrainerName_Jasmine[] = _("JASMINE");
const u8 gText_TrainerName_Pryce[] = _("PRYCE");
const u8 gText_TrainerName_Clair[] = _("CLAIR");

const u8 gText_TrainerName_Will[] = _("WILL");
const u8 gText_TrainerName_Karen[] = _("KAREN");

const u8 gText_TrainerName_Kate[] = _("KATE");
const u8 gText_TrainerName_Raven[] = _("RAVEN");
const u8 gText_TrainerName_Erma[] = _("ERMA");
const u8 gText_TrainerName_Tails[] = _("TMK4");
const u8 gText_TrainerName_Pokabbie[] = _("POKABBIE'S DITTO");

// Items
//
const u8 gText_EscapeRopeDesc[] = _(
    "Use to escape\n"
    "instantly from\n"
    "most encounters.");


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