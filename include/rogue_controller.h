#ifndef ROGUE_CONTROLLER_H
#define ROGUE_CONTROLLER_H

bool8 Rogue_IsRunActive(void);
bool8 Rogue_ForceExpAll(void);

void Rogue_OnNewGame(void);
void Rogue_OnLoadMap(void);
void Rogue_OnWarpIntoMap(void);
void Rogue_OnSetWarpData(struct WarpData *warp);

void Rogue_Battle_StartTrainerBattle(void);

void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *mon, u16 species, u8 level, u8 fixedIV, u8 hasFixedPersonality, u32 fixedPersonality, u8 otIdType, u32 fixedOtId);
void Rogue_CreateWildMon(u8 area, u16* species, u8* level);

#endif