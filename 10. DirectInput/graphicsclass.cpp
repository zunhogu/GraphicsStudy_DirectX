#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
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

	// �ؽ�Ʈ ��ü ����
	m_Text = new TextClass();
	if (!m_Text) {
		return false;
	}

	// ��Ʈ�� ��ü �ʱ�ȭ
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK); 
		return false;
	}
	
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// �ؽ�Ʈ ��ü ��ȯ
	if(m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
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
bool GraphicsClass::Frame(int mouseX, int mouseY) {
	bool result;

	// ���콺 �Է°��� �� �����Ӹ��� üũ�ؼ� ���콺 ���� �����Ѵ�.
	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result) {
		return false;
	}

	// 
	result = Render();
	if (!result) {
		return false;
	}
	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
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

	// 2D �������� �ϱ� ���ؼ��� ���翵 ����� ���Ѵ�. �̴� �Ϲ� ������Ĵ�� ���޵� ���̴�.
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� �����ϱ� ���� Z���۸� ����. 
	m_D3D->TurnZBufferOff();

	// �ؽ�Ʈ ���� ��ȭ�Ӱ� �׷������� ���� ������ �Ҵ�.
	m_D3D->TurnOnAlphaBlending();

	// �׸��� ���� ȭ���� (100, 100) ��ġ�� ��Ʈ���� �׸���. �� ��ġ�� ���Ƿ� �ٲ� �� �ִ�.
	 result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix); 
	 if(!result) {
		 return false;
	 }
	 // ���� ������ ����. 
	 m_D3D->TurnOffAlphaBlending();

	// 2D �������� ������ 3D ��ü�� �׸������ؼ� Z���۸� �ٽ� �Ҵ�.
	m_D3D->TurnZBufferOn();

	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}