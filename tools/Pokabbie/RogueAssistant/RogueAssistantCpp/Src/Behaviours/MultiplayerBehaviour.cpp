#include "Behaviours/MultiplayerBehaviour.h"
#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"

// Keep in sync with game
#define NET_STATE_NONE              0
#define NET_STATE_ACTIVE            (1 << 0)
#define NET_STATE_HOST              (2 << 0)

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
	// TODO - Close server
	return;
}

void MultiplayerBehaviour::OnUpdate(GameConnection& game)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	if (!game.GetObservedGameMemory().IsMultiplayerStateValid())
		return;

	GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();
	u8 const* multiplayerBlob = game.GetObservedGameMemory().GetMultiplayerStateBlob();

	u8 requestFlags = multiplayerBlob[rogueHeader.netRequestStateOffset];

	if (m_RequestFlags != requestFlags)
	{
		// Restart multiplayer as we're not valid anymore :(
		game.RemoveBehaviour(this);
		return;
	}



	//u8 requestFlags = multiplayerBlob[rogueHeader.netRequestStateOffset];
	//u8 currentFlags = multiplayerBlob[rogueHeader.netCurrentStateOffset];
	//
	//// Request flags?
	//game.WriteRequest(CreateAnonymousMessageId(), multiplayerAddress + rogueHeader.netCurrentStateOffset, &requestFlags, sizeof(requestFlags));
}

void MultiplayerBehaviour::OpenHostConnection(GameConnection& game)
{
	SendMultiplayerConfirmationToGame(game);
}

void MultiplayerBehaviour::OpenClientConnection(GameConnection& game)
{
	SendMultiplayerConfirmationToGame(game);
}

void MultiplayerBehaviour::CloseConnection(GameConnection& game)
{

}

void MultiplayerBehaviour::SendMultiplayerConfirmationToGame(GameConnection& game)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();

	game.WriteRequest(CreateAnonymousMessageId(), multiplayerAddress + rogueHeader.netCurrentStateOffset, &m_RequestFlags, sizeof(m_RequestFlags));
}