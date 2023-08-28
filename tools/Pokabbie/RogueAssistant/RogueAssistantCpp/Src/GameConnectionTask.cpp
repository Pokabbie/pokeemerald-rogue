#include "GameConnectionTask.h"
#include "GameConnection.h"
#include "Log.h"

GameConnectionTask::GameConnectionTask()
	: m_InternalId(c_InvalidId)
	, m_State(GameConnectionTaskState::Initialising)
{
}

void GameConnectionTask::Then(DataCallback callback)
{
	m_Listeners.push_back(callback);
}

GameConnectionTaskRef GameConnectionTask::Then(GameConnectionTaskRef other)
{
	ASSERT_FAIL("todo - not implemented");
	return other;
}

void GameConnectionTask::OnTaskUpdate()
{
	ASSERT_FAIL("todo - not implemented");
}

void GameConnectionTask::OnTaskCompleted(u8 const* data, size_t size)
{
	for (auto it : m_Listeners)
		it(data, size);

	m_State = GameConnectionTaskState::Succeeded;
}
