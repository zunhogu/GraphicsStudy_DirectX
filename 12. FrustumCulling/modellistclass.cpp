#include "modellistclass.h"

ModelListClass::ModelListClass(){
	m_ModelInfoList = 0;
}

ModelListClass::ModelListClass(const ModelListClass& other){}

ModelListClass::~ModelListClass(){}

bool ModelListClass::Initialize(int numModels) {
	int i;
	float red, green, blue;

	// 모델의 수를 저장한다.
	m_modelCount = numModels;

	// 모델 리스트 객체를 생성한다.
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList) {
		return false;
	}

	// 랜덤 숫자 생성기의 seed를 현재 시간으로 주고 임의로 색상과 위치를 정하여 모델 배열에 저장한다.
	srand((unsigned int)time(NULL));

	for (i = 0; i < m_modelCount; i++){
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

// Shutdown 함수는 모델 정보가 담긴 배열을 해제한다.
void ModelListClass::Shutdown(){
	// 모델 리스트 객체를 반환한다.
	if (m_ModelInfoList){
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}

	return;
}

// GetModelCount함수는 현재 관리하고 있는 모델들의 숫자를 알려준다.
int ModelListClass::GetModelCount(){
	return m_modelCount;
}

// GetData 함수는 주어진 인덱스로 모델 리스트를 뒤져서 구체 모델의 위치와 색상을 가져온다.
void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color){
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;

	return;
}