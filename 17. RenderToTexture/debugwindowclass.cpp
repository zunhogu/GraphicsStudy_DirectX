#include "debugwindowclass.h"

DebugWindowClass::DebugWindowClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

DebugWindowClass::DebugWindowClass(const DebugWindowClass& other) {}

DebugWindowClass::~DebugWindowClass() {}

bool DebugWindowClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight) {
	bool result;

	// ��ũ���� ����� �����Ѵ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ��Ʈ�� ����� �����Ѵ�.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// �������ۿ� �ε������۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device); 
	if (!result) { 
		return false;
	}
	
	return true;

}

void DebugWindowClass::Shutdown() {
	ShutdownBuffers();
	
	return;
}

// Render�Լ��� ������ ���� / �ε��� ���۸� ����� ���۷� Ȱ��ȭ�Ѵ�.
bool DebugWindowClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY) {
	bool result;

	// ���� �������ۿ� ���� ä���.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result) {
		return false;
	}

	// �ʱ�ȭ�� ���� / �ε��� ���۸� �׷��� ���������ο� �ִ´�.
	RenderBuffers(deviceContext);
	
	return true;
}

int DebugWindowClass::GetIndexCount() {
	return m_indexCount;
}

// InitializeBuffers �Լ��� ���� / �ε��� �迭�� ����� �������۸� �����.
// �̶� �������۴� ���� �������۷� ���� ä�����ݰ� �Ҵ縸 ���ش�.
bool DebugWindowClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;
	unsigned long* indices; 
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData, indexData; 
	HRESULT result; int i;

	// ���� / �ε��� �迭�� ����� ����
	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	// ����/�ε��� �迭 ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// ���� �迭�� 0���� �ʱ�ȭ�Ͽ� ������ �Ҵ�
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭 �ʱ�ȭ 
	for (i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}

	// ���������� description �ۼ�
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ������ ����Ű�� �������ҽ� ����ü �ۼ�
	vertexData.pSysMem = vertices; 
	vertexData.SysMemPitch = 0; 
	vertexData.SysMemSlicePitch = 0;

	// �������� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); 
	if (FAILED(result)) { 
		return false; 
	}

	// �ε��� ������ description �ۼ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	indexBufferDesc.CPUAccessFlags = 0; 
	indexBufferDesc.MiscFlags = 0; 
	indexBufferDesc.StructureByteStride = 0;
	
	// �ε����� ����Ű�� �������ҽ� ����ü �ۼ�
	indexData.pSysMem = indices; 
	indexData.SysMemPitch = 0; 
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// ����/�ε��� �迭 ��ȯ
	delete [] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void DebugWindowClass::ShutdownBuffers() {
	// �ε��� ���� ��ȯ
	if(m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0; 
	} 
	// ���� ���� ��ȯ
	if(m_vertexBuffer) {
		m_vertexBuffer->Release(); 
		m_vertexBuffer = 0; 
	} 
	return; 
}

// UpdateBuffers �Լ��� ������ �������۸� �������� �ʱ�ȭ�ϴ� �Լ���.
bool DebugWindowClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY) {
	float left, right, top, bottom; 
	VertexType* vertices; 
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	VertexType* verticesPtr;
	HRESULT result;

	// ���� ���� �������ϰ��� �ϴ� ��ġ�� ������ �ʾҴٸ� �� �Լ��� �ǳʶڴ�.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY)) { 
		return true; 
	}

	// ���� ������ ��ġ�� ����Ǿ��ٸ� ���� ��ġ�� ������ش�.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// ȭ���� ũ��� ������ ��ġ�� ������ ������ ��ġ�� ������ش�.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX; 
	
	// Calculate the screen coordinates of the right side of the bitmap. 
	right = left + (float)m_bitmapWidth; 
	
	// Calculate the screen coordinates of the top of the bitmap. 
	top = (float)(m_screenHeight / 2) - (float)positionY; 
	
	// Calculate the screen coordinates of the bottom of the bitmap. 
	bottom = top - (float)m_bitmapHeight;

	// �ӽ� ���� �迭�� ������ش�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// �����迭�� �ʱ�ȭ�Ѵ�.
	// First triangle. 
	vertices[0].position = XMFLOAT3(left, top, 0.0f);	// Top left. 
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f); // Bottom right. 
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	
	vertices[2].position = XMFLOAT3(left, bottom, 0.0f); // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// Second triangle. 
	vertices[3].position = XMFLOAT3(left, top, 0.0f); // Top left. 
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);
	
	vertices[4].position = XMFLOAT3(right, top, 0.0f); // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	
	vertices[5].position = XMFLOAT3(right, bottom, 0.0f); // Bottom right. 
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// �̹� ������� �������ۿ� �������� ���۸� ��ٴ�.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	if (FAILED(result)) { 
		return false; 
	}

	// ���������� �����͸� �����´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// �������۷� �����迭�� ���� �����Ѵ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// �������۸� ��ٴ�.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �����迭�� ��ȯ�Ѵ�.
	delete[] vertices;
	vertices = 0;

	return true;
}

// ������� ����/�ε��� ���۸� ����� ���۷� Ȱ��ȭ���� �׷��� ���������ο� ����ִ´�.
void DebugWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// ���� ���� Ȱ��ȭ
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �ε��� ���� Ȱ��ȭ 
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �������۸� �������ϱ����� �⺻ Ÿ�� ���� (�ﰢ��)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

