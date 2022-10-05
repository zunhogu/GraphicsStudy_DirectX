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
	// QuadTreeClass�� ���� Ʈ���� ���(���� ����)�� �� Ŭ������ TerrainClass�� ������ �޾� �ü� �ֵ��� ����������h��. 
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	// ���� Ʈ���� ���(���� ����)�� ������ �ִ� ������ ��� ����ü��.
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

	// �θ� ����κ��� �������� �������� ��� ���� �迭
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};

