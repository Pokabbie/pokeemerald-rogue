#ifndef ROGUE_CONFIG_H
#define ROGUE_CONFIG_H

//#define ROGUE_EXPANSION

#define ROGUE_VERSION_VANILLA       0
#define ROGUE_VERSION_EXPANSION     1

#ifdef ROGUE_EXPANSION
#define ROGUE_VERSION ROGUE_VERSION_EXPANSION
#else
#define ROGUE_VERSION ROGUE_VERSION_VANILLA
#endif

//#define ROGUE_FEATURE_AUTOMATION // Activate this for builds where automated external interactions are enabled (e.g. Soak Tests)
//#define ROGUE_FEATURE_SKIP_SAVE_WARNINGS // Activate this if you intend on putting on a physical cart with 64k FLASH save


#ifndef ROGUE_FEATURE_AUTOMATION
// Debugging defines
// Override these when debugging
//
#define ROGUE_DEBUG
//#define ROGUE_DEBUG_PAUSE_PANEL
#define ROGUE_DEBUG_STEAL_TEAM
#define ROGUE_DEBUG_LVL_5_TRAINERS
#define ROGUE_DEBUG_LOGGING
//#define ROGUE_DEBUG_ALLOW_SAVE_SCUMMING
//#define ROGUE_DEBUG_INSTANT_CATCH
#else

// Automation defines
// Don't adjust these unless needed
//
#define ROGUE_DEBUG
#define ROGUE_DEBUG_PAUSE_PANEL
//#define ROGUE_DEBUG_STEAL_TEAM
//#define ROGUE_DEBUG_LVL_5_TRAINERS
#define ROGUE_DEBUG_LOGGING
#define ROGUE_DEBUG_INSTANT_CATCH

#endif

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define ROGUE_HUB_BERRY_TREE_COUNT 20


// Items
//
#define ROGUE_SHOP_NONE                 0
#define ROGUE_SHOP_GENERAL              1
#define ROGUE_SHOP_BALLS                2
#define ROGUE_SHOP_TMS                  3
#define ROGUE_SHOP_BATTLE_ENHANCERS     4
#define ROGUE_SHOP_HELD_ITEMS           5
#define ROGUE_SHOP_RARE_HELD_ITEMS      6
#define ROGUE_SHOP_QUEST_REWARDS        8
#define ROGUE_SHOP_BERRIES              9
#define ROGUE_SHOP_CHARMS               10
#define ROGUE_SHOP_HUB_UPGRADES         11

#define ITEM_LINK_CABLE                 (ITEM_ROGUE_ITEM_FIRST + 0)
#define ITEM_QUEST_LOG                  (ITEM_ROGUE_ITEM_FIRST + 1)
#define ITEM_HEALING_FLASK              (ITEM_ROGUE_ITEM_FIRST + 2)
#define ITEM_BASIC_RIDING_WHISTLE       (ITEM_ROGUE_ITEM_FIRST + 3)
#define ITEM_GOLD_RIDING_WHISTLE        (ITEM_ROGUE_ITEM_FIRST + 4)

// TODO - Classify these as dynamic items, to be wiped every patch (Allows easily moving them around)
// Reserved 30 charms then 30 items
#define FIRST_ITEM_CHARM                (ITEM_ROGUE_ITEM_FIRST + 50)

#define ITEM_SHOP_PRICE_CHARM           (FIRST_ITEM_CHARM + 0)
#define ITEM_FLINCH_CHARM               (FIRST_ITEM_CHARM + 1)
#define ITEM_CRIT_CHARM                 (FIRST_ITEM_CHARM + 2)
#define ITEM_SHED_SKIN_CHARM            (FIRST_ITEM_CHARM + 3)
#define ITEM_WILD_IV_CHARM              (FIRST_ITEM_CHARM + 4)
#define ITEM_CATCHING_CHARM             (FIRST_ITEM_CHARM + 5)
#define ITEM_GRACE_CHARM                (FIRST_ITEM_CHARM + 6)
#define ITEM_WILD_ENCOUNTER_CHARM       (FIRST_ITEM_CHARM + 7)
#define ITEM_MOVE_PRIORITY_CHARM        (FIRST_ITEM_CHARM + 8)
#define ITEM_ENDURE_CHARM               (FIRST_ITEM_CHARM + 9)

