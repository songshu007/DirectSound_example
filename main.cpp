#include "WavFile.h"
#include <dsound.h>
#include <winerror.h>

#pragma comment(lib, "dsound.lib")

LPGUID GuidList[256] = { 0 };

// 枚举声音设备回调函数
BOOL CALLBACK DSEnumCallback(
    LPGUID lpGuid,              // 设备GUID，为控制面板上的默认设备时为NULL
    LPCWSTR lpcstrDescription,   // 字符串，为该音频设备的描述
    LPCWSTR lpcstrModule,        // 字符串，该字符串指定与此设备对应的 DirectSound 驱动程序的模块名称
    LPVOID lpContext            // 用户自定义数据，由 DirectSoundEnumerate 函数第二个参数指定
)
{
    static int num = 0;
    if (lpGuid == NULL)
    {
        wchar_t buf[256] = { 0 };
        wsprintfW(buf, L"第 %d 个设备：%ls（默认设备）\n", num, lpcstrDescription);
        WriteConsoleW(lpContext, buf, wcslen(buf), 0, 0);
    }
    else
    {
        wchar_t buf[256] = { 0 };
        wsprintfW(buf, L"第 %d 个设备：%ls\n", num, lpcstrDescription);
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
    
    // 枚举所有声音设备
    if (SUCCEEDED(DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumCallback, GetStdHandle(STD_OUTPUT_HANDLE))))
    {
        printf("设备枚举完成！\n");
    }
    else
    {
        printf("设备枚举失败！\n");
        return 0;
    }

    LPGUID GUID;
    int a;
    printf("选择你要播放的设备...\n");
    scanf_s("%d", &a);
    GUID = GuidList[a];

    // 创建设备
    IDirectSound* ps;
    if (SUCCEEDED(DirectSoundCreate(GUID, &ps, NULL)))
    {
        printf("设备创建成功！\n");
    }
    else
    {
        printf("设备创建失败！\n");
        return 0;
    }

    // 设置声音协作度
    ps->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE);

    // 打开音频文件
    printf("将你要播放的文件拖入...\n");
    std::wstring text3;
    std::wcin.imbue(std::locale("chs"));
    std::wcin >> text3;

    WavFile waveFile;
    if (waveFile.LoadFromFile(text3.c_str()) != true)
    {
        printf("文件打开失败！");
        return 0;
    }

    // 创建声音缓冲区
    WAVEFORMATEX wav;
    memset(&wav, 0, sizeof(WAVEFORMATEX));
    wav.wFormatTag = WAVE_FORMAT_PCM;
    wav.nChannels = waveFile.GetHeadData()->usNumChannels;
    wav.nSamplesPerSec = waveFile.GetHeadData()->dwSampleRate;
    wav.wBitsPerSample = waveFile.GetHeadData()->dwSubChunkSize;
    wav.nBlockAlign = waveFile.GetHeadData()->usBlockAlign;
    wav.nAvgBytesPerSec = waveFile.GetHeadData()->dwBytesPerSec;
    wav.cbSize = 0;

    DSBUFFERDESC bufferDesc;    // 声音缓冲区描述
    memset(&bufferDesc, 0, sizeof(DSBUFFERDESC));
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
    bufferDesc.dwBufferBytes = waveFile.GetHeadData()->dwDataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &wav;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    IDirectSoundBuffer* soundBuffer;   // 新缓冲区的地址
    if (SUCCEEDED(ps->CreateSoundBuffer(&bufferDesc, &soundBuffer, NULL)))
    {
        printf("缓冲区创建成功！\n");
    }
    else
    {
        printf("缓冲区创建失败！\n");
        return 0;
    }

    // 写入数据
    unsigned char* BufferPtr;
    DWORD BufferSize;
    soundBuffer->Lock(0, waveFile.GetHeadData()->dwDataSize, reinterpret_cast<LPVOID*> (&BufferPtr), &BufferSize, NULL, 0, 0);
    memcpy(BufferPtr, waveFile.GetDataBuffer(), waveFile.GetHeadData()->dwDataSize);
    soundBuffer->Unlock((LPVOID*)BufferPtr, BufferSize, NULL, 0);

    // 开始播放
    soundBuffer->Play(0, 0, DSBPLAY_LOOPING); 
    soundBuffer->SetVolume(0);  // 设置音量，范围：最高：0，最低：-10000

    DSBCAPS csp;
    memset(&csp, 0, sizeof(DSBCAPS));
    csp.dwSize = sizeof(DSBCAPS);
    soundBuffer->GetCaps(&csp);

    // 关闭光标
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

        printf("%u / %u（字节）", pos, csp.dwBufferBytes);
        Sleep(17);
    }
    
    return 0;
}
