#ifndef _LIGHTCLASS_H_ 
#define _LIGHTCLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

class LightClass {
private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_specularColor;
	float m_specularPower;
	XMFLOAT3 m_direction;
public: 
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);
	void SetDirection(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	XMFLOAT3 GetDirection();
};

#endif