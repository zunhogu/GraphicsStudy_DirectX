#include "debugwindowclass.h"

DebugWindowClass::DebugWindowClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

DebugWindowClass::DebugWindowClass(const DebugWindowClass& other) {}

DebugWindowClass::~DebugWindowClass() {}

bool DebugWindowClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight) {
	bool result;

	// 스크린의 사이즈를 저장한다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 비트맵 사이즈를 저장한다.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 정점버퍼와 인덱스버퍼를 초기화한다.
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

// Render함수는 생성된 정점 / 인덱스 버퍼를 어셈블리 버퍼로 활성화한다.
bool DebugWindowClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY) {
	bool result;

	// 동적 정점버퍼에 값을 채운다.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result) {
		return false;
	}

	// 초기화된 정점 / 인덱스 버퍼를 그래픽 파이프라인에 넣는다.
	RenderBuffers(deviceContext);
	
	return true;
}

int DebugWindowClass::GetIndexCount() {
	return m_indexCount;
}

// InitializeBuffers 함수는 정점 / 인덱스 배열을 만든뒤 정점버퍼를 만든다.
// 이때 정점버퍼는 동적 정점버퍼로 값은 채우지읺고 할당만 해준다.
bool DebugWindowClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;
	unsigned long* indices; 
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData, indexData; 
	HRESULT result; int i;

	// 정점 / 인덱스 배열의 사이즈를 설정
	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	// 정점/인덱스 배열 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// 정점 배열을 0으로 초기화하여 공간만 할당
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열 초기화 
	for (i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}

	// 정점버퍼의 description 작성
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점을 가르키는 보조리소스 구조체 작성
	vertexData.pSysMem = vertices; 
	vertexData.SysMemPitch = 0; 
	vertexData.SysMemSlicePitch = 0;

	// 정점버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); 
	if (FAILED(result)) { 
		return false; 
	}

	// 인덱스 버퍼의 description 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	indexBufferDesc.CPUAccessFlags = 0; 
	indexBufferDesc.MiscFlags = 0; 
	indexBufferDesc.StructureByteStride = 0;
	
	// 인덱스를 가르키는 보조리소스 구조체 작성
	indexData.pSysMem = indices; 
	indexData.SysMemPitch = 0; 
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// 정점/인덱스 배열 반환
	delete [] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void DebugWindowClass::ShutdownBuffers() {
	// 인덱스 버퍼 반환
	if(m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0; 
	} 
	// 정점 버퍼 반환
	if(m_vertexBuffer) {
		m_vertexBuffer->Release(); 
		m_vertexBuffer = 0; 
	} 
	return; 
}

// UpdateBuffers 함수는 생성된 정점버퍼를 동적으로 초기화하는 함수다.
bool DebugWindowClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY) {
	float left, right, top, bottom; 
	VertexType* vertices; 
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	VertexType* verticesPtr;
	HRESULT result;

	// 만약 현재 렌더링하고자 하는 위치가 변하지 않았다면 이 함수는 건너뛴다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY)) { 
		return true; 
	}

	// 만약 렌더링 위치가 변경되었다면 현재 위치를 기억해준다.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 화면의 크기와 렌더링 위치를 가지고 정점의 위치를 계산해준다.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX; 
	
	// Calculate the screen coordinates of the right side of the bitmap. 
	right = left + (float)m_bitmapWidth; 
	
	// Calculate the screen coordinates of the top of the bitmap. 
	top = (float)(m_screenHeight / 2) - (float)positionY; 
	
	// Calculate the screen coordinates of the bottom of the bitmap. 
	bottom = top - (float)m_bitmapHeight;

	// 임시 정점 배열을 만들어준다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// 정점배열을 초기화한다.
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

	// 이미 만들어진 정점버퍼에 쓰기위해 버퍼를 잠근다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	if (FAILED(result)) { 
		return false; 
	}

	// 정점버퍼의 포인터를 가져온다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 정점버퍼로 정점배열의 값을 복사한다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점버퍼를 잠근다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 정점배열을 반환한다.
	delete[] vertices;
	vertices = 0;

	return true;
}

// 만들어진 정점/인덱스 버퍼를 어셈블리 버퍼로 활성화시켜 그래픽 파이프라인에 집어넣는다.
void DebugWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// 정점 버퍼 활성화
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 인덱스 버퍼 활성화 
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점버퍼를 렌더링하기위한 기본 타입 설정 (삼각형)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

