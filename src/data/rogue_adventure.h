#include "constants/rogue.h"

// Test block of code to move adv path generator over to structs
// Feels a bit ott :/


//#define ADVPATH_SUBROOM_ROUTE_CALM          0
//#define ADVPATH_SUBROOM_ROUTE_AVERAGE       1
//#define ADVPATH_SUBROOM_ROUTE_TOUGH         2
//#define ADVPATH_SUBROOM_COUNT               3
//
//#define ADVPATH_SUBROOM_RESTSTOP_BATTLE     0
//#define ADVPATH_SUBROOM_RESTSTOP_SHOP       1
//#define ADVPATH_SUBROOM_RESTSTOP_FULL       2
//#define ADVPATH_SUBROOM_RESTSTOP_COUNT      3
//
//#define ADVPATH_SUBROOM_MAX_COUNT (max(ADVPATH_SUBROOM_COUNT, ADVPATH_SUBROOM_RESTSTOP_COUNT))

//#define ADVPATH_WEIGHT_MODIFY_NONE      0
//#define ADVPATH_WEIGHT_MODIFY_SET       1
//#define ADVPATH_WEIGHT_MODIFY_ADD       2
//#define ADVPATH_WEIGHT_MODIFY_SUB       3
//#define ADVPATH_WEIGHT_MODIFY_MUL       4
//#define ADVPATH_WEIGHT_MODIFY_DIV       5
//
//struct RogueAdvRoomWeightMethod
//{
//    u16 baseWeight;
//    u16 targetWeight;
//    u8 fromDifficulty;// inclusive
//    u8 toDifficulty; // inclusive
//};
//
//struct RogueAdvRoomWeightModifier
//{
//    u8 modifyType;
//    u16 amount;
//    u8 difficultyModuloCondition;
//};
//
//
//struct RogueAdvRoomWeightRange
//{
//    u8 fromDifficulty;// inclusive
//    u8 toDifficulty; // inclusive
//    struct RogueAdvRoomWeightMethod method;
//};
//
//#define MAX_SUPPORTED_WEIGHT_METHODS 4
//
//#define MAX_DIFFICULTY 32 // <- todo swap out 20, just placeholder
//
//// Difficulty ranges (used for method funcs)
//#define DIFFICULTY_RANGE(from, to) .fromDifficulty=from, .toDifficulty=to
//#define FULL_DIFFICULTY_RANGE .fromDifficulty=0, .toDifficulty=MAX_DIFFICULTY
//
//// Method funcs
//#define IMPOSSIBLE_SPAWN_METHOD .baseWeight=0, .targetWeight=0
//#define SET_WEIGHT_METHOD(weight) .baseWeight=weight, .targetWeight=weight
//#define SCALING_BETWEEN_WEIGHT_METHOD(fromWeight, toWeight, min, max) .baseWeight=fromWeight, .targetWeight=toWeight
//
//// Modify funcs
//#define MODIFY_WEIGHT(type, value) { .modifyType=ADVPATH_WEIGHT_MODIFY_##type, .amount=value }
//#define MODIFY_IMPOSSIBLE MODIFY_WEIGHT(SET, 0)
//
//struct RogueAdvRoomWeighting
//{
//    struct RogueAdvRoomWeightRange defaultMethod[MAX_SUPPORTED_WEIGHT_METHODS];
//    struct RogueAdvRoomWeightModifier nextRoomModifiers[ADVPATH_ROOM_COUNT];
//    struct RogueAdvRoomWeightRange defaultSubRoomMethod[ADVPATH_SUBROOM_WEIGHT_COUNT][MAX_SUPPORTED_WEIGHT_METHODS];
//
//
//
//   u8 minLength;
//   u8 maxLength;
//   u16 roomWeights[ADVPATH_ROOM_WEIGHT_COUNT];
//   u16 maxRoomCount[ADVPATH_ROOM_WEIGHT_COUNT];
//   u16 subRoomWeights[ADVPATH_SUBROOM_WEIGHT_COUNT];
//
//   u16 subRoomWeights[ADVPATH_SUBROOM_WEIGHT_COUNT];
//};
//
//struct RogueAdvPathGenerator // Attach to mode Difficult Option???
//{
//    u8 minLength;
//    u8 maxLength;
//    struct RogueAdvRoomWeighting roomWeights[ADVPATH_ROOM_WEIGHT_COUNT];
//
//   u16 roomWeights[ADVPATH_ROOM_WEIGHT_COUNT];
//   u16 maxRoomCount[ADVPATH_ROOM_WEIGHT_COUNT];
//   u16 subRoomWeights[ADVPATH_SUBROOM_WEIGHT_COUNT];
//};
////
////struct RogueAdventurePhase
////{
////    u8 levelCap;
////    u8 levelStep;
////    u16 bossTrainerFlagsInclude;
////    u16 bossTrainerFlagsExclude;
////    struct RogueAdvPathGenerator pathGenerator;
////};
//
//static const struct struct RogueAdvPathGenerator sPathGenerator_Standard[] = 
//{
//    {
//        .minLength = 3,
//        .maxLength = 3,
//        .roomWeights = 
//        {
//            [ADVPATH_ROOM_NONE] = 
//            {
//                .defaultMethod = 
//                {
//                    { DIFFICULTY_RANGE(0, 7), SCALING_BETWEEN_WEIGHT_METHOD(50, 200) },
//                    { DIFFICULTY_RANGE(8, MAX_DIFFICULTY), SET_WEIGHT_METHOD(1200) },
//                }
//                .nextRoomModifiers = 
//                {
//                    [ADVPATH_ROOM_LEGENDARY] = { MODIFY_IMPOSSIBLE },
//                    [ADVPATH_ROOM_ROUTE] = { MODIFY_WEIGHT(ADD, 300) },
//                    [ADVPATH_ROOM_NONE] = { MODIFY_WEIGHT(DIV, 2) }
//                }
//            },
//            [ADVPATH_ROOM_ROUTE] = 
//            {
//                .defaultMethod = 
//                {
//                    { DIFFICULTY_RANGE(0, 5), SCALING_BETWEEN_WEIGHT_METHOD(2000, 1500) },
//                    { DIFFICULTY_RANGE(6, 7), SET_WEIGHT_METHOD(1500) },
//                    { DIFFICULTY_RANGE(8, MAX_DIFFICULTY), SET_WEIGHT_METHOD(400) },
//                }
//                .nextRoomModifiers = 
//                {
//                    [ADVPATH_ROOM_BOSS] = { MODIFY_WEIGHT(SET, 100), } 
//                },
//                .defaultSubRoomMethod =
//                {
//                    [ADVPATH_SUBROOM_ROUTE_CALM] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 7), SET_WEIGHT_METHOD(200) },
//                        { DIFFICULTY_RANGE(8, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                    },
//                    [ADVPATH_SUBROOM_ROUTE_AVERAGE] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 7), SET_WEIGHT_METHOD(300) },
//                        { DIFFICULTY_RANGE(8, MAX_DIFFICULTY), IMPOSSIBLE_SPAWN_METHOD },
//                    },
//                    [ADVPATH_SUBROOM_ROUTE_TOUGH] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 7), SET_WEIGHT_METHOD(100) },
//                        { DIFFICULTY_RANGE(8, MAX_DIFFICULTY), SET_WEIGHT_METHOD(500) },
//                    },
//                },
//            },
//            [ADVPATH_ROOM_RESTSTOP] =
//            {
//                .defaultMethod = 
//                {
//                    { DIFFICULTY_RANGE(0, 0), SET_WEIGHT_METHOD(0) },
//                    { DIFFICULTY_RANGE(1, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                }
//                .nextRoomModifiers = 
//                {
//                    [ADVPATH_ROOM_BOSS] = SET_WEIGHT_METHOD(1500),
//                    [ADVPATH_ROOM_LEGENDARY] = SET_WEIGHT_METHOD(0),
//                    [ADVPATH_ROOM_RESTSTOP] = SET_WEIGHT_METHOD(0),
//                },
//                .defaultSubRoomMethod =
//                {
//                    [ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 11), SET_WEIGHT_METHOD(300) },
//                        { DIFFICULTY_RANGE(11, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                    },
//                    [ADVPATH_SUBROOM_RESTSTOP_SHOP] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 11), SET_WEIGHT_METHOD(300) },
//                        { DIFFICULTY_RANGE(11, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                    },
//                    [ADVPATH_SUBROOM_RESTSTOP_FULL] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 11), SET_WEIGHT_METHOD(100) },
//                        { DIFFICULTY_RANGE(11, MAX_DIFFICULTY), SET_WEIGHT_METHOD(500) },
//                    },
//                },
//            },
//            [ADVPATH_ROOM_LEGENDARY] =
//            {
//                .defaultMethod = 
//                {
//                    { DIFFICULTY_RANGE(0, 0), SET_WEIGHT_METHOD(0) },
//                    { DIFFICULTY_RANGE(1, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                }
//                .nextRoomModifiers = 
//                {
//                    [ADVPATH_ROOM_BOSS] = SET_WEIGHT_METHOD(1500),
//                    [ADVPATH_ROOM_LEGENDARY] = SET_WEIGHT_METHOD(0),
//                    [ADVPATH_ROOM_RESTSTOP] = SET_WEIGHT_METHOD(0),
//                },
//                .defaultSubRoomMethod =
//                {
//                    [ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 11), SET_WEIGHT_METHOD(300) },
//                        { DIFFICULTY_RANGE(11, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                    },
//                    [ADVPATH_SUBROOM_RESTSTOP_SHOP] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 11), SET_WEIGHT_METHOD(300) },
//                        { DIFFICULTY_RANGE(11, MAX_DIFFICULTY), SET_WEIGHT_METHOD(100) },
//                    },
//                    [ADVPATH_SUBROOM_RESTSTOP_FULL] = 
//                    {
//                        { DIFFICULTY_RANGE(0, 11), SET_WEIGHT_METHOD(100) },
//                        { DIFFICULTY_RANGE(11, MAX_DIFFICULTY), SET_WEIGHT_METHOD(500) },
//                    },
//                },
//            },
//
//
//            
//#define ADVPATH_ROOM_LEGENDARY  3
//#define ADVPATH_ROOM_MINIBOSS   4
//#define ADVPATH_ROOM_WILD_DEN   5
//#define ADVPATH_ROOM_GAMESHOW   6
//#define ADVPATH_ROOM_DARK_DEAL  7
//#define ADVPATH_ROOM_LAB        8
//
//            [ADVPATH_ROOM_RESTSTOP] = 250,
//            [ADVPATH_ROOM_LEGENDARY] = 0,
//            [ADVPATH_ROOM_MINIBOSS] = 0,
//            [ADVPATH_ROOM_WILD_DEN] = 40,
//            [ADVPATH_ROOM_GAMESHOW] = 0,
//            [ADVPATH_ROOM_DARK_DEAL] = 0,
//            [ADVPATH_ROOM_LAB] = 0
//        },
//        .maxRoomCount = 
//        {
//            [ADVPATH_ROOM_NONE] = 0,
//            [ADVPATH_ROOM_ROUTE] = 0,
//            [ADVPATH_ROOM_RESTSTOP] = 0,
//            [ADVPATH_ROOM_LEGENDARY] = 1,
//            [ADVPATH_ROOM_MINIBOSS] = 0,
//            [ADVPATH_ROOM_WILD_DEN] = 2,
//            [ADVPATH_ROOM_GAMESHOW] = 2,
//            [ADVPATH_ROOM_DARK_DEAL] = 1,
//            [ADVPATH_ROOM_LAB] = 1
//        },
//        .subRoomWeights = 
//        {
//            [ADVPATH_SUBROOM_ROUTE_CALM] = 500,
//            [ADVPATH_SUBROOM_ROUTE_AVERAGE] = 250,
//            [ADVPATH_SUBROOM_ROUTE_TOUGH] = 50,
//            [ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 100,
//            [ADVPATH_SUBROOM_RESTSTOP_SHOP] = 100,
//            [ADVPATH_SUBROOM_RESTSTOP_FULL] = 20,
//        }
//    }
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