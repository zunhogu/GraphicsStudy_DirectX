// Maya 2011 .obj 파일들을 우리의 DirectX11 포맷으로 바꾸는 프로그램은 꽤 간단하여서 main.cpp 하나만 있어도 충분하다.
// 이 프로그램은 커맨드 창을 열고 변환할 .obj 파일의 이름을 묻는다.
// 유저가 그 파일 명을 입력하면 파일을 열고 읽어 구조체들을 채워넣는다.
// 전부 읽어들인 후 왼손 좌표계로 바꾼다 (Maya는 오른손 좌표계)
// 그 뒤 구조체들을 modle.txt 에 저잫안다. 
// 이전에 만들었던 DirectX11 프로그램으로 불러들인다.

#include <iostream>
#include <fstream>
using namespace std;

typedef struct {
	float x, y, z;
}VertexType;

typedef struct {
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;

void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);

int main() {
	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	// 파일 이름을 입력받는다.
	GetModelFilename(filename);

	// 입력받은 파일이름으로 파일을 읽는다.
	// 읽은 파일을 가지고 정점의 개수, uv좌표, 법선벡터, 삼각형 표면을 읽어서 구조체에 넣는다.
	// 왼손좌표로 바꾼다음 텍스트파일에 저장한다.
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result) {
		return -1;
	}

	// 읽은 값이 올바른지 커맨드창에 보여준다.
	cout << endl;
	cout << "Vertices : " << vertexCount <<endl;
	cout << "UVs : " << textureCount << endl;
	cout << "Normals : " << normalCount << endl;
	cout << "Faces : " << faceCount << endl;

	// 읽은 값들을 구조체에 저장한다. 
	result = LoadDataStructures(filename, vertexCount, normalCount, normalCount, faceCount);
	if (!result) {
		return -1;
	}

	// 변환이 완료되었음을 알린다.
	cout << "\nFile has been converted" << endl;
	cout << "\nDo you wish to exit (y/n)>";
	cin >> garbage;

	return 0;
}

void GetModelFilename(char* filename) {
	bool done;
	ifstream fin;

	done = false;
	while (!done) {
		// 파일 이름을 입력받는다.
		cout << "Enter model filename : ";
		cin >> filename;

		fin.open(filename);

		if (fin.good()) {
			done = true;
		}
		else {
			fin.clear();
			cout << endl;
			cout << "File " << filename << "could not be opened" << endl << endl;
		}
	}
	return;
}

bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount) {
	ifstream fin;
	char input;

	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// 파일을 연다.
	fin.open(filename);
	if (fin.fail() == true) {
		return false; 
	}

	// 파일을 읽는다.
	fin.get(input);
	while (!fin.eof()) {
		
		// 시작인 v에 대해서 찾는다.
		if (input == 'v') {
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// 시작인 f에 대해서 찾는다. 
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// 시작이 v나 f가 아니면 한줄을 넘어간다.
		while (input != '\n') {
			fin.get(input);
		}

		// 다음줄의 시작 문자를 읽는다. 
		fin.get(input);
	}
	fin.close();

	return true;
}

bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount) {
	VertexType* vertices, * texcoords, * normals;
	FaceType* faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	// 카운팅한 개수를 가지고 구조체를 만든다.
	vertices = new VertexType[vertexCount];
	if (!vertices) {
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords) {
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals) {
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces) {
		return false;
	}

	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// 파일을 한번 더읽어 이번에는 개수가 아닌 데이터를 받아온다. 
	fin.open(filename);

	if (fin.fail() == true) {
		return false;
	}
	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);

			// 띄워쓰기 구분자로 데이터들을 읽는다.
			if (input == ' ') {
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// 읽어 들인 데이터를 왼손좌표계로 변경한다.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}
			if (input == 't') {
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				texcoords[texcoordIndex].y = texcoords[texcoordIndex].y * -1.0f;
				texcoordIndex++;
			}
			if (input == 'n') {
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}

		}
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') {
				// faces의 경우는 소스에서 순서를 재구성하기 보다는 그리기 방향을 시계방향에서 반시계방향으로 바꾼다. 
				// 따라서 입력도 반대로 입력해준다. 
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;

				faceIndex++;
			}
		}
		// 다음줄로 넘어감
		while (input != '\n') {
			fin.get(input);
		}
		// 다음줄의 첫번째 문자 읽음
		fin.get(input);
	}
	fin.close();

	// output file을 생성
	fout.open("model.txt");

	// output file에 정점 개수 출력
	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	// output file에 정점 데이터 출력, 출력은 f기준으로 출력됨
	for (int i = 0; i < faceIndex; i++) {
		// 현재 obj 파일의 f의 경우 0 인덱스가 없기때문에 -1을 해주어서 배열의 0번 인덱스에도 접근가능하게끔 해야한다.
		vIndex = faces[i].vIndex1 - 1; 
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1; 
		tIndex = faces[i].tIndex2 - 1; 
		nIndex = faces[i].nIndex2 - 1; 
		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' ' << normals[nIndex].x << ' '
			<< normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1; 
		tIndex = faces[i].tIndex3 - 1; 
		nIndex = faces[i].nIndex3 - 1; 
		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' ' << normals[nIndex].x << ' ' 
			<< normals[nIndex].y << ' ' << normals[nIndex].z << endl;
	}

	// output file 닫는다.
	 fout.close(); 

	// 포인터들을 반환한다.
	if(vertices) {
		delete [] vertices;
		vertices = 0; 
	} 
	if(texcoords) {
		delete [] texcoords;
		texcoords = 0; 
	} 
	if(normals) { 
		delete [] normals;
		normals = 0; 
	} 
	if(faces) {
		delete [] faces; 
		faces = 0; 
	}

	return true;
}

