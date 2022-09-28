#ifndef _CPUCLASS_H_ 
#define _CPUCLASS_H_

// CPU 사용량을 알아오기위해서 pdh 라이브러리를 사용한다.
#pragma comment(lib, "pdh.lib")
#include <pdh.h>

class CpuClass {
private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
public:
	CpuClass();
	CpuClass(const CpuClass&);
	~CpuClass();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();
};

#endif
