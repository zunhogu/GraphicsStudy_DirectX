#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_

#include <windows.h>

// TimerClas�� ���� �� �� ������ ���� ��Ȯ�� �ð��� ��� ���ػ� Ÿ�̸� Ŭ������.
// �̰��� �ֿ뵵�� ������Ʈ�� �̵��� ���� �������� ǥ�� �ð���� ����ȭ�Ѵ�.
// ����� ���� ������ ������ ��� ������ �� �ִ��� �����ش� �ڵ带 �ۼ��Ѵ�.
// �밳 TimerClass�� �����Ӱ� ������ 1�ʿ� ���� %�� �ٲ㼭 ��ü�� �׸�ŭ �����̵��� �ϴ� ��쿡 ���� ����Ѵ�.
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