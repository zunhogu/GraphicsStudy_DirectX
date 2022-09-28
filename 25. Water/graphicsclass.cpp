#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0; 
	m_GroundModel = 0; 
	m_WallModel = 0; 
	m_BathModel = 0; 
	m_WaterModel = 0;
	m_Light = 0;
	m_RefractionTexture = 0; 
	m_ReflectionTexture = 0; 
	m_LightShader = 0;
	m_RefractionShader = 0;
	m_WaterShader = 0;
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
	m_GroundModel = new ModelClass();
	if (!m_GroundModel) {
		return false;
	}

	result = m_GroundModel->Initialize(m_D3D->GetDevice(),"../Engine/data/ground.txt", L"../Engine/data/ground01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK); 
		return false;
	}

	m_WallModel = new ModelClass();
	if (!m_GroundModel) {
		return false;
	}

	result = m_WallModel->Initialize(m_D3D->GetDevice(), "../Engine/data/wall.txt", L"../Engine/data/wall01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}

	m_BathModel = new ModelClass();
	if (!m_BathModel) {
		return false;
	}

	result = m_BathModel->Initialize(m_D3D->GetDevice(), "../Engine/data/bath.txt", L"../Engine/data/marble01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	m_WaterModel = new ModelClass();
	if (!m_WaterModel) {
		return false;
	}

	result = m_WaterModel->Initialize(m_D3D->GetDevice(), "../Engine/data/water.txt", L"../Engine/data/water01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// Light ��ü ���� �� �ʱ�ȭ
	m_Light = new LightClass;
	if(!m_Light) {
		return false;
	} 

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f); 
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
	m_Light->SetDirection(0.0f, -1.0f, 0.5f);

	// �ݻ� ���� �ؽ�ó�� ����� ���� RTT ��ü 2�� ����
	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture) {
		return false;
	}
	result = m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	m_RefractionTexture = new RenderTextureClass;
	if (!m_RefractionTexture) {
		return false;
	}

	result = m_RefractionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// RefractionShaderClass ��ü ����
	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader) {
		return false;
	}

	// RefractionShaderClass ��ü �ʱ�ȭ
	result = m_RefractionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}

	// LightShaderClass�� �����Ѵ�.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader) {
		return false; 
	} 
	
	// LightShaderClass�� �ʱ�ȭ�Ѵ�.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) { 
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); 
		return false;
	}

	// WaterShaderClass�� �����Ѵ�.
	m_WaterShader = new WaterShaderClass;
	if (!m_WaterShader) {
		return false;
	}

	// WaterShaderClass�� �ʱ�ȭ�Ѵ�.
	result = m_WaterShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the water shader object.", L"Error", MB_OK);
		return false;
	}
	// Set the height of the water.
	m_waterHeight = 2.75f;

	// Initialize the position of the water.
	m_waterTranslation = 0.0f;
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ����
	if (m_WaterShader) {
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}
	if (m_RefractionShader) {
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}

	// RTT ��ü ����
	if (m_RefractionTexture) { 
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0; 
	}
	if (m_ReflectionTexture) {
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// Light ��ü ����
	if (m_Light) {
		delete m_Light;
		m_Light = 0; 
	}

	// �� ��ü ����
	if (m_WaterModel) {
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = 0;
	}
	if (m_BathModel) {
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = 0;
	}
	if (m_WallModel){
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = 0;
	}

	if (m_GroundModel) {
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
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

	m_waterTranslation += 0.001f; 
	if (m_waterTranslation > 1.0f) { 
		m_waterTranslation -= 1.0f;
	}

	m_Camera->SetPosition(-15.0f, 10.0f, -22.0f);
	m_Camera->SetRotation(0.0f, 35.0f, 0.0f);

	return true;
}

bool GraphicsClass::Render() {
	// ���� �ݻ�, ���� �ؽ�ó�� RTT�� ���� �������Ѵ�.
	// ���� ��ü ���� ����ۿ� �������Ѵ�.
	// ���� RTT�� ���� ����� ���̰� �ʹ�. �� �����Ӹ��� ĸó�ϴ� ��� 15~30 �����Ӹ��� ĸ���ϴ� ����� �ִ�.

	bool result;

	result = RenderRefractionToTexture();
	if (!result) {
		return false;
	}

	result = RenderReflectionToTexture();
	if (!result) {
		return false;
	}

	result = RenderScene();
	if (!result) {
		return false;
	}

	return true;
}

// RenderRefractionToTexture �Լ��� ������ �� �Ʒ� ��鸸 �ؽ�ó�� �׸���.
bool GraphicsClass::RenderRefractionToTexture() {
	XMFLOAT4 clipPlane;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Ŭ���� ����� ����ؼ� �� ���� �ִ� ��� �͵��� �߶󳻰� �� �Ʒ��� �ִ� �͸� �׸���.
	// ������ Ŭ���� ����� ������ ��¦ ���ʿ� ������ �Ѵٴ°� �ָ��ؾ��Ѵ�.
	// �� ������ ���κп� �븻 ������ ������Ų ���ø� ��ġ�� �˰� ���̴� ������ �ֱ� �����̴�.
	// Ŭ���� ����� ��¦ ���� ���÷��� �� ���� ������ ���̰Բ� �� �� �ִ�.
	// �� ���α׷��� �����Ҷ� Ŭ���� ����� ���̸� �� ���̿� ���� ���� �� ������ ���� �Ͼ�� Ȯ���� �� �ִ�.
	clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

	// ����Ÿ���� �ؽ�ó�� ����
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// �ؽ�ó�� �����ϰ� ����
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ü�� ���� ������� ����, ������ ���� ���� �Ϲ� ����ĸ� ����ص� �ȴ�.
	m_Camera->Render();

	// ī�޶� ��ü�� D3D ��ü�� ���� ����, ��, ���� ����� �޾ƿ�
	m_D3D->GetWorldMatrix(worldMatrix); 
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the bath model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// ���� ���� �׷��� ���������ο� �ִ´�.
	m_BathModel->Render(m_D3D->GetDeviceContext());

	// ����� �������Ѵ�.
	result = m_RefractionShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor(), clipPlane);
	if (!result) { 
		return false; 
	}

	// ����Ÿ���� ����۷� �ǵ�����.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

// RenderReflectionToTexture �Լ��� �ݻ�� ����� �ؽ�ó�� �׸���. �� ���� �ִ� ��ü�� �׸��� �빮�� ���� �ݻ�Ǵ� ��ó�� ���̴� ���𵨸� �׸���.
bool GraphicsClass::RenderReflectionToTexture() {
	XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	bool result;

	// ����Ÿ���� �ؽ�ó�� ����
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// �ؽ�ó�� �����ϰ� ����
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// �Ϲ� ����Ĵ�� �ݻ������ �ʿ��ϴ�. ���� �ݻ������ �����Ѵ�.
	m_Camera->RenderReflection(m_waterHeight);

	// �������, �ݻ�����, ��������� �����´�.
	m_D3D->GetWorldMatrix(worldMatrix); 
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ��������� ������ ��ü�� �̵������ش�.
	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

	// �� ���� �׷������������ο� �ִ´�.
	m_WallModel->Render(m_D3D->GetDeviceContext());

	// ����� �������Ѵ�.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor());
	if (!result) { 
		return false; 
	}

	// ����Ÿ���� ����۷� �ǵ�����.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

// RednerScene �Լ��� ����ۿ� ����� �������ϴ� �Լ���.
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

	// �ٴ� ���� ���� �׸���.
	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

	m_GroundModel->Render(m_D3D->GetDeviceContext());

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor());
	if (!result) {
		return false;
	}

	// ���� ��� ����
	m_D3D->GetWorldMatrix(worldMatrix);

	// �� ���� �׸���.
	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

	m_WallModel->Render(m_D3D->GetDeviceContext());

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix,projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor());
	if (!result){
		return false;
	}

	// ���� ���� �׸���.
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	m_BathModel->Render(m_D3D->GetDeviceContext());

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(),m_Light->GetAmbientColor());
	if (!result){
		return false;
	}


	// �� ���� �׸���.
	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	worldMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);

	m_WaterModel->Render(m_D3D->GetDeviceContext());

	result = m_WaterShader->Render(m_D3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(), m_waterTranslation, 0.01f);
	if (!result){
		return false;
	}

	// �������� ������ ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();


	return true;
}