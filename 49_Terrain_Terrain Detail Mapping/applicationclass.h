////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 1.0f;  // 근거리에서 깊이 버퍼의 정밀도를 수정하기 위해 근거리 평면을 카메라에서 조금 더 멀리 이동시킨다. (0.1 -> 1.0)

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "TerrainShaderClass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "TerrainShaderClass.h"
#include "lightclass.h"

#include "debugwindowclass.h"
#include "textureshaderclass.h"
#include "rendertextureclass.h"
#include "depthshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float);

	bool RenderSceneToTexture();
	bool RenderGraphics();

private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	TerrainShaderClass* m_ColorShader;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;

	TerrainShaderClass* m_TerrainShader;
	LightClass* m_Light;

	DebugWindowClass* m_DebugWindow;
	TextureShaderClass* m_TextureShader;
	RenderTextureClass* m_RenderTexture;
	DepthShaderClass* m_DepthShader;
};

#endif