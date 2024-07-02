#ifndef ROGUE_CONFIG_H
#define ROGUE_CONFIG_H

#define ROGUE_VERSION_VANILLA       0
#define ROGUE_VERSION_EXPANSION     1


#ifdef ROGUE_EXPANSION
#define ROGUE_VERSION ROGUE_VERSION_EXPANSION
#else
#define ROGUE_VERSION ROGUE_VERSION_VANILLA
#endif

#ifdef ROGUE_DEBUG
#define ROGUE_DEBUG_LOGGING
#endif

#ifndef TESTING
#define TESTING 0
#endif

// Seems to be slower overall
//#define ROGUE_FEATURE_HQ_RANDOM 1
//#define ROGUE_FEATURE_SAFTEY_CHECKS 1

#ifdef ROGUE_EXPANSION
#define ROGUE_FEATURE_REMOVE_HIDDEN_MACHINES
#endif
//#define ROGUE_FEATURE_AUTOMATION // Activate this for builds where automated external interactions are enabled (e.g. Soak Tests)
//#define ROGUE_FEATURE_SKIP_SAVE_WARNINGS // Activate this if you intend on putting on a physical cart with 64k FLASH save

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#ifdef ROGUE_DEBUG
#define DEBUG_CODE(x) x
#define RELEASE_CODE(x)

#define UNUSED_DEBUG UNUSED
#define UNUSED_RELEASE
#else
#define DEBUG_CODE(x)
#define RELEASE_CODE(x) x

#define UNUSED_DEBUG
#define UNUSED_RELEASE UNUSED
#endif

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
#define ROGUE_SHOP_TREATS               10
#define ROGUE_SHOP_CURSES               11
#define ROGUE_SHOP_CHARMS               12
#define ROGUE_SHOP_HUB_UPGRADES         13
#define ROGUE_SHOP_COURIER              14

#define ITEM_LINK_CABLE                 (ITEM_ROGUE_ITEM_FIRST + 0)
#define ITEM_QUEST_LOG                  (ITEM_ROGUE_ITEM_FIRST + 1)
#define ITEM_HEALING_FLASK              (ITEM_ROGUE_ITEM_FIRST + 2)
#define ITEM_BASIC_RIDING_WHISTLE       (ITEM_ROGUE_ITEM_FIRST + 3)
#define ITEM_GOLD_RIDING_WHISTLE        (ITEM_ROGUE_ITEM_FIRST + 4)
#define ITEM_C_GEAR                     (ITEM_ROGUE_ITEM_FIRST + 5)
#define ITEM_DAYCARE_PHONE              (ITEM_ROGUE_ITEM_FIRST + 6)
#define ITEM_BUILDING_SUPPLIES          (ITEM_ROGUE_ITEM_FIRST + 7)
#define ITEM_ALOLA_STONE                (ITEM_ROGUE_ITEM_FIRST + 8)
#define ITEM_GALAR_STONE                (ITEM_ROGUE_ITEM_FIRST + 9)
#define ITEM_HISUI_STONE                (ITEM_ROGUE_ITEM_FIRST + 10)
#define ITEM_SMALL_COIN_CASE            (ITEM_ROGUE_ITEM_FIRST + 11)
#define ITEM_LARGE_COIN_CASE            (ITEM_ROGUE_ITEM_FIRST + 12)
#define ITEM_GOLDEN_SEED                (ITEM_ROGUE_ITEM_FIRST + 13)

// Free 10 - 19

#define ITEM_POKEBLOCK_NORMAL           (ITEM_ROGUE_ITEM_FIRST + 20)
#define ITEM_POKEBLOCK_FIGHTING         (ITEM_ROGUE_ITEM_FIRST + 21)
#define ITEM_POKEBLOCK_FLYING           (ITEM_ROGUE_ITEM_FIRST + 22)
#define ITEM_POKEBLOCK_POISON           (ITEM_ROGUE_ITEM_FIRST + 23)
#define ITEM_POKEBLOCK_GROUND           (ITEM_ROGUE_ITEM_FIRST + 24)
#define ITEM_POKEBLOCK_ROCK             (ITEM_ROGUE_ITEM_FIRST + 25)
#define ITEM_POKEBLOCK_BUG              (ITEM_ROGUE_ITEM_FIRST + 26)
#define ITEM_POKEBLOCK_GHOST            (ITEM_ROGUE_ITEM_FIRST + 27)
#define ITEM_POKEBLOCK_STEEL            (ITEM_ROGUE_ITEM_FIRST + 28)
#define ITEM_POKEBLOCK_FIRE             (ITEM_ROGUE_ITEM_FIRST + 29)
#define ITEM_POKEBLOCK_WATER            (ITEM_ROGUE_ITEM_FIRST + 30)
#define ITEM_POKEBLOCK_GRASS            (ITEM_ROGUE_ITEM_FIRST + 31)
#define ITEM_POKEBLOCK_ELECTRIC         (ITEM_ROGUE_ITEM_FIRST + 32)
#define ITEM_POKEBLOCK_PSYCHIC          (ITEM_ROGUE_ITEM_FIRST + 33)
#define ITEM_POKEBLOCK_ICE              (ITEM_ROGUE_ITEM_FIRST + 34)
#define ITEM_POKEBLOCK_DRAGON           (ITEM_ROGUE_ITEM_FIRST + 35)
#define ITEM_POKEBLOCK_DARK             (ITEM_ROGUE_ITEM_FIRST + 36)
#define ITEM_POKEBLOCK_FAIRY            (ITEM_ROGUE_ITEM_FIRST + 37)
#define ITEM_POKEBLOCK_SHINY            (ITEM_ROGUE_ITEM_FIRST + 38)

