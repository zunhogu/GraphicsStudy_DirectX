#include "colorshaderclass.h"


ColorShaderClass::ColorShaderClass(){
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_hullShader = 0;
	m_domainShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_tessellationBuffer = 0;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other){}


ColorShaderClass::~ColorShaderClass(){}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result;

	//���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, L"../Engine/color.vs", L"../Engine/color.hs", L"../Engine/color.ds", L"../Engine/color.ps");
	if (!result) {
		return false;
	}
	return true;
}

void ColorShaderClass::Shutdown() {
	// ���� ���̴��� �ȼ� ���̴� �� �׿� ���õ� �͵��� ��ȯ�Ѵ�.
	ShutdownShader();

	return;
}

// Render �Լ������� SetShaderParameters �Լ��� ����� ���̴����� ���� ���ڵ��� �����Ѵ�.
// ���ڵ��� �����Ǹ� RenderShader �Լ��� ȣ���� Ȱ��ȭ�� �������۵��� ������ ���̴��� �̿��ؼ� �׸��⸦ �����Ѵ�.
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount) {

	bool result;

	// �������� ����� ���̴��� ���ڸ� �Է��Ѵ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount);
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
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename) {

	// ���̴� �ʱ�ȭ ������ ���̴� ������ -> ���̴� ���� -> ���̴����� ����� ���� ���� ����ü�� description�� ���� �Ѵ� -> ����ü�� �Է� ���̾ƿ��� ����־�´�.
	// �Է� ���̾ƿ�(�Է� ��ġ)�� �ְԵǸ� Direct3D���� ���� ���� ������ �˷��ش�.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* hullShaderBuffer; 
	ID3D10Blob* domainShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;

	// �� �Լ����� ����ϴ� �����͵��� null�� �����Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	hullShaderBuffer = 0;
	domainShaderBuffer = 0;
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

	// �� ���̴� �������Ѵ�.
	result = D3DCompileFromFile(hsFilename, NULL, NULL, "hs_main", "hs_5_0", 0, 0, &hullShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���Ѵ�.
		else {
			MessageBox(hwnd, hsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ������ ���̴� �������Ѵ�.
	result = D3DCompileFromFile(dsFilename, NULL, NULL, "ds_main", "ds_5_0", 0, 0, &domainShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���Ѵ�.
		else {
			MessageBox(hwnd, dsFilename, L"Missing Shader File", MB_OK);
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

	// ���۷κ��� �� ���̴��� �����Ѵ�.
	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result)) {
		return false;
	}

	// ���۷κ��� �����μ��̴��� �����Ѵ�.
	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result)) {
		return false;
	}

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)){
		return false;
	}

	// ���� �Է� ���̾ƿ� description�� �ۼ��Ѵ�.
	// �� ������ ModelClass�� ���̴��� �ִ� VertexType�� ��ġ�ؾ� �Ѵ�.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	// ���̾ƿ��� ��� ������ �����´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)){
		return false;
	}

	// �� �̻� ������ �ʴ� ���̴� ���۴� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	hullShaderBuffer->Release(); 
	hullShaderBuffer = 0; 
	
	domainShaderBuffer->Release();
	domainShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���̴��� ����ϱ� ���� ������ �ܰ�� ��� ���۸�  �����ϴ� ���̴�.
	// ���� ���� ���̴����� �ϳ��� ������۸� ������ �ֱ� ������ �װ��� �����Ͽ� ���̴��� ���� �������̽����� ����� �� �ִ�.
	// �� �� �����Ӹ��� ������۸� ������Ʈ�ϱ� ������ ������ ����� ������ �ʿ䰡 �ִ�.
	// �ٷ� BindFlags �÷��׷� ������۸� �� ���۷� �Ѵٴ°��� �����Ѵ�.
	// CPUAccessFlags�� �뵵�� ����� �ϱ� �빮�� D3D11_CPU_ACCESS_WRITE�� �����Ѵ�.
	// descrition�� ä������ ���� ��� ������ �������̽��� ����� SetShaderParameters �Լ��� �̿��Ͽ� ���̴��� ���� �����鿡 ������ �� �ִ�.

	// ������ ���̴����� ����ϴ� ��� ��� ������ description�� �ۼ��Ѵ�.
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

	// �� ���̴����� ����ϴ� ������� description�� �ۼ��Ѵ�.
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	// ��� ������ �����͸� �����Ͽ� �� Ŭ�������� �� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	result = device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer);
	if (FAILED(result)){
		return false;
	}

	return true;
}

//ShutdownShader �Լ������� InitializeShader �Լ����� �����ߴ� 4���� �������̽����� �����Ѵ�.
void ColorShaderClass::ShutdownShader(){
	// ��� ���۸� �����Ѵ�.
	if (m_matrixBuffer){
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	
	if (m_tessellationBuffer) {
		m_tessellationBuffer->Release();
		m_tessellationBuffer = 0;
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

	// ������ ���̴��� �����Ѵ�.
	if (m_domainShader) {
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// �� ���̴��� �����Ѵ�.
	if (m_hullShader) {
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// ���� ���̴��� �����Ѵ�.
	if (m_vertexShader){
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

//OutputShaderErrorMessage �Լ������� ���� / �ȼ� ���̴��� ������ ���� ������ ���� ������� ���� �޼����� ����ϴ� ���� �Ѵ�.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
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
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount){
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TessellationBufferType* dataPtr2;
	unsigned int bufferNumber;


	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// ��� ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// �׼����̼� ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// �׼����̼� ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr2 = (TessellationBufferType*)mappedResource.pData;

	// �׼����̼� ��� ���ۿ� ����� �����Ѵ�.
	dataPtr2->tessellationAmount = tessellationAmount;

	// �׼����̼� ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_tessellationBuffer, 0);

	// �׼����̼� ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// �׼����̼� ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}

// ���̴� �������� ������ ������ �ǰ� ����, RenderShader �Լ������� �Է� ���̾ƿ��� �Է� ������� �����ϴ� ���̴�. �̷ν� GPU ���� ������ �ڷᱸ���� �˰� �ȴ�.
// �� �������۸� �׸��� ���� �������̴��� �ȼ� ���̴��� �����ϴ� ���̴�. ���̴��� �����ǰ� ���� Direct3D ��ġ ���ؽ�Ʈ���� DirectX11 DrawIndexed �Լ��� �̿��� �ﰢ���� �׷�����.
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

