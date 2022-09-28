#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}

ModelClass::ModelClass(const ModelClass& other) { }

ModelClass::~ModelClass() { }

// Initialize �Լ��� ���� ���ۿ� �ε��� ������ �ʱ�ȭ �Լ��� ȣ���Ѵ�.
bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename) {
	bool result;

	// ���� �����´�.
	result = LoadModel(modelFilename);
	if (!result) {
		return false;
	}

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// �� ���� �ؽ�ó�� �����´�. 
	result = LoadTexture(device, textureFilename);
	if (!result) {
		return false;
	}

	return true;
}

// Shutdown �Լ��� ���� ���ۿ� �ε��� ���۸� �����ϴ� �Լ��� ȣ���Ѵ�.
void ModelClass::Shutdown()
{
	// �ؽ�ó�� ��ȯ�Ѵ�.
	ReleaseTexture();

	// ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();

	// ���� ��ȯ�Ѵ�.
	ReleaseModel();

	return;
}

// Render �Լ��� GraphicsClass::Render �Լ����� ȣ���Ѵ�.
// �� �Լ������� RenderBuffers �Լ��� ȣ���Ͽ� ���� ���ۿ� �ε��� ���۸� �׷��� ���������ο� �־� �÷� ���̴��� �̵��� �׸� �� �ֵ��� �Ѵ�.
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// ���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �Ѵ�.
	RenderBuffers(deviceContext);
	return;
}

// GetIndexCount �Լ��� ���� �ε����� ������ �˷��ش�.
// �÷����̴������� ���� �׸��� ���ؼ��� �� ������ �ʿ��ϴ�.
int ModelClass::GetIndexCount() {
	return m_indexCount;
}

