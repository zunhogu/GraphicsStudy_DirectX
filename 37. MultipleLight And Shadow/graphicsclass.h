#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//���� windows�� �׷����� ������� �������̹Ƿ� d3dclass.h�� include ���ش�.
#include "d3dclass.h"
#include "cameraclass.h"
#include "lightclass.h"
#include "modelclass.h"
#include "RenderTextureClass.h"
#include "shadowshaderclass.h"
#include "depthshaderclass.h"

//Graphics�� SystemClass�� �����ϴ� �� �ٸ� ��ü�� �� ���ø����̼��� ��� �׷��� ����� �� ��ü �ȿ� ĸ��ȭ �Ǿ��ִ�.
//����� Ǯ��ũ�� ���� ���� �׷��Ȱ� ���õ� �������� ������ �Ͽ���.

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOWMAP_WIDTH = 1024; 
const int SHADOWMAP_HEIGHT = 1024;

class GraphicsClass {
private:
	//D3DClass�� �����ϴ� �����͸� �����Ѵ�.
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	LightClass* m_Light;
	LightClass* m_Light2;
	ModelClass * m_CubeModel, * m_GroundModel, * m_SphereModel;
	RenderTextureClass* m_RenderTexture; 
	RenderTextureClass* m_RenderTexture2; 
	DepthShaderClass* m_DepthShader; 
	ShadowShaderClass* m_ShadowShader;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float, float, float, float, float, float);

private:
	bool RenderSceneToTexture();
	bool RenderSceneToTexture2();
	bool Render();
};

#endif