#define ITEM_POKEBLOCK_HP               (ITEM_ROGUE_ITEM_FIRST + 39)
#define ITEM_POKEBLOCK_ATK              (ITEM_ROGUE_ITEM_FIRST + 40)
#define ITEM_POKEBLOCK_DEF              (ITEM_ROGUE_ITEM_FIRST + 41)
#define ITEM_POKEBLOCK_SPEED            (ITEM_ROGUE_ITEM_FIRST + 42)
#define ITEM_POKEBLOCK_SPATK            (ITEM_ROGUE_ITEM_FIRST + 43)
#define ITEM_POKEBLOCK_SPDEF            (ITEM_ROGUE_ITEM_FIRST + 44)

#define FIRST_ITEM_POKEBLOCK            ITEM_POKEBLOCK_NORMAL
#define LAST_ITEM_POKEBLOCK             ITEM_POKEBLOCK_SPDEF
#define POKEBLOCK_ITEM_COUNT            (LAST_ITEM_POKEBLOCK - FIRST_ITEM_POKEBLOCK + 1)

#ifdef ROGUE_DEBUG
#define ITEM_DEBUG_MINT_HARDY           (ITEM_ROGUE_ITEM_FIRST + 46)
#define ITEM_DEBUG_MINT_ADAMANT         (ITEM_ROGUE_ITEM_FIRST + 47)
#define ITEM_DEBUG_UNUSED0              (ITEM_ROGUE_ITEM_FIRST + 48)
#define ITEM_DEBUG_UNUSED1              (ITEM_ROGUE_ITEM_FIRST + 49)
#endif


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
#define ITEM_TORMENT_CHARM              (FIRST_ITEM_CHARM + 10)
#define ITEM_PRESSURE_CHARM             (FIRST_ITEM_CHARM + 11)
#define ITEM_UNAWARE_CHARM              (FIRST_ITEM_CHARM + 12)
#define ITEM_ADAPTABILITY_CHARM         (FIRST_ITEM_CHARM + 13)
#define ITEM_INFINITE_EXTRA_LIFE_CHARM  (FIRST_ITEM_CHARM + 14)
#define ITEM_ALLOW_SAVE_SCUM_CHARM      (FIRST_ITEM_CHARM + 15)

#define LAST_ITEM_CHARM                 (ITEM_ALLOW_SAVE_SCUM_CHARM)


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
#define ITEM_SHUFFLE_CURSE              (FIRST_ITEM_CURSE + 12)
#define ITEM_MOVE_PRIORITY_CURSE        (FIRST_ITEM_CURSE + 13)
#define ITEM_ENDURE_CURSE               (FIRST_ITEM_CURSE + 14)
#define ITEM_TORMENT_CURSE              (FIRST_ITEM_CURSE + 15)
#define ITEM_PRESSURE_CURSE             (FIRST_ITEM_CURSE + 16)
#define ITEM_UNAWARE_CURSE              (FIRST_ITEM_CURSE + 17)
#define ITEM_ADAPTABILITY_CURSE         (FIRST_ITEM_CURSE + 18)
#define ITEM_SNOWBALL_CURSE             (FIRST_ITEM_CURSE + 19)
#define ITEM_RANDOMAN_ROUTE_SPAWN_CURSE (FIRST_ITEM_CURSE + 20)
#define ITEM_RANDOMAN_ALWAYS_SPAWN_CURSE (FIRST_ITEM_CURSE + 21)
#define ITEM_AUTO_MOVE_CURSE            (FIRST_ITEM_CURSE + 22)
#define ITEM_ONE_HIT_CURSE              (FIRST_ITEM_CURSE + 23)
#define ITEM_SNAG_CURSE                 (FIRST_ITEM_CURSE + 24)
#define ITEM_WILD_EGG_SPECIES_CURSE     (FIRST_ITEM_CURSE + 25)

#define LAST_ITEM_CURSE                 (ITEM_WILD_EGG_SPECIES_CURSE)

#define ITEM_TR01                       (LAST_ITEM_CURSE + 1)
#define ITEM_TR50                       (LAST_ITEM_CURSE + 50)

#define FIRST_ITEM_TR                   ITEM_TR01
#define LAST_ITEM_TR                    ITEM_TR50
#define NUM_TECHNICAL_RECORDS           50

#define ITEM_SORT_MODE_TYPE         0
#define ITEM_SORT_MODE_NAME         1
#define ITEM_SORT_MODE_VALUE        2
#define ITEM_SORT_MODE_AMOUNT       3
#define ITEM_SORT_MODE_COUNT        4

#define ITEM_BAG_SLOTS_PER_UPGRADE      10
#define ITEM_BAG_MAX_CAPACITY_UPGRADE   37


