#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//���� windows�� �׷����� ������� �������̹Ƿ� d3dclass.h�� include ���ش�.
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "multitextureshaderclass.h"

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
	MultiTextureShaderClass* m_MultiTextureShader;
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();
};

#endif