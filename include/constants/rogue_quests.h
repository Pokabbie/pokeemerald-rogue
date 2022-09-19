
#define QUEST_FLAGS_NONE                  0
#define QUEST_FLAGS_REPEATABLE            (1 << 0) // By default quest are only completable once
#define QUEST_FLAGS_GLOBALALLY_TRACKED    (1 << 1) // By default quests are only completable once

#define QUEST_TITLE_LENGTH 16
#define QUEST_DESC_LENGTH 84

#define  QUEST_MAX_REWARD_COUNT     3
#define  QUEST_MAX_FOLLOWING_QUESTS 38

#define  QUEST_REWARD_NONE          0
#define  QUEST_REWARD_SET_FLAG      1 // Params(1): flag
#define  QUEST_REWARD_CLEAR_FLAG    2 // Params(1): flag
#define  QUEST_REWARD_GIVE_ITEM     3 // Params(2): item, count
#define  QUEST_REWARD_GIVE_MONEY    4 // Params(2): amount
#define  QUEST_REWARD_CUSTOM_TEXT   5 // Params(0)


#define QUEST_NONE                  0 // Have to keep 0 as NONE so memzero works
#define QUEST_FirstAdventure        1
#define QUEST_Gym1                  2
#define QUEST_Gym2                  3
#define QUEST_Gym3                  4
#define QUEST_Gym4                  5
#define QUEST_Gym5                  6
#define QUEST_Gym6                  7
#define QUEST_Gym7                  8
#define QUEST_Gym8                  9
#define QUEST_GymChallenge          10
#define QUEST_GymMaster             11
#define QUEST_EliteMaster           12
#define QUEST_Champion              13

#define QUEST_Collector1            (QUEST_Champion + 1)
#define QUEST_Collector2            (QUEST_Champion + 2)
#define QUEST_ShoppingSpree         (QUEST_Champion + 3)
#define QUEST_BigSaver              (QUEST_Champion + 4)
#define QUEST_Bike1                 (QUEST_Champion + 5)
#define QUEST_Bike2                 (QUEST_Champion + 6)
#define QUEST_Bike2                 (QUEST_Champion + 6)
#define QUEST_NoFainting1           (QUEST_Champion + 7)
#define QUEST_NoFainting2           (QUEST_Champion + 8)
#define QUEST_NoFainting3           (QUEST_Champion + 9)
#define QUEST_MrRandoman            (QUEST_Champion + 10)
#define QUEST_ChaosChampion         (QUEST_Champion + 11)

#define QUEST_Electric_Master       (QUEST_ChaosChampion + 1)
#define QUEST_Electric_Champion     (QUEST_ChaosChampion + 2)

#define QUEST_FIRST QUEST_FirstAdventure
#define QUEST_LAST QUEST_Electric_Champion
#define QUEST_CAPACITY (QUEST_LAST + 1)