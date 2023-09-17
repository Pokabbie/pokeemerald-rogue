#include "Behaviours/MultiplayerBehaviour.h"
#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"

void MultiplayerBehaviour::OnAttach(GameConnection& game)
{

}

void MultiplayerBehaviour::OnDetach(GameConnection& game)
{

}

void MultiplayerBehaviour::OnUpdate(GameConnection& game)
{
	if (!game.IsReady() || !game.GetObservedGameMemory().AreHeadersValid())
		return;

	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	if (game.GetObservedGameMemory().IsMuliplayerStateValid())
	{
		GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();
		u8 const* multiplayerBlob = game.GetObservedGameMemory().GetMultiplayerStateBlob();

		u8 requestFlags = multiplayerBlob[rogueHeader.netRequestStateOffset];
		u8 currentFlags = multiplayerBlob[rogueHeader.netCurrentStateOffset];

		if (requestFlags != currentFlags)
		{
			// TODO - Setup server or join server here
			game.WriteRequest(CreateAnonymousMessageId(), multiplayerAddress + rogueHeader.netCurrentStateOffset, &requestFlags, sizeof(requestFlags));
		}

		return;
	}
}