#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_FloorModel = 0;
	m_ReflectShader = 0;
	m_RenderTexture = 0; 
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) {}


GraphicsClass::~GraphicsClass() {}


//GraphicsClass::Initiallize() ���� D3D ��ü�� �ʱ�ȭ��Ų��.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;

	// Dricect3D ��ü�� �����Ѵ�.
	m_D3D = new D3DClass;
	if (!m_D3D) {
		return false;
	}

	// Direct3D ��ü�� �ʱ�ȭ�Ѵ�.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// ī�޶� ��ü ����
	m_Camera = new CameraClass();
	if (!m_Camera) {
		return false;
	}

	// �� ��ü ����
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	// �� ��ü �ʱ�ȭ
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/Cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// Floor �� ��ü ����
	m_FloorModel = new ModelClass();
	if (!m_FloorModel) {
		return false;
	}

	// Floor �� ��ü �ʱ�ȭ
	result = m_FloorModel->Initialize(m_D3D->GetDevice(), "../Engine/data/floor.txt", L"../Engine/data/blue01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// RTT ��ü�� �����Ѵ�.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	// RTT ��ü�� �ʱ�ȭ�Ѵ�.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// ReflectionShaderClass ��ü ����
	m_ReflectShader = new ReflectionShaderClass;
	if (!m_ReflectShader) {
		return false;
	}

	// ReflectionShaderClass ��ü �ʱ�ȭ
	result = m_ReflectShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ��� ���̴��� �����Ѵ�.
	m_TextureShader = new TextureShaderClass; 
	if(!m_TextureShader) {
		return false; 
	} 
	
	// �ؽ��� ���̴��� �ʱ�ȭ�Ѵ�.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd); 
	if(!result) { 
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK); 
		return false;
	}

	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// Release the texture shader object.
	if (m_TextureShader){
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture){
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the light shader object.
	if (m_ReflectShader){
		m_ReflectShader->Shutdown();
		delete m_ReflectShader;
		m_ReflectShader = 0;
	}

	// Release the model object.
	if (m_Model){
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_FloorModel) {
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
	}

	// Release the camera object.
	if (m_Camera){
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D){
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// Frame()�� �� �����Ӹ��� Render �Լ��� �θ����� �ٲ۴�.
bool GraphicsClass::Frame() {
	bool result;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// �ݻ�� ����� �ؽ�ó�� �׸���.
	result = RenderToTexture(); 
	if (!result) { 
		return false; 
	}

	// ����ۿ� �Ϲ� ����� �׸���.
	result = RenderScene();
	if (!result){
		return false;
	}

	return true;
}

// RenderToTexture() �Լ��� ����Ÿ���� �ؽ�ó�� �����ϰ�, ������ ���Ŀ��� ���� Ÿ���� ����۷� ������.
bool GraphicsClass::RenderToTexture(){
	
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	bool result;
	static float rotation = 0.0f;

	// RTT�� ������ Ÿ���� �ǵ����Ѵ�.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// RTT�� �ʱ�ȭ�Ѵ�.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// ������ ī�޶� �ƴ� ���ο� ������� �����.
	m_Camera->RenderReflection(-1.5f);

	// ���� ������� �����´�. 
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// �� �����Ӹ��� ȸ������ ����Ѵ�.
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}
	worldMatrix = XMMatrixRotationY(rotation);

	// ���� �׷��� ���������ο� �ִ´�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// ���� �������Ѵ�.
	result =m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	// ����Ÿ���� �ٽ� ����۷� ������.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderScene(){
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;
	bool result;
	static float rotation = 0.0f;

	// ����� �����ϱ� ���Ͽ� ���۸� �ʱ�ȭ�Ѵ�.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� �ٰ��Ͽ� ������� �����Ѵ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü���� ����, ��, ���� ����� ���´�.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// �� �����Ӹ��� ȸ������ ����Ѵ�.
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f){
		rotation -= 360.0f;
	}
	worldMatrix = XMMatrixRotationY(rotation);

	// �Ϲ� ���� ������ �ε��� ���۸� �׷��� ���������ο� �־� ������ �� �غ� �Ѵ�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// �Ϲ� ���� �������� �Ѵ�.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result){
		return false;
	}

	// ��������� �ٽ� ������ ������ �Ʒ��� �ٴ� ���� �׸��� ���� �Ʒ��� �̵���Ų��.
	m_D3D->GetWorldMatrix(worldMatrix); 
	worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);
	
	// ī�޶� �ݻ� �� ����� �����´�.
	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	// �ٴ� ���� �׷��� ���������ο� �ִ´�.
	m_FloorModel->Render(m_D3D->GetDeviceContext());

	// �ٴ� ���� �׸���.
	result = m_ReflectShader->Render(m_D3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), reflectionMatrix);
	if (!result) {
		return false;
	}

	// �������� �Ϸ�� ����� ȭ�鿡 �Ѹ���.
	m_D3D->EndScene();

	return true;
}