#pragma once

/////////////
// GLOBALS //
/////////////
const int MAX_TRIANGLES = 10000;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "terrainclass.h"
#include "FrustumClass.h"
#include "Terrainshaderclass.h"

class QuadTreeClass
{
private:
	// QuadTreeClass는 쿼드 트리의 노드(서브 쿼드)가 될 클래스로 TerrainClass의 정보를 받아 올수 있도록 만들어져야햔다. 
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	// 쿼드 트리의 노드(서브 쿼드)가 가지고 있는 정보를 담는 구조체다.
	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		NodeType* nodes[4];
	};

public:
	QuadTreeClass();
	QuadTreeClass(const QuadTreeClass&);
	~QuadTreeClass();

	bool Initialize(TerrainClass*, ID3D11Device*);
	void Shutdown();
	void Render(FrustumClass*, ID3D11DeviceContext*, Terrainshaderclass*);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, FrustumClass*, ID3D11DeviceContext*, Terrainshaderclass*);

private:
	int m_triangleCount, m_drawCount;

	// 부모 쿼드로부터 물려받을 정점들을 담기 위한 배열
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};

