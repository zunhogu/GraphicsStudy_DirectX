#ifndef _RENDERTEXTURECLASS_H_ 
#define _RENDERTEXTURECLASS_H_

#include <d3d11.h>

// RenderToTextureClass는 백버퍼 대신 사용자가 원하는 텍스처를 렌더 타겟으로 설정할 수 있다.
// 또한 렌더링된 데이터를 자원뷰 형식으로도 가져올 수 있다.
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