#ifndef ROGUE_ADVENTUREPATHS_H
#define ROGUE_ADVENTUREPATHS_H

#define ROGUE_WARP_UNSET 0
#define ROGUE_WARP_TO_ROOM 1
#define ROGUE_WARP_TO_ADVPATH 2 

extern EWRAM_DATA struct RogueAdvPath gRogueAdvPath;

bool8 RogueAdv_GenerateAdventurePathsIfRequired();
void RogueAdv_ApplyAdventureMetatiles();

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp);
void RogueAdv_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity);
bool8 RogueAdv_CanUseEscapeRope(void);

void RogueAdv_GetNodeParams();
void RogueAdv_ExecuteNodeAction();

void RogueAdv_DebugExecuteRandomNextNode();

#endif