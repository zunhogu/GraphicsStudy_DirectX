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
	
	// gird model���� �������� ��������
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT4 color;
	};

	// height map���� �������� ���� ����
	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float r, g, b;

		// ���̸� ���ۿ��� ������ ���� rgb index ���� �����µ� �̴� terrain�� polygon���� � Material�� ������� �����ִ� �ε�����.
		// ������ �ش� ���� Material Group�� red, green, blue index�� ��Ī �ɰ��̴�.
		int rIndex, gIndex, bIndex;
	};

	// lighting ����� ���� ����ü
	struct VectorType
	{
		float x, y, z;
	};

	// �ش� ����ü�� Material Group�� ��Ÿ����.
	struct MaterialGroupType
	{
		int textureIndex1, textureIndex2, alphaIndex;  // ����� texture array index
		int red, green, blue;   // ���̸ʰ� ��Ī�� MaterialMap�� rgb index ��
		ID3D11Buffer* vertexBuffer, * indexBuffer;   // ���� ���� ���� / �ε��� ����
		int vertexCount, indexCount; // ���� ���� ���� / �ε��� ����
		VertexType* vertices;   // ���� �� ���� / �ε��� ī��Ʈ
		unsigned long* indices; // ���� �� ���� / �ε��� �迭
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