#include "global.h"
#include "constants/layouts.h"
#include "event_data.h"
#include "gba/isagbprint.h"
#include "random.h"
#include "string_util.h"

#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_gifts.h"
#include "rogue_multiplayer.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_quest.h"

#include "data/rogue/pokemon_nicknames.h"

STATIC_ASSERT(ARRAY_COUNT(sNicknameTable_Global) != 0, sNicknameTable_Global_IsntEmpty);

struct RogueDifficultyLocal
{
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
    struct RogueDifficultyPresetToggle toggles[CONFIG_TOGGLE_COUNT + 1];
    struct RogueDifficultyPresetRange ranges[CONFIG_RANGE_COUNT + 1];
};

static const struct GameModeRules sGameModeRules[ROGUE_GAME_MODE_COUNT] = 
{
    [ROGUE_GAME_MODE_STANDARD] = {}, // we should never have anything here, every rule/flag should default to off in standard
    [ROGUE_GAME_MODE_RAINBOW] = 
    {
        .trainerOrder = TRAINER_ORDER_RAINBOW,
    },
    [ROGUE_GAME_MODE_OFFICIAL] = 
    {
        .trainerOrder = TRAINER_ORDER_OFFICIAL,
        .disableChallengeQuests = TRUE,
    },
    [ROGUE_GAME_MODE_GAUNTLET] = 
    {
        .initialLevelOffset = 80,
        .levelOffsetInterval = 10,
        .enterPartySize = PARTY_SIZE,
        .trainerOrder = TRAINER_ORDER_DEFAULT,
        .disableChallengeQuests = TRUE,
        .disablePerBadgeLvlCaps = TRUE,
        .forceEndGameTrainers = TRUE,
        .forceEndGameRouteItems = TRUE,
        .forceRandomanAlwaysActive = TRUE,
        .disableRivalEncounters = TRUE,
        .disableRouteTrainers = TRUE,
        .forceFullShopInventory = TRUE,
        .adventureGenerator = ADV_GENERATOR_GAUNTLET,
    },
    [ROGUE_GAME_MODE_RAINBOW_GAUNTLET] = 
    {
        .initialLevelOffset = 80,
        .levelOffsetInterval = 10,
        .enterPartySize = PARTY_SIZE,
        .trainerOrder = TRAINER_ORDER_RAINBOW,
        .disableChallengeQuests = TRUE,
        .disablePerBadgeLvlCaps = TRUE,
        .forceEndGameTrainers = TRUE,
        .forceEndGameRouteItems = TRUE,
        .forceRandomanAlwaysActive = TRUE,
        .disableRivalEncounters = TRUE,
        .disableRouteTrainers = TRUE,
        .forceFullShopInventory = TRUE,
        .adventureGenerator = ADV_GENERATOR_GAUNTLET,
    },
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
            { .id=CONFIG_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=CONFIG_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_EASY },
            { .id=CONFIG_RANGE_ITEM, .value=DIFFICULTY_LEVEL_EASY },
            { .id=CONFIG_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_EASY },
            { .id=CONFIG_RANGE_COUNT },
        }
    },
    [DIFFICULTY_LEVEL_AVERAGE] = 
    {
        .toggles = 
        {
            { .id=CONFIG_TOGGLE_SWITCH_MODE, .value=FALSE },
            { .id=CONFIG_TOGGLE_AFFECTION, .value=FALSE },
            { .id=CONFIG_TOGGLE_RELEASE_MONS, .value=TRUE },
            { .id=CONFIG_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=CONFIG_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_AVERAGE },
            { .id=CONFIG_RANGE_ITEM, .value=DIFFICULTY_LEVEL_AVERAGE },
            { .id=CONFIG_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_AVERAGE },
            { .id=CONFIG_RANGE_COUNT },
        }
    },
    [DIFFICULTY_LEVEL_HARD] = 
    {
        .toggles = 
        {
            { .id=CONFIG_TOGGLE_OVER_LVL, .value=FALSE },
            { .id=CONFIG_TOGGLE_EV_GAIN, .value=FALSE },
            { .id=CONFIG_TOGGLE_SWITCH_MODE, .value=FALSE },
            { .id=CONFIG_TOGGLE_AFFECTION, .value=FALSE },
            { .id=CONFIG_TOGGLE_RELEASE_MONS, .value=TRUE },
            { .id=CONFIG_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=CONFIG_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_HARD },
            { .id=CONFIG_RANGE_ITEM, .value=DIFFICULTY_LEVEL_HARD },
            { .id=CONFIG_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_HARD },
            { .id=CONFIG_RANGE_COUNT },
        }
    },
    [DIFFICULTY_LEVEL_BRUTAL] = 
    {
        .toggles = 
        {
            { .id=CONFIG_TOGGLE_OVER_LVL, .value=FALSE },
            { .id=CONFIG_TOGGLE_EV_GAIN, .value=FALSE },
            { .id=CONFIG_TOGGLE_SWITCH_MODE, .value=FALSE },
            { .id=CONFIG_TOGGLE_AFFECTION, .value=FALSE },
            { .id=CONFIG_TOGGLE_RELEASE_MONS, .value=TRUE },
            { .id=CONFIG_TOGGLE_BAG_WIPE, .value=TRUE },
            { .id=CONFIG_TOGGLE_DIVERSE_TRAINERS, .value=TRUE },
            { .id=CONFIG_TOGGLE_COUNT },
        },
        .ranges = 
        {
            { .id=CONFIG_RANGE_TRAINER, .value=DIFFICULTY_LEVEL_BRUTAL },
            { .id=CONFIG_RANGE_ITEM, .value=DIFFICULTY_LEVEL_BRUTAL },
            { .id=CONFIG_RANGE_LEGENDARY, .value=DIFFICULTY_LEVEL_BRUTAL },
            { .id=CONFIG_RANGE_COUNT },
        }
    }
};

