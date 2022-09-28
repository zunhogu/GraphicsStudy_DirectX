#include "fontclass.h"

FontClass::FontClass(){
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other){}

FontClass::~FontClass(){}

// Initialize 함수는 화면크기와 이미지 크기를 저장한다. 렌더링 할 정확한 정점 위치가 필요하기 때문이다.
bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename){
	bool result;

	// 폰트 인덱스 파일을 로드한다.
	result = LoadFontData(fontFilename);
	if (!result){
		return false;
	}

	// 폰트 텍스쳐 파일을 로드한다.
	result = LoadTexture(device, textureFilename);
	if (!result){
		return false;
	}

	return true;
}

void FontClass::Shutdown(){
	// 텍스처를 반환한다.
	ReleaseTexture();

	// 버퍼를 반환한다.
	ReleaseFontData();

	return;
}

// LoadFontData 함수는 텍스처에서의 글자 위치 정보를 담고있는 fontdata.txt 파일을 불러온다.
bool FontClass::LoadFontData(char* filename) {
	ifstream fin;
	int i;
	char temp;

	// FontType형 배열을 만든다. 배열의 크기는 텍스처에 있는 글자수, fontdata.txt 파일의 인덱스 개수인 95로 설정한다.
	m_Font = new FontType[95];
	if (!m_Font) {
		return false;
	}

	// fontdata.txt 파일의 각 라인을 읽어 m_Font 배열에 저장한다. 여기서는 단순히 텍스처의 U좌표인 left와 right 그리고 문자의 픽셀 너비를 읽어들인다.
	fin.open(filename);
	if (fin.fail()){
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (i = 0; i < 95; i++){
		fin.get(temp);
		while (temp != ' '){
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' '){
			fin.get(temp);
		}
		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// 파일을 닫는다. 
	fin.close();

	return true;
}

// ReleaseFontData 함수는 앞서 읽어들인 인덱스 데이터를 해제한다.
void FontClass::ReleaseFontData() {
	if (m_Font) {
		delete[] m_Font;
		m_Font = 0;
	}
	return;
}

// LoadTexture 함수는 font.dds 파일을 읽어 텍스처 셰이더에 전달한다. 이 텍스처에서 글자를 뽑아내 각각의 사각형에 입혀 화면에 그린다. 
bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename){
	bool result;

	// 텍스철르 생성한다.
	m_Texture = new TextureClass;
	if (!m_Texture){
		return false;
	}

	// 텍스처를 초기화한다.
	result = m_Texture->Initialize(device, filename);
	if (!result){
		return false;
	}

	return true;
}

// ReleaseTexture 함수는 글꼴에 사용된 텍스쳐를 해제한다.
void FontClass::ReleaseTexture(){

	// 텍스처 파일을 반환한다.
	if (m_Texture){
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

// GetTexture 함수는 텍스처 자원에 대한 포인터를 TextClass로 반환하고 셰이더 클래스에서 이를 가지고 그림을 그릴때 접근한다.
ID3D11ShaderResourceView* FontClass::GetTexture(){
	return m_Texture->GetTexture();
}

// BuildVertexArray 함수는 TextClass에서 호출되어 인자로 받은 문장으로 정점버퍼를 만들어낸다.
// 이렇게 TextClass는 만들어진 자신만의 정점버퍼를 가지게 되어 손쉽게 글자를 그릴 수 있다.
// 첫번째 인자인 vertices는 정점배열을 가리키는 포인터로서 TextClass에게 이 함수에서 내용을 채운 배열을 전달하는데 사용된다.
// 두번째 인자인 senetences는 정점배열을 만드는데 필요한 문장이다.
// drawX와 drawY는 문자이 화면에 그려질 화면상의 좌표다.
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY) {
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	// void 포인터로 들어온 정점배열 포인터의 자료형 지정
	vertexPtr = (VertexType*)vertices;

	// 그릴 문자열의 길이 받음
	numLetters = (int)strlen(sentence);

	// 정점배열의 인덱스 초기화
	index = 0;

	// 반복문을 돌면서 정점/인덱스 배열을 만든다.
	// 우선 문장을 구성하는 문자들을 취하여 문자당 두개의 삼각형을 만든다.
	// 그리고 나서 이 두삼각형에 실제로 그려질 글장에 해당하는 텍스처를 m_Font 배열의 U좌표와 픽셀 너비를 사용하여 매핑시킨다.
	// 일단 글자에 해당하는 도형이 생성되면 X좌표를 갱신하여 글자가 그려질 위치를 잡는다.
	// Draw each letter onto a quad.
	for (i = 0; i < numLetters; i++){
		// 문자열에서 문자를 하나씩 읽는다.
		letter = ((int)sentence[i]) - 32;

		// 문자가 빈공간이면 3픽셀을 옮겨라
		if (letter == 0){
			drawX = drawX + 3.0f;
		}
		else{
			// 첫번째 삼각형 만든다.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// 두번째 삼각형만든다.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// 한글자를 썼으면 x좌표를 한 픽셀 옮긴다. 
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
}

