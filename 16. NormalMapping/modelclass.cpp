#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_TextureArray = 0;
	m_model = 0;
} 

ModelClass::ModelClass(const ModelClass& other) { } 

ModelClass::~ModelClass() { }

// Initialize 함수는 정점 버퍼와 인덱스 버퍼의 초기화 함수를 호출한다.
bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, WCHAR* textureFilename3) {
	bool result;

	// 모델을 가져온다.
	result = LoadModel(modelFilename);
	if (!result) {
		return false;
	}

	// 모델 데이터가 로드되면 면의 접선과 정점을 게산후 법선벡터를 구한다.
	CalculateModelVectors();

	// 정점 버퍼와 인덱스 버퍼를 초기화한다.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// 이 모델의 텍스처를 가져온다. 
	result = LoadTextures(device, textureFilename1, textureFilename2, textureFilename3);
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
ID3D11ShaderResourceView** ModelClass::GetTextureArray() {
	return m_TextureArray->GetTextureArray();

}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {

	// 정점 배열을 만들어주고, 모델에 있는 정점들을 정점배열에 넣고, 정점배열들로 정점 버퍼를 만들어준다.

	VertexType* vertices;   // 정점 배열
	unsigned long* indices;  // 인덱스 배열
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 이제는 정점과 인덱스의 수, 값을 직접 입력하는 것이 아니라 모델을 가져온 뒤 모델배열을 만들고 모델배열의 정점과 인덱스 값을 넣어준다.
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

	// 직접 정점과 인덱스 값을 넣어주는 것이 아니라 for 루프를 돌면서 m_model 안에 있는 정보들을 정점배열로 복사한다.
	// 인덱스배열의 경우는 해당 배열에서의 위치가 곧 인덱스 번호이기때문에 간단히 지정가능하다.

	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

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
bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
{
	bool result;

	// 텍스처 객체를 생성한다.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray) {
		return false;
	}

	// 생성된 텍스처 객체를 초기화한다.
	result = m_TextureArray->Initialize(device, filename1, filename2, filename3);
	if (!result) {
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


// LoadModel 함수는 텍스트 파일의 모델 데이터를 m_model 배열로 로드하는 역할을 한다.
bool ModelClass::LoadModel(char* filename) {
	ifstream fin;
	char input;
	int i;

	// 파일을 연다.
	fin.open(filename);
	if (fin.fail()) {
		return false;
	}

	// 정점수를 읽는다.
	fin.get(input);
	while(input != ':'){
		fin.get(input);
	}
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;

	// ModelType의 배열을 생성하고 각 라인을 읽어 그 값을 배열에 넣는다. 정점과 인덱스 수가 이함수에서 결정된다.
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

	// 데이터를 읽는다.
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	fin.get(input); 
	fin.get(input);

	for (int i = 0; i < m_vertexCount; i++) {
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}
	fin.close();

	return true;
}

void ModelClass::ReleaseModel() {
	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}
}

// CalculateModelVector 함수는 접선과 종법선을 구한다. 그리고 법선 벡터를 계산한다.
// 우선 모델에 있는 삼각형 수를 계산한다. 
// 그리고나서 각 삼각형의 세 정점을 가지고 접선과 종법선을 계산한다.
// 계산을 마치면 그 결과인 법선 벡터를 모델 구조체에 도로 저장한다. 
void ModelClass::CalculateModelVectors(){
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// 모델의 삼각형의 개수 계산 
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// 삼각형의 세 정점을 가지고 법선 벡터를 구함 
	for (i = 0; i < faceCount; i++){
		// 삼각형의 각 꼭짓점에대한 위치, uv, 법선 벡터의 정보를 담아둔다. 
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		// 접선과 종법선을 계산한다. 
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// 법선벡터를 계산한다. 
		CalculateNormal(tangent, binormal, normal);

		// 법선벡터를 저장한다. 
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;
	}

	return;
}

// CalculateTangentBinormal 함수는 입력된 세 정점으로 tangent와 binormal을 계산햐준다.
void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,VectorType& tangent, VectorType& binormal){
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

// CalculateNormal 함수는 tangent와 binormal을 입력으로 받아 외적연산으로 법선 벡터를 구한다. 
void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal){
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}
