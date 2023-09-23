#include "global.h"
//#include "constants/abilities.h"
//#include "constants/battle.h"
//#include "constants/event_objects.h"
//#include "constants/heal_locations.h"
//#include "constants/hold_effects.h"
//#include "constants/items.h"
//#include "constants/layouts.h"
//#include "constants/rogue.h"
//#include "constants/rgb.h"
//#include "constants/trainer_types.h"
//#include "constants/weather.h"
//#include "data.h"
#include "gba/isagbprint.h"

#include "rogue_save.h"
#include "rogue_settings.h"

struct RogueDifficultyLocal
{
    u8 presetLevel;
    u8 rewardLevel;
    bool8 areLevelsValid;
};

struct RogueDifficultyPresetToggle
{
    u8 id;
    bool8 value;
};

struct RogueDifficultyPresetRange
{
    u8 id;
    u8 value;
};

struct RogueDifficultyPreset
{
    struct RogueDifficultyPresetToggle toggles[DIFFICULTY_TOGGLE_COUNT + 1];
    struct RogueDifficultyPresetRange ranges[DIFFICULTY_RANGE_COUNT + 1];
};

EWRAM_DATA struct RogueDifficultyLocal gRogueDifficultyLocal;

#ifdef ROGUE_DEBUG
EWRAM_DATA struct RogueDebugConfig gRogueDebug = {0};
#endif

const struct RogueDifficultyPreset gRogueDifficultyPresets[DIFFICULTY_PRESET_COUNT] = 
{
    // Easy difficulty can actually have ANYTHING set, these values are just the defaults, recommendations
    // Easy difficulty provides all the default values
    [DIFFICULTY_LEVEL_EASY] = 
    {
        .toggles = 
        {
            // no required toggles
            { .id=DIFFICULTY_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=DIFFICULTY_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_EASY },
            { .id=DIFFICULTY_RANGE_ITEM, .value=DIFFICULTY_LEVEL_EASY },
            { .id=DIFFICULTY_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_EASY },
            { .id=DIFFICULTY_RANGE_COUNT },
        }
    },
    [DIFFICULTY_LEVEL_MEDIUM] = 
    {
        .toggles = 
        {
            { .id=DIFFICULTY_TOGGLE_SWITCH_MODE, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=DIFFICULTY_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_MEDIUM },
            { .id=DIFFICULTY_RANGE_ITEM, .value=DIFFICULTY_LEVEL_MEDIUM },
            { .id=DIFFICULTY_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_MEDIUM },
            { .id=DIFFICULTY_RANGE_COUNT },
        }
    },
    [DIFFICULTY_LEVEL_HARD] = 
    {
        .toggles = 
        {
            { .id=DIFFICULTY_TOGGLE_OVER_LVL, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_EV_GAIN, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_SWITCH_MODE, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=DIFFICULTY_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_HARD },
            { .id=DIFFICULTY_RANGE_ITEM, .value=DIFFICULTY_LEVEL_HARD },
            { .id=DIFFICULTY_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_HARD },
            { .id=DIFFICULTY_RANGE_COUNT },
        }
    },
    [DIFFICULTY_LEVEL_BRUTAL] = 
    {
        .toggles = 
        {
            { .id=DIFFICULTY_TOGGLE_OVER_LVL, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_EV_GAIN, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_SWITCH_MODE, .value=FALSE },
            { .id=DIFFICULTY_TOGGLE_BAG_WIPE, .value=TRUE },
            { .id=DIFFICULTY_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=DIFFICULTY_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_BRUTAL },
            { .id=DIFFICULTY_RANGE_ITEM, .value=DIFFICULTY_LEVEL_BRUTAL },
            { .id=DIFFICULTY_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_BRUTAL },
            { .id=DIFFICULTY_RANGE_COUNT },
        }
    }
};


void Rogue_SetConfigToggle(u16 elem, bool8 state)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < DIFFICULTY_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueSaveBlock->difficultyConfig.toggleBits));
    if(state)
    {
        gRogueSaveBlock->difficultyConfig.toggleBits[idx] |= bitMask;
    }
    else
    {
        gRogueSaveBlock->difficultyConfig.toggleBits[idx] &= ~bitMask;
    }

    gRogueDifficultyLocal.areLevelsValid = FALSE;
}

