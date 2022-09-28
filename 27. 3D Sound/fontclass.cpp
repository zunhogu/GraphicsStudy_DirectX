#include "fontclass.h"

FontClass::FontClass(){
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other){}

FontClass::~FontClass(){}

// Initialize �Լ��� ȭ��ũ��� �̹��� ũ�⸦ �����Ѵ�. ������ �� ��Ȯ�� ���� ��ġ�� �ʿ��ϱ� �����̴�.
bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename){
	bool result;

	// ��Ʈ �ε��� ������ �ε��Ѵ�.
	result = LoadFontData(fontFilename);
	if (!result){
		return false;
	}

	// ��Ʈ �ؽ��� ������ �ε��Ѵ�.
	result = LoadTexture(device, textureFilename);
	if (!result){
		return false;
	}

	return true;
}

void FontClass::Shutdown(){
	// �ؽ�ó�� ��ȯ�Ѵ�.
	ReleaseTexture();

	// ���۸� ��ȯ�Ѵ�.
	ReleaseFontData();

	return;
}

// LoadFontData �Լ��� �ؽ�ó������ ���� ��ġ ������ ����ִ� fontdata.txt ������ �ҷ��´�.
bool FontClass::LoadFontData(char* filename) {
	ifstream fin;
	int i;
	char temp;

	// FontType�� �迭�� �����. �迭�� ũ��� �ؽ�ó�� �ִ� ���ڼ�, fontdata.txt ������ �ε��� ������ 95�� �����Ѵ�.
	m_Font = new FontType[95];
	if (!m_Font) {
		return false;
	}

	// fontdata.txt ������ �� ������ �о� m_Font �迭�� �����Ѵ�. ���⼭�� �ܼ��� �ؽ�ó�� U��ǥ�� left�� right �׸��� ������ �ȼ� �ʺ� �о���δ�.
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

	// ������ �ݴ´�. 
	fin.close();

	return true;
}

// ReleaseFontData �Լ��� �ռ� �о���� �ε��� �����͸� �����Ѵ�.
void FontClass::ReleaseFontData() {
	if (m_Font) {
		delete[] m_Font;
		m_Font = 0;
	}
	return;
}

// LoadTexture �Լ��� font.dds ������ �о� �ؽ�ó ���̴��� �����Ѵ�. �� �ؽ�ó���� ���ڸ� �̾Ƴ� ������ �簢���� ���� ȭ�鿡 �׸���. 
bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename){
	bool result;

	// �ؽ�ö�� �����Ѵ�.
	m_Texture = new TextureClass;
	if (!m_Texture){
		return false;
	}

	// �ؽ�ó�� �ʱ�ȭ�Ѵ�.
	result = m_Texture->Initialize(device, filename);
	if (!result){
		return false;
	}

	return true;
}

// ReleaseTexture �Լ��� �۲ÿ� ���� �ؽ��ĸ� �����Ѵ�.
void FontClass::ReleaseTexture(){

	// �ؽ�ó ������ ��ȯ�Ѵ�.
	if (m_Texture){
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

// GetTexture �Լ��� �ؽ�ó �ڿ��� ���� �����͸� TextClass�� ��ȯ�ϰ� ���̴� Ŭ�������� �̸� ������ �׸��� �׸��� �����Ѵ�.
ID3D11ShaderResourceView* FontClass::GetTexture(){
	return m_Texture->GetTexture();
}

// BuildVertexArray �Լ��� TextClass���� ȣ��Ǿ� ���ڷ� ���� �������� �������۸� ������.
// �̷��� TextClass�� ������� �ڽŸ��� �������۸� ������ �Ǿ� �ս��� ���ڸ� �׸� �� �ִ�.
// ù��° ������ vertices�� �����迭�� ����Ű�� �����ͷμ� TextClass���� �� �Լ����� ������ ä�� �迭�� �����ϴµ� ���ȴ�.
// �ι�° ������ senetences�� �����迭�� ����µ� �ʿ��� �����̴�.
// drawX�� drawY�� ������ ȭ�鿡 �׷��� ȭ����� ��ǥ��.
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY) {
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	// void �����ͷ� ���� �����迭 �������� �ڷ��� ����
	vertexPtr = (VertexType*)vertices;

	// �׸� ���ڿ��� ���� ����
	numLetters = (int)strlen(sentence);

	// �����迭�� �ε��� �ʱ�ȭ
	index = 0;

	// �ݺ����� ���鼭 ����/�ε��� �迭�� �����.
	// �켱 ������ �����ϴ� ���ڵ��� ���Ͽ� ���ڴ� �ΰ��� �ﰢ���� �����.
	// �׸��� ���� �� �λﰢ���� ������ �׷��� ���忡 �ش��ϴ� �ؽ�ó�� m_Font �迭�� U��ǥ�� �ȼ� �ʺ� ����Ͽ� ���ν�Ų��.
	// �ϴ� ���ڿ� �ش��ϴ� ������ �����Ǹ� X��ǥ�� �����Ͽ� ���ڰ� �׷��� ��ġ�� ��´�.
	// Draw each letter onto a quad.
	for (i = 0; i < numLetters; i++){
		// ���ڿ����� ���ڸ� �ϳ��� �д´�.
		letter = ((int)sentence[i]) - 32;

		// ���ڰ� ������̸� 3�ȼ��� �Űܶ�
		if (letter == 0){
			drawX = drawX + 3.0f;
		}
		else{
			// ù��° �ﰢ�� �����.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// �ι�° �ﰢ�������.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// �ѱ��ڸ� ������ x��ǥ�� �� �ȼ� �ű��. 
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
}

