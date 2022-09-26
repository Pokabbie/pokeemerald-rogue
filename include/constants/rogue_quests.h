
#define QUEST_FLAGS_NONE                  0
#define QUEST_FLAGS_REPEATABLE            (1 << 0) // By default quest are only completable once
#define QUEST_FLAGS_GLOBALALLY_TRACKED    (1 << 1) // unused currently
#define QUEST_FLAGS_ACTIVE_IN_HUB         (1 << 2) // currently means ONLY active during hub phase

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
#define QUEST_MeetPokabbie          14

#define QUEST_Collector1            (QUEST_MeetPokabbie + 1)
#define QUEST_Collector2            (QUEST_MeetPokabbie + 2)
#define QUEST_CollectorLegend       (QUEST_MeetPokabbie + 3)
#define QUEST_ShoppingSpree         (QUEST_MeetPokabbie + 4)
#define QUEST_BigSaver              (QUEST_MeetPokabbie + 5)
#define QUEST_Bike1                 (QUEST_MeetPokabbie + 6)
#define QUEST_Bike2                 (QUEST_MeetPokabbie + 7)
#define QUEST_NoFainting1           (QUEST_MeetPokabbie + 8)
#define QUEST_NoFainting2           (QUEST_MeetPokabbie + 9)
#define QUEST_NoFainting3           (QUEST_MeetPokabbie + 10)
#define QUEST_BerryCollector        (QUEST_MeetPokabbie + 11)
#define QUEST_DenExplorer           (QUEST_MeetPokabbie + 12)
#define QUEST_MrRandoman            (QUEST_MeetPokabbie + 13)
#define QUEST_ChaosChampion         (QUEST_MeetPokabbie + 14)

#define QUEST_NORMAL_Champion       (QUEST_ChaosChampion + 1)
#define QUEST_FIGHTING_Champion     (QUEST_ChaosChampion + 2)
#define QUEST_FLYING_Champion       (QUEST_ChaosChampion + 3)
#define QUEST_POISON_Champion       (QUEST_ChaosChampion + 4)
#define QUEST_GROUND_Champion       (QUEST_ChaosChampion + 5)
#define QUEST_ROCK_Champion         (QUEST_ChaosChampion + 6)
#define QUEST_BUG_Champion          (QUEST_ChaosChampion + 7)
#define QUEST_GHOST_Champion        (QUEST_ChaosChampion + 8)
#define QUEST_STEEL_Champion        (QUEST_ChaosChampion + 9)
#define QUEST_FIRE_Champion         (QUEST_ChaosChampion + 10)
#define QUEST_WATER_Champion        (QUEST_ChaosChampion + 11)
#define QUEST_GRASS_Champion        (QUEST_ChaosChampion + 12)
#define QUEST_ELECTRIC_Champion     (QUEST_ChaosChampion + 13)
#define QUEST_PSYCHIC_Champion      (QUEST_ChaosChampion + 14)
#define QUEST_ICE_Champion          (QUEST_ChaosChampion + 15)
#define QUEST_DRAGON_Champion       (QUEST_ChaosChampion + 16)
#define QUEST_DARK_Champion         (QUEST_ChaosChampion + 17)
#ifdef ROGUE_EXPANSION
#define QUEST_FAIRY_Champion        (QUEST_ChaosChampion + 18)
#endif

#ifdef ROGUE_EXPANSION
#define QUEST_FIRST QUEST_FirstAdventure
#define QUEST_LAST QUEST_FAIRY_Champion
#else
#define QUEST_FIRST QUEST_FirstAdventure
#define QUEST_LAST QUEST_DARK_Champion
#endif

#define QUEST_CAPACITY (QUEST_LAST + 1)