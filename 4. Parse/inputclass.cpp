#include "inputclass.h"

//SystemClass.cpp�� �Ȱ��� ������ ��������� null������, ��������ڿ� �Ҹ��ڴ� �ƹ��͵� ���ϰ� �����Ѵ�.
InputClass::InputClass() {}

InputClass::InputClass(const InputClass& other) {}

InputClass::~InputClass() {}

void InputClass::Initialize() {
	int i;

	// ���Ű�� ������ ������ĳ�� �ʱ�ȭ�Ѵ�.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input) {
	// Ű�� ���ȴٸ� �� ���·� �迭�� �����Ѵ�.
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input) {
	// Ű�� �������ٸ� �� ���·� �迭�� �����Ѵ�.
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key) {
	// ���� Ű�� ����/�� �������� ��ȯ�Ѵ�.
	return m_keys[key];
}