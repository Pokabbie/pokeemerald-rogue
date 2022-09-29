#define ROGUE_DEBUG
//#define ROGUE_DEBUG_PAUSE_PANEL
#define ROGUE_DEBUG_STEAL_TEAM

#define ROGUE_EXPANSION

#define ROGUE_FEATURE_ENCOUNTER_PREVIEW
//#define ROGUE_FEATURE_SKIP_SAVE_WARNINGS // Activate this if you intend on putting on a physical cart with 64k FLASH save

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define ROGUE_ROUTE_COUNT 12
#define ROGUE_HUB_BERRY_TREE_COUNT 20

#define ROGUE_MAX_ADVPATH_ROWS 7
#define ROGUE_MAX_ADVPATH_COLUMNS 7


// Items
//
#define ROGUE_SHOP_NONE                 0
#define ROGUE_SHOP_MEDICINE             1
#define ROGUE_SHOP_BALLS                2
#define ROGUE_SHOP_TMS                  3
#define ROGUE_SHOP_BATTLE_ENHANCERS     4
#define ROGUE_SHOP_HELD_ITEMS           5
#define ROGUE_SHOP_RARE_HELD_ITEMS      6
#define ROGUE_SHOP_BERRIES              7


#define ITEM_LINK_CABLE ITEM_EXP_SHARE

#ifdef ROGUE_EXPANSION
#define ITEM_QUEST_LOG ITEM_KEY_TO_ROOM_1
#else
#define ITEM_QUEST_LOG ITEM_ROOM_1_KEY
#endif

// Trainers
//
// RogueNote: Overwrite from these trainer ranges
#define TRAINER_FLAG_NONE                   0
#define TRAINER_FLAG_GYM                    (1 << 0)
#define TRAINER_FLAG_ELITE                  (1 << 1)
#define TRAINER_FLAG_PRE_CHAMP              (1 << 2)
#define TRAINER_FLAG_FINAL_CHAMP            (1 << 3)
#define TRAINER_FLAG_HOENN                  (1 << 4)
#define TRAINER_FLAG_THIRDSLOT_ACE_TYPE     (1 << 5)

#define TRAINER_ROGUE_BREEDER_F             (TRAINER_JULIE + 0)
#define TRAINER_ROGUE_BREEDER_M             (TRAINER_JULIE + 1)
#define TRAINER_ROGUE_RICH_F                (TRAINER_JULIE + 2)
#define TRAINER_ROGUE_RICH_M                (TRAINER_JULIE + 4)
#define TRAINER_ROGUE_COOLTRAINER_F         (TRAINER_JULIE + 5)
#define TRAINER_ROGUE_COOLTRAINER_M         (TRAINER_JULIE + 6)
#define TRAINER_ROGUE_POKEFAN_F             (TRAINER_JULIE + 7)
#define TRAINER_ROGUE_POKEFAN_M             (TRAINER_JULIE + 8)
#define TRAINER_ROGUE_SCHOOL_KID_F          (TRAINER_JULIE + 9)
#define TRAINER_ROGUE_SCHOOL_KID_M          (TRAINER_JULIE + 10)
#define TRAINER_ROGUE_TUBER_F               (TRAINER_JULIE + 11)
#define TRAINER_ROGUE_TUBER_M               (TRAINER_JULIE + 12)
#define TRAINER_ROGUE_POSH_F                (TRAINER_JULIE + 13)
#define TRAINER_ROGUE_POSH_M                (TRAINER_JULIE + 14)
#define TRAINER_ROGUE_MAGMA_F               (TRAINER_JULIE + 15)
#define TRAINER_ROGUE_MAGMA_M               (TRAINER_JULIE + 16)
#define TRAINER_ROGUE_AQUA_F                (TRAINER_JULIE + 17)
#define TRAINER_ROGUE_AQUA_M                (TRAINER_JULIE + 18)

#define TRAINER_ROGUE_BOSS_ROXANNE           TRAINER_ROXANNE_1
#define TRAINER_ROGUE_BOSS_BRAWLY            TRAINER_BRAWLY_1
#define TRAINER_ROGUE_BOSS_WATTSON           TRAINER_WATTSON_1
#define TRAINER_ROGUE_BOSS_FLANNERY          TRAINER_FLANNERY_1
#define TRAINER_ROGUE_BOSS_NORMAN            TRAINER_NORMAN_1
#define TRAINER_ROGUE_BOSS_WINONA            TRAINER_WINONA_1
#define TRAINER_ROGUE_BOSS_TATE_AND_LIZA     TRAINER_TATE_AND_LIZA_1
#define TRAINER_ROGUE_BOSS_JUAN              TRAINER_JUAN_1

