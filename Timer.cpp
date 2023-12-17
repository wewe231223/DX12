#include "pch.h"
#include "Timer.h"

Timer::Timer(){

	if (::QueryPerformanceFrequency((LARGE_INTEGER*)m_performanceFrequency)) {
		m_hardwarePerformanceConter = true;
		::QueryPerformanceCounter((LARGE_INTEGER*)m_lastTime);

		m_timeScale = 1.f / m_performanceFrequency;
	}
	else {
		m_hardwarePerformanceConter = false;
		m_lastTime = ::GetTickCount64();
		m_timeScale = 0.001f;
	}
}

Timer::~Timer(){
}

void Timer::Tick(float LockFPS){
	if (m_hardwarePerformanceConter) {
		QueryPerformanceCounter((LARGE_INTEGER*)&m_currentTime);
	}
	else {
		m_currentTime = ::GetTickCount64();
	}

	float TimeElapsed = (m_currentTime - m_lastTime) * m_timeScale;



	// Parameter time Lock(wait) 
	if (LockFPS > 0.f) {
		while (TimeElapsed < (1.f / LockFPS)) {
			if(m_hardwarePerformanceConter){
				::QueryPerformanceCounter((LARGE_INTEGER*)&m_currentTime);
			}
			else {
				m_currentTime = ::GetTickCount64();
			}

			TimeElapsed = (m_currentTime - m_lastTime) * m_timeScale;
		}
	}

	m_lastTime = m_currentTime;

	if (fabsf(TimeElapsed - m_timeElapsed) < 1.f) {
		// postpone the saved value one by one
		::memmove(&m_frameTime[1], m_frameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_frameTime[0] = TimeElapsed;

		if (m_sampleCount < MAX_SAMPLE_COUNT) {
			m_sampleCount++;
		}
	}
	
	m_framePerSecond++;
	m_framePerSecondTimeElapsed += TimeElapsed;

	if (m_framePerSecondTimeElapsed > 1.f) {
		m_currentFrameRate = m_framePerSecond;
		m_framePerSecond = 0;
		m_framePerSecondTimeElapsed = 0.f;
	}


	m_timeElapsed = favg(m_frameTime, m_sampleCount);
}

ULONG Timer::GetFrameRate(LPTSTR lpszString, int CharacterNumbers){

	if (lpszString) {
		_itow_s(m_currentFrameRate, lpszString, CharacterNumbers, 10);
		wcscat_s(lpszString, CharacterNumbers, _T(" FPS)"));
	}



	return m_currentFrameRate;
}

float Timer::GetTimeElapsed(){
	return m_timeElapsed;
}
