#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0; 
	m_Light = 0;
	m_CubeModel = 0;
	m_GroundModel = 0;
	m_SphereModel = 0;
	m_RenderTexture = 0;
	m_DepthShader = 0; 
	m_ShadowShader = 0;


}


GraphicsClass::GraphicsClass(const GraphicsClass& other) {
}


GraphicsClass::~GraphicsClass() {
}


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

	// ī�޶� ��ü �ʱ�ȭ
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// ���� ��ü ���� �� �ʱ�ȭ
	m_Light = new LightClass;
	if (!m_Light) {
		return false;
	}

	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); 
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// �� ��ü ���� �� �ʱ�ȭ
	m_CubeModel = new ModelClass();
	if (!m_CubeModel) {
		return false;
	}

	result = m_CubeModel->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/wall01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK); 
		return false;
	}

	m_SphereModel = new ModelClass();
	if (!m_CubeModel) {
		return false;
	}
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);

	result = m_SphereModel->Initialize(m_D3D->GetDevice(), "../Engine/data/sphere.txt", L"../Engine/data/ice.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the sphere  model object.", L"Error", MB_OK);
		return false;
	}
	m_SphereModel->SetPosition(0.0f, 2.0f, -2.0f);

	m_GroundModel = new ModelClass();
	if (!m_CubeModel) {
		return false;
	}

	result = m_GroundModel->Initialize(m_D3D->GetDevice(), "../Engine/data/plane01.txt", L"../Engine/data/metal001.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the plane model object.", L"Error", MB_OK);
		return false;
	}
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

	// RTT ��ü ���� �� �ʱ�ȭ
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture){
		return false;
	}

	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// ShaderClass ��ü ���� �� �ʱ�ȭ
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader){
		return false;
	}

	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader){
		return false;
	}

	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ��ȯ
	if(m_ShadowShader) {
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	} 
	if (m_DepthShader) {
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// RTT ��ü ��ȯ
	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// �� ��ü ��ȯ
	if(m_GroundModel) {
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	} 
	if (m_SphereModel) {
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = 0;
	}
	if (m_CubeModel) {
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
	}

	// ���� ��ü ��ȯ
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	// ī�޶� ��ü ��ȯ
	if(m_Camera) { 
		delete m_Camera; m_Camera = 0; 
	}
	// Direct3D ��ü ��ȯ
	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// Frame()�� �� �����Ӹ��� Render �Լ��� �θ����� �ٲ۴�.
bool GraphicsClass::Frame(float posX, float posY, float posZ, float rotX, float rotY, float rotZ){
	bool result;
	static float lightPositionX = -5.0f;

	// ī�޶� ��ġ ����
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// �� �����Ӹ��� ������ ��ġ�� �����Ѵ�.
	lightPositionX += 0.05f;
	if (lightPositionX > 5.0f) {
		lightPositionX = -5.0f;
	}

	// ������ ��ġ ����
	m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);

	// ������
	result = Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::RenderSceneToTexture() {
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;
	bool result;

	// ���� Ÿ�� ����
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext()); 
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ������ ��ġ���� ������ �� ����� �����Ѵ�.
	m_Light->GenerateViewMatrix();

	// d3d ��ü���� ���� ����� �����´�.
	m_D3D->GetWorldMatrix(worldMatrix);

	// ������ �� �� ���� ����� ���Ѵ�.
	m_Light->GetViewMatrix(lightViewMatrix); 
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// ������ ���� �̵������ �����Ѵ�.
	m_CubeModel->GetPosition(posX, posY, posZ); 
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ���� ���̴��� ������ ���� �׸���.
	m_CubeModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix); 
	if (!result) { 
		return false;
	}

	// ���� ����� �ʱ�ȭ�Ѵ�.
	m_D3D->GetWorldMatrix(worldMatrix);

	// �� ���� �̵� ����� �����Ѵ�.
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ���� ���̴��� �� ���� �׸���.
	m_SphereModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) {
		return false;
	}

	// ���� ����� �ʱ�ȭ�Ѵ�.
	m_D3D->GetWorldMatrix(worldMatrix);

	// �ٴ� ���� �̵� ����� �����Ѵ�.
	m_GroundModel->GetPosition(posX, posY, posZ); 
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ���� ���̴��� �ٴ� ���� �׸���.
	m_GroundModel->Render(m_D3D->GetDeviceContext()); 
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) { 
		return false;
	}

	// ���� Ÿ���� ����۷� ������.
	m_D3D->SetBackBufferRenderTarget();

	// ����Ʈ ������� ������.
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;
	float posX, posY, posZ;

	// �켱 ����� �ؽ�ó�� �׸���.
	result = RenderSceneToTexture(); 
	if (!result) { 
		return false; 
	}

	// ������ ������ ���� ����� �����Ѵ�.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶�ü�κ��� ������� �����Ѵ�.
	m_Camera->Render();

	// ������ ��ġ���� ���� ��, ���� ����� �����Ѵ�.
	m_Light->GenerateViewMatrix();

	// ī�޶�, d3d ��ü�κ��� ����, ��, ���� ����� �޾ƿ´�.
	m_Camera->GetViewMatrix(viewMatrix); 
	m_D3D->GetWorldMatrix(worldMatrix); 
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ���� ��ü���� ���� ��, ���� ����� �޾ƿ´�.
	m_Light->GetViewMatrix(lightViewMatrix); 
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// ���� �� ���� �̵������ �����Ѵ�.
	m_CubeModel->GetPosition(posX, posY, posZ); 
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ������ ���� �׷��� �������̶��ο� �ִ´�.
	m_CubeModel->Render(m_D3D->GetDeviceContext());

	// ������ �� ������ ����
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_CubeModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()); 
	if (!result) {
		return false;
	}

	// ���� ��� �ʱ�ȭ
	m_D3D->GetWorldMatrix(worldMatrix);

	// �� ���� �̵������ �����Ѵ�.
	m_SphereModel->GetPosition(posX, posY, posZ);

	// �� ���� �׷��� �������̶��ο� �ִ´�.
	m_SphereModel->Render(m_D3D->GetDeviceContext());
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �� �� ������ ����
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_SphereModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result) {
		return false;
	}

	// ���� ��� �ʱ�ȭ
	m_D3D->GetWorldMatrix(worldMatrix);

	// �ٴ� ���� �̵������ �����Ѵ�.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �ٴ� ���� �׷��� �������̶��ο� �ִ´�.
	m_GroundModel->Render(m_D3D->GetDeviceContext());

	// �ٴ� �� ������ ����
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_GroundModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result) {
		return false;
	}	
	
	// ���� ��� �ʱ�ȭ
	m_D3D->GetWorldMatrix(worldMatrix);

	// ���� �׸���.
	m_D3D->EndScene();

	return true;
}