// Routes
//
#define ROUTE_FLAG_NONE                   0
#define ROUTE_FLAG_KANTO                  (1 << 1)
#define ROUTE_FLAG_JOHTO                  (1 << 2)
#define ROUTE_FLAG_HOENN                  (1 << 3)
#define ROUTE_FLAG_SINNOH                 (1 << 4)

#define ROUTE_FLAG_ANY                    (ROUTE_FLAG_KANTO | ROUTE_FLAG_JOHTO | ROUTE_FLAG_HOENN | ROUTE_FLAG_SINNOH)

// Adventure
//
#define ROGUE_ADVPATH_ROOM_CAPACITY 32

// AdvPath
//
#define ADVPATH_ROOM_NONE               0
#define ADVPATH_ROOM_ROUTE              1
#define ADVPATH_ROOM_RESTSTOP           2
#define ADVPATH_ROOM_LEGENDARY          3
#define ADVPATH_ROOM_MINIBOSS           4 // unused
#define ADVPATH_ROOM_TEAM_HIDEOUT       5
#define ADVPATH_ROOM_WILD_DEN           6
#define ADVPATH_ROOM_HONEY_TREE         7
#define ADVPATH_ROOM_GAMESHOW           8
#define ADVPATH_ROOM_DARK_DEAL          9
#define ADVPATH_ROOM_LAB                10
#define ADVPATH_ROOM_SHRINE             11
#define ADVPATH_ROOM_CATCHING_CONTEST   12
#define ADVPATH_ROOM_SIGN               13
#define ADVPATH_ROOM_BATTLE_SIM         14

#define ADVPATH_ROOM_BOSS               15

#define ADVPATH_ROOM_COUNT              16


#define ADVPATH_SUBROOM_ROUTE_CALM          0
#define ADVPATH_SUBROOM_ROUTE_AVERAGE       1
#define ADVPATH_SUBROOM_ROUTE_TOUGH         2

#define ADVPATH_SUBROOM_RESTSTOP_BATTLE     0
#define ADVPATH_SUBROOM_RESTSTOP_SHOP       1
#define ADVPATH_SUBROOM_RESTSTOP_DAYCARE    2
#define ADVPATH_SUBROOM_RESTSTOP_FULL       3


#define ADVPATH_ROOM_WEIGHT_COUNT       (ADVPATH_ROOM_LAB + 1) // Ignore boss room
#define ADVPATH_SUBROOM_WEIGHT_COUNT    (max(ADVPATH_SUBROOM_ROUTE_TOUGH, ADVPATH_SUBROOM_RESTSTOP_FULL) + 1)

#define ADVPATH_LEGEND_ROAMER           0
#define ADVPATH_LEGEND_MINOR            1
#define ADVPATH_LEGEND_BOX              2
#define ADVPATH_LEGEND_COUNT            3

#define ADVPATH_TEAM_ENCOUNTER_EARLY        0
#define ADVPATH_TEAM_ENCOUNTER_PRE_LEGEND   1
#define ADVPATH_TEAM_ENCOUNTER_COUNT        2

#define ADVPATH_INVALID_ROOM_ID (0)

// Trainers
//
#define TRAINER_FLAG_NONE                       0

#define TRAINER_FLAG_CLASS_BOSS                 (1 << 0) // gym, elite 4 or champion
#define TRAINER_FLAG_CLASS_RIVAL                (1 << 1)
#define TRAINER_FLAG_CLASS_ROUTE                (1 << 2)
#define TRAINER_FLAG_CLASS_TEAM                 (1 << 3)
#define TRAINER_FLAG_CLASS_TEAM_BOSS            (1 << 4)
#define TRAINER_FLAG_CLASS_SPECIAL              (1 << 5) // e.g. exp trainer

#define TRAINER_FLAG_REGION_KANTO               (1 << 6)
#define TRAINER_FLAG_REGION_JOHTO               (1 << 7)
#define TRAINER_FLAG_REGION_HOENN               (1 << 8)
#define TRAINER_FLAG_REGION_SINNOH              (1 << 9)
#define TRAINER_FLAG_REGION_UNOVA               (1 << 10)
#define TRAINER_FLAG_REGION_KALOS               (1 << 11)
#define TRAINER_FLAG_REGION_ALOLA               (1 << 12)
#define TRAINER_FLAG_REGION_GALAR               (1 << 13)
#define TRAINER_FLAG_REGION_PALDEA              (1 << 14)
#define TRAINER_FLAG_REGION_ROGUE               (1 << 15)

#define TRAINER_FLAG_MISC_RAINBOW_ONLY          (1 << 16)
#define TRAINER_FLAG_MISC_RAINBOW_EXCLUDE       (1 << 17)

#define TRAINER_FLAG_CLASS_ANY                  (TRAINER_FLAG_CLASS_BOSS | TRAINER_FLAG_CLASS_RIVAL | TRAINER_FLAG_CLASS_ROUTE | TRAINER_FLAG_CLASS_TEAM)
#define TRAINER_FLAG_REGION_ANY                 (TRAINER_FLAG_REGION_KANTO | TRAINER_FLAG_REGION_JOHTO | TRAINER_FLAG_REGION_HOENN | TRAINER_FLAG_REGION_SINNOH | TRAINER_FLAG_REGION_UNOVA | TRAINER_FLAG_REGION_KALOS | TRAINER_FLAG_REGION_ALOLA | TRAINER_FLAG_REGION_GALAR | TRAINER_FLAG_REGION_PALDEA | TRAINER_FLAG_REGION_ROGUE)
#define TRAINER_FLAG_REGION_DEFAULT             (TRAINER_FLAG_REGION_ANY & ~TRAINER_FLAG_REGION_ROGUE)


