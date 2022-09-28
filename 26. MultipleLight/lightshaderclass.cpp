#include "LightShaderClass.h"


LightShaderClass::LightShaderClass(){
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightColorBuffer = 0;
	m_lightPositionBuffer = 0;
}


LightShaderClass::LightShaderClass(const LightShaderClass& other){}


LightShaderClass::~LightShaderClass(){}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result;

	//���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, L"../Engine/light.vs", L"../Engine/light.ps");
	if (!result) {
		return false;
	}
	return true;
}

void LightShaderClass::Shutdown() {
	// ���� ���̴��� �ȼ� ���̴� �� �׿� ���õ� �͵��� ��ȯ�Ѵ�.
	ShutdownShader();

	return;
}

// Render �Լ������� SetShaderParameters �Լ��� ����� ���̴����� ���� ���ڵ��� �����Ѵ�.
// ���ڵ��� �����Ǹ� RenderShader �Լ��� ȣ���� Ȱ��ȭ�� �������۵��� ������ ���̴��� �̿��ؼ� �׸��⸦ �����Ѵ�.
// Render �Լ����� �ؽ�ó ���÷� ������ �߰��Ͽ� �ȼ����̴��� ���������� �߰��� ���̴�. 
// Render �Լ��� SetShaderParameters���� ���� ����� ���� ������ ���̴��� ������۷� ���̴��� ������ �������� �ʱ�ȭ �ؾ��Ѵ�.
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColor[], XMFLOAT4 lightPosition[]) {

	bool result;

	// �������� ����� ���̴��� ���ڸ� �Է��Ѵ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, diffuseColor, lightPosition);
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
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	// ���̴� �ʱ�ȭ ������ ���̴� ������ -> ���̴� ���� -> ���̴����� ����� ���� ���� ����ü�� description�� ���� �Ѵ� -> ����ü�� �Է� ���̾ƿ��� ����־�´�.
	// �Է� ���̾ƿ�(�Է� ��ġ)�� �ְԵǸ� Direct3D���� ���� ���� ������ �˷��ش�.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightColorBufferDesc; 
	D3D11_BUFFER_DESC lightPositionBufferDesc;


	// �� �Լ����� ����ϴ� �����͵��� null�� �����Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
	// ���� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vs_main", "vs_5_0",0, 0,&vertexShaderBuffer, &errorMessage);
	if (FAILED(result)){
		// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage){
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���Ѵ�.
		else{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ps_main", "ps_5_0",0, 0 ,&pixelShaderBuffer, &errorMessage);
	if (FAILED(result)){
		// ���̴� �������� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage){
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޼����� ���ٸ� �ܼ��� ���̴� ������ ã�� ���Ѵ�.
		else{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ���� ���̴��� �ȼ� ���̴��� ���۷� �� �����ϵǸ� �̸� �̿��Ͽ� ���̴� ��ü�� ���� �� �ִ�.
	// ���⼭ ���� �����͸� ���� ���̴��� �ȼ� ���̴��� �������̽��μ� ����Ѵ�.

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)){
		return false;
	}

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)){
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)){
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
	if (FAILED(result)){
		return false;
	}

	//�� ���� ���� ������� ����
	lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
	lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColorBufferDesc.MiscFlags = 0;
	lightColorBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightColorBufferDesc, NULL, &m_lightColorBuffer);
	if (FAILED(result)) {
		return false;
	}


	//�� ���� ��ġ ������� ����
	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;
	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
	if (FAILED(result)) {
		return false;
	}

	// samplerDesc ����ü�� description�� �ۼ��Ѵ�.
	// samplerDesc ����ü���� ���� �߿��� ���� Filter�� Filter�� ���� ���� ǥ�鿡�� �ؽ�ó�� ��� �ȼ��� ���ǰų� ȥ�յ� ������ �����ϴ� ����̴�.
	// ���� ��� ó���� �鿡���� �������� ���� ���� ����� �����ִ�  D3D11_FILTER_MIN_MAG_MIP_LINEAR ����Ѵ�.

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

	// �ؽ�ó���÷��� �����Ѵ�. 
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)){
		return false;
	}

	return true;
}

//ShutdownShader �Լ������� InitializeShader �Լ����� �����ߴ� 4���� �������̽����� �����Ѵ�.
void LightShaderClass::ShutdownShader(){
	// ��� ���۸� �����Ѵ�.
	if (m_matrixBuffer){
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_lightColorBuffer) {
		m_lightColorBuffer->Release();
		m_lightColorBuffer = 0;
	}

	if (m_lightPositionBuffer) {
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = 0;
	}

	// ���̾ƿ��� �����Ѵ�.
	if (m_layout){
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴��� �����Ѵ�.
	if (m_pixelShader){
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ���� ���̴��� �����Ѵ�.
	if (m_vertexShader){
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	// �ؽ�ó ���÷��� �����Ѵ�.
	if(m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0; 
	}
	return;
}

//OutputShaderErrorMessage �Լ������� ���� / �ȼ� ���̴��� ������ ���� ������ ���� ������� ���� �޼����� ����ϴ� ���� �Ѵ�.
void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
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
	for (i = 0; i < bufferSize; i++){
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
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColor[], XMFLOAT4 lightPosition[]){
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	MatrixBufferType* dataPtr;
	LightColorBufferType* dataPtr2;
	LightPositionBufferType* dataPtr3;
	unsigned int bufferNumber;


	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// ������ ��ġ ������۵� ���� ��İ� �Ȱ��� �����Ѵ�.
	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}


	dataPtr3 = (LightPositionBufferType*)mappedResource.pData;

	dataPtr3->lightPosition[0] = lightPosition[0];
	dataPtr3->lightPosition[1] = lightPosition[1];
	dataPtr3->lightPosition[2] = lightPosition[2];
	dataPtr3->lightPosition[3] = lightPosition[3];

	deviceContext->Unmap(m_lightPositionBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

	// �� ���� ���������۵� ���� ��İ� �Ȱ��� �����Ѵ�.
	result = deviceContext->Map(m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}


	dataPtr2 = (LightColorBufferType*)mappedResource.pData;

	dataPtr2->diffuseColor[0] = diffuseColor[0];
	dataPtr2->diffuseColor[1] = diffuseColor[1];
	dataPtr2->diffuseColor[2] = diffuseColor[2];
	dataPtr2->diffuseColor[3] = diffuseColor[3];

	deviceContext->Unmap(m_lightColorBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightColorBuffer);

	// �� �ؽ�ó�� �ȼ� ���̴��� ����ִ´�.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

// ���̴� �������� ������ ������ �ǰ� ����, RenderShader �Լ������� �Է� ���̾ƿ��� �Է� ������� �����ϴ� ���̴�. �̷ν� GPU ���� ������ �ڷᱸ���� �˰� �ȴ�.
// �� �������۸� �׸��� ���� �������̴��� �ȼ� ���̴��� �����ϴ� ���̴�. ���̴��� �����ǰ� ���� Direct3D ��ġ ���ؽ�Ʈ���� DirectX11 DrawIndexed �Լ��� �̿��� �ﰢ���� �׷�����.
void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// ���⼭ �ȼ� ���̴��� ���÷� ���¸� �����ϴ� �κ��� �߰��Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

