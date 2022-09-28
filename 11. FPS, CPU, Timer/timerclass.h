#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_

#include <windows.h>

// TimerClas는 실행 중 매 프레임 간의 정확한 시간을 재는 고해상도 타이머 클래스다.
// 이것의 주용도는 오브젝트의 이동을 위해 프레임의 표준 시간대와 동기화한다.
// 현재는 직접 쓸일이 없지만 어떻게 적용할 수 있는지 보여준느 코드를 작성한다.
// 대개 TimerClass는 프레임간 간격을 1초에 대한 %로 바꿔서 물체를 그만큼 움직이도록 하는 경우에 많이 사용한다.
class TimerClass{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

#endif