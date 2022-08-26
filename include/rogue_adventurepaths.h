#ifndef ROGUE_ADVENTUREPATHS_H
#define ROGUE_ADVENTUREPATHS_H

extern EWRAM_DATA struct RogueAdvPath gRogueAdvPath;

bool8 RogueAdv_GenerateAdventurePathsIfRequired();
void RogueAdv_ApplyAdventureMetatiles();
void RogueAdv_UpdateObjectGFX();

void RogueAdv_EnqueueNextWarp(struct WarpData *warp);

void RogueAdv_BufferNodeMessage();
void RogueAdv_ExecuteNodeAction();

#endif