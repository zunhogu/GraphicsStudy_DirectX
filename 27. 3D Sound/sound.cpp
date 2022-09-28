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

	// DirectSound API 1차 버퍼를 초기화한다. 그 다음에는 LoadFile 함수를 호출하여 .wav 파일을 로드하고 그것으로 2차 버퍼를 초기화한다.
	// 로드가 완료되면 PlayWaveFile을 호출하여 .wav 파일을 재생한다.
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

// Shutdown 함수는 ShutdownWaveFile 함수를 통하여.wav 오디오 데이터를 가지고 있는 2차 버퍼를 해제합니다.
// 그것이 완료된 후에는 ShutdownDirectSound 함수로 1차 버퍼와 DirectSound 인터페이스를 해제합니다.
void SoundClass::Shutdown() {

	// 2차 버퍼 해제
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);


	// DirectSound API 1차 버퍼와 DirectSound 인터페이스 해제
	ShutdownDirectSound();

	return;
}

// InitializeDirectSound 함수는 DirectSound와 1차 버퍼의 인터페이스에 대한 포인터를 얻어온다.
// 여기서 모든 음향 장치를 조회하여 그 중 특정 장치를 1차 버퍼로 정해 그 포인터를 얻어올 수 있다.
// 현재는 기본 음향장치에대한 포인터만 얻어온다.
bool SoundClass::InitializeDirectSound(HWND hwnd) {
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	// 기본 음향장치에대한 포인터를 생성한다.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 생성한 포인터로 협력 레벨을 세팅한다 
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result)) {
		return false;
	}
	// 1차 버퍼에 접근을 위한 description을 작성해야한다.
	// 여기서는 dwFlags가 중요하다. 이것으로 1차 버퍼의 볼륨을 조절할 수 있도록 작성한다. 

	// 1차 버퍼에 접근을 위한 description을 작성
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;  // DSBCPAS_CTRL_3D 마스크를 사용해서 음향효과가 3D 기능을 가짐을 알려준다.
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 1차 버퍼를 생성한다.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 기본 음향장치를 1차 버퍼로 하여 제어권을 얻었다면 이 장치의 포맷을 우리의 오디오 파일 포맷으로 바꾼다.
	// 고품질 사운드 품질을 원하기 때문에 비압축 CD 오디오 품질로 설정한다. 
	// 주 음향 버퍼의 소리 포맷을 동일하게 두고, 보조 음향 버퍼만 모노 사운드로 사용하도록 설정한다. Direct Sound는 자동으로 서로 다른 여러 보저 버퍼들을 주 버퍼를 합쳐줄것이다.
	// Format에 대한 description 작성
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 1차버퍼를 우리의 오디오 파일 포맷 세팅
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result)) {
		return false;
	}

	// 주 음향 버퍼가 만들어지면 여기에서 청자 인터페이스를 가져올 수 있을것이다. 이를 통해 3차원 세계에서 배치된 소리를 듣는 청자를 위치시킬 수 있게 된다.
	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener); 
	if (FAILED(result)) { 
		return false; 
	}

	// 일단 청자의 초기 위치를 원점으로 설정한다. DSC3D_IMMEDIATE 인자는 DIrect Sound가 위치의 변경 내용을 나중에 처리하지 않고 지금 바로처리하도록 하는 것이다.
	m_listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

	return true;

}

