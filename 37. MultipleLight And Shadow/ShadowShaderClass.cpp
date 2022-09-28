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

	//���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, L"../Engine/shadow.vs", L"../Engine/shadow.ps");
	if (!result) {
		return false;
	}
	return true;
}

void ShadowShaderClass::Shutdown() {
	// ���� ���̴��� �ȼ� ���̴� �� �׿� ���õ� �͵��� ��ȯ�Ѵ�.
	ShutdownShader();

	return;
}

// Render �Լ������� SetShaderParameters �Լ��� ����� ���̴����� ���� ���ڵ��� �����Ѵ�.
// ���ڵ��� �����Ǹ� RenderShader �Լ��� ȣ���� Ȱ��ȭ�� �������۵��� ������ ���̴��� �̿��ؼ� �׸��⸦ �����Ѵ�.
// Render �Լ����� �ؽ�ó ���÷� ������ �߰��Ͽ� �ȼ����̴��� ���������� �߰��� ���̴�. 
bool ShadowShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightViewProjectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMMATRIX lightViewMatrix2, XMMATRIX lightViewProjectionMatrix2, ID3D11ShaderResourceView* depthMapTexture2, XMFLOAT3 lightPosition2, XMFLOAT4 diffuseColor2) {

	bool result;

	// �������� ����� ���̴��� ���ڸ� �Է��Ѵ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightViewProjectionMatrix, texture, depthMapTexture, lightPosition, ambientColor, diffuseColor, lightViewMatrix2, lightViewProjectionMatrix2, depthMapTexture2, lightPosition2, diffuseColor2);
	if (!result) {
		return false;
	}

	// ���̴��� ���ڸ� �� �޾ƿԴٸ� ���̴� �̿��Ͽ� Ȱ��ȭ�� ���۸� �׸���.
	RenderShader(deviceContext, indexCount);

	return true;
}


