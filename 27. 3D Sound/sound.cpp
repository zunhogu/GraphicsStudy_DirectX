#include "soundclass.h"

SoundClass::SoundClass() {
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
	m_listener = 0; 
	m_secondaryBuffer1 = 0;
	m_secondary3DBuffer1 = 0;
}


SoundClass::SoundClass(const SoundClass& other) {}


SoundClass::~SoundClass() {}


bool SoundClass::Initialize(HWND hwnd)
{
	bool result;

	// DirectSound API 1�� ���۸� �ʱ�ȭ�Ѵ�. �� �������� LoadFile �Լ��� ȣ���Ͽ� .wav ������ �ε��ϰ� �װ����� 2�� ���۸� �ʱ�ȭ�Ѵ�.
	// �ε尡 �Ϸ�Ǹ� PlayWaveFile�� ȣ���Ͽ� .wav ������ ����Ѵ�.
	result = InitializeDirectSound(hwnd);
	if (!result) {
		return false;
	}

	result = LoadWaveFile("../Engine/data/sound02.wav", &m_secondaryBuffer1, &m_secondary3DBuffer1);
	if (!result) { 
		return false;
	}

	result = PlayWaveFile();
	if (!result) {
		return false;
	}

	return true;
}

// Shutdown �Լ��� ShutdownWaveFile �Լ��� ���Ͽ�.wav ����� �����͸� ������ �ִ� 2�� ���۸� �����մϴ�.
// �װ��� �Ϸ�� �Ŀ��� ShutdownDirectSound �Լ��� 1�� ���ۿ� DirectSound �������̽��� �����մϴ�.
void SoundClass::Shutdown() {

	// 2�� ���� ����
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);


	// DirectSound API 1�� ���ۿ� DirectSound �������̽� ����
	ShutdownDirectSound();

	return;
}

// InitializeDirectSound �Լ��� DirectSound�� 1�� ������ �������̽��� ���� �����͸� ���´�.
// ���⼭ ��� ���� ��ġ�� ��ȸ�Ͽ� �� �� Ư�� ��ġ�� 1�� ���۷� ���� �� �����͸� ���� �� �ִ�.
// ����� �⺻ ������ġ������ �����͸� ���´�.
bool SoundClass::InitializeDirectSound(HWND hwnd) {
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	// �⺻ ������ġ������ �����͸� �����Ѵ�.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ������ �����ͷ� ���� ������ �����Ѵ� 
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result)) {
		return false;
	}
	// 1�� ���ۿ� ������ ���� description�� �ۼ��ؾ��Ѵ�.
	// ���⼭�� dwFlags�� �߿��ϴ�. �̰����� 1�� ������ ������ ������ �� �ֵ��� �ۼ��Ѵ�. 

	// 1�� ���ۿ� ������ ���� description�� �ۼ�
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;  // DSBCPAS_CTRL_3D ����ũ�� ����ؼ� ����ȿ���� 3D ����� ������ �˷��ش�.
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 1�� ���۸� �����Ѵ�.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) {
		return false;
	}

	// �⺻ ������ġ�� 1�� ���۷� �Ͽ� ������� ����ٸ� �� ��ġ�� ������ �츮�� ����� ���� �������� �ٲ۴�.
	// ��ǰ�� ���� ǰ���� ���ϱ� ������ ����� CD ����� ǰ���� �����Ѵ�. 
	// �� ���� ������ �Ҹ� ������ �����ϰ� �ΰ�, ���� ���� ���۸� ��� ����� ����ϵ��� �����Ѵ�. Direct Sound�� �ڵ����� ���� �ٸ� ���� ���� ���۵��� �� ���۸� �����ٰ��̴�.
	// Format�� ���� description �ۼ�
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 1�����۸� �츮�� ����� ���� ���� ����
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result)) {
		return false;
	}

	// �� ���� ���۰� ��������� ���⿡�� û�� �������̽��� ������ �� �������̴�. �̸� ���� 3���� ���迡�� ��ġ�� �Ҹ��� ��� û�ڸ� ��ġ��ų �� �ְ� �ȴ�.
	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener); 
	if (FAILED(result)) { 
		return false; 
	}

	// �ϴ� û���� �ʱ� ��ġ�� �������� �����Ѵ�. DSC3D_IMMEDIATE ���ڴ� DIrect Sound�� ��ġ�� ���� ������ ���߿� ó������ �ʰ� ���� �ٷ�ó���ϵ��� �ϴ� ���̴�.
	m_listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

	return true;

}

