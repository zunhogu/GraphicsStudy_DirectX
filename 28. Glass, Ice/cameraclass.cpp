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
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upV, positonV, lookAtV;
	float radians;

	// ������ ����Ű�� ���͸� �����.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upV = XMLoadFloat3(&up);

	// ī�޶� ��ġ�� �����Ѵ�. �� ��� �ݻ縦 ���� y ��ġ�� ������Ų��.
	// ��� �ݻ縦 ���� ī�޶��� Y���� ������ŵ�ϴ�.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positonV = XMLoadFloat3(&position);

	// ���� ���
	radians = m_rotationY * 0.0174532925f;

	// ī�޶� �ٶ󺸴� ���⸦ �ʱ�ȭ�Ѵ�.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = m_positionY;
	lookAt.z = cosf(radians) + m_positionZ;

	lookAtV = XMLoadFloat3(&lookAt);

	// ������� �����.
	m_viewMatrix = XMMatrixLookAtLH(positonV, lookAtV, upV);

	return;
}

// ������� �����ϴ� Render �Լ��� ������ ���ķδ� GetViewMatrix �Լ��� ����Ͽ� ������� ��� �� �� �ְ� �ȴ�.
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix){
	viewMatrix = m_viewMatrix;
	return;
}

// RenerReflection �Լ��� ���� Render�Լ����� �� ����� �����ϴ� �Ͱ� ���� ������� �ݻ������ �����.
// �Ѱ��� �������̶�� Y�࿡ �ִ� �������� ���̸� ���ڷ� �޾Ƽ� �ݻ縦 ���� �� ���� ������Ų�ٴ� ���̴�.
// �̷��� �ϸ� ���̴����� ����� �ݻ������ ����� �� �ִ�. 
void CameraClass::RenderReflection(float height){
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upV, positonV, lookAtV;
	float radians;

	// ������ ����Ű�� ���͸� �����.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upV = XMLoadFloat3(&up);

	// ī�޶� ��ġ�� �����Ѵ�. �� ��� �ݻ縦 ���� y ��ġ�� ������Ų��.
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f);
	position.z = m_positionZ;

	positonV = XMLoadFloat3(&position);

	// ���� ���
	radians = m_rotationY * 0.0174532925f;

	// ī�޶� �ٶ󺸴� ���⸦ �ʱ�ȭ�Ѵ�.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_positionZ;

	lookAtV = XMLoadFloat3(&lookAt);

	// ���� �� ���͸� �̿��Ͽ� �� ����� �����Ѵ�.
	m_reflectionViewMatrix = XMMatrixLookAtLH(positonV, lookAtV, upV);

	return;
}

XMMATRIX CameraClass::GetReflectionViewMatrix() {
	return m_reflectionViewMatrix;
}