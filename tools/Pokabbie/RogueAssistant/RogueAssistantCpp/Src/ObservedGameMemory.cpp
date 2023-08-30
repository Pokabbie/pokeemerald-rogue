#include "ObservedGameMemory.h"
#include "GameConnection.h"
#include "Log.h"


// Helpers
//

enum class ObservedMemoryID : u16
{
	GFHeader,
	RogueHeader,
	AssitantState,
	MultiplayerStatePtr,
	MultiplayerState,
};

inline static GameMessageID CreateMessageId(GameMessageChannel channel, ObservedMemoryID param)
{
	GameMessageID id;
	id.Channel = channel;
	id.Param16 = (u16)param;
	return id;
}

// ObservedBlob
//

ObservedBlob::ObservedBlob(size_t size)
	: m_IsValid(false)
{
	Resize(size);
}

void ObservedBlob::Resize(size_t size)
{
	m_Data.resize(size);
}

bool ObservedBlob::SetData(u8 const* data, size_t size)
{
	ASSERT_MSG(size == GetSize(), "Unexpected size");
	if (size == GetSize())
	{
		memcpy(m_Data.data(), data, GetSize());
		m_IsValid = true;
		return true;
	}

	return false;
}

void ObservedBlob::Clear()
{
	m_IsValid = false;
}

// ObservedGameMemory
//

ObservedGameMemory::ObservedGameMemory(GameConnection& game)
	: m_Game(game)
	//, m_AssistantState(ObservedGameMemoryType::Immediate, game.GetGameRogueHeader().assistantState, sizeof(GameStructures::RogueAssistantState))
	//, m_RogueNetMultiplayer(ObservedGameMemoryType::Virtual, game.GetGameRogueHeader().multiplayerPtr, game.GetGameRogueHeader().netMultiplayerSize)
{
}

void ObservedGameMemory::Update()
{
	if (!m_GFRomHeader.IsValid())
	{
		GameMessageID messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::GFHeader);
		m_Game.ReadRequest(messageId, GameAddresses::c_GFHeaderAddress, m_GFRomHeader.GetSize());
	}
	else if(!m_RogueHeader.IsValid())
	{
		GameMessageID messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::RogueHeader);
		m_Game.ReadRequest(messageId, m_GFRomHeader->rogueAssistantHeader, m_RogueHeader.GetSize());
	}
	else
	{
		// Both headers are valid, so can update other memory now
		GameMessageID messageId;

		// Grab assistant state
		//
		messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::AssitantState);
		m_Game.ReadRequest(messageId, m_RogueHeader->assistantState, m_AssistantState.GetSize());

		// Grab multiplayer state, if we have one
		//
		messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::MultiplayerStatePtr);
		m_Game.ReadRequest(messageId, m_RogueHeader->multiplayerPtr, m_MultiplayerStatePtr.GetSize());

		if (m_MultiplayerStatePtr.Get() != 0)
		{
			if (m_MultiplayerState.GetSize() != m_RogueHeader->netMultiplayerSize)
				m_MultiplayerState.Resize(m_RogueHeader->netMultiplayerSize);

			messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::MultiplayerState);
			m_Game.ReadRequest(messageId, m_MultiplayerStatePtr.Get(), m_MultiplayerState.GetSize());
		}
		else
		{
			// Pointing to null
			m_MultiplayerState.Clear();
		}
	}
}

void ObservedGameMemory::OnRecieveMessage(GameMessageID messageId, u8 const* data, size_t size)
{
	ObservedMemoryID memoryId = (ObservedMemoryID)messageId.Param16;

	switch (memoryId)
	{
	case ObservedMemoryID::GFHeader:
		if (m_GFRomHeader.SetData(data, size))
		{
			// A couple of verification handshakes are placed in the handshake, so check those before continuing
			if (m_GFRomHeader->rogueAssistantHandshake1 != 20012 || m_GFRomHeader->rogueAssistantHandshake2 != 30035)
			{
				LOG_WARN("Invalid GF Header handshakes");
				m_Game.Disconnect();
				return;
			}
		}
		else
		{
			LOG_WARN("Invalid GF Header size");
			m_Game.Disconnect();
		}
		break;

	case ObservedMemoryID::RogueHeader:
		if (!m_RogueHeader.SetData(data, size))
		{
			LOG_WARN("Invalid GF Header size");
			m_Game.Disconnect();
		}
		break;

	case ObservedMemoryID::AssitantState:
		m_AssistantState.SetData(data, size);
		break;

	case ObservedMemoryID::MultiplayerStatePtr:
		m_MultiplayerStatePtr.SetData(data, size);
		break;

	case ObservedMemoryID::MultiplayerState:
		m_MultiplayerState.SetData(data, size);
		break;
	}
}

bool ObservedGameMemory::IsMuliplayerStateValid() const
{
	return m_MultiplayerStatePtr.IsValid() && m_MultiplayerStatePtr.Get() != 0 && m_MultiplayerState.IsValid();
}