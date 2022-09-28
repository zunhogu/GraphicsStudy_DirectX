#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_FloorModel = 0;
	m_ReflectShader = 0;
	m_RenderTexture = 0; 
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
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/Cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// Floor 모델 객체 생성
	m_FloorModel = new ModelClass();
	if (!m_FloorModel) {
		return false;
	}

	// Floor 모델 객체 초기화
	result = m_FloorModel->Initialize(m_D3D->GetDevice(), "../Engine/data/floor.txt", L"../Engine/data/blue01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

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

	// ReflectionShaderClass 객체 생성
	m_ReflectShader = new ReflectionShaderClass;
	if (!m_ReflectShader) {
		return false;
	}

	// ReflectionShaderClass 객체 초기화
	result = m_ReflectShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
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

	// Release the render to texture object.
	if (m_RenderTexture){
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the light shader object.
	if (m_ReflectShader){
		m_ReflectShader->Shutdown();
		delete m_ReflectShader;
		m_ReflectShader = 0;
	}

	// Release the model object.
	if (m_Model){
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_FloorModel) {
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
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

	// 반사된 장면을 텍스처에 그린다.
	result = RenderToTexture(); 
	if (!result) { 
		return false; 
	}

	// 백버퍼에 일반 장면을 그린다.
	result = RenderScene();
	if (!result){
		return false;
	}

	return true;
}

// RenderToTexture() 함수는 렌더타겟을 텍스처롤 설정하고, 렌더링 이후에는 렌더 타겟을 백버퍼로 돌린다.
bool GraphicsClass::RenderToTexture(){
	
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	bool result;
	static float rotation = 0.0f;

	// RTT가 렌더링 타겟이 되도록한다.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// RTT를 초기화한다.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// 기존의 카메라가 아닌 새로운 뷰행렬을 만든다.
	m_Camera->RenderReflection(-1.5f);

	// 만든 뷰행렬을 가져온다. 
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 매 프레임마다 회전값을 계산한다.
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델을 그래픽 파이프라인에 넣는다.
	m_Model->Render(m_D3D->GetDeviceContext());

	// 모델을 렌더링한다.
	result =m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	// 렌더타깃을 다시 백버퍼로 돌린다.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

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

	// 매 프레임마다 회전값을 계산한다.
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f){
		rotation -= 360.0f;
	}
	worldMatrix = XMMatrixRotationY(rotation);

	// 일반 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 넣어 렌더링 할 준비를 한다.
	m_Model->Render(m_D3D->GetDeviceContext());

	// 일반 모델을 렌더링을 한다.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result){
		return false;
	}

	// 월드행렬을 다시 가져와 육면의 아래에 바닥 모델을 그리기 위해 아래로 이동시킨다.
	m_D3D->GetWorldMatrix(worldMatrix); 
	worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);
	
	// 카메라 반사 뷰 행렬을 가져온다.
	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	// 바닥 모델을 그래픽 파이프라인에 넣는다.
	m_FloorModel->Render(m_D3D->GetDeviceContext());

	// 바닥 모델을 그린다.
	result = m_ReflectShader->Render(m_D3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), reflectionMatrix);
	if (!result) {
		return false;
	}

	// 렌더링이 완료된 장면을 화면에 뿌린다.
	m_D3D->EndScene();

	return true;
}