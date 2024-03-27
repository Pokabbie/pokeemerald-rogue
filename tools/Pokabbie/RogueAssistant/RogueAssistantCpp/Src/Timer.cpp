#include "Timer.h"

#define NANOSECONDS_PER_SECOND (1000000000)

TimeDurationNS const UpdateTimer::c_1UPS = NANOSECONDS_PER_SECOND;
TimeDurationNS const UpdateTimer::c_5UPS = NANOSECONDS_PER_SECOND / 5;
TimeDurationNS const UpdateTimer::c_10UPS = NANOSECONDS_PER_SECOND / 10;
TimeDurationNS const UpdateTimer::c_20UPS = NANOSECONDS_PER_SECOND / 20;
TimeDurationNS const UpdateTimer::c_25UPS = NANOSECONDS_PER_SECOND / 25;
TimeDurationNS const UpdateTimer::c_30UPS = NANOSECONDS_PER_SECOND / 30;
TimeDurationNS const UpdateTimer::c_60UPS = NANOSECONDS_PER_SECOND / 60;

TimeDurationNS UpdateTimer::GetCurrentClock()
{
	auto timePoint = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch());
	return duration.count();
}

UpdateTimer::UpdateTimer(TimeDurationNS const& interval)
	: m_UpdateInterval(interval)
	, m_LastUpdate()
	, m_Timer(0)
	, m_WasPaused(true)
{
}

bool UpdateTimer::Update()
{
	TimeDurationNS currentTime = GetCurrentClock();
	auto deltaTime = currentTime - m_LastUpdate;

	m_LastUpdate = currentTime;
	m_Timer += deltaTime;

	if(m_WasPaused || m_Timer >= m_UpdateInterval)
	{
		if (m_WasPaused)
		{
			m_Timer = 0;
			m_WasPaused = false;
		}
		else
		{
			m_Timer -= m_UpdateInterval;

			// Gotten too far ahead
			if (m_Timer >= m_UpdateInterval * 3)
				m_Timer = m_UpdateInterval;

			if (m_Timer < 0)
				m_Timer = 0;
		}
		return true;
	}

	return false;
}