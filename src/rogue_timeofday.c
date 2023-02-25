#include "global.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/event_objects.h"
#include "constants/heal_locations.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/rogue.h"
#include "constants/rgb.h"
#include "constants/weather.h"
#include "data.h"
#include "gba/isagbprint.h"

#include "battle.h"
#include "battle_setup.h"
#include "berry.h"
#include "event_data.h"
#include "graphics.h"
#include "item.h"
#include "load_save.h"
#include "main.h"
#include "money.h"
#include "overworld.h"
#include "party_menu.h"
#include "palette.h"
#include "play_time.h"
#include "player_pc.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "rtc.h"
#include "safari_zone.h"
#include "script.h"
#include "siirtc.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"

#include "rogue.h"
#include "rogue_timeofday.h"

// Time is measured in minutes
// 0 -> 1440

#define CALC_TIME(hour, minute) (hour * 60 + minute)
#define CALC_HOUR_FROM_TIME(time) (time / 60)
#define CALC_MINS_FROM_TIME(time) (time % 60 )

#define RGB_NIGHT RGB(9, 7, 13)
#define RGB_SUNRISE RGB(30, 18, 8)
#define RGB_DAYTIME RGB_WHITE
#define RGB_SUNSET RGB(27, 10, 3)

struct ToDPalette
{
    u16 time;
    u16 colour;
};

static const struct ToDPalette sToDPaletteLookup[] =
{
    {
        .time = CALC_TIME(5, 0),
        .colour = RGB_NIGHT,
    },
    {
        .time = CALC_TIME(6, 0),
        .colour = RGB_SUNRISE,
    },
    {
        .time = CALC_TIME(8, 0),
        .colour = RGB_DAYTIME,
    },

    {
        .time = CALC_TIME(19, 0),
        .colour = RGB_DAYTIME,
    },

    {
        .time = CALC_TIME(20, 0),
        .colour = RGB_SUNSET,
    },

    {
        .time = CALC_TIME(22, 0),
        .colour = RGB_NIGHT,
    },
};

EWRAM_DATA static u16 sTimeOfDayMinutes = 0; // Maybe should have this in IWRAM?

u16 RogueToD_GetTime()
{
    return sTimeOfDayMinutes;
}

void RogueToD_SetTime(u16 time)
{
    sTimeOfDayMinutes = time;
}

u16 RogueToD_GetHours()
{
    return CALC_HOUR_FROM_TIME(sTimeOfDayMinutes);
}

u16 RogueToD_GetMinutes()
{
    return CALC_MINS_FROM_TIME(sTimeOfDayMinutes);
}

u16 RogueToD_AddMinutes(u16 minutes)
{
    sTimeOfDayMinutes = (sTimeOfDayMinutes + minutes) % CALC_TIME(24, 00);
}

bool8 RogueToD_IsDawn()
{
    return sTimeOfDayMinutes >= CALC_TIME(6, 0) && sTimeOfDayMinutes < CALC_TIME(8, 0);
}

bool8 RogueToD_IsDay()
{
    return sTimeOfDayMinutes >= CALC_TIME(8, 0) && sTimeOfDayMinutes < CALC_TIME(20, 0);
}

bool8 RogueToD_IsDusk()
{
    return sTimeOfDayMinutes >= CALC_TIME(20, 0) && sTimeOfDayMinutes < CALC_TIME(22, 0);
}

bool8 RogueToD_IsNight()
{
    return sTimeOfDayMinutes >= CALC_TIME(22, 0) || sTimeOfDayMinutes < CALC_TIME(5, 0);
}

static u16 RGB_Lerp(u16 colA, u16 colB, u16 perc)
{
    u16 rA, gA, bA, rB, gB, bB;

    rA = GET_R(colA);
    gA = GET_G(colA);
    bA = GET_B(colA);

    rB = GET_R(colB);
    gB = GET_G(colB);
    bB = GET_B(colB);

    rA = (rA * (100 - perc) + rB * perc) / 100;
    gA = (gA * (100 - perc) + gB * perc) / 100;
    bA = (bA * (100 - perc) + bB * perc) / 100;

    return RGB(rA, gA, bA);
}

static u16 CalcColourForToD(u16 time)
{
    u16 i;

    // Check the bounds
    if(time < sToDPaletteLookup[0].time)
    {
        return sToDPaletteLookup[0].colour;
    }

    if(time >= sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].time)
    {
        return sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].colour;
    }

    // Blend other ranges
    for(i = 0; i < ARRAY_COUNT(sToDPaletteLookup) - 1; ++i)
    {
        const struct ToDPalette* palA = &sToDPaletteLookup[i];
        const struct ToDPalette* palB = &sToDPaletteLookup[i + 1];

        if(time >= palA->time && time < palB->time)
        {
            u16 t = ((time - palA->time) * 100) / (palB->time - palA->time);
            return RGB_Lerp(palA->colour, palB->colour, t);
        }
    }

    return RGB_WHITE;
}

static void TintPalette_CustomMultiply(u16 *palette, u16 count, u16 rTone, u16 gTone, u16 bTone)
{
    s32 r, g, b, i;
    u32 gray;

    for (i = 0; i < count; i++)
    {
        r = GET_R(*palette);
        g = GET_G(*palette);
        b = GET_B(*palette);

        r = (r * rTone) / 31;
        g = (g * gTone) / 31;
        b = (b * bTone) / 31;

        if (r > 31)
            r = 31;
        if (g > 31)
            g = 31;
        if (b > 31)
            b = 31;

        *palette++ = RGB2(r, g, b);
    }
}

static void TintPalette_ToD(u16 *palette, u16 count)
{
    u16 colour = CalcColourForToD(sTimeOfDayMinutes);
    TintPalette_CustomMultiply(palette, count, GET_R(colour), GET_G(colour), GET_B(colour));
}

void RogueToD_ModifyOverworldPalette(u16 offset, u16 count)
{
    TintPalette_ToD(&gPlttBufferUnfaded[offset], count * 16);
    CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], count * 16);
}
