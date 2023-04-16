#ifndef ROGUE_ASSISTANT_H
#define ROGUE_ASSISTANT_H

void Rogue_AssistantInit();
void Rogue_AssistantMainCB();
void Rogue_AssistantOverworldCB();

bool8 Rogue_IsNetMultiplayerActive();
bool8 Rogue_IsNetMultiplayerHost();
void Rogue_RemoveNetObjectEvents();

void Rogue_CreateMultiplayerConnectTask(bool8 asHost);

#endif