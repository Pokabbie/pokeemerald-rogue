#ifndef ROGUE_SETTINGS__H
#define ROGUE_SETTINGS__H

typedef void (*RogueDifficultyMenuCallback)(void);

struct GameModeRules
{
    u8 initialLevelOffset;
    u8 levelOffsetInterval;
    u8 enterPartySize;
    u8 adventureGenerator;
    u8 trainerOrder : 2;
    u8 disableMainQuests : 1;
    u8 disableChallengeQuests : 1;
    u8 disablePerBadgeLvlCaps : 1;
    u8 forceEndGameTrainers : 1;
    u8 forceEndGameRouteItems : 1;
    u8 forceRandomanAlwaysActive : 1;
    u8 disableRivalEncounters : 1;
    u8 disableRouteTrainers : 1;
    u8 forceFullShopInventory : 1;
};

void Rogue_SetConfigToggle(u16 elem, bool8 state);
bool8 Rogue_GetConfigToggle(u16 elem);

void Rogue_SetConfigRange(u16 elem, u8 value);
u8 Rogue_GetConfigRange(u16 elem);

bool8 Rogue_CanEditConfig();

struct GameModeRules const* Rogue_GetModeRules();
bool8 Rogue_ShouldDisableMainQuests();
bool8 Rogue_ShouldDisableChallengeQuests();

void RogueDebug_SetConfigToggle(u16 elem, bool8 state);
bool8 RogueDebug_GetConfigToggle(u16 elem);

void RogueDebug_SetConfigRange(u16 elem, u8 value);
u8 RogueDebug_GetConfigRange(u16 elem);

void Rogue_ResetSettingsToDefaults();
void Rogue_SetDifficultyPreset(u8 preset);
u8 Rogue_GetDifficultyPreset();
u8 Rogue_GetDifficultyRewardLevel();

u8 Rogue_GetStartingMonCapacity();

bool8 Rogue_ShouldSkipAssignNickname(struct Pokemon* mon);
bool8 Rogue_ShouldSkipAssignNicknameYesNoMessage();
bool8 Rogue_ShouldForceNicknameScreen();

// UI
void Rogue_OpenDifficultyConfigMenu(RogueDifficultyMenuCallback callback);
void CB2_InitDifficultyConfigMenu(void);

#endif