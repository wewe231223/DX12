#pragma once

constexpr ULONG MAX_SAMPLE_COUNT = 50;


class Timer {
private:
	bool m_hardwarePerformanceConter{ false };
	float m_timeScale{ 0.f };
	float m_timeElapsed{ 0.f };
	__int64 m_currentTime{ 0 };
	__int64 m_lastTime{ 0 };
	__int64 m_performanceFrequency{ 0 };
	float m_frameTime[MAX_SAMPLE_COUNT]{};

	ULONG m_sampleCount{ 0 };
	ULONG m_currentFrameRate{ 0 };
	ULONG m_framePerSecond{ 0 };
	float m_framePerSecondTimeElapsed{ 0.f };

public:

	Timer();
	virtual ~Timer();

	void Tick(float LockFPS = 0.f);
	ULONG GetFrameRate(LPTSTR lpszString = NULL, int CharacterNumbers = 0);
	float GetTimeElapsed();

};