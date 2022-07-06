#include "WavFile.h"

WavFile::WavFile()
{
}

WavFile::~WavFile()
{
    delete[] this->WaveData;
}

bool WavFile::LoadFromFile(const TCHAR* file)
{
    memset(&this->WaveHead, 0, sizeof(WaveHeader));
    FILE* pFile;
#ifdef UNICODE

    if (_wfopen_s(&pFile, file, __TEXT("rb")) != 0)
    {
        return false;
    }

#else

    if (fopen_s(&pFile, file, __TEXT("rb")) != 0)
    {
        return false;
    }

#endif

    UWORD HeadSize = 0;
    BYTE* text = new BYTE[HeadBufferSize];
    fread(text, HeadBufferSize, 1, pFile);

    for (size_t i = 0; i < HeadBufferSize; i++)
    {
        if (text[i] == 'R' && text[i + 1] == 'I' && text[i + 2] == 'F' && text[i + 3] == 'F')
        {
            // 文件大小 总大小-8（字节为单位）
            DWORD temp = 0x00000000;
            temp = temp | text[i + 7];
            temp <<= 8;
            temp = temp | text[i + 6];
            temp <<= 8;
            temp = temp | text[i + 5];
            temp <<= 8;
            temp = temp | text[i + 4];
            this->WaveHead.dwChunkSize = temp;
        }
        if (text[i] == 'W' && text[i + 1] == 'A' && text[i + 2] == 'V' && text[i + 3] == 'E' && 
            text[i + 4] == 'f' && text[i + 5] == 'm' && text[i + 6] == 't' && text[i + 7] == ' ')
        {
            // 格式块长度
            DWORD temp = 0x00000000;
            temp = temp | text[i + 11];
            temp <<= 8;
            temp = temp | text[i + 10];
            temp <<= 8;
            temp = temp | text[i + 9];
            temp <<= 8;
            temp = temp | text[i + 8];
            this->WaveHead.dwSubChunkSize = temp;
            // 编码格式
            USHORT temp2 = 0x0000;
            temp2 = temp2 | text[i + 13];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 12];
            this->WaveHead.usAudioFormat = temp2;
            // 声道个数
            temp2 = 0x0000;
            temp2 = temp2 | text[i + 15];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 14];
            this->WaveHead.usNumChannels = temp2;
            // 采样频率比如44100khz
            temp = 0x00000000;
            temp = temp | text[i + 19];
            temp <<= 8;
            temp = temp | text[i + 18];
            temp <<= 8;
            temp = temp | text[i + 17];
            temp <<= 8;
            temp = temp | text[i + 16];
            this->WaveHead.dwSampleRate = temp;
            // 数据传输速率
            temp = 0x00000000;
            temp = temp | text[i + 23];
            temp <<= 8;
            temp = temp | text[i + 22];
            temp <<= 8;
            temp = temp | text[i + 21];
            temp <<= 8;
            temp = temp | text[i + 20];
            this->WaveHead.dwBytesPerSec = temp;
            // 数据块对齐单位
            temp2 = 0x0000;
            temp2 = temp2 | text[i + 25];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 24];
            this->WaveHead.usBlockAlign = temp2;
            // 采样位数
            temp2 = 0x0000;
            temp2 = temp2 | text[i + 27];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 26];
            this->WaveHead.usBitsPerSamp = temp2;
        }
        if (text[i] == 'd' && text[i + 1] == 'a' && text[i + 2] == 't' && text[i + 3] == 'a')
        {
            // data 块大小（字节为单位）
            DWORD temp = 0x00000000;
            temp = temp | text[i + 7];
            temp <<= 8;
            temp = temp | text[i + 6];
            temp <<= 8;
            temp = temp | text[i + 5];
            temp <<= 8;
            temp = temp | text[i + 4];
            this->WaveHead.dwDataSize = temp;
            HeadSize += 8;
            break;
        }
        HeadSize++;
    }

    fseek(pFile, HeadSize, SEEK_SET);     // 复位文件指针到 data 区开始
    this->WaveData = new BYTE[this->WaveHead.dwDataSize];
    memset(this->WaveData, 0, this->WaveHead.dwDataSize);
    fread(this->WaveData, this->WaveHead.dwDataSize, 1, pFile);

    delete[] text;
    fclose(pFile);
    this->PrintfData();
	return true;
}

const WaveHeader* WavFile::GetHeadData()
{
    return &this->WaveHead;
}

BYTE* WavFile::GetDataBuffer()
{
    return this->WaveData;
}

void WavFile::PrintfData()
{
    printf("音频数据：\n");
    printf("文件大小：%d\n", this->WaveHead.dwChunkSize);
    printf("格式块长度：%d\n", this->WaveHead.dwSubChunkSize);
    printf("编码格式：%d\n", this->WaveHead.usAudioFormat);
    printf("声道个数：%d\n", this->WaveHead.usNumChannels);
    printf("采样频率：%d\n", this->WaveHead.dwSampleRate);
    printf("数据传输速率：%d\n", this->WaveHead.dwBytesPerSec);
    printf("数据块对齐单位：%d\n", this->WaveHead.usBlockAlign);
    printf("采样位数：%d\n", this->WaveHead.usBitsPerSamp);
    printf("data 块大小：%d\n", this->WaveHead.dwDataSize);
    printf("总时长：%f 分\n", (float)this->WaveHead.dwDataSize / (float)this->WaveHead.dwBytesPerSec / 60.0f);
}
