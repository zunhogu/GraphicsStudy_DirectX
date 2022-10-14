////////////////////////////////////////////////////////////////////////////////
// Filename: foliageclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FOLIAGECLASS_H_
#define _FOLIAGECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <time.h>   
using namespace DirectX;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FoliageClass
////////////////////////////////////////////////////////////////////////////////
class FoliageClass
{
private:
	// Foliage �𵨿��� Foliage�� ���� ��ġ�� uv ��ǥ���� ������ �ִ�. �ٸ� ��� ������ �ν��Ͻ� ���ۿ� ��ġ�Ѵ�.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	// �迭�� ����� Foliage�� �Ӽ��� �����ϴµ� ���ȴ�. ����� x �� z, ��ġ�� ������ ���Եȴ�.
	struct FoliageType
	{
		float x, z;
		float r, g, b;
	};

	// ��ġ, ȸ�� �� �ܵ� ���� ���� ���� �ν��Ͻ�ȭ�ȴ�. ��ġ�� �ΰ��� ȸ���� ���� ��Ŀ� ����ȴ�.
	struct InstanceType
	{
		XMMATRIX matrix;
		XMFLOAT3 color;
	};

public:
	FoliageClass();
	FoliageClass(const FoliageClass&);
	~FoliageClass();

	bool Initialize(ID3D11Device*, WCHAR*, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool Frame(XMFLOAT3, ID3D11DeviceContext*);

	int GetVertexCount();
	int GetInstanceCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool GeneratePositions();

private:
	int m_foliageCount;
	FoliageType* m_foliageArray;  // �� ��ǳ ������ ��ġ�� ������ �����Ѵ�. ���⿡ �����͸� ����Ͽ� �ν��ͽ� �迭�� �����Ѵ�.
	InstanceType* m_Instances;  // m_foliageArray�� �� ��ǳ ������ ���� ��� ��İ� ������ �����Ѵ�. ������Ʈ�� ���� �������� ���� �ν��ͽ� ���ۿ� �����Ѵ�.
	ID3D11Buffer* m_vertexBuffer, * m_instanceBuffer;  // ������ �ν��Ͻ� ���۸� ����Ͽ� ��ǳ�� �������Ѵ�. �ν��Ͻ� ���۸� ����� ���� �ε��� ���۰� �ʿ����� �ʴ�.
	int m_vertexCount, m_instanceCount;  
	TextureClass* m_Texture;

	// �ٶ��� ����� ȸ���� �����ϱ� ���� ����
	float m_windRotation;
	int m_windDirection;
};

#endif