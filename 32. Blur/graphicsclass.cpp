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

//GraphicsClass::Initiallize() ���� D3D ��ü�� �ʱ�ȭ��Ų��.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	int downSampleWidth, downSampleHeight;
	bool result;

	// �ٿ� ���ø� ũ�⸦ ���Ѵ�. ����� ����
	downSampleWidth = screenWidth / 2; 
	downSampleHeight = screenHeight / 2;

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

	// �� ��ü ����
	m_Model = new ModelClass();
	if (!m_Model) {
		return false;
	}

	// �� ��ü �ʱ�ȭ
	// ���� Initialize�� ���� �׸��� �� ���Ǵ� �ؽ�ó�� ���ڷ� �޴´�. 
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// �簢�� �� ���� �� �ʱ�ȭ
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

	// ���̴� ��ü ���� �� �ʱ�ȭ 
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

	// RTT ��ü ���� �� �ʱ�ȭ
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

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// RTT ��ü ��ȯ
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

	// ���̴� ��ü ��ȯ
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

	// �� ��ü ��ȯ
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

	// ī�޶� ��ü ��ȯ
	if (m_Camera){
		delete m_Camera;
		m_Camera = 0;
	}

	// D3D ��ü ��ȯ
	if (m_D3D){
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

// Frame()�� �� �����Ӹ��� Render �Լ��� �θ����� �ٲ۴�.
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

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// ����� ���� �ؽ�ó�� �׸���.
	result = RenderSceneToTexture(rotation); 
	if (!result) {
		return false;
	}

	// RTT�� ������ �ٿ� ���ø��Ѵ�.
	result = DownSampleTexture(); 
	if (!result) { 
		return false; 
	}

	// �ٿ� ���� RTT ���� �� ���̴��� �������Ѵ�.
	result = RenderHorizontalBlurToTexture(); 
	if (!result) {
		return false; 
	}

	// ���� ���� ������ �ؽ�ó�� ���� �� ���̴��� �������Ѵ�.
	result = RenderVerticalBlurToTexture();
	if (!result) { 
		return false;
	}

	// ���� ������ �ؽ�ó�� �����ø��Ѵ�.
	result = UpSampleTexture(); 
	if (!result) { 
		return false;
	}

	// �����ø��� �� �ؽ��ĸ� ȭ�鿡 �׸���.
	result = Render2DTextureScene(); 
	if(!result) { 
		return false; 
	}

	return true;

}

// �� �˰����� ù��° ȭ���� RTT�� �׸��� �Լ�
bool GraphicsClass::RenderSceneToTexture(float rotation){
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// RTT�� ������ Ÿ���� �ǵ����Ѵ�.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// RTT�� �ʱ�ȭ�Ѵ�.
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

	// ����Ÿ���� �ٽ� ����۷� ������.
	m_D3D->SetBackBufferRenderTarget();

	// ����Ʈ�� ������� ������.
	m_D3D->ResetViewport();

	return true;
}


// Initialize �Լ����� ȭ�� ���� ũ��� ������� ���� ������ �𵨿� RTT �ؽ�ó�� �ٿ� ���ø��ϴ� �Լ�
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

// �ٿ� ���� �ؽ�ó�� ���� �� ���̴��� ������ �ϰ� ���� �� ���̴��� �����ų RTT�� ����� �Լ�
bool GraphicsClass::RenderHorizontalBlurToTexture(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;
	
	// ���� �� ���̴����� ����� ȭ���� �ӳʺ��� �ε��Ҽ��� ���·� �����Ѵ�.
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

// ���� �� ���̴��� �����ϴ� �Լ� 
bool GraphicsClass::RenderVerticalBlurToTexture(){
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;


	// ���� �� ���̴����� ����� ȭ���� �ӳʺ��� �ε��Ҽ��� ���·� �����Ѵ�.
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

// ���� ���� ����� RTT�� �����ø� ����
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

// ������ �Լ��� ���� ������� ȭ�鿡 �ѷ��ִ� �Լ�
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






