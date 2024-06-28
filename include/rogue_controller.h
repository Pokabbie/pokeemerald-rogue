#ifndef ROGUE_CONTROLLER_H
#define ROGUE_CONTROLLER_H

#include "rogue_baked.h"

enum
{
    SHINY_ROLL_DYNAMIC,
    SHINY_ROLL_STATIC,
    SHINY_ROLL_SHINY_LOCKED,
};

struct MenuAction;
struct MusicPlayerInfo;

extern struct RogueSaveBlock* gRogueSaveBlock;
extern EWRAM_DATA struct RogueRunData gRogueRun;

#ifdef ROGUE_DEBUG
extern EWRAM_DATA struct RogueDebugConfig gRogueDebug;
#endif

#define OVERWORLD_FLAG 0

u16 RogueRandomRange(u16 range, u8 seedFlag);
bool8 RogueRandomChance(u8 chance, u16 seedFlag);

u16 Rogue_GetShinyOdds(u8 shinyRoll);
bool8 Rogue_RollShinyState(u8 shinyRoll);

void RemoveMonAtSlot(u8 slot, bool8 keepItems, bool8 compactPartySlots);
void RemoveAnyFaintedMons(bool8 keepItems);

bool8 Rogue_IsRunActive(void);
bool8 Rogue_IsVictoryLapActive(void);
bool8 Rogue_InWildSafari(void);
bool8 Rogue_UseSafariBattle(void);
bool8 Rogue_CanChangeSafariBall(void);
u8 Rogue_GetCurrentDifficulty(void);
void Rogue_SetCurrentDifficulty(u8 difficulty);
u16* Rogue_GetVictoryLapHistoryBufferPtr();
u32 Rogue_GetVictoryLapHistoryBufferSize();

bool8 Rogue_ForceExpAll(void);
bool8 Rogue_EnableExpGain(void);
bool8 Rogue_EnableAffectionMechanics(void);
bool8 Rogue_ShouldReleaseFaintedMons(void);
bool8 Rogue_FastBattleAnims(void);
u8 Rogue_GetBattleSpeedScale(bool8 forHealthbar);
bool8 Rogue_UseKeyBattleAnims(void);
bool8 Rogue_GetBattleAnimsEnabled(void);
bool8 Rogue_UseFinalQuestEffects(void);
bool8 Rogue_AssumeFinalQuestFakeChamp(void);
bool8 Rogue_IsFinalQuestFinalBoss(void);
bool8 Rogue_ApplyFinalQuestFinalBossTeamSwap(void);
bool8 Rogue_Use100PercEffects(void);
bool8 Rogue_Use200PercEffects(void);

u8 Rogue_ModifySoundVolume(struct MusicPlayerInfo *mplayInfo, u8 volume, u16 soundType);
u16 Rogue_ModifyPlayBGM(u16 songNum);
u16 Rogue_ModifyPlaySE(u16 songNum);
u16 Rogue_ModifyPlayFanfare(u16 songNum);

void Rogue_ModifyBattleWaitTime(u16* waitTime, bool8 awaitingMessage);
s16 Rogue_ModifyBattleSlideAnim(s16 rate);
bool8 Rogue_UseFastLevelUpAnim();
void Rogue_ModifyBattleWinnings(u16 trainerNum, u32* money);
void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain);
void Rogue_ModifyEVGain(int* multiplier);
void Rogue_ModifyCatchRate(u16 species, u16* catchRate, u16* ballMultiplier);
void Rogue_ModifyCaughtMon(struct Pokemon *mon);
void Rogue_OnAcceptCaughtMon(struct Pokemon *mon);
void Rogue_ModifyEggMon(struct Pokemon *mon);
void Rogue_DiscardedCaughtMon(struct Pokemon *mon);
u16 Rogue_ModifyItemPickupAmount(u16 itemId, u16 amount);
u8 Rogue_ModifyGenderRatio(u8 genderRatio);

