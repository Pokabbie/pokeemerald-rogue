#pragma once
#include "Defines.h"
#include "GameData.h"
#include "GameConnectionRPCs.h"
#include "GameConnectionTask.h"
#include "ObservedGameMemory.h"
#include "SFML/Network.hpp"

#include <functional>
#include <memory>

class GameConnectionManager;
class IGameConnectionTask;

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

	GameConnectionTaskRef WriteData(size_t addr, void const* data, size_t size);
	GameConnectionTaskRef ReadData(size_t addr, size_t size);

	ObservedGameMemory const& GetObservedGameMemory() const;

	bool IsRogueAssistantStateValid() const;
	GameStructures::RogueAssistantState const& GetRogueAssistantState() const;

	//template<typename T>
	//inline void WriteValue(size_t addr, T& value);
	//
	//template<typename T>
	//inline void ReadValue(size_t addr);

private:
	void SendCommand(std::string const& command);
	void FlushCommands();
	void OnRecieveData(u8* data, size_t size);

	bool HandleExpectedHandshake(std::string const& expectedHandshake, u8* data, size_t size);
	void ParseGameHeaders();

	GameConnectionTaskRef AllocConnectionTask();

	static std::string const c_FirstHandshake;
	static std::string const c_SecondHandshake;

	sf::TcpSocket m_Socket;
	GameConnectionState m_State;

	bool m_GameHeadersValid;
	GameStructures::GFRomHeader m_GFRomHeader;
	GameStructures::RogueAssistantHeader m_RogueHeader;

	u8 m_RecieveBuffer[2048];
	u8 m_SendBuffer[2048];
	size_t m_SendSize;
	std::vector<GameConnectionTaskRef> m_RecvTasks;

	RPCQueue m_GameRPCs;
	std::unique_ptr<ObservedGameMemory> m_ObservedGameMemory;

};

// Templates
//
//template<typename T>
//void GameConnection::WriteValue(size_t addr, T& value, std::function<void(T const&)> callback)
//{ 
//	if (callback != nullptr)
//		WriteData(addr, &value, sizeof(T), [callback](u8 const* data, size_t size) 
//			{
//				T const* value = (T const*)data;
//				callback(*value);
//			}
//		);
//	else
//		WriteData(addr, &value, sizeof(T));
//}
//
//template<typename T>
//void GameConnection::ReadValue(size_t addr, std::function<void(T const&)> callback) 
//{
//	if (callback != nullptr)
//		ReadData(addr, sizeof(T), [callback](u8 const* data, size_t size)
//			{
//				T const* value = (T const*)data;
//				callback(*value);
//			}
//		);
//	else
//		ReadData(addr, sizeof(T));
//}