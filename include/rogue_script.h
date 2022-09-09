#ifndef ROGUE_SCRIPT_H
#define ROGUE_SCRIPT_H


bool8 Rogue_CheckPartyHasRoomForMon(void);

void Rogue_RandomisePartyMon(void);
void Rogue_AlterMonIVs(void);
void Rogue_ApplyStatusToMon(void);
void Rogue_ReducePartySize(void);

u16 Rogue_GetMonEvoCount(void);
void Rogue_GetMonEvoParams(void);

#endif