//#define ROGUE_DEBUG
//#define ROGUE_DEBUG_PAUSE_PANEL
//#define ROGUE_DEBUG_STEAL_TEAM

#define ROGUE_EXPANSION

#define ROGUE_FEATURE_ENCOUNTER_PREVIEW
//#define ROGUE_FEATURE_SKIP_SAVE_WARNINGS // Activate this if you intend on putting on a physical cart with 64k FLASH save

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define ROGUE_HUB_BERRY_TREE_COUNT 20

#define ROGUE_MAX_ADVPATH_ROWS 7
#define ROGUE_MAX_ADVPATH_COLUMNS 7


// Items
//
#define ROGUE_SHOP_NONE                 0
#define ROGUE_SHOP_GENERAL              1
#define ROGUE_SHOP_BALLS                2
#define ROGUE_SHOP_TMS                  3
#define ROGUE_SHOP_BATTLE_ENHANCERS     4
#define ROGUE_SHOP_HELD_ITEMS           5
#define ROGUE_SHOP_RARE_HELD_ITEMS      6
#define ROGUE_SHOP_BERRIES              7
#define ROGUE_SHOP_CHARMS               8

// Old replacement-style items
#define ITEM_LINK_CABLE ITEM_EXP_SHARE

#ifdef ROGUE_EXPANSION
#define ITEM_QUEST_LOG ITEM_KEY_TO_ROOM_1
#else
#define ITEM_QUEST_LOG ITEM_ROOM_1_KEY
#endif


#define ITEM_SHOP_PRICE_CHARM           ITEM_ROGUE_ITEM0
#define ITEM_FLINCH_CHARM               ITEM_ROGUE_ITEM1
#define ITEM_CRIT_CHARM                 ITEM_ROGUE_ITEM2
#define ITEM_SHED_SKIN_CHARM            ITEM_ROGUE_ITEM3
#define ITEM_WILD_IV_CHARM              ITEM_ROGUE_ITEM4
#define ITEM_CATCHING_CHARM             ITEM_ROGUE_ITEM5
#define ITEM_GRACE_CHARM                ITEM_ROGUE_ITEM6

#define ITEM_SHOP_PRICE_CURSE           ITEM_ROGUE_ITEM10
#define ITEM_FLINCH_CURSE               ITEM_ROGUE_ITEM11
#define ITEM_CRIT_CURSE                 ITEM_ROGUE_ITEM12
#define ITEM_SHED_SKIN_CURSE            ITEM_ROGUE_ITEM13
#define ITEM_WILD_IV_CURSE              ITEM_ROGUE_ITEM14
#define ITEM_CATCHING_CURSE             ITEM_ROGUE_ITEM15
#define ITEM_GRACE_CURSE                ITEM_ROGUE_ITEM16

#define ITEM_CHARM_CURSE_OFFSET (ITEM_SHOP_PRICE_CURSE - ITEM_SHOP_PRICE_CHARM)

#define FIRST_ITEM_CHARM        ITEM_SHOP_PRICE_CHARM
#define LAST_ITEM_CHARM         ITEM_GRACE_CHARM

#define FIRST_ITEM_CURSE        ITEM_SHOP_PRICE_CURSE
#define LAST_ITEM_CURSE         ITEM_GRACE_CURSE

// Routes
//
#define ROUTE_FLAG_NONE                   0
#define ROUTE_FLAG_HOENN                  (1 << 1)
#define ROUTE_FLAG_KANTO                  (1 << 2)
#define ROUTE_FLAG_JOHTO                  (1 << 3)
#define ROUTE_FLAG_FALLBACK_REGION        (1 << 4) // If the player unselects all route expansions, use this custom fallback set

// Trainers
//
#define TRAINER_FLAG_NONE                   0
#define TRAINER_FLAG_GYM                    (1 << 0)
#define TRAINER_FLAG_ELITE                  (1 << 1)
#define TRAINER_FLAG_PRE_CHAMP              (1 << 2)
#define TRAINER_FLAG_FINAL_CHAMP            (1 << 3)
#define TRAINER_FLAG_HOENN                  (1 << 4)
#define TRAINER_FLAG_KANTO                  (1 << 5)
#define TRAINER_FLAG_JOHTO                  (1 << 6)
#define TRAINER_FLAG_FALLBACK_REGION        (1 << 7) // If the player unselects all boss expansions, use this custom fallback set
#define TRAINER_FLAG_RAINBOW_EXCLUDE        (1 << 8)
#define TRAINER_FLAG_DISABLE_WEATHER        (1 << 9)
#define TRAINER_FLAG_THIRDSLOT_WEATHER      (1 << 10)

