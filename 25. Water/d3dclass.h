//헤더에서 먼저 나타나는 것은 객체 모듈을 사용하기 위해 링크하는 라이브러리들을 명시하는 것이다.
//이 라이브러리들은 DriectX 초기화, 3D 그래픽 랜더링, 새로고침 비율(역자주 : FPS)을 얻거나
//사용하는 그래픽카드의 정보 등과 같은 하드웨어로의 통신을 위해 필요한 Driect3D 기능을 담고있다.
#ifndef  _D3DClass_H_
#define _D3DClass_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "d3d11.lib") 
//#pragma comment(lib, "d3dx11.lib") 
//#pragma comment(lib, "d3dx10.lib")


//링크한 라이브러리들을 DirectX 타입 정의와 같은 것들에 대한 헤더를 include 한다.
#include <dxgi.h> 
#include <d3dcommon.h> 
#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;
//DirectX11에서는 d3dxmath10.h를 사용할 수 없다. 그 기능과 거의 유사한 DirectXMath.h를 include해오자

//D3DClass는 기본클래스 생성자가 있고, 복사생성자와 파괴자가 있다.
class D3DClass {
private:
	bool m_vsync_enabled;    // 렌더링을 모니터 주사율 or 빠르게 다시 그릴 것인지 정하는 변수
	int m_videoCardMemory;   // 그래픽카드 사용 가능한 메모리크기
	char m_videoCardDescription[128];   // 그래픽카드 이름 
	IDXGISwapChain* m_swapChain;   // 스왑체인
	ID3D11Device* m_device;   // Direct3D 장치
	ID3D11DeviceContext* m_deviceContext;   //Direct3D 장치 컨텍스트
	ID3D11RenderTargetView* m_renderTargetView;   // Direct3D 장치의 렌더링을 할 버퍼를 가르키는 포인터 나중에 백버퍼가 연결된다. 
	ID3D11Texture2D* m_depthStencilBuffer;   // 깊이-스텐실 버퍼 
	ID3D11DepthStencilState* m_depthStencilState; // 깊이-스텐실 상태 변수
	ID3D11DepthStencilView* m_depthStencilView;  // 깊이-스텐실 뷰 
	ID3D11RasterizerState* m_rasterState; // 레스터화기 변수

	XMMATRIX m_projectionMatrix;   // 원근 투영공간행렬
	XMMATRIX m_worldMatrix;   // 월드 투영공간 행렬
	XMMATRIX m_orthoMatrix;   // 직교 투영공간 행렬
	//뷰행령에 대해서는 나중에 나올 카메라 클래스 뷰행렬이 들어간다.

	ID3D11DepthStencilState* m_depthDisabledStencilState; 
	ID3D11BlendState* m_alphaEnableBlendingState; 
	ID3D11BlendState* m_alphaDisableBlendingState;

public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn(); 
	void TurnZBufferOff(); 
	void TurnOnAlphaBlending(); 
	void TurnOffAlphaBlending();

	// GetdepthStencilView함수는 깊이 스텐실 뷰를 가져올 수 있는 함수다.
	ID3D11DepthStencilView* GetDepthStencilView(); 
	
	// SetBackBufferRenderTarget함수는 백버퍼를 현재 렌더 타겟으로 설정할 수 있게 해준다.
	void SetBackBufferRenderTarget();
};

#endif 
