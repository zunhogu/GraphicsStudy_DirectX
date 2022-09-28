#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//기존 windows의 그래픽은 사용하지 않을것이므로 d3dclass.h를 include 해준다.
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "orthowindowclass.h"
#include "rendertextureclass.h" 
#include "textureshaderclass.h"
#include "horizontalblurshaderclass.h" 
#include "verticalblurshaderclass.h" 


//Graphics는 SystemClass가 생성하는 또 다른 객체로 이 어플리케이션의 모든 그래픽 기능이 이 객체 안에 캡슐화 되어있다.
//헤더에 풀스크린 모드와 같은 그래픽과 관련된 전역변수 선언을 하였다.

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass {
private:
	//D3DClass를 참조하는 포인터를 선언한다.
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	// 다운 샘플링, 업샘플링 하기위한 사각형 모델
	OrthoWindowClass* m_SmallWindow, * m_FullScreenWindow;
	// 블러 알고리즘 각 단계별로 결과값을 저장하기 위한 RTT 객체
	RenderTextureClass* m_RenderTexture, * m_DownSampleTexure, * m_HorizontalBlurTexture, * m_VerticalBlurTexture, * m_UpSampleTexure;
	TextureShaderClass* m_TextureShader;
	HorizontalBlurShaderClass* m_HorizontalBlurShader; 
	VerticalBlurShaderClass* m_VerticalBlurShader;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render(float);
	bool RenderSceneToTexture(float);
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();

};

#endif