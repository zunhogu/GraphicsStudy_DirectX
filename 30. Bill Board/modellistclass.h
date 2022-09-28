#ifndef _MODELLISTCLASS_H_
#define _MODELLISTCLASS_H_


#include <stdlib.h>
#include <time.h>
#include <DirectXMath.h>
using namespace DirectX;


// ModelListClass�� �夸���� ��� �𵨵鿡 ���� ������ ����/�����ϴ� Ŭ������.
// ����� �Ѱ��� ��ü �𵨸� ����ϹǷ� ��ü�� ũ��� ���� �����Ѵ�. 
// �� Ŭ������ ���� �ٸ� Ÿ���� �𵨵��� ��� ������ �� �հ� Ȯ���� �� �ִ�.
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
