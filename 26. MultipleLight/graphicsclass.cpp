#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light1 = 0;
	m_Light2 = 0; 
	m_Light3 = 0;
	m_Light4 = 0;
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

	// �� ��ü ����
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	// �� ��ü �ʱ�ȭ
	// ���� Initialize�� ���� �׸��� �� ���Ǵ� �ؽ�ó�� ���ڷ� �޴´�. 
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/plane01.txt", L"../Engine/data/stone01.dds");
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

	// LightClass1 ��ü ����
	m_Light1 = new LightClass;
	if (!m_Light1) {
		MessageBox(hwnd, L"Could not initialize the light1 object.", L"Error", MB_OK);
		return false;
	}

	// LightClass1 ��ü �ʱ�ȭ
	m_Light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light1->SetPosition(-3.0f, 1.0f, 3.0f);

	// LightClass2 ��ü ����
	m_Light2 = new LightClass;
	if (!m_Light2) {
		MessageBox(hwnd, L"Could not initialize the light1 object.", L"Error", MB_OK);
		return false;
	}

	// LightClass2 ��ü �ʱ�ȭ
	m_Light2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f); 
	m_Light2->SetPosition(3.0f, 1.0f, 3.0f);

	// LightClass3 ��ü ����
	m_Light3 = new LightClass;
	if (!m_Light3) {
		MessageBox(hwnd, L"Could not initialize the light1 object.", L"Error", MB_OK);
		return false;
	}

	// LightClass3 ��ü �ʱ�ȭ
	m_Light3->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_Light3->SetPosition(-3.0f, 1.0f, -3.0f);

	// LightClass4 ��ü ����
	m_Light4 = new LightClass;
	if (!m_Light4) {
		MessageBox(hwnd, L"Could not initialize the light1 object.", L"Error", MB_OK);
		return false;
	}

	// LightClass4 ��ü �ʱ�ȭ
	m_Light4->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
	m_Light4->SetPosition(3.0f, 1.0f, -3.0f);

	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���� ��ü ��ȯ
	if (m_Light1) {
		delete m_Light1; 
		m_Light1 = 0; 
	}
	
	if (m_Light2) {
		delete m_Light2; 
		m_Light2 = 0; 
	} 
	if (m_Light3) { 
		delete m_Light3;
		m_Light3 = 0;
	} 
	if (m_Light4) {
		delete m_Light4;
		m_Light4 = 0;
	}

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
	return;
}

// Frame()�� �� �����Ӹ��� Render �Լ��� �θ����� �ٲ۴�.
bool GraphicsClass::Frame() {
	bool result;

	m_Camera->SetPosition(0.0f, 2.0f, -12.0f);

	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMFLOAT4 diffuseColor[4];
	XMFLOAT4 lightPosition[4];
	bool result;


	// �� ���� ���� �迭 �ʱ�ȭ
	diffuseColor[0] = m_Light1->GetDiffuseColor();
	diffuseColor[1] = m_Light2->GetDiffuseColor();
	diffuseColor[2] = m_Light3->GetDiffuseColor();
	diffuseColor[3] = m_Light4->GetDiffuseColor();

	// �� ���� ��ġ �迭 �ʱ�ȭ
	lightPosition[0] = m_Light1->GetPosition();
	lightPosition[1] = m_Light2->GetPosition();
	lightPosition[2] = m_Light3->GetPosition();
	lightPosition[3] = m_Light4->GetPosition();


	// �� �׸��⸦ �����ϱ� ���� ������ ������ �����.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� ������� ����� ���ؼ� ī�޶��� Render�Լ��� ȣ���Ѵ�.
	m_Camera->Render();

	// ������� ��������� ���� ������� ���纻�� ������ �� �ִ�.
	// ���� D3DClass ��ü�� ���� ������İ� ������ĵ� �����ؿ´�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);


	// �׷��� ModelClass::Render �Լ��� ȣ���Ͽ� �׷��� ���������ο��� �ﰢ�� ���� �׸����� �Ѵ�. 
	m_Model->Render(m_D3D->GetDeviceContext());

	// ������ ���̴��� ȣ���Ͽ� �غ�� ������ �� ����� ����Ͽ� �������� �׷�����. �׷��� �ﰢ���� ����ۿ� �׷����� �ȴ�.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), diffuseColor, lightPosition);
	if (!result) { 
		return false;
	}

	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}