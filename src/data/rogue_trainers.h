#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/pokemon.h"
#include "constants/weather.h"

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