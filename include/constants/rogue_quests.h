
#define QUEST_FLAGS_NONE                  0
#define QUEST_FLAGS_REPEATABLE            (1 << 0) // By default quest are only completable once
#define QUEST_FLAGS_GLOBALALLY_TRACKED    (1 << 1) // By default quests are only completable once

#define QUEST_TITLE_LENGTH 16
#define QUEST_DESC_LENGTH 84


#define  QUEST_REWARD_NONE          0
#define  QUEST_REWARD_SET_FLAG      1 // Params(1): flag
#define  QUEST_REWARD_CLEAR_FLAG    2 // Params(1): flag
#define  QUEST_REWARD_GIVE_ITEM     3 // Params(2): item, count
#define  QUEST_REWARD_GIVE_MONEY    4 // Params(2): amount


#define QUEST_NONE                  0
#define QUEST_Testing1              1
#define QUEST_Testing2              2
#define QUEST_Electric_Master       3
#define QUEST_Electric_Champion     4

#define QUEST_COUNT (QUEST_Electric_Champion + 1)