// ColorShaderClass�� GPU �� �����ϴ� 3D �𵨵��� �׸��� �� ����ϴ� HLSL ���̴��� ȣ���ϴ� ���̴�.

#ifndef _TEXTURESHADERCLASS_H_ 
#define _TEXTURESHADERCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

const int NUM_LIGHTS = 4;

class LightShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightColorBufferType { 
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	}; 
	struct LightPositionBufferType { 
		XMFLOAT4 lightPosition[NUM_LIGHTS];
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	//�Ʒ� �Լ����� ���̴��� �ʱ�ȭ�� �������� �����Ѵ�.
	// Render �Լ��� ���̴��� ���Ǵ� �������� �����ϰ� ���̴��� �̿��� �غ�� ���� �������� �׷�����.

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4[], XMFLOAT4[]);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*); 
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4[], XMFLOAT4[]);
	void RenderShader(ID3D11DeviceContext*, int);
private: 
	ID3D11VertexShader* m_vertexShader; 
	ID3D11PixelShader* m_pixelShader; 
	ID3D11InputLayout* m_layout; 
	// �������� ���� �ʱ�ȭ�ϱ� ���� �������
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightColorBuffer; 
	ID3D11Buffer* m_lightPositionBuffer;
	// �ؽ�ó ���÷� ��ü�� ����� ���� ��ü�� ����
	ID3D11SamplerState* m_sampleState;
};

#endif