// Class specific flags
#define CLASS_FLAG_NONE                       0

// CLASS_FLAG_BOSS
//
#define CLASS_FLAG_BOSS_GYM_1                   (1 << 0)
#define CLASS_FLAG_BOSS_GYM_2                   (1 << 1)
#define CLASS_FLAG_BOSS_GYM_3                   (1 << 2)
#define CLASS_FLAG_BOSS_GYM_4                   (1 << 3)
#define CLASS_FLAG_BOSS_GYM_5                   (1 << 4)
#define CLASS_FLAG_BOSS_GYM_6                   (1 << 5)
#define CLASS_FLAG_BOSS_GYM_7                   (1 << 6)
#define CLASS_FLAG_BOSS_GYM_8                   (1 << 7)
#define CLASS_FLAG_BOSS_ELITE_1                 (1 << 8)
#define CLASS_FLAG_BOSS_ELITE_2                 (1 << 9)
#define CLASS_FLAG_BOSS_ELITE_3                 (1 << 10)
#define CLASS_FLAG_BOSS_ELITE_4                 (1 << 11)
#define CLASS_FLAG_BOSS_CHAMP                   (1 << 12)

#define CLASS_FLAG_BOSS_ANY_GYM                 (CLASS_FLAG_BOSS_GYM_1 | CLASS_FLAG_BOSS_GYM_2 | CLASS_FLAG_BOSS_GYM_3 | CLASS_FLAG_BOSS_GYM_4 | CLASS_FLAG_BOSS_GYM_5 | CLASS_FLAG_BOSS_GYM_6 | CLASS_FLAG_BOSS_GYM_7 | CLASS_FLAG_BOSS_GYM_8)
#define CLASS_FLAG_BOSS_ANY_ELITE               (CLASS_FLAG_BOSS_ELITE_1 | CLASS_FLAG_BOSS_ELITE_2 | CLASS_FLAG_BOSS_ELITE_3 | CLASS_FLAG_BOSS_ELITE_4)
#define CLASS_FLAG_BOSS_ANY                     (CLASS_FLAG_BOSS_ANY_GYM | CLASS_FLAG_BOSS_ANY_ELITE | CLASS_FLAG_BOSS_CHAMP)

// TRAINER_FLAG_CLASS_TEAM
//
#define CLASS_FLAG_TEAM_ROCKET                  (1 << 0)
#define CLASS_FLAG_TEAM_AQUA                    (1 << 1)
#define CLASS_FLAG_TEAM_MAGMA                   (1 << 2)
#define CLASS_FLAG_TEAM_GALACTIC                (1 << 3)

#define CLASS_FLAG_ANY_TEAM                     (CLASS_FLAG_TEAM_ROCKET | CLASS_FLAG_TEAM_AQUA | CLASS_FLAG_TEAM_MAGMA | CLASS_FLAG_TEAM_GALACTIC)

// TRAINER_FLAG_CLASS_SPECIAL
//
#define CLASS_FLAG_EXP_TRAINER                  (1 << 0)
#define CLASS_FLAG_BATTLE_SIM                   (1 << 1)

// Strings
#define TRAINER_STRING_PRE_BATTLE_OPENNING      0 // before encounter music (Only supported for gyms)
#define TRAINER_STRING_PRE_BATTLE_TAUNT         1 // after encounter music, before battle
#define TRAINER_STRING_POST_BATTLE_TAUNT        2 // before leave battle victory message
#define TRAINER_STRING_POST_BATTLE_CLOSER       3 // after battle (back in overworld; auto speak for gyms)
#define TRAINER_STRING_COUNT                    4

// Teams
#define TEAM_NUM_KANTO_ROCKET   0
#define TEAM_NUM_JOHTO_ROCKET   1
#define TEAM_NUM_AQUA           2
#define TEAM_NUM_MAGMA          3
#define TEAM_NUM_GALACTIC       4
#define TEAM_NUM_COUNT          5

#define TEAM_PRE_LEGEND_MAP_OFFSET 100


// 8 badges, 4 elite, 2 champion
#define ROGUE_MAX_BOSS_COUNT 14
#define ROGUE_FINAL_CHAMP_DIFFICULTY 13
#define ROGUE_CHAMP_START_DIFFICULTY 12
#define ROGUE_ELITE_START_DIFFICULTY 8
#define ROGUE_GYM_MID_DIFFICULTY 4
#define ROGUE_GYM_START_DIFFICULTY 0

#define ROGUE_RIVAL_TOTAL_MON_COUNT 10
#define ROGUE_RIVAL_MAX_ROUTE_ENCOUNTERS 4

#define ROGUE_MAX_ACTIVE_TRAINER_COUNT 18

