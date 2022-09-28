// ModelClass�� 3D �𵨵��� ������ �������� ĸ��ȭ�ϴ� Ŭ������.
// �� �ڵ忡���� ������ ���� �ﰢ���� ������
// �� �ﰢ���� ȭ�鿡 �׷����� ���� �������ۿ� ���ι��۵� ������.

#ifndef _MODELCLASS_H_ 
#define _MODELCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;

class ModelClass {
private :
	// �������ۿ� �� ���������� �������ش�.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
public :
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// �Ʒ� �Լ����� 3D ���� �������ۿ� �ε��� ���۵��� �ʱ�ȭ�� ���� ������ �����Ѵ�.
	// Render �Լ��� GPU�� �𵨵��� ���������� �ְ� �÷� ���̴��� �׸� �غ� �Ѵ�.

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private :
	// �Ʒ� ��� �������� ��������, �ε��� ����, �� ������ ũ�� ������ ������ �ִ� ���۴�.
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif