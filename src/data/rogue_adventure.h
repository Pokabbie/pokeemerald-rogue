

//struct RogueAdvPathGenerator // Attach to mode Difficult Option???
//{
//   u8 minLength;
//   u8 maxLength;
//   u16 roomWeights[ADVPATH_ROOM_WEIGHT_COUNT];
//   u16 maxRoomCount[ADVPATH_ROOM_WEIGHT_COUNT];
//   u16 subRoomWeights[ADVPATH_SUBROOM_WEIGHT_COUNT];
//};
//
//struct RogueAdventurePhase
//{
//    u8 levelCap;
//    u8 levelStep;
//    u16 bossTrainerFlagsInclude;
//    u16 bossTrainerFlagsExclude;
//    struct RogueAdvPathGenerator pathGenerator;
//};

static const struct RogueAdventurePhase sAdventure_Standard[] = 
{
    {
        .levelCap = 15,
        .levelStep = 3,
        .bossTrainerFlagsInclude = TRAINER_FLAG_GYM,
        .bossTrainerFlagsExclude = TRAINER_FLAG_NONE,
        .pathGenerator = 
        {
            .minLength = 3,
            .maxLength = 3,
            .roomWeights = 
            {
                [ADVPATH_ROOM_NONE] = 500,
                [ADVPATH_ROOM_ROUTE] = 500,
                [ADVPATH_ROOM_RESTSTOP] = 250,
                [ADVPATH_ROOM_LEGENDARY] = 0,
                [ADVPATH_ROOM_MINIBOSS] = 0,
                [ADVPATH_ROOM_WILD_DEN] = 40,
                [ADVPATH_ROOM_GAMESHOW] = 0,
                [ADVPATH_ROOM_DARK_DEAL] = 0,
                [ADVPATH_ROOM_LAB] = 0
            },
            .maxRoomCount = 
            {
                [ADVPATH_ROOM_NONE] = 0,
                [ADVPATH_ROOM_ROUTE] = 0,
                [ADVPATH_ROOM_RESTSTOP] = 0,
                [ADVPATH_ROOM_LEGENDARY] = 1,
                [ADVPATH_ROOM_MINIBOSS] = 0,
                [ADVPATH_ROOM_WILD_DEN] = 2,
                [ADVPATH_ROOM_GAMESHOW] = 2,
                [ADVPATH_ROOM_DARK_DEAL] = 1,
                [ADVPATH_ROOM_LAB] = 1
            },
            .subRoomWeights = 
            {
                [ADVPATH_SUBROOM_ROUTE_CALM] = 500,
                [ADVPATH_SUBROOM_ROUTE_AVERAGE] = 250,
                [ADVPATH_SUBROOM_ROUTE_TOUGH] = 50,
                [ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 100,
                [ADVPATH_SUBROOM_RESTSTOP_SHOP] = 100,
                [ADVPATH_SUBROOM_RESTSTOP_FULL] = 20,
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