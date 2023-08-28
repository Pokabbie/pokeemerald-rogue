#pragma once
#include "Defines.h"
#include "GameData.h"
#include "SFML/Network.hpp"

#include <functional>
#include <memory>

class GameConnectionManager;

typedef std::function<void(u8 const* data, size_t size)> DataCallback;

enum class GameConnectionState
{
	AwaitingFirstHandshake,
	AwaitingSecondHandshake,
	Connected,
	Disconnected
};

class GameConnection
{
	friend GameConnectionManager;
public:
	GameConnection();
	~GameConnection();

	void Update();
	void Disconnect();

	inline bool IsReady() const { return m_State == GameConnectionState::Connected; }
	inline bool HasDisconnected() const { return m_State == GameConnectionState::Disconnected; }

	GameStructures::GFRomHeader const& GetGameGFHeader() const;
	GameStructures::RogueAssistantHeader const& GetGameRogueHeader() const;

	void WriteData(size_t addr, void const* data, size_t size, DataCallback callback = nullptr);
	void ReadData(size_t addr, size_t size, DataCallback callback = nullptr);

	template<typename T>
	inline void WriteValue(size_t addr, T& value, std::function<void(T const&)> callback = nullptr);

	template<typename T>
	inline void ReadValue(size_t addr, std::function<void(T const&)> callback = nullptr);

private:
	void OnRecieveData(u8* data, size_t size);

	bool HandleExpectedHandshake(std::string const& expectedHandshake, u8* data, size_t size);
	void ParseGameHeaders();

	u8 RegisterRecieveCallback(DataCallback callback);

	static std::string const c_FirstHandshake;
	static std::string const c_SecondHandshake;

	sf::TcpSocket m_Socket;
	GameConnectionState m_State;

	bool m_GameHeadersValid;
	GameStructures::GFRomHeader m_GFRomHeader;
	GameStructures::RogueAssistantHeader m_RogueHeader;

	u8 m_RecieveBuffer[2048];
	u8 m_SendBuffer[2048];
	std::vector<DataCallback> m_RecvCallbacks;

};

// Templates
//
template<typename T>
void GameConnection::WriteValue(size_t addr, T& value, std::function<void(T const&)> callback)
{ 
	if (callback != nullptr)
		WriteData(addr, &value, sizeof(T), [callback](u8 const* data, size_t size) 
			{
				T const* value = (T const*)data;
				callback(*value);
			}
		);
	else
		WriteData(addr, &value, sizeof(T));
}

template<typename T>
void GameConnection::ReadValue(size_t addr, std::function<void(T const&)> callback) 
{
	if (callback != nullptr)
		ReadData(addr, sizeof(T), [callback](u8 const* data, size_t size)
			{
				T const* value = (T const*)data;
				callback(*value);
			}
		);
	else
		ReadData(addr, sizeof(T));
}