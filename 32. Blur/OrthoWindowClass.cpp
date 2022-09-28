#include "OrthoWindowClass.h"

OrthoWindowClass::OrthoWindowClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

OrthoWindowClass::OrthoWindowClass(const OrthoWindowClass& other) {}

OrthoWindowClass::~OrthoWindowClass() {}

bool OrthoWindowClass::Initialize(ID3D11Device* device, int windowWidth, int windowHeight) {
	bool result;

	// 정점버퍼와 인덱스버퍼를 초기화한다.
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

// Render함수는 생성된 정점 / 인덱스 버퍼를 어셈블리 버퍼로 활성화한다.
void OrthoWindowClass::Render(ID3D11DeviceContext* deviceContext) {

	// 초기화된 정점 / 인덱스 버퍼를 그래픽 파이프라인에 넣는다.
	RenderBuffers(deviceContext);

	return;
}

int OrthoWindowClass::GetIndexCount() {
	return m_indexCount;
}

// InitializeBuffers 함수는 정점 / 인덱스 배열을 만든뒤 정점버퍼를 만든다.
// 이때 정점버퍼는 동적 정점버퍼로 값은 채우지읺고 할당만 해준다.
bool OrthoWindowClass::InitializeBuffers(ID3D11Device* device, int windowWidth, int windowHeight){
	float left, right, top, bottom;
	VertexType* vertices;
	unsigned long* indices; 
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData, indexData; 
	HRESULT result; 
	int i;
	
	// 모든 2D 렌더링에서는 2D 윈도우의 왼쪽, 오른쪽, 위, 아래 좌표가 필요하기 때문에 퐈면의 크기를 이용한 2D 윈도우 좌표를 계산한다. 참고로 정중앙 좌표는 (0,0)
	// 윈도우의 왼쪽 화면 좌표를 계산
	left = (float)((windowWidth / 2) * -1); 
	
	// 윈도우의 오른쪽 화면 좌표를 계산
	right = left + (float)windowWidth; 
	
	// 윈도우의 위쪽 화면 좌표를 계산
	top = (float)(windowHeight / 2); 
	
	// 윈도우의 아래쪽 화면 좌표를 계산
	bottom = top - (float)windowHeight;

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

	// 첫 번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 인덱스 배열 초기화 
	for (i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}

	// 정점버퍼의 description 작성
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void OrthoWindowClass::ShutdownBuffers() {
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


// 만들어진 정점/인덱스 버퍼를 어셈블리 버퍼로 활성화시켜 그래픽 파이프라인에 집어넣는다.
void OrthoWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
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

