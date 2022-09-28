#include "lightclass.h"

LightClass::LightClass() {}

LightClass::LightClass(const LightClass& other) {}

LightClass::~LightClass() {}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha) {
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha) {
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetPosition(float x, float y, float z) {
	m_position = XMFLOAT3(x, y, z);
}

void LightClass::SetLookAt(float x, float y, float z) {
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	return;
}

XMFLOAT4 LightClass::GetAmbientColor() {
	return m_ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor() {
	return m_diffuseColor; 
} 

XMFLOAT3 LightClass::GetPosition() {
	return m_position; 
}

void LightClass::GenerateViewMatrix() {
	XMFLOAT3 up;
	XMVECTOR positionV, lookAtV, upV;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	positionV = XMLoadFloat3(&m_position);
	lookAtV = XMLoadFloat3(&m_lookAt);
	upV = XMLoadFloat3(&up);

	m_viewMatrix = XMMatrixLookAtLH(positionV, lookAtV, upV);
	
	return;
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear) {
	float fieldOfView, screenAspect;

	// 종횡비 설정
	fieldOfView = (float)XM_PI / 2.0f; 
	screenAspect = 1.0f; 

	// 투영행렬 생성
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	return; 
}

void LightClass::GetViewMatrix(XMMATRIX& viewMatrix) {
	viewMatrix = m_viewMatrix; 
	return; 
}

void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix) {
	projectionMatrix = m_projectionMatrix;
	return;
}


