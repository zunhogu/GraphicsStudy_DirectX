#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

//현재는 간단한 코드를 위해 윈도우에서 제공하는 Input을 사용하지만 실제로는 DirectX에서 제공하는 Input이 훨씬 빠르다.

//InputClass는 유저의 키보드를 입력한다.이 클래스는 SystemClass::MessageHandler() 로부터 입력을 전달 받는다.

//이 객체는 키보드 배열에 있는 각 키의 상태를 저장한다.만약 키보드 상태에대한 질의가 들어오면 이 객체는 현재 이키가 눌렸는지에대한 정보를 알려준다.

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