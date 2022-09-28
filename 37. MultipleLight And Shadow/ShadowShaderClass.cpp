#include "ShadowShaderClass.h"


ShadowShaderClass::ShadowShaderClass() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_lightBuffer1 = 0;
	m_lightBuffer2 = 0;
	m_sampleStateWrap = 0;
	m_sampleStateClamp = 0;
}


ShadowShaderClass::ShadowShaderClass(const ShadowShaderClass& other) {}


ShadowShaderClass::~ShadowShaderClass() {}

bool ShadowShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result;

	//정점 셰이더와 픽셀 셰이더를 초기화한다.
	result = InitializeShader(device, hwnd, L"../Engine/shadow.vs", L"../Engine/shadow.ps");
	if (!result) {
		return false;
	}
	return true;
}

void ShadowShaderClass::Shutdown() {
	// 정점 셰이더와 픽셀 셰이더 및 그와 관련된 것들을 반환한다.
	ShutdownShader();

	return;
}

// Render 함수에서는 SetShaderParameters 함수를 사용해 셰이더에서 사용될 인자들을 설정한다.
// 인자들이 설정되면 RenderShader 함수를 호출해 활성화된 정점버퍼들을 가지고 셰이더를 이용해서 그리기를 수행한다.
// Render 함수에는 텍스처 샘플러 변수를 추가하여 픽셀셰이더의 전역변수에 추가할 것이다. 
bool ShadowShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightViewProjectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMMATRIX lightViewMatrix2, XMMATRIX lightViewProjectionMatrix2, ID3D11ShaderResourceView* depthMapTexture2, XMFLOAT3 lightPosition2, XMFLOAT4 diffuseColor2) {

	bool result;

	// 렌더링에 사용할 셰이더의 인자를 입력한다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightViewProjectionMatrix, texture, depthMapTexture, lightPosition, ambientColor, diffuseColor, lightViewMatrix2, lightViewProjectionMatrix2, depthMapTexture2, lightPosition2, diffuseColor2);
	if (!result) {
		return false;
	}

	// 셰이더의 인자를 다 받아왔다면 셰이더 이용하여 활성화된 버퍼를 그린다.
	RenderShader(deviceContext, indexCount);

	return true;
}


// 가장 중요한 InitializeShader 함수를 사용하여 Shader를 초기화시킨다.
// 이 함수에서는 실제 셰이더 파일을 불러오고 DIrectX와 GPU에서 사용 가능하도록 해준다.
// 또한 레이아웃을 세팅하고 어덯게 정점버퍼의 데이터가 GPU에서 사용되는지 볼 수 있다.
bool ShadowShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {

	// 셰이더 초기화 순서는 셰이더 컴파일 -> 셰이더 생성 -> 셰이더에서 사용할 정점 성분 구조체를 description을 서술 한다 -> 구조체를 입력 레이아웃에 집어넣어는다.
	// 입력 레이아웃(입력 배치)에 넣게되면 Direct3D에게 정점 성분 정보를 알려준다.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc1;
	D3D11_BUFFER_DESC lightBufferDesc2;
	D3D11_SAMPLER_DESC samplerDesc;

	// 이 함수에서 사용하는 포인터들을 null로 설정한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 컴파일한다.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vs_main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// 셰이더가 컴파일에 실패하면 에러 메세지를 기록한다.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메세지가 없다면 셰이더 파일을 찾지 못한다.
		else {
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더를 컴파일한다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ps_main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// 셰이더 컴파일이 실패하면 에러 메세지를 기록한다.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 에러 메세지가 없다면 단순히 셰이더 파일을 찾지 못한다.
		else {
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 정점 셰이더와 픽셀 셰이더가 버퍼로 잘 컴파일되면 이를 이용하여 셰이더 객체를 만들 수 있다.
	// 여기서 나온 포인터를 정점 셰이더와 픽셀 셰이더의 인터페이스로서 사용한다.

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	// 버퍼로부터 픽셀 셰이더를 생성한다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// 정점 입력 레이아웃 description을 작성한다.
	// 이 설정은 ModelClass와 셰이더에 있는 VertexType와 일치해야 한다.
	// 입력 레이아웃의 두번째 정점 성분도 바꾼다. 
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;


	// 레이아웃의 요소 갯수를 가져온다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 셰이더를 사용하기 위한 마지막 단계는 상수 버퍼를  설정하는 것이다.
	// 현재 정점 셰이더에서 하나의 상수버퍼를 가지고 있기 때문에 그것을 세팅하여 셰이더에 대한 인터페이스에서 사용할 수 있다.
	// 단 매 프레임마다 상수버퍼를 업데이트하기 때문에 버퍼의 사용은 동적일 필요가 있다.
	// 바로 BindFlags 플래그로 상수버퍼를 이 버퍼로 한다는것을 설정한다.
	// CPUAccessFlags도 용도에 맞춰야 하기 대문에 D3D11_CPU_ACCESS_WRITE로 설정한다.
	// descrition이 채워지고 나면 상수 버퍼의 인터페이스를 만들고 SetShaderParameters 함수를 이용하여 셰이더의 내부 변수들에 접근할 수 있다.

	// 정점 셰이더에 있는 행렬 상수 버퍼의 description을 작성한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	// 조명 상수 버퍼 1 생성
	lightBufferDesc1.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc1.ByteWidth = sizeof(LightBufferType1);
	lightBufferDesc1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc1.MiscFlags = 0;
	lightBufferDesc1.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc1, NULL, &m_lightBuffer1);
	if (FAILED(result)) {
		return false;
	}

	// 조명 상수 버퍼 2 생성
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc2, NULL, &m_lightBuffer2);
	if (FAILED(result)) {
		return false;
	}

	// 래핑 텍스처 샘플러 상태 디스크립션 작성

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처샘플러1 를 생성한다. 
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result)) {
		return false;
	}

	// clamp 텍스처 샘플러 상태 드스크립션을 작성한다.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

