
#include "generated/quest_consts.h"


// Constant flags
//
#define QUEST_CONST_NONE                    0
#define QUEST_CONST_UNLOCKED_BY_DEFAULT     (1 << 0)
#define QUEST_CONST_IS_MAIN_QUEST           (1 << 1)
#define QUEST_CONST_IS_CHALLENGE            (1 << 2)
#define QUEST_CONST_IS_MON_MASTERY          (1 << 3)
#define QUEST_CONST_ACTIVE_IN_RUN           (1 << 4)
#define QUEST_CONST_ACTIVE_IN_HUB           (1 << 5)

#define QUEST_CONST_ACTIVE_ALWAYS           (QUEST_CONST_ACTIVE_IN_RUN | QUEST_CONST_ACTIVE_IN_HUB)
#define QUEST_CONST_MAIN_QUEST_DEFAULT      (QUEST_CONST_IS_MAIN_QUEST)
#define QUEST_CONST_CHALLENGE_DEFAULT       (QUEST_CONST_IS_CHALLENGE)
#define QUEST_CONST_MON_MASTERY_DEFAULT     (QUEST_CONST_IS_MON_MASTERY)

// State flags
//
#define QUEST_STATE_NONE                    0
#define QUEST_STATE_UNLOCKED                (1 << 0)
#define QUEST_STATE_ACTIVE                  (1 << 1)
#define QUEST_STATE_PINNED                  (1 << 2)
#define QUEST_STATE_PENDING_REWARDS         (1 << 3)
#define QUEST_STATE_NEW_UNLOCK              (1 << 4)
#define QUEST_STATE_HAS_COMPLETE            (1 << 5)

// Triggers
//
#define QUEST_TRIGGER_NONE                  0
#define QUEST_TRIGGER_RUN_START             (1 << 0)
#define QUEST_TRIGGER_RUN_END               (1 << 1)
#define QUEST_TRIGGER_WILD_BATTLE_START     (1 << 2)
#define QUEST_TRIGGER_WILD_BATTLE_END       (1 << 4)
#define QUEST_TRIGGER_TRAINER_BATTLE_START  (1 << 5)
#define QUEST_TRIGGER_TRAINER_BATTLE_END    (1 << 6)
#define QUEST_TRIGGER_MISC_UPDATE           (1 << 7) // called at misc points in time e.g. on new quests unlocked, run end, save loaded
#define QUEST_TRIGGER_EARN_BADGE            (1 << 8)
#define QUEST_TRIGGER_ENTER_HALL_OF_FAME    (1 << 9)
#define QUEST_TRIGGER_MAP_SPECIFIC_EVENT    (1 << 10) // expected to be called from script so can check for specific map in quest conditions
#define QUEST_TRIGGER_POKEDEX_UPDATE        (1 << 11) 
#define QUEST_TRIGGER_ENTER_ENCOUNTER       (1 << 12) // on warp into encounter map
#define QUEST_TRIGGER_EXIT_ENCOUNTER        (1 << 13) // on warp out of encounter map
#define QUEST_TRIGGER_MON_FAINTED           (1 << 14)

#define QUEST_TRIGGER_ANY_BATTLE_START      (QUEST_TRIGGER_WILD_BATTLE_START | QUEST_TRIGGER_TRAINER_BATTLE_START)
#define QUEST_TRIGGER_ANY_BATTLE_END        (QUEST_TRIGGER_WILD_BATTLE_END | QUEST_TRIGGER_TRAINER_BATTLE_END)

// States
//
#define QUEST_STATUS_PENDING                0
#define QUEST_STATUS_SUCCESS                1
#define QUEST_STATUS_FAIL                   2
#define QUEST_STATUS_BREAK                  3 // skip any remaining triggers queued for this quest

// Values
//
#define QUEST_REWARD_SMALL_MONEY            1000
#define QUEST_REWARD_MEDIUM_MONEY           2000
#define QUEST_REWARD_LARGE_MONEY            5000

#define QUEST_REWARD_SMALL_BUILD_AMOUNT     1
#define QUEST_REWARD_MEDIUM_BUILD_AMOUNT    2
#define QUEST_REWARD_LARGE_BUILD_AMOUNT     5