static struct RogueDifficultyConfig* GetWritableDifficultyConfig()
{
    return &gRogueSaveBlock->difficultyConfig;
}

static struct RogueDifficultyConfig const* GetReadableDifficultyConfig()
{
    if(RogueMP_IsActive() && RogueMP_IsClient())
    {
        AGB_ASSERT(gRogueMultiplayer != NULL);
        return &gRogueMultiplayer->gameState.hub.difficultyConfig;
    }

    return &gRogueSaveBlock->difficultyConfig;
}

static bool8 IsDifficultyToggle(u16 elem)
{
    switch (elem)
    {
    case CONFIG_TOGGLE_OVER_LVL:
    case CONFIG_TOGGLE_EV_GAIN:
    case CONFIG_TOGGLE_BAG_WIPE:
    case CONFIG_TOGGLE_SWITCH_MODE:
    case CONFIG_TOGGLE_DIVERSE_TRAINERS:
    case CONFIG_TOGGLE_AFFECTION:
    case CONFIG_TOGGLE_RELEASE_MONS:
        return TRUE;
    }

    return FALSE;
}

static bool8 IsDifficultyRange(u16 elem)
{
    switch (elem)
    {
    case CONFIG_RANGE_TRAINER:
    case CONFIG_RANGE_ITEM:
    case CONFIG_RANGE_LEGENDARY:
        return TRUE;
    }

    return FALSE;
}

void Rogue_SetConfigToggle(u16 elem, bool8 state)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;
    struct RogueDifficultyConfig* config = GetWritableDifficultyConfig();

    AGB_ASSERT(elem < CONFIG_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(config->toggleBits));

    if(elem < CONFIG_TOGGLE_COUNT)
    {
        if(state)
        {
            config->toggleBits[idx] |= bitMask;
        }
        else
        {
            config->toggleBits[idx] &= ~bitMask;
        }

        gRogueDifficultyLocal.areLevelsValid = FALSE;

        if(IsDifficultyToggle(elem))
            config->rangeValues[CONFIG_RANGE_DIFFICULTY_PRESET] = DIFFICULTY_LEVEL_CUSTOM;
    }
}

bool8 Rogue_GetConfigToggle(u16 elem)
{
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;
    struct RogueDifficultyConfig const* config = GetReadableDifficultyConfig();

    AGB_ASSERT(elem < CONFIG_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(config->toggleBits));
    return (config->toggleBits[idx] & bitMask) != 0;
}

