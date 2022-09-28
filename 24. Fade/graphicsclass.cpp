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


//GraphicsClass::Initiallize() ���� D3D ��ü�� �ʱ�ȭ��Ų��.
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;

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
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}

	// �� ��ü �ʱ�ȭ
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/Cube.txt", L"../Engine/data/seafloor.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	// TextureShaderClass ��ü ����
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// TextureShaderClass ��ü �ʱ�ȭ
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// RTT ��ü ����
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	// RTT ��ü �ʱ�ȭ
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// ��Ʈ�� ��ü ����
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap) {
		return false;
	}

	// ��Ʈ�� ��ü �ʱ�ȭ
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	// ���̵� �������� �����Ѵ�.
	m_fadeInTime = 3000.0f;

	// ���� �ð��� 0ms�� �ʱ�ȭ
	m_accumulatedTime = 0;

	// ���̵� ������� 0%�� �Ͽ� ȭ���� �˰� �������� �Ѵ�.
	m_fadePercentage = 0;

	// ���̵� ����Ʈ�� ������ ���������� ǥ���Ѵ�.
	m_fadeDone = false;

	// ���̵� ��ü����
	m_FadeShader = new FadeShaderClass();
	if (!m_FadeShader) {
		return false;
	}
	// ���̵� ��ü �ʱ�ȭ
	result = m_FadeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {
	// ���̵� ��ü ��ȯ
	if (m_FadeShader) {
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}
	// ��Ʈ�� ��ü ��ȯ
	if (m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	// RTT ��ü ��ȯ
	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	// ���̴� ��ü ��ȯ
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// �� ��ü ��ȯ
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// ī�޶� ��ü ��ȯ
	if (m_Camera) {
		delete m_Camera; m_Camera = 0;
	}
	// Direct3D ��ü ��ȯ
	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

// Frame()������ ���̵� �� ȿ���� ������ �����Ѵ�. �� �����Ӹ��� ������ �� �ð��� �Լ��� ���ڷ� ���޵ǰ� �ؽ�ó�� ��� ��ŭ ���������� ���ȴ�.
// ������ ������ �ð��� ���̵� �� �ð��� �ѰԵǸ� m_FadeDone ������ true�� �ٲپ� ���̵� ȿ���� �����Ű�⵵ �Ѵ�.
bool GraphicsClass::Frame(float frameTime) {
	if (!m_fadeDone) {
		// �����ð��� ����Ѵ�.
		m_accumulatedTime += frameTime;

		// �ð��� �����鼭 ���̵� ȿ���� ������� �����ϵ��� �Ѵ�.
		if (m_accumulatedTime < m_fadeInTime) {
			// ������ �ð��� �̿��Ͽ� ����������� ����Ǿ����� ����Ѵ�.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else {
			// ���̵� �� �ð��� ���� ��� ���̵� ȿ���� ����.
			m_fadeDone = true;
			// ������� 100%
			m_fadePercentage = 1.0f;
		}
	}

	// ī�޶��� ��ġ�� ����
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}

//Render()�Լ��� ����ü�� ȸ���Ӹ��ƴ϶� ���� �� �ؽ��Ŀ� ���̵� �� ȿ���� ��ȯ�� ����Ͽ� ��� ȿ���� ������ �Ϲ� ����� �׸��� ����� ����Ѵ�.
bool GraphicsClass::Render() {
	bool result;
	static float rotation = 0.0f;

	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}

	if (m_fadeDone) {
		// ���̵� ȿ���� ������ ����۷� �Ϲ������ �׸���.
		RenderNormalScene(rotation);
	}
	else {
		// ���̵� ȿ���� ������ �ʾҵ��� RTT�� �׷��� �� �ؽ�ó�� ���̵� ȿ���� �ش�.
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

// RenderToTexture �Լ��� ȸ���ϴ� ����ü�� ����۰� �ƴ� �ؽ�ó�� �׸������Ѵ�.
bool GraphicsClass::RenderToTexture(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// RTT�� ������ Ÿ���� �ǵ����Ѵ�.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// ���ٽ� ���� ������ �ʱ�ȭ�Ѵ�.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� �̿��Ͽ� ������� �����.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, ��, ���� ����� ���Ѵ�.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// �� �����Ӹ��� ���� ����� ȸ������ŭ ȸ����Ų��.
	worldMatrix = XMMatrixRotationY(rotation);

	// ���� �׷��� ���������ο� �ִ´�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// ���� �������Ѵ�.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	// ����Ÿ���� �ٽ� ����۷� ������.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}


// RenderFAdingScene �Լ��� ���� �� �ؽ�ó�� �׷��� �ؽ�ó�� FadeShaderClass�� ������ ��ġ 2D ��Ʈ��ó�� ȭ�鿡 �׷��ִ´�.
// FadeShaderClas ��ü�� m_fadePercentage�� ���� �̿��Ͽ� �� �����Ӹ��� ������� ��Ⱑ �����ϴ��� �˾Ƴ���.
bool GraphicsClass::RenderFadingScene() {
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// ȭ���� �׸��� ���Ͽ� ȭ���� �ʱ�ȭ�Ѵ�.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� �̿��Ͽ� �� ����� �����Ѵ�.
	m_Camera->Render();

	// ī�޶�, D3D ��ü���� ����, ��, ���� ����� �����´�.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� �Ұ��̹Ƿ� Z���۸� ����.
	m_D3D->TurnZBufferOff();

	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) {
		return false;
	}

	// ���̵� ���̴��� �̿��Ͽ� ��Ʈ���� �׸���. 
	result = m_FadeShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_fadePercentage);
	if (!result) {
		return false;
	}

	// 2D �������� ������ Z ���۸� �ٽ� ���ش�.
	m_D3D->TurnZBufferOn();

	// �׷��� ����Ÿ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}

// RenderNormalScene �Լ��� ���̵��� �ƴ� �Ϲ�ȭ���� �׷��ִ� �Լ���.
bool GraphicsClass::RenderNormalScene(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// ����� �ʱ�ȭ
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ü�� ���� ����� ����
	m_Camera->Render();

	// ī�޶�, d3d ��ü�� ������ ����, ��, ������� ���Ѵ�.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ������Ŀ� ȸ������ �����ش�.
	worldMatrix = XMMatrixRotationY(rotation);

	// ���� �׷��� ���������ο� �־��ش�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// �������� �Ѵ� 
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result) {
		return false;
	}

	// �׷��� ����Ÿ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}