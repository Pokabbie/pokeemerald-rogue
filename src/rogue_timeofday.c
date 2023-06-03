#include "global.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/event_objects.h"
#include "constants/heal_locations.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/map_types.h"
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

#define RGB_NIGHT           RGB(13, 11, 17)
#define RGB_NIGHT_BATTLE    RGB(14, 12, 18)
#define RGB_SUNRISE         RGB(31, 20, 10)
#define RGB_DAYTIME         RGB_WHITE
#define RGB_SUNSET          RGB(31, 18, 7)


#define RGB_255_CHANNEL(v) (u8)(((u32)v * (u32)31) / (u32)255)
#define RGB_255(r, g, b) RGB(RGB_255_CHANNEL(r), RGB_255_CHANNEL(g), RGB_255_CHANNEL(b))

#define DAYS_PER_SEASON     2 

struct ToDPalette
{
    u16 time;
    u16 overworldColour;
    u16 battleColour;
    u8 timeCode;
};

enum
{
    TIME_CODE_DAWN,
    TIME_CODE_DAY,
    TIME_CODE_DUSK,
    TIME_CODE_NIGHT,
};


static void RecalculateToDData(u16 time);

static const struct ToDPalette sToDPaletteLookup[] =
{
    {
        .time = CALC_TIME(3, 0),
        .overworldColour = RGB_NIGHT,
        .battleColour = RGB_NIGHT_BATTLE,
        .timeCode = TIME_CODE_NIGHT,
    },
    {
        .time = CALC_TIME(5, 0),
        .overworldColour = RGB_SUNRISE,
        .battleColour = RGB_SUNRISE,
        .timeCode = TIME_CODE_DAWN,
    },
    {
        .time = CALC_TIME(8, 0),
        .overworldColour = RGB_DAYTIME,
        .battleColour = RGB_DAYTIME,
        .timeCode = TIME_CODE_DAY,
    },

    {
        .time = CALC_TIME(19, 0),
        .overworldColour = RGB_DAYTIME,
        .battleColour = RGB_DAYTIME,
        .timeCode = TIME_CODE_DAY,
    },

    {
        .time = CALC_TIME(21, 0),
        .overworldColour = RGB_SUNSET,
        .battleColour = RGB_SUNSET,
        .timeCode = TIME_CODE_DUSK,
    },

    {
        .time = CALC_TIME(23, 0),
        .overworldColour = RGB_NIGHT,
        .battleColour = RGB_NIGHT_BATTLE,
        .timeCode = TIME_CODE_NIGHT,
    },
};

EWRAM_DATA static u16 sTimeOfDayMinutes = 0; // Maybe should have this in IWRAM?
EWRAM_DATA static u8 sTimeOfDayTimeCode = TIME_CODE_NIGHT;
EWRAM_DATA static u8 sSeasonCounter = 0;
EWRAM_DATA static u16 sTimeOfDayOverworldColour = 0;
EWRAM_DATA static u16 sTimeOfDayBattleColour = 0;

//extern const u16 gTilesetPalettes_General[][16];
extern const u16 gTilesetPalettes_General02_Spring[];
extern const u16 gTilesetPalettes_General02_Summer[];
extern const u16 gTilesetPalettes_General02_Autumn[];
extern const u16 gTilesetPalettes_General02_Winter[];

u16 RogueToD_GetTime()
{
    return sTimeOfDayMinutes;
}

void RogueToD_SetTime(u16 time)
{
    u16 prevMins = sTimeOfDayMinutes;
    sTimeOfDayMinutes = time % CALC_TIME(24, 00);

    // Just changed day
    if(prevMins > sTimeOfDayMinutes)
    {
        RogueToD_SetSeasonCounter(sSeasonCounter + 1);
    }

    RecalculateToDData(sTimeOfDayMinutes);
}

u8 RogueToD_GetSeason()
{
    u8 season = sSeasonCounter / DAYS_PER_SEASON;
    return min(season, SEASON_COUNT - 1);
}

void RogueToD_SetSeason(u8 season)
{
    season = min(season, SEASON_COUNT - 1);
    RogueToD_SetSeasonCounter(season * DAYS_PER_SEASON);
}

u8 RogueToD_GetSeasonCounter()
{
    return sSeasonCounter;
}

void RogueToD_SetSeasonCounter(u8 value)
{
    sSeasonCounter = (value) % (DAYS_PER_SEASON * SEASON_COUNT);
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
    RogueToD_SetTime(sTimeOfDayMinutes + minutes);
}

bool8 RogueToD_IsDawn()
{
    return sTimeOfDayTimeCode == TIME_CODE_DAWN;
}

bool8 RogueToD_IsDay()
{
    return sTimeOfDayTimeCode == TIME_CODE_DAY;
}

bool8 RogueToD_IsDusk()
{
    return sTimeOfDayTimeCode == TIME_CODE_DUSK;
}

