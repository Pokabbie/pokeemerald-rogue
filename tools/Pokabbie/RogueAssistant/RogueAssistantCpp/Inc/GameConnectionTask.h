#pragma once
#include "Defines.h"

#include <functional>
#include <memory>
#include <vector>

class GameConnection;
class GameConnectionTask;
class ObservedGameMemoryEntry;

typedef std::function<void(u8 const* data, size_t size)> DataCallback;
typedef std::shared_ptr<GameConnectionTask> GameConnectionTaskRef;
typedef std::shared_ptr<ObservedGameMemoryEntry> ObservedGameMemoryRef;

enum class GameConnectionTaskState
{
	Initialising,
	Processing,
	Succeeded,
	Failed
};

class GameConnectionTask : public std::enable_shared_from_this<GameConnectionTask>
{
	friend GameConnection;
public:
	static const u32 c_InvalidId = std::numeric_limits<u32>::max();

	GameConnectionTask();

	inline u32 GetInternalId() const { return m_InternalId; }
	inline bool HasCompleted() const { return m_State == GameConnectionTaskState::Succeeded || m_State == GameConnectionTaskState::Failed; }

	void Then(DataCallback callback);
	GameConnectionTaskRef Then(GameConnectionTaskRef other);

private:
	void OnTaskUpdate();
	void OnTaskCompleted(u8 const* data, size_t size);

	u32 m_InternalId;
	GameConnectionTaskState m_State;
	std::vector<DataCallback> m_Listeners;
};

