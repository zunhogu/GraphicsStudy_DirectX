#include "cpuclass.h"
CpuClass::CpuClass() { } 

CpuClass::CpuClass(const CpuClass& other) { } 

CpuClass::~CpuClass() { }

// Initialize �Լ������� cpu ��뷮�� �����ϱ� ���� �ʿ��� �ڵ��� �ʱ�ȭ�Ѵ�.
// ���⼭ ������ ���Ǵ� ���� cpu�� ��뷮 ��� �ý����� ��� cpu���� ��뷮 ������ ���Ͽ� �����ش�.
// ���� �ڵ��̳� ���Ǹ� ���� ���ϴ� ���� ������ cpu ��뷮 ������ ���� �� ���ٸ� m_canReadCpu �� false�� �ٲ� cpu ��뷮�� 0�� �ǵ����Ѵ�.
// �Ϻ� os������ ���Ѷ����� �� ����� ����� �� ���� �� �� �ִ�. ���� Ÿ�̸Ӹ� ������ cpu ��뷮�� �ʸ��� ��������

void CpuClass::Initialize() {
	PDH_STATUS status;

	m_canReadCpu = true;

	// CPU ���Ǹ� ���� �ڵ��� �ʱ�ȭ�Ѵ�.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS) {
		m_canReadCpu = false;
	}

	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle); 
	if (status != ERROR_SUCCESS) {
		m_canReadCpu = false;
	}

	m_lastSampleTime = GetTickCount();

	m_cpuUsage = 0;

	return;
}

// Shutdown �Լ��� cpu��뷮�� �����ϴ� �� ����� �ڵ��� �����Ѵ�.
void CpuClass::Shutdown(){
	if (m_canReadCpu){
		PdhCloseQuery(m_queryHandle);
	}

	return;
}

// ������ �Լ��� �� �����Ӹ��� ȣ��ȴ�. ������ ���� Ƚ���� ���̱� ���� m_lastSampleTime�̶�� ������ �ξ� 1�ʸ��� CPU ��뷮�� ���ø��ϰ� �Ѵ�.
// �׷��� �� �ʸ��� cpu���� ��뷮�� ����� �� ���� m_cpuUsage ������ �����Ѵ�. 
void CpuClass::Frame(){
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu){
		if ((m_lastSampleTime + 1000) < GetTickCount()){
			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}

	return;
}


// GetCpuPercentage �Լ��� ���� cpu ��뷮�� �����ش�. � �����ε� cpu ��뷮�� ���� �� ���� �ȴٸ� �� ���� 0 �����Ѵ�.
int CpuClass::GetCpuPercentage(){
	int usage;
	if (m_canReadCpu) { usage = (int)m_cpuUsage; }
	else { usage = 0; }
	return usage;
}


