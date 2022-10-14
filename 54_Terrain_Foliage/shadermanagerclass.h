////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADERMANAGERCLASS_H_
#define _SHADERMANAGERCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "textureshaderclass.h"
#include "fontshaderclass.h"
#include "foliageshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ShaderManagerClass
////////////////////////////////////////////////////////////////////////////////
class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Initialize(D3DClass*, HWND);
	void Shutdown();

	void RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderFontShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	void RenderFoliageShader(ID3D11DeviceContext*, int, int, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	TextureShaderClass* m_TextureShader;
	FontShaderClass* m_FontShader;
	FoliageShaderClass* m_FoliageShader;
};

#endif