// Some defines to use in trainer setup just to make the EX/Vanilla versioning easier ;)
#ifdef ROGUE_EXPANSION
#define ITEM_POKABBIE_POKEBALL          ITEM_PARK_BALL
#define ITEM_FINAL_TRAINERS_POKEBALL    ITEM_CHERISH_BALL
#define ITEM_FINAL_BOSS_POKEBALL        ITEM_CHERISH_BALL
#define ITEM_CATCHING_CONTEST_POKEBALL  ITEM_SAFARI_BALL
#define ITEM_CUSTOM_MON_POKEBALL        ITEM_CHERISH_BALL

#define SPECIES_RAVEN_ACE               SPECIES_GOOMY
#define SPECIES_ERMA_ACE                SPECIES_FOMANTIS
#define SPECIES_KATE_ACE                SPECIES_TAROUNTULA

#define SPECIES_CYRUS_ACE               SPECIES_GIRATINA
#else
#define ITEM_POKABBIE_POKEBALL          ITEM_LUXURY_BALL
#define ITEM_FINAL_TRAINERS_POKEBALL    ITEM_PREMIER_BALL
#define ITEM_FINAL_BOSS_POKEBALL        ITEM_LUXURY_BALL
#define ITEM_CATCHING_CONTEST_POKEBALL  ITEM_SAFARI_BALL
#define ITEM_CUSTOM_MON_POKEBALL        ITEM_LUXURY_BALL

#define SPECIES_RAVEN_ACE               SPECIES_NONE
#define SPECIES_ERMA_ACE                SPECIES_NONE
#define SPECIES_KATE_ACE                SPECIES_GASTLY

#define SPECIES_CYRUS_ACE               SPECIES_RAYQUAZA
#endif

// For final quest we're going to only show a single rival per "gym gen"
#define TYPE_FINAL_TYPE_1               TYPE_NORMAL
#define TYPE_FINAL_TYPE_2               TYPE_FIGHTING
#define TYPE_FINAL_TYPE_3               TYPE_FLYING
#define TYPE_FINAL_TYPE_4               TYPE_POISON
#define TYPE_FINAL_TYPE_5               TYPE_GROUND
#define TYPE_FINAL_TYPE_6               TYPE_ROCK
#define TYPE_FINAL_TYPE_7               TYPE_BUG
#define TYPE_FINAL_TYPE_8               TYPE_GHOST
#define TYPE_FINAL_TYPE_9               TYPE_STEEL

// Mon Preset flags
//
#define MON_FLAG_NONE                   0
#define MON_FLAG_SINGLES_STRONG         (1 << 0) // Used to filter for E4/Champ fights
#define MON_FLAG_DOUBLES_STRONG         (1 << 1)
#define MON_FLAG_STRONG_WILD            (1 << 2) // Used to filter for early legendary encounters

// These's are the category names fed in by the Showdown presets
#ifdef ROGUE_EXPANSION
#define MON_FLAGS_GEN6UBERS                 MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN6OU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN6UU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN6RU                    MON_FLAG_NONE
#define MON_FLAGS_GEN6NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN6PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN6ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN6LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN6DOUBLESOU             MON_FLAG_DOUBLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN6BATTLESPOTSINGLES     MON_FLAG_NONE
#define MON_FLAGS_GEN6BATTLESPOTDOUBLES     MON_FLAG_DOUBLES_STRONG
#define MON_FLAGS_GEN61V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN6ANYTHINGGOES          MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN6MONOTYPE              MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN6VGC2016               MON_FLAG_STRONG_WILD

#define MON_FLAGS_GEN7UBERS                 MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7OU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7UU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7RU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN7LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN7DOUBLESOU             MON_FLAG_DOUBLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7BATTLESPOTSINGLES     MON_FLAG_NONE
#define MON_FLAGS_GEN7BATTLESPOTDOUBLES     MON_FLAG_DOUBLES_STRONG
#define MON_FLAGS_GEN7VGC2017               MON_FLAG_DOUBLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7VGC2018               MON_FLAG_DOUBLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN71V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN7ANYTHINGGOES          MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN7LETSGOOU              MON_FLAG_NONE
#define MON_FLAGS_GEN7MONOTYPE              MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD

#define MON_FLAGS_GEN8UBERS                 MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8OU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8UU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8RU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN8LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN8NATIONALDEX           MON_FLAG_NONE
#define MON_FLAGS_GEN8DOUBLESOU             MON_FLAG_DOUBLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8BATTLESTADIUMSINGLES  MON_FLAG_NONE
#define MON_FLAGS_GEN8VGC2020               MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN81V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN8ANYTHINGGOES          MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8NATIONALDEXAG         MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8MONOTYPE              MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN8NATIONALDEXMONOTYPE   MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD


#define MON_FLAGS_GEN9UBERS                 MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN9OU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN9UU                    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN9RU                    MON_FLAG_NONE
#define MON_FLAGS_GEN9NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN9PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN9ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN91V1                   MON_FLAG_NONE
#define MON_FLAGS_GEN9LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN9NATIONALDEX           MON_FLAG_NONE
#define MON_FLAGS_GEN9NATIONALDEXMONOTYPE   MON_FLAG_NONE
#define MON_FLAGS_GEN9ANYTHINGGOES          MON_FLAG_NONE
#define MON_FLAGS_GEN9NATIONALDEXAG         MON_FLAG_NONE
#define MON_FLAGS_GEN9VGC2023REGULATIONE    MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN9DOUBLESOU             MON_FLAG_DOUBLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN9MONOTYPE              MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD

#else

#define MON_FLAGS_GEN3UBERS                 MON_FLAG_SINGLES_STRONG | MON_FLAG_STRONG_WILD
#define MON_FLAGS_GEN3OU                    MON_FLAG_SINGLES_STRONG
#define MON_FLAGS_GEN3UU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3NU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3PU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3LC                    MON_FLAG_NONE
#define MON_FLAGS_GEN3ZU                    MON_FLAG_NONE
#define MON_FLAGS_GEN3DOUBLESOU             MON_FLAG_DOUBLES_STRONG
#define MON_FLAGS_GEN31V1                   MON_FLAG_SINGLES_STRONG
#endif

// Difficulty/Config lab settings
//
#define DIFFICULTY_LEVEL_EASY       0
#define DIFFICULTY_LEVEL_AVERAGE    1
#define DIFFICULTY_LEVEL_HARD       2
#define DIFFICULTY_LEVEL_BRUTAL     3
#define DIFFICULTY_LEVEL_CUSTOM     4

#define DIFFICULTY_PRESET_COUNT     4 // ignore custom

#define DIFFICULTY_LEVEL_NONE       DIFFICULTY_PRESET_COUNT


// CONFIG_TOGGLE_
#define CONFIG_TOGGLE_EXP_ALL               0
#define CONFIG_TOGGLE_OVER_LVL              1
#define CONFIG_TOGGLE_EV_GAIN               2
#define CONFIG_TOGGLE_OVERWORLD_MONS        3
#define CONFIG_TOGGLE_BAG_WIPE              4
#define CONFIG_TOGGLE_SWITCH_MODE           5
#define CONFIG_TOGGLE_DIVERSE_TRAINERS      6
#define CONFIG_TOGGLE_AFFECTION             7
#define CONFIG_TOGGLE_RELEASE_MONS          8
#define CONFIG_TOGGLE_TRAINER_ROGUE         9
#define CONFIG_TOGGLE_TRAINER_KANTO         10
#define CONFIG_TOGGLE_TRAINER_JOHTO         11
#define CONFIG_TOGGLE_TRAINER_HOENN         12
#ifdef ROGUE_EXPANSION
#define CONFIG_TOGGLE_TRAINER_SINNOH        13
#define CONFIG_TOGGLE_TRAINER_UNOVA         14
#define CONFIG_TOGGLE_TRAINER_KALOS         15
#define CONFIG_TOGGLE_TRAINER_ALOLA         16
#define CONFIG_TOGGLE_TRAINER_GALAR         17
#define CONFIG_TOGGLE_TRAINER_PALDEA        18
#endif
#define CONFIG_TOGGLE_COUNT                 19
#define CONFIG_TOGGLE_BYTE_COUNT            (1 + CONFIG_TOGGLE_COUNT / 8)

// CONFIG_RANGE_
#define CONFIG_RANGE_TRAINER                0
#define CONFIG_RANGE_ITEM                   1 // unused
#define CONFIG_RANGE_LEGENDARY              2 // unused
#define CONFIG_RANGE_BATTLE_FORMAT          3
#define CONFIG_RANGE_POKEDEX_VARIANT        4
#define CONFIG_RANGE_GAME_MODE_NUM          5
#define CONFIG_RANGE_DIFFICULTY_PRESET      6
#define CONFIG_RANGE_COUNT                  7

#define DEBUG_START_VALUE           0x7FFF

// DEBUG_TOGGLE_
#define DEBUG_TOGGLE_INFO_PANEL                     (DEBUG_START_VALUE + 0)
#define DEBUG_TOGGLE_STEAL_TEAM                     (DEBUG_START_VALUE + 1)
#define DEBUG_TOGGLE_TRAINER_LVL_5                  (DEBUG_START_VALUE + 2)
#define DEBUG_TOGGLE_ALLOW_SAVE_SCUM                (DEBUG_START_VALUE + 3)
#define DEBUG_TOGGLE_INSTANT_CAPTURE                (DEBUG_START_VALUE + 4)
#define DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR     (DEBUG_START_VALUE + 5)
#define DEBUG_TOGGLE_DEBUG_SHOPS                    (DEBUG_START_VALUE + 6)
#define DEBUG_TOGGLE_DEBUG_LEGENDS                  (DEBUG_START_VALUE + 7)
#define DEBUG_TOGGLE_DEBUG_MON_QUERY                (DEBUG_START_VALUE + 8)
#define DEBUG_TOGGLE_DEBUG_ITEM_QUERY               (DEBUG_START_VALUE + 9)
#define DEBUG_TOGGLE_HIDE_FOLLOWER                  (DEBUG_START_VALUE + 10)
#define DEBUG_TOGGLE_STOP_WILD_SPAWNING             (DEBUG_START_VALUE + 11)
#define DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT      (DEBUG_START_VALUE + 12)
#define DEBUG_TOGGLE_COUNT                          13
#define DEBUG_TOGGLE_BYTE_COUNT                     (1 + DEBUG_TOGGLE_COUNT / 8)

// DEBUG_RANGE_
#define DEBUG_RANGE_START_DIFFICULTY                (DEBUG_START_VALUE + 0)
#define DEBUG_RANGE_FORCED_ROUTE                    (DEBUG_START_VALUE + 1)
// ...
#define DEBUG_RANGE_FORCED_EVIL_TEAM                (DEBUG_START_VALUE + 3)
#define DEBUG_RANGE_COUNT                           4