void Rogue_SetConfigRange(u16 elem, u8 value)
{
    struct RogueDifficultyConfig* config = GetWritableDifficultyConfig();
    AGB_ASSERT(elem < CONFIG_RANGE_COUNT);

    if(elem < CONFIG_RANGE_COUNT)
    {
        config->rangeValues[elem] = value;
        gRogueDifficultyLocal.areLevelsValid = FALSE;

        if(IsDifficultyRange(elem))
            config->rangeValues[CONFIG_RANGE_DIFFICULTY_PRESET] = DIFFICULTY_LEVEL_CUSTOM;
    }
}

u8 Rogue_GetConfigRange(u16 elem)
{
    struct RogueDifficultyConfig const* config = GetReadableDifficultyConfig();
    AGB_ASSERT(elem < CONFIG_RANGE_COUNT);
    return config->rangeValues[elem];
}

bool8 Rogue_CanEditConfig()
{
    // Client can never change config
    if(RogueMP_IsActive() && RogueMP_IsClient())
        return FALSE;

    return !Rogue_IsRunActive();
}

struct GameModeRules const* Rogue_GetModeRules()
{
    u8 mode = Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM);
    AGB_ASSERT(mode < ROGUE_GAME_MODE_COUNT);
    return &sGameModeRules[mode];
}

bool8 Rogue_ShouldDisableMainQuests()
{
    struct AdventureReplay const* replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];

    if(Rogue_GetModeRules()->disableMainQuests)
        return TRUE;

    if(Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE) && replay->isValid)
        return TRUE;

    if(Rogue_IsRunActive() && AnyCharmsActive())
        return TRUE;
    
    return FALSE;
}

bool8 Rogue_ShouldDisableChallengeQuests()
{
    struct AdventureReplay const* replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];

    if(RogueQuest_HasUnlockedChallenges())
    {
        if(Rogue_GetModeRules()->disableChallengeQuests)
            return TRUE;

        if(Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE) && replay->isValid)
            return TRUE;

        if(Rogue_IsRunActive() && AnyCharmsActive())
            return TRUE;
    }
    
    return FALSE;
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
    return FALSE;
}

void RogueDebug_SetConfigRange(u16 elem, u8 value)
{
}

u8 RogueDebug_GetConfigRange(u16 elem)
{
    return 0;
}

#endif

static void Rogue_ResetToDefaults(bool8 difficultySettingsOnly)
{
    // Reset all values to the default prior to presets
    // These should be the lowest of the low
    gRogueDifficultyLocal.rewardLevel = DIFFICULTY_LEVEL_EASY;
    gRogueDifficultyLocal.areLevelsValid = FALSE;

    Rogue_SetConfigToggle(CONFIG_TOGGLE_OVER_LVL, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_EV_GAIN, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_BAG_WIPE, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SWITCH_MODE, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_AFFECTION, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_DIVERSE_TRAINERS, FALSE);

    // Set these all to the lowest
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_EASY);
    Rogue_SetConfigRange(CONFIG_RANGE_ITEM, DIFFICULTY_LEVEL_EASY);
    Rogue_SetConfigRange(CONFIG_RANGE_LEGENDARY, DIFFICULTY_LEVEL_EASY);

    if(!difficultySettingsOnly)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_OVERWORLD_MONS, TRUE);
        Rogue_SetConfigToggle(CONFIG_TOGGLE_EXP_ALL, TRUE);
        Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, BATTLE_FORMAT_SINGLES);
    }
}

static void Rogue_SetDifficultyPresetInternal(u8 preset)
{
    u8 i, j;
    const struct RogueDifficultyPresetToggle* toggle;
    const struct RogueDifficultyPresetRange* range;

    Rogue_ResetToDefaults(TRUE);

    for(i = 0; i < DIFFICULTY_PRESET_COUNT; ++i)
    {
        for(j = 0; j < ARRAY_COUNT(gRogueDifficultyPresets[i].toggles); ++j)
        {
            toggle = &gRogueDifficultyPresets[i].toggles[j];

            if(toggle->id == CONFIG_TOGGLE_COUNT)
                break;

            Rogue_SetConfigToggle(toggle->id, toggle->value);
        }

        for(j = 0; j < ARRAY_COUNT(gRogueDifficultyPresets[i].ranges); ++j)
        {
            range = &gRogueDifficultyPresets[i].ranges[j];

            if(range->id == CONFIG_RANGE_COUNT)
                break;

            Rogue_SetConfigRange(range->id, range->value);
        }

        // Have applied preset up until this point
        if(i == preset)
            break;
    }

    gRogueDifficultyLocal.rewardLevel = preset;
    gRogueDifficultyLocal.areLevelsValid = TRUE;
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

            if(toggle->id == CONFIG_TOGGLE_COUNT)
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

                if(range->id == CONFIG_RANGE_COUNT)
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


void Rogue_ResetSettingsToDefaults()
{
    Rogue_ResetToDefaults(FALSE);
    Rogue_SetDifficultyPreset(DIFFICULTY_LEVEL_AVERAGE);

    // Clear all here (Expect to be set later, if not will assert)
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO,  FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO,  FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN,  FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE,  FALSE);
#ifdef ROGUE_EXPANSION
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA,  FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS,  FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA,  FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR,  FALSE);
#endif
}

