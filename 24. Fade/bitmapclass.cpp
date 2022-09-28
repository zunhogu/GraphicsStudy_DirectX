#include "bitmapclass.h"

BitmapClass::BitmapClass(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

BitmapClass::BitmapClass(const BitmapClass& other){}

BitmapClass::~BitmapClass(){}

// Initialize 함수는 화면크기와 이미지 크기를 저장한다. 렌더링 할 정확한 정점 위치가 필요하기 때문이다.
bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight){
	bool result;

	// 화면의 사이즈를 입력받는다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 이미지의 사이즈를 입력받는다.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 렌더링 위치 변수를 -1로 초기화한다. 이 변수는 이전 위치를 기억해두는 용도다
	// 만약 이전 프레임과 비교하여 위치가 변하지 않았다면 동적 정점 버퍼를 바꾸지 않기 때문에 성능향상을 꾀할 수 있다.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// 버퍼를 생성한다.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown(){
	// 버퍼를 반환한다.
	ShutdownBuffers();

	return;
}

// Render 함수는 입력으로 화면에 그려질 위치를 받는다. 이는 UpdateBuffers 함수가 위치를 지정하는데 사용한다.
// 이전 프레임과 위치가 바귀면 동적 정점 버퍼의 정점들의 위치를 새로운 위치로 갱신한다. 그렇지 않으면 UpdateBuffer 함수를 지나친다.
// RenderBuffers 함수를 수행한뒤 정점/인덱스 버퍼를 어셈블리 버퍼로 변환한다.
bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY){
	bool result;

	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	RenderBuffers(deviceContext);

	return true;
}

// GetIndexCount 함수는 2D 이미지의 정점 수를 리턴한다. 언제나 6을 반환한다.
int BitmapClass::GetIndexCount(){
	return m_indexCount;
}

// InitializeBuffers 함수는 2D 이미지를 그리는데 사용하는 정점/인덱스 버퍼를 만드는데 사용한다. 
bool BitmapClass::InitializeBuffers(ID3D11Device* device){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// 두개의 삼각혀을 만들기 위해서는 정점 6개가 필요하다.
	m_vertexCount = 6;

	// 정점개수에 따른 인덱스의 개수도 초기화한다.
	m_indexCount = m_vertexCount;

	// 정점배열 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}

	// 인덱스 배열생성
	indices = new unsigned long[m_indexCount];
	if (!indices){
		return false;
	}

	// 정점배열의 크기만큼 메모리값을 세팅한다. 그 이유는 초기화시키지않고 정점버퍼를 만들기위해서는 메모리공간이 할당되어야한다. 
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열 초기화
	for (i = 0; i < m_indexCount; i++){
		indices[i] = i;
	}

	// 여기부터 ModelClass와 큰 차이가 생긴다.
	// 정점버퍼 description에서 동적 정점버퍼를 생성하여 필요하다면 정점 버퍼 내부값을 수정할 수 있게 한다.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;   // 동적이라는 속성 부여를 위해 다음과 같이 설정한다.
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   // 이 설정도 수정을 위해 다음과 같은 Flag를 설정한다. 
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 구조체를 서술한다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼의 내용은 정점의 위치가 바뀌더라도 언제나 여섯개의 정점을 가리키기 때문에 인덱스버퍼는 동적 버퍼로 만들지 않는다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)){
		return false;
	}

	// 인덱스버퍼의 description 작성해준다.
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
	if (FAILED(result)){
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
void BitmapClass::ShutdownBuffers(){
	// Release the index buffer.
	if (m_indexBuffer){
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer){
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

// UpdateBuffers 함수는 매 프레임마다 불려 필요할 경우 동적 정점버퍼의 내용을 새 위치로 변경한다.
bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY){
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// 이미지 위치가 이전과 비교하여 달라졌는지 확인한다. 바뀌지 않았다면 버퍼를 수정할 필요가 없으므로 그냥 빠져나간다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY)){
		return true;
	}

	// 만약 이미지 위치가 바뀌었다면 다음번에 그려질 위치를 위해 새로운 위치를 기록한다.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 이미지의 네변의 위치가 계산된다. 

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// 좌표가 계산되면 임시로 정점배열을 만들고 새로운 정점 위치를 채워넣는다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}

	// 첫번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 정점버퍼의 내용을 쓸수 있도록 잠근다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// 정점버퍼의 데이터에 대한 포인터를 가져온다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 임시 정점배열의 내용을 메모리에 미리 할당된 정점버퍼에 복사한다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼를 잠근다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 정점배열을 반환한다.
	delete[] vertices;
	vertices = 0;

	return true;
}

// RenderBuffers 함수는 이전의 ModelClass 처럼 정점/인덱스 버퍼를 gpu 어셈블리 버퍼로 변환시킨다.
void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext){
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