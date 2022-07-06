#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#define HeadBufferSize 256

struct WaveHeader
{
    DWORD dwChunkSize; // 文件大小 总大小-8（字节为单位）
    DWORD dwSubChunkSize; // 格式块长度
    USHORT usAudioFormat; // 编码格式
    USHORT usNumChannels; // 声道个数
    DWORD dwSampleRate; // 采样频率比如44100khz
    DWORD dwBytesPerSec; // 数据传输速率，该数值为:声道数×采样频率×每样本的数据位数/8。播放软件利用此值可以估计缓冲区的大小
    USHORT usBlockAlign; // 数据块对齐单位，采样帧大小。该数值为:声道数×位数/8。播放软件需要一次处理多个该值大小的字节数据,用该数值调整缓冲区
    USHORT usBitsPerSamp; // 采样位数
    DWORD dwDataSize; // data 块大小（字节为单位）
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