#define LAST_ITEM_CHARM                 (ITEM_ENDURE_CHARM)


#define FIRST_ITEM_CURSE                (ITEM_ROGUE_ITEM_FIRST + 80)

#define ITEM_SHOP_PRICE_CURSE           (FIRST_ITEM_CURSE + 0)
#define ITEM_FLINCH_CURSE               (FIRST_ITEM_CURSE + 1)
#define ITEM_CRIT_CURSE                 (FIRST_ITEM_CURSE + 2)
#define ITEM_SHED_SKIN_CURSE            (FIRST_ITEM_CURSE + 3)
#define ITEM_WILD_IV_CURSE              (FIRST_ITEM_CURSE + 4)
#define ITEM_CATCHING_CURSE             (FIRST_ITEM_CURSE + 5)
#define ITEM_GRACE_CURSE                (FIRST_ITEM_CURSE + 6)
#define ITEM_WILD_ENCOUNTER_CURSE       (FIRST_ITEM_CURSE + 7)
#define ITEM_PARTY_CURSE                (FIRST_ITEM_CURSE + 8)
#define ITEM_EVERSTONE_CURSE            (FIRST_ITEM_CURSE + 9)
#define ITEM_BATTLE_ITEM_CURSE          (FIRST_ITEM_CURSE + 10)
#define ITEM_SPECIES_CLAUSE_CURSE       (FIRST_ITEM_CURSE + 11)
#define ITEM_ITEM_SHUFFLE_CURSE         (FIRST_ITEM_CURSE + 12)
#define ITEM_MOVE_PRIORITY_CURSE        (FIRST_ITEM_CURSE + 13)
#define ITEM_ENDURE_CURSE               (FIRST_ITEM_CURSE + 14)

#define LAST_ITEM_CURSE                 (ITEM_ENDURE_CURSE)


// Routes
//
#define ROUTE_FLAG_NONE                   0
#define ROUTE_FLAG_HOENN                  (1 << 1)
#define ROUTE_FLAG_KANTO                  (1 << 2)
#define ROUTE_FLAG_JOHTO                  (1 << 3)
#define ROUTE_FLAG_FALLBACK_REGION        (1 << 4) // If the player unselects all route expansions, use this custom fallback set

// Adventure
//
#define ROGUE_ADVPATH_ROOM_CAPACITY 32

#define ROGUE_ADV_STANDARD      0
#define ROGUE_ADV_RAINBOW       1
#define ROGUE_ADV_GAUNTLET      2

// AdvPath
//
#define ADVPATH_ROOM_NONE       0
#define ADVPATH_ROOM_ROUTE      1
#define ADVPATH_ROOM_RESTSTOP   2
#define ADVPATH_ROOM_LEGENDARY  3
#define ADVPATH_ROOM_MINIBOSS   4
#define ADVPATH_ROOM_WILD_DEN   5
#define ADVPATH_ROOM_GAMESHOW   6
#define ADVPATH_ROOM_DARK_DEAL  7
#define ADVPATH_ROOM_LAB        8

#define ADVPATH_ROOM_BOSS       9

#define ADVPATH_ROOM_COUNT      10


#define ADVPATH_SUBROOM_ROUTE_CALM          0
#define ADVPATH_SUBROOM_ROUTE_AVERAGE       1
#define ADVPATH_SUBROOM_ROUTE_TOUGH         2

#define ADVPATH_SUBROOM_RESTSTOP_BATTLE     0
#define ADVPATH_SUBROOM_RESTSTOP_SHOP       1
#define ADVPATH_SUBROOM_RESTSTOP_FULL       2


#define ADVPATH_ROOM_WEIGHT_COUNT       (ADVPATH_ROOM_LAB + 1) // Ignore boss room
#define ADVPATH_SUBROOM_WEIGHT_COUNT    (max(ADVPATH_SUBROOM_ROUTE_TOUGH, ADVPATH_SUBROOM_RESTSTOP_FULL) + 1)

