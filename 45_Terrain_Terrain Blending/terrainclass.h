////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <cstdio>

#include "textureclass.h"
#include "terrainshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
	
	// gird model에서 가져오는 정점정보
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT4 color;
	};

	// height map에서 가져오는 높이 정보
	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float r, g, b;

		// 높이맵 버퍼에는 다음과 같은 rgb index 값이 들어오는데 이는 terrain의 polygon에서 어떤 Material을 사용할지 정해주는 인덱스다.
		// 실제로 해당 값은 Material Group의 red, green, blue index와 매칭 될것이다.
		int rIndex, gIndex, bIndex;
	};

	// lighting 계산을 위한 구조체
	struct VectorType
	{
		float x, y, z;
	};

	// 해당 구조체는 Material Group을 나타낸다.
	struct MaterialGroupType
	{
		int textureIndex1, textureIndex2, alphaIndex;  // 사용한 texture array index
		int red, green, blue;   // 높이맵과 매칭될 MaterialMap의 rgb index 값
		ID3D11Buffer* vertexBuffer, * indexBuffer;   // 쿼드 내의 정점 / 인덱스 버퍼
		int vertexCount, indexCount; // 쿼드 내의 정점 / 인덱스 버퍼
		VertexType* vertices;   // 쿼드 내 정점 / 인덱스 카운트
		unsigned long* indices; // 쿼드 내 정점 / 인덱스 배열
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*, char*, char*, char*);
	void Shutdown();

	bool Render(ID3D11DeviceContext*, Terrainshaderclass*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT3);

private:

	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	bool LoadColorMap(char*);
	
	bool LoadMaterialFile(char*, char*, ID3D11Device*);
	bool LoadMaterialMap(char*);
	bool LoadMaterialBuffers(ID3D11Device*);
	void ReleaseMaterials();

private:
	int m_terrainWidth, m_terrainHeight;

	HeightMapType* m_heightMap;
	TextureClass* m_Texture;
	int m_textureCount, m_materialCount;
	TextureClass* m_Textures;
	MaterialGroupType* m_Materials;
};

#endif