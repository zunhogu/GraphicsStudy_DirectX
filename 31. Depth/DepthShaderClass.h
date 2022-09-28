// ColorShaderClass�� GPU �� �����ϴ� 3D �𵨵��� �׸��� �� ����ϴ� HLSL ���̴��� ȣ���ϴ� ���̴�.

#ifndef _DEPTHSHADERCLASS_H_ 
#define _DEPTHSHADERCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

class DepthShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	//�Ʒ� �Լ����� ���̴��� �ʱ�ȭ�� �������� �����Ѵ�.
	// Render �Լ��� ���̴��� ���Ǵ� �������� �����ϰ� ���̴��� �̿��� �غ�� ���� �������� �׷�����.

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*); 
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);
private: 
	ID3D11VertexShader* m_vertexShader; 
	ID3D11PixelShader* m_pixelShader; 
	ID3D11InputLayout* m_layout; 
	ID3D11Buffer* m_matrixBuffer;
};

#endif