#ifndef _ORTHOWINDOWCLASS_H_ 
#define _ORTHOWINDOWCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// DebugWindowClass는 자신의 텍스처는 가지고 있지 않고 RTT로 그려진 텍스처를 사용한다.
// 이 클래스의 목적은 RTT의 결과를 따로 보여주기 위해 만드는 텍스처 클래스가 없는 modelclass다.
// 이 클래스로 정점 / 인덱스 버퍼를 생성하고 이를 셰이더에 집어넣는다.
// 새로운 셰이더나 다중처리된 효과를 디버그 할때 여러개의 화면을 띄우게되면 각 단계별로 결과물들을 동시에 띄워볼 수 있으므로 디버깅이 용이하다.
class OrthoWindowClass {
private:
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer; 
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;

public:
	OrthoWindowClass();
	OrthoWindowClass(const OrthoWindowClass&);
	~OrthoWindowClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*, int, int); 
	void ShutdownBuffers(); 
	void RenderBuffers(ID3D11DeviceContext*);
};


#endif