//ShutdownShader 함수에서는 InitializeShader 함수에서 생성했던 4개의 인터페이스들을 정리한다.
void ShadowShaderClass::ShutdownShader() {
	// 상수 버퍼를 해제한다.
	if (m_lightBuffer2) {
		m_lightBuffer2->Release();
		m_lightBuffer2 = 0;
	}
	if (m_lightBuffer1) {
		m_lightBuffer1->Release();
		m_lightBuffer1 = 0;
	}
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_lightBuffer1) {
		m_lightBuffer1->Release();
		m_lightBuffer1 = 0;
	}

	// 레이아웃을 해제한다.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더를 해제한다.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 정점 셰이더를 해제한다.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	// 텍스처 샘플러를 해제한다.
	if (m_sampleStateClamp) {
		m_sampleStateClamp->Release();
		m_sampleStateClamp = 0;
	}
	if (m_sampleStateWrap) {
		m_sampleStateWrap->Release();
		m_sampleStateWrap = 0;
	}
	return;
}

//OutputShaderErrorMessage 함수에서는 정점 / 픽셀 셰이더의 컴파일 도중 오류로 인해 만들어진 에러 메세지를 출력하는 일을 한다.
void ShadowShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져온다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메세지의 길이를 가져온다.
	bufferSize = errorMessage->GetBufferSize();

	// 파일을 열고 안에 메세지를 기록한다.
	fout.open("shader-error.txt");

	// 에러 메세지를 쓴다.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	// 파일을 닫는다.
	fout.close();

	// 에러 메세지를 반환한다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려준다.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


// SetShaderVariables 함수는 셰이더의 전역변수를 쉽게 다룰 수 있게 하기 위해 만들어졌다.
// 이 함수에 사용된 행렬들은 GraphicsClass에서 만들어진 것이다.
// SetShaderParameters 함수에 텍스처 자원을 포인터를 인자로 추가하고 이를 가지고 텍스처 샘플러에 초기화한다.
bool ShadowShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightViewProjectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMMATRIX lightViewMatrix2, XMMATRIX lightViewProjectionMatrix2, ID3D11ShaderResourceView* depthMapTexture2, XMFLOAT3 lightPosition2, XMFLOAT4 diffuseColor2) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType1* dataPtr2;
	LightBufferType2* dataPtr3;
	unsigned int bufferNumber;


	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	lightViewProjectionMatrix = XMMatrixTranspose(lightViewProjectionMatrix);
	lightViewMatrix2= XMMatrixTranspose(lightViewMatrix2);
	lightViewProjectionMatrix2 = XMMatrixTranspose(lightViewProjectionMatrix2);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightViewProjectionMatrix;
	dataPtr->lightView2 = lightViewMatrix2;
	dataPtr->lightProjection2 = lightViewProjectionMatrix2;

	// 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 조명 상수 버퍼2 초기화
	result = deviceContext->Map(m_lightBuffer1, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	dataPtr2 = (LightBufferType1*)mappedResource.pData;

	dataPtr2->lightPosition = lightPosition;
	dataPtr2->padding = 0.0f;
	dataPtr2->lightPosition2 = lightPosition2;
	dataPtr2->padding2 = 0.0f;

	deviceContext->Unmap(m_lightBuffer1, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer1);


	// 조명 상수 버퍼2 초기화
	result = deviceContext->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	dataPtr3 = (LightBufferType2*)mappedResource.pData;

	dataPtr3->ambientColor = ambientColor;
	dataPtr3->diffuseColor = diffuseColor;
	dataPtr3->diffuseColor2 = diffuseColor2;

	deviceContext->Unmap(m_lightBuffer2, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer2);

	// 또 텍스처를 픽셀 셰이더에 집어넣는다. 이때 두번째 인수는 들어갈 텍스처의 개수다.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMapTexture);
	deviceContext->PSSetShaderResources(2, 1, &depthMapTexture2);

	return true;
}

// 셰이더 전역변수 설정이 마무리 되고 나면, RenderShader 함수에서는 입력 레이아웃을 입력 어셈블러에 연결하는 것이다. 이로써 GPU 정점 버퍼의 자료구조를 알게 된다.
// 또 정점버퍼를 그리기 위한 정점셰이더와 픽셀 셰이더를 설정하는 것이다. 셰이더가 설정되고 나면 Direct3D 장치 컨텍스트에서 DirectX11 DrawIndexed 함수를 이용해 삼각형을 그려낸다.
void ShadowShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 여기서 픽셀 셰이더의 샘플러 상태를 설정하는 부분을 추가한다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	// 삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