#define ADVPATH_INVALID_ROOM_ID (0)

// Trainers
//
#define TRAINER_FLAG_NONE                       0
#define TRAINER_FLAG_GYM                        (1 << 0)
#define TRAINER_FLAG_ELITE                      (1 << 1)
#define TRAINER_FLAG_PRE_CHAMP                  (1 << 2)
#define TRAINER_FLAG_FINAL_CHAMP                (1 << 3)
#define TRAINER_FLAG_HOENN                      (1 << 4)
#define TRAINER_FLAG_KANTO                      (1 << 5)
#define TRAINER_FLAG_JOHTO                      (1 << 6)
#define TRAINER_FLAG_FALLBACK_REGION            (1 << 7) // If the player unselects all boss expansions, use this custom fallback set
#define TRAINER_FLAG_RAINBOW_EXCLUDE            (1 << 8)
#define TRAINER_FLAG_DISABLE_WEATHER            (1 << 9)
#define TRAINER_FLAG_THIRDSLOT_WEATHER          (1 << 10)
#define TRAINER_FLAG_RAINBOW_CHAMP              (1 << 11)
#define TRAINER_FLAG_NAME_IS_PLAYER             (1 << 12)
#define TRAINER_FLAG_NAME_IS_OPPOSITE_AVATAR    (1 << 13)
#define TRAINER_FLAG_ANY_REGION                 (TRAINER_FLAG_KANTO | TRAINER_FLAG_JOHTO | TRAINER_FLAG_HOENN | TRAINER_FLAG_FALLBACK_REGION)

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

#define TRAINER_GENERATOR_FLAG_NONE                     0
#define TRAINER_GENERATOR_FLAG_PREFER_STRONG_PRESETS    (1 << 0)
#define TRAINER_GENERATOR_FLAG_FORCE_STRONG_PRESETS     (1 << 1)
#define TRAINER_GENERATOR_FLAG_ALLOW_STRONG_LEGENDARY   (1 << 2)
#define TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY     (1 << 3)
#define TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY           (1 << 4)
#define TRAINER_GENERATOR_FLAG_ALLOW_ITEM_EVOS          (1 << 5)
#define TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE          (1 << 6)
#define TRAINER_GENERATOR_FLAG_COUNTER_COVERAGE         (1 << 7)
#define TRAINER_GENERATOR_FLAG_MIRROR_EXACT             (1 << 8)
#define TRAINER_GENERATOR_FLAG_MIRROR_SPECIES           (1 << 9)
#define TRAINER_GENERATOR_FLAG_MIRROR_ANY               (TRAINER_GENERATOR_FLAG_MIRROR_EXACT | TRAINER_GENERATOR_FLAG_MIRROR_SPECIES)

// 8 badges, 4 elite, 2 champion
#define ROGUE_MAX_BOSS_COUNT 14

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
#define MON_FLAGS_GEN7ANYTHINGGOES          MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
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
#define MON_FLAGS_GEN8ANYTHINGGOES          MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8NATIONALDEXAG         MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8MONOTYPE              MON_FLAG_STRONG | MON_FLAG_STRONG_WILD

#define MON_FLAGS_GEN9OU                    MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN9NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN9PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN91V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN9LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN9NATIONALDEX           MON_FLAG_NONE
#define MON_FLAGS_GEN9NATIONALDEXMONOTYPE   MON_FLAG_NONE
#define MON_FLAGS_GEN9MONOTYPE              MON_FLAG_STRONG | MON_FLAG_STRONG_WILD

#else

#define MON_FLAGS_GEN3UBERS                 MON_FLAG_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN3OU                    MON_FLAG_STRONG
#define MON_FLAGS_GEN3UU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3DOUBLESOU             MON_FLAG_DOUBLES
#define MON_FLAGS_GEN31V1                   MON_FLAG_STRONG
#endif

// Difficulty/Config lab settings
//
#define DIFFICULTY_LEVEL_EASY       0
#define DIFFICULTY_LEVEL_MEDIUM     1
#define DIFFICULTY_LEVEL_HARD       2
#define DIFFICULTY_LEVEL_BRUTAL     3
#define DIFFICULTY_LEVEL_CUSTOM     4

