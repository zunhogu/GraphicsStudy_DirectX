#include "timerclass.h" 

TimerClass::TimerClass() { } 

TimerClass::TimerClass(const TimerClass& other) { }

TimerClass::~TimerClass() { }

// Initialize 함순는 시스템이 고해상도 타이머를 지원하는지 알아봐야한다.
// 만약 타이머의 주기를 받으면 이 값을 가지고 밀리초마다 카운터에서 몇번이나 틱이 일어나는지를 계산한다.
// 타이머 주기를 가지고 프레임간 시간을 계산할 수 있다. Initialize 함수의 마지막에서는 첫 프레임 간격을 물어봐서 타이머를 시작한다.
bool TimerClass::Initialize() {
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0) {
		return false;
	}

	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

// 프레임 함수는 메인 프로그램에서 매 루프마다 호출하도록한다. 이렇게 매루프마다 시간의 차이를 계산해서 프레임 간의 시간 간격을 알 수 있다.
// 질의하고 계산한 뒤 그 값을 m_frame에 저장하여 이를 호출하는 다른 오브젝트들이 동기화될 수 있게 한다. 
// 이를 통해 서로 다른 오브젝트들의 그려지는 시간이 동기화 될 수 있게한다.
// 그리고 나서 현재 시간을 다음 프레임의 시작시간으로 저장한다. 
void TimerClass::Frame(){
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

// GetTime 함수는 가장 최근에 계산된 프레임 시간을 리턴한다.
float TimerClass::GetTime(){
	return m_frameTime;
}