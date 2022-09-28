// ModelClass는 3D 모델들의 복잡한 기하학을 캡슐화하는 클래스다.
// 이 코드에서는 간단한 모델인 삼각혀을 만들어보자
// 또 삼각형이 화면에 그려지기 위한 정점버퍼와 색인버퍼도 만들어보자.

#ifndef _MODELCLASS_H_ 
#define _MODELCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;

// 모델 클래스에서는 텍스처 자원에 접근해야한다. 그 이유는 버퍼를 만들때 들어갈 입력값을 준비하는 과정에서 셰이더는 uv좌표를 원하기때문이다.
#include "textureclass.h"

class ModelClass {
private :
	// 정점버퍼에 들어갈 정점성분을 정의해준다.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	struct InstanceType {
		XMFLOAT3 position;
	};
public :
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// 아래 함수들은 3D 모델의 정점버퍼와 인덱스 버퍼들의 초기화와 종료 과정을 제어한다.
	// Render 함수는 GPU에 모델들의 기하정보를 넣고 컬러 셰이더로 그릴 준비를 한다.

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	// GetInstanceCount 함수는 기존의 인덱스 개수를 가져오는 기능과 동일하다.
	int GetInstanceCount(); 

	// ModelClass 역시 셰이더에게 자신의 텍스처 자원을 전달하고 그리기 위한 GetTexture 함수를 갖고 있다.
	ID3D11ShaderResourceView* GetTexture();

private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// 모델의 텍스처를 불러오고 반환하는 데 사용할 LoadTexture 함수와 ReleaseTexture 함수를 가지고 있다. 
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private :
	// 아래 멤버 변수들은 정점버퍼, 인덱스 버퍼, 각 버퍼의 크기 정보를 가지고 있는 버퍼다.
	ID3D11Buffer* m_vertexBuffer, * m_instanceBuffer;
	int m_vertexCount, m_instanceCount;
	TextureClass* m_Texture;
};

#endif