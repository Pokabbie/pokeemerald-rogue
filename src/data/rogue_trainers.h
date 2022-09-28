#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/pokemon.h"
#include "constants/weather.h"

#define ENCOUNTER_MAP(id, map) { .encounterId=id, .layout=LAYOUT_##map, .group=MAP_GROUP(map), .num=MAP_NUM(map) }

const u8 gRogueTypeWeatherTable[NUMBER_OF_MON_TYPES] = 
{
    [TYPE_NORMAL] = WEATHER_NONE,
    [TYPE_FIGHTING] = WEATHER_SUNNY,
    [TYPE_FLYING] = WEATHER_NONE,
    [TYPE_POISON] = WEATHER_SHADE,
    [TYPE_GROUND] = WEATHER_SANDSTORM,
    [TYPE_ROCK] = WEATHER_SANDSTORM,
    [TYPE_BUG] = WEATHER_SHADE,
    [TYPE_GHOST] = WEATHER_VOLCANIC_ASH,
    [TYPE_STEEL] = WEATHER_SANDSTORM,
    [TYPE_MYSTERY] = WEATHER_UNDERWATER_BUBBLES,
    [TYPE_FIRE] = WEATHER_DROUGHT,
    [TYPE_WATER] = WEATHER_DOWNPOUR,
    [TYPE_GRASS] = WEATHER_LEAVES,
    [TYPE_ELECTRIC] = WEATHER_RAIN_THUNDERSTORM,
    [TYPE_PSYCHIC] = WEATHER_FOG_HORIZONTAL,
    [TYPE_ICE] = WEATHER_SNOW,
    [TYPE_DRAGON] = WEATHER_DROUGHT,
    [TYPE_DARK] = WEATHER_VOLCANIC_ASH,
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = WEATHER_FOG_DIAGONAL,
#endif
};

const struct RogueEncounterMap gRogueTypeToEliteRoom[NUMBER_OF_MON_TYPES] = 
{
    [TYPE_NORMAL] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_FIGHTING] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_FLYING] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_POISON] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_GROUND] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_ROCK] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_BUG] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_GHOST] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_STEEL] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_MYSTERY] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
    [TYPE_FIRE] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_WATER] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
    [TYPE_GRASS] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_ELECTRIC] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_PSYCHIC] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_ICE] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
    [TYPE_DRAGON] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_DARK] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
#endif
};

