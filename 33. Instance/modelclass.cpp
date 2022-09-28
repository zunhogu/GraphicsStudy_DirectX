#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0; 
	m_instanceBuffer = 0;
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

int ModelClass::GetVertexCount() { 
	return m_vertexCount; 
}

int ModelClass::GetInstanceCount() {
	return m_instanceCount;
}

//GetTexture 함수는 모델 텍스쳐 자원을 반환한다.텍스쳐 셰이더가 모델을 그리기 위해서는 이 텍스쳐에 접근할 수 있어야 합니다.
ID3D11ShaderResourceView* ModelClass::GetTexture(){
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;   // 정점 배열
	InstanceType* instances;   // 인스턴스 배열
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;

	// 정점 배열의 길이를 설정한다.
	m_vertexCount = 3;

	// 정점 배열을 생성한다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// 인덱스 배열과 버퍼는 만들지 않는다.

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
	delete[] vertices;
	vertices = 0;

	// 인스턴스 개수를 초기화 
	m_instanceCount = 4;

	// 인스턴스 배열을 생성
	instances = new InstanceType[m_instanceCount];
	if (!instances) {
		return false;
	}
	
	// 인스턴스 배열 초기화
	instances[0].position = XMFLOAT3(-1.5f, -1.5f, 5.0f);
	instances[1].position = XMFLOAT3(-1.5f, 1.5f, 5.0f);
	instances[2].position = XMFLOAT3(1.5f, -1.5f, 5.0f);
	instances[3].position = XMFLOAT3(1.5f, 1.5f, 5.0f);

	// 인스턴스 버퍼 discription을 정점버퍼의 discription과 동일하게 작성
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// 인스턴스를 가르키는 리소스 구조체에 서술
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result)) {
		return false;
	}

	delete[] instances;
	instances = 0;

	return true;
}

// ShutdownBuffers 함수는 단순히 InitializeBuffers 함수에서 만들었던 정점버퍼와 인덱스버퍼를 초기화한다.
void ModelClass::ShutdownBuffers() {
	
	// 인덱스 버퍼를 해제한다.
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// 정점 버퍼를 해제한다.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

// RenderBuffers에서 버퍼들을 그래픽 파이프라인으로 전달하는데 인스턴스 버퍼는 단지 다른 정보를 담고 있는 두번째 정점 버퍼이기 때문에 정점 버퍼가 세팅되는 시점에 같이 세팅된다.
// 따라서 IASetVertexBuffers 함수를 호출할 때 스트라이드 오프셋, 버퍼를 하나만 보내는 것이 아니라 배열을 만들어 보낸다.
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int strides[2]; 
	unsigned int offsets[2]; 
	ID3D11Buffer* bufferPointers[2];

	// 버퍼의 스트라이드 설정 
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// 버퍼의 offset 설정
	offsets[0] = 0; 
	offsets[1] = 0;

	// 정점 버퍼와 인스턴스 버퍼의 포인터를 담고 있는 배열 만듬
	bufferPointers[0] = m_vertexBuffer; 
	bufferPointers[1] = m_instanceBuffer;

	// 정점 버퍼와 인스턴스 버퍼를 하번에 디바이스 컨텍스트에 설정
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// 정점 버퍼의 내용이 그려질 도형의 타입을 정한다. 
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
