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
		// FontType ����ü�� �۲� �ε��� ������ �ε��� �����͸� �����ϱ����� ���ȴ�.
		float left, right; 
		int size;
	};
	struct VertexType {
		// VertexType ����ü�� ������ ���ڰ� �׷��� �簢���� ����µ� �ʿ��� ���� �����͸� �����Ѵ�.
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

	// BuildVertexArry �Լ��� ���ڿ��� �Է����� �޾� ���ڸ� �׸� �ﰢ������ �����迭�� ����� ��ȯ�ϴ� ������ �Ѵ�.
	// �� �Լ��� �׷��� ������ ������ TextClass�� �����ڿ��� ȣ��ȴ�. 
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