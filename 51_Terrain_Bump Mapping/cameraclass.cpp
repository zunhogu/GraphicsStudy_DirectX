////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


void CameraClass::Render()
{
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
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix) {
	viewMatrix = m_viewMatrix;
	return;
}

void CameraClass::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	float radians;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Calculate the rotation in radians.
	radians = m_rotationY * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = m_positionY;
	lookAt.z = cosf(radians) + m_positionZ;

	XMVECTOR posV = XMLoadFloat3(&position);
	XMVECTOR lookAtV = XMLoadFloat3(&lookAt);
	XMVECTOR upV = XMLoadFloat3(&up);

	// Create the base view matrix from the three vectors.
	m_baseViewMatrix = XMMatrixLookAtLH(posV, lookAtV, upV);

	return;
}


void CameraClass::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
	return;
}