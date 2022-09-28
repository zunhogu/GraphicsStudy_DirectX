#include "fpsclass.h"

FpsClass::FpsClass() {}

FpsClass::FpsClass(const FpsClass& other){}

FpsClass::~FpsClass(){}

// 초기화함수
void FpsClass::Initialize() {
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();  // 시작 시간을 받아온다.
	return;
}

// 프레임 함수, 매 프레임마다 호출되어 1씩 증가하도록 한다. 만약 1초 이상 지났다면 현재 프레임 수를 m_fps 변수에 저장한다.
// 그리고 프레임 수를 초기화하고 다시 타이머를 돌린다.
void FpsClass::Frame() {
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000)) {
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

// 기록한 Fps 값을 반환한다.
int FpsClass::GetFps() {
	return m_fps;
}

