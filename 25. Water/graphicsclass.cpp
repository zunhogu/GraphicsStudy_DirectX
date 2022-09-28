#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0; 
	m_GroundModel = 0; 
	m_WallModel = 0; 
	m_BathModel = 0; 
	m_WaterModel = 0;
	m_Light = 0;
	m_RefractionTexture = 0; 
	m_ReflectionTexture = 0; 
	m_LightShader = 0;
	m_RefractionShader = 0;
	m_WaterShader = 0;
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
	m_GroundModel = new ModelClass();
	if (!m_GroundModel) {
		return false;
	}

	result = m_GroundModel->Initialize(m_D3D->GetDevice(),"../Engine/data/ground.txt", L"../Engine/data/ground01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK); 
		return false;
	}

	m_WallModel = new ModelClass();
	if (!m_GroundModel) {
		return false;
	}

	result = m_WallModel->Initialize(m_D3D->GetDevice(), "../Engine/data/wall.txt", L"../Engine/data/wall01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}

	m_BathModel = new ModelClass();
	if (!m_BathModel) {
		return false;
	}

	result = m_BathModel->Initialize(m_D3D->GetDevice(), "../Engine/data/bath.txt", L"../Engine/data/marble01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	m_WaterModel = new ModelClass();
	if (!m_WaterModel) {
		return false;
	}

	result = m_WaterModel->Initialize(m_D3D->GetDevice(), "../Engine/data/water.txt", L"../Engine/data/water01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// Light 객체 생성 및 초기화
	m_Light = new LightClass;
	if(!m_Light) {
		return false;
	} 

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f); 
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
	m_Light->SetDirection(0.0f, -1.0f, 0.5f);

	// 반사 굴절 텍스처를 만들기 위한 RTT 객체 2개 생성
	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture) {
		return false;
	}
	result = m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	m_RefractionTexture = new RenderTextureClass;
	if (!m_RefractionTexture) {
		return false;
	}

	result = m_RefractionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// RefractionShaderClass 객체 생성
	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader) {
		return false;
	}

	// RefractionShaderClass 객체 초기화
	result = m_RefractionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}

	// LightShaderClass를 생성한다.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader) {
		return false; 
	} 
	
	// LightShaderClass를 초기화한다.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) { 
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); 
		return false;
	}

	// WaterShaderClass를 생성한다.
	m_WaterShader = new WaterShaderClass;
	if (!m_WaterShader) {
		return false;
	}

	// WaterShaderClass를 초기화한다.
	result = m_WaterShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the water shader object.", L"Error", MB_OK);
		return false;
	}
	// Set the height of the water.
	m_waterHeight = 2.75f;

	// Initialize the position of the water.
	m_waterTranslation = 0.0f;
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 셰이더 객체 해제
	if (m_WaterShader) {
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}
	if (m_RefractionShader) {
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}

	// RTT 객체 해제
	if (m_RefractionTexture) { 
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0; 
	}
	if (m_ReflectionTexture) {
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// Light 객체 해제
	if (m_Light) {
		delete m_Light;
		m_Light = 0; 
	}

	// 모델 객체 해제
	if (m_WaterModel) {
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = 0;
	}
	if (m_BathModel) {
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = 0;
	}
	if (m_WallModel){
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = 0;
	}

	if (m_GroundModel) {
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
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

	m_waterTranslation += 0.001f; 
	if (m_waterTranslation > 1.0f) { 
		m_waterTranslation -= 1.0f;
	}

	m_Camera->SetPosition(-15.0f, 10.0f, -22.0f);
	m_Camera->SetRotation(0.0f, 35.0f, 0.0f);

	return true;
}

bool GraphicsClass::Render() {
	// 먼저 반사, 굴절 텍스처를 RTT를 통해 렌더링한다.
	// 그후 전체 씬을 백버퍼에 렌더링한다.
	// 만약 RTT로 인한 비용을 줄이고 싶다. 매 프레임마다 캡처하는 대신 15~30 프레임마다 캡쳐하는 방법이 있다.

	bool result;

	result = RenderRefractionToTexture();
	if (!result) {
		return false;
	}

	result = RenderReflectionToTexture();
	if (!result) {
		return false;
	}

	result = RenderScene();
	if (!result) {
		return false;
	}

	return true;
}

// RenderRefractionToTexture 함수는 굴절된 물 아래 장면만 텍스처에 그린다.
bool GraphicsClass::RenderRefractionToTexture() {
	XMFLOAT4 clipPlane;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 클리핑 평면을 사용해서 물 위에 있는 모든 것들은 잘라내고 그 아래에 있는 것만 그린다.
	// 하지만 클리핑 평면이 물보다 살짝 위쪽에 가도록 한다는걸 주목해야한다.
	// 그 이유는 경계부분에 노말 맵으로 변형시킨 샘플링 위치가 검게 보이는 문제가 있기 때문이다.
	// 클리핑 평면을 살짝 위로 들어올려서 더 많은 색상이 모이게끔 할 수 있다.
	// 이 프로그램을 실행할때 클리핑 평면의 높이를 물 높이와 맞춰 보면 그 결함이 자주 일어남을 확인할 수 있다.
	clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

	// 렌더타겟을 텍스처로 지정
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// 텍스처를 깨끗하게 만듬
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 객체를 통해 뷰행렬을 만듬, 굴절과 같은 경우는 일반 뷰행렬만 사용해도 된다.
	m_Camera->Render();

	// 카메라 객체와 D3D 객체를 통해 월드, 뷰, 투영 행렬을 받아옴
	m_D3D->GetWorldMatrix(worldMatrix); 
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the bath model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// 욕조 모델을 그래픽 파이프라인에 넣는다.
	m_BathModel->Render(m_D3D->GetDeviceContext());

	// 장면을 렌더링한다.
	result = m_RefractionShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor(), clipPlane);
	if (!result) { 
		return false; 
	}

	// 렌더타겟을 백버퍼로 되돌린다.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

// RenderReflectionToTexture 함수는 반사된 장면을 텍스처에 그린다. 물 위에 있는 물체만 그리기 대문에 물에 반사되는 것처럼 보이는 벽모델만 그린다.
bool GraphicsClass::RenderReflectionToTexture() {
	XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	bool result;

	// 렌더타겟을 텍스처로 지정
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// 텍스처를 깨끗하게 만듬
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 일반 뷰행렬대신 반사행렬이 필요하다. 따라서 반사행렬을 설정한다.
	m_Camera->RenderReflection(m_waterHeight);

	// 월드행렬, 반사뷰행렬, 투영행렬을 가져온다.
	m_D3D->GetWorldMatrix(worldMatrix); 
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 월드행렬을 움직여 물체를 이동시켜준다.
	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

	// 벽 모델을 그래픽파이프라인에 넣는다.
	m_WallModel->Render(m_D3D->GetDeviceContext());

	// 장면을 렌더링한다.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor());
	if (!result) { 
		return false; 
	}

	// 렌더타겟을 백버퍼로 되돌린다.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

// RednerScene 함수는 백버퍼에 장면을 렌더링하는 함수다.
bool GraphicsClass::RenderScene(){
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;
	bool result;
	static float rotation = 0.0f;

	// 장면을 시작하기 위하여 버퍼를 초기화한다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 근거하여 뷰행렬을 생성한다.
	m_Camera->Render();

	// 카메라와 d3d 객체에서 월드, 뷰, 월드 행렬을 얻어온다.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 바닥 모델을 먼저 그린다.
	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

	m_GroundModel->Render(m_D3D->GetDeviceContext());

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor());
	if (!result) {
		return false;
	}

	// 월드 행렬 리셋
	m_D3D->GetWorldMatrix(worldMatrix);

	// 벽 모델을 그린다.
	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

	m_WallModel->Render(m_D3D->GetDeviceContext());

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix,projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor());
	if (!result){
		return false;
	}

	// 욕조 모델을 그린다.
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	m_BathModel->Render(m_D3D->GetDeviceContext());

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(),m_Light->GetAmbientColor());
	if (!result){
		return false;
	}


	// 물 모델을 그린다.
	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	worldMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);

	m_WaterModel->Render(m_D3D->GetDeviceContext());

	result = m_WaterShader->Render(m_D3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(), m_waterTranslation, 0.01f);
	if (!result){
		return false;
	}

	// 렌더링이 끝나면 화면에 표시한다.
	m_D3D->EndScene();


	return true;
}