// TextureClass�� �ؽ�ó �ڿ��� �ҷ����� �����ϴ� �۾��� ĸ��ȭ�Ѵ�.
// ��� �ؽ�ó�� ���ؼ� �� Ŭ������ ������� �־���Ѵ�.

#ifndef _TEXTUREARRAYCLASS_H_ 
#define _TEXTUREARRAYCLASS_H_

#include <d3d11.h>
#include <directxtex.h>

class TextureClass {
private:
	// �ؽ�ó �ڿ��� ���� ������
	ID3D11ShaderResourceView* m_texture;
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	// �Ʒ� ���Լ��� �־��� ������ �̸����� �ؽ�ó�� �ҷ����� ���̻� ������ �ʴ� �ؽ�ó�� �����Ѵ�.
	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	DirectX::ScratchImage LoadTextureFromFile(WCHAR*);

	// GetTexture�Լ��� ���̴����� �ҷ��� �ؽ�ó�� ��ȯ�Ѵ�.
	ID3D11ShaderResourceView* GetTexture();
};

#endif