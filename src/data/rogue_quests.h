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
        .rewards = { 
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 3 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 1000 } } 
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
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 4 } },
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
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 4 } },
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
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 4 } },
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
            { .type=QUEST_REWARD_GIVE_ITEM, .params={ ITEM_RARE_CANDY, 4 } },
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 15000 } } 
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