bool8 RogueToD_IsNight()
{
    return sTimeOfDayTimeCode == TIME_CODE_NIGHT;
}

// Will only apply override palette if the input matches
static void TintPalette_CompareOverride(u16 *palette, u16 count, const u16* comparePalette, const u16* overridePalette)
{
    u16 i, j;
    u16 colour;

    for (i = 0; i < count; i++)
    {
        colour = *palette;

        for(j = 0; j < 16; ++j)
        {
            if(comparePalette[j] == colour)
            {
                colour = overridePalette[j];
                break;
            }
        }

        *palette++ = colour;
    }
}

static void TintPalette_CustomMultiply(u16 *palette, u16 count, u16 rTone, u16 gTone, u16 bTone)
{
    s32 r, g, b;
    u16 i, j;
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

static void TintPalette_Season(u16 *palette, u16 count)
{
    switch (RogueToD_GetSeason())
    {
    case SEASON_SPRING:
        break;
    case SEASON_SUMMER:
        TintPalette_CompareOverride(palette, count, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Summer);
        break;
    case SEASON_AUTUMN:
        TintPalette_CompareOverride(palette, count, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Autumn);
        break;
    case SEASON_WINTER:
        TintPalette_CompareOverride(palette, count, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Winter);
        break;
    }
}

static void TintPalette_ToD(u16 *palette, u16 count, u16 colour)
{
    TintPalette_CustomMultiply(palette, count, GET_R(colour), GET_G(colour), GET_B(colour));
}

static bool8 ShouldApplySeasonTintForCurrentMap()
{
    return gSaveBlock2Ptr->seasonVisuals && gMapHeader.mapType != MAP_TYPE_INDOOR;
}

static bool8 ShouldApplyTodTintForCurrentMap()
{
    return gSaveBlock2Ptr->timeOfDayVisuals && gMapHeader.mapType != MAP_TYPE_INDOOR;
}

static u16 GetDesiredTintForCurrentMap(u16 inTint, bool8 isOverworld)
{
    if(gMapHeader.cave || gMapHeader.mapType == MAP_TYPE_UNDERGROUND)
    {
        return isOverworld ? RGB(21, 21, 21) : RGB_WHITE;
    }

    return inTint;
}

void RogueToD_ModifyOverworldPalette(u16 offset, u16 count)
{
    bool8 isObjectPal = (offset / 16 + count) >= 16;
    bool8 isDirty = FALSE;

    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS && isObjectPal)
    {
        // We don't want to tint the overworld sprites in the adventure paths screen
        return;
    }

    if(ShouldApplySeasonTintForCurrentMap())
    {
        TintPalette_Season(&gPlttBufferUnfaded[offset], count * 16);
        isDirty = TRUE;
    }

    if(ShouldApplyTodTintForCurrentMap())
    {
        TintPalette_ToD(&gPlttBufferUnfaded[offset], count * 16, GetDesiredTintForCurrentMap(sTimeOfDayOverworldColour, TRUE));
        isDirty = TRUE;
    }

    if(isDirty)
    {
        CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], count * 16);
    }
}

void RogueToD_ModifyBattlePalette(u16 offset, u16 count)
{
    bool8 isDirty = FALSE;

    if(ShouldApplyTodTintForCurrentMap())
    {
        TintPalette_ToD(&gPlttBufferUnfaded[offset], count * 16, GetDesiredTintForCurrentMap(sTimeOfDayBattleColour, TRUE));
        isDirty = TRUE;
    }

    if(isDirty)
    {
        CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], count * 16);
    }
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

static void RecalculateToDData(u16 time)
{
    // Check the bounds
    if(time < sToDPaletteLookup[0].time)
    {
        sTimeOfDayOverworldColour = sToDPaletteLookup[0].overworldColour;
        sTimeOfDayBattleColour = sToDPaletteLookup[0].battleColour;

        sTimeOfDayTimeCode = sToDPaletteLookup[0].timeCode;
    }
    else if(time >= sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].time)
    {
        sTimeOfDayOverworldColour = sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].overworldColour;
        sTimeOfDayBattleColour = sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].battleColour;

        sTimeOfDayTimeCode = sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].timeCode;
    }
    // Blend other ranges
    else
    {
        u16 i;
        for(i = 0; i < ARRAY_COUNT(sToDPaletteLookup) - 1; ++i)
        {
            const struct ToDPalette* palA = &sToDPaletteLookup[i];
            const struct ToDPalette* palB = &sToDPaletteLookup[i + 1];

            if(time >= palA->time && time < palB->time)
            {
                u16 t = ((time - palA->time) * 100) / (palB->time - palA->time);
                sTimeOfDayOverworldColour = RGB_Lerp(palA->overworldColour, palB->overworldColour, t);
                sTimeOfDayBattleColour = RGB_Lerp(palA->battleColour, palB->battleColour, t);

                sTimeOfDayTimeCode = palA->timeCode;
                break;
            }
        }
    }
}