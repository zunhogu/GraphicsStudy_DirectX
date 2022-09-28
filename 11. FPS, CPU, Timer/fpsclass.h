#ifndef _FPSCLASS_H_ 
#define _FPSCLASS_H_

#pragma comment(lib, "winmm.lib")

// FpsClass는 단순히 타이머가 있는 카운터다. 1초의 시간 동안 얼마의 프레임들이 그려지는지 세고 정기적으로 이 숫자를 갱신한다.

#include <windows.h>
#include <mmsyscom.h>

class FpsClass {
private:
	int m_fps, m_count;
	unsigned long m_startTime;
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFps();
};

#endif