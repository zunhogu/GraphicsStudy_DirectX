#include "graphicsclass.h"

GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_Camera = 0;
	m_Bitmap = 0;
	m_TextureShader = 0;
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

	// ī�޶� ��ü �ʱ�ȭ
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// �� ��ü ����
	m_Bitmap = new BitmapClass();
	if (!m_Bitmap) {
		return false;
	}

	// ��Ʈ�� ��ü �ʱ�ȭ
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Engine/data/seafloor.dds", 256, 256);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK); 
		return false;
	}

	// TextureShaderClass ��ü ����
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader) {
		return false; 
	} 
	
	// TextureShaderClass ��ü �ʱ�ȭ
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	
	return true;
}

// GraphicsClass::Shutdown() ������ ��� �׷��� ��ü�� ���� ������ �Ͼ�� �Լ��̱� ������ D3DClass�� �� �Լ����� ��ü�ȴ�.
// ���⼭ �Ű澵 �κ��� m_D3D�� �ʱ�ȭ �Ǿ����� Ȯ���ϴ� �κ��ε� ���� �ʱ�ȭ ���� ������ �������� ���������� �����ϰ� �������� �������̴�.
// ��� �����͸� null�� �ʱ�ȭ�ϴ� ���� �� �߿����� �� �� �ִ�. 
void GraphicsClass::Shutdown() {

	// ���̴� ��ü ��ȯ
	if(m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	} 

	// �� ��ü ��ȯ
	if(m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	} 

	// ī�޶� ��ü ��ȯ
	if(m_Camera) { 
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

// Frame()�� �� �����Ӹ��� Render �Լ��� �θ����� �ٲ۴�.
bool GraphicsClass::Frame() {
	bool result;

	// ������ ȿ���� ���� �ΰ��ϱ� ���� ȸ������ Render�Լ��� �ش�. 
	// �� ȸ������ �� �����Ӹ��� �ٲ�Ƿ� Frame �Լ����� �������ش�.

	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.01f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}

	// �׷��� �������� �����Ѵ�.
	result = Render(rotation);
	if (!result) {
		return false;
	}

	return true;
}

//Render()�Լ��� ���� ������â�� �׸��� �׸��� �Լ���.
bool GraphicsClass::Render(float rotation) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// �� �׸��⸦ �����ϱ� ���� ������ ������ �����.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� ������� ����� ���ؼ� ī�޶��� Render�Լ��� ȣ���Ѵ�.
	m_Camera->Render();

	// ������� ��������� ���� ������� ���纻�� ������ �� �ִ�.
	// ���� D3DClass ��ü�� ���� ������İ� ������ĵ� �����ؿ´�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 2D �������� �ϱ� ���ؼ��� ���翵 ����� ���Ѵ�. �̴� �Ϲ� ������Ĵ�� ���޵� ���̴�.
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� �����ϱ� ���� Z���۸� ����. 
	m_D3D->TurnZBufferOff();

	// �׸��� ���� ȭ���� (100, 100) ��ġ�� ��Ʈ���� �׸���. �� ��ġ�� ���Ƿ� �ٲ� �� �ִ�.
	 result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 100, 100); 
	 if(!result) {
		 return false;
	 }

	// �ϴ� ����/�ε��� ���۰� �غ�Ǿ��ٸ� �ؽ��� ���̴��� �̿��� �׸��� �ȴ�.
	// �̶� ������ ���� projectionMatrix ��� orthoMatrix�� ���ڷ� ���´ٴ� ���� ��������
	// ���� �� ����� ������ ��� �ٲ�� ���̶�� 2D ���������� ���� �⺻ ������� ���� �ʿ䰡 �ִ�.
	// ���� ī�޶�� �����̱⶧���� ������ ������� �������̴�.

	// ������ ���̴��� ȣ���Ͽ� �غ�� ������ �� ����� ����Ͽ� �������� �׷�����. �׷��� �ﰢ���� ����ۿ� �׷����� �ȴ�.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) { 
		return false;
	}

	// 2D �������� ������ 3D ��ü�� �׸������ؼ� Z���۸� �ٽ� �Ҵ�.
	m_D3D->TurnZBufferOn();

	// �� �׸��Ⱑ �Ϸ�Ǹ� ���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}