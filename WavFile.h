#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#define HeadBufferSize 256

struct WaveHeader
{
    DWORD dwChunkSize; // �ļ���С �ܴ�С-8���ֽ�Ϊ��λ��
    DWORD dwSubChunkSize; // ��ʽ�鳤��
    USHORT usAudioFormat; // �����ʽ
    USHORT usNumChannels; // ��������
    DWORD dwSampleRate; // ����Ƶ�ʱ���44100khz
    DWORD dwBytesPerSec; // ���ݴ������ʣ�����ֵΪ:������������Ƶ�ʡ�ÿ����������λ��/8������������ô�ֵ���Թ��ƻ������Ĵ�С
    USHORT usBlockAlign; // ���ݿ���뵥λ������֡��С������ֵΪ:��������λ��/8�����������Ҫһ�δ�������ֵ��С���ֽ�����,�ø���ֵ����������
    USHORT usBitsPerSamp; // ����λ��
    DWORD dwDataSize; // data ���С���ֽ�Ϊ��λ��
};

class WavFile
{
private:
    WaveHeader WaveHead;
    BYTE* WaveData;

    void PrintfData();
public:
    WavFile();
    ~WavFile();
	bool LoadFromFile(const TCHAR* file);
    const WaveHeader* GetHeadData();
    BYTE* GetDataBuffer();
};

