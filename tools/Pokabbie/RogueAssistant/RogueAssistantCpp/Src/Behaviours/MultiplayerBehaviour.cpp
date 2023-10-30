#include "Behaviours/MultiplayerBehaviour.h"
#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"

enum RogueNetChannel
{
	Handshake,
	Num,
};

// Keep in sync with game
#define NET_STATE_NONE              0
#define NET_STATE_ACTIVE            (1 << 0)
#define NET_STATE_HOST              (2 << 0)

#define NET_HANDSHAKE_STATE_NONE                0
#define NET_HANDSHAKE_STATE_SEND_TO_HOST        1
#define NET_HANDSHAKE_STATE_SEND_TO_CLIENT      2


MultiplayerBehaviour::MultiplayerBehaviour()
	: m_Port(20125)
	, m_RequestFlags(0)
	, m_NetServer(nullptr)
	, m_NetClient(nullptr)
	, m_NetPeer(nullptr)
	, m_ConnState(ConnectionState::Default)
	, m_PendingHandshake(nullptr)
{
}

void MultiplayerBehaviour::OnAttach(GameConnection& game)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	if (game.GetObservedGameMemory().IsMultiplayerStateValid())
	{
		GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();
		u8 const* multiplayerBlob = game.GetObservedGameMemory().GetMultiplayerStateBlob();

		u8 requestFlags = multiplayerBlob[rogueHeader.netRequestStateOffset];
		m_RequestFlags = requestFlags;

		if (m_RequestFlags & NET_STATE_HOST)
		{
			OpenHostConnection(game);
		}
		else
		{
			OpenClientConnection(game);
		}
	}
}

void MultiplayerBehaviour::OnDetach(GameConnection& game)
{
	CloseConnection(game);
}

void MultiplayerBehaviour::OnUpdate(GameConnection& game)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	if (!game.GetObservedGameMemory().IsMultiplayerStateValid())
		return;

	u8 const* multiplayerBlob = game.GetObservedGameMemory().GetMultiplayerStateBlob();

	u8 requestFlags = multiplayerBlob[rogueHeader.netRequestStateOffset];

	if (m_RequestFlags != requestFlags)
	{
		// Restart multiplayer as we're not valid anymore :(
		game.RemoveBehaviour(this);
		return;
	}

	if (m_PendingHandshake)
	{
		// Temporarily pause incoming requests if we're processing a handshake
		ASSERT_MSG(IsHost(), "Can only process handshakes if as host");

		u8 handshakeState = multiplayerBlob[rogueHeader.netHandshakeOffset + rogueHeader.netHandshakeStateOffset];
		if (handshakeState == NET_HANDSHAKE_STATE_SEND_TO_CLIENT)
		{
			ENetPacket* packet = enet_packet_create(
				&multiplayerBlob[rogueHeader.netHandshakeOffset],
				rogueHeader.netHandshakeSize,
				ENET_PACKET_FLAG_RELIABLE
			);
			enet_peer_send(m_PendingHandshake, RogueNetChannel::Handshake, packet);
			m_PendingHandshake = nullptr;
		}
	}
	else
	{
		// Handle incoming/outgoing messages
		PollConnection(game);
	}

	// Handle handshake
	//
	switch (m_ConnState)
	{
	case MultiplayerBehaviour::ConnectionState::AwaitingHandshake:
		{
			u8 handshakeState = multiplayerBlob[rogueHeader.netHandshakeOffset + rogueHeader.netHandshakeStateOffset];
			if (handshakeState == NET_HANDSHAKE_STATE_SEND_TO_HOST)
			{
				ENetPacket* packet = enet_packet_create(
					&multiplayerBlob[rogueHeader.netHandshakeOffset], 
					rogueHeader.netHandshakeSize,
					ENET_PACKET_FLAG_RELIABLE
				);
				enet_peer_send(m_NetPeer, RogueNetChannel::Handshake, packet);
				m_ConnState = ConnectionState::AwaitingResponse;
			}
		}
		break;

	case MultiplayerBehaviour::ConnectionState::AwaitingResponse:
		break;

	case MultiplayerBehaviour::ConnectionState::ConnectionConfirmed:
		SendMultiplayerConfirmationToGame(game);
		m_ConnState = ConnectionState::Connected;
		break;

	case MultiplayerBehaviour::ConnectionState::Connected:
		break;
	}
}

void MultiplayerBehaviour::OpenHostConnection(GameConnection& game)
{
	LOG_INFO("ENet: Openning Host");

	if (enet_initialize() != 0)
	{
		ASSERT_FAIL("ENet: Failed to initialise");
		game.RemoveBehaviour(this);
		return;
	}

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = m_Port;

	m_NetServer = enet_host_create(&address,
		4, // client count
		RogueNetChannel::Num,  // channel count
		0,  // assumed incoming bandwidth
		0   // assumed outgoing bandwidth
	);

	if (m_NetServer == nullptr)
	{
		LOG_ERROR("ENet: Failed to create host");
		game.RemoveBehaviour(this);
		return;
	}

	m_ConnState = ConnectionState::ConnectionConfirmed;
}

