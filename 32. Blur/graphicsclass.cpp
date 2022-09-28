#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0; 
	m_Camera = 0; 
	m_Model = 0;
	m_SmallWindow = 0;
	m_FullScreenWindow = 0;
	m_TextureShader = 0;
	m_HorizontalBlurShader = 0;
	m_VerticalBlurShader = 0;
	m_RenderTexture = 0; 
	m_DownSampleTexure = 0;
	m_HorizontalBlurTexture = 0;
	m_VerticalBlurTexture = 0;
	m_UpSampleTexure = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other) {}

GraphicsClass::~GraphicsClass() {}

//GraphicsClass::Initiallize() 에서 D3D 객체를 초기화시킨다.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	int downSampleWidth, downSampleHeight;
	bool result;

	// 다운 샘플링 크기를 정한다. 현재는 절반
	downSampleWidth = screenWidth / 2; 
	downSampleHeight = screenHeight / 2;

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
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// 사각형 모델 생성 및 초기화
	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow) {
		return false;
	}

	result = m_SmallWindow->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow) {
		return false;
	}

	result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 셰이더 객체 생성 및 초기화 
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader) {
		return false;
	}

	result = m_HorizontalBlurShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader) {
		return false;
	}

	result = m_VerticalBlurShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// RTT 객체 생성 및 초기화
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		return false;
	}

	m_DownSampleTexure = new RenderTextureClass;
	if (!m_DownSampleTexure) {
		return false;
	}

	result = m_DownSampleTexure->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		return false;
	}

	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture) {
		return false;
	}

	result = m_HorizontalBlurTexture->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		return false;
	}

	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture) {
		return false;
	}

	result = m_VerticalBlurTexture->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		return false;
	}

	m_UpSampleTexure = new RenderTextureClass;
	if (!m_UpSampleTexure) {
		return false;
	}

	result = m_UpSampleTexure->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		return false;
	}
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// RTT 객체 반환
	if (m_UpSampleTexure) {
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}
	if (m_VerticalBlurTexture) {
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}
	if (m_HorizontalBlurTexture) {
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}
	if (m_DownSampleTexure) {
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}
	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// 셰이더 객체 반환
	if (m_VerticalBlurShader) {
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	if (m_HorizontalBlurShader) {
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 모델 객체 반환
	if (m_FullScreenWindow) {
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}

	if (m_SmallWindow){
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}

	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 카메라 객체 반환
	if (m_Camera){
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D 객체 반환
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
	static float rotation = 0.0f;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}

	result = Render(rotation);
	if (!result) {
		return false;
	}

	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// 장면을 렌더 텍스처에 그린다.
	result = RenderSceneToTexture(rotation); 
	if (!result) {
		return false;
	}

	// RTT를 가지고 다운 샘플링한다.
	result = DownSampleTexture(); 
	if (!result) { 
		return false; 
	}

	// 다운 샘플 RTT 수평 블러 셰이더를 렌더링한다.
	result = RenderHorizontalBlurToTexture(); 
	if (!result) {
		return false; 
	}

	// 수평 블러를 적용한 텍스처에 수직 블러 셰이더를 렌더링한다.
	result = RenderVerticalBlurToTexture();
	if (!result) { 
		return false;
	}

	// 최종 블러링된 텍스처를 업샘플링한다.
	result = UpSampleTexture(); 
	if (!result) { 
		return false;
	}

	// 업샘플링된 블러 텍스쳐를 화면에 그린다.
	result = Render2DTextureScene(); 
	if(!result) { 
		return false; 
	}

	return true;

}

// 블러 알고리즘의 첫번째 화면을 RTT에 그리는 함수
bool GraphicsClass::RenderSceneToTexture(float rotation){
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// RTT가 렌더링 타겟이 되도록한다.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// RTT를 초기화한다.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix); 
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) { 
		return false; 
	}

	// 렌더타깃을 다시 백버퍼로 돌린다.
	m_D3D->SetBackBufferRenderTarget();

	// 뷰포트를 원래대로 돌린다.
	m_D3D->ResetViewport();

	return true;
}


// Initialize 함수에서 화면 절반 크기로 만들었떤 작은 윈도우 모델에 RTT 텍스처를 다운 샘플링하는 함수
bool GraphicsClass::DownSampleTexture(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	m_DownSampleTexure->SetRenderTarget(m_D3D->GetDeviceContext());

	m_DownSampleTexure->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	m_SmallWindow->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView());
	if (!result){
		return false;
	}

	m_D3D->TurnZBufferOn();

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

// 다운 샘플 텍스처를 수평 블러 셰이더를 렌더링 하고 수직 블러 셰이더에 적용시킬 RTT를 만드는 함수
bool GraphicsClass::RenderHorizontalBlurToTexture(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;
	
	// 수평 블러 세이더에서 사용할 화면으 ㅣ너비값을 부동소수점 형태로 저장한다.
	screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	m_HorizontalBlurTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	m_HorizontalBlurTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	m_SmallWindow->Render(m_D3D->GetDeviceContext());

	result = m_HorizontalBlurShader->Render(m_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_DownSampleTexure->GetShaderResourceView(), screenSizeX);
	if (!result){
		return false;
	}

	m_D3D->TurnZBufferOn();

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

// 수직 블러 셰이더를 적용하는 함수 
bool GraphicsClass::RenderVerticalBlurToTexture(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;


	// 수직 블러 세이더에서 사용할 화면으 ㅣ너비값을 부동소수점 형태로 저장한다.
	screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	m_VerticalBlurTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	m_VerticalBlurTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	m_SmallWindow->Render(m_D3D->GetDeviceContext());

	result = m_VerticalBlurShader->Render(m_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);
	if (!result){
		return false;
	}

	m_D3D->TurnZBufferOn();

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

// 최종 블러가 적용된 RTT를 업샘플링 적용
bool GraphicsClass::UpSampleTexture(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	m_UpSampleTexure->SetRenderTarget(m_D3D->GetDeviceContext());

	m_UpSampleTexure->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView());
	if (!result){
		return false;
	}

	m_D3D->TurnZBufferOn();

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

// 마지막 함수는 최종 결과물을 화면에 뿌려주는 함수
bool GraphicsClass::Render2DTextureScene(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	m_D3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_UpSampleTexure->GetShaderResourceView());
	if (!result){
		return false;
	}

	m_D3D->TurnZBufferOn();

	m_D3D->EndScene();

	return true;
}






