#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
	m_RenderTexture = 0;
	m_Bitmap = 0;
	m_FadeShader = 0;
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

	// 모델 객체 생성
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}

	// 모델 객체 초기화
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/Cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	// TextureShaderClass 객체 생성
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// TextureShaderClass 객체 초기화
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// RTT 객체 생성
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	// RTT 객체 초기화
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// 비트맵 객체 생성
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap) {
		return false;
	}

	// 비트맵 객체 초기화
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// 페이드 변수들을 설정한다.
	m_fadeInTime = 3000.0f;

	// 누적 시간을 0ms로 초기화
	m_accumulatedTime = 0;

	// 페이드 진행률을 0%로 하여 화면이 검게 나오도록 한다.
	m_fadePercentage = 0;

	// 페이드 이펙트가 끝나지 않은것으로 표시한다.
	m_fadeDone = false;

	// 페이드 객체생성
	m_FadeShader = new FadeShaderClass();
	if (!m_FadeShader) {
		return false;
	}
	// 페이드 객체 초기화
	result = m_FadeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

// GraphicsClass::Shutdown() 에서는 모든 그래픽 객체에 대한 해제가 일어나는 함수이기 때문에 D3DClass도 이 함수에서 해체된다.
// 여기서 신경쓸 부분은 m_D3D가 초기화 되었는지 확인하는 부분인데 만약 초기화 되지 않으면 설정되지 않은것으로 간주하고 정리하지 않을것이다.
// 모든 포인터를 null로 초기화하는 것이 왜 중요한지 알 수 있다. 
void GraphicsClass::Shutdown() {
	// 페이드 객체 반환
	if (m_FadeShader) {
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}
	// 비트맵 객체 반환
	if (m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	// RTT 객체 반환
	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	// 셰이더 객체 반환
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 모델 객체 반환
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 카메라 객체 반환
	if (m_Camera) {
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

// Frame()에서는 페이드 인 효과의 진행을 관장한다. 매 프레임마다 프레임 간 시간이 함수의 인자로 전달되고 텍스처가 어느 만큼 보여야할지 계산된다.
// 누적된 프레임 시간이 페이드 인 시간을 넘게되면 m_FadeDone 변수도 true로 바꾸어 페이드 효과를 종료시키기도 한다.
bool GraphicsClass::Frame(float frameTime) {
	if (!m_fadeDone) {
		// 누적시간을 기록한다.
		m_accumulatedTime += frameTime;

		// 시간이 지나면서 페이드 효과의 진행률이 증가하도록 한다.
		if (m_accumulatedTime < m_fadeInTime) {
			// 누적된 시간을 이용하여 어느정도까지 진행되었는지 계산한다.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else {
			// 페이드 인 시간이 지난 경우 페이드 효과를 끈다.
			m_fadeDone = true;
			// 진행률은 100%
			m_fadePercentage = 1.0f;
		}
	}

	// 카메라의 위치를 설정
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}

//Render()함수는 육면체의 회전뿐만아니라 렌더 투 텍스쳐와 페이드 인 효과를 전환을 담당하여 페드 효과가 끝나면 일반 장면을 그리는 기능을 담당한다.
bool GraphicsClass::Render() {
	bool result;
	static float rotation = 0.0f;

	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}

	if (m_fadeDone) {
		// 페이드 효과가 끝나면 백버퍼로 일반장면을 그린다.
		RenderNormalScene(rotation);
	}
	else {
		// 페이드 효과가 끝나지 않았따면 RTT로 그려낸 뒤 텍스처에 페이드 효과를 준다.
		result = RenderToTexture(rotation);
		if (!result) {
			return false;
		}

		result = RenderFadingScene();
		if (!result) {
			return false;
		}
	}
	return true;
}

// RenderToTexture 함수는 회전하는 육면체를 백버퍼가 아닌 텍스처에 그리도록한다.
bool GraphicsClass::RenderToTexture(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// RTT가 렌더링 타겟이 되도록한다.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// 스텐실 뷰의 내용을 초기화한다.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 위치를 이용하여 뷰행렬을 만든다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰, 투영 행렬을 구한다.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 매 프레임마다 월드 행렬을 회전값만큼 회전시킨다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델을 그래픽 파이프라인에 넣는다.
	m_Model->Render(m_D3D->GetDeviceContext());

	// 모델을 렌더링한다.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	// 렌더타깃을 다시 백버퍼로 돌린다.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}


// RenderFAdingScene 함수는 렌더 투 텍스처로 그려진 텍스처를 FadeShaderClass를 가지고 마치 2D 비트맵처럼 화면에 그려넣는다.
// FadeShaderClas 객체는 m_fadePercentage의 값을 이용하여 매 프레임마다 어느정도 밝기가 들어가야하는지 알아낸다.
bool GraphicsClass::RenderFadingScene() {
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// 화면을 그리기 위하여 화면을 초기화한다.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 위치를 이용하여 뷰 행렬을 생성한다.
	m_Camera->Render();

	// 카메라, D3D 객체에서 월드, 뷰, 직교 행렬을 가져온다.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 할것이므로 Z버퍼를 끈다.
	m_D3D->TurnZBufferOff();

	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) {
		return false;
	}

	// 페이드 셰이더를 이용하여 비트맵을 그린다. 
	result = m_FadeShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_fadePercentage);
	if (!result) {
		return false;
	}

	// 2D 렌더링이 끝나면 Z 버퍼를 다시 켜준다.
	m_D3D->TurnZBufferOn();

	// 그려진 렌더타겟을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}

// RenderNormalScene 함수는 페이딩이 아닌 일반화면을 그려주는 함수다.
bool GraphicsClass::RenderNormalScene(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 백버퍼 초기화
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 객체를 통해 뷰행렬 생성
	m_Camera->Render();

	// 카메라, d3d 객체를 가지고 월드, 뷰, 투영행렬 구한다.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 월드행렬에 회전값을 곱해준다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델을 그래픽 파이프라인에 넣어준다.
	m_Model->Render(m_D3D->GetDeviceContext());

	// 렌더링을 한다 
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	// 그려진 렌더타겟을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}