//GetTexture �Լ��� �� �ؽ��� �ڿ��� ��ȯ�Ѵ�.�ؽ��� ���̴��� ���� �׸��� ���ؼ��� �� �ؽ��Ŀ� ������ �� �־�� �մϴ�.
ID3D11ShaderResourceView* ModelClass::GetTexture() {
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {

	// ���� �迭�� ������ְ�, �𵨿� �ִ� �������� �����迭�� �ְ�, �����迭��� ���� ���۸� ������ش�.

	VertexType* vertices;   // ���� �迭
	unsigned long* indices;  // �ε��� �迭
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ������ ������ �ε����� ��, ���� ���� �Է��ϴ� ���� �ƴ϶� ���� ������ �� �𵨹迭�� ����� �𵨹迭�� ������ �ε��� ���� �־��ش�.
	// ���� �迭�� �����Ѵ�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// �ε��� �迭�� �����Ѵ�.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// ���� �迭�� ������ָ� ���� �������� �迭�� �־��ش�.
	// ���⼭ ������ ���� �ð� �������� ���� ���� �־���Ѵ�. ���� �ݽð� �������� ����� DirectX���� �� �ﰢ���� �ݴ����� �ٶ󺻴ٰ� �Ǵ��Ѵ�.
	// �̷��� �Ǹ� ������ȭ�� �ܰ迡�� �ĸ鼱���� ���� �׷����� �ʰ� �ȴ�.
	// ���� GPU���� ������ �׸����� �Ҷ� �� ������ ����ϰ� �ϴ� ���� �߿��ϴ�.

	// ���� �迭�� ���� ���� ��� �ؽ�ó ������ ������ �ȴ�. 
	// �ؽ�ó ������ uv��ǥ������ ���� ǥ���� ���� ���ϴ� �κ��� �ؽ�ó�� ���� �� �ִ�.
	// ����� �ִ��� �����ϰ� �ϱ����� ���� ������ �Ѵ�. 

	// ���� ������ �ε��� ���� �־��ִ� ���� �ƴ϶� for ������ ���鼭 m_model �ȿ� �ִ� �������� �����迭�� �����Ѵ�.
	// �ε����迭�� ���� �ش� �迭������ ��ġ�� �� �ε��� ��ȣ�̱⶧���� ������ ���������ϴ�.

	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}


	// ���� �迭�� �ε��� �迭�� ä�������Ƿ� �̰����� ���� ���ۿ� �ε��� ���۸� �����.
	// ���۸� �ʱ�ȭ �ϱ����ؼ��� ������ ����ü ������ �ʿ��ϴ�.
	// �� ������ ������ �ʱ�ȭ�� ����� ����ü ������ �ʿ��ϴ�.

	// ���� ���� ����ü description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;   // �������۰� ���̴� �뵵 (����� GPU�� ������ �ڿ��� �о�� �ϹǷ� �� �뵵�� ����)
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;   // ���������� ũ�� 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // ���������� ��� ������ ���� �÷��׸� �����Ѵ�. 
	vertexBufferDesc.CPUAccessFlags = 0;    // CPU�� �÷��׿� �����ϴ� ����� �����Ѵ�.(���� ���� ���� CPU�� ���ۿ� �������� ������ 0���� ����)
	vertexBufferDesc.MiscFlags = 0;    // �������ۿ� ���ؼ��� �� ��Ÿ �÷��׸� ����� �ʿ䰡 ����.
	vertexBufferDesc.StructureByteStride = 0;   // ������ ���ۿ� ����� ���� ������ ���� �̿��� ���۴� 0���� �����ϸ� �ȴ�. 

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �����Ѵ�.
	vertexData.pSysMem = vertices;
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

	// ���� ���ۿ� �ε��� ���۸� ���� �Ŀ��� �̹� ���� ����Ǿ��� ������ �����迭�� �ε��� �迭�� �����Ѵ�.

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

// ShutdownBuffers �Լ��� �ܼ��� InitializeBuffers �Լ����� ������� �������ۿ� �ε������۸� �ʱ�ȭ�Ѵ�.
void ModelClass::ShutdownBuffers() {

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

// RenderBuffers �Լ��� Render �Լ����� ȣ��ȴ�. 
// �� �Լ��� ������ ���� ���ۿ� �ε��� ���۸� GPU�� ������� ���۷� Ȱ��ȭ ��Ű�� ���̴�.
// �ϴ� GPU�� Ȱ��ȭ�� �������۸� ������ �Ǹ� ���̴��� �̿��ؼ� ������ ������ �׸� �� �ְ� �ȴ�.
// ���� �� �Լ������� �� ������ �ﰢ���̳� ����, ��ä�� �� � ������� �׸��� ���� �����Ѵ�.
// ������� �Է¿� �������ۿ� �ε��� ���۸� �ְ� DirectX��  IASetPrimitiveTopology �Լ��� �̿��� GPU���� �� �������� �ﰢ���� �׸����� �ֹ��Ѵ�. 
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

// LoadTexture �Լ��� ������ ���� �̸����� �ؽ��� ��ü�� ����� �ʱ�ȭ�ϴ� �Լ���.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// �ؽ�ó ��ü�� �����Ѵ�.
	m_Texture = new TextureClass;
	if (!m_Texture) {
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	result = m_Texture->Initialize(device, filename);
	if (!result) {
		return false;
	}

	return true;
}

//ReleaseTexture �Լ��� LoadTexture ���� ������ �ؽ��� ��ü�� ��ȯ�Ѵ�.
void ModelClass::ReleaseTexture()
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


// LoadModel �Լ��� �ؽ�Ʈ ������ �� �����͸� m_model �迭�� �ε��ϴ� ������ �Ѵ�.
bool ModelClass::LoadModel(char* filename) {
	ifstream fin;
	char input;
	int i;

	// ������ ����.
	fin.open(filename);
	if (fin.fail()) {
		return false;
	}

	// �������� �д´�.
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;

	// ModelType�� �迭�� �����ϰ� �� ������ �о� �� ���� �迭�� �ִ´�. ������ �ε��� ���� ���Լ����� �����ȴ�.
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

	// �����͸� �д´�.
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