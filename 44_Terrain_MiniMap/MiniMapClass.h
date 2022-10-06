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

	// MiniMap�� ���Ǵ� ��Ʈ���� ������ ����. MiniMap ��ü�� ��Ʈ��, �̴ϸ� ��迡 ���� ��Ʈ��, �̴ϸʿ� ǥ������ ��ǥ������ ��Ʈ�� �� 3����.
	BitmapClass * m_MiniMapBitmap, * m_Border, * m_Point;
};
