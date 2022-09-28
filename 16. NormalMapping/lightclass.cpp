#include "lightclass.h"

LightClass::LightClass() {}

LightClass::LightClass(const LightClass& other) {}

LightClass::~LightClass(){}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha) {
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetPosition(float x, float y, float z) {
	m_Position = XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha) {
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

XMFLOAT4 LightClass::GetDiffuseColor() {
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetPosition() {
	return m_Position;
}

XMFLOAT4 LightClass::GetAmbientColor() {
	return m_ambientColor;
}