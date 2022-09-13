
const u8 gText_RogueQuest_Title_None[] = _("NONE");
const u8 gText_RogueQuest_Desc_None[] = _("-");

const u8 gText_RogueQuest_Title_Testing1[] = _("Test Quest 1");
const u8 gText_RogueQuest_Desc_Testing1[] = _("TODO Quest Desc 1");

const u8 gText_RogueQuest_Title_Testing2[] = _("Test Quest 2");
const u8 gText_RogueQuest_Desc_Testing2[] = _("TODO Quest Desc 2");

const struct RogueQuestConstants gRogueQuests[ROGUE_QUEST_COUNT] = 
{
    [ROGUE_QUEST_NONE] = 
    {
        .title = gText_RogueQuest_Title_None,
        .desc = gText_RogueQuest_Desc_None,
        .flags = ROGUE_QUEST_FLAGS_NONE
    },
    [ROGUE_QUEST_Testing1] = 
    {
        .title = gText_RogueQuest_Title_Testing1,
        .desc = gText_RogueQuest_Desc_Testing1,
        .flags = ROGUE_QUEST_FLAGS_SINGLE_MEDAL | ROGUE_QUEST_FLAGS_REPEATABLE | ROGUE_QUEST_FLAGS_GLOBALALLY_TRACKED

    },
    [ROGUE_QUEST_Testing2] = 
    {
        .title = gText_RogueQuest_Title_Testing2,
        .desc = gText_RogueQuest_Desc_Testing2,
        .flags = ROGUE_QUEST_FLAGS_NONE
    },
};