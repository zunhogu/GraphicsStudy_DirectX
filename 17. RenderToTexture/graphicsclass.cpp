#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_RenderTexture = 0; 
	m_DebugWindow = 0; 
	m_TextureShader = 0;
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

	// 모델 객체 생성
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	// 모델 객체 초기화
	// 모델의 Initialize는 모델을 그리는 데 사용되는 텍스처를 인자로 받는다. 
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/Cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// LightShaderClass 객체 생성
	m_LightShader = new LightShaderClass;
	if(!m_LightShader) {
		return false; 
	} 
	
	// LightShaderClass 객체 초기화
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// LightClass 객체 생성
	m_Light = new LightClass;
	if (!m_Light) {
		MessageBox(hwnd, L"Could not initialize the light object.", L"Error", MB_OK);
		return false;
	}

	// LightClass 객체 초기화
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 0.0f);
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 0.0f);
	
	// RTT 객체를 생성한다.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	// RTT 객체를 초기화한다.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}
	
	// 디보그 윈도우를 생성한다.
	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow) {
		return false;
	}

	// 디버그 윈도우를 초기화한다.
	result = m_DebugWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, 100, 100); 
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK); 
		return false; 
	}

	// 텍스쳐 셰이더를 생성한다.
	m_TextureShader = new TextureShaderClass; 
	if(!m_TextureShader) {
		return false; 
	} 
	
	// 텍스쳐 셰이더를 초기화한다.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd); 
	if(!result) { 
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK); 
		return false;
	}


	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// Release the texture shader object.
	if (m_TextureShader){
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the debug window object.
	if (m_DebugWindow){
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture){
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the light object.
	if (m_Light){
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader){
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the model object.
	if (m_Model){
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera){
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D){
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// Frame()도 매 프레임마다 Render 함수를 부르도록 바꾼다.
bool GraphicsClass::Frame() {
	bool result;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// 전체 씬을 텍스처에 그린다.
	result = RenderToTexture(); 
	if (!result) { 
		return false; 
	}

	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지운다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 백버퍼에 평소처럼 전체 씬을 그린다.
	result = RenderScene();
	if (!result){
		return false;
	}

	// 2D 렌덜링을 하기 위해 Z버퍼를 끈다.
	m_D3D->TurnZBufferOff();

	// 카메라와 d3d 객체로부터 월드, 뷰, 직교 행렬을 얻어온다.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 디버그 윈도우의 정점과 인덱스 버퍼를 그래픽 파이프라인에 넣어 렌더링할 준비를한다.
	result = m_DebugWindow->Render(m_D3D->GetDeviceContext(), 50, 50);
	if (!result){
		return false;
	}

	// 미리 그린 텍스처를 가지고 텍스처셰이더 불러와 윈도우에 그린다.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView());
	if (!result){
		return false;
	}

	// 2D 렌더링이 끝났으면 다시 Z버퍼를 켠다.
	m_D3D->TurnZBufferOn();

	// 화면에 렌더링된 씬을 표시한다.
	m_D3D->EndScene();

	return true;
}

// RenderToTexture() 함수는 렌더타겟을 텍스처롤 설정하고, 렌더링 이후에는 렌더 타겟을 백버퍼로 돌린다.
bool GraphicsClass::RenderToTexture(){
	bool result;

	// RTT가 렌더링 타겟이 되도록한다.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// RTT를 초기화한다.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// 여기서 씬을 그리면 백버퍼 대신 RTT에 렌더링 된다. 
	result = RenderScene();
	if (!result){
		return false;
	}

	// 렌더타깃을 다시 백버퍼로 돌린다.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderScene(){
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	static float rotation = 0.0f;

	// 카메라의 위치에 근거하여 뷰행렬을 생성한다.
	m_Camera->Render();

	// 카메라와 d3d 객체에서 월드, 뷰, 월드 행렬을 얻어온다.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 매 프레임마다 회전값을 계산한다.
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f){
		rotation -= 360.0f;
	}
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 넣어 렌더링 할 준비를 한다.
	m_Model->Render(m_D3D->GetDeviceContext());

	// 렌더링을 한다.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Camera->GetPosition());
	if (!result){
		return false;
	}

	return true;
}