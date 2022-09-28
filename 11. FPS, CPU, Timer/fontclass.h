#ifndef _FONTCLASS_H_ 
#define _FONTCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <fstream> 
using namespace std;

#include "textureclass.h"

class FontClass {
private:
	struct FontType {
		// FontType 구조체는 글꼴 인덱스 파일의 인덱스 데이터를 저장하기위해 사용된다.
		float left, right; 
		int size;
	};
	struct VertexType {
		// VertexType 구조체는 실제로 문자가 그려질 사각형을 만드는데 필요한 정점 데이터를 저장한다.
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*); 
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	// BuildVertexArry 함수는 문자열을 입력으로 받아 글자를 그릴 삼각형들의 정점배열을 만들어 반환하는 역할을 한다.
	// 이 함수는 그려낼 문장이 있을때 TextClass의 생성자에서 호출된다. 
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture(); 

private:
	FontType* m_Font;
	TextureClass* m_Texture;
}; 

#endif