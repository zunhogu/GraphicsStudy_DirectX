#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
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
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 셰이더 객체 반환
	if(m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	} 

	// 모델 객체 반환
	if(m_Model) { 
		m_Model->Shutdown(); 
		delete m_Model; m_Model = 0; 
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

	// 조명 객체 반환
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}
	return;
}

// Frame()도 매 프레임마다 Render 함수를 부르도록 바꾼다.
bool GraphicsClass::Frame() {
	bool result;

	// 조명의 효과를 더욱 부각하기 위해 회전값을 Render함수에 준다. 
	// 이 회전값은 매 프레임마다 바뀌므로 Frame 함수에다 전달해준다.

	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}

	// 그래픽 렌더링을 수행한다.
	result = Render(rotation);
	if (!result) {
		return false;
	}

	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
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

	// 여기서 월드행렬 값을 인자로 입력받은 회전값만큼 회전시킨다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 그러고 ModelClass::Render 함수를 호출하여 그래픽 파이프라인에서 삼각형 모델을 그리도록 한다. 
	m_Model->Render(m_D3D->GetDeviceContext());

	// 이제는 셰이더를 호출하여 준비된 정점과 세 행렬을 사용하여 정점들을 그려낸다. 그러면 삼각형이 백버퍼에 그려지게 된다.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
	if (!result) { 
		return false;
	}

	// 씬 그리기가 완료되면 버퍼에 그려진 씬을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}