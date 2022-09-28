#ifndef _MODELLISTCLASS_H_
#define _MODELLISTCLASS_H_


#include <stdlib.h>
#include <time.h>
#include <DirectXMath.h>
using namespace DirectX;


// ModelListClass는 장ㅈ면의 모든 모델들에 대한 정보를 유지/관리하는 클래스다.
// 현재는 한가지 구체 모델만 사용하므로 구체의 크기와 색상만 유지한다. 
// 이 클래스는 여러 다른 타입의 모델들을 모두 관리할 수 잇게 확장할 수 있다.
class ModelListClass{
private:
	struct ModelInfoType{
		XMFLOAT4 color;
		float positionX, positionY, positionZ;
	};

public:
	ModelListClass();
	ModelListClass(const ModelListClass&);
	~ModelListClass();

	bool Initialize(int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMFLOAT4&);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;
};

#endif
