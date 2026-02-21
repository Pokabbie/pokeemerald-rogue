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
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_player_customisation.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_timeofday.h"

// Time is measured in minutes
// 0 -> 1440

#define CALC_TIME(hour, minute) (hour * 60 + minute)
#define CALC_HOUR_FROM_TIME(time) (time / 60)
#define CALC_MINS_FROM_TIME(time) (time % 60)

#define RGB_NIGHT           RGB(9, 15, 21)
#define RGB_SUNRISE         RGB(31, 19, 12)
#define RGB_DAYTIME         RGB_WHITE
#define RGB_SUNSET          RGB(31, 16, 16)

#define DAYS_PER_SEASON     2 

struct ToDPalette
{
    u16 time;
    u16 overworldColour;
    u16 battleColour;
    u8 timeCode;
};

struct ToDData
{
    u16 overworldColour;
    u16 battleColour;
    u8 timeCode;
    bool8 areCalcsValid : 1;
    bool8 timeVisualsTempDisabled : 1;
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

EWRAM_DATA static struct ToDData sTimeOfDay = {0};

//extern const u16 gTilesetPalettes_General[][16];
extern const u16 gTilesetPalettes_General02_Spring[];
extern const u16 gTilesetPalettes_General02_Summer[];
extern const u16 gTilesetPalettes_General02_Autumn[];
extern const u16 gTilesetPalettes_General02_Winter[];

extern const u16 gTilesetPalettes_General02_Day[];
extern const u16 gTilesetPalettes_General02_Night[];

u16 RogueToD_GetTime()
{
    return gRogueSaveBlock->timeOfDayMinutes;
}

void RogueToD_SetTime(u16 time)
{
    u16 prevMins = gRogueSaveBlock->timeOfDayMinutes;
    gRogueSaveBlock->timeOfDayMinutes = time % CALC_TIME(24, 00);

    //RogueHub_UpdateWeatherState

    // Just changed day
    if(prevMins > gRogueSaveBlock->timeOfDayMinutes)
    {
        RogueToD_SetSeasonCounter(gRogueSaveBlock->seasonCounter + 1);

        if(!Rogue_IsRunActive())
        {
            RogueHub_UpdateWeatherState();
            RogueHub_OnNewDayStarted();
        }
    }
    else if(prevMins < CALC_TIME(12, 0) && gRogueSaveBlock->timeOfDayMinutes >= CALC_TIME(12, 0))
    {
        // Each half day update the weather state
        if(!Rogue_IsRunActive())
            RogueHub_UpdateWeatherState();
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

u8 RogueToD_GetVisualSeason()
{
    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_ROUTE_SINNOH_217:
    case LAYOUT_ROGUE_ROUTE_SINNOH_MT_CORONET:
        return SEASON_WINTER;
    }

    return RogueToD_GetSeason();
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

void RogueToD_AddMinutes(u16 minutes)
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
static void TintPalette_CompareOverride(u16 *palette, u16 size, const u16* comparePalette, const u16* overridePalette)
{
    u16 i, j;
    u16 colour;

    for (i = 0; i < size / sizeof(u16); i++)
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

static void TintPalette_CustomMultiply(u16 *palette, u16 size, u16 rTone, u16 gTone, u16 bTone)
{
    s32 r, g, b;
    u16 i;

    for (i = 0; i < size / sizeof(u16); i++)
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

// Will only apply override palette if the input matches
static void UNUSED TintPalette_CompareOverrideWithMultiplyFallback(u16 *palette, u16 size, const u16* comparePalette, const u16* overridePalette,  u16 rTone, u16 gTone, u16 bTone)
{
    s32 r, g, b;
    u16 i, j;
    u16 colour;

    for (i = 0; i < size / sizeof(u16); i++)
    {
        colour = *palette;

        for(j = 0; j < 16; ++j)
        {
            // We have a valid override to apply (e.g. lights)
            if(comparePalette[j] == colour)
            {
                colour = overridePalette[j];
                break;
            }
        }

        // Apply tone fallback
        if(j == 16)
        {
            r = GET_R(colour);
            g = GET_G(colour);
            b = GET_B(colour);

            r = (r * rTone) / 31;
            g = (g * gTone) / 31;
            b = (b * bTone) / 31;

            if (r > 31)
                r = 31;
            if (g > 31)
                g = 31;
            if (b > 31)
                b = 31;

            colour = RGB2(r, g, b);
        }

        *palette++ = colour;
    }
}

static void TintPalette_Season(u16 *palette, u16 size)
{
    switch (RogueToD_GetVisualSeason())
    {
    case SEASON_SPRING:
        break;
    case SEASON_SUMMER:
        TintPalette_CompareOverride(palette, size, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Summer);
        break;
    case SEASON_AUTUMN:
        TintPalette_CompareOverride(palette, size, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Autumn);
        break;
    case SEASON_WINTER:
        TintPalette_CompareOverride(palette, size, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Winter);
        break;
    }
}

static void TintPalette_ToD(u16 *palette, u16 size, u16 colour)
{
    // Glowing water :(
    //if(RogueToD_IsNight())
    //{
    //    TintPalette_CompareOverrideWithMultiplyFallback(
    //        palette, size, 
    //        gTilesetPalettes_General02_Day, gTilesetPalettes_General02_Night,
    //        GET_R(colour), GET_G(colour), GET_B(colour)
    //    );
    //}
    //else
    {
        TintPalette_CustomMultiply(palette, size, GET_R(colour), GET_G(colour), GET_B(colour));
    }
}

bool8 RogueToD_ApplySeasonVisuals()
{
    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_ROUTE_SINNOH_217:
    case LAYOUT_ROGUE_ROUTE_SINNOH_MT_CORONET:
        return SEASON_WINTER;

    // Force on for credits
    case LAYOUT_ROGUE_BOSS_VICTORY_LAP:
        return TRUE;
    }


    return gSaveBlock2Ptr->seasonVisuals && gMapHeader.mapType != MAP_TYPE_INDOOR;
}

bool8 RogueToD_ApplyTimeVisuals()
{
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS)
    {
        // Time of day is bad for visibility on adventure paths screen so just disable it
        return FALSE;
    }

    return gSaveBlock2Ptr->timeOfDayVisuals && gMapHeader.mapType != MAP_TYPE_INDOOR && !sTimeOfDay.timeVisualsTempDisabled;
}

bool8 RogueToD_ApplyWeatherVisuals()
{
    return gSaveBlock2Ptr->weatherVisuals;
}

void RogueToD_SetTempDisableTimeVisuals(bool8 state)
{
    sTimeOfDay.timeVisualsTempDisabled = state;
}

static u16 GetDesiredTintForCurrentMap(u16 inTint, bool8 isOverworld)
{
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR))
        return RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY);

