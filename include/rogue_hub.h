#ifndef ROGUE_HUB_H
#define ROGUE_HUB_H

void RogueHub_Enter();
void RogueHub_Exit();
void RogueHub_ClearProgress();

bool8 RogueHub_HasUpgrade(u16 upgradeId);
void RogueHub_SetUpgrade(u16 upgradeId, bool8 state);
bool8 RogueHub_HasUpgradeRequirements(u16 upgradeId);

bool8 RogueHub_HasAreaBuilt(u8 area);
void RogueHub_BuildArea(u8 area, s8 x, s8 y);
bool8 RogueHub_HasAreaBuildRequirements(u8 area);

u8 RogueHub_FindAreaAtCoord(s8 x, s8 y);
bool8 RogueHub_AreaHasFreeConnection(u8 area, u8 dir);
u8 RogueHub_GetAreaAtConnection(u8 area, u8 dir);

u8 RogueHub_GetAreaFromCurrentMap();
void RogueHub_GetAvaliableAreasToBuild(u8* outAreas, u8* outCount);
void RogueHub_GetAvaliableDirectionsToBuild(u8* outDirs, u8* outCount);
void RogueHub_GetAvaliableUpgrades(u8 area, u8* outUpgrades, u8* outUpgradeCount);

void RogueHub_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp);
bool8 RogueHub_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection);
void RogueHub_ApplyMapMetatiles();

void RogueHub_GetAreaBuildsMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity);
void RogueHub_GetAreaBuildDirectionMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity);
void RogueHub_GetAreaUpgradesMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity);
void RogueHub_GetUpgradeFromMultichoiceResult();
void RogueHub_ApplyHubUpgrade();

#endif