

// Make sure to bump ROGUE_COMPAT_VERSION in rogue_controller.c when changing this
const u8 gText_RogueVersion[] = _("v1.2.1");

#ifdef ROGUE_EXPANSION
const u8 gText_RogueVersionPrefix[] = _("EX");
#else
const u8 gText_RogueVersionPrefix[] = _("Vanilla");
#endif

const u8 gText_RogueVersionSpacer[] = _(" - ");

#ifdef ROGUE_DEBUG
const u8 gText_RogueVersionSuffix[] = _("(DEBUG)");
#else
const u8 gText_RogueVersionSuffix[] = _("The Questing Update");
#endif


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

const u8 gText_TrainerName_Kate[] = _("Kate");
const u8 gText_TrainerName_Raven[] = _("Raven");
const u8 gText_TrainerName_Erma[] = _("Erma");
const u8 gText_TrainerName_Tails[] = _("TMK4");
const u8 gText_TrainerName_Pokabbie[] = _("POKABBIE'S DITTO");

// Items
//
#ifdef ROGUE_EXPANSION
const u8 gText_ItemLinkCable[] = _("Link Cable");
const u8 gText_ItemQuestLog[] = _("Quest Log");

const u8 gText_ItemShoppingCharm[] = _("Discount Charm");
const u8 gText_ItemFlinchCharm[] = _("Flinch Charm");
const u8 gText_ItemCritCharm[] = _("Crit Charm");
const u8 gText_ItemShedSkinCharm[] = _("Shed Skin Charm");
const u8 gText_ItemWildIVCharm[] = _("Strength Charm");
const u8 gText_ItemCatchingCharm[] = _("Catching Charm");
const u8 gText_ItemGraceCharm[] = _("Grace Charm");
const u8 gText_ItemEncounterCharm[] = _("Wild Charm");

const u8 gText_ItemShoppingCurse[] = _("Discount Curse");
const u8 gText_ItemFlinchCurse[] = _("Flinch Curse");
const u8 gText_ItemCritCurse[] = _("Crit Curse");
const u8 gText_ItemShedSkinCurse[] = _("Shed Skin Curse");
const u8 gText_ItemWildIVCurse[] = _("Strength Curse");
const u8 gText_ItemCatchingCurse[] = _("Catching Curse");
const u8 gText_ItemGraceCurse[] = _("Grace Curse");
const u8 gText_ItemEncounterCurse[] = _("Wild Curse");
const u8 gText_ItemPartyCurse[] = _("Capacity Curse");
const u8 gText_ItemEverstoneCurse[] = _("Everstone Curse");
const u8 gText_ItemBattleItemCurse[] = _("Embargo Curse");
const u8 gText_ItemSpeciesClauseCurse[] = _("Species Curse");
const u8 gText_ItemItemShuffleCurse[] = _("Shuffle Curse");

#else

const u8 gText_ItemLinkCable[] = _("LINK CABLE");
const u8 gText_ItemQuestLog[] = _("QUEST LOG");

const u8 gText_ItemShoppingCharm[] = _("DISCOUNT CHARM");
const u8 gText_ItemFlinchCharm[] = _("FLINCH CHARM");
const u8 gText_ItemCritCharm[] = _("CRIT CHARM");
const u8 gText_ItemShedSkinCharm[] = _("SHED SKIN CHARM");
const u8 gText_ItemWildIVCharm[] = _("STRENGTH CHARM");
const u8 gText_ItemCatchingCharm[] = _("CATCHING CHARM");
const u8 gText_ItemGraceCharm[] = _("GRACE CHARM");
const u8 gText_ItemEncounterCharm[] = _("WILD CHARM");

const u8 gText_ItemShoppingCurse[] = _("DISCOUNT CURSE");
const u8 gText_ItemFlinchCurse[] = _("FLINCH CURSE");
const u8 gText_ItemCritCurse[] = _("CRIT CURSE");
const u8 gText_ItemShedSkinCurse[] = _("SHED SKIN CURSE");
const u8 gText_ItemWildIVCurse[] = _("STRENGTH CURSE");
const u8 gText_ItemCatchingCurse[] = _("CATCHING CURSE");
const u8 gText_ItemGraceCurse[] = _("GRACE CURSE");
const u8 gText_ItemEncounterCurse[] = _("WILD CURSE");
const u8 gText_ItemPartyCurse[] = _("CAPACITY CURSE");
const u8 gText_ItemEverstoneCurse[] = _("EVERSTONE CURSE");
const u8 gText_ItemBattleItemCurse[] = _("EMBARGO CURSE");
const u8 gText_ItemSpeciesClauseCurse[] = _("SPECIES CURSE");
const u8 gText_ItemItemShuffleCurse[] = _("SHUFFLE CURSE");

