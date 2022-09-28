#include "texturearrayclass.h"
#include <string>
using namespace std;

TextureArrayClass::TextureArrayClass() {
	m_textures[0] = 0;
	m_textures[1] = 0;
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass& other) {}

TextureArrayClass::~TextureArrayClass() {}

// Initialize �Լ��� �ؽ�ó�� �ʱ�ȭ�ϴ� �Լ���.
bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2) {

	// �ؽ�ó ������ �������Ͽ� �ڿ��並 ���� �׷��� ���������ο� ���ε� ���Ѿ��Ѵ�.
	// ����� �ΰ����� �ִ�.
	// DirectX::CreateShaderResourceView�� ����ϴ� ���
	// DirectX::CreateTexture�� ����Ͽ� �ؽ�ó�� �����ϰ� D3D11���� �����ϴ� CreateShaderResourceView�� ����Ͽ� ����� ��� �ΰ����� �ִ�.(ť�� �ؽ�ó�� ���鶧)

	HRESULT result;
	// ��ũ���� �ؽ�ó�� �ε��Ѵ�.
	DirectX::ScratchImage image1 = LoadTextureFromFile(filename1);
	DirectX::ScratchImage image2 = LoadTextureFromFile(filename2);
	// �ε��� �ؽ�ó�� ������ �ڿ��並 �����. 

	// ù�� ° ��� 
	DirectX::CreateShaderResourceView(device, image1.GetImages(), image1.GetImageCount(), image1.GetMetadata(), &m_textures[0]);
	DirectX::CreateShaderResourceView(device, image2.GetImages(), image2.GetImageCount(), image2.GetMetadata(), &m_textures[1]);
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
void TextureArrayClass::Shutdown()
{
	if (m_textures[0]){
		m_textures[0]->Release();
		m_textures[0] = 0;
	}
	if (m_textures[1]) {
		m_textures[1]->Release();
		m_textures[1] = 0;
	}
	return;
}

// GetTexture �Լ��� �ٸ� ��ü�� �� �ؽ�ó �ڿ��� �����Ϸ� �Ҷ� �ڿ��� ��ȯ�Ѵ�.
ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray(){
		return m_textures;
}


DirectX::ScratchImage TextureArrayClass::LoadTextureFromFile(WCHAR* filename) {

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