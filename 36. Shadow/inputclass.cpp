#include "inputclass.h"

//SystemClass.cpp와 똑같은 이유로 멤버변수를 null값으로, 복사생성자와 소멸자는 아무것도 안하게 정의한다.
InputClass::InputClass() {
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other) {}

InputClass::~InputClass() {}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	HRESULT result;

	// 화면의 너비, 높이를 저장한다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 마우스 위치를 초기화한다.
	m_mouseX = 0;
	m_mouseY = 0;

	////////////////////////////////////////////////////////////////////////////////
	// Direct input의 인터페이스를 초기화한다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// 키보드 입력장치를 초기화한다.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 입력할 데이터 포맷을 세팅한다.
	// 이는 키보드에서 우리가 입력할 데이터를 미리 정의할 수 있다.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	// 키보드 협력레벨(cooperative level)을 정하는 것은 이 장치가 무엇을 하는지, 그리고 어떻게 사용될 것인지를 결정하므로 중요하다.
	// DISCL_EXCLUSIVE 플래그로 다른 프로그램들과 공유하지 않는다고 알려준다. (배제 상태)
	// 이렇게 하면 오직 이 어플리케이션만 이 입력을 감지할 수 있게 해준다.
	// 하지만 만일 이 어플리케이션이 아닌 다른 어플리케이션에도 키보드 입력에 접근가능하게 하려면 DISCL_NONEXCLUSIVE를 사용하면 된다. (비배제 상태)
	// 중요한 것은 비배제 상태이고 풀스크린이 아니라면 장치가 다른 윈도우로 포커스가 이동했는지, 그리고 다시 포커스를 얻어서 장치르 사용할 수 있게 되엇는지 확인한다는 것이다.
	// 이런 포커스를 잃어버린 경우를 예를 들면 대개 다른 윈도우가 메인 윈도우가 되어 포커스를 얻었다던가 아니면 우리 어플리케이션이 최소화된 경우를 말한다.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// 키보드 세팅이 되면 Acquire 함수를 호출하여 이 포인터로 키보드에 대한 접근을 취득한다.
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// 마우스 입력장치를 초기화한다.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 마우스 입력장치의 포맷을 세팅한다.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	// 마우에 대해서는 비배제 상태로 설정한다. 마우스가 포커스를 잃었는지 다시 찾았는지 확인해야한다.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// 마우스 세팅이 되면 Acquire 함수를 호출하여 이 포인터로 마우스 대한 접근을 취득한다.
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}
	return  true;
}

// Shutdown 함수는 두 장치와 direct input에 대한 인터페이스를 해제한다..이 절차는 항상 Unacquire->Release 순으로 이루어진다.
void InputClass::Shutdown()
{
	// 마우스 포인터 해제
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// 키보드 포인터 해제
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// direct input 인터페이스 해제
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

// Frame함수는 장치의 현재 상태를 읽어 앞서 만든 상태버퍼에 기록한다.
// 각 장치의 상태를 읽은 뒤 변경사항을 처리하게 한다.
bool InputClass::Frame() {
	bool result;

	// 현재 키보드의 상태를 읽는다.
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	// 현재 마우스의 상태를 읽는다.
	result = ReadMouse();
	if (!result) {
		return false;
	}

	// 마우스와 키보드의 변화를 처리한다.
	ProcessInput();

	return true;
}

// ReadKeyboard 함수는 키보드의 상태를 m_keyboardState 변수에 저장한다.
// 이 상태 변수는 모든 키에 대해 현재 눌렸는지 그렇지 않은지 보여준다.
// 만약 키보드 상태를 읽는데 실패한다면 다섯가지 요인들 중 하나다.
// 여기서 다룰 두 요인은 키보드가 포커스를 잃거나 취득 불가 상태인 경우다.
// 이런 경우라면 매 프레임마다 제어권을 돌려받을 때까지 Acquire 함수를 호출해야한다
// 윈도우가 최소화된 경우 Acquire 함수가 실패하게 된다. 하지만 다시 되돌아 온 경우 Acquire 함수는 다시 키보드 상태를 읽을 수 있게 된다.
// 다른 에러 타입들은 다루지 않는다.
bool InputClass::ReadKeyboard() {
	HRESULT result;

	// 키보드 장치의 상태를 읽는다.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {
		// 키보드가 포커스를 잃거나, 취득불가인 경우 다시 Acquire함수를 매프레임마다 호출해서 제어권을 돌려받아야함
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}
	return true;
}

// ReadMouse 함수는 키보드의 상태를 읽는 ReadKeyboard 함수처럼 마우스의 상태를 읽는다.
// 하지만 마우스의 상태는 이전 프레임과 달라진 위치(변위)만을 저장한다.
// 예를 들어 마우스를 5단위만큼 오른쪽으로 이동시켰다면 화면상의 위치를 알려주지는 않는다.
// 하지만 변위(delta) 정보는 알아낼 수 있기 때문에 이를 이용하여 마우스의 위치를 자체적으로 관리할 수 있다.
bool InputClass::ReadMouse() {
	HRESULT result;

	// 마우스 장치의 상태를 읽는다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		// 마우스가 포커스를 잃거나, 취득불가인 경우 다시 Acquire함수를 매프레임마다 호출해서 제어권을 돌려받아야함
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_mouse->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

// ProcessInput 함수는 이전 프레임에서의 변경사항이 실제로 적용되는 곳이다.
// 이 튜토리얼에서는 단순히 마우스의 위치를 잡는 것처럼 현재 마우스 위치를 갱신하도록 한다.
// 그러기 위해서는 m_mouseX, m_mouseY 변수를 사용하여 마우스의 위치를 기록한다. 
// 이렇게 하여 유저가 마우스를 움직여도 그 위치를 계속 따라가게한다.

// 주의해야할 점은 마우스가 화면 바깥으로 나가지 않게 해야한다는 것이다.
// 심지어 유저가 마우스를 0의 위치에서 계속 왼쪽으로 움직인다하더라도 다시 오른쪽으로 가기전까지는 마우스 위치가 계속 0으로 고전시켜야한다.
void InputClass::ProcessInput() {

	// 마우스 변위값을 가지고 마우스 위치값을 기록한다.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 만약 화면범위를 넘어서면 값을 고정시킨다.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

// IsEscapePressed라는 함수는 어떻게 특정 키가 눌렸는지 확인할 수 있는 함수다.
// 또 어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수도 있다.
bool InputClass::IsEscapePressed() {

	// 현재 ESC키가 눌렸는지 확인
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