#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//���� windows�� �׷����� ������� �������̹Ƿ� d3dclass.h�� include ���ش�.
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include "rendertextureclass.h" 
#include "GlassShaderClass.h"

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
	ModelClass* m_WindowModel;
	RenderTextureClass* m_RenderTexture;
	TextureShaderClass* m_TextureShader;
	GlassShaderClass* m_GlassShader;
	// ���� ���̿� ��ġ�� �����ϴ� ����
	float m_waterHeight, m_waterTranslation;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool RenderToTexture(float);
	bool Render(float);
private:
};

#endif