#include "ParticleSystemClass.h"

ParticleSystemClass::ParticleSystemClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertices = 0;
	m_Texture = 0;
	m_particleList = 0;
} 

ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other) { }

ParticleSystemClass::~ParticleSystemClass() { }

// Initialize 함수는 정점 버퍼와 인덱스 버퍼의 초기화 함수를 호출한다.
bool ParticleSystemClass::Initialize(ID3D11Device* device, WCHAR* textureFilename) {
	bool result;

	// 이 모델의 텍스처를 가져온다. 
	result = LoadTexture(device, textureFilename); 
	if(!result) { 
		return false; 
	}

	// 파티클 시스템을 초기화한다.
	result = InitializeParticleSystem();
	if (!result) { 
		return false; 
	}

	// 정점 버퍼와 인덱스 버퍼를 초기화한다.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	return true;
}

// Shutdown 함수는 정점 버퍼와 인덱스 버퍼를 정리하는 함수를 호출한다.
void ParticleSystemClass::Shutdown(){

	// 정점 버퍼와 인덱스 버퍼를 해제
	ShutdownBuffers();

	// 파티클 시스템을 해제한다.
	ShutdownParticleSystem();

	// 텍스처를 해제
	ReleaseTexture();

	return;
}

bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext) {
	bool result;

	KillParticles();

	EmitParticles(frameTime);

	UpdateParticles(frameTime);

	result = UpdateBuffers(deviceContext);
	if (!result) {
		return false;
	}

	return true;
}

// Render 함수는 GraphicsClass::Render 함수에서 호출한다.
// 이 함수에서는 RenderBuffers 함수를 호출하여 정점 버퍼와 인덱스 버퍼를 그래픽 파이프라인에 넣어 컬러 셰이더가 이들을 그릴 수 있도록 한다.
void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext){
	// 정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 한다.
	RenderBuffers(deviceContext);
	return;
}

// GetIndexCount 함수는 모델의 인덱스의 갯수를 알려준다.
// 컬러셰이더에서는 모델을 그리기 위해서는 이 정보가 필요하다.
int ParticleSystemClass::GetIndexCount() {
	return m_indexCount;
}

//GetTexture 함수는 모델 텍스쳐 자원을 반환한다.텍스쳐 셰이더가 모델을 그리기 위해서는 이 텍스쳐에 접근할 수 있어야 합니다.
ID3D11ShaderResourceView* ParticleSystemClass::GetTexture(){
	return m_Texture->GetTexture();
}


// InitialBuffers 함수는 파티클을 그리는 데 사용할 정점 및 인덱스 버퍼를 준비한다.
// 매 프레임마다 파티클들을 갱신할 것이기 때문에 정점 버퍼는 동적 버퍼로 생성되어야 한다.
bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device) {

	// 정점 배열을 만들어주고, 모델에 있는 정점들을 정점배열에 넣고, 정점배열들로 정점 버퍼를 만들어준다.

	unsigned long* indices;  // 인덱스 배열
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 정점 배열의 길이를 설정한다. 최대 정점 개수로 설정
	m_vertexCount = m_maxParticles * 6;

	// 인덱스 배열의 길이를 설정한다.
	m_indexCount = m_vertexCount;

	// 정점 배열을 생성한다.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices) {
		return false;
	}

	// 인덱스 배열을 생성한다.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// 정점 배열 0으로 초기화
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열 초기화
	for (int i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}


	// 정점 배열과 인덱스 배열이 채워졌으므로 이것으로 정점 버퍼와 인덱스 버퍼를 만든다.
	// 버퍼를 초기화 하기위해서는 버퍼의 구조체 서술이 필요하다.
	// 그 다음은 버퍼의 초기화에 사용할 구조체 서술이 필요하다.

	// 정점 버퍼 구조체 description
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;   // 정점버퍼가 쓰이는 용도 (현재는 GPU가 버퍼의 자원을 읽어야 하므로 이 용도로 지정)
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;   // 정점버퍼의 크기 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // 정점버퍼의 경우 다음과 같은 플래그를 지정한다. 
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU가 플래그에 접근하는 방식을 지정한다.(버퍼 생성 이후 CPU가 버퍼에 접근하지 않으면 0으로 지정)
	vertexBufferDesc.MiscFlags = 0;    // 정점버퍼에 대해서는 이 기타 플래그를 사용할 필요가 없다.
	vertexBufferDesc.StructureByteStride = 0;   // 구조적 버퍼에 저장된 원소 구조적 버퍼 이외의 버퍼는 0으로 설정하면 된다. 

	// 정점 데이터를 가리키는 보조 리소스 구조체를 서술한다.
	vertexData.pSysMem = m_vertices;
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

	// 인덱스 배열을 제거한다.
	delete[] indices;
	indices = 0;

	return true;
}

bool ParticleSystemClass::InitializeParticleSystem() {
	int i;

	// 파티클 속성으로 사용할 모든 요소를 초기화한다.
	// 이 파티클시스템에서는 파티클이 생겨날이 생겨날 위치에 대한 임의의 분산 값을 설정한다.
	m_particleDeviationX = 0.5f; 
	m_particleDeviationY = 0.1f; 
	m_particleDeviationZ = 2.0f;

	// 파티클 속도 및 속도 편차를 정한다.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// 파티클의 실제 크기
	m_particleSize = 0.2f;

	// 파티클이 초당 생겨날 갯수
	m_particlesPerSecond = 250.0f;

	// 파티클 시스템에게 허락된 파티클의 최대 개수
	m_maxParticles = 5000;

	// 파티클 배열을 생성한다.
	m_particleList = new ParticleType[m_maxParticles]; 
	if (!m_particleList) {
		return false; 
	}

	// 파티클 배열 초기화
	for (i = 0; i < m_maxParticles; i++) {
		m_particleList[i].active = false;
	}

	// 시작하기 위해 두 카운트 변수를 0으로 초기화한다.
	// 아무 파티클 방출하지 않았기 때문에 현재 파티클 개수를 0으로 초기화한다.
	m_currentParticleCount = 0;

	// 초딩 방출 비용을 위해 누적 시간을 초기화한다
	m_accumulatedTime = 0.0f;
}



