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
#include "rogue_player_customisation.h"
#include "rogue_save.h"
#include "rogue_timeofday.h"

// Time is measured in minutes
// 0 -> 1440

#define CALC_TIME(hour, minute) (hour * 60 + minute)
#define CALC_HOUR_FROM_TIME(time) (time / 60)
#define CALC_MINS_FROM_TIME(time) (time % 60)

#define RGB_NIGHT           RGB(9, 12, 19)
#define RGB_SUNRISE         RGB(31, 16, 6)
#define RGB_DAYTIME         RGB_WHITE
#define RGB_SUNSET          RGB(31, 12, 9)

#define DAYS_PER_SEASON     2 

struct ToDPalette
{
    u16 time;
    u16 overworldColour;
    u16 battleColour;
    u8 timeCode;
};

struct TODData
{
    bool8 areCalcsValid;
    u8 timeCode;
    u16 overworldColour;
    u16 battleColour;
};
enum
{
    TIME_CODE_DAWN,
    TIME_CODE_DAY,
    TIME_CODE_DUSK,
    TIME_CODE_NIGHT,
    TIME_CODE_COUNT,
};


static void RecalculateToDDataIfRequired();

static const struct ToDPalette sToDPaletteLookup[] =
{
    {
        .time = CALC_TIME(5, 0),
        .overworldColour = RGB_NIGHT,
        .battleColour = RGB_NIGHT,
        .timeCode = TIME_CODE_NIGHT,
    },
    {
        .time = CALC_TIME(6, 0),
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
        .battleColour = RGB_NIGHT,
        .timeCode = TIME_CODE_NIGHT,
    },
};

EWRAM_DATA static u8 sTimeOfDayTimeCode = TIME_CODE_NIGHT;
EWRAM_DATA static u16 sTimeOfDayOverworldColour = 0;
EWRAM_DATA static u16 sTimeOfDayBattleColour = 0;

EWRAM_DATA static struct TODData sTimeOfDay = {0};

//extern const u16 gTilesetPalettes_General[][16];
extern const u16 gTilesetPalettes_General02_Spring[];
extern const u16 gTilesetPalettes_General02_Summer[];
extern const u16 gTilesetPalettes_General02_Autumn[];
extern const u16 gTilesetPalettes_General02_Winter[];

u16 RogueToD_GetTime()
{
    return gRogueSaveBlock->timeOfDayMinutes;
}

void RogueToD_SetTime(u16 time)
{
    u16 prevMins = gRogueSaveBlock->timeOfDayMinutes;
    gRogueSaveBlock->timeOfDayMinutes = time % CALC_TIME(24, 00);

    // Just changed day
    if(prevMins > gRogueSaveBlock->timeOfDayMinutes)
    {
        RogueToD_SetSeasonCounter(gRogueSaveBlock->seasonCounter + 1);
    }

    sTimeOfDay.areCalcsValid = FALSE;
}

u8 RogueToD_GetSeason()
{
    u8 season = gRogueSaveBlock->seasonCounter / DAYS_PER_SEASON;
    return min(season, SEASON_COUNT - 1);
}

void RogueToD_SetSeason(u8 season)
{
    season = min(season, SEASON_COUNT - 1);
    RogueToD_SetSeasonCounter(season * DAYS_PER_SEASON);
}

u8 RogueToD_GetSeasonCounter()
{
    return gRogueSaveBlock->seasonCounter;
}

void RogueToD_SetSeasonCounter(u8 value)
{
    gRogueSaveBlock->seasonCounter = (value) % (DAYS_PER_SEASON * SEASON_COUNT);
}

void RogueToD_SetTimePreset(u8 time, u8 season)
{
    u8 i;

    if(time < TIME_CODE_COUNT)
    {
        // Skip first as it's the end of night not the start
        for(i = 1; i < ARRAY_COUNT(sToDPaletteLookup); ++i)
        {
            if(sToDPaletteLookup[i].timeCode == time)
            {
                RogueToD_SetTime(sToDPaletteLookup[i].time);
                break;
            }
        }
    }

    if(season < SEASON_COUNT)
    {
        RogueToD_SetSeason(season);
    }
}

u16 RogueToD_GetHours()
{
    return CALC_HOUR_FROM_TIME(gRogueSaveBlock->timeOfDayMinutes);
}

