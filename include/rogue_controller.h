#ifndef ROGUE_CONTROLLER_H
#define ROGUE_CONTROLLER_H

#define ROGUE_DEBUG

extern struct RogueRunData gRogueRun;
extern struct RogueHubData gRogueSaveData;

bool8 Rogue_IsRunActive(void);
bool8 Rogue_ForceExpAll(void);
void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain);
void Rogue_ModifyCatchRate(u8* catchRate, u8* ballMultiplier);

bool8 Rogue_ShouldShowMiniMenu(void);
u8* Rogue_GetMiniMenuContent(void);

void Rogue_OnNewGame(void);
void Rogue_SetDefaultOptions(void);
void Rogue_OnLoadMap(void);
void Rogue_OnWarpIntoMap(void);
void Rogue_OnSetWarpData(struct WarpData *warp);

void Rogue_Battle_StartTrainerBattle(void);
void Rogue_Battle_EndTrainerBattle(void);
//void Rogue_Battle_StartWildBattle(void);
void Rogue_Battle_EndWildBattle(void);

void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *mon, u16 species, u8 level, u8 fixedIV, u8 hasFixedPersonality, u32 fixedPersonality, u8 otIdType, u32 fixedOtId);
void Rogue_CreateWildMon(u8 area, u16* species, u8* level);

#endif