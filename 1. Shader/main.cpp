#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass* System;
	bool result;

	// �ý��� ��ü�� �����Ѵ�.
	System = new SystemClass;
	if (!System) {
		return 0;
	}

	// ������ �ý��� ��ü�� �ʱ�ȭ�Ѵ�.
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	// �ý��� ��ü�� �޸𸮿��� �����Ѵ�.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}