#define PARTY_FLAG_NONE                     0
#define PARTY_FLAG_THIRDSLOT_ACE_TYPE       (1 << 1)
#define PARTY_FLAG_THIRDSLOT_FALLBACK_TYPE  (1 << 2)
#define PARTY_FLAG_CUSTOM_INPUT_QUERY       (1 << 3) // Provides list of baby mons to feed into the generator
#define PARTY_FLAG_CUSTOM_FINAL_QUERY       (1 << 4) // Provideds the final list of mons to pick from
#define PARTY_FLAG_REGION_DEX_DISABLE_QUERY (1 << 5) // Custom query will be ignored when a regional dex is active
#define PARTY_FLAG_STRONG_PRESETS_IGNORE    (1 << 6)
#define PARTY_FLAG_MIRROR_EXACT             (1 << 7)
#define PARTY_FLAG_MIRROR_SPECIES           (1 << 8)
#define PARTY_FLAG_COUNTER_TYPINGS          (1 << 9)
#define PARTY_FLAG_UNIQUE_COVERAGE          (1 << 10)
#define PARTY_FLAG_MIRROR_ANY               (PARTY_FLAG_MIRROR_EXACT | PARTY_FLAG_MIRROR_SPECIES)
#define PARTY_FLAG_CUSTOM_QUERY_ANY         (PARTY_FLAG_CUSTOM_INPUT_QUERY | PARTY_FLAG_CUSTOM_FINAL_QUERY)

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

// Bosses
//
// Hoenn
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

// Extra/Brains
#define TRAINER_ROGUE_BOSS_ANABEL            TRAINER_WALLY_VR_2

// Kanto
#define TRAINER_ROGUE_BOSS_BROCK            TRAINER_ROXANNE_2
#define TRAINER_ROGUE_BOSS_MISTY            TRAINER_BRAWLY_2
#define TRAINER_ROGUE_BOSS_LTSURGE          TRAINER_WATTSON_2
#define TRAINER_ROGUE_BOSS_ERIKA            TRAINER_FLANNERY_2
#define TRAINER_ROGUE_BOSS_KOGA             TRAINER_NORMAN_2
#define TRAINER_ROGUE_BOSS_SABRINA          TRAINER_WINONA_2
#define TRAINER_ROGUE_BOSS_BLAINE           TRAINER_TATE_AND_LIZA_2
#define TRAINER_ROGUE_BOSS_GIOVANNI         TRAINER_JUAN_2

#define TRAINER_ROGUE_BOSS_LORELEI          TRAINER_ANDRES_1
#define TRAINER_ROGUE_BOSS_BRUNO            TRAINER_CORY_1
#define TRAINER_ROGUE_BOSS_AGATHA           TRAINER_PABLO_1
#define TRAINER_ROGUE_BOSS_LANCE            TRAINER_KOJI_1

#define TRAINER_ROGUE_BOSS_BLUE             TRAINER_CRISTIN_1
#define TRAINER_ROGUE_BOSS_PROFOAK          TRAINER_FERNANDO_1

// Johto
#define TRAINER_ROGUE_BOSS_FALKNER          TRAINER_ROXANNE_3
#define TRAINER_ROGUE_BOSS_BUGSY            TRAINER_BRAWLY_3
#define TRAINER_ROGUE_BOSS_WHITNEY          TRAINER_WATTSON_3
#define TRAINER_ROGUE_BOSS_MORTY            TRAINER_FLANNERY_3
#define TRAINER_ROGUE_BOSS_CHUCK            TRAINER_NORMAN_3
#define TRAINER_ROGUE_BOSS_JASMINE          TRAINER_WINONA_3
#define TRAINER_ROGUE_BOSS_PRYCE            TRAINER_TATE_AND_LIZA_3
#define TRAINER_ROGUE_BOSS_CLAIR            TRAINER_JUAN_3

#define TRAINER_ROGUE_BOSS_WILL             TRAINER_ANDRES_2
#define TRAINER_ROGUE_BOSS_JOHTO_KOGA       TRAINER_CORY_2
#define TRAINER_ROGUE_BOSS_JOHTO_BRUNO      TRAINER_PABLO_2
#define TRAINER_ROGUE_BOSS_KAREN            TRAINER_KOJI_2

#define TRAINER_ROGUE_BOSS_JOHTO_LANCE      TRAINER_CRISTIN_2
#define TRAINER_ROGUE_BOSS_RED              TRAINER_FERNANDO_2

// Glitch
#define TRAINER_ROGUE_BOSS_KATE             TRAINER_ANDRES_3
#define TRAINER_ROGUE_BOSS_TAILS            TRAINER_CORY_3
#define TRAINER_ROGUE_BOSS_RAVEN            TRAINER_PABLO_3
#define TRAINER_ROGUE_BOSS_ERMA             TRAINER_KOJI_3

