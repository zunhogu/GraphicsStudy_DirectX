#include "inputclass.h"

//SystemClass.cpp�� �Ȱ��� ������ ��������� null������, ��������ڿ� �Ҹ��ڴ� �ƹ��͵� ���ϰ� �����Ѵ�.
InputClass::InputClass() {
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other) {}

InputClass::~InputClass() {}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	HRESULT result;

	// ȭ���� �ʺ�, ���̸� �����Ѵ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ���콺 ��ġ�� �ʱ�ȭ�Ѵ�.
	m_mouseX = 0;
	m_mouseY = 0;

	////////////////////////////////////////////////////////////////////////////////
	// Direct input�� �������̽��� �ʱ�ȭ�Ѵ�.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// Ű���� �Է���ġ�� �ʱ�ȭ�Ѵ�.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	// �Է��� ������ ������ �����Ѵ�.
	// �̴� Ű���忡�� �츮�� �Է��� �����͸� �̸� ������ �� �ִ�.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	// Ű���� ���·���(cooperative level)�� ���ϴ� ���� �� ��ġ�� ������ �ϴ���, �׸��� ��� ���� �������� �����ϹǷ� �߿��ϴ�.
	// DISCL_EXCLUSIVE �÷��׷� �ٸ� ���α׷���� �������� �ʴ´ٰ� �˷��ش�. (���� ����)
	// �̷��� �ϸ� ���� �� ���ø����̼Ǹ� �� �Է��� ������ �� �ְ� ���ش�.
	// ������ ���� �� ���ø����̼��� �ƴ� �ٸ� ���ø����̼ǿ��� Ű���� �Է¿� ���ٰ����ϰ� �Ϸ��� DISCL_NONEXCLUSIVE�� ����ϸ� �ȴ�. (����� ����)
	// �߿��� ���� ����� �����̰� Ǯ��ũ���� �ƴ϶�� ��ġ�� �ٸ� ������� ��Ŀ���� �̵��ߴ���, �׸��� �ٽ� ��Ŀ���� �� ��ġ�� ����� �� �ְ� �Ǿ����� Ȯ���Ѵٴ� ���̴�.
	// �̷� ��Ŀ���� �Ҿ���� ��츦 ���� ��� �밳 �ٸ� �����찡 ���� �����찡 �Ǿ� ��Ŀ���� ����ٴ��� �ƴϸ� �츮 ���ø����̼��� �ּ�ȭ�� ��츦 ���Ѵ�.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// Ű���� ������ �Ǹ� Acquire �Լ��� ȣ���Ͽ� �� �����ͷ� Ű���忡 ���� ������ ����Ѵ�.
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// ���콺 �Է���ġ�� �ʱ�ȭ�Ѵ�.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	// ���콺 �Է���ġ�� ������ �����Ѵ�.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	// ���쿡 ���ؼ��� ����� ���·� �����Ѵ�. ���콺�� ��Ŀ���� �Ҿ����� �ٽ� ã�Ҵ��� Ȯ���ؾ��Ѵ�.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// ���콺 ������ �Ǹ� Acquire �Լ��� ȣ���Ͽ� �� �����ͷ� ���콺 ���� ������ ����Ѵ�.
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}
	return  true;
}

