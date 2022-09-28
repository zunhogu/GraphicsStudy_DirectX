#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0; 
	m_Model = 0; 
	m_WindowModel = 0; 
	m_RenderTexture = 0;
	m_TextureShader = 0;
	m_GlassShader = 0;
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

	// �� ��ü ���� �� �ʱ�ȭ
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/cube.txt", L"../Engine/data/seafloor.dds", L"../Engine/data/bump03.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK); 
		return false;
	}

	m_WindowModel = new ModelClass();
	if (!m_WindowModel) {
		return false;
	}

	result = m_WindowModel->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/ice01.dds", L"../Engine/data/icebump01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}

	// �ݻ� ���� �ؽ�ó�� ����� ���� RTT ��ü 2�� ����
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// ShaderClass ����

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_GlassShader = new GlassShaderClass;
	if (!m_GlassShader) {
		return false;
	}

	result = m_GlassShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ����
	if (m_GlassShader) {
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = 0;
	}
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// RTT ��ü ����
	if (m_RenderTexture) { 
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}


	// �� ��ü ����
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	if (m_WindowModel) {
		m_WindowModel->Shutdown();
		delete m_WindowModel;
		m_WindowModel = 0;
	}

	// ī�޶� ��ü ����
	if (m_Camera){
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D ��ġ ����
	if (m_D3D){
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// �� �����Ӹ��� ���� ��ġ�� �����Ͽ� ���� �帧�� ǥ���Ѵ�.
bool GraphicsClass::Frame() {
	bool result;
	static float rotation = 0.0f;

	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f){
		rotation -= 360.0f;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	result = RenderToTexture(rotation); 
	if (!result) { 
		return false; 
	}

	result = Render(rotation); 
	if (!result) { 
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float rotation) {
	// ���� �ݻ�, ���� �ؽ�ó�� RTT�� ���� �������Ѵ�.
	// ���� ��ü ���� ����ۿ� �������Ѵ�.
	// ���� RTT�� ���� ����� ���̰� �ʹ�. �� �����Ӹ��� ĸó�ϴ� ��� 15~30 �����Ӹ��� ĸ���ϴ� ����� �ִ�.

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float refractionScale; 
	bool result;

	// ���� ũ�� �ʱ�ȭ
	refractionScale = 0.05f;

	// ����� �ʱ�ȭ
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ������ü �� ������

	// ī�޶� ��ü�κ��� ����� ����
	m_Camera->Render();

	// ����, ��, ������� ������
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ������� ȸ��
	worldMatrix = XMMatrixRotationY(rotation);

	// ���� �׷��� ���������ο� ����
	m_Model->Render(m_D3D->GetDeviceContext());

	// �� ������
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) { 
		return false;
	}

	// ������� ����
	m_D3D->GetWorldMatrix(worldMatrix);

	// ���� �� ������
	
	//���� �� ��ġ ����
	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, -1.5f);

	// ���� �� �׷������������ο� ����
	m_WindowModel->Render(m_D3D->GetDeviceContext());

	result = m_GlassShader->Render(m_D3D->GetDeviceContext(), m_WindowModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WindowModel->GetTexture(), m_WindowModel->GetNormalMap(), m_RenderTexture->GetShaderResourceView(), refractionScale); 
	if (!result) { 
		return false;
	}

	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::RenderToTexture(float rotation) {
	XMMATRIX viewMatrix, worldMatrix, projectionMatrix;
	bool result;

	// ����Ÿ���� �ؽ�ó�� ����
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// �ؽ�ó�� �����ϰ� ����
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_D3D->GetDeviceContext());

	// �� ������
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	m_D3D->SetBackBufferRenderTarget();

	return true;
}