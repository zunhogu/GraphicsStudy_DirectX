#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//아래의 코드는 자주 사용되지 않는 API들을 담고있는 Win32헤더를 포함하지 않음으로 빌드속도를 높이기 위함이다.
#define WIN32_LEAN_AND_MEAN

//Windows.h를 선언해야지 윈도우클래스를 생성/제거 하는 함수들을 호출할 수 있다.
//또 다른 유용한 win32함수들을 사용할 수 있다.
#include <windows.h>

//SystemClass에서 사용하기 위한 프레임워크의 다른 두 클래스의 헤더를 포함한다.
#include "inputclass.h"
#include "graphicsclass.h"
#include "timerclass.h" 

class SystemClass {
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	//WinMain함수에서 실행될 함수 
	bool Initialize();
	void Shutdown();
	void Run();

	//m_Input에게 시스템 메세지를 전달하거나 메세지 처리기로 메세지를 전달하는 함수 
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	//Run()에서 실행
	bool Frame();
	//Initiallize()에서 실행
	void InitializeWindows(int&, int&);
	//Shutdown()에서 실행
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	TimerClass* m_Timer;

};

// wndProc 함수와 ApplicationHandle 포인터는 MessageHandler()에서 사용한다.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;

#endif