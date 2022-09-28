#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_ModelList = 0;
	m_Frustum = 0;
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

	// 모델 객체 생성
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	
	// 모델 객체 초기화
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/data/sphere.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Light 객체 생성
	m_Light = new LightClass; 
	if (!m_Light) {
		return false; 
	} 
	
	// Light 객체 초기화
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// LightShader 객체 생성
	m_LightShader = new LightShaderClass; 
	if (!m_LightShader) {
		return false;
	} 
	
	// LightShader 객체 초기화 
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); 
		return false; 
	}

	// ModelList 객체 생성
	m_ModelList = new ModelListClass();
	if (!m_ModelList) {
		return false;
	}

	// ModelList 객체 초기화
	result = m_ModelList->Initialize(25);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// Frsutum 객체 생성, 매 프레임마다 ConstructFrustum 함수를 호출할 것이기 때문에 초기화함수는 필요하하지않다.
	m_Frustum = new FrustumClass;
	if (!m_Frustum) {
		return false; 
	}
	
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {

	// Frustum 객체 반환
	if(m_Frustum) { 
		delete m_Frustum;
		m_Frustum = 0; 
	}

	// ModelList 객체 반환
	if (m_ModelList) {
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// Light Shader 객체 반환
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Light 객체 반환
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	// 모델 객체 반환
	if (m_Model) {
		m_Model->Shutdown(); 
		delete m_Model;
		m_Model = 0; 
	}

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
// 프레임 함수에서는 SystemClass로 부터 카메라의 회전 값을 인자로 받는다. Render 함수에서 뷰 행렬이 제대로 갱신
bool GraphicsClass::Frame(float rotationY) {
	bool result;

	// 카메로 위치 입력 
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// 카메라 회전값 설정
	m_Camera->SetRotation(0.0f, rotationY, 0.0f); 
	
	return true;
}

//Render()함수는 실제 윈도우창에 그림을 그리는 함수다.
bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	XMFLOAT4 color;
	bool renderModel, result;

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

	// Render 함수의 큰 변화는 매 프레임마다 뷰 행렬에 근거하여 시야 프러스텀을 구축하는 것이다.
	// 이 구축 과정은 뷰행렬이 바뀌거나 우리의 프러스텀 확인이 맞지 않을때 발생한다.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 모델의 개수를 입력받는다.
	modelCount = m_ModelList->GetModelCount();

	// ModelList에서 그릴 모델의 개수를 카운트 
	renderCount = 1;

	// ModelListClass 객체의 모든 객체를 돌아본다.
	for (index = 0; index < modelCount; index++) {
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// 구체들의 반지릉을 설정 이 반지름이 커지면 커질수록 모델이 시야 프러스텀에 들어갈 확률이 높아지지만, 너무 높으면 모든 모델이 프러스텀 내부로 들어가는 것처럼 설정된다.
		radius = 1.0f;

		// 구체가 현재 시야 프러스텀에 보이는지를 체크하는 코드
		// 만약 프로스텀 내부에 있따면 그리고, 내부에 없다면 그리지 않고 넘어간다. 
		renderModel = m_Frustum->CheckCube(positionX, positionY, positionZ, radius);

		if (renderModel) {  // 시야 프러스텀 내부의 모델이면 이 if문 진입

			// worldMatrix를 수정해서 모델을 움직인다. 
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// 만들어진 모델의 정점 / 인덱스 버퍼를 어셈블리 버퍼로 활성화
			m_Model->Render(m_D3D->GetDeviceContext());

			// 정점 / 인덱스 버퍼를 정점 입력레이아웃에 넣어 셰이더로 전달, 전달 받은 셰이더는 이를 통해 모델을 그림
			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), color, m_Light->GetDirection());
			
			// worldMatrix를 새로 고침
			m_D3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}

	// 실제로 몇 개의 구체들이 그려지는지 화면에 보여주기 위해 TextClass를 약간 바꾼다.
	// FrustumClass를 통해 그 숫자를 가져올 수 있는데, 그려지는 궤 수 대신 컬링되어 보이지 않게 된 숫자도 표시할 수 있다.
	
	result = m_Text->SetRenderCount(renderCount, m_D3D->GetDeviceContext());
	if (!result){
		return false;
	}

	// 2D 렌덜링을 시작하기 전에 Z버퍼를 끈다. 
	m_D3D->TurnZBufferOff();

	//  텍스트 배경과 조화롭게 그려지도록 알파 블렌딩을 켠다.
	m_D3D->TurnOnAlphaBlending();

	//그리고 나서 화면의 (100, 100) 위치에 비트맵을 그린다. 이 위치는 임의로 바꿀 수 있다.
	m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result){
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