// Maya 2011 .obj ���ϵ��� �츮�� DirectX11 �������� �ٲٴ� ���α׷��� �� �����Ͽ��� main.cpp �ϳ��� �־ ����ϴ�.
// �� ���α׷��� Ŀ�ǵ� â�� ���� ��ȯ�� .obj ������ �̸��� ���´�.
// ������ �� ���� ���� �Է��ϸ� ������ ���� �о� ����ü���� ä���ִ´�.
// ���� �о���� �� �޼� ��ǥ��� �ٲ۴� (Maya�� ������ ��ǥ��)
// �� �� ����ü���� modle.txt �� �����ȴ�. 
// ������ ������� DirectX11 ���α׷����� �ҷ����δ�.

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

	// ���� �̸��� �Է¹޴´�.
	GetModelFilename(filename);

	// �Է¹��� �����̸����� ������ �д´�.
	// ���� ������ ������ ������ ����, uv��ǥ, ��������, �ﰢ�� ǥ���� �о ����ü�� �ִ´�.
	// �޼���ǥ�� �ٲ۴��� �ؽ�Ʈ���Ͽ� �����Ѵ�.
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result) {
		return -1;
	}

	// ���� ���� �ùٸ��� Ŀ�ǵ�â�� �����ش�.
	cout << endl;
	cout << "Vertices : " << vertexCount <<endl;
	cout << "UVs : " << textureCount << endl;
	cout << "Normals : " << normalCount << endl;
	cout << "Faces : " << faceCount << endl;

	// ���� ������ ����ü�� �����Ѵ�. 
	result = LoadDataStructures(filename, vertexCount, normalCount, normalCount, faceCount);
	if (!result) {
		return -1;
	}

	// ��ȯ�� �Ϸ�Ǿ����� �˸���.
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
		// ���� �̸��� �Է¹޴´�.
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

	// ������ ����.
	fin.open(filename);
	if (fin.fail() == true) {
		return false; 
	}

	// ������ �д´�.
	fin.get(input);
	while (!fin.eof()) {
		
		// ������ v�� ���ؼ� ã�´�.
		if (input == 'v') {
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// ������ f�� ���ؼ� ã�´�. 
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// ������ v�� f�� �ƴϸ� ������ �Ѿ��.
		while (input != '\n') {
			fin.get(input);
		}

		// �������� ���� ���ڸ� �д´�. 
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

	// ī������ ������ ������ ����ü�� �����.
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

	// ������ �ѹ� ���о� �̹����� ������ �ƴ� �����͸� �޾ƿ´�. 
	fin.open(filename);

	if (fin.fail() == true) {
		return false;
	}
	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);

			// ������� �����ڷ� �����͵��� �д´�.
			if (input == ' ') {
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// �о� ���� �����͸� �޼���ǥ��� �����Ѵ�.
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
				// faces�� ���� �ҽ����� ������ �籸���ϱ� ���ٴ� �׸��� ������ �ð���⿡�� �ݽð�������� �ٲ۴�. 
				// ���� �Էµ� �ݴ�� �Է����ش�. 
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;

				faceIndex++;
			}
		}
		// �����ٷ� �Ѿ
		while (input != '\n') {
			fin.get(input);
		}
		// �������� ù��° ���� ����
		fin.get(input);
	}
	fin.close();

	// output file�� ����
	fout.open("model.txt");

	// output file�� ���� ���� ���
	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	// output file�� ���� ������ ���, ����� f�������� ��µ�
	for (int i = 0; i < faceIndex; i++) {
		// ���� obj ������ f�� ��� 0 �ε����� ���⶧���� -1�� ���־ �迭�� 0�� �ε������� ���ٰ����ϰԲ� �ؾ��Ѵ�.
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

	// output file �ݴ´�.
	 fout.close(); 

	// �����͵��� ��ȯ�Ѵ�.
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

