#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0; 
	m_instanceBuffer = 0;
	m_Texture = 0;
} 

ModelClass::ModelClass(const ModelClass& other) { } 

ModelClass::~ModelClass() { }

// Initialize �Լ��� ���� ���ۿ� �ε��� ������ �ʱ�ȭ �Լ��� ȣ���Ѵ�.
bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename) {
	bool result;

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// �� ���� �ؽ�ó�� �����´�. 
	result = LoadTexture(device, textureFilename); 
	if(!result) { 
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

int ModelClass::GetVertexCount() { 
	return m_vertexCount; 
}

int ModelClass::GetInstanceCount() {
	return m_instanceCount;
}

//GetTexture �Լ��� �� �ؽ��� �ڿ��� ��ȯ�Ѵ�.�ؽ��� ���̴��� ���� �׸��� ���ؼ��� �� �ؽ��Ŀ� ������ �� �־�� �մϴ�.
ID3D11ShaderResourceView* ModelClass::GetTexture(){
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;   // ���� �迭
	InstanceType* instances;   // �ν��Ͻ� �迭
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;

	// ���� �迭�� ���̸� �����Ѵ�.
	m_vertexCount = 3;

	// ���� �迭�� �����Ѵ�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// �ε��� �迭�� ���۴� ������ �ʴ´�.

	// ���� �迭�� ������ָ� ���� �������� �迭�� �־��ش�.
	// ���⼭ ������ ���� �ð� �������� ���� ���� �־���Ѵ�. ���� �ݽð� �������� ����� DirectX���� �� �ﰢ���� �ݴ����� �ٶ󺻴ٰ� �Ǵ��Ѵ�.
	// �̷��� �Ǹ� ������ȭ�� �ܰ迡�� �ĸ鼱���� ���� �׷����� �ʰ� �ȴ�.
	// ���� GPU���� ������ �׸����� �Ҷ� �� ������ ����ϰ� �ϴ� ���� �߿��ϴ�.

	// ���� �迭�� ���� ���� ��� �ؽ�ó ������ ������ �ȴ�. 
	// �ؽ�ó ������ uv��ǥ������ ���� ǥ���� ���� ���ϴ� �κ��� �ؽ�ó�� ���� �� �ִ�.
	// ����� �ִ��� �����ϰ� �ϱ����� ���� ������ �Ѵ�. 

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);   // ���� �Ʒ� 
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);   // ��� ��� 
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);   // ������ �Ʒ� 
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

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
	delete[] vertices;
	vertices = 0;

	// �ν��Ͻ� ������ �ʱ�ȭ 
	m_instanceCount = 4;

	// �ν��Ͻ� �迭�� ����
	instances = new InstanceType[m_instanceCount];
	if (!instances) {
		return false;
	}
	
	// �ν��Ͻ� �迭 �ʱ�ȭ
	instances[0].position = XMFLOAT3(-1.5f, -1.5f, 5.0f);
	instances[1].position = XMFLOAT3(-1.5f, 1.5f, 5.0f);
	instances[2].position = XMFLOAT3(1.5f, -1.5f, 5.0f);
	instances[3].position = XMFLOAT3(1.5f, 1.5f, 5.0f);

	// �ν��Ͻ� ���� discription�� ���������� discription�� �����ϰ� �ۼ�
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// �ν��Ͻ��� ����Ű�� ���ҽ� ����ü�� ����
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

// ShutdownBuffers �Լ��� �ܼ��� InitializeBuffers �Լ����� ������� �������ۿ� �ε������۸� �ʱ�ȭ�Ѵ�.
void ModelClass::ShutdownBuffers() {
	
	// �ε��� ���۸� �����Ѵ�.
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// ���� ���۸� �����Ѵ�.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

// RenderBuffers���� ���۵��� �׷��� �������������� �����ϴµ� �ν��Ͻ� ���۴� ���� �ٸ� ������ ��� �ִ� �ι�° ���� �����̱� ������ ���� ���۰� ���õǴ� ������ ���� ���õȴ�.
// ���� IASetVertexBuffers �Լ��� ȣ���� �� ��Ʈ���̵� ������, ���۸� �ϳ��� ������ ���� �ƴ϶� �迭�� ����� ������.
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int strides[2]; 
	unsigned int offsets[2]; 
	ID3D11Buffer* bufferPointers[2];

	// ������ ��Ʈ���̵� ���� 
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// ������ offset ����
	offsets[0] = 0; 
	offsets[1] = 0;

	// ���� ���ۿ� �ν��Ͻ� ������ �����͸� ��� �ִ� �迭 ����
	bufferPointers[0] = m_vertexBuffer; 
	bufferPointers[1] = m_instanceBuffer;

	// ���� ���ۿ� �ν��Ͻ� ���۸� �Ϲ��� ����̽� ���ؽ�Ʈ�� ����
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// ���� ������ ������ �׷��� ������ Ÿ���� ���Ѵ�. 
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

// LoadTexture �Լ��� ������ ���� �̸����� �ؽ��� ��ü�� ����� �ʱ�ȭ�ϴ� �Լ���.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
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
