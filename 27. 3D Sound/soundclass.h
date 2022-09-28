#ifndef _SOUNDCLASS_H_ 
#define _SOUNDCLASS_H_

// DirectSound가 컴파일 되기 위해서는 다음 라이브러리와 헤더들이 필요하다.
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib") 
#pragma comment(lib, "winmm.lib")

#include <windows.h> 
#include <mmsystem.h> 
#include <dsound.h> 
#include <stdio.h>

class SoundClass {
private:
	// WaveHeaderType 구조체는 .wav 파일 포맷을 위한것이다.
	// wav 파일을 로드할때는 우선 파일의 헤더정보를 읽어와 오디오 데이터를 로드하기 위한 정보들을 가져온다.
	// 만약 다른 오디오 파일 포맷을 사용한다면 다른 헤더 형식으로 변경해야한다.
	struct WaveHeaderType {
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};
public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	// Initialize 함수는 DirectSound를 초기화하고, .wav 오디오 파일을 로드하고 재생하는 일을한다.
	//Shutdown 함수는.wav 파일과 DirectSound를 해제한다.
	bool Initialize(HWND);
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);

	bool PlayWaveFile();

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;

	// 청자객체를 생성하낟.
	IDirectSound3DListener8* m_listener; 
	IDirectSoundBuffer8* m_secondaryBuffer1;
	// 3D 사운드를 조절할 3D 음향 버퍼를 선언한다.
	IDirectSound3DBuffer8* m_secondary3DBuffer1;
};


#endif