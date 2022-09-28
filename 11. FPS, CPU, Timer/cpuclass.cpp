#include "cpuclass.h"
CpuClass::CpuClass() { } 

CpuClass::CpuClass(const CpuClass& other) { } 

CpuClass::~CpuClass() { }

// Initialize 함수에서는 cpu 사용량을 질의하기 위해 필요한 핸들을 초기화한다.
// 여기서 설정된 질의는 개개 cpu의 사용량 대신 시스템의 모든 cpu들의 사용량 총합을 합하여 돌려준다.
// 만약 핸들이나 질의를 얻지 못하는 등의 이유로 cpu 사용량 정보를 읽을 수 없다면 m_canReadCpu 가 false로 바뀌어서 cpu 사용량을 0이 되도록한다.
// 일부 os에서는 권한때문에 이 기능을 사용할 수 없을 수 도 있다. 또한 타이머를 돌려서 cpu 사용량을 초마다 측정하자

void CpuClass::Initialize() {
	PDH_STATUS status;

	m_canReadCpu = true;

	// CPU 질의를 이한 핸들을 초기화한다.
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

// Shutdown 함수는 cpu사용량을 질의하는 데 사용한 핸들을 해제한다.
void CpuClass::Shutdown(){
	if (m_canReadCpu){
		PdhCloseQuery(m_queryHandle);
	}

	return;
}

// 프레임 함수는 매 프레임마다 호출된다. 하지만 질의 횟술르 줄이기 위해 m_lastSampleTime이라는 변수를 두어 1초마다 CPU 사용량을 샘플링하게 한다.
// 그렇게 매 초마다 cpu에게 사용량을 물어보고 그 값을 m_cpuUsage 변수에 저장한다. 
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


// GetCpuPercentage 함수는 현재 cpu 사용량을 돌려준다. 어떤 이유로든 cpu 사용량을 읽을 수 없게 된다면 그 값을 0 으로한다.
int CpuClass::GetCpuPercentage(){
	int usage;
	if (m_canReadCpu) { usage = (int)m_cpuUsage; }
	else { usage = 0; }
	return usage;
}


