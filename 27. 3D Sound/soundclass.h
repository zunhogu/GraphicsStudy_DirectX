#ifndef _SOUNDCLASS_H_ 
#define _SOUNDCLASS_H_

// DirectSound�� ������ �Ǳ� ���ؼ��� ���� ���̺귯���� ������� �ʿ��ϴ�.
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib") 
#pragma comment(lib, "winmm.lib")

#include <windows.h> 
#include <mmsystem.h> 
#include <dsound.h> 
#include <stdio.h>

class SoundClass {
private:
	// WaveHeaderType ����ü�� .wav ���� ������ ���Ѱ��̴�.
	// wav ������ �ε��Ҷ��� �켱 ������ ��������� �о�� ����� �����͸� �ε��ϱ� ���� �������� �����´�.
	// ���� �ٸ� ����� ���� ������ ����Ѵٸ� �ٸ� ��� �������� �����ؾ��Ѵ�.
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

	// Initialize �Լ��� DirectSound�� �ʱ�ȭ�ϰ�, .wav ����� ������ �ε��ϰ� ����ϴ� �����Ѵ�.
	//Shutdown �Լ���.wav ���ϰ� DirectSound�� �����Ѵ�.
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

	// û�ڰ�ü�� �����ϳ�.
	IDirectSound3DListener8* m_listener; 
	IDirectSoundBuffer8* m_secondaryBuffer1;
	// 3D ���带 ������ 3D ���� ���۸� �����Ѵ�.
	IDirectSound3DBuffer8* m_secondary3DBuffer1;
};


#endif