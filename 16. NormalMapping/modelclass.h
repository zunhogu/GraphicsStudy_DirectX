// ModelClass�� 3D �𵨵��� ������ �������� ĸ��ȭ�ϴ� Ŭ������.
// �� �ڵ忡���� ������ ���� �ﰢ���� ������
// �� �ﰢ���� ȭ�鿡 �׷����� ���� �������ۿ� ���ι��۵� ������.

#ifndef _MODELCLASS_H_ 
#define _MODELCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

// �� Ŭ���������� �ؽ�ó �ڿ��� �����ؾ��Ѵ�. �� ������ ���۸� ���鶧 �� �Է°��� �غ��ϴ� �������� ���̴��� uv��ǥ�� ���ϱ⶧���̴�.
#include "texturearrayclass.h"

class ModelClass {
private :
	// �������ۿ� �� ���������� �������ش�.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		// ���� �𵨿��� ������ �������� ���� ������ ������� �ʴ�.
		// ���� ���� ������༭ �־���Ѵ�.
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};
	// �� ������ ǥ���ϱ� ���� ���ο� ����ü
	struct ModelType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};
public :
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// �Ʒ� �Լ����� 3D ���� �������ۿ� �ε��� ���۵��� �ʱ�ȭ�� ���� ������ �����Ѵ�.
	// Render �Լ��� GPU�� �𵨵��� ���������� �ְ� �÷� ���̴��� �׸� �غ� �Ѵ�.

	bool Initialize(ID3D11Device*, char*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	// ModelClass ���� ���̴����� �ڽ��� �ؽ�ó �ڿ��� �����ϰ� �׸��� ���� GetTexture �Լ��� ���� �ִ�.
	ID3D11ShaderResourceView** GetTextureArray();

private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// ���� �ؽ�ó�� �ҷ����� ��ȯ�ϴ� �� ����� LoadTexture �Լ��� ReleaseTexture �Լ��� ������ �ִ�. 
	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTextures();

	// �� �����͸� �о���̰� �����ϴ� ������ �ϴ� �Լ��� �߰��Ѵ�.
	bool LoadModel(char*);
	void ReleaseModel();

	// ������ ���������� �ִٸ� �� ������ ������ ������ �������� ���ؾ��Ѵ�. 
	// �̴� �ε��Ҽ��� ������ ���� ���̴����� �ϴ°��� ���� �ʴ�. ���� ���� �ҷ��ö� �Ʒ��� �Լ��� �̿��ؼ� ����Ѵ�.
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);
private :
	// �Ʒ� ��� �������� ��������, �ε��� ����, �� ������ ũ�� ������ ������ �ִ� ���۴�.
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureArrayClass* m_TextureArray;

	// �� �����͸� ���� �� �������ۿ� �������� ������ �����͸� �����ϴ� ������ �����Ѵ�.
	ModelType* m_model;
};

#endif