#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//���� windows�� �׷����� ������� �������̹Ƿ� d3dclass.h�� include ���ش�.
#include "d3dclass.h"
#include "cameraclass.h"
#include "bitmapclass.h"
#include "TextureShaderClass.h"
#include "modelclass.h"
#include "rendertextureclass.h"
#include "fadeshaderclass.h"

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
	BitmapClass* m_Bitmap;
	ModelClass* m_Model;
	RenderTextureClass* m_RenderTexture;
	TextureShaderClass* m_TextureShader;
	float m_fadeInTime, m_accumulatedTime, m_fadePercentage;
	bool m_fadeDone;
	FadeShaderClass* m_FadeShader;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);
	bool Render();

private:
	bool RenderToTexture(float);
	bool RenderFadingScene();
	bool RenderNormalScene(float);
};

#endif