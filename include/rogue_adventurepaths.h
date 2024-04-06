#ifndef ROGUE_ADVENTUREPATHS_H
#define ROGUE_ADVENTUREPATHS_H

#define ROGUE_WARP_UNSET 0
#define ROGUE_WARP_TO_ROOM 1
#define ROGUE_WARP_TO_ADVPATH 2 

extern EWRAM_DATA struct RogueAdvPath gRogueAdvPath;

bool8 RogueAdv_GenerateAdventurePathsIfRequired();
void RogueAdv_ApplyAdventureMetatiles();
void RogueAdv_Debug_ForceRegenerateAdventurePaths();

u8 RogueAdv_GetTileNum();
bool8 RogueAdv_IsViewingPath();

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp);
void RogueAdv_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity);
bool8 RogueAdv_CanUseEscapeRope(void);

u8 Rogue_GetTypeForHintForRoom(struct RogueAdvPathRoom const* room);
void RogueAdv_GetLastInteractedRoomParams();
void RogueAdv_WarpLastInteractedRoom();

#endif