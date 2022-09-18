static const u8 gText_PreviewUnlocksBuilding[] = _("New HUB Building");
static const u8 gText_GiveUnlocksBuilding[] = _("New HUB Building Unlocked!");

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
            { .type=QUEST_REWARD_GIVE_MONEY, .params={ 1000 } },
            { .type=QUEST_REWARD_CUSTOM_TEXT, .previewText=gText_PreviewUnlocksBuilding, .giveText=gText_GiveUnlocksBuilding },
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
        },
        .unlockedQuests = { 
            QUEST_Electric_Master, 
            QUEST_Electric_Champion
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