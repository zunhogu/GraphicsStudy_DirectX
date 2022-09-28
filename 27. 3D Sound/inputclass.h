#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

// Direct input�� ����Ϸ��� ����� �� ������ define���� �����Ѵ�.
// �׷��� ������ �����Ϸ��� �⺻���� ���� 8�� ���ٴ� �޼����� ������.
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
	// Ű���� ��ġ, ���콺 ��ġ�� ���� �������̽�
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	// ���� Ű����� ���콺�� ���¸� ����ϴµ� ���Ǵ� ����
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif