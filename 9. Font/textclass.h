#ifndef _TEXTCLASS_H_ 
#define _TEXTCLASS_H_

#include "fontclass.h"
#include "fontshaderclass.h"


// TextClass는 FontClass에서 생성한 정점/인덱스배열을 가지고 정점/인덱스 버퍼를 만들고
// 그 버퍼를 어셈블리 버퍼에그려주는 역할을 수행한다. 
class TextClass {
private:
	// SentenceType 구조체는 각 문장의 렌더링 정보를 저장하는데 사용한다.
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
	
	// 그릴 문장은 두개 
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
};

#endif