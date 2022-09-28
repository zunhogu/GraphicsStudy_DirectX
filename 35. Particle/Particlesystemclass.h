// ModelClass�� 3D �𵨵��� ������ �������� ĸ��ȭ�ϴ� Ŭ������.
// �� �ڵ忡���� ������ ���� �ﰢ���� ������
// �� �ﰢ���� ȭ�鿡 �׷����� ���� �������ۿ� ���ι��۵� ������.

#ifndef _PARTICLESYSTEMCLASS_H_ 
#define _PARTICLESYSTEMCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;

// �� Ŭ���������� �ؽ�ó �ڿ��� �����ؾ��Ѵ�. �� ������ ���۸� ���鶧 �� �Է°��� �غ��ϴ� �������� ���̴��� uv��ǥ�� ���ϱ⶧���̴�.
#include "textureclass.h"

class ParticleSystemClass {
private :
	// �������ۿ� �� ���������� �������ش�.
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 color;
	};
	// ��ƼŬ�� �����ϱ� ���ؼ��� �Է�������ܰ迡�� ��ƼŬ ���۸� �����Ѵ�.
	struct ParticleType { 
		float positionX, positionY, positionZ; 
		float red, green, blue; 
		float velocity; bool active; 
	};

public :
	ParticleSystemClass();
	ParticleSystemClass(const ParticleSystemClass&);
	~ParticleSystemClass();

	// �Ʒ� �Լ����� 3D ���� �������ۿ� �ε��� ���۵��� �ʱ�ȭ�� ���� ������ �����Ѵ�.
	// Render �Լ��� GPU�� �𵨵��� ���������� �ְ� �÷� ���̴��� �׸� �غ� �Ѵ�.

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	// ModelClass ���� ���̴����� �ڽ��� �ؽ�ó �ڿ��� �����ϰ� �׸��� ���� GetTexture �Լ��� ���� �ִ�.
	ID3D11ShaderResourceView* GetTexture();
	bool Frame(float, ID3D11DeviceContext*);
private :
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool UpdateBuffers(ID3D11DeviceContext*);

	// ���� �ؽ�ó�� �ҷ����� ��ȯ�ϴ� �� ����� LoadTexture �Լ��� ReleaseTexture �Լ��� ������ �ִ�. 
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeParticleSystem(); 
	void ShutdownParticleSystem();

	void EmitParticles(float); 
	void UpdateParticles(float);
	void KillParticles();


private :
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ; 
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond; 
	int m_maxParticles;

	int m_currentParticleCount; 
	float m_accumulatedTime;

	// �Ʒ� ��� �������� ��������, �ε��� ����, �� ������ ũ�� ������ ������ �ִ� ���۴�.
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;

	VertexType* m_vertices;
	ParticleType* m_particleList;
};

#endif