#define BATTLE_FORMAT_SINGLES   0
#define BATTLE_FORMAT_DOUBLES   1
#define BATTLE_FORMAT_MIXED     2
#define BATTLE_FORMAT_COUNT     3


// Game Modes
//
#define ROGUE_GAME_MODE_STANDARD                0
#define ROGUE_GAME_MODE_RAINBOW                 1
#define ROGUE_GAME_MODE_OFFICIAL                2
#define ROGUE_GAME_MODE_GAUNTLET                3
#define ROGUE_GAME_MODE_RAINBOW_GAUNTLET        4
#define ROGUE_GAME_MODE_COUNT                   5

// Order
#define TRAINER_ORDER_DEFAULT                   0
#define TRAINER_ORDER_RAINBOW                   1
#define TRAINER_ORDER_OFFICIAL                  2
#define TRAINER_ORDER_COUNT                     3

// Generators
#define ADV_GENERATOR_STANDARD                  0
#define ADV_GENERATOR_GAUNTLET                  1


// Rogue Campaigns OLD
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
#define NET_PLAYER_CAPACITY 2   // host and connecting player (2 palettes reserved for each player: main body and follow/ride mon)
#define NET_PLAYER_MOVEMENT_BUFFER_SIZE         8

#define NET_HANDSHAKE_STATE_NONE                0
#define NET_HANDSHAKE_STATE_SEND_TO_HOST        1
#define NET_HANDSHAKE_STATE_SEND_TO_CLIENT      2

#define NET_CMD_BUFFER_SIZE                 64
#define NET_CMD_UNRESERVED_BUFFER_SIZE      62 // 2 bytes at front reserved for cmd ID

// Home
//
#define ASSISTANT_HOME_LOCAL_BOXES          10
#define ASSISTANT_HOME_REMOTE_BOXES         (10 * 5)
#define ASSISTANT_HOME_TOTAL_BOXES          (ASSISTANT_HOME_LOCAL_BOXES + ASSISTANT_HOME_REMOTE_BOXES)

// Multiplayer
//
#define MP_PLAYER_STATUS_NONE               0
#define MP_PLAYER_STATUS_TALK_TO_PLAYER     1
#define MP_PLAYER_STATUS_COUNT              2

// Sound
//
#define ROGUE_SOUND_TYPE_UNKNOWN            0
#define ROGUE_SOUND_TYPE_CRY                1
#define ROGUE_SOUND_TYPE_MUSIC              2

#define BATTLE_MUSIC_TYPE_TRAINER           0
#define BATTLE_MUSIC_TYPE_WILD              1

// Known battle music that we can dyanmically lookup in code
// other music players will be added on from BATTLE_MUSIC_UNKNOWN_START
#define BATTLE_MUSIC_NONE                   0
#define BATTLE_MUSIC_WILD_BATTLE            1
#define BATTLE_MUSIC_LEGENDARY_BATTLE       2
#define BATTLE_MUSIC_UNKNOWN_START          3

// Intro states for VAR_ROGUE_INTRO_STATE
//
#define ROGUE_INTRO_STATE_SPAWN                 0
#define ROGUE_INTRO_STATE_EXPLORE               1
#define ROGUE_INTRO_STATE_LEAVE_LAB             2
#define ROGUE_INTRO_STATE_CATCH_MON             3
#define ROGUE_INTRO_STATE_REPORT_TO_PROF        4
#define ROGUE_INTRO_STATE_COLLECT_QUEST_LOG     5
#define ROGUE_INTRO_STATE_GO_ON_ADVENTURE       6
#define ROGUE_INTRO_STATE_LEARN_TO_BUILD        7
#define ROGUE_INTRO_STATE_COMPLETE              8

// Pokedex
//
#define POKEDEX_DYNAMIC_VARIANT_START               POKEDEX_VARIANT_END

#define POKEDEX_DYNAMIC_VARIANT_NORMAL_SAFARI       (POKEDEX_DYNAMIC_VARIANT_START + 1)
#define POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI       (POKEDEX_DYNAMIC_VARIANT_START + 2)
#define POKEDEX_DYNAMIC_VARIANT_EGG_SPECIES         (POKEDEX_DYNAMIC_VARIANT_START + 3)
#define POKEDEX_DYNAMIC_VARIANT_FINAL_SPECIES       (POKEDEX_DYNAMIC_VARIANT_START + 4)

#define POKEDEX_DYNAMIC_VARIANT_END                 POKEDEX_DYNAMIC_VARIANT_FINAL_SPECIES
#define POKEDEX_INVALID_VARIANT                     (POKEDEX_DYNAMIC_VARIANT_END + 1)

#ifdef ROGUE_EXPANSION
#define POKEDEX_VARIANT_DEFAULT         POKEDEX_VARIANT_ROGUE_MODERN
#define POKEDEX_VARIANT_NATIONAL_MAX    POKEDEX_VARIANT_NATIONAL_GEN9
#define POKEDEX_MAX_GEN                 9
#else
#define POKEDEX_VARIANT_DEFAULT         POKEDEX_VARIANT_NATIONAL_GEN3
#define POKEDEX_VARIANT_NATIONAL_MAX    POKEDEX_VARIANT_NATIONAL_GEN3
#define POKEDEX_MAX_GEN                 3
#endif

