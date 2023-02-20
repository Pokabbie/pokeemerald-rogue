
static const struct RogueAdventurePhase sAdventure_Standard[] = 
{
    {
        .levelCap = 15,
        .levelStep = 3,
        .bossTrainerFlagsInclude = TRAINER_FLAG_GYM,
        .bossTrainerFlagsExclude = TRAINER_FLAG_NONE,
        .pathGenerator = 
        {
            .minLength = 5,
            .maxLength = 6,
            //.minLength = 10,
            //.maxLength = 10,
            .roomWeights = 
            {
                [ADVPATH_ROOM_NONE] = 100,
                [ADVPATH_ROOM_ROUTE] = 100,
                [ADVPATH_ROOM_RESTSTOP] = 50,
                [ADVPATH_ROOM_LEGENDARY] = 10,
                [ADVPATH_ROOM_MINIBOSS] = 20,
                [ADVPATH_ROOM_WILD_DEN] = 30,
                [ADVPATH_ROOM_GAMESHOW] = 30,
                [ADVPATH_ROOM_DARK_DEAL] = 30,
                [ADVPATH_ROOM_LAB] = 10
            },
            .subRoomWeights = 
            {
                [ADVPATH_SUBROOM_ROUTE_CALM] = 100,
                [ADVPATH_SUBROOM_ROUTE_AVERAGE] = 100,
                [ADVPATH_SUBROOM_ROUTE_TOUGH] = 100,
                [ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 100,
                [ADVPATH_SUBROOM_RESTSTOP_SHOP] = 100,
                [ADVPATH_SUBROOM_RESTSTOP_FULL] = 100,
            }
        }
    }
};

const struct RogueAdventureSettings gRogueAdventureSettings[] = 
{
    [ROGUE_ADV_STANDARD] = 
    {
        .phases = sAdventure_Standard,
        .phaseCount = ARRAY_COUNT(sAdventure_Standard),
    }
};


//#define ROGUE_ADV_STANDARD      0
//#define ROGUE_ADV_RAINBOW       1
//#define ROGUE_ADV_GAUNTLET      2