// ShutdownDirectSound 함수는 1차 버퍼와 DirectSound의 해제를 담당한다.
void SoundClass::ShutdownDirectSound()
{
	// 청자 개체 해재
	if (m_listener) { 
		m_listener->Release(); 
		m_listener = 0; 
	}

	// 1차 버퍼 해제
	if (m_primaryBuffer) {
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// DirectSound 인터페이스 해제
	if (m_DirectSound) {
		m_DirectSound->Release();
		m_DirectSound = 0;
	}
	return;
}

// LoadWaveFile 함수는.wav 오디오 파일을 로드하여 그 데이터를 2차 버퍼에 복사한다.
// 다른 포맷으로 하기 원한다면 이 함수를 바꾸거나 그런 기능의 새로운 함수를 만들어 사용할 수 있다.
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

	// 우선 .wav 파일을 열고 파일의 헤더 정보를 읽어들인다.
	// 헤더에는 오디오 파일의 각종 정보가 있어서 이를 이용하여 데이터를 담을 2차 버퍼를 생성할 수 있다.
	// 오디오 파일의 헤더에는 또한 데이터가 얼마나 크고 어느 지점부터 시작하는지에 대한 정보도 담고있다.
	// 여기서 RIFF나 WAVE, WAVE_FORMAT_PCM 과 같은 태그를 읽어서 포함한 올바른 오디오 파일인지 확인한다.
	// 또한 몇가지 작업을 더하여 44.1KHz 스테레오 16bit 오디오 파일인지 확인한다.
	// 만약 이 파일이 22.1KHz이거나 8비트이거나 등등의 다른 경우라면 이 파일의 요구사항에 일치하지 않아 실패한다.

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

	// 3D 음향 파일은 반드시 단일 채널(모노 사운드)여야한다!!!
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

	// 이제는 웨이브 파일의 헤더가 충분히 검증된 상태이므로 데이터를 복사할 2차 버퍼를 생성할 수 있다.
	// 1차 버퍼와 비슷하게 우선 웨이브 포맷을 설정하고 버퍼의 description을 작성한다.
	// 1차 버퍼가 아니기때문에 dwFlag나 dwBufferBytes는 값이 조금 다를 것이다.
	// 보조 음향 버퍼는 2개 채널(스테레오)가 아니라 단일 채널(모노)를 사용하도록 한다.
	// 웨이브 포맷의 description 작성
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 2차 버퍼 description 작성
	// 버퍼에 3D 옵션을 주기위해 dwFlags 인자에 DSBCAPS_CTRL_3D 비트 마스크를 사용하여 알려준다.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 2차 버퍼를 생성하는 방법은 상당히 이상하다.
	// 우선 버퍼의 description을 가지고 임시적으로 IDirectSoundBuffer 객체를 만든다.
	// 이것이 성공하면 QueryInterface를 IID_IDirectSoundBuffer8 인자와 함게 호출하여 이 임시 버퍼로 IDirectSoundBuffer8의 2차 버퍼를 만들게 된다.

	// IDirectSoundBuffer 객체를 만든다.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 2차버퍼 생성
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 임시 IDirectSoundBuffer는 반환한다.
	tempBuffer->Release();
	tempBuffer = 0;

	// 2차 버퍼가 준비되면 오디오 파일의 웨이브 데이터를 가져온다.
	// 우선 메모리 버퍼에 불러들여서 필요하다면 적절히 수정을 가한다.
	// 그 뒤에 2차 버퍼를 잠궈 접근 권한을 취득한 뒤, memcpy를 사용하여 데이터를 복사하고 잠금을 해제한다.
	// 이제 2차 버퍼 사용할 준비는 되었다. 하지만 기존 버퍼 사용법과는 조금 다르다.
	// 2차 버퍼를 잠글 때 두개의 포인터가 필요하다.
	// 버퍼가 원형으로 구성되어 있기때문에 중간부터 버퍼를 쓰게 되는 경우 그 지점에서 버퍼 사이즈를 아는 것이 필요하기 때문이다.
	// 그래야 버퍼의 범위를 넘어가지 않게 쓰기 작업을 할 수 있다.
	// 이런 구조는 오디오 재생하는 것과 같은일에 유용하다
	// 이 튜토리얼에서는 작업을 간단하게 하기 위해 오디오 파일과 같은 크기의 버퍼를 만들고 버퍼의 시작부터 파일을 기록한다.


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

	// wav파일을 보조 음향 버퍼로 불러왔으므로 여기에서 3D 인터페이스를 얻어올 수 있다. 하지만 볼륨 조절과 같은 기존 음향 기능은 secondaryBuffer를 사용해야한다.
	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer); 
	if (FAILED(result)) {
		return false;
	}

	return true;

}

// ShutdownWaveFile 함수는 2차 버퍼를 해제하는 일만을 한다.
void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	// 3D 인터페이스 해제한다.
	if(*secondary3DBuffer) {
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = 0; 
	}

	// 2차 버퍼를 해제한다.
	if (*secondaryBuffer) {
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

// PlayWaveFile 함순느 2차 버퍼에 저장되어 있는 오디오 파일을 재생한다.
// Play함수에서 1차 버퍼의 오디와의 믹싱을 자동으로 수행한뒤 재생한다. 
// 여기서는 2차 버퍼의 시작 부분으로 시작위치를 지정한다.
// 그렇지 않으면 가장 최근에 재생되었던 부분부터 시작하게 된다.
// 그리고 소리를 조정할 수 있게 세팅하였으므로 음량을 최대로 맞춰보자
bool SoundClass::PlayWaveFile()
{
	HRESULT result;
	float positionX, positionY, positionZ;

	// 3D 음향을 배치해야할 곳으로 위치를 지정한다. 현재는 왼쪽으로 한다.
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

	// 3D 인터페이스 음향의 위치를 3차원 좌표로 설정한다. DS3D_IMMEDIATE 플래그는 음향 위치의 변경을 조금 미루어 처리하지 않고 즉시 처리하는 플래그다.
	m_secondary3DBuffer1->SetPosition(positionX, positionY, positionZ, DS3D_IMMEDIATE);

	// Play the contents of the secondary sound buffer.
	result = m_secondaryBuffer1->Play(0, 0, 0);
	if (FAILED(result)){
		return false;
	}

	return true;
}
