#ifndef _TEXTCLASS_H_ 
#define _TEXTCLASS_H_

#include "fontclass.h"
#include "fontshaderclass.h"


// TextClass�� FontClass���� ������ ����/�ε����迭�� ������ ����/�ε��� ���۸� �����
// �� ���۸� ����� ���ۿ��׷��ִ� ������ �����Ѵ�. 
class TextClass {
private:
	// SentenceType ����ü�� �� ������ ������ ������ �����ϴµ� ����Ѵ�.
	struct SentenceType{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);

private: 
	bool InitializeSentence(SentenceType**, int, ID3D11Device*); 
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*); 
	void ReleaseSentence(SentenceType**); 
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);

private: 
	FontClass* m_Font; 
	FontShaderClass* m_FontShader; 
	int m_screenWidth, m_screenHeight; 
	XMMATRIX m_baseViewMatrix;
	
	// �׸� ������ �ΰ� 
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
};

#endif