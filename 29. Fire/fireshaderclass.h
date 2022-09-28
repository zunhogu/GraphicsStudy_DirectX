// ColorShaderClass는 GPU 상에 존재하는 3D 모델들을 그리는 데 사용하는 HLSL 셰이더를 호출하는 것이다.

#ifndef _FIRESHADERCLASS_H_
#define _FIRESHADERCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

class FireShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	// 정점셰이더에서 노이즈를 계산하기위한 상수버퍼를 사용하고 있기 때문에 상수버퍼를 만들어준다.
	struct NoiseBufferType{
		float frameTime;   // 프레임 속도
		XMFLOAT3 scrollSpeeds;   // 세 개의 다른 노이즈 스크롤 속도
		XMFLOAT3 scales;   // 세 개의 다른 노이즈 크기 변수
		float padding;
	};
	// 픽셀셰이더에서 불꽃 효과를 만들기위해 노이즈값을 흩뜨리는 왜곡 상수버퍼
	struct DistortionBufferType { 
		XMFLOAT2 distortion1;   // 노이즈1 왜곡 배열
		XMFLOAT2 distortion2;   // 노이즈2 왜곡 배열
		XMFLOAT2 distortion3;   // 노이즈3 왜곡 배열
		float distortionScale;  // 왜곡 크기 값
		float distortionBias;   // 왜곡 바이어스 값
	};

public:
	FireShaderClass();
	FireShaderClass(const FireShaderClass&);
	~FireShaderClass();

	//아래 함수들은 셰이더의 초기화와 마무리를 제어한다.
	// Render 함수는 셰이더에 사용되는 변수들을 설정하고 셰이더를 이용해 준비된 모델의 정점들을 그려낸다.

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**, float, XMFLOAT3, XMFLOAT3, XMFLOAT2, XMFLOAT2, XMFLOAT2, float, float);


private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*); 
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**, float, XMFLOAT3, XMFLOAT3, XMFLOAT2, XMFLOAT2, XMFLOAT2, float, float);

	void RenderShader(ID3D11DeviceContext*, int);
private: 
	ID3D11VertexShader* m_vertexShader; 
	ID3D11PixelShader* m_pixelShader; 
	ID3D11InputLayout* m_layout; 
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_noiseBuffer;
	ID3D11SamplerState* m_sampleState2;
	ID3D11Buffer* m_distortionBuffer;
};

#endif