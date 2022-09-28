#include "texturearrayclass.h"
#include <string>
using namespace std;

TextureArrayClass::TextureArrayClass() {
	m_textures[0] = 0;
	m_textures[1] = 0;
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass& other) {}

TextureArrayClass::~TextureArrayClass() {}

// Initialize 함수는 텍스처를 초기화하는 함수다.
bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2) {

	// 텍스처 파일을 컴파일하여 자원뷰를 만들어서 그래픽 파이프라인에 바인딩 시켜야한다.
	// 방법은 두가지가 있다.
	// DirectX::CreateShaderResourceView를 사용하는 방법
	// DirectX::CreateTexture를 사용하여 텍스처를 생성하고 D3D11에서 제공하는 CreateShaderResourceView를 사용하여 만드는 방법 두가지가 있다.(큐브 텍스처를 만들때)

	HRESULT result;
	// 디스크에서 텍스처를 로드한다.
	DirectX::ScratchImage image1 = LoadTextureFromFile(filename1);
	DirectX::ScratchImage image2 = LoadTextureFromFile(filename2);
	// 로드한 텍스처를 가지고 자원뷰를 만든다. 

	// 첫번 째 방법 
	DirectX::CreateShaderResourceView(device, image1.GetImages(), image1.GetImageCount(), image1.GetMetadata(), &m_textures[0]);
	DirectX::CreateShaderResourceView(device, image2.GetImages(), image2.GetImageCount(), image2.GetMetadata(), &m_textures[1]);
	//
	
	//// 두번 째 방법
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

// Shutdown 함수는 로드된 텍스쳐 자원을 반환하고 그 포인터를 null로 되돌린다.
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

// GetTexture 함수는 다른 객체가 이 텍스처 자원에 접근하려 할때 자원을 반환한다.
ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray(){
		return m_textures;
}


DirectX::ScratchImage TextureArrayClass::LoadTextureFromFile(WCHAR* filename) {

	// WCAHR* -> CHAR* 로변경
	CHAR* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);
	pStr = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, filename, -1, pStr, strSize, 0, 0);
	//

	HRESULT result;
	string str(pStr);
	std::wstring wsTmp(str.begin(), str.end());

	std::wstring ws = wsTmp;

	//텍스처 로드
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