#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
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

	// �� ��ü ����
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	// �� ��ü �ʱ�ȭ
	// ���� Initialize�� ���� �׸��� �� ���Ǵ� �ؽ�ó�� ���ڷ� �޴´�. 
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/Cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// LightShaderClass ��ü ����
	m_LightShader = new LightShaderClass;
	if(!m_LightShader) {
		return false; 
	} 
	
	// LightShaderClass ��ü �ʱ�ȭ
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// LightClass ��ü ����
	m_Light = new LightClass;
	if (!m_Light) {
		MessageBox(hwnd, L"Could not initialize the light object.", L"Error", MB_OK);
		return false;
	}

	// LightClass ��ü �ʱ�ȭ
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ��ȯ
	if(m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	} 

	// �� ��ü ��ȯ
	if(m_Model) { 
		m_Model->Shutdown(); 
		delete m_Model; m_Model = 0; 
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

	// ���� ��ü ��ȯ
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}
	return;
}

// Frame()�� �� �����Ӹ��� Render �Լ��� �θ����� �ٲ۴�.
bool GraphicsClass::Frame() {
	bool result;

	// ������ ȿ���� ���� �ΰ��ϱ� ���� ȸ������ Render�Լ��� �ش�. 
	// �� ȸ������ �� �����Ӹ��� �ٲ�Ƿ� Frame �Լ����� �������ش�.

	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}

	// �׷��� �������� �����Ѵ�.
	result = Render(rotation);
	if (!result) {
		return false;
	}

	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
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

	// ���⼭ ������� ���� ���ڷ� �Է¹��� ȸ������ŭ ȸ����Ų��.
	worldMatrix = XMMatrixRotationY(rotation);

	// �׷��� ModelClass::Render �Լ��� ȣ���Ͽ� �׷��� ���������ο��� �ﰢ�� ���� �׸����� �Ѵ�. 
	m_Model->Render(m_D3D->GetDeviceContext());

	// ������ ���̴��� ȣ���Ͽ� �غ�� ������ �� ����� ����Ͽ� �������� �׷�����. �׷��� �ﰢ���� ����ۿ� �׷����� �ȴ�.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
	if (!result) { 
		return false;
	}

	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}