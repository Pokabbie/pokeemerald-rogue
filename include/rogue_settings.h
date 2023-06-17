#ifndef ROGUE_SETTINGS__H
#define ROGUE_SETTINGS__H

void Rogue_SetConfigToggle(u16 elem, bool8 state);
bool8 Rogue_GetConfigToggle(u16 elem);

void Rogue_SetConfigRange(u16 elem, u8 value);
u8 Rogue_GetConfigRange(u16 elem);

void Rogue_SetDifficultyPreset(u8 preset);
u8 Rogue_GetDifficultyPreset();
u8 Rogue_GetDifficultyRewardLevel();

// UI
void CB2_InitDifficultyConfigMenu(void);

#endif