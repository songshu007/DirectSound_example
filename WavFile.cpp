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
            // �ļ���С �ܴ�С-8���ֽ�Ϊ��λ��
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
            // ��ʽ�鳤��
            DWORD temp = 0x00000000;
            temp = temp | text[i + 11];
            temp <<= 8;
            temp = temp | text[i + 10];
            temp <<= 8;
            temp = temp | text[i + 9];
            temp <<= 8;
            temp = temp | text[i + 8];
            this->WaveHead.dwSubChunkSize = temp;
            // �����ʽ
            USHORT temp2 = 0x0000;
            temp2 = temp2 | text[i + 13];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 12];
            this->WaveHead.usAudioFormat = temp2;
            // ��������
            temp2 = 0x0000;
            temp2 = temp2 | text[i + 15];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 14];
            this->WaveHead.usNumChannels = temp2;
            // ����Ƶ�ʱ���44100khz
            temp = 0x00000000;
            temp = temp | text[i + 19];
            temp <<= 8;
            temp = temp | text[i + 18];
            temp <<= 8;
            temp = temp | text[i + 17];
            temp <<= 8;
            temp = temp | text[i + 16];
            this->WaveHead.dwSampleRate = temp;
            // ���ݴ�������
            temp = 0x00000000;
            temp = temp | text[i + 23];
            temp <<= 8;
            temp = temp | text[i + 22];
            temp <<= 8;
            temp = temp | text[i + 21];
            temp <<= 8;
            temp = temp | text[i + 20];
            this->WaveHead.dwBytesPerSec = temp;
            // ���ݿ���뵥λ
            temp2 = 0x0000;
            temp2 = temp2 | text[i + 25];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 24];
            this->WaveHead.usBlockAlign = temp2;
            // ����λ��
            temp2 = 0x0000;
            temp2 = temp2 | text[i + 27];
            temp2 <<= 8;
            temp2 = temp2 | text[i + 26];
            this->WaveHead.usBitsPerSamp = temp2;
        }
        if (text[i] == 'd' && text[i + 1] == 'a' && text[i + 2] == 't' && text[i + 3] == 'a')
        {
            // data ���С���ֽ�Ϊ��λ��
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

    fseek(pFile, HeadSize, SEEK_SET);     // ��λ�ļ�ָ�뵽 data ����ʼ
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
    printf("��Ƶ���ݣ�\n");
    printf("�ļ���С��%d\n", this->WaveHead.dwChunkSize);
    printf("��ʽ�鳤�ȣ�%d\n", this->WaveHead.dwSubChunkSize);
    printf("�����ʽ��%d\n", this->WaveHead.usAudioFormat);
    printf("����������%d\n", this->WaveHead.usNumChannels);
    printf("����Ƶ�ʣ�%d\n", this->WaveHead.dwSampleRate);
    printf("���ݴ������ʣ�%d\n", this->WaveHead.dwBytesPerSec);
    printf("���ݿ���뵥λ��%d\n", this->WaveHead.usBlockAlign);
    printf("����λ����%d\n", this->WaveHead.usBitsPerSamp);
    printf("data ���С��%d\n", this->WaveHead.dwDataSize);
    printf("��ʱ����%f ��\n", (float)this->WaveHead.dwDataSize / (float)this->WaveHead.dwBytesPerSec / 60.0f);
}