#define DIFFICULTY_PRESET_COUNT     4 // ignore custom

// DIFFICULTY_TOGGLE_
#define DIFFICULTY_TOGGLE_EXP_ALL           0
#define DIFFICULTY_TOGGLE_OVER_LVL          1
#define DIFFICULTY_TOGGLE_EV_GAIN           2
#define DIFFICULTY_TOGGLE_OVERWORLD_MONS    3
#define DIFFICULTY_TOGGLE_BAG_WIPE          4
#define DIFFICULTY_TOGGLE_SWITCH_MODE       5
#define DIFFICULTY_TOGGLE_COUNT             6

// DIFFICULTY_RANGE_
#define DIFFICULTY_RANGE_TRAINER    0
#define DIFFICULTY_RANGE_ITEM       1
#define DIFFICULTY_RANGE_LEGENDARY  2
#define DIFFICULTY_RANGE_COUNT      3

// Rogue Campaigns
//
#define ROGUE_CAMPAIGN_NONE                     0
#define ROGUE_CAMPAIGN_LOW_BST                  1
#define ROGUE_CAMPAIGN_CLASSIC                  2
#define ROGUE_CAMPAIGN_MINIBOSS_BATTLER         3
#define ROGUE_CAMPAIGN_AUTO_BATTLER             4
#define ROGUE_CAMPAIGN_LATERMANNER              5
#define ROGUE_CAMPAIGN_POKEBALL_LIMIT           6
#define ROGUE_CAMPAIGN_ONE_HP                   7

#define ROGUE_CAMPAIGN_FIRST                    ROGUE_CAMPAIGN_LOW_BST
#define ROGUE_CAMPAIGN_LAST                     ROGUE_CAMPAIGN_ONE_HP
#define ROGUE_CAMPAIGN_COUNT                    (ROGUE_CAMPAIGN_LAST - ROGUE_CAMPAIGN_FIRST + 1)

// Popups
//
#define POPUP_MSG_QUEST_COMPLETE                0
#define POPUP_MSG_QUEST_FAIL                    1
#define POPUP_MSG_LEGENDARY_CLAUSE              2
#define POPUP_MSG_CAMPAIGN_ANNOUNCE             3
#define POPUP_MSG_SAFARI_ENCOUNTERS             4
#define POPUP_MSG_PARTNER_EVO_WARNING           5
#define POPUP_MSG_ENCOUNTER_CHAIN               6
#define POPUP_MSG_ENCOUNTER_CHAIN_END           7

// Rogue Assistant
//
#define NET_PLAYER_CAPACITY 4

// Sound
//
#define ROGUE_SOUND_TYPE_UNKNOWN    0
#define ROGUE_SOUND_TYPE_CRY        1
#define ROGUE_SOUND_TYPE_MUSIC      2

// Misc.
//
// Number of bytes needed to cover all species
#define SPECIES_FLAGS_BYTE_COUNT (1 + NUM_SPECIES / 8)

#define STARTER_MON_LEVEL 10

#define FOLLOWMON_SHINY_OFFSET 10000
#define FOLLOWMON_MAX_SPAWN_SLOTS 6

#define WILD_ENCOUNTER_GRASS_CAPACITY 9
#define WILD_ENCOUNTER_WATER_CAPACITY 2
#define WILD_ENCOUNTER_TOTAL_CAPACITY (WILD_ENCOUNTER_GRASS_CAPACITY + WILD_ENCOUNTER_WATER_CAPACITY)

#define SEASON_SPRING   0
#define SEASON_SUMMER   1
#define SEASON_AUTUMN   2
#define SEASON_WINTER   3
#define SEASON_COUNT    4

#define TIME_PRESET_DAWN        0
#define TIME_PRESET_MIDDAY      1
#define TIME_PRESET_DUSK        2
#define TIME_PRESET_MIDNIGHT    3
#define TIME_PRESET_COUNT       4

#include "rogue_pokedex.h"
#include "rogue_quests.h"
#include "rogue_hub.h"

#endif