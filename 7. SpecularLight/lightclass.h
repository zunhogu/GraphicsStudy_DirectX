#ifndef _LIGHTCLASS_H_ 
#define _LIGHTCLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

class LightClass {
private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_ambientColor;
public: 
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetAmbientColor(float, float, float, float);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	XMFLOAT4 GetAmbientColor();
};

#endif