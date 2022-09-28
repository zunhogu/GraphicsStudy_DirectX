#ifndef _SHADOWSHADERCLASS_H_ 
#define _SHADOWSHADERCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

class ShadowShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};
	struct LightBufferType1 {
		XMFLOAT3 lightPosition;
		float padding;
	};
	struct LightBufferType2 {
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
	};

public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	//�Ʒ� �Լ����� ���̴��� �ʱ�ȭ�� �������� �����Ѵ�.
	// Render �Լ��� ���̴��� ���Ǵ� �������� �����ϰ� ���̴��� �̿��� �غ�� ���� �������� �׷�����.

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView* ,ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*); 
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);
private: 
	ID3D11VertexShader* m_vertexShader; 
	ID3D11PixelShader* m_pixelShader; 
	ID3D11InputLayout* m_layout; 
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer1;
	ID3D11Buffer* m_lightBuffer2;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
};

#endif