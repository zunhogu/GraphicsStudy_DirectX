#ifndef _FPSCLASS_H_ 
#define _FPSCLASS_H_

#pragma comment(lib, "winmm.lib")

// FpsClass�� �ܼ��� Ÿ�̸Ӱ� �ִ� ī���ʹ�. 1���� �ð� ���� ���� �����ӵ��� �׷������� ���� ���������� �� ���ڸ� �����Ѵ�.

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