#include "textureclass.h"
#include <string>
using namespace std;

TextureClass::TextureClass() {
	m_texture = 0;
}

TextureClass::TextureClass(const TextureClass& other) {}

TextureClass::~TextureClass() {}

// Initialize �Լ��� �ؽ�ó�� �ʱ�ȭ�ϴ� �Լ���.
bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename) {

	// �ؽ�ó ������ �������Ͽ� �ڿ��並 ���� �׷��� ���������ο� ���ε� ���Ѿ��Ѵ�.
	// ����� �ΰ����� �ִ�.
	// DirectX::CreateShaderResourceView�� ����ϴ� ���
	// DirectX::CreateTexture�� ����Ͽ� �ؽ�ó�� �����ϰ� D3D11���� �����ϴ� CreateShaderResourceView�� ����Ͽ� ����� ��� �ΰ����� �ִ�.(ť�� �ؽ�ó�� ���鶧)

	HRESULT result;
	// ��ũ���� �ؽ�ó�� �ε��Ѵ�.
	DirectX::ScratchImage image = LoadTextureFromFile(filename);

	// �ε��� �ؽ�ó�� ������ �ڿ��並 �����. 

	// ù�� ° ��� 
	DirectX::CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_texture);
	//

	//// �ι� ° ���
	//ID3D11Texture2D* SMTexture = nullptr;
	//DirectX::CreateTexture(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), (ID3D11Resource**)&SMTexture);

	//D3D11_TEXTURE2D_DESC textureDesc;
	//SMTexture->GetDesc(&textureDesc);

	//D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	//SMViewDesc.Format = textureDesc.Format;
	//SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	//SMViewDesc.TextureCube.MipLevels = textureDesc.MipLevels;
	//SMViewDesc.TextureCube.MostDetailedMip = 0;

	//result = device->CreateShaderResourceView(SMTexture, &SMViewDesc, &m_texture);
	////
	return true;
}

// Shutdown �Լ��� �ε�� �ؽ��� �ڿ��� ��ȯ�ϰ� �� �����͸� null�� �ǵ�����.
void TextureClass::Shutdown()
{
	if (m_texture) {
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

// GetTexture �Լ��� �ٸ� ��ü�� �� �ؽ�ó �ڿ��� �����Ϸ� �Ҷ� �ڿ��� ��ȯ�Ѵ�.
ID3D11ShaderResourceView* TextureClass::GetTexture() {
	return m_texture;
}


DirectX::ScratchImage TextureClass::LoadTextureFromFile(WCHAR* filename) {

	// WCAHR* -> CHAR* �κ���
	CHAR* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);
	pStr = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, filename, -1, pStr, strSize, 0, 0);
	//

	HRESULT result;
	string str(pStr);
	std::wstring wsTmp(str.begin(), str.end());

	std::wstring ws = wsTmp;

	//�ؽ�ó �ε�
	DirectX::ScratchImage image;
	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(ws.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0) {
		result = DirectX::LoadFromDDSFile(ws.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else if (_wcsicmp(ext, L".tga") == 0) {
		result = DirectX::LoadFromTGAFile(ws.c_str(), nullptr, image);
	}
	else {
		result = DirectX::LoadFromWICFile(ws.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}
	return image;
}