// Gift Mons
//
#define OTID_FLAG_CUSTOM_MON            (1 << 31)
#define OTID_FLAG_DYNAMIC_CUSTOM_MON    (1 << 30)

// Dynamic custom mon flag is only valid if the first is set
#define OTID_FLAG_STANDARD_MASK         (~OTID_FLAG_CUSTOM_MON)

// Misc.
//
// Number of bytes needed to cover all species
#define SPECIES_FLAGS_BYTE_COUNT (1 + NUM_SPECIES / 8)

#ifdef ROGUE_EXPANSION
#define SPECIES_FINAL_EVO_STAGE_COUNT   606
#define SPECIES_EGG_EVO_STAGE_COUNT     569
#else
#define SPECIES_FINAL_EVO_STAGE_COUNT   214
#define SPECIES_EGG_EVO_STAGE_COUNT     202
#endif

#define MON_MASTERY_TOTAL_COUNT     SPECIES_EGG_EVO_STAGE_COUNT
#define MON_MASTERY_BYTE_COUNT      (1 + MON_MASTERY_TOTAL_COUNT / 8)

#define INVALID_HISTORY_ENTRY ((u16)-1)

#define STARTER_MON_LEVEL 10
#define GYM_FIRST_LEVEL_CAP 15
#define GYM_LEVEL_CAP_JUMP 10
#define MON_DATA_TEMP_PARTNER_RIBBON         MON_DATA_EFFORT_RIBBON


#define FOLLOWMON_SHINY_OFFSET              10000
#define FOLLOWMON_MAX_SPAWN_SLOTS           6 // 4 reserved palette slots, 1 for the 10th pal slot (if not in use) and 1 for the follower (todo change to 6)
#define FOLLOWMON_IDEAL_OBJECT_EVENT_COUNT  8

#define WILD_ENCOUNTER_GRASS_CAPACITY 9
#define WILD_ENCOUNTER_WATER_CAPACITY 3
#define WILD_ENCOUNTER_TOTAL_CAPACITY (WILD_ENCOUNTER_GRASS_CAPACITY + WILD_ENCOUNTER_WATER_CAPACITY)

// safariMons slots:
// 0-44 are for regular mons
// 45-49 are for legends
#define ROGUE_SAFARI_TOTAL_MONS             60
#define ROGUE_SAFARI_LEGENDS_START_INDEX    45
#define ROGUE_SAFARI_TOTAL_CUSTOM_MONS      16

#define ROGUE_SAFARI_LEGENDS_POKEBLOCK_COST 20

#define LAB_MON_COUNT               3
#define DAYCARE_SLOT_COUNT          3
#define DYNAMIC_UNIQUE_MON_COUNT    5

#define ROGUE_ADVENTURE_REPLAY_MOST_RECENT  0
#define ROGUE_ADVENTURE_REPLAY_REMEMBERED   1
#define ROGUE_ADVENTURE_REPLAY_COUNT        2

// don't waste a bit on mystery type (shift everything down)
#define __MON_TYPE_VAL(type)     (type > TYPE_MYSTERY ? (type - 1) : type)
#define MON_TYPE_VAL_TO_FLAGS(type)     ((u32)(!IS_STANDARD_TYPE(type) ? 0U : ((u32)1 << (u32)__MON_TYPE_VAL(type))))

#define __MON_GEN_VAL(gen)      ((gen - 1) % POKEDEX_MAX_GEN)
#define MON_GEN_TO_FLAGS(gen)   ((u32)1 << (u32)__MON_GEN_VAL(gen))

#define RIDE_WHISTLE_BASIC      0
#define RIDE_WHISTLE_GOLD       1

// Special modes
#define ROGUE_SPECIAL_MODE_NONE           0
#define ROGUE_SPECIAL_MODE_PLANTING       1
#define ROGUE_SPECIAL_MODE_DECORATING     2

// Placeholder gfx
#define TRAINER_PIC_PLACEHOLDER_NPC         TRAINER_PIC_COLLECTOR
#define OBJ_EVENT_GFX_PLACEHOLDER_NPC       OBJ_EVENT_GFX_MISC_KALOS_ENGINEER

#define TRAINER_PIC_DEBUG_NPC               TRAINER_PIC_COLLECTOR
#define OBJ_EVENT_GFX_DEBUG_NPC             OBJ_EVENT_GFX_QUINTY_PLUMP

#define REDIRECT_PARAM_NONE                 0
#define REDIRECT_PARAM_TRAINER_CLASS        1
#define REDIRECT_PARAM_SPECIES              2

// Seasons/ToD
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

// Pies
#define PIE_SIZE_SMALL      0
#define PIE_SIZE_MEDIUM     1
#define PIE_SIZE_LARGE      2

#define PIE_CRUST_AMOUNT                3
#define PIE_SMALL_FILLING_AMOUNT        3
#define PIE_MEDIUM_FILLING_AMOUNT       6
#define PIE_LARGE_FILLING_AMOUNT        9

#include "rogue_pokedex.h"
#include "rogue_quests.h"
#include "rogue_hub.h"

#endif