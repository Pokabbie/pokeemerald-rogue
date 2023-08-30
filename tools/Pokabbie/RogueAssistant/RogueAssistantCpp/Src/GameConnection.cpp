#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"

#include <SFML/Network.hpp>

std::string const GameConnection::c_FirstHandshake = "3to8UEaoManH7wB4lKlLRgywSHHKmI0g";
std::string const GameConnection::c_SecondHandshake = "Em68TrzBAFlyhBCOm4XQIjGWbdNhuplY";

GameConnection::GameConnection()
	: m_State(GameConnectionState::AwaitingFirstHandshake)
	, m_GameRPCs(*this)
	, m_SendSize(0)
	, m_GameHeadersValid(false)
{
	m_ObservedGameMemory = std::make_unique<ObservedGameMemory>(*this);
	m_Socket.setBlocking(false);
}

GameConnection::~GameConnection()
{
	m_Socket.disconnect();
}

void GameConnection::Update()
{
	size_t recvSize;
	if (m_Socket.receive(m_RecieveBuffer, sizeof(m_RecieveBuffer), recvSize) == sf::Socket::Done)
	{
		OnRecieveData(m_RecieveBuffer, recvSize);
	}
	
	if (IsReady())
	{
		m_ObservedGameMemory->Update();

		if (m_GameHeadersValid)
		{
			m_GameRPCs.Update();
		}
	}

	FlushCommands();

	// Update tasks
	// TODO 
}

void GameConnection::Disconnect()
{
	m_Socket.disconnect();
	m_State = GameConnectionState::Disconnected;
}

GameStructures::GFRomHeader const& GameConnection::GetGameGFHeader() const
{
	ASSERT_MSG(m_GameHeadersValid, "Attempt to use invalid header");
	return m_GFRomHeader;
}

GameStructures::RogueAssistantHeader const& GameConnection::GetGameRogueHeader() const
{
	ASSERT_MSG(m_GameHeadersValid, "Attempt to use invalid header");
	return m_RogueHeader;
}

ObservedGameMemory const& GameConnection::GetObservedGameMemory() const
{
	ASSERT_MSG(m_ObservedGameMemory != nullptr, "Attempt to use observed game memory before initialise");
	return *m_ObservedGameMemory.get();
}

// helper todo - should move
template<typename T>
bool str2num(T& i, char const* s, int base = 0)
{
	char* end;
	long  l;
	errno = 0;
	l = strtol(s, &end, base);
	if ((errno == ERANGE && l == LONG_MAX))// || l > (long)std::numeric_limits<T>::max()) 
	{
		return false;
	}
	if ((errno == ERANGE && l == LONG_MIN))// || l < (long)std::numeric_limits<T>::min()) 
	{
		return false;
	}
	if (*s == '\0' || *end != '\0') {
		return false;
	}
	i = (T)l;
	return true;
}

void GameConnection::OnRecieveData(u8* data, size_t size)
{
	switch (m_State)
	{
	case GameConnectionState::AwaitingFirstHandshake:
		if (HandleExpectedHandshake(c_FirstHandshake, data, size))
		{
			// Tell script to continue
			m_Socket.send("con", 3);
			m_State = GameConnectionState::AwaitingSecondHandshake;
		}
		else
		{
			Disconnect();
		}
		break;

	case GameConnectionState::AwaitingSecondHandshake:
		if (HandleExpectedHandshake(c_SecondHandshake, data, size))
		{
			m_State = GameConnectionState::Connected;
			LOG_INFO("Game: Connection accepted");
		}
		else
		{
			Disconnect();
		}
		break;

	case GameConnectionState::Connected:

		// Attempt to read and call registered callbacks
		std::string readId;
		std::string readSize;

		size_t offset = 0;
		u8 readMode = 0;

		for (; offset < size; ++offset)
		{
			if (data[offset] == ';')
			{
				if (readMode == 1)
				{
					// Read both readId and readSize
					GameMessageID messageId;
					u32 blockSize = 0;
					if (str2num(messageId.CompactedID, readId.c_str()) && str2num(blockSize, readSize.c_str()))
					{
						OnRecieveMessage(messageId, &data[offset + 1], blockSize);
					}
					else
					{
						ASSERT_FAIL("Failed to parse incoming recv");
					}

					offset += blockSize;

					// Clear for next 
					readId.clear();
					readSize.clear();
					readMode = 0;
				}
				else
					++readMode;
			}
			else if(readMode == 0)
				readId += data[offset];
			else
				readSize += data[offset];
		}

		break;
	}
}

void GameConnection::OnRecieveMessage(GameMessageID messageId, u8 const* data, size_t size)
{
	switch (messageId.Channel)
	{
	case GameMessageChannel::CommonRead:
		m_ObservedGameMemory->OnRecieveMessage(messageId, data, size);
		break;

	default:
		break;
	}
}

bool GameConnection::HandleExpectedHandshake(std::string const& expectedHandshake, u8* data, size_t size)
{
	if (size != expectedHandshake.length())
	{
		LOG_WARN("Invalid incoming handshake size");
	}
	else
	{
		int result = strncmp(expectedHandshake.c_str(), (const char*)data, size);
		if (result == 0)
		{
			// Handshake matches
			return true;
		}
		else
		{
			LOG_WARN("Unexpected handshake");
		}
	}

	return false;
}


void GameConnection::WriteRequest(GameMessageID messageId, size_t addr, void const* data, size_t size)
{
	ASSERT_MSG(IsReady(), "Attempting to write data, but not ready");

	// Really inefficient, but works...
	// Write name then numbers in ascii split by ;
	std::string command = "writeBytes;" + std::to_string(messageId.CompactedID) + ";" + std::to_string(addr);
	u8 const* read = reinterpret_cast<u8 const*>(data);

	for (size_t i = 0; i < size; ++i)
	{
		command += ";" + std::to_string(read[i]);
	}

	SendCommand(command);
}

void GameConnection::ReadRequest(GameMessageID messageId, size_t addr, size_t size)
{
	ASSERT_MSG(IsReady(), "Attempting to write data, but not ready");

	// Really inefficient, but works...
	// Write name then numbers in ascii split by ;
	std::string command = "readBytes;" + std::to_string(messageId.CompactedID) + ";" + std::to_string(addr) + ";" + std::to_string(size);

	SendCommand(command);
}

void GameConnection::SendCommand(std::string const& command)
{
	size_t size = command.length();

	if (m_SendSize != 0)
	{
		// Append : between multiple commands
		if (m_SendSize + size + 1 >= sizeof(m_SendBuffer))
		{
			// Reached capacity, so have to send what we have buffered now
			FlushCommands();
		}
	}

	memcpy(m_SendBuffer + m_SendSize, command.c_str(), size);
	m_SendSize += size;
	m_SendBuffer[m_SendSize++] = ':';
}

void GameConnection::FlushCommands()
{
	if (m_SendSize != 0)
	{
		// Send data in a big batch
		size_t sentAmount;
		size_t offset = 0;
		m_Socket.send(m_SendBuffer, m_SendSize, sentAmount);

		if (m_SendSize != sentAmount)
		{
			ASSERT_FAIL("Couldn't set every things! (Need to see when this happens to decide how to handle it)");
		}

		m_SendSize = 0;
	}
}
