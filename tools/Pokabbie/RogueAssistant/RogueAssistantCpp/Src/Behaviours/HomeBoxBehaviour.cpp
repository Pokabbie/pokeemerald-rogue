#include "Behaviours/CommonBehaviour.h"
#include "Behaviours/HomeBoxBehaviour.h"
#include "DataStream.h"
#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"
#include "UserData.h"

void HomeBoxBehaviour::OnAttach(GameConnection& game)
{
	m_State = State::First;
	m_ActiveBoxData.clear();
}

void HomeBoxBehaviour::OnDetach(GameConnection& game)
{
	m_BoxWriteRequest = std::queue<BoxWriteRequest>(); // make sure we finish writing any pending writes
	HandlePendingFileWrite(game);
}

void HomeBoxBehaviour::OnUpdate(GameConnection& game)
{
	HandlePendingFileWrite(game);

	if (game.GetObservedGameMemory().IsHomeBoxStateValid())
	{
		GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
		u8 const* homeBoxState = game.GetObservedGameMemory().GetHomeBoxStateBlob();
		GameAddress writeAddress = game.GetObservedGameMemory().GetHomeBoxStatePtr();

		switch (m_State)
		{
		case HomeBoxBehaviour::State::OpenOfflineFile:
		{
			std::wstring filePath = L"PUT_TID_HERE/boxes.dat";

			// Set defaults
			m_StoredBoxData.resize(0);
			m_HasPendingFileWrite = false;
			//m_HasPendingFileWrite = true;

			if (UserData::DoesFileExist(filePath))
			{
				std::fstream fileStream;

				if (UserData::TryOpenReadFile(filePath, fileStream) && LoadDataFromFile(game, fileStream))
				{
					// successfully parsed existing data
					//m_HasPendingFileWrite = false;
				}
				else
				{
					// Failed, so reset
					//m_StoredBoxData.resize(0);
					//m_HasPendingFileWrite = true;
				}
			}

			// Open the file for us to write to now so we can hold onto it during this interaction
			m_WriteFilePath = filePath;
			//m_HasPendingFileWrite = true;

			m_State = HomeBoxBehaviour::State::InitialiseBoxData;
		}
		break;

		case HomeBoxBehaviour::State::InitialiseBoxData:
		{
			if (m_StoredBoxData.empty())
			{
				// Load data
				m_StoredBoxData.resize(rogueHeader.homeTotalBoxCount - rogueHeader.homeLocalBoxCount);

				// Fill with nothing
				for (size_t i = 0; i < m_StoredBoxData.size(); ++i)
				{
					// We expect the game to handle the default state, so just fill with 0's here for simplicity
					m_StoredBoxData[i].m_MinimalData.resize(rogueHeader.homeMinimalBoxSize, 0);
					m_StoredBoxData[i].m_MonData.resize(rogueHeader.homeDestMonSize, 0);
				}
				break;
			}

			if (m_ActiveBoxData.size() != rogueHeader.homeLocalBoxCount)
			{
				// Need to keep requesting boxes
				if (game.GetObservedGameMemory().RequestPokemonStorageData(m_ActiveBoxData.size()))
				{
					m_ActiveBoxData.emplace_back();
					m_State = HomeBoxBehaviour::State::WaitingForBoxData;
				}
			}
			else
			{
				// We're done gathering data from client, so now fill in the local stored data too!
				// 
				for (size_t i = 0; i < m_StoredBoxData.size(); ++i)
				{
					m_ActiveBoxData.emplace_back();

					// TEMP
					m_ActiveBoxData.back().m_MinimalData = m_StoredBoxData[i].m_MinimalData;
					m_ActiveBoxData.back().m_MonData = m_StoredBoxData[i].m_MonData;
				}

				ASSERT_MSG(m_ActiveBoxData.size() == rogueHeader.homeTotalBoxCount, "Unexpected Active Box Count");

				m_State = HomeBoxBehaviour::State::SendGameDataInit;
			}

		}
		break;

		case HomeBoxBehaviour::State::WaitingForBoxData:
		{
			if (game.GetObservedGameMemory().IsPokemonStorageBlobReady())
			{
				// Initialise and then loop
				InitaliseLocalBoxData(game, m_ActiveBoxData.size() - 1);
				m_State = HomeBoxBehaviour::State::InitialiseBoxData;
			}
		}
		break;

		case HomeBoxBehaviour::State::SendGameDataInit:
		{
			// Send the current stored boxe states
			for (u32 boxId = rogueHeader.homeLocalBoxCount; boxId < rogueHeader.homeTotalBoxCount; ++boxId)
			{
				WriteMinimalBox(game, boxId, m_ActiveBoxData[boxId].m_MinimalData.data());
			}

			// Reset index orders
			{
				m_LocalActiveBoxIndices.clear();
				m_RemoteActiveBoxIndices.clear();
				for (u32 i = 0; i < rogueHeader.homeTotalBoxCount; ++i)
				{
					m_LocalActiveBoxIndices.push_back(static_cast<u8>(i));
					m_RemoteActiveBoxIndices.push_back(static_cast<u8>(i));
				}

				for (u32 i = 0; i < rogueHeader.homeLocalBoxCount; ++i)
				{
					m_LocalActiveBoxIndices.push_back(static_cast<u8>(i));
				}

				game.WriteRequest(CreateAnonymousMessageId(), writeAddress + rogueHeader.homeRemoteIndexOrderOffset, m_RemoteActiveBoxIndices.data(), rogueHeader.homeTotalBoxCount);
			}

			m_State = HomeBoxBehaviour::State::WaitForInit;
		}
		break;

		case HomeBoxBehaviour::State::WaitForInit:
		{
			// Wait for write to go through to game
			if (homeBoxState[rogueHeader.homeRemoteIndexOrderOffset + rogueHeader.homeTotalBoxCount - 1] != 255)
			{
				m_State = HomeBoxBehaviour::State::Update;
			}
		}
		break;

		case HomeBoxBehaviour::State::Update:
		{
			// Only update the active write
			if (PumpWriteMonBox(game))
				break;

			// Copy the indices here (So we can save the data once we're done)
			memcpy(m_RemoteActiveBoxIndices.data(), &homeBoxState[rogueHeader.homeRemoteIndexOrderOffset], rogueHeader.homeTotalBoxCount);
			
			// Process any boxes which have been swapped
			for (u32 i = 0; i < rogueHeader.homeTotalBoxCount; ++i)
			{
				if (m_RemoteActiveBoxIndices[i] != m_LocalActiveBoxIndices[i])
				{
					// Send over any pokemon box contents for boxes which differ
					if(i < rogueHeader.homeLocalBoxCount)
						BeginWriteMonBox(game, i, m_ActiveBoxData[m_RemoteActiveBoxIndices[i]].m_MonData.data());

					m_LocalActiveBoxIndices[i] = m_RemoteActiveBoxIndices[i];
					m_HasPendingFileWrite = true;
				}
			}
		}
		break;
		}
	}
}

