#ifndef _RENDERTEXTURECLASS_H_ 
#define _RENDERTEXTURECLASS_H_

#include <d3d11.h>

// RenderToTextureClass�� ����� ��� ����ڰ� ���ϴ� �ؽ�ó�� ���� Ÿ������ ������ �� �ִ�.
// ���� �������� �����͸� �ڿ��� �������ε� ������ �� �ִ�.
class RenderTextureClass {
private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();
};

#endif