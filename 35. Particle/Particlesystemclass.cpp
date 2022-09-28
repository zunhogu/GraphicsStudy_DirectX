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

// Initialize �Լ��� ���� ���ۿ� �ε��� ������ �ʱ�ȭ �Լ��� ȣ���Ѵ�.
bool ParticleSystemClass::Initialize(ID3D11Device* device, WCHAR* textureFilename) {
	bool result;

	// �� ���� �ؽ�ó�� �����´�. 
	result = LoadTexture(device, textureFilename); 
	if(!result) { 
		return false; 
	}

	// ��ƼŬ �ý����� �ʱ�ȭ�Ѵ�.
	result = InitializeParticleSystem();
	if (!result) { 
		return false; 
	}

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	return true;
}

// Shutdown �Լ��� ���� ���ۿ� �ε��� ���۸� �����ϴ� �Լ��� ȣ���Ѵ�.
void ParticleSystemClass::Shutdown(){

	// ���� ���ۿ� �ε��� ���۸� ����
	ShutdownBuffers();

	// ��ƼŬ �ý����� �����Ѵ�.
	ShutdownParticleSystem();

	// �ؽ�ó�� ����
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

// Render �Լ��� GraphicsClass::Render �Լ����� ȣ���Ѵ�.
// �� �Լ������� RenderBuffers �Լ��� ȣ���Ͽ� ���� ���ۿ� �ε��� ���۸� �׷��� ���������ο� �־� �÷� ���̴��� �̵��� �׸� �� �ֵ��� �Ѵ�.
void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext){
	// ���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �Ѵ�.
	RenderBuffers(deviceContext);
	return;
}

// GetIndexCount �Լ��� ���� �ε����� ������ �˷��ش�.
// �÷����̴������� ���� �׸��� ���ؼ��� �� ������ �ʿ��ϴ�.
int ParticleSystemClass::GetIndexCount() {
	return m_indexCount;
}

//GetTexture �Լ��� �� �ؽ��� �ڿ��� ��ȯ�Ѵ�.�ؽ��� ���̴��� ���� �׸��� ���ؼ��� �� �ؽ��Ŀ� ������ �� �־�� �մϴ�.
ID3D11ShaderResourceView* ParticleSystemClass::GetTexture(){
	return m_Texture->GetTexture();
}


// InitialBuffers �Լ��� ��ƼŬ�� �׸��� �� ����� ���� �� �ε��� ���۸� �غ��Ѵ�.
// �� �����Ӹ��� ��ƼŬ���� ������ ���̱� ������ ���� ���۴� ���� ���۷� �����Ǿ�� �Ѵ�.
bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device) {

	// ���� �迭�� ������ְ�, �𵨿� �ִ� �������� �����迭�� �ְ�, �����迭��� ���� ���۸� ������ش�.

	unsigned long* indices;  // �ε��� �迭
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ���� �迭�� ���̸� �����Ѵ�. �ִ� ���� ������ ����
	m_vertexCount = m_maxParticles * 6;

	// �ε��� �迭�� ���̸� �����Ѵ�.
	m_indexCount = m_vertexCount;

	// ���� �迭�� �����Ѵ�.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices) {
		return false;
	}

	// �ε��� �迭�� �����Ѵ�.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// ���� �迭 0���� �ʱ�ȭ
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭 �ʱ�ȭ
	for (int i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}


	// ���� �迭�� �ε��� �迭�� ä�������Ƿ� �̰����� ���� ���ۿ� �ε��� ���۸� �����.
	// ���۸� �ʱ�ȭ �ϱ����ؼ��� ������ ����ü ������ �ʿ��ϴ�.
	// �� ������ ������ �ʱ�ȭ�� ����� ����ü ������ �ʿ��ϴ�.

	// ���� ���� ����ü description
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;   // �������۰� ���̴� �뵵 (����� GPU�� ������ �ڿ��� �о�� �ϹǷ� �� �뵵�� ����)
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;   // ���������� ũ�� 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // ���������� ��� ������ ���� �÷��׸� �����Ѵ�. 
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU�� �÷��׿� �����ϴ� ����� �����Ѵ�.(���� ���� ���� CPU�� ���ۿ� �������� ������ 0���� ����)
	vertexBufferDesc.MiscFlags = 0;    // �������ۿ� ���ؼ��� �� ��Ÿ �÷��׸� ����� �ʿ䰡 ����.
	vertexBufferDesc.StructureByteStride = 0;   // ������ ���ۿ� ����� ���� ������ ���� �̿��� ���۴� 0���� �����ϸ� �ȴ�. 

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �����Ѵ�.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����Ѵ�.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); 
	if (FAILED(result)) {
		return false; 
	}

	// �ε��� ���۵� �������ۿ� �Ȱ��� �������� �����Ѵ�.

	// �ε��� ���� ����ü�� �����Ѵ�.
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
	if (FAILED(result)) {
		return false;
	}

	// �ε��� �迭�� �����Ѵ�.
	delete[] indices;
	indices = 0;

	return true;
}

