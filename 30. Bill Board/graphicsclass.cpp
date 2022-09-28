#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;
	m_FloorModel = 0;
	m_BillboardModel = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) {
}


GraphicsClass::~GraphicsClass() {
}


//GraphicsClass::Initiallize() ���� D3D ��ü�� �ʱ�ȭ��Ų��.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;
	XMMATRIX baseViewMatrix;

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

	// ī�޶� ��ü�� ���� ���ο� �� ����� �����Ͽ� TextClass�� ����ϰ� �ؾ��Ѵ�.
	// �̸� ���Ͽ� ���ڰ� �׻� ȭ���� ���� ��ġ�� �׷����� �ȴ�.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);


	// �ٴ� �� ��ü ����
	m_FloorModel = new ModelClass;
	if (!m_FloorModel) {
		return false;
	}
	
	// �ٴ� �� ��ü �ʱ�ȭ
	result = m_FloorModel->Initialize(m_D3D->GetDevice(), "../Engine/data/floor.txt", L"../Engine/data/grid01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// ������ �� ��ü ����
	m_BillboardModel = new ModelClass;
	if (!m_BillboardModel) {
		return false;
	}
	
	// ������ �� ��ü �ʱ�ȭ
	result = m_BillboardModel->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	// LightShader ��ü ����
	m_TextureShader = new TextureShaderClass; 
	if (!m_TextureShader) {
		return false;
	} 
	
	// LightShader ��ü �ʱ�ȭ 
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); 
		return false; 
	}
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// Light Shader ��ü ��ȯ
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// �� ��ȯ
	if (m_BillboardModel) {
		m_BillboardModel->Shutdown();
		delete m_BillboardModel;
		m_BillboardModel = 0;
	}
	if (m_FloorModel) {
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
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
// ������ �Լ������� SystemClass�� ���� ī�޶��� ȸ�� ���� ���ڷ� �޴´�. Render �Լ����� �� ����� ����� ����
bool GraphicsClass::Frame(float positionX, float positionY, float positionZ){
	bool result;

	// ī�޷� ��ġ �Է� 
	m_Camera->SetPosition(positionX, positionY, positionZ);

	result = Render();
	if (!result) { 
		return false;
	}
	
	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;
	double angle; float rotation;
	bool result;

	// �� �׸��⸦ �����ϱ� ���� ������ ������ �����.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� ������� ����� ���ؼ� ī�޶��� Render�Լ��� ȣ���Ѵ�.
	m_Camera->Render();

	// ������� ��������� ���� ������� ���纻�� ������ �� �ִ�.
	// ���� D3DClass ��ü�� ���� ������İ� ������ĵ� �����ؿ´�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_FloorModel->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture()); 
	if (!result) {
		return false;
	}

	// ī�޶��� ��ġ�� ���� �� ������ ���� ��ġ�� �����Ѵ�.
	cameraPosition = m_Camera->GetPosition(); 

	modelPosition.x = 0.0f; 
	modelPosition.y = 1.5f;
	modelPosition.z = 0.0f;

	// ī�޶� ��ġ�� �̿��Ͽ� �������� ȸ������ ����Ͽ� ī�޶�� ���ֺ��� �ֶǷ� �Ѵ�. 
	// atan2 �Լ��� �̿��Ͽ� ������ �𵨿� ����� ȸ������ ����մϴ�. �̷��� �Ͽ� ������ ���� ���� ī�޶� ��ġ�� �ٶ󺸰� �Ѵ�.
	angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI); 
	
	// ȸ�������� �������� ��ȯ�Ѵ�.. 
	rotation = (float)angle * 0.0174532925f;

	// ���� ����� �̿��Ͽ� ���������� ������ ȸ������ ����Ѵ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// ������ ���� �̵������ �����Ѵ�.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// ȸ����İ� �̵������ �����Ͽ� ������ ���� ���� ���� ����� ����Ѵ�.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_BillboardModel->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BillboardModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BillboardModel->GetTexture()); 
	if (!result) { 
		return false;
	}

	m_D3D->EndScene();
	
	return true;

}