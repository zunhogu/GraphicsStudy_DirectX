#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_ParticleSystem = 0;
	m_ParticleShader = 0;
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

	// ��ƼŬ �ý��� ��ü ����
	m_ParticleSystem = new ParticleSystemClass();
	if (!m_ParticleSystem) {
		return false;
	}

	// ��ƼŬ �ý��� ��ü �ʱ�ȭ
	// ��ƼŬ �ý��� Initialize�� ���� �׸��� �� ���Ǵ� �ؽ�ó�� ���ڷ� �޴´�. 
	result = m_ParticleSystem->Initialize(m_D3D->GetDevice(), L"../Engine/data/star.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the particle system object.", L"Error", MB_OK); 
		return false;
	}

	// ParticleSystemClass ��ü ����
	m_ParticleShader = new ParticleShaderClass;
	if(!m_ParticleShader) {
		return false; 
	} 
	
	// ParticleSystemClass ��ü �ʱ�ȭ
	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ��ȯ
	if(m_ParticleShader) {
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	} 

	// �� ��ü ��ȯ
	if(m_ParticleSystem) {
		m_ParticleSystem->Shutdown();
		delete m_ParticleSystem; 
		m_ParticleSystem = 0;
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
bool GraphicsClass::Frame(float frameTime) {
	bool result;

	m_ParticleSystem->Frame(frameTime, m_D3D->GetDeviceContext());

	// �׷��� �������� �����Ѵ�.
	result = Render();
	if (!result) {
		return false;
	}

	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
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

	// ���� ������ �Ҵ�.
	m_D3D->TurnOnAlphaBlending();

	// �Է������� �ܰ�
	m_ParticleSystem->Render(m_D3D->GetDeviceContext());

	// ������ ���̴��� ȣ���Ͽ� �غ�� ������ �� ����� ����Ͽ� �������� �׷�����. �׷��� �ﰢ���� ����ۿ� �׷����� �ȴ�.
	result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem->GetTexture());
	if (!result) { 
		return false;
	}
	
	// ���� ������ ����.
	m_D3D->TurnOffAlphaBlending();


	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}