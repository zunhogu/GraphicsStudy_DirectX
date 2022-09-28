#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_FireShader = 0;
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
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/square.txt", L"../Engine/data/fire01.dds", L"../Engine/data/noise01.dds", L"../Engine/data/alpha01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK); 
		return false;
	}

	// TextureShaderClass 객체 생성
	m_FireShader = new FireShaderClass;
	if(!m_FireShader) {
		return false; 
	} 
	
	// TexturerClass 객체 초기화
	result = m_FireShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// 셰이더 객체 반환
	if(m_FireShader) {
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
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
	return;
}

// Frame()도 매 프레임마다 Render 함수를 부르도록 바꾼다.
bool GraphicsClass::Frame() {
	bool result;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f); 
	
	result = Render();
	if(!result) { 
		return false;
	} 
	
	return true;

}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result; 
	XMFLOAT3 scrollSpeeds, scales;
	XMFLOAT2 distortion1, distortion2, distortion3;
	float distortionScale, distortionBias;
	static float frameTime = 0.0f;

	// 매 프레임마다 frameTime 값을 증가시킨다. 이 값을 이용하여 셰이더 안에 있는 세 개의 다른 노이즈 텍스처를 스크롤한다.
	// 참고로 FPS를 60으로 고정하지 않는다면 매 프레임마다 시간의 차이를 감안하여 타이머를 업데이트하여 불꽃이 FPS에 상관없이 일정한 속도로 타오르르도록 해야한다.
	// 프레임 카운터 증가
	frameTime += 0.01f; 
	if (frameTime > 1000.0f) {
		frameTime = 0.0f; 
	}

	// 세 노이즈 텍스처의 스크롤 속도를 설정한다.
	scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// 세 크기값을 사용하여 세 가지의 다른 노이즈 옥타브 텍스쳐를 만든다.
	scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// 세 노이즈 텍스처의 서로 다른 세 개의 x, y 왜곡 인자를 설정한다.
	distortion1 = XMFLOAT2(0.1f, 0.2f);
	distortion2 = XMFLOAT2(0.1f, 0.3f);
	distortion3 = XMFLOAT2(0.1f, 0.1f);

	// 노이즈 텍스처를 불꽃 모양으로 교란시키는데 사용할 바이어스 크기 값을 설정한다.
	distortionScale = 0.8f; 
	distortionBias = 0.5f;

	// 씬 그리기를 시작하기 위해 버퍼의 내용을 지운다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치를 토대로 뷰행렬을 만들기 위해서 카메라의 Render함수를 호출한다.
	m_Camera->Render();

	// 뷰행령이 만들어지고 나면 뷰행렬의 복사본을 가져올 수 있다.
	// 또한 D3DClass 객체로 부터 월드행렬과 투영행렬도 복사해온다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	/// 불꽃의 반투명을 위해 알파블렌딩을 켠다.
	m_D3D->TurnOnAlphaBlending();

	// 그러고 ModelClass::Render 함수를 호출하여 그래픽 파이프라인에서 삼각형 모델을 그리도록 한다. 
	m_Model->Render(m_D3D->GetDeviceContext());

	// 이제는 셰이더를 호출하여 준비된 정점과 세 행렬을 사용하여 정점들을 그려낸다. 그러면 삼각형이 백버퍼에 그려지게 된다.
	result = m_FireShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,m_Model->GetTextureArray(), frameTime, scrollSpeeds,scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
	if (!result) { 
		return false;
	}

	// 알파 블렌딩을 끈다. 
	m_D3D->TurnOffAlphaBlending();


	// 씬 그리기가 완료되면 버퍼에 그려진 씬을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}