const void* Rogue_ModifyPaletteLoad(const void* input);
bool8 Rogue_ModifyPaletteDecompress(const u32* input, void* writeBuffer);
bool8 Rogue_ModifyObjectPaletteSlot(u16 graphicsId, u8* palSlot);

void Rogue_ModifyOverworldPalette(u16 offset, u16 count);
void Rogue_ModifyBattlePalette(u16 offset, u16 count);

const u8* Rogue_ModifyFieldMessage(const u8* str);
const u8* Rogue_ModifyBattleMessage(const u8* str);
void Rogue_ModifyBattleMon(u8 monId, struct BattlePokemon* battleMon, bool8 isPlayer);

const u8* Rogue_ModifyOverworldInteractionScript(struct MapPosition *position, u16 metatileBehavior, u8 direction, u8 const* script);
u16 Rogue_ModifyOverworldMapWeather(u16 weather);

const struct Tileset * Rogue_ModifyOverworldTileset(const struct Tileset * tileset);

bool8 Rogue_CanRenameMon(struct Pokemon* mon);

bool8 Rogue_ShouldShowMiniMenu(void);
u16 Rogue_MiniMenuHeight(void);
u8* Rogue_GetMiniMenuContent(void);
void Rogue_CreateMiniMenuExtraGFX(void);
void Rogue_RemoveMiniMenuExtraGFX(void);

u8 SpeciesToGen(u16 species);
bool8 Rogue_IsItemEnabled(u16 item);

bool8 IsMegaEvolutionEnabled(void);
bool8 IsZMovesEnabled(void);
bool8 IsDynamaxEnabled(void);
bool8 IsTerastallizeEnabled(void);
bool8 IsHealingFlaskEnabled(void);

void Rogue_RandomiseStarters();
void Rogue_ResetConfigHubSettings(void);
void Rogue_OnNewGame(void);
void Rogue_GameClear(void);
void Rogue_SetDefaultOptions(void);
void Rogue_NotifySaveVersionUpdated(u16 fromNumber, u16 toNumber);
void Rogue_NotifySaveLoaded(void);
bool8 Rogue_IsObjectEventExcludedFromSave(struct ObjectEvent* objectEvent);
void Rogue_OnSecondPassed(void);
void Rogue_OnMinutePassed(void);
void Rogue_OnHourPassed(void);

bool8 Rogue_OnProcessPlayerFieldInput(void);
bool8 Rogue_IsPartnerMonInTeam(void);
u16 Rogue_PostRunRewardLvls();
u16 Rogue_PostRunRewardMoney();

void Rogue_MainInit(void);
void Rogue_MainEarlyCB(void);
void Rogue_MainLateCB(void);
void Rogue_OverworldCB(u16 newKeys, u16 heldKeys, bool8 inputActive);
void Rogue_OnReturnToField();
bool8 Rogue_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider);
bool8 Rogue_IsRunningToggledOn();

void Rogue_OnSpawnObjectEvent(struct ObjectEvent *objectEvent, u8 objectEventId);
void Rogue_OnRemoveObjectEvent(struct ObjectEvent *objectEvent);
void Rogue_OnMovementType_Player(struct Sprite *sprite);
void Rogue_OnObjectEventMovement(u8 objectEventId);
void Rogue_OnResumeMap();
void Rogue_OnObjectEventsInit();
void Rogue_OnResetAllSprites();
bool8 Rogue_TryGetCachedObjectEventId(u32 localId, u8* eventObjectId);

void Rogue_GetHotTrackingData(u16* count, u16* average, u16* min, u16* max);

void Rogue_OnLoadMap(void);
bool8 Rogue_ShouldSkipReloadMapTileView();
void Rogue_OnWarpIntoMap(void);
void Rogue_OnSetWarpData(struct WarpData *warp);
void Rogue_ModifyMapHeader(struct MapHeader *mapHeader);

void Rogue_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp);
bool8 Rogue_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection);
void Rogue_ModifyObjectEvents(struct MapHeader *mapHeader, bool8 loadingFromSave, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity);

