#include "QuadTreeClass.h"

QuadTreeClass::QuadTreeClass()
{
	m_vertexList = 0;
	m_parentNode = 0;
}

QuadTreeClass::QuadTreeClass(const QuadTreeClass&)
{
}

QuadTreeClass::~QuadTreeClass()
{
}

bool QuadTreeClass::Initialize(TerrainClass* terrain, ID3D11Device* device)
{
	int vertexCount;
	float centerX, centerZ, width;

	// QuadTreeClass�� ���� ó���ؾ��� ���� TerrainClass�� ���� ������ ��� ���̴�. 

	// terrain���κ��� vertexCount�� ��´�.
	vertexCount = terrain->GetVertexCount();

	// vertexList�� �ʱ�ȭ �ϱ����ؼ� triangleCount�� ����Ѵ�.
	m_triangleCount = vertexCount / 3;

	// vertexList�� �ʱ�ȭ�Ѵ�.
	m_vertexList = new VertexType[vertexCount];
	if (!m_vertexList)
		return false;

	terrain->CopyVertexArray((void*)m_vertexList);

	// �ϴ� ���� ����� ���� ������ ä������ ������ �θ����� ������ ����ϴ� ���� �����ϰ�, ���� ���� Ʈ���� ����Լ��� ���� ������ �� �ֵ�. 
	
	// Calculate the center x,z and the width of the mesh.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// Create the parent node for the quad tree.
	m_parentNode = new NodeType;
	if (!m_parentNode)
		return false;

	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(m_parentNode, centerX, centerZ, width, device);

	// ���� Ʈ���� ����ǰ� ���� vertexList�� ���̻� �ʿ����� �ʱ� ������ Release �����ش�.
	if (m_vertexList)
	{
		delete[] m_vertexList;
		m_vertexList = 0;
	}

	return true;
}

void QuadTreeClass::Shutdown()
{
	// Recursively release the quad tree data.
	if (m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}

	return;
}

void QuadTreeClass::Render(FrustumClass* frustum, ID3D11DeviceContext* deviceContext, Terrainshaderclass* shader)
{
	// Render�Լ������� RenderNode�� ȣ���Ѵ�.
	// RenderNode�Լ��� FrustumClas�� ����Ͽ� ��带 �������Ѵ�. 

	// �������� ������ ���� ���� �����ӿ��� �׸� �ﰢ���� ������ 0���� �ʱ�ȭ �ؾ��ϴµ� �� ������ ������ ��忡 ���� ��� �����ϱ� �����̴�. 
	m_drawCount = 0;

	// Render each node that is visible starting at the parent node and moving down the tree.
	RenderNode(m_parentNode, frustum, deviceContext, shader);
}

// ���� ���� �Լ� ȣ�⿡�� �׷��� �ﰢ���� ���� ��ȯ�Ѵ�.
int QuadTreeClass::GetDrawCount()
{
	return m_drawCount;
}