bool8 Rogue_GetConfigToggle(u16 elem)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < DIFFICULTY_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueSaveBlock->difficultyConfig.toggleBits));
    return (gRogueSaveBlock->difficultyConfig.toggleBits[idx] & bitMask) != 0;
}

void Rogue_SetConfigRange(u16 elem, u8 value)
{
    AGB_ASSERT(elem < DIFFICULTY_RANGE_COUNT);
    gRogueSaveBlock->difficultyConfig.rangeValues[elem] = value;
    gRogueDifficultyLocal.areLevelsValid = FALSE;
}

u8 Rogue_GetConfigRange(u16 elem)
{
    AGB_ASSERT(elem < DIFFICULTY_RANGE_COUNT);
    return gRogueSaveBlock->difficultyConfig.rangeValues[elem];
}

#ifdef ROGUE_DEBUG

static u16 GetDebugElementOffset(u16 elem)
{
    AGB_ASSERT(elem >= DEBUG_START_VALUE);
    elem -= DEBUG_START_VALUE;
    return elem;
}

void RogueDebug_SetConfigToggle(u16 e, bool8 state)
{
    u16 elem = GetDebugElementOffset(e);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < DEBUG_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueDebug.toggleBits));
    if(state)
    {
        gRogueDebug.toggleBits[idx] |= bitMask;
    }
    else
    {
        gRogueDebug.toggleBits[idx] &= ~bitMask;
    }
}

bool8 RogueDebug_GetConfigToggle(u16 e)
{
    u16 elem = GetDebugElementOffset(e);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < DEBUG_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueDebug.toggleBits));
    return (gRogueDebug.toggleBits[idx] & bitMask) != 0;
}

void RogueDebug_SetConfigRange(u16 elem, u8 value)
{
    elem = GetDebugElementOffset(elem);

    AGB_ASSERT(elem < DEBUG_RANGE_COUNT);
    gRogueDebug.rangeValues[elem] = value;
}

u8 RogueDebug_GetConfigRange(u16 elem)
{
    elem = GetDebugElementOffset(elem);

    AGB_ASSERT(elem < DEBUG_RANGE_COUNT);
    return gRogueDebug.rangeValues[elem];
}

#else

void RogueDebug_SetConfigToggle(u16 elem, bool8 state)
{
}

bool8 RogueDebug_GetConfigToggle(u16 elem)
{
    return 0;
}

void RogueDebug_SetConfigRange(u16 elem, u8 value)
{
}

u8 RogueDebug_GetConfigRange(u16 elem)
{
    return 0;
}

#endif

static void Rogue_ResetToDefaults()
{
    // Reset all values to the default prior to presets
    // These should be the lowest of the low
    gRogueDifficultyLocal.presetLevel = DIFFICULTY_LEVEL_CUSTOM;
    gRogueDifficultyLocal.rewardLevel = DIFFICULTY_LEVEL_EASY;
    gRogueDifficultyLocal.areLevelsValid = FALSE;

    Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_EXP_ALL, TRUE);
    Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_OVER_LVL, FALSE);
    Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN, TRUE);
    Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_OVERWORLD_MONS, TRUE);
    Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_BAG_WIPE, FALSE);
    Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_SWITCH_MODE, TRUE);

    // Set these all to the lowest
    Rogue_SetConfigRange(DIFFICULTY_RANGE_TRAINER, DIFFICULTY_LEVEL_EASY);
    Rogue_SetConfigRange(DIFFICULTY_RANGE_ITEM, DIFFICULTY_LEVEL_EASY);
    Rogue_SetConfigRange(DIFFICULTY_RANGE_LEGENDARY, DIFFICULTY_LEVEL_EASY);
}

static void Rogue_SetDifficultyPresetInternal(u8 preset)
{
    u8 i, j;
    const struct RogueDifficultyPresetToggle* toggle;
    const struct RogueDifficultyPresetRange* range;

    Rogue_ResetToDefaults();

    for(i = 0; i < DIFFICULTY_PRESET_COUNT; ++i)
    {
        for(j = 0; j < ARRAY_COUNT(gRogueDifficultyPresets[i].toggles); ++j)
        {
            toggle = &gRogueDifficultyPresets[i].toggles[j];

            if(toggle->id == DIFFICULTY_TOGGLE_COUNT)
                break;

            Rogue_SetConfigToggle(toggle->id, toggle->value);
        }

        for(j = 0; j < ARRAY_COUNT(gRogueDifficultyPresets[i].ranges); ++j)
        {
            range = &gRogueDifficultyPresets[i].ranges[j];

            if(range->id == DIFFICULTY_RANGE_COUNT)
                break;

            Rogue_SetConfigRange(range->id, range->value);
        }

        // Have applied preset up until this point
        if(i == preset)
            break;
    }

    gRogueDifficultyLocal.presetLevel = preset;
    gRogueDifficultyLocal.rewardLevel = preset;
    gRogueDifficultyLocal.areLevelsValid = TRUE;
}

