#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0; 
	m_Light = 0;
	m_CubeModel = 0;
	m_GroundModel = 0;
	m_SphereModel = 0;
	m_RenderTexture = 0;
	m_DepthShader = 0; 
	m_ShadowShader = 0;


}


GraphicsClass::GraphicsClass(const GraphicsClass& other) {
}


GraphicsClass::~GraphicsClass() {
}


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

	// 카메라 객체 초기화
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// 조명 객체 생성 및 초기화
	m_Light = new LightClass;
	if (!m_Light) {
		return false;
	}

	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); 
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// 모델 객체 생성 및 초기화
	m_CubeModel = new ModelClass();
	if (!m_CubeModel) {
		return false;
	}

	result = m_CubeModel->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/wall01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK); 
		return false;
	}

	m_SphereModel = new ModelClass();
	if (!m_CubeModel) {
		return false;
	}
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);

	result = m_SphereModel->Initialize(m_D3D->GetDevice(), "../Engine/data/sphere.txt", L"../Engine/data/ice.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the sphere  model object.", L"Error", MB_OK);
		return false;
	}
	m_SphereModel->SetPosition(0.0f, 2.0f, -2.0f);

	m_GroundModel = new ModelClass();
	if (!m_CubeModel) {
		return false;
	}

	result = m_GroundModel->Initialize(m_D3D->GetDevice(), "../Engine/data/plane01.txt", L"../Engine/data/metal001.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the plane model object.", L"Error", MB_OK);
		return false;
	}
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

	// RTT 객체 생성 및 초기화
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture){
		return false;
	}

	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// ShaderClass 객체 생성 및 초기화
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader){
		return false;
	}

	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader){
		return false;
	}

	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 셰이더 객체 반환
	if(m_ShadowShader) {
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	} 
	if (m_DepthShader) {
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// RTT 객체 반환
	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// 모델 객체 반환
	if(m_GroundModel) {
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	} 
	if (m_SphereModel) {
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = 0;
	}
	if (m_CubeModel) {
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
	}

	// 조명 객체 반환
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	// 카메라 객체 반환
	if(m_Camera) { 
		delete m_Camera; m_Camera = 0; 
	}
	// Direct3D 객체 반환
	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// Frame()도 매 프레임마다 Render 함수를 부르도록 바꾼다.
bool GraphicsClass::Frame(float posX, float posY, float posZ, float rotX, float rotY, float rotZ){
	bool result;
	static float lightPositionX = -5.0f;

	// 카메라 위치 설정
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// 매 프레임마다 조명의 위치를 갱신한다.
	lightPositionX += 0.05f;
	if (lightPositionX > 5.0f) {
		lightPositionX = -5.0f;
	}

	// 조명의 위치 설정
	m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);

	// 렌더링
	result = Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::RenderSceneToTexture() {
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;
	bool result;

	// 렌더 타겟 변경
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext()); 
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에서 조명의 뷰 행렬을 생성한다.
	m_Light->GenerateViewMatrix();

	// d3d 객체에서 월드 행렬을 가져온다.
	m_D3D->GetWorldMatrix(worldMatrix);

	// 조명에서 뷰 및 직교 행렬을 구한다.
	m_Light->GetViewMatrix(lightViewMatrix); 
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// 육각형 모델의 이동행렬을 설정한다.
	m_CubeModel->GetPosition(posX, posY, posZ); 
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 육각형 모델을 그린다.
	m_CubeModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix); 
	if (!result) { 
		return false;
	}

	// 월드 행렬을 초기화한다.
	m_D3D->GetWorldMatrix(worldMatrix);

	// 구 모델의 이동 행렬을 설정한다.
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 구 모델을 그린다.
	m_SphereModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) {
		return false;
	}

	// 월드 행렬을 초기화한다.
	m_D3D->GetWorldMatrix(worldMatrix);

	// 바닥 모델의 이동 행렬을 설정한다.
	m_GroundModel->GetPosition(posX, posY, posZ); 
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 바닥 모델을 그린다.
	m_GroundModel->Render(m_D3D->GetDeviceContext()); 
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) { 
		return false;
	}

	// 렌더 타겟을 백버퍼로 돌린다.
	m_D3D->SetBackBufferRenderTarget();

	// 뷰포트 원래대로 돌린다.
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;
	float posX, posY, posZ;

	// 우선 장면을 텍스처에 그린다.
	result = RenderSceneToTexture(); 
	if (!result) { 
		return false; 
	}

	// 버퍼의 내용을 지워 장면을 시작한다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라객체로부터 뷰행렬을 생성한다.
	m_Camera->Render();

	// 조명의 위치에서 조명 뷰, 투영 행렬을 생성한다.
	m_Light->GenerateViewMatrix();

	// 카메라, d3d 객체로부터 월드, 뷰, 투영 행렬을 받아온다.
	m_Camera->GetViewMatrix(viewMatrix); 
	m_D3D->GetWorldMatrix(worldMatrix); 
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 조명 객체에서 조명 뷰, 투영 행렬을 받아온다.
	m_Light->GetViewMatrix(lightViewMatrix); 
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// 육각 형 모델의 이동행렬을 설정한다.
	m_CubeModel->GetPosition(posX, posY, posZ); 
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 육각형 모델을 그래픽 파이파이라인에 넣는다.
	m_CubeModel->Render(m_D3D->GetDeviceContext());

	// 육각형 모델 렌더링 시작
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_CubeModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()); 
	if (!result) {
		return false;
	}

	// 월드 행렬 초기화
	m_D3D->GetWorldMatrix(worldMatrix);

	// 구 모델의 이동행렬을 설정한다.
	m_SphereModel->GetPosition(posX, posY, posZ);

	// 구 모델을 그래픽 파이파이라인에 넣는다.
	m_SphereModel->Render(m_D3D->GetDeviceContext());
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 구 모델 렌더링 시작
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_SphereModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result) {
		return false;
	}

	// 월드 행렬 초기화
	m_D3D->GetWorldMatrix(worldMatrix);

	// 바닥 모델의 이동행렬을 설정한다.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 바닥 모델을 그래픽 파이파이라인에 넣는다.
	m_GroundModel->Render(m_D3D->GetDeviceContext());

	// 바닥 모델 렌더링 시작
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_GroundModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result) {
		return false;
	}	
	
	// 월드 행렬 초기화
	m_D3D->GetWorldMatrix(worldMatrix);

	// 씬을 그린다.
	m_D3D->EndScene();

	return true;
}