#endif


const u8 gText_ItemPlaceholderDesc[] = _(
    "Placeholder\n"
    "TODO");

const u8 gText_ItemLinkCableDesc[] = _(
    "Makes certain\n"
    "species of POKéMON\n"
    "evolve when held.");

const u8 gText_EscapeRopeDesc[] = _(
    "Use to escape\n"
    "instantly from\n"
    "some encounters.");

const u8 gText_ItemQuestLogDesc[] = _(
    "A digital log for\n"
    "tracking Quests\n"
    "and their rewards.");


const u8 sItemShoppingCharmDesc[] = _(
    "Shop prices are\n"
    "reduced by 20%");

const u8 sItemFlinchCharmDesc[] = _(
    "Friendly moves\n"
    "have an extra 10%\n"
    "chance to flinch.");

const u8 sItemCritCharmDesc[] = _(
    "Friendly POKéMON\n"
    "have an increased\n"
    "crit rate.");

const u8 sItemShedSkinCharmDesc[] = _(
    "Friendly POKéMON\n"
    "have a 20% chance\n"
    "to cure status.");

const u8 sItemWildIVCharmDesc[] = _(
    "Caught POKéMON are\n"
    "slightly stronger.");

const u8 sItemCatchingCharmDesc[] = _(
    "BALL effectiveness\n"
    "is increased by\n"
    "100%.");

const u8 sItemGraceCharmDesc[] = _(
    "Friendly moves\n"
    "have an extra 75%\n"
    "for extra effects.");

const u8 sItemEncounterCharmDesc[] = _(
    "Routes contain 2\n"
    "additional wild\n"
    "POKéMON.");
    
    
const u8 sItemShoppingCurseDesc[] = _(
    "Shop prices are\n"
    "increased by 20%");


const u8 sItemFlinchCurseDesc[] = _(
    "Opponent moves\n"
    "have an extra 5%\n"
    "chance to flinch.");

const u8 sItemCritCurseDesc[] = _(
    "Opponent POKéMON\n"
    "have an increased\n"
    "crit rate.");

const u8 sItemShedSkinCurseDesc[] = _(
    "Opponent POKéMON\n"
    "have a 15% chance\n"
    "to cure status.");

const u8 sItemWildIVCurseDesc[] = _(
    "Caught POKéMON are\n"
    "slightly weaker.");

const u8 sItemCatchingCurseDesc[] = _(
    "BALL effectiveness\n"
    "is decreased by\n"
    "25%.");

const u8 sItemGraceCurseDesc[] = _(
    "Opponent moves\n"
    "have an extra 50%\n"
    "for extra effects.");

const u8 sItemEncounterCurseDesc[] = _(
    "Routes contain 1\n"
    "less wild POKéMON.");

const u8 sItemPartyCurseDesc[] = _(
    "The maximum number\n"
    "of POKéMON in your\n"
    "party is reduced.");

const u8 sItemEverstoneCurseDesc[] = _(
    "LVL UP Evolutions\n"
    "become LVL whilst\n"
    "holding LINK CABLE.");

const u8 sItemBattleItemCurseDesc[] = _(
    "Items cannot be\n"
    "used in battle.\n"
    "(Except POKéBALLS)");

const u8 sItemSpeciesClauseCurseDesc[] = _(
    "POKéMON can't be\n"
    "caught if species\n"
    "in party already.");

const u8 sItemItemShuffleCurseDesc[] = _(
    "Entering a Battle\n"
    "will shuffle Held\n"
    "Items between {PKMN}.");


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
const u8 gText_QuestLogMarkerInactive[] = _("INACTIVE");
const u8 gText_QuestLogStatusIncomplete[] = _("INCOMPLETE");
const u8 gText_QuestLogStatusComplete[] = _("COMPLETE");
const u8 gText_QuestLogStatusCollection[] = _("READY TO COLLECT");
const u8 gText_QuestLogStatusCollected[] = _("ALREADY COLLECTED");
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
const u8 gText_QuestLogOverviewRewardsToCollect[] = _("PENDING REWARDS");
const u8 gText_QuestLogBack[] = _("Back");

const u8 gText_QuestRewardGive[] = _("Recieved {STR_VAR_1}!");
const u8 gText_QuestRewardGiveMon[] = _("Recieved a {STR_VAR_1}!");
const u8 gText_QuestRewardGiveShinyMon[] = _("Recieved a Shiny {STR_VAR_1}!");
const u8 gText_QuestRewardGiveMoney[] = _("Recieved ¥{STR_VAR_1}!");

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