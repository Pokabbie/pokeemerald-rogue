#ifndef ROGUE_SETTINGS__H
#define ROGUE_SETTINGS__H

typedef void (*RogueDifficultyMenuCallback)(void);

void Rogue_SetConfigToggle(u16 elem, bool8 state);
bool8 Rogue_GetConfigToggle(u16 elem);

void Rogue_SetConfigRange(u16 elem, u8 value);
u8 Rogue_GetConfigRange(u16 elem);

bool8 Rogue_CanEditConfig();

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

// UI
void Rogue_OpenDifficultyConfigMenu(RogueDifficultyMenuCallback callback);
void CB2_InitDifficultyConfigMenu(void);

#endif