void Rogue_SetDifficultyPreset(u8 preset)
{
    Rogue_SetDifficultyPresetInternal(preset);
    Rogue_SetConfigRange(CONFIG_RANGE_DIFFICULTY_PRESET, preset);
}

static void EnsureLevelsAreValid()
{
    if(RogueMP_IsActive() && RogueMP_IsClient())
    {
        // Always assume we're invalid for now, as we don't have a nice way to tell when host changed the settings :/
        gRogueDifficultyLocal.areLevelsValid = FALSE;
    }

    if(!gRogueDifficultyLocal.areLevelsValid)
    {
        gRogueDifficultyLocal.rewardLevel = Rogue_CalcRewardDifficultyPreset();
        gRogueDifficultyLocal.areLevelsValid = TRUE;
    }
}

u8 Rogue_GetDifficultyPreset()
{
    EnsureLevelsAreValid();

    return Rogue_GetConfigRange(CONFIG_RANGE_DIFFICULTY_PRESET);
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

u8 Rogue_GetStartingMonCapacity()
{
    u8 partySize = 1;

    if(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT) == BATTLE_FORMAT_DOUBLES || Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT) == BATTLE_FORMAT_MIXED)
        partySize = 2;

    partySize = max(partySize, Rogue_GetModeRules()->enterPartySize);

    return partySize;
}

static u16 GetCurrentNicknameMode()
{
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL)
        return gSaveBlock2Ptr->optionsNicknameMode;

    if(Rogue_InWildSafari())
        return OPTIONS_NICKNAME_MODE_NEVER;

    return gSaveBlock2Ptr->optionsNicknameMode;
}

static bool8 DoesPartyContainNickname(u8 const* str)
{
    u8 i;
    u8 nickname[POKEMON_NAME_LENGTH];

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nickname);
        if(StringCompareN(str, nickname, POKEMON_NAME_LENGTH) == 0)
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_ShouldSkipAssignNickname(struct Pokemon* mon)
{
    u32 customMonId;

    // Never give snagged mons nicknames
    if(FlagGet(FLAG_ROGUE_IN_SNAG_BATTLE))
        return TRUE;

    // Don't give exotic mons nicknames
    customMonId = RogueGift_GetCustomMonId(mon);

    if(customMonId)
    {
        if(!RogueGift_CanRenameCustomMon(customMonId))
            return TRUE;
    }

    switch (GetCurrentNicknameMode())
    {
    case OPTIONS_NICKNAME_MODE_NEVER:
        return TRUE;

    case OPTIONS_NICKNAME_RANDOM:
        {
            u16 nicknameIdx;
            
            while(TRUE)
            {
                nicknameIdx = Random() % ARRAY_COUNT(sNicknameTable_Global);

                if(!DoesPartyContainNickname(sNicknameTable_Global[nicknameIdx]))
                    break;
            }

            SetMonData(mon, MON_DATA_NICKNAME, sNicknameTable_Global[nicknameIdx]);
            return TRUE;
        }
    }
    
    return FALSE;
}

bool8 Rogue_ShouldSkipAssignNicknameYesNoMessage(struct Pokemon* mon)
{
    switch (GetCurrentNicknameMode())
    {
    case OPTIONS_NICKNAME_MODE_ASK:
        return FALSE;
    }

    return TRUE;
}

bool8 Rogue_ShouldForceNicknameScreen()
{
    switch (GetCurrentNicknameMode())
    {
    case OPTIONS_NICKNAME_MODE_ALWAYS:
        return TRUE;
    }

    return FALSE;
}