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
	HomeBoxStatePtr,
	HomeBoxState,
	GamePokemonStorageData,
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
{
}

void ObservedGameMemory::Update()
{
	if (!m_GFRomHeader.IsValid())
	{
		GameMessageID messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::GFHeader);
		m_Game.ReadRequest(messageId, GameAddresses::c_GFHeaderAddress, m_GFRomHeader.GetSize());
	}
	else if (!m_RogueHeader.IsValid())
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
		// NOTE: This is laggy to get, as it's so large
		// Should consider setting up a system which will grab in smaller sizes over multiple frames
		//messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::AssitantState);
		//m_Game.ReadRequest(messageId, m_RogueHeader->assistantState, m_AssistantState.GetSize());

		// Grab multiplayer state, if we have one
		//
		messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::MultiplayerStatePtr);
		m_Game.ReadRequest(messageId, m_RogueHeader->multiplayerPtr, m_MultiplayerStatePtr.GetSize());

		if (m_MultiplayerStatePtr.IsValid() && m_MultiplayerStatePtr.Get() != 0)
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

		// Grab Home Box state
		//
		messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::HomeBoxStatePtr);
		m_Game.ReadRequest(messageId, m_RogueHeader->homeBoxPtr, m_HomeBoxStatePtr.GetSize());

		if (m_HomeBoxStatePtr.IsValid() && m_HomeBoxStatePtr.Get() != 0)
		{
			if (m_HomeBoxState.GetSize() != m_RogueHeader->homeBoxSize)
				m_HomeBoxState.Resize(m_RogueHeader->homeBoxSize);

			messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::HomeBoxState);
			m_Game.ReadRequest(messageId, m_HomeBoxStatePtr.Get(), m_HomeBoxState.GetSize());
		}
		else
		{
			// Pointing to null
			m_HomeBoxState.Clear();
			m_PokemonStorageData.Clear();
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

	case ObservedMemoryID::HomeBoxStatePtr:
		m_HomeBoxStatePtr.SetData(data, size);
		break;

	case ObservedMemoryID::HomeBoxState:
		m_HomeBoxState.SetData(data, size);
		break;

	case ObservedMemoryID::GamePokemonStorageData:
		m_PokemonStorageData.SetData(data, size);
		break;
	}
}

bool ObservedGameMemory::AreHeadersValid() const
{
	return m_GFRomHeader.IsValid() && m_RogueHeader.IsValid();
}

bool ObservedGameMemory::IsMultiplayerStateValid() const
{
	return m_MultiplayerStatePtr.IsValid() && m_MultiplayerStatePtr.Get() != 0 && m_MultiplayerState.IsValid();
}

bool ObservedGameMemory::IsHomeBoxStateValid() const
{
	return m_HomeBoxStatePtr.IsValid() && m_HomeBoxStatePtr.Get() != 0 && m_HomeBoxState.IsValid();
}

GameAddress ObservedGameMemory::GetPokemonStoragePtr() const
{
	if (IsHomeBoxStateValid())
	{
		u8 const* ptr = &m_HomeBoxState.GetData()[m_RogueHeader->homeDestMonOffset];
		GameAddress storagePtrAddr = *(GameAddress*)ptr;
		return storagePtrAddr;
	}

	return 0;
}

bool ObservedGameMemory::RequestPokemonStorageData(u32 boxId)
{
	m_PokemonStorageData.Clear();

	if (IsHomeBoxStateValid())
	{
		GameStructures::RogueAssistantHeader const& rogueHeader = m_Game.GetObservedGameMemory().GetRogueHeader();
		u8 const* homeBoxState = GetHomeBoxStateBlob();

		GameMessageID messageId = CreateMessageId(GameMessageChannel::CommonRead, ObservedMemoryID::GamePokemonStorageData);

		m_Game.ReadRequest(messageId, GetPokemonStoragePtr() + rogueHeader.homeDestMonSize * boxId, rogueHeader.homeDestMonSize);
		m_PokemonStorageData.Resize(rogueHeader.homeDestMonSize);
		return true;
	}

	return false;
	//m_PokemonStorageData.Resize()
}