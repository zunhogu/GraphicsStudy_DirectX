#include "textclass.h"

TextClass::TextClass(){
	m_Font = 0;
	m_FontShader = 0;
	m_sentence1 = 0;
	m_sentence2 = 0;
}

TextClass::TextClass(const TextClass& other){}

TextClass::~TextClass(){}


bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	bool result;

	// ȭ���� ����� �Է¹޴´�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// �⺻ �� ����� �����Ѵ�. �ֳ��ϸ� �ؽ�Ʈ�� ������ ī�޶�� �ٸ� ������ ī�޶�� ȭ�鿡 �׷������ϹǷ�
	m_baseViewMatrix = baseViewMatrix;

	// �۲� ��ü�� ����
	m_Font = new FontClass;
	if (!m_Font) {
		return false;
	}

	// �۲� ��ü �ʱ�ȭ
	result = m_Font->Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// �۲� ���̴� ��ü ����
	m_FontShader = new FontShaderClass;
	if (!m_FontShader) {
		return false;
	}

	// �۲� ���̴� ��ü �ʱ�ȭ
	result = m_FontShader->Initialize(device, hwnd);
	if (!result) { 
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK); 
		return false; 
	}

	// ù��° ���� �ʱ�ȭ 
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result){
		return false;
	}

	// ���ڿ��̳� ��ġ, ������� �ٲ�� ������ UpdateSentence�Լ��� ������� ����
	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result){
		return false;
	}

	// �ι�° ���� �ʱ�ȭ
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result){
		return false;
	}

	// ���ڿ��̳� ��ġ, ������� �ٲ�� ������ UpdateSentence�Լ��� ������� ����
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result){
		return false;
	}

	return true;
}

// Shutdown �Լ��� �� �����, �۲� ��ü, �׸��� �۲� ���̴��� �����Ѵ�.
void TextClass::Shutdown()
{
	// ù��° ���ڿ� ��ȯ
	ReleaseSentence(&m_sentence1);

	// �ι�° ���ڿ� ��ȯ
	ReleaseSentence(&m_sentence2);

	// FontShader ��ü ��ȯ
	if (m_FontShader){
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Font ��ü ��ȯ
	if (m_Font){
		m_Font->Shutdown();
		delete m_Font;
	}

	return;
}

// Render �Լ��� �� ������ ȭ�鿡 �׸���.
bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix){
	bool result;

	// ù��° ������ �׸���.
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result){
		return false;
	}

	// �ι�° ������ �׸���.
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result){
		return false;
	}

	return true;
}

// InitializeSentence �Լ��� ������ �����ϰ� �׸��� ���� �� �������۷� �̷���� SentenceType �� ������ �����Ѵ�.
// maxLength���ڴ� �������۰� �󸶳� Ŀ�� �ϴ��� �����Ѵ�.
// ��� ������ ���õ� ����/�ε��� ���۸� ������ ���۵��� �� �Լ��� �պκп��� �ʱ�ȭ�ȴ�..
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// ù��° �������۸� �����Ѵ�.
	*sentence = new SentenceType;
	if (!*sentence){
		return false;
	}

	// �������۸� Null�� �ʱ�ȭ
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// �ִ���� ����
	(*sentence)->maxLength = maxLength;

	// �ִ� ������ ���� 6�� �����ִ� ������ �����ϳ��� 6���� ������ ���� �簢���� ����� ������
	(*sentence)->vertexCount = 6 * maxLength;

	// �ε��������� ���� ����
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// �����迭 ����
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices){
		return false;
	}

	// �ε����迭 ����
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices){
		return false;
	}

	// �����迭�� 0 ������ �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// �ε����迭 �ʱ�ȭ 
	for (i = 0; i < (*sentence)->indexCount; i++){
		indices[i] = i;
	}

	// ������ ������ ������ �ٲ� �� �ֵ��� ���� ������ description�� �ۼ��� �� Usage �׸��� dynamic���� ����.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� �������ҽ� ����ü description �ۼ�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������� ���� ���� �ʱ�ȭ�� x
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result)){
		return false;
	}

	// �ε��� ���۴� �� ������ �ٲ��� �ʱ� ������ �Ϲ����� ���� ���۷� ����
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� �������ҽ� ����ü description �ۼ�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε������� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result)){
		return false;
	}

	// �����迭 ��ȯ
	delete[] vertices;
	vertices = 0;

	// �ε����迭 ��ȯ
	delete[] indices;
	indices = 0;

	return true;
}

