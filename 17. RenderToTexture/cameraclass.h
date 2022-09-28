// 지금까지 정점/인덱스 버퍼를 셋업하고, 셰이더를 호출하였다. 마지막으로 필요한 것은 월드에서 우리가 바라보는 시점이다.
// 이를 구현하기 위해서는 어떻게 우리가 장면을 보는지에 대한 정보를 DirectX11에게 전달하는 카메라 클래스가 필요하다.
// 카메라 클래스는 카메라의 위치와 현재 회전 상태를 계속 가지고 있어야한다. 또한 이 이정보를 활용해서 셰이더에서 사용할 뷰 행렬도 생성해야한다.

#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <DirectXMath.h>
using namespace DirectX;


class CameraClass{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	// SetPosition과 SetRotation 함수는 현재 카메라의 위치나, 회전상태를 설정하는데 사용 할 것이다. 
	// Render 함수는 카메라의 위치와 회전 상태에 기반한 뷰 행렬을 생성하는데 사용 될것이다.
	// 마지막으로 GetViewMatrix 함순느 셰이더에서 카메라 객체의 뷰행렬을 받아오는데 사용 할 것이다. 
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	
	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif