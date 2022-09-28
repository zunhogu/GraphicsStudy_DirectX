#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//기존 windows의 그래픽은 사용하지 않을것이므로 d3dclass.h를 include 해준다.
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "RefractionShaderClass.h"
#include "lightclass.h"
#include "lightshaderclass.h"
#include "rendertextureclass.h" 
#include "WaterShaderClass.h"

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
	ModelClass* m_GroundModel, * m_WallModel, * m_BathModel, * m_WaterModel;
	LightClass* m_Light;
	RenderTextureClass* m_RefractionTexture, * m_ReflectionTexture;
	LightShaderClass* m_LightShader;
	RefractionShaderClass* m_RefractionShader; 
	WaterShaderClass* m_WaterShader;
	// 물의 높이와 위치를 지정하는 변수
	float m_waterHeight, m_waterTranslation;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();
private:
	bool RenderRefractionToTexture(); 
	bool RenderReflectionToTexture();
	bool RenderScene();
};

#endif