    if(gMapHeader.cave || gMapHeader.mapType == MAP_TYPE_UNDERGROUND)
    {
        return isOverworld ? RGB(18, 16, 22) : RGB_WHITE;
    }

    return inTint;
}

void RogueToD_ModifyOverworldPalette(u16 offset, u16 size)
{
    bool8 isObjectPal = offset >= OBJ_PLTT_ID(0);
    bool8 isDirty = FALSE;

    //if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS && isObjectPal)
    //{
    //    // We don't want to tint the overworld sprites in the adventure paths screen
    //    return;
    //}

    if(RogueToD_ApplySeasonVisuals())
    {
        TintPalette_Season(&gPlttBufferUnfaded[offset], size);
        isDirty = TRUE;
    }

    if(RogueToD_ApplyTimeVisuals())
    {
        RecalculateToDDataIfRequired();
        TintPalette_ToD(&gPlttBufferUnfaded[offset], size, GetDesiredTintForCurrentMap(sTimeOfDay.overworldColour, TRUE));
        isDirty = TRUE;
    }

    if(isDirty)
    {
        CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], size);
    }
}

void RogueToD_ModifyBattlePalette(u16 offset, u16 size)
{
    bool8 isDirty = FALSE;

    if(RogueToD_ApplyTimeVisuals())
    {
        RecalculateToDDataIfRequired();
        TintPalette_ToD(&gPlttBufferUnfaded[offset], size, GetDesiredTintForCurrentMap(sTimeOfDay.battleColour, FALSE));
        isDirty = TRUE;
    }

    if(isDirty)
    {
        CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], size);
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