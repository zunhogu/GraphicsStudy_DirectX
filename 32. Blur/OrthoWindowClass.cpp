#include "OrthoWindowClass.h"

OrthoWindowClass::OrthoWindowClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

OrthoWindowClass::OrthoWindowClass(const OrthoWindowClass& other) {}

OrthoWindowClass::~OrthoWindowClass() {}

bool OrthoWindowClass::Initialize(ID3D11Device* device, int windowWidth, int windowHeight) {
	bool result;

	// �������ۿ� �ε������۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device, windowWidth, windowHeight);
	if (!result) { 
		return false;
	}
	
	return true;

}

void OrthoWindowClass::Shutdown() {
	ShutdownBuffers();
	
	return;
}

// Render�Լ��� ������ ���� / �ε��� ���۸� ����� ���۷� Ȱ��ȭ�Ѵ�.
void OrthoWindowClass::Render(ID3D11DeviceContext* deviceContext) {

	// �ʱ�ȭ�� ���� / �ε��� ���۸� �׷��� ���������ο� �ִ´�.
	RenderBuffers(deviceContext);

	return;
}

int OrthoWindowClass::GetIndexCount() {
	return m_indexCount;
}

// InitializeBuffers �Լ��� ���� / �ε��� �迭�� ����� �������۸� �����.
// �̶� �������۴� ���� �������۷� ���� ä�����ݰ� �Ҵ縸 ���ش�.
bool OrthoWindowClass::InitializeBuffers(ID3D11Device* device, int windowWidth, int windowHeight){
	float left, right, top, bottom;
	VertexType* vertices;
	unsigned long* indices; 
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData, indexData; 
	HRESULT result; 
	int i;
	
	// ��� 2D ������������ 2D �������� ����, ������, ��, �Ʒ� ��ǥ�� �ʿ��ϱ� ������ ǡ���� ũ�⸦ �̿��� 2D ������ ��ǥ�� ����Ѵ�. ����� ���߾� ��ǥ�� (0,0)
	// �������� ���� ȭ�� ��ǥ�� ���
	left = (float)((windowWidth / 2) * -1); 
	
	// �������� ������ ȭ�� ��ǥ�� ���
	right = left + (float)windowWidth; 
	
	// �������� ���� ȭ�� ��ǥ�� ���
	top = (float)(windowHeight / 2); 
	
	// �������� �Ʒ��� ȭ�� ��ǥ�� ���
	bottom = top - (float)windowHeight;

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

	// ù ��° �ﰢ��
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// �� ��° �ﰢ��
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// �ε��� �迭 �ʱ�ȭ 
	for (i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}

	// ���������� description �ۼ�
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void OrthoWindowClass::ShutdownBuffers() {
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


// ������� ����/�ε��� ���۸� ����� ���۷� Ȱ��ȭ���� �׷��� ���������ο� ����ִ´�.
void OrthoWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
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

