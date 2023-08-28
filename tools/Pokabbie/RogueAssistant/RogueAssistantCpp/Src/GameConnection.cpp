#include "GameConnection.h"
#include "GameData.h"
#include "Log.h"

#include <SFML/Network.hpp>

std::string const GameConnection::c_FirstHandshake = "3to8UEaoManH7wB4lKlLRgywSHHKmI0g";
std::string const GameConnection::c_SecondHandshake = "Em68TrzBAFlyhBCOm4XQIjGWbdNhuplY";

GameConnection::GameConnection()
	: m_State(GameConnectionState::AwaitingFirstHandshake)
	, m_GameHeadersValid(false)
{
	m_Socket.setBlocking(false);
	m_RecvCallbacks.resize(16); // Reserve callback size
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

// helper todo - should move
template<typename T>
bool str2num(T& i, char const* s, int base = 0)
{
	char* end;
	long  l;
	errno = 0;
	l = strtol(s, &end, base);
	if ((errno == ERANGE && l == LONG_MAX) || l > std::numeric_limits<T>::max()) {
		return false;
	}
	if ((errno == ERANGE && l == LONG_MIN) || l < std::numeric_limits<T>::min()) {
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

			// Kick off read for headers (We can disconnect if these are invalid)
			ParseGameHeaders();
		}
		else
		{
			Disconnect();
		}
		break;

	case GameConnectionState::Connected:

		// Attempt to read and call registered callbacks
		std::string temp;
		size_t offset;

		for (offset = 0; offset < size; ++offset)
		{
			if (data[offset] == ';')
			{
				++offset;
				break;
			}
			else
				temp += data[offset];
		}

		if (offset < size)
		{
			u8 id;
			if (str2num(id, temp.c_str()))
			{
				if (id < m_RecvCallbacks.size() && m_RecvCallbacks[id] != nullptr)
				{
					// Call data recv callback and then clear the slot
					m_RecvCallbacks[id](&data[offset], size - offset);
					m_RecvCallbacks[id] = nullptr;
				}
			}
		}

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

void GameConnection::ParseGameHeaders()
{
	// Read the GF header first
	ReadData(GameAddresses::c_GFHeaderAddress, sizeof(m_GFRomHeader), [&](u8 const* data, size_t size)
		{
			if (size == sizeof(m_GFRomHeader))
			{
				memcpy(&m_GFRomHeader, data, sizeof(m_GFRomHeader));

				// A couple of verification handshakes are placed in the handshake, so check those before continuing
				if (m_GFRomHeader.rogueAssistantHandshake1 != 20012 || m_GFRomHeader.rogueAssistantHandshake2 != 30035)
				{
					LOG_WARN("Game: Invalid GF Header handshakes");
					Disconnect();
					return;
				}

				// Now read Rogue Header
				ReadData(m_GFRomHeader.rogueAssistantHeader, sizeof(m_RogueHeader), [&](u8 const* data, size_t size)
					{
						if (size == sizeof(m_RogueHeader))
						{
							memcpy(&m_RogueHeader, data, sizeof(m_RogueHeader));
							m_GameHeadersValid = true;

							// TEST
							//u16 token = 124;
							//WriteValue(m_RogueHeader.inCommBuffer, token);
						}
						else
						{
							LOG_WARN("Game: Failed to read Rogue Header");
							Disconnect();
						}
					}
				);
			}
			else
			{
				LOG_WARN("Game: Invalid GF Header size");
				Disconnect();
			}
		}
	);
}


void GameConnection::WriteData(size_t addr, void const* data, size_t size, DataCallback callback)
{
	ASSERT_MSG(IsReady(), "Attempting to write data, but not ready");

	// Really inefficient, but works...
	// Write name then numbers in ascii split by ;
	std::string command = "writeBytes;" + std::to_string(RegisterRecieveCallback(callback)) + ";" + std::to_string(addr);
	u8 const* read = reinterpret_cast<u8 const*>(data);

	for (size_t i = 0; i < size; ++i)
	{
		command += ";" + std::to_string(read[i]);
	}

	m_Socket.send(command.c_str(), command.length());
}

void GameConnection::ReadData(size_t addr, size_t size, DataCallback callback)
{
	ASSERT_MSG(IsReady(), "Attempting to read data, but not ready");

	// Really inefficient, but works...
	// Write name then numbers in ascii split by ;
	std::string command = "readBytes;" + std::to_string(RegisterRecieveCallback(callback)) + ";" + std::to_string(addr) + ";" + std::to_string(size);
	m_Socket.send(command.c_str(), command.length());
}

u8 GameConnection::RegisterRecieveCallback(DataCallback callback)
{
	if (callback == nullptr)
	{
		// null callback
		return 0;
	}

	// Skip slot 0, as that's reserved for null i.e. no callback
	for (u8 i = 1; i < (u8)m_RecvCallbacks.size(); ++i)
	{
		if (m_RecvCallbacks[i] == nullptr)
		{
			m_RecvCallbacks[i] = callback;
			return i;
		}
	}

	// All slots are filled, so need to expand
	u8 slot = (u8)m_RecvCallbacks.size();
	m_RecvCallbacks.push_back(callback);

	ASSERT_MSG(slot != 0, "Recieved data callback overflow");
	return slot;
}