#pragma once
#include "Defines.h"
#include "GameConnectionTask.h"
#include "GameData.h"

class GameConnection;

enum class ObservedGameMemoryType
{
	Immediate,
	Virtual // 1 indirect lookup first
};


class ObservedGameMemoryEntry
{
public:
	ObservedGameMemoryEntry(ObservedGameMemoryType type, GameAddress address, size_t size);

	void Update(GameConnection& game);

	inline u8 const* GetData() const { return m_Data.data(); }
	inline size_t GetSize() const { return m_Data.size(); }

	inline bool IsNull() const { return m_IsNull; }

	template<typename T>
	inline T const& Get() const { return reinterpret_cast<T const&>(*GetData()); }

private:
	void OnRecvData(u8 const* data, size_t size);

	ObservedGameMemoryType m_MemoryType;
	GameAddress m_Address;
	bool m_IsNull;

	GameConnectionTaskRef m_ActiveTask;
	std::vector<u8> m_Data;
};


class ObservedGameMemory
{
public:
	ObservedGameMemory(GameConnection& game);

	void Update();

	inline ObservedGameMemoryEntry const& GetAssistantState() const { return m_AssistantState; }
	inline ObservedGameMemoryEntry const& GetRogueNetMultiplayer() const { return m_RogueNetMultiplayer; }

private:
	GameConnection& m_Game;
	ObservedGameMemoryEntry m_AssistantState;
	ObservedGameMemoryEntry m_RogueNetMultiplayer;
};