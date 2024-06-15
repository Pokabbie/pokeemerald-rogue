#ifndef ROGUE_TRAINERS_H
#define ROGUE_TRAINERS_H

bool8 Rogue_IsBossTrainer(u16 trainerNum);
bool8 Rogue_IsRivalTrainer(u16 trainerNum);
bool8 Rogue_IsTeamBossTrainer(u16 trainerNum);
bool8 Rogue_IsKeyTrainer(u16 trainerNum);
bool8 Rogue_IsExpTrainer(u16 trainerNum);
bool8 Rogue_IsBattleSimTrainer(u16 trainerNum);
bool8 Rogue_GetTrainerFlag(u16 trainerNum);

u16 Rogue_GetDynamicTrainer(u16 i);
void Rogue_SetDynamicTrainer(u16 i, u16 trainerNum);

const struct RogueTrainer* Rogue_GetTrainer(u16 trainerNum);
struct RogueBattleMusic const* Rogue_GetTrainerMusic(u16 trainerNum);
const u8* Rogue_GetTrainerString(u16 trainerNum, u8 textId);

u16 Rogue_GetTrainerObjectEventGfx(u16 trainerNum);
u16 Rogue_GetTrainerNumFromObjectEvent(struct ObjectEvent *curObject);
u16 Rogue_GetTrainerNumFromLastInteracted();

u8 Rogue_GetTrainerWeather(u16 trainerNum);
u8 Rogue_GetTrainerTypeAssignment(u16 trainerNum);
u16 Rogue_GetTrainerTypeGroupId(u16 trainerNum);
u16 Rogue_GetTrainerPokeballId(u16 trainerNum);
bool8 Rogue_IsValidTrainerShinySpecies(u16 trainerNum, u16 species);
bool8 Rogue_ShouldTrainerSmartSwitch(u16 trainerNum);
bool8 Rogue_ShouldTrainerBeDoubleAware(u16 trainerNum);
bool8 Rogue_ShouldTrainerTrySetup(u16 trainerNum);
bool8 Rogue_ShouldTrainerBeSmart(u16 trainerNum);
bool8 Rogue_ShouldTrainerSaveAceMon(u16 trainerNum);
bool8 Rogue_ShouldDynamaxMon(u16 trainerNum, u8 slot, u8 numOthersAlive);
bool8 Rogue_ShouldTerastallizeMon(u16 trainerNum, u8 slot, u8 numOthersAlive);
s32 Rogue_GetSwitchAIDamageDivisor(u16 trainerNum, u8 slot);
s32 Rogue_GetSwitchAISpeedDivisor(u16 trainerNum, u8 slot);

u8 Rogue_CalculatePlayerMonLvl();
u8 Rogue_CalculateTrainerMonLvl();
u8 Rogue_CalculateMiniBossMonLvl();
u8 Rogue_CalculateRivalMonLvl();
u8 Rogue_CalculateBossMonLvl();

bool8 Rogue_UseCustomPartyGenerator(u16 trainerNum);

void Rogue_ChooseBossTrainersForNewAdventure();
void Rogue_ChooseRivalTrainerForNewAdventure();
void Rogue_ChooseTeamBossTrainerForNewAdventure();
u16 Rogue_ChooseNextBossTrainerForVictoryLap();
void Rogue_ChooseRouteTrainers(u16* writeBuffer, u16 bufferCapacity);
void Rogue_ChooseTeamHideoutTrainers(u16* writeBuffer, u16 bufferCapacity);
void Rogue_ChooseSpectatorTrainers(u16* writeBuffer, u16 bufferCapacity);

void Rogue_GetPreferredElite4Map(u16 trainerNum, s8* mapGroup, s8* mapNum);

u8 Rogue_CreateTrainerParty(u16 trainerNum, struct Pokemon* party, u8 monCapacity, bool8 firstTrainer);

#endif