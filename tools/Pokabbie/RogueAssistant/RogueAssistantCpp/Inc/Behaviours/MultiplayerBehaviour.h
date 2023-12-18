#pragma once
#include "Defines.h"
#include "GameConnectionBehaviour.h"
#include "Timer.h"
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

	struct ServerState
	{
		std::vector<u8> m_PlayerProfiles;
		ENetPeer* m_PendingHandshake;
		UpdateTimer m_GameStateTimer;
		UpdateTimer m_PlayerStateTimer;

		ServerState()
			: m_PendingHandshake(nullptr)
			, m_GameStateTimer(UpdateTimer::c_5UPS)
			, m_PlayerStateTimer(UpdateTimer::c_5UPS)
		{}
	};

	struct ClientState
	{
		UpdateTimer m_PlayerStateTimer;

		ClientState()
			: m_PlayerStateTimer(UpdateTimer::c_5UPS)
		{}
	};

	void OpenHostConnection(GameConnection& game);
	void OpenClientConnection(GameConnection& game);
	void CloseConnection(GameConnection& game);

	void PollConnection(GameConnection& game);
	void ConnectedUpdate(GameConnection& game);
	void HandleIncomingMessage(GameConnection& game, ENetEvent& netEvent);

	void SendMultiplayerConfirmationToGame(GameConnection& game);

	u16 m_Port;
	ConnectionState m_ConnState;

	u8 m_RequestFlags;
	u8 m_PlayerId;
	ENetHost* m_NetServer;

	ENetHost* m_NetClient;
	ENetPeer* m_NetPeer;

	ServerState m_ServerState;
	ClientState m_ClientState;
};