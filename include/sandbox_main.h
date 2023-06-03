#ifndef SANDBOX_MAIN_H
#define SANDBOX_MAIN_H

bool8 Sandbox_UseFastBattleAnims();
u16 Sandbox_ModifyBattleWaitTime(u16 waitTime, bool8 awaitingMessage);
s16 Sandbox_ModifyBattleSlideAnim(s16 speed);

const void* Sandbox_ModifyLoadPalette(const void *src);
const void* Sandbox_ModifyLoadCompressedPalette(const void *src);

u32 Sandbox_GetTrainerAIFlags(u16 trainerNum);

void Sandbox_ModifyOverworldPalette(u16 offset, u16 count);

#endif