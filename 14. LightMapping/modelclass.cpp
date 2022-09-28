#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_model = 0;
	m_TextureArray = 0;
} 

ModelClass::ModelClass(const ModelClass& other) { } 

ModelClass::~ModelClass() { }

// Initialize 함수는 정점 버퍼와 인덱스 버퍼의 초기화 함수를 호출한다.
bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2) {
	bool result;

	// 모델을 로드한다.
	result = LoadModel(modelFilename); 
	if (!result) {
		return false;
	}

	// 정점 버퍼와 인덱스 버퍼를 초기화한다.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// 이 모델의 텍스처를 가져온다. 
	result = LoadTextures(device, textureFilename1, textureFilename2);
	if(!result) { 
		return false; 
	}

	return true;
}

// Shutdown 함수는 정점 버퍼와 인덱스 버퍼를 정리하는 함수를 호출한다.
void ModelClass::Shutdown()
{
	// 텍스처를 반환한다.
	ReleaseTextures();

	// 정점 버퍼와 인덱스 버퍼를 해제합니다.
	ShutdownBuffers();

	// 모델을 반환한다.
	ReleaseModel();

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
ID3D11ShaderResourceView** ModelClass::GetTextureArray(){
	return m_TextureArray->GetTextureArray();

}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {

	// 정점 배열을 만들어주고, 모델에 있는 정점들을 정점배열에 넣고, 정점배열들로 정점 버퍼를 만들어준다.

	VertexType* vertices;   // 정점 배열
	unsigned long* indices;  // 인덱스 배열
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


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

	for (int i = 0; i < m_vertexCount; i++){
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);

		indices[i] = i;
	}

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
bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
	bool result;

	// 텍스처 객체를 생성한다.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray){
		return false;
	}

	// 생성된 텍스처 객체를 초기화한다.
	result = m_TextureArray->Initialize(device, filename1, filename2);
	if (!result){
		return false;
	}

	return true;
}

//ReleaseTexture 함수는 LoadTexture 에서 생성한 텍스쳐 객체를 반환한다.
void ModelClass::ReleaseTextures()
{
	// 텍스처 객체를 반환한다.
	if (m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
	}

	return;
}

bool ModelClass::LoadModel(char* filename){
	ifstream fin;
	char input;
	int i;


	// Open the model file.  If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail()){
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':'){
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model){
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':'){
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++){
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void ModelClass::ReleaseModel(){
	if (m_model){
		delete[] m_model;
		m_model = 0;
	}

	return;
}