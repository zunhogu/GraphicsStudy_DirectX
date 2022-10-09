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
const int TEXTURE_REPEAT = 8;

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
		XMFLOAT4 texture;
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
	};

	// lighting 계산을 위한 구조체
	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	ID3D11ShaderResourceView* GetDetailMapTexture();
private:

	bool LoadHeightMap(char* path);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();

	bool LoadColorMap(char*);

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
private:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	HeightMapType* m_heightMap;
	TextureClass* m_Texture, * m_DetailTexture;
};

#endif