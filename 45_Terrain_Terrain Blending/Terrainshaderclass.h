////////////////////////////////////////////////////////////////////////////////
// Filename: Terrainshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Terrainshaderclass_H_
#define _Terrainshaderclass_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include  <d3dcompiler.h>
#include <fstream>
using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: Terrainshaderclass
////////////////////////////////////////////////////////////////////////////////
class Terrainshaderclass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct TextureInfoBufferType
	{
		bool useAlplha;
		XMFLOAT3 padding2;
	};

public:
	Terrainshaderclass();
	Terrainshaderclass(const Terrainshaderclass&);
	~Terrainshaderclass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	// Texture가 자주 변경될 수 있기 때문에 Terrain Rendering이 효율적으로 수행될수 있도록 Parameter 설정 함수를 다음과 같이 나눈다.
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT3);
	bool SetShaderTextures(ID3D11DeviceContext*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, bool);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT3, ID3D11ShaderResourceView*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_textureInfoBuffer;
};

#endif