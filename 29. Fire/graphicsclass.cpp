#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_FireShader = 0;
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
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/fire01.dds", L"../Engine/data/noise01.dds", L"../Engine/data/alpha01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// TextureShaderClass ��ü ����
	m_FireShader = new FireShaderClass;
	if(!m_FireShader) {
		return false; 
	} 
	
	// TexturerClass ��ü �ʱ�ȭ
	result = m_FireShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ��ȯ
	if(m_FireShader) {
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
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

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f); 
	
	result = Render();
	if(!result) { 
		return false;
	} 
	
	return true;

}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result; 
	XMFLOAT3 scrollSpeeds, scales;
	XMFLOAT2 distortion1, distortion2, distortion3;
	float distortionScale, distortionBias;
	static float frameTime = 0.0f;

	// �� �����Ӹ��� frameTime ���� ������Ų��. �� ���� �̿��Ͽ� ���̴� �ȿ� �ִ� �� ���� �ٸ� ������ �ؽ�ó�� ��ũ���Ѵ�.
	// ����� FPS�� 60���� �������� �ʴ´ٸ� �� �����Ӹ��� �ð��� ���̸� �����Ͽ� Ÿ�̸Ӹ� ������Ʈ�Ͽ� �Ҳ��� FPS�� ������� ������ �ӵ��� Ÿ���������� �ؾ��Ѵ�.
	// ������ ī���� ����
	frameTime += 0.01f; 
	if (frameTime > 1000.0f) {
		frameTime = 0.0f; 
	}

	// �� ������ �ؽ�ó�� ��ũ�� �ӵ��� �����Ѵ�.
	scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// �� ũ�Ⱚ�� ����Ͽ� �� ������ �ٸ� ������ ��Ÿ�� �ؽ��ĸ� �����.
	scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// �� ������ �ؽ�ó�� ���� �ٸ� �� ���� x, y �ְ� ���ڸ� �����Ѵ�.
	distortion1 = XMFLOAT2(0.1f, 0.2f);
	distortion2 = XMFLOAT2(0.1f, 0.3f);
	distortion3 = XMFLOAT2(0.1f, 0.1f);

	// ������ �ؽ�ó�� �Ҳ� ������� ������Ű�µ� ����� ���̾ ũ�� ���� �����Ѵ�.
	distortionScale = 0.8f; 
	distortionBias = 0.5f;

	// �� �׸��⸦ �����ϱ� ���� ������ ������ �����.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� ������� ����� ���ؼ� ī�޶��� Render�Լ��� ȣ���Ѵ�.
	m_Camera->Render();

	// ������� ��������� ���� ������� ���纻�� ������ �� �ִ�.
	// ���� D3DClass ��ü�� ���� ������İ� ������ĵ� �����ؿ´�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	/// �Ҳ��� �������� ���� ���ĺ����� �Ҵ�.
	m_D3D->TurnOnAlphaBlending();

	// �׷��� ModelClass::Render �Լ��� ȣ���Ͽ� �׷��� ���������ο��� �ﰢ�� ���� �׸����� �Ѵ�. 
	m_Model->Render(m_D3D->GetDeviceContext());

	// ������ ���̴��� ȣ���Ͽ� �غ�� ������ �� ����� ����Ͽ� �������� �׷�����. �׷��� �ﰢ���� ����ۿ� �׷����� �ȴ�.
	result = m_FireShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,m_Model->GetTextureArray(), frameTime, scrollSpeeds,scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
	if (!result) { 
		return false;
	}

	// ���� ������ ����. 
	m_D3D->TurnOffAlphaBlending();


	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}