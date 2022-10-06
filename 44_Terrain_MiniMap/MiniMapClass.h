#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "bitmapclass.h"
#include "textureshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: MiniMapClass
////////////////////////////////////////////////////////////////////////////////
class MiniMapClass
{
public:
	MiniMapClass();
	MiniMapClass(const MiniMapClass&);
	~MiniMapClass();

	bool Initialize(ID3D11Device*, HWND, int, int, XMMATRIX, float, float);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, TextureShaderClass*);

	void PositionUpdate(float, float);

private:
	int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
	float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
	XMMATRIX m_viewMatrix;

	// MiniMap에 사용되는 비트맵은 다음과 같다. MiniMap 자체의 비트맵, 미니맵 경계에 대한 비트맵, 미니맵에 표시해줄 좌표에대한 비트맵 총 3개다.
	BitmapClass * m_MiniMapBitmap, * m_Border, * m_Point;
};
