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

	inline u8* GetData() { ASSERT_MSG(m_IsValid, "Observed memory is invalid"); return m_Data.data(); }
	inline u8 const* GetData() const { ASSERT_MSG(m_IsValid, "Observed memory is invalid"); return m_Data.data(); }
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
		m_DebugPtr = static_cast<T*>(static_cast<void*>(m_Data.data()));
#endif
	}

	inline T& Get() { return *static_cast<T*>(static_cast<void*>(GetData())); }
	inline T const& Get() const { return *static_cast<T const*>(static_cast<void const*>(GetData())); }

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

	bool AreHeadersValid() const;
	bool IsMultiplayerStateValid() const;
	bool IsHomeBoxStateValid() const;

	GameStructures::GFRomHeader const& GetGFRomHeader() const { return m_GFRomHeader.Get(); }
	GameStructures::RogueAssistantHeader const& GetRogueHeader() const { return m_RogueHeader.Get(); }
	GameStructures::RogueAssistantState const& GetAssistantState() const { return m_AssistantState.Get(); }
	GameAddress GetMultiplayerStatePtr() const { return m_MultiplayerStatePtr.Get(); }
	u8 const* GetMultiplayerStateBlob() const { return m_MultiplayerState.GetData(); }
	GameAddress GetHomeBoxStatePtr() const { return m_HomeBoxStatePtr.Get(); }
	u8 const* GetHomeBoxStateBlob() const { return m_HomeBoxState.GetData(); }

	GameAddress GetPokemonStoragePtr() const;
	bool RequestPokemonStorageData(u32 boxId);
	bool IsPokemonStorageBlobReady() const { return m_PokemonStorageData.IsValid(); }
	u8 const* GetPokemonStorageBlob() const { return m_PokemonStorageData.GetData(); }

private:
	GameConnection& m_Game;

	ObservedStruct<GameStructures::GFRomHeader> m_GFRomHeader;
	ObservedStruct<GameStructures::RogueAssistantHeader> m_RogueHeader;
	ObservedStruct<GameStructures::RogueAssistantState> m_AssistantState;
	ObservedStruct<GameAddress> m_MultiplayerStatePtr;
	ObservedBlob m_MultiplayerState;
	ObservedStruct<GameAddress> m_HomeBoxStatePtr;
	ObservedBlob m_HomeBoxState;
	ObservedBlob m_PokemonStorageData;
};