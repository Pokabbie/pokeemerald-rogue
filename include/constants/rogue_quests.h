
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
#define  QUEST_REWARD_CUSTOM_TEXT   5 // Params(0)


#define QUEST_NONE                  0
#define QUEST_FirstAdventure        1
#define QUEST_GymChallenge          2
#define QUEST_GymMaster             3
#define QUEST_EliteMaster           4
#define QUEST_Champion              5

#define QUEST_Electric_Master       (QUEST_Champion + 1)
#define QUEST_Electric_Champion     (QUEST_Champion + 2)

#define QUEST_FIRST QUEST_FirstAdventure
#define QUEST_COUNT (QUEST_Electric_Champion + 1)