// ShutdownBuffers 함수는 단순히 InitializeBuffers 함수에서 만들었던 정점버퍼와 인덱스버퍼를 초기화한다.
void ParticleSystemClass::ShutdownBuffers() {
	
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

void ParticleSystemClass::ShutdownParticleSystem(){
	// 파티클 배열을 해제
	if (m_particleList){
		delete[] m_particleList;
		m_particleList = 0;
	}

	return;
}

// UpdateBuffers 함수는 매 프레임마다 불려 필요할 경우 동적 정점버퍼의 내용을 새 위치로 변경한다.
bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext){
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// 정점 배열을 0으로 초기화합니다.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 파티클 목록 배열을 가지고 정점 배열을 만듭니다. 각 파티클은 두 개의 삼각형으로 이루어진 사각형이다.
	index = 0;

	for (i = 0; i < m_currentParticleCount; i++){
		// 왼쪽 아래
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 왼쪽 위
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 아래
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 아래
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 왼쪽 위
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 위
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// 정점 버퍼를 잠근다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// 정점 버퍼의 데이터 포인터를 가져온다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 정점 버퍼에 데이터를 복사한다.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제한다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

// RenderBuffers 함수는 Render 함수에서 호출된다. 
// 이 함수의 목적은 정점 버퍼와 인덱스 버퍼를 GPU의 어셈블리의 버퍼로 활성화 시키는 것이다.
// 일단 GPU가 활성화된 정점버퍼를 가지게 되면 셰이더를 이용해서 버퍼의 내용을 그릴 수 있게 된다.
// 또한 이 함수에서는 이 정점을 삼각형이나 선분, 부채꼴 등 어떤 모양으로 그리게 될지 정의한다.
// 어셈블리의 입력에 정점버퍼와 인덱스 버퍼를 넣고 DirectX의  IASetPrimitiveTopology 함수를 이용해 GPU에게 이 정점들을 삼각형을 그리도록 주문한다. 
void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
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
bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
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
void ParticleSystemClass::ReleaseTexture()
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

// EmitParticles는 새로운 파티클이 방출 될 때 호출된다. 프레임 시간과 파티클 개수 변수를 참고하여 언제 파티클이 나와야 할지 정한다.
// 만약 방출되어야만 한다면 새 파티클이 생성되고 속성들이 설정된다. 그리고 나서 파티클 배열에 Z 뎁스(깊이값) 순서대로 정렬되어 삽입된다.
// 이 파티클 배열을 알파블렌딩이 제대로 동작하기 위하여 깊이값으로 올바르게 정렬되어 있어야 한다. 만약 정열되지 않으면 어색한 결과를 얻을것ㅇ디ㅏ.
void ParticleSystemClass::EmitParticles(float frameTime){
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	// 프레임 시간을 더한다.
	m_accumulatedTime += frameTime;

	// 파티클 방출 여부를 false로 설정
	emitParticle = false;

	// 새 파티클을 낼 시간인지 확인
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond)){
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// 파티클이 나와야 한다면 프레임마다 하나씩 방출
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1))){
		m_currentParticleCount++;

		// 임의로 파티클 속성을 생성
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// 블렌딩하기 위해서는 파티클들을 뒤에서부터 앞으로 그려야 하기 때문에 배열을 정렬
		// Z깊이값으로 정렬할 것이기 때문에 파티클이 어느 지점에 삽입되어야 할 지 찾아야 한다
		index = 0;
		found = false;
		while (!found){
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ)){
				found = true;
			}
			else{
				index++;
			}
		}

		// 위치가 정해졌으므로 그 위치에서부터 배열을 하나씩 밀어 복사해서 새 파티클을 위한 공간을 만든다.
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index){
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// 올바른 깊이 순서대로 파티클 배열에 추가
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}

	return;
}


// UpdateParticels 함수에서 매 프레임마다 파티클 속성들을 업데이트한다.
// 이 예제는 각자의 속도에 기반하여 높이값을 갱신하여 폭포 모양의 파틔클 효과를 만들어낸다.
void ParticleSystemClass::UpdateParticles(float frameTime){
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i < m_currentParticleCount; i++){
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

//KillParticles 함수는 렌더링 주기가 끝난 파티클들을 시스템에서 제거하는 일을 한다.
// 매 프레임마다 함수가 호출되어 제거되어야 할 파티클이 있는지 확인한다.
//이 예제는 파티클의 높이가 -3.0 미만으로 내려갔는지만 확인하고 만약 그렇다면 배열에서 제거하고 다 시 깊이순서대로 정렬한다.
void ParticleSystemClass::KillParticles(){
	int i, j;


	// 특정 높이 범위 아래로 내려간 파티클들을 전부 제거
	for (i = 0; i < m_maxParticles; i++){
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < -3.0f)){
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// 아직 살아 있는 모든 파티클들을 앞으로 당겨 정렬 순서는 유지하면서 사라진 파티클들은 지워지도록 한다.
			for (j = i; j < m_maxParticles - 1; j++){
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}

	return;
}