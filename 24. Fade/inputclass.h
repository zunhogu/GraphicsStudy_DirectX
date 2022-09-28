#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

//����� ������ �ڵ带 ���� �����쿡�� �����ϴ� Input�� ��������� �����δ� DirectX���� �����ϴ� Input�� �ξ� ������.

//InputClass�� ������ Ű���带 �Է��Ѵ�.�� Ŭ������ SystemClass::MessageHandler() �κ��� �Է��� ���� �޴´�.

//�� ��ü�� Ű���� �迭�� �ִ� �� Ű�� ���¸� �����Ѵ�.���� Ű���� ���¿����� ���ǰ� ������ �� ��ü�� ���� ��Ű�� ���ȴ��������� ������ �˷��ش�.

class InputClass {
private:
	bool m_keys[256];
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

};

#endif