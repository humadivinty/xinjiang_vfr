#ifndef MYH264SAVER_H
#define MYH264SAVER_H

#include "CusH264Struct.h"
//#include "CameraModule/ThreadSafeList.h"
#include "libAVI/cAviLib.h"
#include <memory>
#include <deque>
#include "utilityTool/tool_function.h"


#define SAVING_FLAG_NOT_SAVE 0
#define SAVING_FLAG_SAVING 1
#define SAVING_FLAG_SHUT_DOWN 2

#define TIME_FLAG_UNDEFINE -1

class MyH264Saver
{
public:
    MyH264Saver();
    ~MyH264Saver();

    bool addDataStruct(CustH264Struct* pDataStruct);
    bool StartSaveH264(long long  beginTimeStamp, const char* pchFilePath);
    bool StopSaveH264(long long  TimeFlag = 0);
#ifdef WINDOWS
    static DWORD WINAPI  H264DataProceesor( LPVOID lpThreadParameter);
#else
     // void*  H264DataProceesor( void* lpThreadParameter);
#endif
    unsigned long processH264Data();
    unsigned long processH264Data_old();

private:
    void SetIfExit(bool bValue);
    bool GetIfExit();

    void SetSaveFlag(int iValue);
    int GetSaveFlag();

    void SetTimeFlag(long long  iValue);
    long long  GetTimeFlag();

    void SetStopTimeFlag(long long  iValue);
    long long  GetStopTimeFlag();

    void SetIfFirstSave(bool bValue);
    bool GetIfFirstSave();

    void SetSavePath(const char* filePath, size_t bufLength);
    const char* GetSavePath();

    void WriteFormatLog(const char* szfmt, ...);

private:

    bool m_bExit;    
    bool m_bFirstSave;
    int m_iSaveH264Flag;        //   0--not save  ; 1--saving ; 2--shut down saving
    long long m_iTimeFlag;
    long long M_iStopTimeFlag;

    char m_chFilePath[256];

    long long m_iTmpTime;
	int m_lastvideoidx;

    //TemplateThreadSafeList<std::shared_ptr<CustH264Struct > > m_lDataStructList;
    std::deque<std::shared_ptr<CustH264Struct > > m_lDataStructList;
#ifdef WINDOWS
	CRITICAL_SECTION m_DataListLocker;

    CRITICAL_SECTION m_Locker;
    HANDLE m_hThreadSaveH264;
#else
    ThreadMutex m_DataListLocker;

    ThreadMutex m_Locker;
    pthread_t m_hThreadSaveH264;
    bool m_bThreadCreateSussess;
#endif

    CAviLib m_264AviLib;
};

#endif // MYH264SAVER_H
