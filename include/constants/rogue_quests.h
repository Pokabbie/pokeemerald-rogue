
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

#define QUEST_TRIGGER_ANY_BATTLE_START      (QUEST_TRIGGER_WILD_BATTLE_START | QUEST_TRIGGER_TRAINER_BATTLE_START)

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


// old
#define QUEST_FLAGS_NONE                  0
#define QUEST_FLAGS_REPEATABLE            (1 << 0) // By default quest are only completable once
#define QUEST_FLAGS_GLOBALALLY_TRACKED    (1 << 1) // unused currently
#define QUEST_FLAGS_ACTIVE_IN_HUB         (1 << 2) // currently means ONLY active during hub phase

#define QUEST_TITLE_LENGTH 16
#define QUEST_DESC_LENGTH 128

#define  QUEST_MAX_REWARD_COUNT     4
#define  QUEST_MAX_FOLLOWING_QUESTS 24
#define  QUEST_MAX_ITEM_SHOP_REWARD_COUNT 6

#define  QUEST_REWARD_NONE          0
#define  QUEST_REWARD_SET_FLAG      1 // Params(1): flag
#define  QUEST_REWARD_CLEAR_FLAG    2 // Params(1): flag
#define  QUEST_REWARD_GIVE_ITEM     3 // Params(2): item, count
#define  QUEST_REWARD_GIVE_MONEY    4 // Params(2): amount
#define  QUEST_REWARD_CUSTOM_TEXT   5 // Params(0)
#define  QUEST_REWARD_GIVE_POKEMON  6 // Params(3): species, lvl, forceShiny


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
#define QUEST_WobFate               (QUEST_MeetPokabbie + 15)
#define QUEST_Hardcore              (QUEST_MeetPokabbie + 16)
#define QUEST_Hardcore2             (QUEST_MeetPokabbie + 17)
#define QUEST_Hardcore3             (QUEST_MeetPokabbie + 18)
#define QUEST_KantoMode             (QUEST_MeetPokabbie + 19)
#define QUEST_GauntletMode          (QUEST_MeetPokabbie + 20)

#ifdef ROGUE_EXPANSION
#define QUEST_MegaEvo               (QUEST_GauntletMode + 1)
#define QUEST_ZMove                 (QUEST_GauntletMode + 2)

#define QUEST_ShayminItem           (QUEST_GauntletMode + 3)
#define QUEST_HoopaItem             (QUEST_GauntletMode + 4)
#define QUEST_NatureItem            (QUEST_GauntletMode + 5)

#define BASIC_QUEST_BLOCK_LAST QUEST_NatureItem
#else
#define BASIC_QUEST_BLOCK_LAST QUEST_GauntletMode
#endif

#define QUEST_NORMAL_Champion       (BASIC_QUEST_BLOCK_LAST + 1)
#define QUEST_FIGHTING_Champion     (BASIC_QUEST_BLOCK_LAST + 2)
#define QUEST_FLYING_Champion       (BASIC_QUEST_BLOCK_LAST + 3)
#define QUEST_POISON_Champion       (BASIC_QUEST_BLOCK_LAST + 4)
#define QUEST_GROUND_Champion       (BASIC_QUEST_BLOCK_LAST + 5)
#define QUEST_ROCK_Champion         (BASIC_QUEST_BLOCK_LAST + 6)
#define QUEST_BUG_Champion          (BASIC_QUEST_BLOCK_LAST + 7)
#define QUEST_GHOST_Champion        (BASIC_QUEST_BLOCK_LAST + 8)
#define QUEST_STEEL_Champion        (BASIC_QUEST_BLOCK_LAST + 9)
#define QUEST_FIRE_Champion         (BASIC_QUEST_BLOCK_LAST + 10)
#define QUEST_WATER_Champion        (BASIC_QUEST_BLOCK_LAST + 11)
#define QUEST_GRASS_Champion        (BASIC_QUEST_BLOCK_LAST + 12)
#define QUEST_ELECTRIC_Champion     (BASIC_QUEST_BLOCK_LAST + 13)
#define QUEST_PSYCHIC_Champion      (BASIC_QUEST_BLOCK_LAST + 14)
#define QUEST_ICE_Champion          (BASIC_QUEST_BLOCK_LAST + 15)
#define QUEST_DRAGON_Champion       (BASIC_QUEST_BLOCK_LAST + 16)
#define QUEST_DARK_Champion         (BASIC_QUEST_BLOCK_LAST + 17)
#ifdef ROGUE_EXPANSION
#define QUEST_FAIRY_Champion        (BASIC_QUEST_BLOCK_LAST + 18)
#endif

#ifdef ROGUE_EXPANSION
#define _QUEST_LAST_1_2             QUEST_FAIRY_Champion
#else
#define _QUEST_LAST_1_2             QUEST_DARK_Champion
#endif

// 1.3 Quests
// 

#define QUEST_JohtoMode             (_QUEST_LAST_1_2 + 1)
#define QUEST_HoennMode             (_QUEST_LAST_1_2 + 2)
#define QUEST_GlitchMode            (_QUEST_LAST_1_2 + 3)
#ifdef ROGUE_EXPANSION
#define QUEST_SinnohMode            (_QUEST_LAST_1_2 + 4)
#define QUEST_UnovaMode             (_QUEST_LAST_1_2 + 5)
#define QUEST_KalosMode             (_QUEST_LAST_1_2 + 6)
#define QUEST_AlolaMode             (_QUEST_LAST_1_2 + 7)
#define QUEST_GalarMode             (_QUEST_LAST_1_2 + 8)
#endif

#ifdef ROGUE_EXPANSION
#define QUEST_RegionMode_Last       QUEST_GalarMode
#else
#define QUEST_RegionMode_Last       QUEST_GlitchMode
#endif

#define QUEST_OrreMode              (QUEST_RegionMode_Last + 1)
#define QUEST_DevilDeal             (QUEST_RegionMode_Last + 2)
#define QUEST_LegendOnly            (QUEST_RegionMode_Last + 3)
#define QUEST_CursedBody            (QUEST_RegionMode_Last + 4)
#define QUEST_Nuzlocke              (QUEST_RegionMode_Last + 5)
#define QUEST_IronMono1             (QUEST_RegionMode_Last + 6)
#define QUEST_IronMono2             (QUEST_RegionMode_Last + 7)
#define QUEST_Hardcore4             (QUEST_RegionMode_Last + 8)
#define QUEST_CampaignTease         (QUEST_RegionMode_Last + 9)

#ifdef ROGUE_EXPANSION
#define QUEST_DeoxysItem            (QUEST_RegionMode_Last + 10)
#define _QUEST_LAST_1_3 QUEST_DeoxysItem
#else
#define _QUEST_LAST_1_3 QUEST_CampaignTease
#endif

#define QUEST_ShinyOnly             (_QUEST_LAST_1_3 + 1)
#define _QUEST_LAST_1_3_2 QUEST_ShinyOnly

#define QUEST_FIRST QUEST_FirstAdventure
#define QUEST_LAST _QUEST_LAST_1_3_2

#define QUEST_CAPACITY (QUEST_LAST + 1)