void HomeBoxBehaviour::InitaliseLocalBoxData(GameConnection& game, u32 boxId)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	u8 const* homeBoxState = game.GetObservedGameMemory().GetHomeBoxStateBlob();
	
	BoxData& targetBox = m_ActiveBoxData[boxId];

	if (boxId < rogueHeader.homeLocalBoxCount)
	{
		// Copy minimal data
		targetBox.m_MinimalData.resize(rogueHeader.homeMinimalBoxSize);
		memcpy(targetBox.m_MinimalData.data(), GetMinimalBoxPtr(game, boxId), rogueHeader.homeMinimalBoxSize);

		// Copy the actual box mons we have just gathered
		targetBox.m_MonData.resize(rogueHeader.homeDestMonSize);
		memcpy(targetBox.m_MonData.data(), game.GetObservedGameMemory().GetPokemonStorageBlob(), rogueHeader.homeDestMonSize);
	}
	else
	{
		// Load data from disk
		// TODO
	}
}

void HomeBoxBehaviour::WriteMinimalBox(GameConnection& game, u32 boxId, u8 const* data)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	GameAddress writeAddress = game.GetObservedGameMemory().GetHomeBoxStatePtr();

	game.WriteRequest(
		CreateAnonymousMessageId(),
		writeAddress + rogueHeader.homeMinimalBoxOffset + rogueHeader.homeMinimalBoxSize * boxId,
		data,
		rogueHeader.homeMinimalBoxSize
	);
}

