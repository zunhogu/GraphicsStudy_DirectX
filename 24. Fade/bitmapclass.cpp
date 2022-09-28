#include "bitmapclass.h"

BitmapClass::BitmapClass(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

BitmapClass::BitmapClass(const BitmapClass& other){}

BitmapClass::~BitmapClass(){}

// Initialize �Լ��� ȭ��ũ��� �̹��� ũ�⸦ �����Ѵ�. ������ �� ��Ȯ�� ���� ��ġ�� �ʿ��ϱ� �����̴�.
bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight){
	bool result;

	// ȭ���� ����� �Է¹޴´�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// �̹����� ����� �Է¹޴´�.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// ������ ��ġ ������ -1�� �ʱ�ȭ�Ѵ�. �� ������ ���� ��ġ�� ����صδ� �뵵��
	// ���� ���� �����Ӱ� ���Ͽ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʱ� ������ ��������� ���� �� �ִ�.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// ���۸� �����Ѵ�.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown(){
	// ���۸� ��ȯ�Ѵ�.
	ShutdownBuffers();

	return;
}

// Render �Լ��� �Է����� ȭ�鿡 �׷��� ��ġ�� �޴´�. �̴� UpdateBuffers �Լ��� ��ġ�� �����ϴµ� ����Ѵ�.
// ���� �����Ӱ� ��ġ�� �ٱ͸� ���� ���� ������ �������� ��ġ�� ���ο� ��ġ�� �����Ѵ�. �׷��� ������ UpdateBuffer �Լ��� ����ģ��.
// RenderBuffers �Լ��� �����ѵ� ����/�ε��� ���۸� ����� ���۷� ��ȯ�Ѵ�.
bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY){
	bool result;

	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	RenderBuffers(deviceContext);

	return true;
}

// GetIndexCount �Լ��� 2D �̹����� ���� ���� �����Ѵ�. ������ 6�� ��ȯ�Ѵ�.
int BitmapClass::GetIndexCount(){
	return m_indexCount;
}

// InitializeBuffers �Լ��� 2D �̹����� �׸��µ� ����ϴ� ����/�ε��� ���۸� ����µ� ����Ѵ�. 
bool BitmapClass::InitializeBuffers(ID3D11Device* device){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// �ΰ��� �ﰢ���� ����� ���ؼ��� ���� 6���� �ʿ��ϴ�.
	m_vertexCount = 6;

	// ���������� ���� �ε����� ������ �ʱ�ȭ�Ѵ�.
	m_indexCount = m_vertexCount;

	// �����迭 ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}

	// �ε��� �迭����
	indices = new unsigned long[m_indexCount];
	if (!indices){
		return false;
	}

	// �����迭�� ũ�⸸ŭ �޸𸮰��� �����Ѵ�. �� ������ �ʱ�ȭ��Ű���ʰ� �������۸� ��������ؼ��� �޸𸮰����� �Ҵ�Ǿ���Ѵ�. 
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭 �ʱ�ȭ
	for (i = 0; i < m_indexCount; i++){
		indices[i] = i;
	}

	// ������� ModelClass�� ū ���̰� �����.
	// �������� description���� ���� �������۸� �����Ͽ� �ʿ��ϴٸ� ���� ���� ���ΰ��� ������ �� �ְ� �Ѵ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;   // �����̶�� �Ӽ� �ο��� ���� ������ ���� �����Ѵ�.
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   // �� ������ ������ ���� ������ ���� Flag�� �����Ѵ�. 
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �����Ѵ�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �ε��� ������ ������ ������ ��ġ�� �ٲ���� ������ �������� ������ ����Ű�� ������ �ε������۴� ���� ���۷� ������ �ʴ´�.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)){
		return false;
	}

	// �ε��������� description �ۼ����ش�.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �����Ѵ�.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����Ѵ�.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)){
		return false;
	}

	// ���� ���ۿ� �ε��� ���۸� ���� �Ŀ��� �̹� ���� ����Ǿ��� ������ �����迭�� �ε��� �迭�� �����Ѵ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

// ShutdownBuffers �Լ��� �ܼ��� InitializeBuffers �Լ����� ������� �������ۿ� �ε������۸� �ʱ�ȭ�Ѵ�.
void BitmapClass::ShutdownBuffers(){
	// Release the index buffer.
	if (m_indexBuffer){
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer){
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

// UpdateBuffers �Լ��� �� �����Ӹ��� �ҷ� �ʿ��� ��� ���� ���������� ������ �� ��ġ�� �����Ѵ�.
bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY){
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// �̹��� ��ġ�� ������ ���Ͽ� �޶������� Ȯ���Ѵ�. �ٲ��� �ʾҴٸ� ���۸� ������ �ʿ䰡 �����Ƿ� �׳� ����������.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY)){
		return true;
	}

	// ���� �̹��� ��ġ�� �ٲ���ٸ� �������� �׷��� ��ġ�� ���� ���ο� ��ġ�� ����Ѵ�.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// �̹����� �׺��� ��ġ�� ���ȴ�. 

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// ��ǥ�� ���Ǹ� �ӽ÷� �����迭�� ����� ���ο� ���� ��ġ�� ä���ִ´�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}

	// ù��° �ﰢ��
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// �ι�° �ﰢ��
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// ���������� ������ ���� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// ���������� �����Ϳ� ���� �����͸� �����´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// �ӽ� �����迭�� ������ �޸𸮿� �̸� �Ҵ�� �������ۿ� �����Ѵ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// ���� ���۸� ��ٴ�.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �����迭�� ��ȯ�Ѵ�.
	delete[] vertices;
	vertices = 0;

	return true;
}

// RenderBuffers �Լ��� ������ ModelClass ó�� ����/�ε��� ���۸� gpu ����� ���۷� ��ȯ��Ų��.
void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext){
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}