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

	// 화면의 사이즈를 입력받는다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 기본 뷰 행렬을 저장한다. 왜냐하면 텍스트는 기존의 카메라와 다른 고정된 카메라로 화면에 그려져야하므로
	m_baseViewMatrix = baseViewMatrix;

	// 글꼴 객체를 생성
	m_Font = new FontClass;
	if (!m_Font) {
		return false;
	}

	// 글꼴 객체 초기화
	result = m_Font->Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// 글꼴 셰이더 객체 생성
	m_FontShader = new FontShaderClass;
	if (!m_FontShader) {
		return false;
	}

	// 글꼴 셰이더 객체 초기화
	result = m_FontShader->Initialize(device, hwnd);
	if (!result) { 
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK); 
		return false; 
	}

	// 첫번째 문장 초기화 
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result){
		return false;
	}

	// 문자열이나 위치, 색상등이 바뀌면 언제든 UpdateSentence함수로 변경사항 저장
	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result){
		return false;
	}

	// 두번째 문장 초기화
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result){
		return false;
	}

	// 문자열이나 위치, 색상등이 바뀌면 언제든 UpdateSentence함수로 변경사항 저장
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result){
		return false;
	}

	return true;
}

// Shutdown 함수는 두 문장과, 글꼴 객체, 그리고 글꼴 셰이더를 해제한다.
void TextClass::Shutdown()
{
	// 첫번째 문자열 반환
	ReleaseSentence(&m_sentence1);

	// 두번째 문자열 반환
	ReleaseSentence(&m_sentence2);

	// FontShader 객체 반환
	if (m_FontShader){
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Font 객체 반환
	if (m_Font){
		m_Font->Shutdown();
		delete m_Font;
	}

	return;
}

// Render 함수는 두 문장을 화면에 그린다.
bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix){
	bool result;

	// 첫번째 문장을 그린다.
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result){
		return false;
	}

	// 두번째 문자을 그린다.
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result){
		return false;
	}

	return true;
}

// InitializeSentence 함수는 문장을 저장하고 그리기 위해 빈 정점버퍼로 이루어진 SentenceType 형 변수를 생성한다.
// maxLength인자는 정점버퍼가 얼마나 커야 하는지 결정한다.
// 모든 문장은 관련된 정점/인덱스 버퍼를 가지며 버퍼들은 이 함수의 앞부분에서 초기화된다..
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// 첫번째 정점버퍼를 생성한다.
	*sentence = new SentenceType;
	if (!*sentence){
		return false;
	}

	// 정점버퍼를 Null로 초기화
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// 최대길이 설정
	(*sentence)->maxLength = maxLength;

	// 최대 정점수 설정 6을 곱해주는 이유는 글자하나당 6개의 정점을 가져 사각형을 만들기 때문에
	(*sentence)->vertexCount = 6 * maxLength;

	// 인덱스버퍼의 길이 설정
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// 정점배열 생성
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices){
		return false;
	}

	// 인덱스배열 생성
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices){
		return false;
	}

	// 정점배열을 0 값으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// 인덱스배열 초기화 
	for (i = 0; i < (*sentence)->indexCount; i++){
		indices[i] = i;
	}

	// 문장의 내용을 언제든 바꿀 수 있도록 정점 버퍼의 description을 작성할 때 Usage 항목을 dynamic으로 설정.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조리소스 구조체 description 작성
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점버퍼 생성 아직 초기화는 x
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result)){
		return false;
	}

	// 인덱스 버퍼는 그 내용이 바뀌지 않기 때문에 일반적인 정적 버퍼로 생성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조리소스 구조체 description 작성
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result)){
		return false;
	}

	// 정점배열 반환
	delete[] vertices;
	vertices = 0;

	// 인덱스배열 반환
	delete[] indices;
	indices = 0;

	return true;
}

// UpdateSentence 함수는 입력된 문장에 맞게 정점버퍼의 내용을 바꾼다. Map 함수와 UnMap함수, memcpy 함수를 이용해 정점버퍼를 수정한다.
bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext) {

	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// 문장의 색상을 정한다.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// 문장의 길이를 정한다.
	numLetters = (int)strlen(text);

	// 버퍼 길이를 넘어서는 문자열이면 반환
	if (numLetters > sentence->maxLength){
		return false;
	}

	// 임시 정점배열을 생성한다.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices){
		return false;
	}

	// 정점배열을 0 값으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// 화면상 글자가 시작될 위치를 계산한다.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// FontClass 문장 정보를 이용하여 정점배열에 값을 집어넣는다.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// 정점버퍼를 수정하기위해 잠근다.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// 정점버퍼를 접근하는 포인터생성
	verticesPtr = (VertexType*)mappedResource.pData;

	// 정점배열의 내용을 정점버퍼에 복사한다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// 정점버퍼를 잠금해제한다.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// 정점배열을 반환한다.
	delete[] vertices;
	vertices = 0;

	return true;
}

// ReleaseSentence 함수는 문장의 정점 / 인덱스 버퍼와 문장 자체를 할당 해제한다.
void TextClass::ReleaseSentence(SentenceType * *sentence){
	if (*sentence){
		// 정점버퍼를 반환한다.
		if ((*sentence)->vertexBuffer){
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// 인덱스버퍼를 반환한다.
		if ((*sentence)->indexBuffer){
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// 문장을 가르키는 포인터를 반환한다.
		delete* sentence;
		*sentence = 0;
	}

	return;
}

// RenderSentence 함수는 문장의 정점 / 인덱스 버퍼를 입력 어셈블러에 넣고 FontShaderClass를 호출하여 입력된 버퍼들을 그리게 한다.
// m_baseViewMatrix가 아니라 현재의 뷰 행렬을 사용한다는 것을 주의하자
// 이를 통해 현재 카메라가 어디에 있는지에 상관없이 화면의 같은 위치에 글자가 그려지게 된다.
// 마찬가지로 2D 좌표에 표시하는 것이기 때문에 정규 투영 행렬 대신 정사영 행렬을 사용합니다.
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

// TextClass에 마우스의 x, y좌표를 두 문자열로 변환하고 각 문장들을 갱신하여 그 위치가 화면에 그려질 수 있게 한다.
bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;

	// mouseX값을 string 타입으로 바꿈
	_itoa_s(mouseX, tempString, 10);

	// 입력문자열 세팅
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// 입력문자열 출력
	result = UpdateSentence(m_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result){
		return false;
	}

	// mouseY값을 string 타입으로 바꿈
	_itoa_s(mouseY, tempString, 10);

	// 입력문자열 세팅
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// 입력문자열 출력
	result = UpdateSentence(m_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result){
		return false;
	}

	return true;
}