bool ParticleSystemClass::InitializeParticleSystem() {
	int i;

	// ��ƼŬ �Ӽ����� ����� ��� ��Ҹ� �ʱ�ȭ�Ѵ�.
	// �� ��ƼŬ�ý��ۿ����� ��ƼŬ�� ���ܳ��� ���ܳ� ��ġ�� ���� ������ �л� ���� �����Ѵ�.
	m_particleDeviationX = 0.5f; 
	m_particleDeviationY = 0.1f; 
	m_particleDeviationZ = 2.0f;

	// ��ƼŬ �ӵ� �� �ӵ� ������ ���Ѵ�.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// ��ƼŬ�� ���� ũ��
	m_particleSize = 0.2f;

	// ��ƼŬ�� �ʴ� ���ܳ� ����
	m_particlesPerSecond = 250.0f;

	// ��ƼŬ �ý��ۿ��� ����� ��ƼŬ�� �ִ� ����
	m_maxParticles = 5000;

	// ��ƼŬ �迭�� �����Ѵ�.
	m_particleList = new ParticleType[m_maxParticles]; 
	if (!m_particleList) {
		return false; 
	}

	// ��ƼŬ �迭 �ʱ�ȭ
	for (i = 0; i < m_maxParticles; i++) {
		m_particleList[i].active = false;
	}

	// �����ϱ� ���� �� ī��Ʈ ������ 0���� �ʱ�ȭ�Ѵ�.
	// �ƹ� ��ƼŬ �������� �ʾұ� ������ ���� ��ƼŬ ������ 0���� �ʱ�ȭ�Ѵ�.
	m_currentParticleCount = 0;

	// �ʵ� ���� ����� ���� ���� �ð��� �ʱ�ȭ�Ѵ�
	m_accumulatedTime = 0.0f;
}



