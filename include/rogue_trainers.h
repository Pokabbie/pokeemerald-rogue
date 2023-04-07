#ifndef ROGUE_TRAINERS_H
#define ROGUE_TRAINERS_H

bool8 Rogue_IsBossTrainer(u16 trainerNum);
bool8 Rogue_IsMiniBossTrainer(u16 trainerNum);
bool8 Rogue_IsAnyBossTrainer(u16 trainerNum);
bool8 Rogue_TryGetTrainer(u16 trainerNum, const struct RogueTrainer** trainerPtr);
u16 Rogue_GetTrainerObjectEventGfx(u16 trainerNum);
u8 Rogue_GetTrainerWeather(u16 trainerNum);
u8 Rogue_GetTrainerTypeAssignment(u16 trainerNum);

u8 Rogue_CalculatePlayerMonLvl();
u8 Rogue_CalculateTrainerMonLvl();
u8 Rogue_CalculateMiniBossMonLvl();
u8 Rogue_CalculateBossMonLvl();

bool8 Rogue_UseCustomPartyGenerator(u16 trainerNum);

u16 Rogue_NextBossTrainerId();
void Rogue_GetPreferredElite4Map(u16 trainerNum, s8* mapGroup, s8* mapNum);

u8 Rogue_CreateTrainerParty(u16 trainerNum, struct Pokemon* party, u8 monCapacity, bool8 firstTrainer);

#endif