#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_TextureArray = 0;
	m_model = 0;
} 

ModelClass::ModelClass(const ModelClass& other) { } 

ModelClass::~ModelClass() { }

// Initialize �Լ��� ���� ���ۿ� �ε��� ������ �ʱ�ȭ �Լ��� ȣ���Ѵ�.
bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, WCHAR* textureFilename3) {
	bool result;

	// ���� �����´�.
	result = LoadModel(modelFilename);
	if (!result) {
		return false;
	}

	// �� �����Ͱ� �ε�Ǹ� ���� ������ ������ �Ի��� �������͸� ���Ѵ�.
	CalculateModelVectors();

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// �� ���� �ؽ�ó�� �����´�. 
	result = LoadTextures(device, textureFilename1, textureFilename2, textureFilename3);
	if(!result) { 
		return false; 
	}

	return true;
}

// Shutdown �Լ��� ���� ���ۿ� �ε��� ���۸� �����ϴ� �Լ��� ȣ���Ѵ�.
void ModelClass::Shutdown()
{
	// �ؽ�ó�� ��ȯ�Ѵ�.
	ReleaseTextures();

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
ID3D11ShaderResourceView** ModelClass::GetTextureArray() {
	return m_TextureArray->GetTextureArray();

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
		vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

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
bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
{
	bool result;

	// �ؽ�ó ��ü�� �����Ѵ�.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray) {
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	result = m_TextureArray->Initialize(device, filename1, filename2, filename3);
	if (!result) {
		return false;
	}

	return true;
}

//ReleaseTexture �Լ��� LoadTexture ���� ������ �ؽ��� ��ü�� ��ȯ�Ѵ�.
void ModelClass::ReleaseTextures()
{
	// �ؽ�ó ��ü�� ��ȯ�Ѵ�.
	if (m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
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
	while(input != ':'){
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

// CalculateModelVector �Լ��� ������ �������� ���Ѵ�. �׸��� ���� ���͸� ����Ѵ�.
// �켱 �𵨿� �ִ� �ﰢ�� ���� ����Ѵ�. 
// �׸����� �� �ﰢ���� �� ������ ������ ������ �������� ����Ѵ�.
// ����� ��ġ�� �� ����� ���� ���͸� �� ����ü�� ���� �����Ѵ�. 
void ModelClass::CalculateModelVectors(){
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// ���� �ﰢ���� ���� ��� 
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// �ﰢ���� �� ������ ������ ���� ���͸� ���� 
	for (i = 0; i < faceCount; i++){
		// �ﰢ���� �� ������������ ��ġ, uv, ���� ������ ������ ��Ƶд�. 
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

		// ������ �������� ����Ѵ�. 
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// �������͸� ����Ѵ�. 
		CalculateNormal(tangent, binormal, normal);

		// �������͸� �����Ѵ�. 
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

// CalculateTangentBinormal �Լ��� �Էµ� �� �������� tangent�� binormal�� ������ش�.
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

// CalculateNormal �Լ��� tangent�� binormal�� �Է����� �޾� ������������ ���� ���͸� ���Ѵ�. 
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
