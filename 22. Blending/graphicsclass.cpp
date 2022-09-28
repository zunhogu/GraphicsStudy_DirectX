#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model1 = 0;
	m_Model2 = 0;
	m_TextureShader = 0;
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

	// ù��° �� ��ü ����
	m_Model1 = new ModelClass; 
	if(!m_Model1) {
		return false;
	}

	// ù��° �� ��ü �ʱ�ȭ
	result = m_Model1->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/dirt01.dds");
	if(!result) { 
		MessageBox(hwnd, L"Could not initialize the first model object.", L"Error", MB_OK); 
		return false; 
	}

	// �ι�° �� ��ü ����
	m_Model2 = new ModelClass;
	if (!m_Model2) {
		return false;
	}

	// �ι�° �� ��ü �ʱ�ȭ
	result = m_Model2->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/stone01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the first model object.", L"Error", MB_OK);
		return false;
	}

	// TextureShaderClass ��ü ����
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader) {
		return false; 
	} 
	
	// TextureShaderClass ��ü �ʱ�ȭ
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// ���� ���̴� ��ü ����
	m_TransparentShader = new TransparentShaderClass;
	if (!m_TransparentShader) {
		return false;
	}

	result = m_TransparentShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK); 
		return false;
	}
	
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���� ���̴� ��ü ��ȯ
	if (m_TransparentShader) {
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	// �ؽ�ó ���̴� ��ü ��ȯ
	if(m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	} 

	// �� ��ü ��ȯ
	if(m_Model1) {
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
	} 

	if (m_Model2) {
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	float blendAmount;

	// ���� �� ����
	blendAmount = 0.5f;

	// �� �׸��⸦ �����ϱ� ���� ������ ������ �����.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� ������� ����� ���ؼ� ī�޶��� Render�Լ��� ȣ���Ѵ�.
	m_Camera->Render();

	// ������� ��������� ���� ������� ���纻�� ������ �� �ִ�.
	// ���� D3DClass ��ü�� ���� ������İ� ������ĵ� �����ؿ´�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ���� ���� �� �ε��� ���۸� �׷��� ���������ο� �־� �׸� �غ� �Ѵ�.
	m_Model1->Render(m_D3D->GetDeviceContext());

	// ù��° ���� �׸���.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model1->GetTexture()); 
	if (!result) {
		return false;
	}

	// ���� ����� ���������� 1, ī�޶� ���� 1��ŭ �̵���Ų��.
	// �̸����� �ι�°�� �׷��� ���� ��ġ�� �����Ѵ�.
	worldMatrix = XMMatrixTranslation(1.0f, 0.0f, -1.0f);


	// ���� ȿ���� �����ϰԲ� ���ĺ����� �Ҵ�.
	m_D3D->TurnOnAlphaBlending();

	// �ι�° ���� �׷��� ���������ο� �ִ´�.
	m_Model2->Render(m_D3D->GetDeviceContext());

	// �ι�° ���� �׸���.
	result = m_TransparentShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(), blendAmount); 
	if (!result) { 
		return false; 
	}

	// ���ĺ����� ����.
	m_D3D->TurnOffAlphaBlending();

	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}