#ifndef ROGUE_MULTIPLAYER_H
#define ROGUE_MULTIPLAYER_H

enum
{
    MP_CMD_NONE,
    MP_CMD_REQUEST_MON,
    MP_CMD_REQUEST_TALK_TO_PLAYER,
    MP_CMD_COUNT,
};

extern EWRAM_DATA struct RogueNetMultiplayer* gRogueMultiplayer;

void RogueMP_Init();

bool8 RogueMP_IsActive();
bool8 RogueMP_IsActiveOrConnecting();
bool8 RogueMP_IsConnecting();
bool8 RogueMP_IsHost();
bool8 RogueMP_IsClient();
u8 RogueMP_GetLocalPlayerId();
u8 RogueMP_GetRemotePlayerId();
bool8 RogueMP_IsRemotePlayerActive();

u8 RogueMP_GetPlayerOutfitId(u8 playerId);
u16 RogueMP_GetPlayerOutfitStyle(u8 playerId, u8 outfitStyle);
u8 const* RogueMP_GetPlayerName(u8 playerId);
u8 const* RogueMP_GetPlayerHubName(u8 playerId);
u8 const* RogueMP_GetPlayerTrainerId(u8 playerId);

u8 RogueMP_GetPlayerStatus(u8 playerId);
u8 RogueMP_WaitPlayerStatusSync(bool8 canCancel);
u8 RogueMP_WaitUpdatedPlayerStatus(bool8 canCancel);

void RogueMP_PushLocalPlayerStatus(u8 status, u16 param);
bool8 RogueMP_TryExecuteScripts();

void RogueMP_OpenHost();
void RogueMP_OpenClient();
void RogueMP_Close();

void RogueMP_MainCB();
void RogueMP_OverworldCB();
void RogueMP_RemoveObjectEvents();

u8 RogueMP_WaitForConnection();
u8 RogueMP_WaitForOutgoingCommand(bool8 canCancel);
u8 RogueMP_WaitForIncomingCommand(bool8 canCancel);
bool8 RogueMP_IsWaitingForCommandToFinish();


void RogueMP_Cmd_RequestPartyMon(u8 fromSlot, u8 toSlot);
void RogueMP_Cmd_RequestTalkToPlayer();

bool8 RogueMP_HasTalkRequestPending();
void RogueMP_NotifyAcceptTalkRequest();

#endif