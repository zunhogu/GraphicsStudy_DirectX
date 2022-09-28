#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;
	m_FloorModel = 0;
	m_BillboardModel = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) {
}


GraphicsClass::~GraphicsClass() {
}


//GraphicsClass::Initiallize() 에서 D3D 객체를 초기화시킨다.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;
	XMMATRIX baseViewMatrix;

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

	// 카메라 객체로 부터 새로운 뷰 행렬을 생성하여 TextClass가 사용하게 해야한다.
	// 이를 통하여 글자가 항상 화면의 같은 위치에 그려지게 된다.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);


	// 바닥 모델 객체 생성
	m_FloorModel = new ModelClass;
	if (!m_FloorModel) {
		return false;
	}
	
	// 바닥 모델 객체 초기화
	result = m_FloorModel->Initialize(m_D3D->GetDevice(), "../Engine/data/floor.txt", L"../Engine/data/grid01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 빌보드 모델 객체 생성
	m_BillboardModel = new ModelClass;
	if (!m_BillboardModel) {
		return false;
	}
	
	// 빌보드 모델 객체 초기화
	result = m_BillboardModel->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	// LightShader 객체 생성
	m_TextureShader = new TextureShaderClass; 
	if (!m_TextureShader) {
		return false;
	} 
	
	// LightShader 객체 초기화 
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); 
		return false; 
	}
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// Light Shader 객체 반환
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 모델 반환
	if (m_BillboardModel) {
		m_BillboardModel->Shutdown();
		delete m_BillboardModel;
		m_BillboardModel = 0;
	}
	if (m_FloorModel) {
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
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
// 프레임 함수에서는 SystemClass로 부터 카메라의 회전 값을 인자로 받는다. Render 함수에서 뷰 행렬이 제대로 갱신
bool GraphicsClass::Frame(float positionX, float positionY, float positionZ){
	bool result;

	// 카메로 위치 입력 
	m_Camera->SetPosition(positionX, positionY, positionZ);

	result = Render();
	if (!result) { 
		return false;
	}
	
	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;
	double angle; float rotation;
	bool result;

	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지운다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치를 토대로 뷰행렬을 만들기 위해서 카메라의 Render함수를 호출한다.
	m_Camera->Render();

	// 뷰행령이 만들어지고 나면 뷰행렬의 복사본을 가져올 수 있다.
	// 또한 D3DClass 객체로 부터 월드행렬과 투영행렬도 복사해온다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_FloorModel->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture()); 
	if (!result) {
		return false;
	}

	// 카메라의 위치를 얻어온 뒤 빌보드 모델의 위치를 설정한다.
	cameraPosition = m_Camera->GetPosition(); 

	modelPosition.x = 0.0f; 
	modelPosition.y = 1.5f;
	modelPosition.z = 0.0f;

	// 카메라 위치를 이용하요 빌보드의 회전값을 계산하여 카메라와 마주볼수 있또록 한다. 
	// atan2 함수를 이용하여 빌보드 모델에 적용될 회전값을 계산합니다. 이렇게 하여 빌보드 모델이 현재 카메라 위치를 바라보게 한다.
	angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI); 
	
	// 회전각도를 라디안으로 변환한다.. 
	rotation = (float)angle * 0.0174532925f;

	// 월드 행렬을 이용하여 원좀에서의 빌보드 회전값을 계산한다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 빌보드 모델의 이동행렬을 설정한다.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// 회전행렬과 이동행렬을 조합하여 빌보드 모델의 최종 월드 행렬을 계산한다.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_BillboardModel->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BillboardModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BillboardModel->GetTexture()); 
	if (!result) { 
		return false;
	}

	m_D3D->EndScene();
	
	return true;

}