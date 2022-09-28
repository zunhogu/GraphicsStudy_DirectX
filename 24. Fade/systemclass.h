#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//�Ʒ��� �ڵ�� ���� ������ �ʴ� API���� ����ִ� Win32����� �������� �������� ����ӵ��� ���̱� �����̴�.
#define WIN32_LEAN_AND_MEAN

//Windows.h�� �����ؾ��� ������Ŭ������ ����/���� �ϴ� �Լ����� ȣ���� �� �ִ�.
//�� �ٸ� ������ win32�Լ����� ����� �� �ִ�.
#include <windows.h>

//SystemClass���� ����ϱ� ���� �����ӿ�ũ�� �ٸ� �� Ŭ������ ����� �����Ѵ�.
#include "inputclass.h"
#include "graphicsclass.h"
#include "timerclass.h" 

class SystemClass {
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	//WinMain�Լ����� ����� �Լ� 
	bool Initialize();
	void Shutdown();
	void Run();

	//m_Input���� �ý��� �޼����� �����ϰų� �޼��� ó����� �޼����� �����ϴ� �Լ� 
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	//Run()���� ����
	bool Frame();
	//Initiallize()���� ����
	void InitializeWindows(int&, int&);
	//Shutdown()���� ����
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	TimerClass* m_Timer;

};

// wndProc �Լ��� ApplicationHandle �����ʹ� MessageHandler()���� ����Ѵ�.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;

#endif