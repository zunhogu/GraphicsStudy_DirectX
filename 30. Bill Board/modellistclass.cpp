#include "modellistclass.h"

ModelListClass::ModelListClass(){
	m_ModelInfoList = 0;
}

ModelListClass::ModelListClass(const ModelListClass& other){}

ModelListClass::~ModelListClass(){}

bool ModelListClass::Initialize(int numModels) {
	int i;
	float red, green, blue;

	// ���� ���� �����Ѵ�.
	m_modelCount = numModels;

	// �� ����Ʈ ��ü�� �����Ѵ�.
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList) {
		return false;
	}

	// ���� ���� �������� seed�� ���� �ð����� �ְ� ���Ƿ� ����� ��ġ�� ���Ͽ� �� �迭�� �����Ѵ�.
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

// Shutdown �Լ��� �� ������ ��� �迭�� �����Ѵ�.
void ModelListClass::Shutdown(){
	// �� ����Ʈ ��ü�� ��ȯ�Ѵ�.
	if (m_ModelInfoList){
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}

	return;
}

// GetModelCount�Լ��� ���� �����ϰ� �ִ� �𵨵��� ���ڸ� �˷��ش�.
int ModelListClass::GetModelCount(){
	return m_modelCount;
}

// GetData �Լ��� �־��� �ε����� �� ����Ʈ�� ������ ��ü ���� ��ġ�� ������ �����´�.
void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color){
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;

	return;
}