#ifndef _DEBUGWINDOWCLASS_H_
#define _DEBUGWINDOWCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// DebugWindowClass�� �ڽ��� �ؽ�ó�� ������ ���� �ʰ� RTT�� �׷��� �ؽ�ó�� ����Ѵ�.
// �� Ŭ������ ������ RTT�� ����� ���� �����ֱ� ���� ����� �ؽ�ó Ŭ������ ���� modelclass��.
// �� Ŭ������ ���� / �ε��� ���۸� �����ϰ� �̸� ���̴��� ����ִ´�.
// ���ο� ���̴��� ����ó���� ȿ���� ����� �Ҷ� �������� ȭ���� ���ԵǸ� �� �ܰ躰�� ��������� ���ÿ� ����� �� �����Ƿ� ������� �����ϴ�.
class DebugWindowClass {
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
	DebugWindowClass();
	DebugWindowClass(const DebugWindowClass&);
	~DebugWindowClass();

	bool Initialize(ID3D11Device*, int, int, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*); 
	void ShutdownBuffers(); 
	bool UpdateBuffers(ID3D11DeviceContext*, int, int); 
	void RenderBuffers(ID3D11DeviceContext*);
};


#endif