// ShutdownBuffers �Լ��� �ܼ��� InitializeBuffers �Լ����� ������� �������ۿ� �ε������۸� �ʱ�ȭ�Ѵ�.
void ParticleSystemClass::ShutdownBuffers() {
	
	// �ε��� ���۸� �����Ѵ�.
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����Ѵ�.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

void ParticleSystemClass::ShutdownParticleSystem(){
	// ��ƼŬ �迭�� ����
	if (m_particleList){
		delete[] m_particleList;
		m_particleList = 0;
	}

	return;
}

// UpdateBuffers �Լ��� �� �����Ӹ��� �ҷ� �ʿ��� ��� ���� ���������� ������ �� ��ġ�� �����Ѵ�.
bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext){
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// ��ƼŬ ��� �迭�� ������ ���� �迭�� ����ϴ�. �� ��ƼŬ�� �� ���� �ﰢ������ �̷���� �簢���̴�.
	index = 0;

	for (i = 0; i < m_currentParticleCount; i++){
		// ���� �Ʒ�
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ���� ��
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ������ �Ʒ�
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ������ �Ʒ�
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ���� ��
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ������ ��
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// ���� ���۸� ��ٴ�.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// ���� ������ ������ �����͸� �����´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// ���� ���ۿ� �����͸� �����Ѵ�.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// ���� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

// RenderBuffers �Լ��� Render �Լ����� ȣ��ȴ�. 
// �� �Լ��� ������ ���� ���ۿ� �ε��� ���۸� GPU�� ������� ���۷� Ȱ��ȭ ��Ű�� ���̴�.
// �ϴ� GPU�� Ȱ��ȭ�� �������۸� ������ �Ǹ� ���̴��� �̿��ؼ� ������ ������ �׸� �� �ְ� �ȴ�.
// ���� �� �Լ������� �� ������ �ﰢ���̳� ����, ��ä�� �� � ������� �׸��� ���� �����Ѵ�.
// ������� �Է¿� �������ۿ� �ε��� ���۸� �ְ� DirectX��  IASetPrimitiveTopology �Լ��� �̿��� GPU���� �� �������� �ﰢ���� �׸����� �ֹ��Ѵ�. 
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

// LoadTexture �Լ��� ������ ���� �̸����� �ؽ��� ��ü�� ����� �ʱ�ȭ�ϴ� �Լ���.
bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// �ؽ�ó ��ü�� �����Ѵ�.
	m_Texture = new TextureClass;
	if (!m_Texture){
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	result = m_Texture->Initialize(device, filename);
	if (!result){
		return false;
	}

	return true;
}

//ReleaseTexture �Լ��� LoadTexture ���� ������ �ؽ��� ��ü�� ��ȯ�Ѵ�.
void ParticleSystemClass::ReleaseTexture()
{
	// �ؽ�ó ��ü�� ��ȯ�Ѵ�.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

// EmitParticles�� ���ο� ��ƼŬ�� ���� �� �� ȣ��ȴ�. ������ �ð��� ��ƼŬ ���� ������ �����Ͽ� ���� ��ƼŬ�� ���;� ���� ���Ѵ�.
// ���� ����Ǿ�߸� �Ѵٸ� �� ��ƼŬ�� �����ǰ� �Ӽ����� �����ȴ�. �׸��� ���� ��ƼŬ �迭�� Z ����(���̰�) ������� ���ĵǾ� ���Եȴ�.
// �� ��ƼŬ �迭�� ���ĺ����� ����� �����ϱ� ���Ͽ� ���̰����� �ùٸ��� ���ĵǾ� �־�� �Ѵ�. ���� �������� ������ ����� ����� �����ͤ���.
void ParticleSystemClass::EmitParticles(float frameTime){
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	// ������ �ð��� ���Ѵ�.
	m_accumulatedTime += frameTime;

	// ��ƼŬ ���� ���θ� false�� ����
	emitParticle = false;

	// �� ��ƼŬ�� �� �ð����� Ȯ��
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond)){
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// ��ƼŬ�� ���;� �Ѵٸ� �����Ӹ��� �ϳ��� ����
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1))){
		m_currentParticleCount++;

		// ���Ƿ� ��ƼŬ �Ӽ��� ����
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// �����ϱ� ���ؼ��� ��ƼŬ���� �ڿ������� ������ �׷��� �ϱ� ������ �迭�� ����
		// Z���̰����� ������ ���̱� ������ ��ƼŬ�� ��� ������ ���ԵǾ�� �� �� ã�ƾ� �Ѵ�
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

		// ��ġ�� ���������Ƿ� �� ��ġ�������� �迭�� �ϳ��� �о� �����ؼ� �� ��ƼŬ�� ���� ������ �����.
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

		// �ùٸ� ���� ������� ��ƼŬ �迭�� �߰�
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


// UpdateParticels �Լ����� �� �����Ӹ��� ��ƼŬ �Ӽ����� ������Ʈ�Ѵ�.
// �� ������ ������ �ӵ��� ����Ͽ� ���̰��� �����Ͽ� ���� ����� ��ƷŬ ȿ���� ������.
void ParticleSystemClass::UpdateParticles(float frameTime){
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i < m_currentParticleCount; i++){
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

//KillParticles �Լ��� ������ �ֱⰡ ���� ��ƼŬ���� �ý��ۿ��� �����ϴ� ���� �Ѵ�.
// �� �����Ӹ��� �Լ��� ȣ��Ǿ� ���ŵǾ�� �� ��ƼŬ�� �ִ��� Ȯ���Ѵ�.
//�� ������ ��ƼŬ�� ���̰� -3.0 �̸����� ������������ Ȯ���ϰ� ���� �׷��ٸ� �迭���� �����ϰ� �� �� ���̼������ �����Ѵ�.
void ParticleSystemClass::KillParticles(){
	int i, j;


	// Ư�� ���� ���� �Ʒ��� ������ ��ƼŬ���� ���� ����
	for (i = 0; i < m_maxParticles; i++){
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < -3.0f)){
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// ���� ��� �ִ� ��� ��ƼŬ���� ������ ��� ���� ������ �����ϸ鼭 ����� ��ƼŬ���� ���������� �Ѵ�.
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