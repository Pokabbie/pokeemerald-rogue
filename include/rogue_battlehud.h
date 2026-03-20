#ifndef ROGUE_BATTLEHUD_H
#define ROGUE_BATTLEHUD_H

void RogueBH_CreateBattleOverlay();
void RogueBH_RemoveBattleOverlay(bool32 fromResetSprites);

void RogueBH_ToggleStatView();
bool8 RogueBH_IsStatViewActive();
void RogueBH_HandleStatViewUpdate(u32 battler);
void RogueBH_PrintStatView();

#endif