static const struct RogueTrainerEncounter sRouteBossEncounters[] = 
{
    // Hoenn Gyms
    {
        .gfxId = OBJ_EVENT_GFX_ROXANNE,
        .trainerId = TRAINER_ROGUE_BOSS_ROXANNE,
        .victorySetFlag = FLAG_BADGE01_GET,
        .incTypes = { TYPE_ROCK, TYPE_NONE, TYPE_STEEL },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_BRAWLY,
        .trainerId = TRAINER_ROGUE_BOSS_BRAWLY,
        .victorySetFlag = FLAG_BADGE02_GET,
        .incTypes = { TYPE_FIGHTING, TYPE_NONE  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },
    {
        .gfxId = OBJ_EVENT_GFX_WATTSON,
        .trainerId = TRAINER_ROGUE_BOSS_WATTSON,
        .victorySetFlag = FLAG_BADGE03_GET,
        .incTypes = { TYPE_ELECTRIC, TYPE_NONE, TYPE_ELECTRIC },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_FLANNERY,
        .trainerId = TRAINER_ROGUE_BOSS_FLANNERY,
        .victorySetFlag = FLAG_BADGE04_GET,
        .incTypes = { TYPE_FIRE, TYPE_NONE, TYPE_FIRE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_NORMAN,
        .trainerId = TRAINER_ROGUE_BOSS_NORMAN,
        .victorySetFlag = FLAG_BADGE05_GET,
        .incTypes = { TYPE_NORMAL, TYPE_NONE, TYPE_DRAGON },
        .excTypes = { TYPE_FLYING, TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_WINONA,
        .trainerId = TRAINER_ROGUE_BOSS_WINONA,
        .victorySetFlag = FLAG_BADGE06_GET,
        .incTypes = { TYPE_FLYING, TYPE_NONE, TYPE_FLYING  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_LIZA,
        .trainerId = TRAINER_ROGUE_BOSS_TATE_AND_LIZA,
        .victorySetFlag = FLAG_BADGE07_GET,
        .incTypes = { TYPE_PSYCHIC, TYPE_NONE, TYPE_PSYCHIC  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_JUAN,
        .trainerId = TRAINER_ROGUE_BOSS_JUAN,
        .victorySetFlag = FLAG_BADGE08_GET,
        .incTypes = { TYPE_WATER, TYPE_NONE, TYPE_WATER  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },

    // Hoenn Elite
    {
        .gfxId = OBJ_EVENT_GFX_SIDNEY,
        .trainerId = TRAINER_ROGUE_BOSS_SIDNEY,
#ifdef ROGUE_EXPANSION
        .incTypes = { TYPE_DARK, TYPE_NONE, TYPE_FAIRY },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
#else
        .incTypes = { TYPE_DARK, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
#endif
    },
    {
        .gfxId = OBJ_EVENT_GFX_PHOEBE,
        .trainerId = TRAINER_ROGUE_BOSS_PHOEBE,
        .incTypes = { TYPE_GHOST, TYPE_NONE, TYPE_PSYCHIC },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_GLACIA,
        .trainerId = TRAINER_ROGUE_BOSS_GLACIA,
        .incTypes = { TYPE_ICE, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_DRAKE,
        .trainerId = TRAINER_ROGUE_BOSS_DRAKE,
        .incTypes = { TYPE_DRAGON, TYPE_NONE, TYPE_DRAGON },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE | TRAINER_FLAG_ELITE,
    },

    // Hoenn Champs
    {
        .gfxId = OBJ_EVENT_GFX_WALLACE,
        .trainerId = TRAINER_ROGUE_BOSS_WALLACE,
        .incTypes = { TYPE_WATER, TYPE_NONE, TYPE_WATER },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_PRE_CHAMP,
    },
    {
        .gfxId = OBJ_EVENT_GFX_STEVEN,
        .trainerId = TRAINER_ROGUE_BOSS_STEVEN,
        .incTypes = { TYPE_STEEL, TYPE_NONE, TYPE_PSYCHIC },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_FINAL_CHAMP | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },

    // Placeholder Rainbow mode bosses for missing types
    {
        .gfxId = OBJ_EVENT_GFX_LUCY,
        .trainerId = TRAINER_ROGUE_BOSS_LUCY,
        .incTypes = { TYPE_POISON, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_NONE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_BRANDON,
        .trainerId = TRAINER_ROGUE_BOSS_BRANDON,
        .incTypes = { TYPE_GROUND, TYPE_NONE, TYPE_GROUND },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_NONE | TRAINER_FLAG_THIRDSLOT_ACE_TYPE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_TUCKER,
        .trainerId = TRAINER_ROGUE_BOSS_TUCKER,
        .incTypes = { TYPE_BUG, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_NONE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_SPENSER,
        .trainerId = TRAINER_ROGUE_BOSS_SPENSER,
        .incTypes = { TYPE_GRASS, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_NONE,
    },
#ifdef ROGUE_EXPANSION
    {
        .gfxId = OBJ_EVENT_GFX_ANABEL,
        .trainerId = TRAINER_ROGUE_BOSS_ANABEL,
        .incTypes = { TYPE_FAIRY, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_NONE,
    },
#endif
};

const struct RogueTrainerData gRogueBossEncounters = 
{
    .count = ARRAY_COUNT(sRouteBossEncounters),
    .trainers = sRouteBossEncounters
};

static const struct RogueTrainerEncounter sRouteMiniBossEncounters[] = 
{
    {
        .gfxId = OBJ_EVENT_GFX_MAXIE,
        .trainerId = TRAINER_ROGUE_MINI_BOSS_MAXIE,
        .incTypes = { TYPE_FIRE, TYPE_DARK, TYPE_NONE },
        .excTypes = { TYPE_WATER, TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN,
    },
    {
        .gfxId = OBJ_EVENT_GFX_ARCHIE,
        .trainerId = TRAINER_ROGUE_MINI_BOSS_ARCHIE,
        .incTypes = { TYPE_WATER, TYPE_DARK, TYPE_NONE },
        .excTypes = { TYPE_FIRE, TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN,
    },
    {
        .gfxId = OBJ_EVENT_GFX_WALLY,
        .trainerId = TRAINER_ROGUE_MINI_BOSS_WALLY,
#ifdef ROGUE_EXPANSION
        .incTypes = { TYPE_PSYCHIC, TYPE_FAIRY, TYPE_NONE },
#else
        .incTypes = { TYPE_PSYCHIC, TYPE_GRASS, TYPE_NONE },
#endif
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN,
    },
    {
        .gfxId = OBJ_EVENT_GFX_BRENDAN_NORMAL,
        .trainerId = TRAINER_ROGUE_MINI_BOSS_MIRROR,
        .incTypes = { TYPE_MYSTERY, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN,
    },
};

const struct RogueTrainerData gRogueMiniBossEncounters = 
{
    .count = ARRAY_COUNT(sRouteMiniBossEncounters),
    .trainers = sRouteMiniBossEncounters
};

#undef ENCOUNTER_MAP