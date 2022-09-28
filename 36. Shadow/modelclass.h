// ModelClass�� 3D �𵨵��� ������ �������� ĸ��ȭ�ϴ� Ŭ������.
// �� �ڵ忡���� ������ ���� �ﰢ���� ������
// �� �ﰢ���� ȭ�鿡 �׷����� ���� �������ۿ� ���ι��۵� ������.

#ifndef _MODELCLASS_H_ 
#define _MODELCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;
#include <fstream>
using namespace std;

// �� Ŭ���������� �ؽ�ó �ڿ��� �����ؾ��Ѵ�. �� ������ ���۸� ���鶧 �� �Է°��� �غ��ϴ� �������� ���̴��� uv��ǥ�� ���ϱ⶧���̴�.
#include "textureclass.h"

class ModelClass {
private :
	// �������ۿ� �� ���������� �������ش�.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
	struct ModelType{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
public :
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// �Ʒ� �Լ����� 3D ���� �������ۿ� �ε��� ���۵��� �ʱ�ȭ�� ���� ������ �����Ѵ�.
	// Render �Լ��� GPU�� �𵨵��� ���������� �ְ� �÷� ���̴��� �׸� �غ� �Ѵ�.

	bool Initialize(ID3D11Device*,char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	// ModelClass ���� ���̴����� �ڽ��� �ؽ�ó �ڿ��� �����ϰ� �׸��� ���� GetTexture �Լ��� ���� �ִ�.
	ID3D11ShaderResourceView* GetTexture();

	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);

private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// ���� �ؽ�ó�� �ҷ����� ��ȯ�ϴ� �� ����� LoadTexture �Լ��� ReleaseTexture �Լ��� ������ �ִ�. 
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();
private :
	// �Ʒ� ��� �������� ��������, �ε��� ����, �� ������ ũ�� ������ ������ �ִ� ���۴�.
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	TextureClass* m_Texture;

	float m_positionX, m_positionY, m_positionZ;
};

#endif