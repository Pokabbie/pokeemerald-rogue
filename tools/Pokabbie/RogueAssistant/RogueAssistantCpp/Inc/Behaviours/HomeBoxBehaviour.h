#pragma once
#include "Defines.h"
#include "GameConnectionBehaviour.h"

#include <fstream>
#include <vector>
#include <queue>

class DataStream;

class HomeBoxBehaviour : public IGameConnectionBehaviour
{
public:
	virtual void OnAttach(GameConnection& game) override;
	virtual void OnDetach(GameConnection& game) override;
	virtual void OnUpdate(GameConnection& game) override;

	inline bool IsLoading() const { return m_State < State::Update; }
	inline bool IsSaving() const { return m_HasPendingFileWrite; }

private:
	enum class State
	{
		OpenOfflineFile,
		InitialiseBoxData,
		WaitingForBoxData,
		SendGameDataInit,
		WaitForInit,
		Update,

		First = OpenOfflineFile
	};

	struct BoxData
	{
		std::vector<u8> m_MinimalData;
		std::vector<u8> m_MonData;

		u32 CalculateCheckSum() const;
	};

	struct BoxWriteRequest
	{
		u32 m_BoxId;
		u8 const* m_Data;
		size_t m_Offset;
		size_t m_BytesRemaining;
	};

	State m_State;
	std::vector<u8> m_LocalActiveBoxIndices;
	std::vector<u8> m_RemoteActiveBoxIndices;
	std::vector<BoxData> m_ActiveBoxData;
	std::vector<BoxData> m_StoredBoxData;
	std::queue<BoxWriteRequest> m_BoxWriteRequest;

	std::wstring m_WriteFilePath;
	bool m_HasPendingFileWrite = false;

	void InitaliseLocalBoxData(GameConnection& game, u32 boxId);
	void HandlePendingFileWrite(GameConnection& game);
	bool LoadDataFromFile(GameConnection& game, std::fstream& fileStream);
	bool SerializeSavedData(GameConnection& game, DataStream& stream);

	void WriteMinimalBox(GameConnection& game, u32 boxId, u8 const* data);
	u8 const* GetMinimalBoxPtr(GameConnection& game, u32 boxId);

	void BeginWriteMonBox(GameConnection& game, u32 boxId, u8 const* data);
	bool PumpWriteMonBox(GameConnection& game);
	u8 const* GetMonBoxPtr(GameConnection& game, u32 boxId);
};