#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//���� windows�� �׷����� ������� �������̹Ƿ� d3dclass.h�� include ���ش�.
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "orthowindowclass.h"
#include "rendertextureclass.h" 
#include "textureshaderclass.h"
#include "horizontalblurshaderclass.h" 
#include "verticalblurshaderclass.h" 


//Graphics�� SystemClass�� �����ϴ� �� �ٸ� ��ü�� �� ���ø����̼��� ��� �׷��� ����� �� ��ü �ȿ� ĸ��ȭ �Ǿ��ִ�.
//����� Ǯ��ũ�� ���� ���� �׷��Ȱ� ���õ� �������� ������ �Ͽ���.

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass {
private:
	//D3DClass�� �����ϴ� �����͸� �����Ѵ�.
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	// �ٿ� ���ø�, �����ø� �ϱ����� �簢�� ��
	OrthoWindowClass* m_SmallWindow, * m_FullScreenWindow;
	// �� �˰��� �� �ܰ躰�� ������� �����ϱ� ���� RTT ��ü
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