// ���ݱ��� ����/�ε��� ���۸� �¾��ϰ�, ���̴��� ȣ���Ͽ���. ���������� �ʿ��� ���� ���忡�� �츮�� �ٶ󺸴� �����̴�.
// �̸� �����ϱ� ���ؼ��� ��� �츮�� ����� �������� ���� ������ DirectX11���� �����ϴ� ī�޶� Ŭ������ �ʿ��ϴ�.
// ī�޶� Ŭ������ ī�޶��� ��ġ�� ���� ȸ�� ���¸� ��� ������ �־���Ѵ�. ���� �� �������� Ȱ���ؼ� ���̴����� ����� �� ��ĵ� �����ؾ��Ѵ�.

#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <DirectXMath.h>
using namespace DirectX;


class CameraClass{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	// SetPosition�� SetRotation �Լ��� ���� ī�޶��� ��ġ��, ȸ�����¸� �����ϴµ� ��� �� ���̴�. 
	// Render �Լ��� ī�޶��� ��ġ�� ȸ�� ���¿� ����� �� ����� �����ϴµ� ��� �ɰ��̴�.
	// ���������� GetViewMatrix �Լ��� ���̴����� ī�޶� ��ü�� ������� �޾ƿ��µ� ��� �� ���̴�. 
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	
	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif