#include "inputclass.h"

//SystemClass.cpp와 똑같은 이유로 멤버변수를 null값으로, 복사생성자와 소멸자는 아무것도 안하게 정의한다.
InputClass::InputClass() {}

InputClass::InputClass(const InputClass& other) {}

InputClass::~InputClass() {}

void InputClass::Initialize() {
	int i;

	// 모든키를 눌리지 않은상캐로 초기화한다.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input) {
	// 키가 눌렸다면 그 상태로 배열에 저장한다.
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input) {
	// 키가 떼어졌다면 그 상태로 배열에 저장한다.
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key) {
	// 현재 키가 눌림/뗌 상태인지 반환한다.
	return m_keys[key];
}