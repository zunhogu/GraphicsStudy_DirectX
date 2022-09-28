// ModelClass는 3D 모델들의 복잡한 기하학을 캡슐화하는 클래스다.
// 이 코드에서는 간단한 모델인 삼각혀을 만들어보자
// 또 삼각형이 화면에 그려지기 위한 정점버퍼와 색인버퍼도 만들어보자.

#ifndef _MODELCLASS_H_ 
#define _MODELCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
#include <fstream>
using namespace std;
using namespace DirectX;

class ModelClass {
private :
	// 정점버퍼에 들어갈 정점성분을 정의해준다.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
	struct ModelType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
public :
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// 아래 함수들은 3D 모델의 정점버퍼와 인덱스 버퍼들의 초기화와 종료 과정을 제어한다.
	// Render 함수는 GPU에 모델들의 기하정보를 넣고 컬러 셰이더로 그릴 준비를 한다.

	bool Initialize(ID3D11Device*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadModel(char*);
	void ReleaseModel();
private :
	// 아래 멤버 변수들은 정점버퍼, 인덱스 버퍼, 각 버퍼의 크기 정보를 가지고 있는 버퍼다.
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_Model;
};

#endif