u8 const* HomeBoxBehaviour::GetMinimalBoxPtr(GameConnection& game, u32 boxId)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	u8 const* homeBoxState = game.GetObservedGameMemory().GetHomeBoxStateBlob();

	return &homeBoxState[rogueHeader.homeMinimalBoxOffset + rogueHeader.homeMinimalBoxSize * boxId];
}

void HomeBoxBehaviour::BeginWriteMonBox(GameConnection& game, u32 boxId, u8 const* data)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	BoxWriteRequest boxWriteRequest;
	boxWriteRequest.m_BoxId = boxId;
	boxWriteRequest.m_Data = data;

	boxWriteRequest.m_Offset = 0;
	boxWriteRequest.m_BytesRemaining = rogueHeader.homeDestMonSize;

	m_BoxWriteRequest.push(std::move(boxWriteRequest));
}

bool HomeBoxBehaviour::PumpWriteMonBox(GameConnection& game)
{
	if (m_BoxWriteRequest.size() != 0)
	{
		BoxWriteRequest& boxWriteRequest = m_BoxWriteRequest.front();

		GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
		GameAddress writeAddress = game.GetObservedGameMemory().GetPokemonStoragePtr();

		// Send in parts
		if (boxWriteRequest.m_BytesRemaining != 0)
		{
			size_t writeSize = std::min<size_t>(boxWriteRequest.m_BytesRemaining, 1024);

			game.WriteRequest(
				CreateAnonymousMessageId(),
				writeAddress + rogueHeader.homeDestMonSize * boxWriteRequest.m_BoxId + boxWriteRequest.m_Offset,
				boxWriteRequest.m_Data + boxWriteRequest.m_Offset,
				writeSize
			);
			game.ManualFlush();

			boxWriteRequest.m_BytesRemaining -= writeSize;
			boxWriteRequest.m_Offset += writeSize;
			return true;
		}
		else
		{
			m_BoxWriteRequest.pop();
			return true;
		}
	}

	return false;
}

u8 const* HomeBoxBehaviour::GetMonBoxPtr(GameConnection& game, u32 boxId)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();
	u8 const* homeBoxState = game.GetObservedGameMemory().GetHomeBoxStateBlob();

	return &homeBoxState[rogueHeader.homeDestMonOffset + rogueHeader.homeDestMonSize * boxId];
}

#define MAGIC_NUMBER_HEADER 3497814
#define MAGIC_NUMBER_FOOTER 7893612

void HomeBoxBehaviour::HandlePendingFileWrite(GameConnection& game)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	if (m_WriteFilePath.empty())
		return;

	if (m_ActiveBoxData.empty() || m_LocalActiveBoxIndices.empty())
		return;

	if (!m_BoxWriteRequest.empty())
		return;

	std::fstream fileStream;
	if (m_HasPendingFileWrite && UserData::TryOpenWriteFile(m_WriteFilePath, fileStream))
	{
		m_HasPendingFileWrite = false;

		LOG_INFO("Attempting to save Box Data...");

		DataStream stream;
		if (SerializeSavedData(game, stream))
		{
			fileStream.write(reinterpret_cast<char*>(stream.GetData()), stream.GetSize());
			fileStream.close();
			LOG_INFO("Saved.");
		}
		else
		{
			LOG_INFO("Failed to save.");
		}
	}
}

bool HomeBoxBehaviour::LoadDataFromFile(GameConnection& game, std::fstream& fileStream)
{
	m_StoredBoxData.resize(0);

	LOG_INFO("Attempting to load Box Data...");

	DataStream stream(fileStream);
	if (SerializeSavedData(game, stream))
	{
		LOG_INFO("Loaded.");
		return true;
	}
	else
	{
		LOG_INFO("Failed to load.");
		return false;
	}
}