#define TRAINER_ROGUE_BOSS_MIRROR           TRAINER_CRISTIN_3
#define TRAINER_ROGUE_BOSS_POKABBIE         TRAINER_FERNANDO_3

// Minibosses
//
#define TRAINER_ROGUE_MINI_BOSS_MAXIE        TRAINER_MAXIE_MAGMA_HIDEOUT
#define TRAINER_ROGUE_MINI_BOSS_ARCHIE       TRAINER_ARCHIE
#define TRAINER_ROGUE_MINI_BOSS_WALLY        TRAINER_WALLY_VR_1
#define TRAINER_ROGUE_MINI_BOSS_MIRROR       TRAINER_BRENDAN_PLACEHOLDER
#define TRAINER_ROGUE_MINI_BOSS_RIVAL        TRAINER_MAY_PLACEHOLDER

#define TRAINER_ROGUE_MINI_BOSS_LUCY         TRAINER_LUCY
#define TRAINER_ROGUE_MINI_BOSS_BRANDON      TRAINER_BRANDON
#define TRAINER_ROGUE_MINI_BOSS_TUCKER       TRAINER_TUCKER
#define TRAINER_ROGUE_MINI_BOSS_SPENSER      TRAINER_SPENSER
#define TRAINER_ROGUE_MINI_BOSS_GRETA        TRAINER_GRETA
#define TRAINER_ROGUE_MINI_BOSS_NOLAND       TRAINER_NOLAND
#define TRAINER_ROGUE_MINI_BOSS_ANABEL       TRAINER_ANABEL

// Mon Preset flags
//
#define MON_FLAG_NONE                   0
#define MON_FLAG_STRONG                 (1 << 0) // Used to filter for E4/Champ fights
#define MON_FLAG_DOUBLES                (1 << 1)
#define MON_FLAG_STRONG_WILD            (1 << 2) // Used to filter for early legendary encounters

// These's are the category names fed in by the Showdown presets
#ifdef ROGUE_EXPANSION
#define MON_FLAGS_GEN7UBERS                 MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7OU                    MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7UU                    MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7RU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN7DOUBLESOU             MON_FLAG_DOUBLES | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7BATTLESPOTSINGLES     MON_FLAG_NONE
#define MON_FLAGS_GEN7BATTLESPOTDOUBLES     MON_FLAG_DOUBLES
#define MON_FLAGS_GEN7VGC2017               MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7VGC2018               MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN71V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN7ANYTHINGGOES          MON_FLAG_NONE
#define MON_FLAGS_GEN7LETSGOOU              MON_FLAG_NONE
#define MON_FLAGS_GEN7MONOTYPE              MON_FLAG_STRONG | MON_FLAG_STRONG_WILD

#define MON_FLAGS_GEN8UBERS                 MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8OU                    MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8UU                    MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8RU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN8NATIONALDEX           MON_FLAG_NONE
#define MON_FLAGS_GEN8DOUBLESOU             MON_FLAG_DOUBLES | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8BATTLESTADIUMSINGLES  MON_FLAG_NONE
#define MON_FLAGS_GEN8VGC2020               MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN81V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN8ANYTHINGGOES          MON_FLAG_NONE
#define MON_FLAGS_GEN8NATIONALDEXAG         MON_FLAG_NONE
#define MON_FLAGS_GEN8MONOTYPE              MON_FLAG_STRONG | MON_FLAG_STRONG_WILD

#else

#define MON_FLAGS_GEN3UBERS                 MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN3OU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN3UU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3DOUBLESOU             MON_FLAG_DOUBLES
#define MON_FLAGS_GEN31V1                   MON_FLAG_STRONG
#endif

// Character customisation
// This works for everything but the actual input strings, so make sure to increase those to match
//
#define FOREACH_VISUAL_PRESETS(callback) \
    callback(0, 0) \
    callback(0, 1) \
    callback(0, 2) \
    callback(0, 3) \
    callback(0, 4) \
    callback(0, 5) \
    callback(1, 0) \
    callback(1, 1) \
    callback(1, 2) \
    callback(1, 3) \
    callback(1, 4) \
    callback(1, 5) \
    callback(2, 0) \
    callback(2, 1) \
    callback(2, 2) \
    callback(2, 3) \
    callback(2, 4) \
    callback(2, 5) \
    callback(3, 0) \
    callback(3, 1) \
    callback(3, 2) \
    callback(3, 3) \
    callback(3, 4) \
    callback(3, 5)

#include "rogue_quests.h"