// Shutdown �Լ��� �� ��ġ�� direct input�� ���� �������̽��� �����Ѵ�..�� ������ �׻� Unacquire->Release ������ �̷������.
void InputClass::Shutdown()
{
	// ���콺 ������ ����
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Ű���� ������ ����
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// direct input �������̽� ����
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

// Frame�Լ��� ��ġ�� ���� ���¸� �о� �ռ� ���� ���¹��ۿ� ����Ѵ�.
// �� ��ġ�� ���¸� ���� �� ��������� ó���ϰ� �Ѵ�.
bool InputClass::Frame() {
	bool result;

	// ���� Ű������ ���¸� �д´�.
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	// ���� ���콺�� ���¸� �д´�.
	result = ReadMouse();
	if (!result) {
		return false;
	}

	// ���콺�� Ű������ ��ȭ�� ó���Ѵ�.
	ProcessInput();

	return true;
}

// ReadKeyboard �Լ��� Ű������ ���¸� m_keyboardState ������ �����Ѵ�.
// �� ���� ������ ��� Ű�� ���� ���� ���ȴ��� �׷��� ������ �����ش�.
// ���� Ű���� ���¸� �дµ� �����Ѵٸ� �ټ����� ���ε� �� �ϳ���.
// ���⼭ �ٷ� �� ������ Ű���尡 ��Ŀ���� �Ұų� ��� �Ұ� ������ ����.
// �̷� ����� �� �����Ӹ��� ������� �������� ������ Acquire �Լ��� ȣ���ؾ��Ѵ�
// �����찡 �ּ�ȭ�� ��� Acquire �Լ��� �����ϰ� �ȴ�. ������ �ٽ� �ǵ��� �� ��� Acquire �Լ��� �ٽ� Ű���� ���¸� ���� �� �ְ� �ȴ�.
// �ٸ� ���� Ÿ�Ե��� �ٷ��� �ʴ´�.
bool InputClass::ReadKeyboard() {
	HRESULT result;

	// Ű���� ��ġ�� ���¸� �д´�.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {
		// Ű���尡 ��Ŀ���� �Ұų�, ���Ұ��� ��� �ٽ� Acquire�Լ��� �������Ӹ��� ȣ���ؼ� ������� �����޾ƾ���
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}
	return true;
}

// ReadMouse �Լ��� Ű������ ���¸� �д� ReadKeyboard �Լ�ó�� ���콺�� ���¸� �д´�.
// ������ ���콺�� ���´� ���� �����Ӱ� �޶��� ��ġ(����)���� �����Ѵ�.
// ���� ��� ���콺�� 5������ŭ ���������� �̵����״ٸ� ȭ����� ��ġ�� �˷������� �ʴ´�.
// ������ ����(delta) ������ �˾Ƴ� �� �ֱ� ������ �̸� �̿��Ͽ� ���콺�� ��ġ�� ��ü������ ������ �� �ִ�.
bool InputClass::ReadMouse() {
	HRESULT result;

	// ���콺 ��ġ�� ���¸� �д´�.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		// ���콺�� ��Ŀ���� �Ұų�, ���Ұ��� ��� �ٽ� Acquire�Լ��� �������Ӹ��� ȣ���ؼ� ������� �����޾ƾ���
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_mouse->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

// ProcessInput �Լ��� ���� �����ӿ����� ��������� ������ ����Ǵ� ���̴�.
// �� Ʃ�丮�󿡼��� �ܼ��� ���콺�� ��ġ�� ��� ��ó�� ���� ���콺 ��ġ�� �����ϵ��� �Ѵ�.
// �׷��� ���ؼ��� m_mouseX, m_mouseY ������ ����Ͽ� ���콺�� ��ġ�� ����Ѵ�. 
// �̷��� �Ͽ� ������ ���콺�� �������� �� ��ġ�� ��� ���󰡰��Ѵ�.

// �����ؾ��� ���� ���콺�� ȭ�� �ٱ����� ������ �ʰ� �ؾ��Ѵٴ� ���̴�.
// ������ ������ ���콺�� 0�� ��ġ���� ��� �������� �����δ��ϴ��� �ٽ� ���������� ������������ ���콺 ��ġ�� ��� 0���� �������Ѿ��Ѵ�.
void InputClass::ProcessInput() {

	// ���콺 �������� ������ ���콺 ��ġ���� ����Ѵ�.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// ���� ȭ������� �Ѿ�� ���� ������Ų��.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

// IsEscapePressed��� �Լ��� ��� Ư�� Ű�� ���ȴ��� Ȯ���� �� �ִ� �Լ���.
// �� ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� ���� �ִ�.
bool InputClass::IsEscapePressed() {

	// ���� ESCŰ�� ���ȴ��� Ȯ��
	if (m_keyboardState[DIK_ESCAPE] & 0x80) {
		return true;
	}

	return false;
}


bool InputClass::IsLeftPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsRightPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsAPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsZPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsPgUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsPgDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}