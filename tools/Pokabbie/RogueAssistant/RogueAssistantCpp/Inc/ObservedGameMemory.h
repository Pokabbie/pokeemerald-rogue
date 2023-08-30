#pragma once
#include "Defines.h"
#include "GameConnectionMessage.h"
#include "GameData.h"
#include "Log.h"

#include <vector>

class GameConnection;

// Observed game binary blob
//

class ObservedBlob
{
public:
	ObservedBlob(size_t size = 0);

	inline bool IsValid() const { return m_IsValid; }

	inline void* GetData() { return m_Data.data(); }
	inline void const* GetData() const { return m_Data.data(); }
	inline size_t GetSize() const { return m_Data.size(); }

	void Resize(size_t size);

	bool SetData(u8 const* data, size_t size);
	void Clear();

protected:
	bool m_IsValid;
	std::vector<u8> m_Data;
};

// Observed game structure
//

template<typename T>
class ObservedStruct : public ObservedBlob
{
public:
	ObservedStruct()
		: ObservedBlob(sizeof(T))
	{
#if _DEBUG
		m_DebugPtr = static_cast<T*>(GetData());
#endif
	}

	inline T& Get() { return *static_cast<T*>(GetData()); }
	inline T const& Get() const { return *static_cast<T const*>(GetData()); }

	inline T* operator->() { return &Get(); }
	inline T const* operator->() const { return &Get(); }
private:
#if _DEBUG
	T* m_DebugPtr;
#endif
};

// Collection of common observed memory
//

class ObservedGameMemory
{
public:
	ObservedGameMemory(GameConnection& game);

	void Update();
	void OnRecieveMessage(GameMessageID messageId, u8 const* data, size_t size);

	bool IsMuliplayerStateValid() const;

private:
	GameConnection& m_Game;

	ObservedStruct<GameStructures::GFRomHeader> m_GFRomHeader;
	ObservedStruct<GameStructures::RogueAssistantHeader> m_RogueHeader;
	ObservedStruct<GameStructures::RogueAssistantState> m_AssistantState;
	ObservedStruct<GameAddress> m_MultiplayerStatePtr;
	ObservedBlob m_MultiplayerState;
};