#include "pch.h"
#include "Timer.h"
#pragma warning(disable:6387)
// Timer Initialize 
Timer::Timer(){
	
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_performanceFrequency)) {
		m_hardwarePerformanceConter = true;
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_lastTime);

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
	//업데이트시, 하드웨어가 고성능 카운터를 지원한다면 사용하고, 
	//지원하지 않는다면 멀티미디어 카운터를 사용한다.
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
	// 이번 횟수 처리 시간이 이전 처리 시간과 간격이 1초이내라면 
	// 처리 시간 샘플러에 추가한다
	if (fabsf(TimeElapsed - m_timeElapsed) < 1.f) {
		// postpone the saved value one by one
		::memmove(&m_frameTime[1], m_frameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_frameTime[0] = TimeElapsed;

		if (m_sampleCount < MAX_SAMPLE_COUNT) {
			m_sampleCount++;
		}
	}
	// 일반적인 FPS 세기 
	m_framePerSecond++;
	m_framePerSecondTimeElapsed += TimeElapsed;
	if (m_framePerSecondTimeElapsed > 1.f) {
		m_currentFrameRate = m_framePerSecond;
		m_framePerSecond = 0;
		m_framePerSecondTimeElapsed = 0.f;
	}
	// 처리 시간 평균 계산 
	m_timeElapsed = favgf(m_frameTime, m_sampleCount);
}

ULONG Timer::GetFrameRate(LPTSTR lpszString, int CharacterNumbers){
	// 10자리까지 FrameRate 를 lpszString 에 쓰고, " FPS)" 와 결합한다 
	if (lpszString) {
		_itow_s(m_currentFrameRate, lpszString, CharacterNumbers, 10);
		wcscat_s(lpszString, CharacterNumbers, _T(" FPS )"));
	}
	return m_currentFrameRate;
}


// GetDeltaTime 이 아닌이유 ->
// 실제로 이것은 DeltaTime 이 아니라 프레임 처리 시간을  평균 낸 시간
// 일정한 Time Sample 을 얻기위해? 
float Timer::GetTimeElapsed(){
	return m_timeElapsed;
}
