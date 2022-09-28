#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_Texture = 0;
} 

ModelClass::ModelClass(const ModelClass& other) { } 

ModelClass::~ModelClass() { }

// Initialize 함수는 정점 버퍼와 인덱스 버퍼의 초기화 함수를 호출한다.
bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename) {
	bool result;

	// 정점 버퍼와 인덱스 버퍼를 초기화한다.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// 이 모델의 텍스처를 가져온다. 
	result = LoadTexture(device, textureFilename); 
	if(!result) { 
		return false; 
	}

	return true;
}

// Shutdown 함수는 정점 버퍼와 인덱스 버퍼를 정리하는 함수를 호출한다.
void ModelClass::Shutdown()
{
	// 텍스처를 반환한다.
	ReleaseTexture();

	// 정점 버퍼와 인덱스 버퍼를 해제합니다.
	ShutdownBuffers();

	return;
}

// Render 함수는 GraphicsClass::Render 함수에서 호출한다.
// 이 함수에서는 RenderBuffers 함수를 호출하여 정점 버퍼와 인덱스 버퍼를 그래픽 파이프라인에 넣어 컬러 셰이더가 이들을 그릴 수 있도록 한다.
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 한다.
	RenderBuffers(deviceContext);
	return;
}

// GetIndexCount 함수는 모델의 인덱스의 갯수를 알려준다.
// 컬러셰이더에서는 모델을 그리기 위해서는 이 정보가 필요하다.
int ModelClass::GetIndexCount() {
	return m_indexCount;
}

//GetTexture 함수는 모델 텍스쳐 자원을 반환한다.텍스쳐 셰이더가 모델을 그리기 위해서는 이 텍스쳐에 접근할 수 있어야 합니다.
ID3D11ShaderResourceView* ModelClass::GetTexture(){
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {

	// 정점 배열을 만들어주고, 모델에 있는 정점들을 정점배열에 넣고, 정점배열들로 정점 버퍼를 만들어준다.

	VertexType* vertices;   // 정점 배열
	unsigned long* indices;  // 인덱스 배열
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 정점 배열의 길이를 설정한다.
	m_vertexCount = 3;

	// 인덱스 배열의 길이를 설정한다.
	m_indexCount = 3;

	// 정점 배열을 생성한다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// 인덱스 배열을 생성한다.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// 정점 배열을 만들어주면 모델의 정점들을 배열에 넣어준다.
	// 여기서 주의할 점은 시계 방향으로 정점 값을 넣어야한다. 만약 반시계 방향으로 만들면 DirectX에서 이 삼각형을 반대편을 바라본다고 판단한다.
	// 이렇게 되면 레스터화기 단계에서 후면선별을 통해 그려지지 않게 된다.
	// 따라서 GPU에게 도형을 그리도록 할때 이 순서를 기억하게 하는 것이 중요하다.

	// 정점 배열은 색상 성분 대신 텍스처 성분을 가지게 된다. 
	// 텍스처 성분의 uv좌표값으로 도형 표면의 어디든 원하는 부분의 텍스처를 입힐 수 있다.
	// 현재는 최대한 간단하게 하기위해 직접 매핑을 한다. 

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);   // 왼쪽 아래 
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);   // 상단 가운데 
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);   // 오른쪽 아래 
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	// 인덱스 배열에도 정점 배열처럼 값을 넣어준다.
	indices[0] = 0;   // 왼쪽아래
	indices[1] = 1;   // 상단 간운데
	indices[2] = 2;   // 오른쪽 아래


	// 정점 배열과 인덱스 배열이 채워졌으므로 이것으로 정점 버퍼와 인덱스 버퍼를 만든다.
	// 버퍼를 초기화 하기위해서는 버퍼의 구조체 서술이 필요하다.
	// 그 다음은 버퍼의 초기화에 사용할 구조체 서술이 필요하다.

	// 정점 버퍼 구조체 description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;   // 정점버퍼가 쓰이는 용도 (현재는 GPU가 버퍼의 자원을 읽어야 하므로 이 용도로 지정)
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;   // 정점버퍼의 크기 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // 정점버퍼의 경우 다음과 같은 플래그를 지정한다. 
	vertexBufferDesc.CPUAccessFlags = 0;    // CPU가 플래그에 접근하는 방식을 지정한다.(버퍼 생성 이후 CPU가 버퍼에 접근하지 않으면 0으로 지정)
	vertexBufferDesc.MiscFlags = 0;    // 정점버퍼에 대해서는 이 기타 플래그를 사용할 필요가 없다.
	vertexBufferDesc.StructureByteStride = 0;   // 구조적 버퍼에 저장된 원소 구조적 버퍼 이외의 버퍼는 0으로 설정하면 된다. 

	// 정점 데이터를 가리키는 보조 리소스 구조체를 서술한다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성한다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}


	// 인덱스 버퍼도 정점버퍼와 똑같은 과정으로 진행한다.

	// 인덱스 버퍼 구조체를 서술한다.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT; 
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount; 
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	indexBufferDesc.CPUAccessFlags = 0; 
	indexBufferDesc.MiscFlags = 0; 
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 서술한다.
	indexData.pSysMem = indices; 
	indexData.SysMemPitch = 0; 
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성한다.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// 정점 버퍼와 인덱스 버퍼를 만든 후에는 이미 값이 복사되었기 때문에 정점배열과 인덱스 배열을 제거한다.

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

// ShutdownBuffers 함수는 단순히 InitializeBuffers 함수에서 만들었던 정점버퍼와 인덱스버퍼를 초기화한다.
void ModelClass::ShutdownBuffers() {
	
	// 인덱스 버퍼를 해제한다.
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼를 해제한다.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

// RenderBuffers 함수는 Render 함수에서 호출된다. 
// 이 함수의 목적은 정점 버퍼와 인덱스 버퍼를 GPU의 어셈블리의 버퍼로 활성화 시키는 것이다.
// 일단 GPU가 활성화된 정점버퍼를 가지게 되면 셰이더를 이용해서 버퍼의 내용을 그릴 수 있게 된다.
// 또한 이 함수에서는 이 정점을 삼각형이나 선분, 부채꼴 등 어떤 모양으로 그리게 될지 정의한다.
// 어셈블리의 입력에 정점버퍼와 인덱스 버퍼를 넣고 DirectX의  IASetPrimitiveTopology 함수를 이용해 GPU에게 이 정점들을 삼각형을 그리도록 주문한다. 
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
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

// LoadTexture 함수는 제공된 파일 이름으로 텍스쳐 개체를 만들고 초기화하는 함수다.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// 텍스처 객체를 생성한다.
	m_Texture = new TextureClass;
	if (!m_Texture){
		return false;
	}

	// 생성된 텍스처 객체를 초기화한다.
	result = m_Texture->Initialize(device, filename);
	if (!result){
		return false;
	}

	return true;
}

//ReleaseTexture 함수는 LoadTexture 에서 생성한 텍스쳐 객체를 반환한다.
void ModelClass::ReleaseTexture()
{
	// 텍스처 객체를 반환한다.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
