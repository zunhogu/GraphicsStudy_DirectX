#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
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

	// 텍스트 객체 생성
	m_Text = new TextClass();
	if (!m_Text) {
		return false;
	}

	// 비트맵 객체 초기화
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK); 
		return false;
	}
	
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 텍스트 객체 반환
	if(m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
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
bool GraphicsClass::Frame(int mouseX, int mouseY) {
	bool result;

	// 마우스 입력값을 매 프레임마다 체크해서 마우스 값을 갱신한다.
	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result) {
		return false;
	}

	// 
	result = Render();
	if (!result) {
		return false;
	}
	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
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

	// 2D 렌더링을 하기 위해서는 정사영 행렬을 구한다. 이는 일반 투영행렬대신 전달될 것이다.
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌덜링을 시작하기 전에 Z버퍼를 끈다. 
	m_D3D->TurnZBufferOff();

	// 텍스트 배경과 조화롭게 그려지도록 알파 블렌딩을 켠다.
	m_D3D->TurnOnAlphaBlending();

	// 그리고 나서 화면의 (100, 100) 위치에 비트맵을 그린다. 이 위치는 임의로 바꿀 수 있다.
	 result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix); 
	 if(!result) {
		 return false;
	 }
	 // 알파 블렌딩을 끈다. 
	 m_D3D->TurnOffAlphaBlending();

	// 2D 렌더링이 끝나면 3D 객체를 그리기위해서 Z버퍼를 다시 켠다.
	m_D3D->TurnZBufferOn();

	// 씬 그리기가 완료되면 버퍼에 그려진 씬을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}