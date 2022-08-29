#ifndef ROGUE_ADVENTUREPATHS_H
#define ROGUE_ADVENTUREPATHS_H

extern EWRAM_DATA struct RogueAdvPath gRogueAdvPath;

bool8 RogueAdv_GenerateAdventurePathsIfRequired();
void RogueAdv_ApplyAdventureMetatiles();
void RogueAdv_UpdateObjectGFX();

bool8 RogueAdv_OverrideNextWarp(struct WarpData *warp);

void RogueAdv_GetNodeParams();
void RogueAdv_ExecuteNodeAction();

#endif