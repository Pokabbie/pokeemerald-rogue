#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/pokemon.h"
#include "constants/weather.h"

#define ENCOUNTER_MAP(id, map) { .encounterId=id, .layout=LAYOUT_##map, .group=MAP_GROUP(map), .num=MAP_NUM(map) }

const u8 gRogueTypeWeatherTable[NUMBER_OF_MON_TYPES] = 
{
    [TYPE_NORMAL] = WEATHER_SHADE,
    [TYPE_FIGHTING] = WEATHER_SUNNY,
    [TYPE_FLYING] = WEATHER_RAIN,
    [TYPE_POISON] = WEATHER_SHADE,
    [TYPE_GROUND] = WEATHER_SANDSTORM,
    [TYPE_ROCK] = WEATHER_SANDSTORM,
    [TYPE_BUG] = WEATHER_SHADE,
    [TYPE_GHOST] = WEATHER_VOLCANIC_ASH,
    [TYPE_STEEL] = WEATHER_SANDSTORM,
    [TYPE_MYSTERY] = WEATHER_UNDERWATER_BUBBLES,
    [TYPE_FIRE] = WEATHER_DROUGHT,
    [TYPE_WATER] = WEATHER_DOWNPOUR,
    [TYPE_GRASS] = WEATHER_NONE, // TODO - Add petal/leaves weather for grassy terrain
    [TYPE_ELECTRIC] = WEATHER_RAIN_THUNDERSTORM,
    [TYPE_PSYCHIC] = WEATHER_FOG_HORIZONTAL,
    [TYPE_ICE] = WEATHER_SNOW,
    [TYPE_DRAGON] = WEATHER_DROUGHT,
    [TYPE_DARK] = WEATHER_VOLCANIC_ASH,
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = WEATHER_FOG_DIAGONAL,
#endif
};

static const struct RogueTrainerEncounter sRouteBossEncounters[] = 
{
    // Hoenn Gyms
    {
        .gfxId = OBJ_EVENT_GFX_ROXANNE,
        .trainerId = TRAINER_ROGUE_BOSS_ROXANNE,
        .victorySetFlag = FLAG_BADGE01_GET,
        .incTypes = { TYPE_ROCK, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
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
        .incTypes = { TYPE_ELECTRIC, TYPE_NONE  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },
    {
        .gfxId = OBJ_EVENT_GFX_FLANNERY,
        .trainerId = TRAINER_ROGUE_BOSS_FLANNERY,
        .victorySetFlag = FLAG_BADGE04_GET,
        .incTypes = { TYPE_FIRE, TYPE_NONE  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },
    {
        .gfxId = OBJ_EVENT_GFX_NORMAN,
        .trainerId = TRAINER_ROGUE_BOSS_NORMAN,
        .victorySetFlag = FLAG_BADGE05_GET,
        .incTypes = { TYPE_NORMAL, TYPE_NONE  },
        .excTypes = { TYPE_FLYING, TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },
    {
        .gfxId = OBJ_EVENT_GFX_WINONA,
        .trainerId = TRAINER_ROGUE_BOSS_WINONA,
        .victorySetFlag = FLAG_BADGE06_GET,
        .incTypes = { TYPE_FLYING, TYPE_NONE  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },
    {
        .gfxId = OBJ_EVENT_GFX_LIZA,
        .trainerId = TRAINER_ROGUE_BOSS_TATE_AND_LIZA,
        .victorySetFlag = FLAG_BADGE07_GET,
        .incTypes = { TYPE_PSYCHIC, TYPE_NONE  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },
    {
        .gfxId = OBJ_EVENT_GFX_JUAN,
        .trainerId = TRAINER_ROGUE_BOSS_JUAN,
        .victorySetFlag = FLAG_BADGE08_GET,
        .incTypes = { TYPE_WATER, TYPE_NONE  },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
    },

    // Hoenn Elite
    {
        .gfxId = OBJ_EVENT_GFX_SIDNEY,
        .trainerId = TRAINER_ROGUE_BOSS_SIDNEY,
        .incTypes = { TYPE_DARK, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
    },
    {
        .gfxId = OBJ_EVENT_GFX_PHOEBE,
        .trainerId = TRAINER_ROGUE_BOSS_PHOEBE,
        .incTypes = { TYPE_GHOST, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
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
        .incTypes = { TYPE_DRAGON, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
    },

    // Hoenn Champs
    {
        .gfxId = OBJ_EVENT_GFX_WALLACE,
        .trainerId = TRAINER_ROGUE_BOSS_WALLACE,
        .incTypes = { TYPE_WATER, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_PRE_CHAMP,
    },
    {
        .gfxId = OBJ_EVENT_GFX_STEVEN,
        .trainerId = TRAINER_ROGUE_BOSS_STEVEN,
        .incTypes = { TYPE_STEEL, TYPE_NONE },
        .excTypes = { TYPE_NONE },
        .flags = TRAINER_FLAG_HOENN | TRAINER_FLAG_FINAL_CHAMP,
    }
};

const struct RogueTrainerData gRogueBossEncounters = 
{
    .count = ARRAY_COUNT(sRouteBossEncounters),
    .trainers = sRouteBossEncounters
};

static const struct RogueEncounterMap sRouteMiniBossEncounters[] = 
{
    ENCOUNTER_MAP(OBJ_EVENT_GFX_MAXIE, ROGUE_ENCOUNTER_MINI_BOSS),//, WEATHER_DROUGHT),
    ENCOUNTER_MAP(OBJ_EVENT_GFX_ARCHIE, ROGUE_ENCOUNTER_MINI_BOSS),//, WEATHER_DOWNPOUR),
    ENCOUNTER_MAP(OBJ_EVENT_GFX_WALLY, ROGUE_ENCOUNTER_MINI_BOSS),//, WEATHER_FOG_DIAGONAL),
    ENCOUNTER_MAP(OBJ_EVENT_GFX_BRENDAN_NORMAL, ROGUE_ENCOUNTER_MINI_BOSS)//, WEATHER_UNDERWATER_BUBBLES) // Copy party
};

const struct RogueEncounterData gRouteMiniBossEncounters = 
{
    .mapCount = ARRAY_COUNT(sRouteMiniBossEncounters),
    .mapTable = sRouteMiniBossEncounters
};

#undef ENCOUNTER_MAP