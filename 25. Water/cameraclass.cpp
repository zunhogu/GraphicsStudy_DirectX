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
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upV, positonV, lookAtV;
	float radians;

	// 위쪽을 가리키는 벡터를 만든다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upV = XMLoadFloat3(&up);

	// 카메라 위치를 생성한다. 단 평면 반사를 위해 y 위치는 역전시킨다.
	// 평면 반사를 위해 카메라의 Y값을 역전시킵니다.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positonV = XMLoadFloat3(&position);

	// 라디안 계산
	radians = m_rotationY * 0.0174532925f;

	// 카메라가 바라보는 방향를 초기화한다.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = m_positionY;
	lookAt.z = cosf(radians) + m_positionZ;

	lookAtV = XMLoadFloat3(&lookAt);

	// 뷰행렬을 만든다.
	m_viewMatrix = XMMatrixLookAtLH(positonV, lookAtV, upV);

	return;
}

// 뷰행령을 생성하는 Render 함수를 수행한 이후로는 GetViewMatrix 함수를 사용하여 뷰행렬을 얻어 올 수 있게 된다.
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix){
	viewMatrix = m_viewMatrix;
	return;
}

// RenerReflection 함수는 보통 Render함수에서 뷰 행렬을 생성하는 것과 같은 방법으로 반사행렬을 만든다.
// 한가지 차이점이라면 Y축에 있는 평면까지의 높이를 인자로 받아서 반사를 위해 이 값을 역전시킨다는 것이다.
// 이렇게 하면 셰이더에서 사용할 반사행렬을 계산할 수 있다. 
void CameraClass::RenderReflection(float height){
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upV, positonV, lookAtV;
	float radians;

	// 위쪽을 가리키는 벡터를 만든다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upV = XMLoadFloat3(&up);

	// 카메라 위치를 생성한다. 단 평면 반사를 위해 y 위치는 역전시킨다.
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f);
	position.z = m_positionZ;

	positonV = XMLoadFloat3(&position);

	// 라디안 계산
	radians = m_rotationY * 0.0174532925f;

	// 카메라가 바라보는 방향를 초기화한다.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_positionZ;

	lookAtV = XMLoadFloat3(&lookAt);

	// 위의 세 벡터를 이용하여 뷰 행렬을 생성한다.
	m_reflectionViewMatrix = XMMatrixLookAtLH(positonV, lookAtV, upV);

	return;
}

XMMATRIX CameraClass::GetReflectionViewMatrix() {
	return m_reflectionViewMatrix;
}