// ShutdownDirectSound �Լ��� 1�� ���ۿ� DirectSound�� ������ ����Ѵ�.
void SoundClass::ShutdownDirectSound()
{
	// û�� ��ü ����
	if (m_listener) { 
		m_listener->Release(); 
		m_listener = 0; 
	}

	// 1�� ���� ����
	if (m_primaryBuffer) {
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// DirectSound �������̽� ����
	if (m_DirectSound) {
		m_DirectSound->Release();
		m_DirectSound = 0;
	}
	return;
}

// LoadWaveFile �Լ���.wav ����� ������ �ε��Ͽ� �� �����͸� 2�� ���ۿ� �����Ѵ�.
// �ٸ� �������� �ϱ� ���Ѵٸ� �� �Լ��� �ٲٰų� �׷� ����� ���ο� �Լ��� ����� ����� �� �ִ�.
bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer){
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	// �켱 .wav ������ ���� ������ ��� ������ �о���δ�.
	// ������� ����� ������ ���� ������ �־ �̸� �̿��Ͽ� �����͸� ���� 2�� ���۸� ������ �� �ִ�.
	// ����� ������ ������� ���� �����Ͱ� �󸶳� ũ�� ��� �������� �����ϴ����� ���� ������ ����ִ�.
	// ���⼭ RIFF�� WAVE, WAVE_FORMAT_PCM �� ���� �±׸� �о ������ �ùٸ� ����� �������� Ȯ���Ѵ�.
	// ���� ��� �۾��� ���Ͽ� 44.1KHz ���׷��� 16bit ����� �������� Ȯ���Ѵ�.
	// ���� �� ������ 22.1KHz�̰ų� 8��Ʈ�̰ų� ����� �ٸ� ����� �� ������ �䱸���׿� ��ġ���� �ʾ� �����Ѵ�.

	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0) {
		return false;
	}

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1) {
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F')) {
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E')) {
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' ')) {
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM) {
		return false;
	}

	// 3D ���� ������ �ݵ�� ���� ä��(��� ����)�����Ѵ�!!!
	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 1) {
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != 44100) {
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16) {
		return false;
	}

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') || (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a')) {
		return false;
	}

	// ������ ���̺� ������ ����� ����� ������ �����̹Ƿ� �����͸� ������ 2�� ���۸� ������ �� �ִ�.
	// 1�� ���ۿ� ����ϰ� �켱 ���̺� ������ �����ϰ� ������ description�� �ۼ��Ѵ�.
	// 1�� ���۰� �ƴϱ⶧���� dwFlag�� dwBufferBytes�� ���� ���� �ٸ� ���̴�.
	// ���� ���� ���۴� 2�� ä��(���׷���)�� �ƴ϶� ���� ä��(���)�� ����ϵ��� �Ѵ�.
	// ���̺� ������ description �ۼ�
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 2�� ���� description �ۼ�
	// ���ۿ� 3D �ɼ��� �ֱ����� dwFlags ���ڿ� DSBCAPS_CTRL_3D ��Ʈ ����ũ�� ����Ͽ� �˷��ش�.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 2�� ���۸� �����ϴ� ����� ����� �̻��ϴ�.
	// �켱 ������ description�� ������ �ӽ������� IDirectSoundBuffer ��ü�� �����.
	// �̰��� �����ϸ� QueryInterface�� IID_IDirectSoundBuffer8 ���ڿ� �԰� ȣ���Ͽ� �� �ӽ� ���۷� IDirectSoundBuffer8�� 2�� ���۸� ����� �ȴ�.

	// IDirectSoundBuffer ��ü�� �����.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 2������ ����
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ӽ� IDirectSoundBuffer�� ��ȯ�Ѵ�.
	tempBuffer->Release();
	tempBuffer = 0;

	// 2�� ���۰� �غ�Ǹ� ����� ������ ���̺� �����͸� �����´�.
	// �켱 �޸� ���ۿ� �ҷ��鿩�� �ʿ��ϴٸ� ������ ������ ���Ѵ�.
	// �� �ڿ� 2�� ���۸� ��� ���� ������ ����� ��, memcpy�� ����Ͽ� �����͸� �����ϰ� ����� �����Ѵ�.
	// ���� 2�� ���� ����� �غ�� �Ǿ���. ������ ���� ���� �������� ���� �ٸ���.
	// 2�� ���۸� ��� �� �ΰ��� �����Ͱ� �ʿ��ϴ�.
	// ���۰� �������� �����Ǿ� �ֱ⶧���� �߰����� ���۸� ���� �Ǵ� ��� �� �������� ���� ����� �ƴ� ���� �ʿ��ϱ� �����̴�.
	// �׷��� ������ ������ �Ѿ�� �ʰ� ���� �۾��� �� �� �ִ�.
	// �̷� ������ ����� ����ϴ� �Ͱ� �����Ͽ� �����ϴ�
	// �� Ʃ�丮�󿡼��� �۾��� �����ϰ� �ϱ� ���� ����� ���ϰ� ���� ũ���� ���۸� ����� ������ ���ۺ��� ������ ����Ѵ�.


	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData) {
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize) {
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result)) {
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result)) {
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	// wav������ ���� ���� ���۷� �ҷ������Ƿ� ���⿡�� 3D �������̽��� ���� �� �ִ�. ������ ���� ������ ���� ���� ���� ����� secondaryBuffer�� ����ؾ��Ѵ�.
	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer); 
	if (FAILED(result)) {
		return false;
	}

	return true;

}

