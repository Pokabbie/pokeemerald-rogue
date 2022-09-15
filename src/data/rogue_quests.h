const struct RogueQuestConstants gRogueQuests[QUEST_COUNT] = 
{
    [QUEST_NONE] = 
    {
        .title = _("-"),
        .desc = _("-"),
        .flags = QUEST_FLAGS_NONE
    },
    [QUEST_FirstAdventure] = 
    {
        .title = _("To adventure!"),
        .desc = _(
                    "Embark on your first\n"
                    "adventure!"
                ),
        .flags = QUEST_FLAGS_REPEATABLE | QUEST_FLAGS_GLOBALALLY_TRACKED,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 3 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 1000 } } 
        }
    },
    [QUEST_Testing1] = 
    {
        .title = _("Test Quest 1"),
        .desc = _(
                    "A difficult quest in which,\n"
                    "you will have to do things."
                ),
        .flags = QUEST_FLAGS_REPEATABLE | QUEST_FLAGS_GLOBALALLY_TRACKED,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } } 
        }
    },
    [QUEST_Testing2] = 
    {
        .title = _("Test Quest 2"),
        .desc = _(
                    "A 2nd difficult quest in,\n"
                    "which you will have to do\n"
                    "things."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } } ,
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_POTION, 1 } } ,
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 15020 } } 
        }
    },

    [QUEST_Electric_Master] = 
    {
        .title = _("Elec Master"),
        .desc = _(
                    "Only use Electric POKéMON\n"
                    "during an adventure and\n"
                    "defeat all GYM Leaders."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } } ,
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_POTION, 1 } } ,
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 15020 } } 
        }
    },
    [QUEST_Electric_Champion] = 
    {
        .title = _("Elec Champion"),
        .desc = _(
                    "Only use Electric POKéMON\n"
                    "during an adventure and\n"
                    "defeat the Final Champion."
                ),
        .flags = QUEST_FLAGS_NONE,
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 1 } } ,
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_POTION, 1 } } ,
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 15020 } } 
        }
    },
};