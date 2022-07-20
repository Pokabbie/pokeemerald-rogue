#ifndef ROGUE_CONTROLLER_H
#define ROGUE_CONTROLLER_H

void RemoveAnyFaintedMons(void);

bool8 Rogue_IsRunActive(void);
bool8 Rogue_ForceExpAll(void);
void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain);
void Rogue_ModifyCatchRate(u8* catchRate, u8* ballMultiplier);

bool8 Rogue_ShouldShowMiniMenu(void);
u8* Rogue_GetMiniMenuContent(void);

void Rogue_OnNewGame(void);
void Rogue_GameClear(void);
void Rogue_SetDefaultOptions(void);
void Rogue_OnLoadMap(void);
void Rogue_OnWarpIntoMap(void);
void Rogue_OnSetWarpData(struct WarpData *warp);

void Rogue_Battle_StartTrainerBattle(void);
void Rogue_Battle_EndTrainerBattle(void);
//void Rogue_Battle_StartWildBattle(void);
void Rogue_Battle_EndWildBattle(void);

bool8 Rogue_OverrideTrainerItems(u16* items);
void Rogue_PreCreateTrainerParty(u16 trainerNum, bool8* useRogueCreateMon, u8* monsCount);
void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *party, u8 monIdx, u8 totalMonCount);
void Rogue_CreateWildMon(u8 area, u16* species, u8* level);
void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId);

#endif