bool HomeBoxBehaviour::SerializeSavedData(GameConnection& game, DataStream& stream)
{
	GameStructures::RogueAssistantHeader const& rogueHeader = game.GetObservedGameMemory().GetRogueHeader();

	u32 magicNumber = MAGIC_NUMBER_HEADER;
	u32 version = 0; // todo
	u32 boxCount = (rogueHeader.homeTotalBoxCount - rogueHeader.homeLocalBoxCount);
	u32 minimalSize = rogueHeader.homeMinimalBoxSize;
	u32 pokemonSize = rogueHeader.homeDestMonSize;

	stream.Serialize(magicNumber);
	stream.Serialize(version);
	stream.Serialize(boxCount);
	stream.Serialize(minimalSize);
	stream.Serialize(pokemonSize);

	if (magicNumber != MAGIC_NUMBER_HEADER)
	{
		LOG_ERROR("Failed header magic number match");
		return false;
	}

	if (version != 0)
	{
		LOG_ERROR("Failed version number match");
		return false;
	}

	if (minimalSize != rogueHeader.homeMinimalBoxSize)
	{
		LOG_ERROR("Failed minimal size match");
		return false;
	}

	if (pokemonSize != rogueHeader.homeDestMonSize)
	{
		LOG_ERROR("Failed pokemon size match");
		return false;
	}

	if (stream.IsWrite())
	{
		// For writing, we need to repoint which boxes we're going to write based on the game's reordering
		for (u32 i = rogueHeader.homeLocalBoxCount; i < rogueHeader.homeTotalBoxCount; ++i)
		{
			u32 boxId = m_LocalActiveBoxIndices[i];
			BoxData& boxData = m_ActiveBoxData[boxId];

			if (!stream.Serialize(boxData.m_MinimalData.data(), minimalSize))
			{
				LOG_ERROR("Failed box %i minimal data check", boxId);
			}

			if (!stream.Serialize(boxData.m_MonData.data(), pokemonSize))
			{
				LOG_ERROR("Failed box %i pokemon data  check", boxId);
			}

			u32 checksum = boxData.CalculateCheckSum();
			stream.Serialize(checksum);
		}
	}
	else
	{
		// For reading just popupare the stored box data list and we'll use them later
		while (m_StoredBoxData.size() < boxCount)
		{
			m_StoredBoxData.emplace_back();
			BoxData& boxData = m_StoredBoxData.back();

			boxData.m_MinimalData.resize(minimalSize, 0);
			boxData.m_MonData.resize(pokemonSize, 0);
		}

		for (u32 boxId = 0; boxId < boxCount; ++boxId)
		{
			BoxData& boxData = m_StoredBoxData[boxId];

			if (!stream.Serialize(boxData.m_MinimalData.data(), minimalSize))
			{
				LOG_ERROR("Failed box %i minimal data check", boxId);

				boxData.m_MinimalData.clear();
				boxData.m_MinimalData.resize(minimalSize, 0);
			}

			if (!stream.Serialize(boxData.m_MonData.data(), pokemonSize))
			{
				LOG_ERROR("Failed box %i pokemon data check", boxId);

				boxData.m_MonData.clear();
				boxData.m_MonData.resize(pokemonSize, 0);
			}

			u32 checksum;
			stream.Serialize(checksum);

			if (checksum != boxData.CalculateCheckSum())
			{
				LOG_ERROR("Failed box %i data checksum", boxId);

				boxData.m_MinimalData.clear();
				boxData.m_MinimalData.resize(minimalSize, 0);
				boxData.m_MonData.clear();
				boxData.m_MonData.resize(pokemonSize, 0);
			}
		}
	}

	magicNumber = MAGIC_NUMBER_FOOTER;
	stream.Serialize(magicNumber);

	if (magicNumber != MAGIC_NUMBER_FOOTER)
	{
		LOG_ERROR("Failed footer magic number check");
		return false;
	}

	return true;
}

u32 HomeBoxBehaviour::BoxData::CalculateCheckSum() const
{
	u32 output = 0;

	for (size_t i = 0; i < m_MinimalData.size(); ++i)
		output += m_MinimalData[i];

	for (size_t i = 0; i < m_MonData.size(); ++i)
		output += m_MonData[i];

	return output;
}