#ifndef _ORTHOWINDOWCLASS_H_ 
#define _ORTHOWINDOWCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// DebugWindowClass�� �ڽ��� �ؽ�ó�� ������ ���� �ʰ� RTT�� �׷��� �ؽ�ó�� ����Ѵ�.
// �� Ŭ������ ������ RTT�� ����� ���� �����ֱ� ���� ����� �ؽ�ó Ŭ������ ���� modelclass��.
// �� Ŭ������ ���� / �ε��� ���۸� �����ϰ� �̸� ���̴��� ����ִ´�.
// ���ο� ���̴��� ����ó���� ȿ���� ����� �Ҷ� �������� ȭ���� ���ԵǸ� �� �ܰ躰�� ��������� ���ÿ� ����� �� �����Ƿ� ������� �����ϴ�.
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