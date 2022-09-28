#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model1 = 0;
	m_Model2 = 0;
	m_TextureShader = 0;
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

	// 첫번째 모델 객체 생성
	m_Model1 = new ModelClass; 
	if(!m_Model1) {
		return false;
	}

	// 첫번째 모델 객체 초기화
	result = m_Model1->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/dirt01.dds");
	if(!result) { 
		MessageBox(hwnd, L"Could not initialize the first model object.", L"Error", MB_OK); 
		return false; 
	}

	// 두번째 모델 객체 생성
	m_Model2 = new ModelClass;
	if (!m_Model2) {
		return false;
	}

	// 두번째 모델 객체 초기화
	result = m_Model2->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/stone01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the first model object.", L"Error", MB_OK);
		return false;
	}

	// TextureShaderClass 객체 생성
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader) {
		return false; 
	} 
	
	// TextureShaderClass 객체 초기화
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// 투명 셰이더 객체 생성
	m_TransparentShader = new TransparentShaderClass;
	if (!m_TransparentShader) {
		return false;
	}

	result = m_TransparentShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK); 
		return false;
	}
	
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 투명 셰이더 객체 반환
	if (m_TransparentShader) {
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	// 텍스처 셰이더 객체 반환
	if(m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	} 

	// 모델 객체 반환
	if(m_Model1) {
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
	} 

	if (m_Model2) {
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
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
bool GraphicsClass::Frame() {
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	float blendAmount;

	// 블렌딩 값 설정
	blendAmount = 0.5f;

	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지운다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치를 토대로 뷰행렬을 만들기 위해서 카메라의 Render함수를 호출한다.
	m_Camera->Render();

	// 뷰행령이 만들어지고 나면 뷰행렬의 복사본을 가져올 수 있다.
	// 또한 D3DClass 객체로 부터 월드행렬과 투영행렬도 복사해온다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 모델의 정점 및 인덱스 버퍼를 그래픽 파이프라인에 넣어 그릴 준비를 한다.
	m_Model1->Render(m_D3D->GetDeviceContext());

	// 첫번째 모델을 그린다.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model1->GetTexture()); 
	if (!result) {
		return false;
	}

	// 월드 행렬을 오른쪽으로 1, 카메라를 향해 1만큼 이동시킨다.
	// 이를통해 두번째로 그려질 모델의 위치를 조절한다.
	worldMatrix = XMMatrixTranslation(1.0f, 0.0f, -1.0f);


	// 투명 효과가 동작하게끔 알파블렌딩을 켠다.
	m_D3D->TurnOnAlphaBlending();

	// 두번째 모델을 그래픽 파이프라인에 넣는다.
	m_Model2->Render(m_D3D->GetDeviceContext());

	// 두번째 모델을 그린다.
	result = m_TransparentShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(), blendAmount); 
	if (!result) { 
		return false; 
	}

	// 알파블렌딩을 끈다.
	m_D3D->TurnOffAlphaBlending();

	// 씬 그리기가 완료되면 버퍼에 그려진 씬을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}