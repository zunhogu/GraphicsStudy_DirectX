#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

// Direct input을 사용하려면 헤더에 그 버전을 define으로 선언한다.
// 그렇지 않으면 컴파일러는 기본값인 버전 8로 간다는 메세지를 만들어낸다.
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib") 
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputClass {
private:
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
private:
	// 키보드 장치, 마우스 장치에 대한 인터페이스
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	// 현재 키보드와 마우스의 상태를 기록하는데 사용되는 변수
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif