#ifndef ROGUE_TIMEOFDAY__H
#define ROGUE_TIMEOFDAY__H

u16 RogueToD_GetTime();
void RogueToD_SetTime(u16 time);

u16 RogueToD_GetHours();
u16 RogueToD_GetMinutes();
u16 RogueToD_AddMinutes(u16 minutes);

bool8 RogueToD_IsDawn();
bool8 RogueToD_IsDay();
bool8 RogueToD_IsDusk();
bool8 RogueToD_IsNight();

void RogueToD_ModifyOverworldPalette(u16 offset, u16 count);
void RogueToD_ModifyBattlePalette(u16 offset, u16 count);

#endif