void Rogue_ResetAdventurePathBuffers();
u8 Rogue_SelectRouteRoom(u8 difficulty);
u8 Rogue_GetCurrentLegendaryEncounterId(void);
u16 Rogue_GetLegendaryRoomForSpecies(u16 species);
u8 Rogue_GetCurrentTeamHideoutEncounterId(void);
bool8 Rogue_IsBattleAlphaMon(u16 species);
bool8 Rogue_IsBattleRoamerMon(u16 species);
u16 Rogue_SelectWildDenEncounterRoom(void);
u16 Rogue_SelectHoneyTreeEncounterRoom(void);

void Rogue_SelectMiniBossRewardMons();

void Rogue_Battle_StartTrainerBattle(void);
void Rogue_Battle_TrainerTeamReady(void);
void Rogue_Battle_EndTrainerBattle(u16 trainerNum);
void Rogue_Battle_StartWildBattle(void);
void Rogue_Battle_EndWildBattle(void);
void Rogue_Safari_EndWildBattle(void);

bool8 Rogue_AllowItemUse(u16 itemId);
void Rogue_OnItemUse(u16 itemId);
void Rogue_OnSpendMoney(u32 money);
u32 Rogue_GetTotalSpentOnActiveMap();
u16 Rogue_GetBagCapacity();
u16 Rogue_GetBagPocketAmountPerItem(u8 pocket);
u32 Rogue_CalcBagUpgradeCost();

void Rogue_AddPartySnapshot();
void Rogue_DebugFillPartySnapshots();

void Rogue_PreBattleSetup(void);
bool8 Rogue_OverrideTrainerItems(u16* items);

void Rogue_ApplyMonCompetitiveSet(struct Pokemon* mon, u8 level, struct RoguePokemonCompetitiveSet const* preset, struct RoguePokemonCompetitiveSetRules const* rules);

void Rogue_ModifyWildMonHeldItem(u16* itemId);
void Rogue_CreateWildMon(u8 area, u16* species, u8* level, bool8* forceShiny);
u16 Rogue_SelectRandomWildMon(void);
bool8 Rogue_PreferTraditionalWildMons(void);
bool8 Rogue_AreWildMonEnabled(void);
bool8 Rogue_CanScatterPokeblock(u16 itemId);
bool8 Rogue_RerollSingleWildSpecies(u8 type);
bool8 Rogue_TryAddHoneyTreePokeblock(u16 itemId);
void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId);
void Rogue_SafariTypeForMap(u8* outArray, u8 arraySize);
void Rogue_ModifyWildMon(struct Pokemon* mon);
void Rogue_ModifyScriptMon(struct Pokemon* mon);
void Rogue_ModifyGiveMon(struct Pokemon* mon);
struct BoxPokemon* Rogue_GetDaycareBoxMon(u8 slot);
u8 Rogue_GetCurrentDaycareSlotCount();
void Rogue_SwapMonInDaycare(struct Pokemon* partyMon, u8 daycareSlot);
void Rogue_DaycareMultichoiceCallback(struct MenuAction* outList, u8* outCount, u8 listCapacity);
void Rogue_BeginCatchingContest(u8 type, u8 stat);
void Rogue_EndCatchingContest();
bool8 Rogue_IsCatchingContestActive();
void Rogue_GetCatchingContestResults(u16* caughtSpecies, bool8* didWin, u16* winningSpecies);

u8 Rogue_GetEncounterDifficultyModifier();
u16 Rogue_GetTRMove(u16 trNumber);


void Rogue_CopyLabEncounterMonNickname(u16 index, u8* dst);
bool8 Rogue_GiveLabEncounterMon(u16 index);

void Rogue_OpenMartQuery(u16 itemCategory, u16* minSalePrice);
void Rogue_CloseMartQuery();
void Rogue_ModifyTutorMoves(struct Pokemon* mon, u8 tutorType, u8* count, u8* hiddenCount, u16* moves);

void Rogue_CorrectMonDetails(struct Pokemon* party, u8 count);
void Rogue_CorrectBoxMonDetails(struct BoxPokemon* mon);

#endif