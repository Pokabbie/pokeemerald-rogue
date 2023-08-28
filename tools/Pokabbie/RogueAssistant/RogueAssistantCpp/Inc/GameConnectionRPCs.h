#pragma once
#include "Defines.h"

#include <queue>

class GameConnection;

class RPCQueue
{
public:
	typedef void Callback_GetSpeciesName(char const* string);

	RPCQueue(GameConnection& game);

	void Update();

	//void GetSpeciesName(u16 species, Callback_GetSpeciesName callback);

private:
	//typedef void WriteDataCallback(u8 const* data);

	void PushSendBufferData(void const* data, size_t size);
	void ClearSendBuffer();

	void RPC_GetSpeciesName(u16 species);

	template<typename T>
	void PushSendData(T const& value) { PushSendBufferData(&value, sizeof(T)); }

	GameConnection& m_Game;
	u16 m_Counter;

	size_t m_SendBufferSize;
	u8 m_SendBuffer[16];
	//std::queue<WriteDataCallback> m_RPCQueue;
};