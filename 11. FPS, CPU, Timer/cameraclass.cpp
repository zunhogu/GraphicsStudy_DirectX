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


// SetPosition �Լ��� SetRotation �Լ��� ī�޶��� ��ġ�� ȸ���� ���ϴ� �� ���ȴ�.

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

// GetPosition �Լ��� GetRotation �Լ��� ȣ���� �Լ����� ī�޶��� ��ġ�� ȸ�� ������ �����ش�.

XMFLOAT3 CameraClass::GetPosition(){
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 CameraClass::GetRotation(){
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


// Render �Լ��� ī�޶��� ��ġ�� ȸ�� ������ ������ �� ����� ����Ѵ�.
// �켱 �����ǥ�� ���⺤��(up) ��ġ, ȸ�� ���� ������ �ʱ�ȭ�Ѵ�.
// ȭ���� �������� ī�޶��� x, y, z ���� ȸ�� ������ ī�޶��� ȸ���� �����Ѵ�.
// ȸ���� �� �̷������ ī�޶� 3D ������ �ùٸ� ��ġ�� �Űܳ��´�.
// position�� lookAt �׸��� up ���� �� �����Ǹ� D3DXMatrixLookAtLH �Լ��� �̿��� ������� ����� �ִ�.

void CameraClass::Render() {
	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// ���⺤�� ����
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// ī�޶� ��ġ ����
	position = XMVectorSet(m_positionX, m_positionY, m_positionZ, 0.0f);

	// ī�޶� �ٶ󺸴� ���� ����
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// ī�޶��� ȸ�� ���� ����
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// ī�޶� ȸ��
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// ����� ���ϱ�
	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up =  XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
	
	return;
}

// ������� �����ϴ� Render �Լ��� ������ ���ķδ� GetViewMatrix �Լ��� ����Ͽ� ������� ��� �� �� �ְ� �ȴ�.
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix){
	viewMatrix = m_viewMatrix;
	return;
}
