#include "positionclass.h"

PositionClass::PositionClass(){
	m_frameTime = 0.0f;
	m_rotationY = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
}


PositionClass::PositionClass(const PositionClass& other){}


PositionClass::~PositionClass(){}

// SetFrameTime �Լ��� �� Ŭ������ ������ �ӵ��� �����ϱ� ���� ���ȴ�. PositionClass�� �� ������ �ð��� ����Ͽ� ī�޶� �󸶳� ������ �����̰� ȸ���ϴ��� ����Ѵ�.
// �� �Լ��� ī�޶� �����̱� �� �� �������� ���ۿ� �Ҹ������ؾ��Ѵ�.
void PositionClass::SetFrameTime(float time){
	m_frameTime = time;
	return;
}

// GetRotation �Լ��� ī�޶��� Y�� ȸ�� ������ �����Ѵ�..�� Ʃ�丮���� ���� ���� ����� �Լ��� �������� ���߿� �� ���� ������ �˷��ֵ��� Ȯ��� �� �ִ�.
void PositionClass::GetRotation(float& y){
	y = m_rotationY;
	return;
}

// �� ������ ���� �Լ��� ���� ���� �Ѵ�. �� �� �� �����Ӹ��� ȣ��ȴ�. �� �Լ����� �ִ� keydown ���ڴ� ���� ������ �����̳� ������ Ű�� ���������� ��Ÿ����.
// ���� Ű�� ���� �ִٸ� �� �����Ӹ��� �ְ� �ӵ����� �ش� �������� ���ӵȴ�. �̷� ����� �ӵ� ������ �ڵ����� �ε巯�� �����̰� �Ȱ���.
// �׿� ����ϰ� ������ Ű�� ���� ��� keydown ������ false�� �ǰ� �׶� �������Ӹ��� �ӵ��� ������ ������ �ε巴�� �پ�巯 ��ħ�� 0�̵ȴ�.
// �� �ӵ��� �����Ӱ� �ð��� ������ �ֱ� ������ fps���� ������� �����ϰ� �����δ�. �� �Լ��� ī�޶��� �� ��ġ�� ����ϱ� ���� ������ ������ �Ѵ�.
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