// ���� �߿��� InitializeShader �Լ��� ����Ͽ� Shader�� �ʱ�ȭ��Ų��.
// �� �Լ������� ���� ���̴� ������ �ҷ����� DIrectX�� GPU���� ��� �����ϵ��� ���ش�.
// ���� ���̾ƿ��� �����ϰ� ��F�� ���������� �����Ͱ� GPU���� ���Ǵ��� �� �� �ִ�.
bool ShadowShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {

	// ���̴� �ʱ�ȭ ������ ���̴� ������ -> ���̴� ���� -> ���̴����� ����� ���� ���� ����ü�� description�� ���� �Ѵ� -> ����ü�� �Է� ���̾ƿ��� ����־�´�.
	// �Է� ���̾ƿ�(�Է� ��ġ)�� �ְԵǸ� Direct3D���� ���� ���� ������ �˷��ش�.

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

	// �� �Լ����� ����ϴ� �����͵��� null�� �����Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vs_main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���Ѵ�.
		else {
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ps_main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// ���̴� �������� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޼����� ���ٸ� �ܼ��� ���̴� ������ ã�� ���Ѵ�.
		else {
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ���� ���̴��� �ȼ� ���̴��� ���۷� �� �����ϵǸ� �̸� �̿��Ͽ� ���̴� ��ü�� ���� �� �ִ�.
	// ���⼭ ���� �����͸� ���� ���̴��� �ȼ� ���̴��� �������̽��μ� ����Ѵ�.

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// ���� �Է� ���̾ƿ� description�� �ۼ��Ѵ�.
	// �� ������ ModelClass�� ���̴��� �ִ� VertexType�� ��ġ�ؾ� �Ѵ�.
	// �Է� ���̾ƿ��� �ι�° ���� ���е� �ٲ۴�. 
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


	// ���̾ƿ��� ��� ������ �����´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���̴��� ����ϱ� ���� ������ �ܰ�� ��� ���۸�  �����ϴ� ���̴�.
	// ���� ���� ���̴����� �ϳ��� ������۸� ������ �ֱ� ������ �װ��� �����Ͽ� ���̴��� ���� �������̽����� ����� �� �ִ�.
	// �� �� �����Ӹ��� ������۸� ������Ʈ�ϱ� ������ ������ ����� ������ �ʿ䰡 �ִ�.
	// �ٷ� BindFlags �÷��׷� ������۸� �� ���۷� �Ѵٴ°��� �����Ѵ�.
	// CPUAccessFlags�� �뵵�� ����� �ϱ� �빮�� D3D11_CPU_ACCESS_WRITE�� �����Ѵ�.
	// descrition�� ä������ ���� ��� ������ �������̽��� ����� SetShaderParameters �Լ��� �̿��Ͽ� ���̴��� ���� �����鿡 ������ �� �ִ�.

	// ���� ���̴��� �ִ� ��� ��� ������ description�� �ۼ��Ѵ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	// ���� ��� ���� 1 ����
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

	// ���� ��� ���� 2 ����
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

	// ���� �ؽ�ó ���÷� ���� ��ũ���� �ۼ�

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

	// �ؽ�ó���÷�1 �� �����Ѵ�. 
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result)) {
		return false;
	}

	// clamp �ؽ�ó ���÷� ���� �彺ũ������ �ۼ��Ѵ�.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

//ShutdownShader �Լ������� InitializeShader �Լ����� �����ߴ� 4���� �������̽����� �����Ѵ�.
void ShadowShaderClass::ShutdownShader() {
	// ��� ���۸� �����Ѵ�.
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

	// ���̾ƿ��� �����Ѵ�.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴��� �����Ѵ�.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ���� ���̴��� �����Ѵ�.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	// �ؽ�ó ���÷��� �����Ѵ�.
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

//OutputShaderErrorMessage �Լ������� ���� / �ȼ� ���̴��� ������ ���� ������ ���� ������� ���� �޼����� ����ϴ� ���� �Ѵ�.
void ShadowShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// ���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� �����´�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޼����� ���̸� �����´�.
	bufferSize = errorMessage->GetBufferSize();

	// ������ ���� �ȿ� �޼����� ����Ѵ�.
	fout.open("shader-error.txt");

	// ���� �޼����� ����.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	// ������ �ݴ´�.
	fout.close();

	// ���� �޼����� ��ȯ�Ѵ�.
	errorMessage->Release();
	errorMessage = 0;

	// ������ ������ ������ �˾� �޼����� �˷��ش�.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


// SetShaderVariables �Լ��� ���̴��� ���������� ���� �ٷ� �� �ְ� �ϱ� ���� ���������.
// �� �Լ��� ���� ��ĵ��� GraphicsClass���� ������� ���̴�.
// SetShaderParameters �Լ��� �ؽ�ó �ڿ��� �����͸� ���ڷ� �߰��ϰ� �̸� ������ �ؽ�ó ���÷��� �ʱ�ȭ�Ѵ�.
bool ShadowShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightViewProjectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMMATRIX lightViewMatrix2, XMMATRIX lightViewProjectionMatrix2, ID3D11ShaderResourceView* depthMapTexture2, XMFLOAT3 lightPosition2, XMFLOAT4 diffuseColor2) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType1* dataPtr2;
	LightBufferType2* dataPtr3;
	unsigned int bufferNumber;


	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	lightViewProjectionMatrix = XMMatrixTranspose(lightViewProjectionMatrix);
	lightViewMatrix2= XMMatrixTranspose(lightViewMatrix2);
	lightViewProjectionMatrix2 = XMMatrixTranspose(lightViewProjectionMatrix2);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightViewProjectionMatrix;
	dataPtr->lightView2 = lightViewMatrix2;
	dataPtr->lightProjection2 = lightViewProjectionMatrix2;

	// ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// ���� ��� ����2 �ʱ�ȭ
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


	// ���� ��� ����2 �ʱ�ȭ
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

	// �� �ؽ�ó�� �ȼ� ���̴��� ����ִ´�. �̶� �ι�° �μ��� �� �ؽ�ó�� ������.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMapTexture);
	deviceContext->PSSetShaderResources(2, 1, &depthMapTexture2);

	return true;
}

// ���̴� �������� ������ ������ �ǰ� ����, RenderShader �Լ������� �Է� ���̾ƿ��� �Է� ������� �����ϴ� ���̴�. �̷ν� GPU ���� ������ �ڷᱸ���� �˰� �ȴ�.
// �� �������۸� �׸��� ���� �������̴��� �ȼ� ���̴��� �����ϴ� ���̴�. ���̴��� �����ǰ� ���� Direct3D ��ġ ���ؽ�Ʈ���� DirectX11 DrawIndexed �Լ��� �̿��� �ﰢ���� �׷�����.
void ShadowShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// ���⼭ �ȼ� ���̴��� ���÷� ���¸� �����ϴ� �κ��� �߰��Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

