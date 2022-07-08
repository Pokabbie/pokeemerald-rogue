#ifndef ROGUE_CONTROLLER_H
#define ROGUE_CONTROLLER_H

bool8 Rogue_IsRunActive(void);
bool8 Rogue_ForceExpAll(void);

void Rogue_OnNewGame(void);
void Rogue_OnLoadMap(void);
void Rogue_OnWarpIntoMap(void);
void Rogue_OnSetWarpData(struct WarpData *warp);

#endif