// UpdateSentence �Լ��� �Էµ� ���忡 �°� ���������� ������ �ٲ۴�. Map �Լ��� UnMap�Լ�, memcpy �Լ��� �̿��� �������۸� �����Ѵ�.
bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext) {

	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// ������ ������ ���Ѵ�.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// ������ ���̸� ���Ѵ�.
	numLetters = (int)strlen(text);

	// ���� ���̸� �Ѿ�� ���ڿ��̸� ��ȯ
	if (numLetters > sentence->maxLength){
		return false;
	}

	// �ӽ� �����迭�� �����Ѵ�.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices){
		return false;
	}

	// �����迭�� 0 ������ �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// ȭ��� ���ڰ� ���۵� ��ġ�� ����Ѵ�.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// FontClass ���� ������ �̿��Ͽ� �����迭�� ���� ����ִ´�.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// �������۸� �����ϱ����� ��ٴ�.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// �������۸� �����ϴ� �����ͻ���
	verticesPtr = (VertexType*)mappedResource.pData;

	// �����迭�� ������ �������ۿ� �����Ѵ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// �������۸� ��������Ѵ�.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// �����迭�� ��ȯ�Ѵ�.
	delete[] vertices;
	vertices = 0;

	return true;
}

// ReleaseSentence �Լ��� ������ ���� / �ε��� ���ۿ� ���� ��ü�� �Ҵ� �����Ѵ�.
void TextClass::ReleaseSentence(SentenceType * *sentence){
	if (*sentence){
		// �������۸� ��ȯ�Ѵ�.
		if ((*sentence)->vertexBuffer){
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// �ε������۸� ��ȯ�Ѵ�.
		if ((*sentence)->indexBuffer){
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// ������ ����Ű�� �����͸� ��ȯ�Ѵ�.
		delete* sentence;
		*sentence = 0;
	}

	return;
}

// RenderSentence �Լ��� ������ ���� / �ε��� ���۸� �Է� ������� �ְ� FontShaderClass�� ȣ���Ͽ� �Էµ� ���۵��� �׸��� �Ѵ�.
// m_baseViewMatrix�� �ƴ϶� ������ �� ����� ����Ѵٴ� ���� ��������
// �̸� ���� ���� ī�޶� ��� �ִ����� ������� ȭ���� ���� ��ġ�� ���ڰ� �׷����� �ȴ�.
// ���������� 2D ��ǥ�� ǥ���ϴ� ���̱� ������ ���� ���� ��� ��� ���翵 ����� ����մϴ�.
bool TextClass::RenderSentence(ID3D11DeviceContext * deviceContext, SentenceType * sentence, XMMATRIX worldMatrix,
	XMMATRIX orthoMatrix){
	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),pixelColor);
	if (!result){
		false;
	}

	return true;
}

// TextClass�� ���콺�� x, y��ǥ�� �� ���ڿ��� ��ȯ�ϰ� �� ������� �����Ͽ� �� ��ġ�� ȭ�鿡 �׷��� �� �ְ� �Ѵ�.
bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;

	// mouseX���� string Ÿ������ �ٲ�
	_itoa_s(mouseX, tempString, 10);

	// �Է¹��ڿ� ����
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// �Է¹��ڿ� ���
	result = UpdateSentence(m_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result){
		return false;
	}

	// mouseY���� string Ÿ������ �ٲ�
	_itoa_s(mouseY, tempString, 10);

	// �Է¹��ڿ� ����
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// �Է¹��ڿ� ���
	result = UpdateSentence(m_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result){
		return false;
	}

	return true;
}
