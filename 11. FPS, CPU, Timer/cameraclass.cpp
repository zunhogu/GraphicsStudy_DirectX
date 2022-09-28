#include "cameraclass.h"

CameraClass::CameraClass(){
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other){}


CameraClass::~CameraClass(){}


// SetPosition 함수와 SetRotation 함수는 카메라의 위치와 회전을 정하는 데 사용된다.

void CameraClass::SetPosition(float x, float y, float z){
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z){
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

// GetPosition 함수와 GetRotation 함수는 호출한 함수에게 카메라의 위치와 회전 정보를 돌려준다.

XMFLOAT3 CameraClass::GetPosition(){
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 CameraClass::GetRotation(){
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


// Render 함수는 카메라의 위치와 회전 정보를 가지고 뷰 행렬을 계산한다.
// 우선 월드봐표계 상향벡터(up) 위치, 회전 등의 변수를 초기화한다.
// 화면의 원점에서 카메라의 x, y, z 축의 회전 정도로 카메라의 회전을 수행한다.
// 회전이 잘 이루어지면 카메라를 3D 세계의 올바른 위치로 옮겨놓는다.
// position과 lookAt 그리고 up 값이 잘 설정되면 D3DXMatrixLookAtLH 함수를 이용해 뷰행렬을 만들수 있다.

void CameraClass::Render() {
	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// 상향벡터 설정
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// 카메라 위치 설정
	position = XMVectorSet(m_positionX, m_positionY, m_positionZ, 0.0f);

	// 카메라가 바라보는 방향 설정
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// 카메라의 회전 정도 설정
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// 카메라 회전
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 뷰행렬 구하기
	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up =  XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
	
	return;
}

// 뷰행령을 생성하는 Render 함수를 수행한 이후로는 GetViewMatrix 함수를 사용하여 뷰행렬을 얻어 올 수 있게 된다.
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix){
	viewMatrix = m_viewMatrix;
	return;
}
