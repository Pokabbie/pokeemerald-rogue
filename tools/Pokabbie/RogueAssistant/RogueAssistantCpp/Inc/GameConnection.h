#pragma once
#include "Defines.h"
#include "GameData.h"
#include "GameConnectionBehaviour.h"
#include "GameConnectionMessage.h"
#include "GameConnectionRPCs.h"
#include "ObservedGameMemory.h"
#include "SFML/Network.hpp"
#include "Timer.h"

#include <functional>
#include <memory>

class GameConnectionManager;
class IGameConnectionTask;



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

	void AddBehaviour(IGameConnectionBehaviour* behaviour);
	void RemoveBehaviour(IGameConnectionBehaviour* behaviour);

	template<typename T>
	std::shared_ptr<T> AddBehaviour();

	template<typename T>
	T* FindBehaviour();

	inline bool IsReady() const { return m_State == GameConnectionState::Connected; }
	inline bool HasDisconnected() const { return m_State == GameConnectionState::Disconnected; }

	inline bool IsMemoryReadable() const { return IsReady() && GetObservedGameMemory().AreHeadersValid(); }

	void WriteRequest(GameMessageID messageId, size_t addr, void const* data, size_t size);
	void ReadRequest(GameMessageID messageId, size_t addr, size_t size);
	void ManualFlush() { FlushCommands(); }

	ObservedGameMemory& GetObservedGameMemory();
	ObservedGameMemory const& GetObservedGameMemory() const;

	//template<typename T>
	//inline void WriteValue(size_t addr, T& value);
	//
	//template<typename T>
	//inline void ReadValue(size_t addr);

private:
	void AddDefaultBehaviours();
	bool RemoveBehaviourInternal(IGameConnectionBehaviour* behaviour);

	void SendCommand(std::string const& command);
	void FlushCommands();

	void OnRecieveData(u8* data, size_t size);
	void OnRecieveMessage(GameMessageID messageId, u8 const* data, size_t size);

	bool HandleExpectedHandshake(std::string const& expectedHandshake, u8* data, size_t size);

	static std::string const c_FirstHandshake;
	static std::string const c_SecondHandshake;

	sf::TcpSocket m_Socket;
	GameConnectionState m_State;
	UpdateTimer m_UpdateTimer;
	int m_UpdateFrame;

	u8 m_RecieveBuffer[4096];
	u8 m_SendBuffer[4096];
	size_t m_SendSize;

	RPCQueue m_GameRPCs;
	std::unique_ptr<ObservedGameMemory> m_ObservedGameMemory;

	std::vector<GameConnectionBehaviourRef> m_Behaviours;
	std::vector<GameConnectionBehaviourRef> m_BehavioursToRemove;
};

// Templates
//
template<typename T>
std::shared_ptr<T> GameConnection::AddBehaviour()
{
	std::shared_ptr<T> ptr = std::make_shared<T>();
	AddBehaviour(ptr.get());
	return ptr;
}

template<typename T>
T* GameConnection::FindBehaviour()
{
	for (auto& ref : m_Behaviours)
	{
		T* result = dynamic_cast<T*>(ref.get());
		if (result != nullptr)
			return result;
	}

	return nullptr;
}