static u8 Rogue_CalcDifficultyPreset()
{
    u8 p, i;
    bool8 isValid;

    for(p = 0; p < DIFFICULTY_PRESET_COUNT; ++p)
    {
        isValid = TRUE;

        if(isValid)
        {
            // Check if all toggles match
            for(i = 0; i < DIFFICULTY_TOGGLE_COUNT; ++i)
            {
                u8 id = gRogueDifficultyPresets[p].toggles[i].id;
                bool8 expectedValue = gRogueDifficultyPresets[p].toggles[i].value;

                if(id == DIFFICULTY_TOGGLE_COUNT)
                    break;

                if(Rogue_GetConfigToggle(id) != expectedValue)
                {
                    isValid = FALSE;
                    break;
                }
            }
        }

        if(isValid)
        {
            // Check if all ranges match
            for(i = 0; i < DIFFICULTY_RANGE_COUNT; ++i)
            {
                u8 id = gRogueDifficultyPresets[p].ranges[i].id;
                u8 expectedValue = gRogueDifficultyPresets[p].ranges[i].value;

                if(id == DIFFICULTY_RANGE_COUNT)
                    break;

                if(Rogue_GetConfigRange(id) != expectedValue)
                {
                    isValid = FALSE;
                    break;
                }
            }
        }

        if(isValid)
        {
            return p;
        }
    }

    return DIFFICULTY_LEVEL_CUSTOM;
}

static u8 Rogue_CalcRewardDifficultyPreset()
{
    u8 i, j, isValid;
    u8 rewardLevel;
    const struct RogueDifficultyPresetToggle* toggle;
    const struct RogueDifficultyPresetRange* range;

    rewardLevel = DIFFICULTY_LEVEL_EASY;

    for(i = 0; i < DIFFICULTY_PRESET_COUNT; ++i)
    {
        isValid = TRUE;

        for(j = 0; j < ARRAY_COUNT(gRogueDifficultyPresets[i].toggles); ++j)
        {
            toggle = &gRogueDifficultyPresets[i].toggles[j];

            if(toggle->id == DIFFICULTY_TOGGLE_COUNT)
                break;

            // We don't match
            if(Rogue_GetConfigToggle(toggle->id) != toggle->value)
            {
                isValid = FALSE;
                break;
            }
        }

        if(isValid)
        {
            for(j = 0; j < ARRAY_COUNT(gRogueDifficultyPresets[i].ranges); ++j)
            {
                range = &gRogueDifficultyPresets[i].ranges[j];

                if(range->id == DIFFICULTY_RANGE_COUNT)
                    break;

                // We don't AT LEAST match
                if(Rogue_GetConfigRange(range->id) < range->value)
                {
                    isValid = FALSE;
                    break;
                }
            }
        }

        if(!isValid)
            break;

        rewardLevel = i;
    }

    return rewardLevel;
}

void Rogue_SetDifficultyPreset(u8 preset)
{
    Rogue_SetDifficultyPresetInternal(preset);
}

static void EnsureLevelsAreValid()
{
    if(!gRogueDifficultyLocal.areLevelsValid)
    {
        gRogueDifficultyLocal.presetLevel = Rogue_CalcDifficultyPreset();
        gRogueDifficultyLocal.rewardLevel = Rogue_CalcRewardDifficultyPreset();
        gRogueDifficultyLocal.areLevelsValid = TRUE;
    }
}

u8 Rogue_GetDifficultyPreset()
{
    EnsureLevelsAreValid();

    return gRogueDifficultyLocal.presetLevel;
}

u8 Rogue_GetDifficultyRewardLevel()
{
    u8 preset;
    EnsureLevelsAreValid();

    preset = Rogue_GetDifficultyPreset();
    if(preset != DIFFICULTY_LEVEL_CUSTOM)
        return preset;

    return gRogueDifficultyLocal.rewardLevel;
}