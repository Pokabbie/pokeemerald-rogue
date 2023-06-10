#ifndef ROGUE_CONTROLLER_H
#define ROGUE_CONTROLLER_H

#include "rogue_baked.h"

extern EWRAM_DATA struct RogueRunData gRogueRun;
extern EWRAM_DATA struct RogueHubData gRogueHubData;
extern EWRAM_DATA struct RogueGlobalData gRogueGlobalData;

#define OVERWORLD_FLAG 0

u16 RogueRandomRange(u16 range, u8 seedFlag);
bool8 RogueRandomChance(u8 chance, u16 seedFlag);
u16 Rogue_GetStartSeed(void);

u16 Rogue_GetShinyOdds(void);

void RemoveMonAtSlot(u8 slot, bool8 keepItems, bool8 shiftUpwardsParty, bool8 canSendToLab);
void RemoveAnyFaintedMons(bool8 keepItems, bool8 canSendToLab);

bool8 Rogue_IsRunActive(void);
bool8 Rogue_ForceExpAll(void);
bool8 Rogue_FastBattleAnims(void);
bool8 Rogue_UseKeyBattleAnims(void);
bool8 Rogue_GetBattleAnimsEnabled(void);
void Rogue_ModifyBattleWaitTime(u16* waitTime, bool8 awaitingMessage);
s16 Rogue_ModifyBattleSlideAnim(s16 rate);
void Rogue_ModifyBattleWinnings(u16 trainerNum, u32* money);
void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain);
void Rogue_ModifyEVGain(int* multiplier);
void Rogue_ModifyCatchRate(u16* catchRate, u16* ballMultiplier);
void Rogue_ModifyCaughtMon(struct Pokemon *mon);

const u8* Rogue_ModifyPallete8(const u8* input);
const u16* Rogue_ModifyPallete16(const u16* input);
const u32* Rogue_ModifyPallete32(const u32* input);

void Rogue_ModifyOverworldPalette(u16 offset, u16 count);
void Rogue_ModifyBattlePalette(u16 offset, u16 count);

bool8 Rogue_ShouldShowMiniMenu(void);
u16 Rogue_MiniMenuHeight(void);
u8* Rogue_GetMiniMenuContent(void);
void Rogue_CreateMiniMenuExtraGFX(void);
void Rogue_RemoveMiniMenuExtraGFX(void);

u8 SpeciesToGen(u16 species);
u8 ItemToGen(u16 item);
bool8 IsGenEnabled(u8 gen);

bool8 IsMegaEvolutionEnabled(void);
bool8 IsZMovesEnabled(void);
bool8 IsDynamaxEnabled(void);

bool8 CheckPresetMonFlags(u16 species, u32 flag);

void Rogue_ResetConfigHubSettings(void);
void Rogue_OnNewGame(void);
void Rogue_GameClear(void);
void Rogue_SetDefaultOptions(void);
void Rogue_OnSaveGame(void);
void Rogue_OnLoadGame(void);
bool8 Rogue_OnProcessPlayerFieldInput(void);
bool8 Rogue_IsPartnerMonInTeam(void);

u16 Rogue_PostRunRewardLvls();
u16 Rogue_PostRunRewardMoney();

void Rogue_MainInit(void);
void Rogue_MainCB(void);
void Rogue_OverworldCB(void);

void Rogue_OnSpawnObjectEvent(struct ObjectEvent *objectEvent);
void Rogue_OnRemoveObjectEvent(struct ObjectEvent *objectEvent);

u16 Rogue_GetHotTrackingData(u16* count, u16* average, u16* min, u16* max);

void Rogue_OnLoadMap(void);
void Rogue_OnWarpIntoMap(void);
void Rogue_OnSetWarpData(struct WarpData *warp);
void Rogue_ModifyMapHeader(struct MapHeader *mapHeader);

void Rogue_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp);
bool8 Rogue_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection);
void Rogue_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity);
u8 Rogue_SelectRouteRoom(void);
u8 Rogue_SelectLegendaryEncounterRoom(void);
u16 Rogue_SelectWildDenEncounterRoom(void);

void Rogue_SelectMiniBossRewardMons();

void Rogue_Battle_StartTrainerBattle(void);
void Rogue_Battle_EndTrainerBattle(u16 trainerNum);
//void Rogue_Battle_StartWildBattle(void);
void Rogue_Battle_EndWildBattle(void);
void Rogue_Safari_EndWildBattle(void);

bool8 Rogue_AllowItemUse(u16 itemId);
void Rogue_OnItemUse(u16 itemId);

void Rogue_PreBattleSetup(void);
bool8 Rogue_OverrideTrainerItems(u16* items);

void Rogue_ApplyMonPreset(struct Pokemon* mon, u8 level, const struct RogueMonPreset* preset);

void Rogue_ModifyWildMonHeldItem(u16* itemId);
void Rogue_CreateWildMon(u8 area, u16* species, u8* level, bool8* forceShiny);
u16 Rogue_SelectRandomWildMon(void);
bool8 Rogue_PreferTraditionalWildMons(void);
bool8 Rogue_AreWildMonEnabled(void);
void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId);
void Rogue_SafariTypeForMap(u8* outArray, u8 arraySize);
void Rogue_ModifyEventMon(struct Pokemon* mon);
void Rogue_ModifyScriptMon(struct Pokemon* mon);


void Rogue_CopyLabEncounterMonNickname(u16 index, u8* dst);
bool8 Rogue_GiveLabEncounterMon(u16 index);

const u16* Rogue_CreateMartContents(u16 itemCategory, u16* minSalePrice);
void Rogue_ModifyTutorMoves(struct Pokemon* mon, u8 tutorType, u8* count, u8* hiddenCount, u16* moves);

#endif