#define TRAINER_ROGUE_BOSS_SIDNEY            TRAINER_SIDNEY
#define TRAINER_ROGUE_BOSS_PHOEBE            TRAINER_PHOEBE
#define TRAINER_ROGUE_BOSS_GLACIA            TRAINER_GLACIA
#define TRAINER_ROGUE_BOSS_DRAKE             TRAINER_DRAKE

#define TRAINER_ROGUE_BOSS_WALLACE           TRAINER_WALLACE
#define TRAINER_ROGUE_BOSS_STEVEN            TRAINER_STEVEN

#define TRAINER_ROGUE_BOSS_LUCY              TRAINER_LUCY
#define TRAINER_ROGUE_BOSS_BRANDON           TRAINER_BRANDON
#define TRAINER_ROGUE_BOSS_TUCKER            TRAINER_TUCKER
#define TRAINER_ROGUE_BOSS_SPENSER           TRAINER_SPENSER
#define TRAINER_ROGUE_BOSS_ANABEL            TRAINER_ANABEL

#define TRAINER_ROGUE_MINI_BOSS_MAXIE        TRAINER_MAXIE_MAGMA_HIDEOUT
#define TRAINER_ROGUE_MINI_BOSS_ARCHIE       TRAINER_ARCHIE
#define TRAINER_ROGUE_MINI_BOSS_WALLY        TRAINER_WALLY_VR_1
#define TRAINER_ROGUE_MINI_BOSS_MIRROR       TRAINER_BRENDAN_PLACEHOLDER

// Mon Preset flags
//
#define MON_FLAG_NONE                   0
#define MON_FLAG_STRONG                 (1 << 0)
#define MON_FLAG_DOUBLES                (1 << 1)

// These's are the category names fed in by the Showdown presets
#ifdef ROGUE_EXPANSION
#define MON_FLAGS_GEN7UBERS                 MON_FLAG_STRONG
#define MON_FLAGS_GEN7OU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN7UU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN7RU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN7NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN7DOUBLESOU             MON_FLAG_STRONG | MON_FLAG_DOUBLES
#define MON_FLAGS_GEN7BATTLESPOTSINGLES     MON_FLAG_NONE
#define MON_FLAGS_GEN7BATTLESPOTDOUBLES     MON_FLAG_DOUBLES
#define MON_FLAGS_GEN7VGC2017               MON_FLAG_STRONG
#define MON_FLAGS_GEN7VGC2018               MON_FLAG_STRONG
#define MON_FLAGS_GEN71V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN7ANYTHINGGOES          MON_FLAG_NONE
#define MON_FLAGS_GEN7LETSGOOU              MON_FLAG_NONE
#define MON_FLAGS_GEN7MONOTYPE              MON_FLAG_STRONG

#define MON_FLAGS_GEN8UBERS                 MON_FLAG_STRONG
#define MON_FLAGS_GEN8OU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN8UU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN8RU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN8NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN8NATIONALDEX           MON_FLAG_NONE
#define MON_FLAGS_GEN8DOUBLESOU             MON_FLAG_STRONG | MON_FLAG_DOUBLES
#define MON_FLAGS_GEN8BATTLESTADIUMSINGLES  MON_FLAG_NONE
#define MON_FLAGS_GEN8VGC2020               MON_FLAG_STRONG
#define MON_FLAGS_GEN81V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN8ANYTHINGGOES          MON_FLAG_NONE
#define MON_FLAGS_GEN8NATIONALDEXAG         MON_FLAG_NONE
#define MON_FLAGS_GEN8MONOTYPE              MON_FLAG_STRONG

#else

#define MON_FLAGS_GEN3UBERS                 MON_FLAG_STRONG
#define MON_FLAGS_GEN3OU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN3UU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN3NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3DOUBLESOU             MON_FLAG_STRONG | MON_FLAG_DOUBLES
#define MON_FLAGS_GEN31V1                   MON_FLAG_STRONG
#endif


#include "rogue_quests.h"