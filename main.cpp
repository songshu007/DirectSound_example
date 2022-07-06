#include "WavFile.h"
#include <dsound.h>
#include <winerror.h>

#pragma comment(lib, "dsound.lib")

LPGUID GuidList[256] = { 0 };

// ö�������豸�ص�����
BOOL CALLBACK DSEnumCallback(
    LPGUID lpGuid,              // �豸GUID��Ϊ��������ϵ�Ĭ���豸ʱΪNULL
    LPCWSTR lpcstrDescription,   // �ַ�����Ϊ����Ƶ�豸������
    LPCWSTR lpcstrModule,        // �ַ��������ַ���ָ������豸��Ӧ�� DirectSound ���������ģ������
    LPVOID lpContext            // �û��Զ������ݣ��� DirectSoundEnumerate �����ڶ�������ָ��
)
{
    static int num = 0;
    if (lpGuid == NULL)
    {
        wchar_t buf[256] = { 0 };
        wsprintfW(buf, L"�� %d ���豸��%ls��Ĭ���豸��\n", num, lpcstrDescription);
        WriteConsoleW(lpContext, buf, wcslen(buf), 0, 0);
    }
    else
    {
        wchar_t buf[256] = { 0 };
        wsprintfW(buf, L"�� %d ���豸��%ls\n", num, lpcstrDescription);
        WriteConsoleW(lpContext, buf, wcslen(buf), 0, 0);
    }
    GuidList[num] = lpGuid;
    num++;
    return TRUE;
}

int main()
{
    HRESULT hr;
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL)
    {
        hwnd = GetDesktopWindow();
    }
    
    // ö�����������豸
    if (SUCCEEDED(DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumCallback, GetStdHandle(STD_OUTPUT_HANDLE))))
    {
        printf("�豸ö����ɣ�\n");
    }
    else
    {
        printf("�豸ö��ʧ�ܣ�\n");
        return 0;
    }

    LPGUID GUID;
    int a;
    printf("ѡ����Ҫ���ŵ��豸...\n");
    scanf_s("%d", &a);
    GUID = GuidList[a];

    // �����豸
    IDirectSound* ps;
    if (SUCCEEDED(DirectSoundCreate(GUID, &ps, NULL)))
    {
        printf("�豸�����ɹ���\n");
    }
    else
    {
        printf("�豸����ʧ�ܣ�\n");
        return 0;
    }

    // ��������Э����
    ps->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE);

    // ����Ƶ�ļ�
    printf("����Ҫ���ŵ��ļ�����...\n");
    std::wstring text3;
    std::wcin.imbue(std::locale("chs"));
    std::wcin >> text3;

    WavFile waveFile;
    if (waveFile.LoadFromFile(text3.c_str()) != true)
    {
        printf("�ļ���ʧ�ܣ�");
        return 0;
    }

    // ��������������
    WAVEFORMATEX wav;
    memset(&wav, 0, sizeof(WAVEFORMATEX));
    wav.wFormatTag = WAVE_FORMAT_PCM;
    wav.nChannels = waveFile.GetHeadData()->usNumChannels;
    wav.nSamplesPerSec = waveFile.GetHeadData()->dwSampleRate;
    wav.wBitsPerSample = waveFile.GetHeadData()->dwSubChunkSize;
    wav.nBlockAlign = waveFile.GetHeadData()->usBlockAlign;
    wav.nAvgBytesPerSec = waveFile.GetHeadData()->dwBytesPerSec;
    wav.cbSize = 0;

    DSBUFFERDESC bufferDesc;    // ��������������
    memset(&bufferDesc, 0, sizeof(DSBUFFERDESC));
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
    bufferDesc.dwBufferBytes = waveFile.GetHeadData()->dwDataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &wav;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    IDirectSoundBuffer* soundBuffer;   // �»������ĵ�ַ
    if (SUCCEEDED(ps->CreateSoundBuffer(&bufferDesc, &soundBuffer, NULL)))
    {
        printf("�����������ɹ���\n");
    }
    else
    {
        printf("����������ʧ�ܣ�\n");
        return 0;
    }

    // д������
    unsigned char* BufferPtr;
    DWORD BufferSize;
    soundBuffer->Lock(0, waveFile.GetHeadData()->dwDataSize, reinterpret_cast<LPVOID*> (&BufferPtr), &BufferSize, NULL, 0, 0);
    memcpy(BufferPtr, waveFile.GetDataBuffer(), waveFile.GetHeadData()->dwDataSize);
    soundBuffer->Unlock((LPVOID*)BufferPtr, BufferSize, NULL, 0);

    // ��ʼ����
    soundBuffer->Play(0, 0, DSBPLAY_LOOPING); 
    soundBuffer->SetVolume(0);  // ������������Χ����ߣ�0����ͣ�-10000

    DSBCAPS csp;
    memset(&csp, 0, sizeof(DSBCAPS));
    csp.dwSize = sizeof(DSBCAPS);
    soundBuffer->GetCaps(&csp);

    // �رչ��
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    while (1)
    {
        DWORD pos;
        soundBuffer->GetCurrentPosition(&pos, NULL);
        
        printf("%c", '\r');
        printf("%c", '[');
        for (size_t i = 0; i < (int)(pos / (csp.dwBufferBytes / 50)); i++)
        {
            printf("%c", '#');
        }
        for (size_t i = 0; i < 50 - (int)(pos / (csp.dwBufferBytes / 50)); i++)
        {
            printf("%c", ' ');
        }
        printf("%c", ']');

        printf("%u / %u���ֽڣ�", pos, csp.dwBufferBytes);
        Sleep(17);
    }
    
    return 0;
}
