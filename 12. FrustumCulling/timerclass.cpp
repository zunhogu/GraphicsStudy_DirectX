#include "timerclass.h" 

TimerClass::TimerClass() { } 

TimerClass::TimerClass(const TimerClass& other) { }

TimerClass::~TimerClass() { }

// Initialize �Լ��� �ý����� ���ػ� Ÿ�̸Ӹ� �����ϴ��� �˾ƺ����Ѵ�.
// ���� Ÿ�̸��� �ֱ⸦ ������ �� ���� ������ �и��ʸ��� ī���Ϳ��� ����̳� ƽ�� �Ͼ������ ����Ѵ�.
// Ÿ�̸� �ֱ⸦ ������ �����Ӱ� �ð��� ����� �� �ִ�. Initialize �Լ��� ������������ ù ������ ������ ������� Ÿ�̸Ӹ� �����Ѵ�.
bool TimerClass::Initialize() {
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0) {
		return false;
	}

	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

// ������ �Լ��� ���� ���α׷����� �� �������� ȣ���ϵ����Ѵ�. �̷��� �ŷ������� �ð��� ���̸� ����ؼ� ������ ���� �ð� ������ �� �� �ִ�.
// �����ϰ� ����� �� �� ���� m_frame�� �����Ͽ� �̸� ȣ���ϴ� �ٸ� ������Ʈ���� ����ȭ�� �� �ְ� �Ѵ�. 
// �̸� ���� ���� �ٸ� ������Ʈ���� �׷����� �ð��� ����ȭ �� �� �ְ��Ѵ�.
// �׸��� ���� ���� �ð��� ���� �������� ���۽ð����� �����Ѵ�. 
void TimerClass::Frame(){
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

// GetTime �Լ��� ���� �ֱٿ� ���� ������ �ð��� �����Ѵ�.
float TimerClass::GetTime(){
	return m_frameTime;
}