// CalculateMeshDimensions()�� �������� �θ� ����� ���� ����� ����Ѵ�.
// �� vertexList�� �ִ� ��� ������ ����Ͽ� ������ �߽ɰ� ������ ���� �ִ� �ʺ� ����Ѵ�. �̰��� �θ����� ũ�⸦ ����� �� ���ȴ�.
void QuadTreeClass::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	int i;
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;

	// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;

	// Sum all the vertices in the mesh.
	for (i = 0; i < vertexCount; i++)
	{
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

	// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(m_vertexList[0].position.x - centerX);
	minDepth = fabsf(m_vertexList[0].position.z - centerZ);

	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for (i = 0; i < vertexCount; i++)
	{
		width = fabsf(m_vertexList[i].position.x - centerX);
		depth = fabsf(m_vertexList[i].position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// Calculate the maximum diameter of the mesh.
	meshWidth = max(maxX, maxZ) * 2.0f;

}

// CraeteTreeNode�� ��������� ȣ��Ǿ� ����Ʈ���� �����ϴ� �Լ���.
void QuadTreeClass::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
	int numTriangles, i, count, vertexCount, index, vertexIndex;
	float offsetX, offsetZ;
	VertexType* vertices;
	unsigned long* indices;
	bool result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Store the node position and size in world.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// Initialize the triangle count to zero for the node.
	node->triangleCount = 0;

	// Initialize the vertex and index buffer to null.
	node->vertexBuffer = 0;
	node->indexBuffer = 0;

	// Initialize the children nodes of this node to null.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// ���� ����� triangle Count�� ����Ѵ�. 
	numTriangles = CountTriangles(positionX, positionZ, width);

	// triangle�� Count�� ���� ���� ��尡 ��� ������� �����Ǵµ�

	// Case 1 : ��� �ȿ� triangle 0�� �϶� �׳� ��� �ϼ�
	if (numTriangles == 0)
		return;

	// Case 2 : ��� �ȿ� triangle�� MAX_TRIANGLES�� �Ѿ�� ���尡 split �Ǿ�� �Ҷ� 
	if (numTriangles > MAX_TRIANGLES)
	{
		for (i = 0; i < 4; i++)
		{
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// See if there are any triangles in the new node.
			count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// If there are triangles inside where this new node would be then create the child node.
				node->nodes[i] = new NodeType;

				// Extend the tree starting from this new child node now.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
			}
		}

		return;
	}

	// Case3 : MAX_TRIANGLES ������ triangle Count�� �����ٸ� ���� �� �ε��� ���۸� ����� ���� �ε��Ѵ�.
	// �ش� ���� ������忩�� �Ѵ�.
	node->triangleCount = numTriangles;

	// Calculate the number of vertices.
	vertexCount = numTriangles * 3;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];

	// Create the index array.
	indices = new unsigned long[vertexCount];

	// Initialize the index for this new vertex and index array.
	index = 0;

	// Go through all the triangles in the vertex list.
	for (i = 0; i < m_triangleCount; i++)
	{
		// If the triangle is inside this node then add it to the vertex array.
		result = IsTriangleContained(i, positionX, positionZ, width);
		if (result == true)
		{
			// Calculate the index into the terrain vertex list.
			vertexIndex = i * 3;

			// Get the three vertices of this triangle from the vertex list.
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			vertices[index].normal = m_vertexList[vertexIndex].normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			vertices[index].normal = m_vertexList[vertexIndex].normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			vertices[index].normal = m_vertexList[vertexIndex].normal;
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return;

}

// �ش� �Լ��� ���� ���� �ﰢ���� ���� ī��Ʈ�ϴ� �Լ���.
int QuadTreeClass::CountTriangles(float positionX, float positionZ, float width)
{
	int count, i;
	bool result;

	// Initialize the count to zero.
	count = 0;

	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for (i = 0; i < m_triangleCount; i++)
	{
		// If the triangle is inside the node then increment the count by one.
		result = IsTriangleContained(i, positionX, positionZ, width);
		if (result == true)
		{
			count++;
		}
	}

	return count;
}

// IsTriangleContained()�� CountTriangles()���� ȣ��Ǿ� �ﰢ���� ���� ���� �ִ��� true/false ���� ��ȯ�ϴ� �Լ���.
bool QuadTreeClass::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3;
	float minimumX, maximumX, minimumZ, maximumZ;


	// Calculate the radius of this node.
	radius = width / 2.0f;

	// Get the index into the vertex list.
	vertexIndex = index * 3;

	// Get the three vertices of this triangle from the vertex list.
	x1 = m_vertexList[vertexIndex].position.x;
	z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	x2 = m_vertexList[vertexIndex].position.x;
	z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	x3 = m_vertexList[vertexIndex].position.x;
	z3 = m_vertexList[vertexIndex].position.z;

	// Check to see if the minimum of the x coordinates of the triangle is inside the node.
	minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
	{
		return false;
	}

	// Check to see if the maximum of the x coordinates of the triangle is inside the node.
	maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
	{
		return false;
	}

	// Check to see if the minimum of the z coordinates of the triangle is inside the node.
	minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (positionZ + radius))
	{
		return false;
	}

	// Check to see if the maximum of the z coordinates of the triangle is inside the node.
	maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}

// ReleaseNode()�� ��������� ȣ��Ǿ� ���� Ʈ���� ��� ������ Release �ϴ� �Լ���.
void QuadTreeClass::ReleaseNode(NodeType* node)
{
	int i;


	// Recursively go down the tree and release the bottom nodes first.
	for (i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// Release the vertex buffer for this node.
	if (node->vertexBuffer)
	{
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}

	// Release the index buffer for this node.
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	// Release the four child nodes.
	for (i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}

	return;
}

// RenderNode() �� ���� ���� �ﰢ���� �������ϴ� �Լ���. frustum ��ü�� �޾� ����ڰ� �ش� ���带 �����ִ���, �����ִٸ� ���忡 ���� Shader ��ü�� �������� ���ָ� �ȴ�.
void QuadTreeClass::RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* deviceContext, Terrainshaderclass* shader)
{
	bool result;
	int count, i, indexCount;
	unsigned int stride, offset;

	// Frustum Check
	result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));
	if (!result)
		return;

	// ���� Frustum Check�� ��� ���� true�� ������ �ڽĳ��� �ش� �Լ��� ��������� ȣ���Ѵ�.
	count = 0;
	for (i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], frustum, deviceContext, shader);
		}
	}

	// �ڽ� ��尡 ���ٸ��� �������� �ﰢ���� ���̻� �������� �����Ƿ� �������� �ʾƵ� �ȴ�.
	if (count != 0)
		return;

	// �ش� ����� ���۸� ������ ���ش�.

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Determine the number of indices in this node.
	indexCount = node->triangleCount * 3;

	// Call the terrain shader to render the polygons in this node.
	shader->RenderShader(deviceContext, indexCount);

	// Increase the count of the number of polygons that have been rendered during this frame.
	m_drawCount += node->triangleCount;

	return;
}