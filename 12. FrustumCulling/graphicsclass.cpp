#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_ModelList = 0;
	m_Frustum = 0;
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

	// �� ��ü ����
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	
	// �� ��ü �ʱ�ȭ
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/sphere.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Light ��ü ����
	m_Light = new LightClass; 
	if (!m_Light) {
		return false; 
	} 
	
	// Light ��ü �ʱ�ȭ
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// LightShader ��ü ����
	m_LightShader = new LightShaderClass; 
	if (!m_LightShader) {
		return false;
	} 
	
	// LightShader ��ü �ʱ�ȭ 
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); 
		return false; 
	}

	// ModelList ��ü ����
	m_ModelList = new ModelListClass();
	if (!m_ModelList) {
		return false;
	}

	// ModelList ��ü �ʱ�ȭ
	result = m_ModelList->Initialize(25);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// Frsutum ��ü ����, �� �����Ӹ��� ConstructFrustum �Լ��� ȣ���� ���̱� ������ �ʱ�ȭ�Լ��� �ʿ��������ʴ�.
	m_Frustum = new FrustumClass;
	if (!m_Frustum) {
		return false; 
	}
	
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// Frustum ��ü ��ȯ
	if(m_Frustum) { 
		delete m_Frustum;
		m_Frustum = 0; 
	}

	// ModelList ��ü ��ȯ
	if (m_ModelList) {
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// Light Shader ��ü ��ȯ
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Light ��ü ��ȯ
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	// �� ��ü ��ȯ
	if (m_Model) {
		m_Model->Shutdown(); 
		delete m_Model;
		m_Model = 0; 
	}

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
// ������ �Լ������� SystemClass�� ���� ī�޶��� ȸ�� ���� ���ڷ� �޴´�. Render �Լ����� �� ����� ����� ����
bool GraphicsClass::Frame(float rotationY) {
	bool result;

	// ī�޷� ��ġ �Է� 
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// ī�޶� ȸ���� ����
	m_Camera->SetRotation(0.0f, rotationY, 0.0f); 
	
	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	XMFLOAT4 color;
	bool renderModel, result;

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

	// Render �Լ��� ū ��ȭ�� �� �����Ӹ��� �� ��Ŀ� �ٰ��Ͽ� �þ� ���������� �����ϴ� ���̴�.
	// �� ���� ������ ������� �ٲ�ų� �츮�� �������� Ȯ���� ���� ������ �߻��Ѵ�.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// ���� ������ �Է¹޴´�.
	modelCount = m_ModelList->GetModelCount();

	// ModelList���� �׸� ���� ������ ī��Ʈ 
	renderCount = 1;

	// ModelListClass ��ü�� ��� ��ü�� ���ƺ���.
	for (index = 0; index < modelCount; index++) {
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// ��ü���� �������� ���� �� �������� Ŀ���� Ŀ������ ���� �þ� �������ҿ� �� Ȯ���� ����������, �ʹ� ������ ��� ���� �������� ���η� ���� ��ó�� �����ȴ�.
		radius = 1.0f;

		// ��ü�� ���� �þ� �������ҿ� ���̴����� üũ�ϴ� �ڵ�
		// ���� ���ν��� ���ο� �ֵ��� �׸���, ���ο� ���ٸ� �׸��� �ʰ� �Ѿ��. 
		renderModel = m_Frustum->CheckCube(positionX, positionY, positionZ, radius);

		if (renderModel) {  // �þ� �������� ������ ���̸� �� if�� ����

			// worldMatrix�� �����ؼ� ���� �����δ�. 
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// ������� ���� ���� / �ε��� ���۸� ����� ���۷� Ȱ��ȭ
			m_Model->Render(m_D3D->GetDeviceContext());

			// ���� / �ε��� ���۸� ���� �Է·��̾ƿ��� �־� ���̴��� ����, ���� ���� ���̴��� �̸� ���� ���� �׸�
			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), color, m_Light->GetDirection());
			
			// worldMatrix�� ���� ��ħ
			m_D3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}

	// ������ �� ���� ��ü���� �׷������� ȭ�鿡 �����ֱ� ���� TextClass�� �ణ �ٲ۴�.
	// FrustumClass�� ���� �� ���ڸ� ������ �� �ִµ�, �׷����� �� �� ��� �ø��Ǿ� ������ �ʰ� �� ���ڵ� ǥ���� �� �ִ�.
	
	result = m_Text->SetRenderCount(renderCount, m_D3D->GetDeviceContext());
	if (!result){
		return false;
	}

	// 2D �������� �����ϱ� ���� Z���۸� ����. 
	m_D3D->TurnZBufferOff();

	//  �ؽ�Ʈ ���� ��ȭ�Ӱ� �׷������� ���� ������ �Ҵ�.
	m_D3D->TurnOnAlphaBlending();

	//�׸��� ���� ȭ���� (100, 100) ��ġ�� ��Ʈ���� �׸���. �� ��ġ�� ���Ƿ� �ٲ� �� �ִ�.
	m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result){
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