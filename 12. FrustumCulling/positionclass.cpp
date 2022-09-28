#include "positionclass.h"

PositionClass::PositionClass(){
	m_frameTime = 0.0f;
	m_rotationY = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
}


PositionClass::PositionClass(const PositionClass& other){}


PositionClass::~PositionClass(){}

// SetFrameTime 함수는 이 클래스의 프레임 속도를 설정하기 위해 사용된다. PositionClass는 이 프레임 시간을 사용하여 카메라가 얼마나 빠르게 움직이고 회전하는지 계산한다.
// 이 함수는 카메라가 움직이기 전 매 프레임의 시작에 불리도록해야한다.
void PositionClass::SetFrameTime(float time){
	m_frameTime = time;
	return;
}

// GetRotation 함수는 카메라의 Y축 회전 정도를 리턴한다..이 튜토리얼을 위해 만든 도우미 함수일 뿐이지만 나중에 더 많은 정보를 알려주도록 확장될 수 있다.
void PositionClass::GetRotation(float& y){
	y = m_rotationY;
	return;
}

// 두 움직임 관련 함수는 같은 일을 한다. 둘 다 매 프레임마다 호출된다. 각 함수마다 있는 keydown 인자는 가각 유저가 왼쪽이나 오른쪽 키를 눌렀는지를 나타낸다.
// 만약 키가 눌려 있다면 매 프레임마다 최고 속도까지 해당 방향으로 가속된다. 이런 방식의 속도 증가는 자동차의 부드러운 움직이과 똑같다.
// 그와 비슷하게 유저가 키를 떼는 경우 keydown 변수는 false가 되고 그때 매프레임마다 속도가 일정한 비율로 부드럽게 줄어드러 마침내 0이된다.
// 이 속도는 프레임간 시간에 기초해 있기 때문에 fps값에 상관없이 동일하게 움직인다. 각 함수는 카메라의 새 위치를 계산하기 위해 간단한 연산을 한다.
void PositionClass::TurnLeft(bool keydown){
	// If the key is pressed increase the speed at which the camera turns left.  If not slow down the turn speed.
	if (keydown){
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f)){
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else{
		m_leftTurnSpeed -= m_frameTime * 0.005f;

		if (m_leftTurnSpeed < 0.0f){
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY -= m_leftTurnSpeed;
	if (m_rotationY < 0.0f){
		m_rotationY += 360.0f;
	}

	return;
}


void PositionClass::TurnRight(bool keydown){
	// If the key is pressed increase the speed at which the camera turns right.  If not slow down the turn speed.
	if (keydown){
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f)){
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else{
		m_rightTurnSpeed -= m_frameTime * 0.005f;

		if (m_rightTurnSpeed < 0.0f){
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY += m_rightTurnSpeed;
	if(m_rotationY > 360.0f){
		m_rotationY -= 360.0f;
	}

	return;
}
