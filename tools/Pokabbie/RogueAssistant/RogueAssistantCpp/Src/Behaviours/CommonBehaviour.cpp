#include "Behaviours/CommonBehaviour.h"
#include "Behaviours/HomeBoxBehaviour.h"
#include "Behaviours/MultiplayerBehaviour.h"
#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"

void CommonBehaviour::OnAttach(GameConnection & game)
{

}

void CommonBehaviour::OnDetach(GameConnection& game)
{

}

void CommonBehaviour::OnUpdate(GameConnection& game)
{
	if (!game.IsMemoryReadable())
		return;

	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();


	// Notify game that connected, by constantly spamming 0 into the confirm address
	//
	u32 value = 0;
	game.WriteRequest(CreateAnonymousMessageId(), rogueHeader.assistantState + rogueHeader.assistantConfirmOffset, &value, rogueHeader.assistantConfirmSize);


	// Handle multiplayer connect/disconnect
	//
	if (game.GetObservedGameMemory().IsMultiplayerStateValid())
	{
		if (m_MultiplayerBehaviour.expired())
		{
			GameAddress multiplayerAddress = game.GetObservedGameMemory().GetMultiplayerStatePtr();
			u8 const* multiplayerBlob = game.GetObservedGameMemory().GetMultiplayerStateBlob();

			u8 requestFlags = multiplayerBlob[rogueHeader.netRequestStateOffset];
			//u8 currentFlags = multiplayerBlob[rogueHeader.netCurrentStateOffset];

			if (requestFlags != 0)
			{
				m_MultiplayerBehaviour = game.AddBehaviour<MultiplayerBehaviour>();
			}
		}
	}
	else
	{
		auto multiplayer = m_MultiplayerBehaviour.lock();

		if (multiplayer != nullptr)
		{
			game.RemoveBehaviour(multiplayer.get());
			m_MultiplayerBehaviour.reset();
		}
	}

	// Handle home box connect/disconnect
	//
	if (game.GetObservedGameMemory().IsHomeBoxStateValid())
	{
		if (m_HomeBoxBehaviour.expired())
		{
			m_HomeBoxBehaviour = game.AddBehaviour<HomeBoxBehaviour>();
		}
	}
	else
	{
		auto homeBox = m_HomeBoxBehaviour.lock();

		if (homeBox != nullptr)
		{
			game.RemoveBehaviour(homeBox.get());
			m_HomeBoxBehaviour.reset();
		}
	}
}