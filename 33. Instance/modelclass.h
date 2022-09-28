// ModelClass�� 3D �𵨵��� ������ �������� ĸ��ȭ�ϴ� Ŭ������.
// �� �ڵ忡���� ������ ���� �ﰢ���� ������
// �� �ﰢ���� ȭ�鿡 �׷����� ���� �������ۿ� ���ι��۵� ������.

#ifndef _MODELCLASS_H_ 
#define _MODELCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;

// �� Ŭ���������� �ؽ�ó �ڿ��� �����ؾ��Ѵ�. �� ������ ���۸� ���鶧 �� �Է°��� �غ��ϴ� �������� ���̴��� uv��ǥ�� ���ϱ⶧���̴�.
#include "textureclass.h"

class ModelClass {
private :
	// �������ۿ� �� ���������� �������ش�.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	struct InstanceType {
		XMFLOAT3 position;
	};
public :
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// �Ʒ� �Լ����� 3D ���� �������ۿ� �ε��� ���۵��� �ʱ�ȭ�� ���� ������ �����Ѵ�.
	// Render �Լ��� GPU�� �𵨵��� ���������� �ְ� �÷� ���̴��� �׸� �غ� �Ѵ�.

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	// GetInstanceCount �Լ��� ������ �ε��� ������ �������� ��ɰ� �����ϴ�.
	int GetInstanceCount(); 

	// ModelClass ���� ���̴����� �ڽ��� �ؽ�ó �ڿ��� �����ϰ� �׸��� ���� GetTexture �Լ��� ���� �ִ�.
	ID3D11ShaderResourceView* GetTexture();

private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// ���� �ؽ�ó�� �ҷ����� ��ȯ�ϴ� �� ����� LoadTexture �Լ��� ReleaseTexture �Լ��� ������ �ִ�. 
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private :
	// �Ʒ� ��� �������� ��������, �ε��� ����, �� ������ ũ�� ������ ������ �ִ� ���۴�.
	ID3D11Buffer* m_vertexBuffer, * m_instanceBuffer;
	int m_vertexCount, m_instanceCount;
	TextureClass* m_Texture;
};

#endif