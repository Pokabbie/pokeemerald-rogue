#include "ObservedGameMemory.h"
#include "GameConnection.h"
#include "Log.h"

ObservedGameMemoryEntry::ObservedGameMemoryEntry(ObservedGameMemoryType type, GameAddress address, size_t size)
	: m_MemoryType(type)
	, m_Address(address)
	, m_IsNull(true)
{
	m_Data.resize(size);
}

void ObservedGameMemoryEntry::Update(GameConnection& game)
{
	if (m_ActiveTask == nullptr || m_ActiveTask->HasCompleted())
	{
		if (m_MemoryType == ObservedGameMemoryType::Virtual)
		{
			m_ActiveTask = game.ReadData(m_Address, sizeof(GameAddress));
			m_ActiveTask->Then([&](u8 const* data, size_t size)
				{
					GameAddress const* address = (GameAddress const*)data;

					if (*address == 0)
					{
						m_IsNull = true;
					}
					else
					{
						// Now to the actual lookup with the known address
						m_ActiveTask = game.ReadData(*address, GetSize());
						m_ActiveTask->Then([&](u8 const* data, size_t size)
							{
								OnRecvData(data, size);
							}
						);
					}
				}
			);
		}
		else
		{
			m_ActiveTask = game.ReadData(m_Address, GetSize());
			m_ActiveTask->Then([&](u8 const* data, size_t size)
				{
					OnRecvData(data, size);
				}
			);
		}
	}
}

void ObservedGameMemoryEntry::OnRecvData(u8 const* data, size_t size)
{
	ASSERT_MSG(size == GetSize(), "Size invalid");
	memcpy(m_Data.data(), data, GetSize());
	m_IsNull = false;
}

ObservedGameMemory::ObservedGameMemory(GameConnection& game)
	: m_Game(game)
	, m_AssistantState(ObservedGameMemoryType::Immediate, game.GetGameRogueHeader().assistantState, sizeof(GameStructures::RogueAssistantState))
	, m_RogueNetMultiplayer(ObservedGameMemoryType::Virtual, game.GetGameRogueHeader().multiplayerPtr, game.GetGameRogueHeader().netMultiplayerSize)
{
}

void ObservedGameMemory::Update()
{
	m_AssistantState.Update(m_Game);
	m_RogueNetMultiplayer.Update(m_Game);
}