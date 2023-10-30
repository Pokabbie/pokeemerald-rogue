#pragma once
#include "Defines.h"
#include "GameConnectionBehaviour.h"
#include "enet/enet.h"

#include <queue>

class MultiplayerBehaviour : public IGameConnectionBehaviour
{
public:
	MultiplayerBehaviour();

	virtual void OnAttach(GameConnection& game) override;
	virtual void OnDetach(GameConnection& game) override;
	virtual void OnUpdate(GameConnection& game) override;

	inline bool IsHost() const { return m_NetServer != nullptr; }

private:
	enum class ConnectionState
	{
		AwaitingHandshake,
		AwaitingResponse,
		ConnectionConfirmed,
		Connected,

		Default = AwaitingHandshake
	};

	void OpenHostConnection(GameConnection& game);
	void OpenClientConnection(GameConnection& game);
	void CloseConnection(GameConnection& game);

	void PollConnection(GameConnection& game);
	void HandleIncomingMessage(GameConnection& game, ENetEvent& netEvent);

	void SendMultiplayerConfirmationToGame(GameConnection& game);

	u16 m_Port;
	ConnectionState m_ConnState;

	u8 m_RequestFlags;
	ENetHost* m_NetServer;
	ENetPeer* m_PendingHandshake;

	ENetHost* m_NetClient;
	ENetPeer* m_NetPeer;
};