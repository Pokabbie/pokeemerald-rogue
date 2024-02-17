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
void RogueHub_BuildAreaInConnDir(u8 area, u8 connDir);
bool8 RogueHub_HasAreaBuildRequirements(u8 area);

u8 RogueHub_FindAreaAtCoord(s8 x, s8 y);
u8 RogueHub_FindAreaInDir(u8 area, u8 dir);
bool8 RogueHub_AreaHasFreeConnection(u8 area, u8 dir);
u8 RogueHub_GetAreaAtConnection(u8 area, u8 dir);
bool8 RogueHub_CanBuildConnectionBetween(u8 fromArea, u8 toArea, u8 dir);

u8 RogueHub_GetAreaFromCurrentMap();

void RogueHub_OnNewDayStarted();
u16 RogueHub_GetWeatherState();

void RogueHub_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp);
bool8 RogueHub_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection);
void RogueHub_ApplyMapMetatiles();

bool8 RogueHub_IsPlayerBaseLayout(u16 layoutId);
void RogueHub_ModifyPlayerBaseObjectEvents(u16 layoutId, bool8 loadingFromSave, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity);
const u8* RogueHub_GetDecoratingScriptFor(u16 layoutId, struct MapPosition *position, u16 metatileBehavior, u8 direction, u8 const* existingScript);

#endif