// ShutdownWaveFile �Լ��� 2�� ���۸� �����ϴ� �ϸ��� �Ѵ�.
void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	// 3D �������̽� �����Ѵ�.
	if(*secondary3DBuffer) {
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = 0; 
	}

	// 2�� ���۸� �����Ѵ�.
	if (*secondaryBuffer) {
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

// PlayWaveFile �Լ��� 2�� ���ۿ� ����Ǿ� �ִ� ����� ������ ����Ѵ�.
// Play�Լ����� 1�� ������ ������� �ͽ��� �ڵ����� �����ѵ� ����Ѵ�. 
// ���⼭�� 2�� ������ ���� �κ����� ������ġ�� �����Ѵ�.
// �׷��� ������ ���� �ֱٿ� ����Ǿ��� �κк��� �����ϰ� �ȴ�.
// �׸��� �Ҹ��� ������ �� �ְ� �����Ͽ����Ƿ� ������ �ִ�� ���纸��
bool SoundClass::PlayWaveFile()
{
	HRESULT result;
	float positionX, positionY, positionZ;

	// 3D ������ ��ġ�ؾ��� ������ ��ġ�� �����Ѵ�. ����� �������� �Ѵ�.
	positionX = -2.0f; 
	positionY = 0.0f;
	positionZ = 0.0f;

	// Set position at the beginning of the sound buffer.
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result)){
		return false;
	}

	// Set volume of the buffer to 100%.
	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result)){
		return false;
	}

	// 3D �������̽� ������ ��ġ�� 3���� ��ǥ�� �����Ѵ�. DS3D_IMMEDIATE �÷��״� ���� ��ġ�� ������ ���� �̷�� ó������ �ʰ� ��� ó���ϴ� �÷��״�.
	m_secondary3DBuffer1->SetPosition(positionX, positionY, positionZ, DS3D_IMMEDIATE);

	// Play the contents of the secondary sound buffer.
	result = m_secondaryBuffer1->Play(0, 0, 0);
	if (FAILED(result)){
		return false;
	}

	return true;
}
