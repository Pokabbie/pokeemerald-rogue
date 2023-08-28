#ifndef ROGUE_MULTIPLAYER_H
#define ROGUE_MULTIPLAYER_H

extern EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer;

bool8 RogueMP_Init();

bool8 RogueMP_IsActive();
bool8 RogueMP_IsHost();
bool8 RogueMP_IsClient();

void RogueMP_OpenHost();
void RogueMP_OpenClient();
void RogueMP_Close();

void RogueMP_Update();

void RogueMP_WaitForConnection();

#endif