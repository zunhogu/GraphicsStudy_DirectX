#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "textureclass.h"

class BitmapClass {
private:
	struct VertexType {
		// 2D 이미지를 그리기위해서는 단순히 위치벡터와 텍스처 좌표만 가져오면 된다.
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int); 
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*); 
	void ShutdownBuffers(); 
	bool UpdateBuffers(ID3D11DeviceContext*, int, int); 
	void RenderBuffers(ID3D11DeviceContext*); 

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture(); 

private: 
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer; 
	int m_vertexCount, m_indexCount; 
	TextureClass* m_texture;

	// 3D ModelClass와는 다르게 BitmapClass에서는 화면 크기, 이미지 크기, 이전에 그렸던 위치를 기억해야한다.
	int m_screenWidth, m_screenHeight; 
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
}; 

#endif