void MultiplayerBehaviour::OpenClientConnection(GameConnection& game)
{
	LOG_INFO("ENet: Openning Client");

	if (enet_initialize() != 0)
	{
		ASSERT_FAIL("ENet: Failed to initialise");
		game.RemoveBehaviour(this);
		return;
	}
		
	m_NetClient = enet_host_create(nullptr, // null address to indicate this host is for client connection
		1, // client count
		RogueNetChannel::Num,  // channel count
		0,  // assumed incoming bandwidth
		0   // assumed outgoing bandwidth
	);
	
	if (m_NetClient == nullptr)
	{
		LOG_ERROR("ENet: Failed to create client");
		game.RemoveBehaviour(this);
		return;
	}

	ENetAddress address;
	enet_address_set_host(&address, "localhost");
	address.port = m_Port;

	m_NetPeer = enet_host_connect(m_NetClient, &address, RogueNetChannel::Num, 0);

	if (m_NetPeer == nullptr)
	{
		LOG_ERROR("ENet: Failed to create client peer");
		game.RemoveBehaviour(this);
		return;
	}

	// Attempt to connect
	ENetEvent netEvent;
	if (enet_host_service(m_NetClient, &netEvent, 5000) > 0 && netEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		LOG_INFO("ENet: Connected successfully!");
	}
	else
	{
		enet_peer_reset(m_NetPeer);
		m_NetPeer = nullptr;

		LOG_ERROR("ENet: Failed to connect.");
		game.RemoveBehaviour(this);
		return;
	}

	m_ConnState = ConnectionState::AwaitingHandshake;
}

void MultiplayerBehaviour::CloseConnection(GameConnection& game)
{
	if (m_NetServer != nullptr)
	{
		LOG_INFO("ENet: Closing Host");

		enet_host_destroy(m_NetServer);
		enet_deinitialize();

		m_NetServer = nullptr;
	}

	if (m_NetClient != nullptr)
	{
		LOG_INFO("ENet: Closing Client");

		if(m_NetPeer != nullptr)
			enet_peer_reset(m_NetPeer);

		enet_host_destroy(m_NetClient);
		enet_deinitialize();

		m_NetClient = nullptr;
		m_NetPeer = nullptr;
	}
}

void MultiplayerBehaviour::PollConnection(GameConnection& game)
{
	ENetHost* conn = m_NetServer != nullptr ? m_NetServer : m_NetClient;

	if (conn != nullptr)
	{
		ENetEvent netEvent;
		if (enet_host_service(conn, &netEvent, 0) > 0)
		{
			switch (netEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				LOG_INFO("ENet: Connected %x:%u", netEvent.peer->address.host, netEvent.peer->address.port);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				HandleIncomingMessage(game, netEvent);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				LOG_INFO("ENet: Disconnected %x:%u", netEvent.peer->address.host, netEvent.peer->address.port);
				break;

			default:
				ASSERT_FAIL("ENet: Unrecognized event type");
				break;
			}
		}
	}
}

void MultiplayerBehaviour::HandleIncomingMessage(GameConnection& game, ENetEvent& netEvent)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	ASSERT_MSG(game.GetObservedGameMemory().IsMultiplayerStateValid(), "Multiplayer state invalid");

	GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();

	switch (netEvent.channelID)
	{
	case RogueNetChannel::Handshake:
		if (netEvent.packet->dataLength <= rogueHeader.netHandshakeSize)
		{
			game.WriteRequest(CreateAnonymousMessageId(), multiplayerAddress + rogueHeader.netHandshakeOffset, netEvent.packet->data, netEvent.packet->dataLength);

			// If we're the host wait until we get a response
			if (IsHost())
			{
				ASSERT_MSG(m_PendingHandshake == nullptr, "Host cannot handle multiple handshakes at once");
				m_PendingHandshake = netEvent.peer;
			}
			else
			{
				if (m_ConnState == ConnectionState::AwaitingResponse)
				{
					// Client recieved handshake response so confirm connection
					m_ConnState = ConnectionState::ConnectionConfirmed;
				}
			}
		}
		else
		{
			ASSERT_FAIL("Handshake data size missmatch (Attempting to connect with out of date version?)");
		}
		break;
	}

	enet_packet_destroy(netEvent.packet);
}

void MultiplayerBehaviour::SendMultiplayerConfirmationToGame(GameConnection& game)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();

	game.WriteRequest(CreateAnonymousMessageId(), multiplayerAddress + rogueHeader.netCurrentStateOffset, &m_RequestFlags, sizeof(m_RequestFlags));
}