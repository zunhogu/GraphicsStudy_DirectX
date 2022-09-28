#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_

#include <math.h>

// ī�޶� ���� / ������ ȭ��ǥ Ű�� �����ϱ� ���� ī�޶��� ��ġ�� ����ϰ� �����ϴ� ���� �ϴ� Ŭ������.
// ���� ���� ��Ŭ������ �¿� ȸ���ϴ� �� �ۿ� �� �� ������ ���߿� �ٸ� �������� �ֵ��� Ȯ���� �� �ִ�.
// �����ϴ� ������ �߿��� �ε巯�� ī�޶� �̵�ȿ���� ���� �����̳� ���ӵ� ���Եȴ�.
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