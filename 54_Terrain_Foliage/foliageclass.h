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
	// Foliage 모델에는 Foliage에 대한 위치와 uv 좌표값만 가지고 있다. 다른 모든 정보는 인스턴스 버퍼에 위치한다.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	// 배열에 저장될 Foliage의 속성을 정의하는데 사용된다. 현재는 x 및 z, 위치와 색상을 갖게된다.
	struct FoliageType
	{
		float x, z;
		float r, g, b;
	};

	// 위치, 회전 및 잔디 쿼드 색상에 대해 인스턴스화된다. 위치와 두개의 회전은 단일 행렬에 저장된다.
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
	FoliageType* m_foliageArray;  // 각 단풍 조각의 위치와 색상을 저장한다. 여기에 데이터를 사용하여 인스터스 배열을 빌드한다.
	InstanceType* m_Instances;  // m_foliageArray에 각 단풍 조각에 대한 모든 행렬과 색상을 저장한다. 업데이트한 다음 렌더링을 위해 인스터스 버퍼에 복사한다.
	ID3D11Buffer* m_vertexBuffer, * m_instanceBuffer;  // 정점과 인스턴스 버퍼를 사용하여 단풍을 렌더링한다. 인스턴스 버퍼를 사용할 때는 인덱스 버퍼가 필요하지 않다.
	int m_vertexCount, m_instanceCount;  
	TextureClass* m_Texture;

	// 바람의 방향과 회전을 저장하기 위한 변수
	float m_windRotation;
	int m_windDirection;
};

#endif