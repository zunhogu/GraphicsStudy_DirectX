#include "fpsclass.h"

FpsClass::FpsClass() {}

FpsClass::FpsClass(const FpsClass& other){}

FpsClass::~FpsClass(){}

// �ʱ�ȭ�Լ�
void FpsClass::Initialize() {
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();  // ���� �ð��� �޾ƿ´�.
	return;
}

// ������ �Լ�, �� �����Ӹ��� ȣ��Ǿ� 1�� �����ϵ��� �Ѵ�. ���� 1�� �̻� �����ٸ� ���� ������ ���� m_fps ������ �����Ѵ�.
// �׸��� ������ ���� �ʱ�ȭ�ϰ� �ٽ� Ÿ�̸Ӹ� ������.
void FpsClass::Frame() {
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000)) {
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

// ����� Fps ���� ��ȯ�Ѵ�.
int FpsClass::GetFps() {
	return m_fps;
}