u16 RogueToD_GetMinutes()
{
    return CALC_MINS_FROM_TIME(gRogueSaveBlock->timeOfDayMinutes);
}

u16 RogueToD_AddMinutes(u16 minutes)
{
    RogueToD_SetTime(gRogueSaveBlock->timeOfDayMinutes + minutes);
}

bool8 RogueToD_IsDawn()
{
    return sTimeOfDay.timeCode == TIME_CODE_DAWN;
}

bool8 RogueToD_IsDay()
{
    return sTimeOfDay.timeCode == TIME_CODE_DAY;
}

bool8 RogueToD_IsDusk()
{
    return sTimeOfDay.timeCode == TIME_CODE_DUSK;
}

bool8 RogueToD_IsNight()
{
    return sTimeOfDay.timeCode == TIME_CODE_NIGHT;
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

bool8 RogueToD_ApplySeasonVisuals()
{
    return gSaveBlock2Ptr->seasonVisuals && gMapHeader.mapType != MAP_TYPE_INDOOR;
}

bool8 RogueToD_ApplyTimeVisuals()
{
    return gSaveBlock2Ptr->timeOfDayVisuals && gMapHeader.mapType != MAP_TYPE_INDOOR;
}

static u16 GetDesiredTintForCurrentMap(u16 inTint, bool8 isOverworld)
{
#if defined(ROGUE_DEBUG) && defined(ROGUE_DEBUG_TOD_TINT_USES_PLAYER_COLOUR)
    return RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY);
#else
    if(gMapHeader.cave || gMapHeader.mapType == MAP_TYPE_UNDERGROUND)
    {
        return isOverworld ? RGB(18, 16, 22) : RGB_WHITE;
    }

    return inTint;
#endif
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

    if(RogueToD_ApplySeasonVisuals())
    {
        TintPalette_Season(&gPlttBufferUnfaded[offset], count * 16);
        isDirty = TRUE;
    }

    if(RogueToD_ApplyTimeVisuals())
    {
        RecalculateToDDataIfRequired();
        TintPalette_ToD(&gPlttBufferUnfaded[offset], count * 16, GetDesiredTintForCurrentMap(sTimeOfDay.overworldColour, TRUE));
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

    if(RogueToD_ApplyTimeVisuals())
    {
        RecalculateToDDataIfRequired();
        TintPalette_ToD(&gPlttBufferUnfaded[offset], count * 16, GetDesiredTintForCurrentMap(sTimeOfDay.battleColour, FALSE));
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

static void RecalculateToDDataIfRequired()
{
    if(sTimeOfDay.areCalcsValid != TRUE)
    {
        sTimeOfDay.areCalcsValid = TRUE;

        // Check the bounds
        if(gRogueSaveBlock->timeOfDayMinutes < sToDPaletteLookup[0].time)
        {
            sTimeOfDay.overworldColour = sToDPaletteLookup[0].overworldColour;
            sTimeOfDay.battleColour = sToDPaletteLookup[0].battleColour;

            sTimeOfDay.timeCode = sToDPaletteLookup[0].timeCode;
        }
        else if(gRogueSaveBlock->timeOfDayMinutes >= sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].time)
        {
            sTimeOfDay.overworldColour = sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].overworldColour;
            sTimeOfDay.battleColour = sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].battleColour;

            sTimeOfDay.timeCode = sToDPaletteLookup[ARRAY_COUNT(sToDPaletteLookup) - 1].timeCode;
        }
        // Blend other ranges
        else
        {
            u16 i;
            for(i = 0; i < ARRAY_COUNT(sToDPaletteLookup) - 1; ++i)
            {
                const struct ToDPalette* palA = &sToDPaletteLookup[i];
                const struct ToDPalette* palB = &sToDPaletteLookup[i + 1];

                if(gRogueSaveBlock->timeOfDayMinutes >= palA->time && gRogueSaveBlock->timeOfDayMinutes < palB->time)
                {
                    u16 t = ((gRogueSaveBlock->timeOfDayMinutes - palA->time) * 100) / (palB->time - palA->time);
                    sTimeOfDay.overworldColour = RGB_Lerp(palA->overworldColour, palB->overworldColour, t);
                    sTimeOfDay.battleColour = RGB_Lerp(palA->battleColour, palB->battleColour, t);

                    sTimeOfDay.timeCode = palA->timeCode;
                    break;
                }
            }
        }
    }
}