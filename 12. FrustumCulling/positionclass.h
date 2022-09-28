#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_

#include <math.h>

// 카메라를 왼쪽 / 오른쪽 화살표 키로 조작하기 위해 카메라의 위치를 계산하고 유지하는 일을 하는 클래스다.
// 지금 당장 이클랫느느 좌우 회전하는 것 밖에 할 수 없지만 나중에 다른 움직임을 넣도록 확장할 수 있다.
// 구현하는 움직임 중에는 부드러운 카메라 이동효과를 위한 가속이나 감속도 포함된다.
class PositionClass{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetFrameTime(float);
	void GetRotation(float&);

	void TurnLeft(bool);
	void TurnRight(bool);

private:
	float m_frameTime;
	float m_rotationY;
	float m_leftTurnSpeed, m_rightTurnSpeed;
};

#endif