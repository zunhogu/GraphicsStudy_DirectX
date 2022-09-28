#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0; 
	m_Model = 0; 
	m_WindowModel = 0; 
	m_RenderTexture = 0;
	m_TextureShader = 0;
	m_GlassShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) {}


GraphicsClass::~GraphicsClass() {}


//GraphicsClass::Initiallize() 에서 D3D 객체를 초기화시킨다.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;
	
	// Dricect3D 객체를 생성한다.
	m_D3D = new D3DClass;
	if (!m_D3D) {
		return false;
	}

	// Direct3D 객체를 초기화한다.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 객체 생성
	m_Camera = new CameraClass();
	if (!m_Camera) {
		return false;
	}

	// 모델 객체 생성 및 초기화
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/cube.txt", L"../Engine/data/seafloor.dds", L"../Engine/data/bump03.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK); 
		return false;
	}

	m_WindowModel = new ModelClass();
	if (!m_WindowModel) {
		return false;
	}

	result = m_WindowModel->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/ice01.dds", L"../Engine/data/icebump01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}

	// 반사 굴절 텍스처를 만들기 위한 RTT 객체 2개 생성
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// ShaderClass 생성

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_GlassShader = new GlassShaderClass;
	if (!m_GlassShader) {
		return false;
	}

	result = m_GlassShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 셰이더 객체 해제
	if (m_GlassShader) {
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = 0;
	}
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// RTT 객체 해제
	if (m_RenderTexture) { 
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}


	// 모델 객체 해제
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	if (m_WindowModel) {
		m_WindowModel->Shutdown();
		delete m_WindowModel;
		m_WindowModel = 0;
	}

	// 카메라 객체 해제
	if (m_Camera){
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D 장치 해제
	if (m_D3D){
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// 매 프레임마다 물의 위치를 갱신하여 물의 흐름을 표현한다.
bool GraphicsClass::Frame() {
	bool result;
	static float rotation = 0.0f;

	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f){
		rotation -= 360.0f;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	result = RenderToTexture(rotation); 
	if (!result) { 
		return false; 
	}

	result = Render(rotation); 
	if (!result) { 
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float rotation) {
	// 먼저 반사, 굴절 텍스처를 RTT를 통해 렌더링한다.
	// 그후 전체 씬을 백버퍼에 렌더링한다.
	// 만약 RTT로 인한 비용을 줄이고 싶다. 매 프레임마다 캡처하는 대신 15~30 프레임마다 캡쳐하는 방법이 있다.

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float refractionScale; 
	bool result;

	// 굴절 크기 초기화
	refractionScale = 0.05f;

	// 백버퍼 초기화
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 정육면체 모델 렌더링

	// 카메라 객체로부터 뷰행렬 생성
	m_Camera->Render();

	// 월드, 뷰, 투영행렬 가져옴
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 월드행렬 회전
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델을 그래픽 파이프라인에 넣음
	m_Model->Render(m_D3D->GetDeviceContext());

	// 모델 렌더링
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) { 
		return false;
	}

	// 월드행렬 리셋
	m_D3D->GetWorldMatrix(worldMatrix);

	// 유리 모델 렌더링
	
	//유리 모델 위치 지정
	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, -1.5f);

	// 유리 모델 그래픽파이프라인에 넣음
	m_WindowModel->Render(m_D3D->GetDeviceContext());

	result = m_GlassShader->Render(m_D3D->GetDeviceContext(), m_WindowModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WindowModel->GetTexture(), m_WindowModel->GetNormalMap(), m_RenderTexture->GetShaderResourceView(), refractionScale); 
	if (!result) { 
		return false;
	}

	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::RenderToTexture(float rotation) {
	XMMATRIX viewMatrix, worldMatrix, projectionMatrix;
	bool result;

	// 렌더타겟을 텍스처로 지정
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// 텍스처를 깨끗하게 만듬
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_D3D->GetDeviceContext());

	// 모델 렌더링
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	m_D3D->SetBackBufferRenderTarget();

	return true;
}