//#include "stdafx.h"
#include "MyH264Saver.h"
#include "utilityTool/tool_function.h"
#include<string.h>
//#include <QDebug>
//#include <time.h>       //time
#include<sys/time.h>
#include<stdarg.h>

#define LIST_SIZE 512

//void*  H264DataProceesor(void* lpThreadParameter)
//{
//    if (NULL == lpThreadParameter)
//    {
//        return 0;
//    }
//    MyH264Saver* pSaver = (MyH264Saver*)lpThreadParameter;
//    pSaver->processH264Data();
//}

MyH264Saver::MyH264Saver():
m_bExit(false),
m_iSaveH264Flag(0),
m_iTimeFlag(0),
M_iStopTimeFlag(TIME_FLAG_UNDEFINE),
m_iTmpTime(0),
m_lastvideoidx(-1),
m_bFirstSave(false),
  m_bLogEnable(false),
//m_pLocker(NULL),
m_hThreadSaveH264(NULL),
m_lDataStructList(VIDEO_FRAME_LIST_SIZE)
{
#ifdef WINDOWS
    InitializeCriticalSection(&m_Locker);
	InitializeCriticalSection(&m_DataListLocker);
    m_hThreadSaveH264 = CreateThread(NULL, 0, H264DataProceesor, this, 0, NULL);
#else
    int iRet_thrd = pthread_create(&m_hThreadSaveH264, NULL, &H264DataProceesor, (void *) this);
    m_bThreadCreateSussess = (iRet_thrd == 0) ? true :false;
    printf("libAvi pthread_create = %d \n", iRet_thrd);
#endif
}


MyH264Saver::~MyH264Saver()
{
    SetIfExit(true);
    SetSaveFlag(false);

#ifdef WINDOWS
    Tool_SafeCloseThread(m_hThreadSaveH264);

    DeleteCriticalSection(&m_Locker);
	DeleteCriticalSection(&m_DataListLocker);
#else
    if(m_bThreadCreateSussess)
    {
        pthread_join(m_hThreadSaveH264, NULL);
    }
#endif
}

bool MyH264Saver::addDataStruct(CustH264Struct* pDataStruct)
{
    if (pDataStruct == NULL)
    {
        return false;
    }
	//char buf[256] = { 0 };
	//sprintf_s(buf, "%ld\n", pDataStruct->m_llFrameTime);
	//OutputDebugString(buf);

    std::shared_ptr<CustH264Struct> pData = std::shared_ptr<CustH264Struct>(pDataStruct);
//#ifdef WINDOWS
//	EnterCriticalSection(&m_DataListLocker);
//#else
//    m_DataListLocker.lock();
//#endif
//    if (m_lDataStructList.size() > LIST_SIZE)
//    {
//        m_lDataStructList.pop_front();
//    }
//    m_lDataStructList.push_back(pData);
//#ifdef WINDOWS
//	LeaveCriticalSection(&m_DataListLocker);
//#else
//    m_DataListLocker.unlock();
//#endif

    m_lDataStructList.AddOneData(pData);
    return true;
}

bool MyH264Saver::StartSaveH264(long long beginTimeStamp, const char* pchFilePath)
{
    //qDebug()<< "StartSaveH264 :"<< "beginTimeStamp "<< beginTimeStamp;
    SetSaveFlag(SAVING_FLAG_SAVING);
    SetStopTimeFlag(TIME_FLAG_UNDEFINE);
    SetIfFirstSave(true);
    SetStartTimeFlag(beginTimeStamp);
    SetSavePath(pchFilePath, strlen(pchFilePath));
	m_lastvideoidx = -1;
    return true;
}

bool MyH264Saver::StopSaveH264(long long TimeFlag)
{
    //SetSaveFlag(SAVING_FLAG_SHUT_DOWN);
    SetStopTimeFlag(TimeFlag);
    return true;
}
#ifdef WINDOWS
unsigned long WINAPI MyH264Saver::H264DataProceesor(LPVOID lpThreadParameter)
{
    if (NULL == lpThreadParameter)
    {
        return 0;
    }
    MyH264Saver* pSaver = (MyH264Saver*)lpThreadParameter;
    return pSaver->processH264Data();
}
#else
void*  MyH264Saver::H264DataProceesor(void* lpThreadParameter)
{
    if (NULL == lpThreadParameter)
    {
        return 0;
    }
    MyH264Saver* pSaver = (MyH264Saver*)lpThreadParameter;
    //pSaver->processH264Data();
    pSaver->processH264Data_mp4();
}
#endif

unsigned long MyH264Saver::processH264Data_mp4()
{
    WriteFormatLog("MyH264Saver::processH264Data begin.\n");
       int  iSaveFlag = 0;
       int iVideoWidth = -1;
       int iVideoHeight = -1;

       int iFlag = 1;

       long long iCurrentFrameTimeFlag = -1;
       long long iLastFrameTimeFlag = -1;
       long long iVideoStopTimeFlag = -1;

       long long iLastSaveFlag = -1;
       long long iVideoBeginTimeFlag = -1;
       long long iTimeNowFlag = -1;

       int iLastFrameIndex = -1;
       int iCurrentFrameIndex = -1;

       int iDataListIndex = -1;
       int iLastNullIndex = 0;
       bool bFindEmptyData = false;
       //int iEmptyDataCount = 0;
       while (!GetIfExit())
       {

           char buf[256] = { 0 };
           //iVideoStopTimeFlag = GetStopTimeFlag();
           iVideoBeginTimeFlag = GetStartTimeFlag();
           iTimeNowFlag = Tool_GetTickCount();

           iSaveFlag = GetSaveFlag();


           if (iSaveFlag != iLastSaveFlag)
           {
               WriteFormatLog("stop time flag change, lastStopTimeFlag =  %lld , current Stop time flag =  %lld , system time Now =  %lld ,  video beginTIme =  %lld , save flag = %d\n",
                   iLastSaveFlag,
                   iVideoStopTimeFlag,
                   iTimeNowFlag,
                   iVideoBeginTimeFlag,
                   iSaveFlag);
               iLastSaveFlag = iSaveFlag;
           }

           std::shared_ptr<CustH264Struct > pData = nullptr;
           switch (iSaveFlag)
           {
           case SAVING_FLAG_NOT_SAVE:
               usleep(10 * 1000);
               //printf("MyH264Saver::processH264Data SAVING_FLAG_NOT_SAVE, break \n");
               //Sleep(10);
               break;
           case SAVING_FLAG_SAVING:
               //printf("MyH264Saver::processH264Data SAVING_FLAG_SAVING \n",GetSavePath() );

               if (-1 == iDataListIndex)
               {
                   //查找所需的index
                   if (bFindEmptyData)
                   {
                       iLastNullIndex = (iLastNullIndex == VIDEO_FRAME_LIST_SIZE - 1 )? 0 : iLastNullIndex;

                       WriteFormatLog("this is new search , iDataListIndex = %d  search index.\n", iDataListIndex);
                   }
                   else
                   {
                       iLastNullIndex = m_lDataStructList.GetOldestDataIndex();
                       WriteFormatLog("iDataListIndex = %d  search index.\n", iDataListIndex);
                   }

                   for (int i = 0; i < VIDEO_FRAME_LIST_SIZE; i++)
                   {
                       int iSearchIndex = (i + iLastNullIndex) % VIDEO_FRAME_LIST_SIZE;
                       pData = m_lDataStructList.GetOneDataByIndex(iSearchIndex);
                       if (pData == nullptr)
                       {
                           iLastNullIndex = iSearchIndex;
                           bFindEmptyData = true;
                           WriteFormatLog("search index = %d, pData == nullptr , finish search.\n", iSearchIndex);
                           break;
                       }
                       iCurrentFrameTimeFlag = pData->m_llFrameTime;
                       iCurrentFrameIndex = pData->index;
                       iVideoBeginTimeFlag = GetStartTimeFlag();
                       if (iCurrentFrameTimeFlag >= iVideoBeginTimeFlag)
                       {
                           iDataListIndex = iSearchIndex;
                           WriteFormatLog("search index = %d, iCurrentFrameIndex = %d, iCurrentFrameTimeFlag  %lld  >= iVideoBeginTimeFlag  %lld , set iDataListIndex = %d, break .\n",
                               iSearchIndex,
                               iCurrentFrameIndex,
                               iCurrentFrameTimeFlag,
                               iVideoBeginTimeFlag,
                               iDataListIndex);
                           iLastFrameIndex = iCurrentFrameIndex;
                           iLastFrameTimeFlag = iCurrentFrameTimeFlag;
                           break;
                       }

                       if (iVideoBeginTimeFlag - iCurrentFrameTimeFlag > 1000)
                       {
                           i = i + 25;
                           WriteFormatLog("NewSearch iVideoBeginTimeFlag (   %lld   )- iCurrentFrameTimeFlag(   %lld   ) > 1000 ms, next search index +25, continue .\n", iVideoBeginTimeFlag, iCurrentFrameTimeFlag);
                       }
                       else
                       {
                           i++;
                       }

                       WriteFormatLog("search index = %d, iCurrentFrameTimeFlag  %lld  < iVideoBeginTimeFlag  %lld , continue .\n", iSearchIndex, iCurrentFrameTimeFlag, iVideoBeginTimeFlag);
                       if ((VIDEO_FRAME_LIST_SIZE - 1) == i)
                       {
                           bFindEmptyData = false;
                       }
                   }

                   if(pData != nullptr
                           && pData->m_llFrameTime < GetStartTimeFlag()
                           )
                   {
                       WriteFormatLog("NewSearch search finish, but still can`t find the frame, set frame data to null. iCurrentFrameTimeFlag   %lld   < iVideoBeginTimeFlag   %lld    .\n",
                                      pData->m_llFrameTime,
                                      GetStartTimeFlag());
                       pData = nullptr;
                   }
               }
               else
               {
                   iDataListIndex = (iDataListIndex >= (VIDEO_FRAME_LIST_SIZE - 1)) ? 0 : (iDataListIndex + 1);
                   pData = m_lDataStructList.GetOneDataByIndex(iDataListIndex);
                   if (pData == nullptr)
                   {
                       //iEmptyDataCount++;
                       WriteFormatLog("iDataListIndex = %d, but pData == nullptr, continue.\n", iDataListIndex);

                       iDataListIndex = (iDataListIndex == 0)?  VIDEO_FRAME_LIST_SIZE - 1: (iDataListIndex -1);
                       usleep(200 * 1000);
                       break;
                   }

                   iCurrentFrameTimeFlag = pData->m_llFrameTime;
                   iCurrentFrameIndex = pData->index;
                   iVideoBeginTimeFlag = GetStartTimeFlag();
                   iVideoStopTimeFlag = GetStopTimeFlag();

                   if (TIME_FLAG_UNDEFINE != iVideoStopTimeFlag
                       && iCurrentFrameTimeFlag >= iVideoStopTimeFlag)
                   {
                       WriteFormatLog("search index = %d, iCurrentFrameIndex = %d, iLastFrameIndex = %d,  iCurrentFrameTimeFlag  %lld  >  iVideoStopTimeFlag  %lld , SetSaveFlag SAVING_FLAG_SHUT_DOWN .\n",
                           iDataListIndex,
                           iCurrentFrameIndex,
                           iLastFrameIndex,
                           iCurrentFrameTimeFlag,
                           iVideoStopTimeFlag);

                       SetSaveFlag(SAVING_FLAG_SHUT_DOWN);
                       iDataListIndex = -1;
                       iLastNullIndex = 0;
                       iLastFrameIndex = -1;
                   }
                   else
                   {
                       if (iCurrentFrameTimeFlag >= iVideoBeginTimeFlag)
                       {
                           if (iLastFrameIndex == -1
   //                            || TIME_FLAG_UNDEFINE == iVideoStopTimeFlag
   //                            || (iCurrentFrameIndex == (iLastFrameIndex + 1) && iCurrentFrameTimeFlag > iLastFrameTimeFlag)
                               || (iCurrentFrameIndex > iLastFrameIndex  && iCurrentFrameTimeFlag > iLastFrameTimeFlag)
                               || ((iCurrentFrameTimeFlag > iLastFrameTimeFlag) && (iCurrentFrameIndex == 0) && iLastFrameIndex == VIDEO_FRAME_LIST_SIZE)
                               )
                           {
                               WriteFormatLog("search index = %d, iCurrentFrameIndex = %d, iCurrentFrameTimeFlag  %lld  >  iVideoBeginTimeFlag  %lld  and < iVideoStopTimeFlag ( %lld ) ,  Save frame .\n",
                                   iDataListIndex,
                                   iCurrentFrameIndex,
                                   iCurrentFrameTimeFlag,
                                   iVideoBeginTimeFlag,
                                   iVideoStopTimeFlag);

                               iLastFrameTimeFlag = iCurrentFrameTimeFlag;
                               iLastFrameIndex = iCurrentFrameIndex;
                           }
                           else
                           {
                               WriteFormatLog("search index = %d, iCurrentFrameTimeFlag ( %lld ) > iVideoBeginTimeFlag ( %lld ) ,and < iVideoStopTimeFlag ( %lld ) but nosave frame, iCurrentFrameIndex = %d, iLastFrameTimeFlag =  %lld , iLastFrameIndex = %d, ",
                                   iDataListIndex,
                                   iCurrentFrameTimeFlag,
                                   iVideoBeginTimeFlag,
                                   iVideoStopTimeFlag,
                                   iCurrentFrameIndex,
                                   iLastFrameTimeFlag,
                                   iLastFrameIndex
                                   );
                               pData = nullptr;
                           }

                       }
                       else
                       {
                           WriteFormatLog("search index = %d, iCurrentFrameIndex = %d, iCurrentFrameTimeFlag  %lld  <  iVideoBeginTimeFlag  %lld  , no Save frame .\n",
                               iDataListIndex,
                               iCurrentFrameIndex,
                               iCurrentFrameTimeFlag,
                               iVideoBeginTimeFlag);
                           pData = nullptr;

                           WriteFormatLog("search index = %d, wait the next time ,do not go forward",iDataListIndex );
                           iDataListIndex = (iDataListIndex == 0)?  VIDEO_FRAME_LIST_SIZE - 1: (iDataListIndex -1);
                           usleep(200 * 1000);

                       }
                   }
               }

               if (pData == nullptr)
               {
                   WriteFormatLog("MyH264Saver::processH264Data SAVING_FLAG_SAVING  pData == nullptr\n");
                   break;
               }

               iVideoWidth = pData->m_iWidth;
               iVideoHeight = pData->m_iHeight;
               if (GetIfFirstSave())
               {
                   m_264Mp4Lib.FileClose();
                   if (iVideoWidth > 0
                       && iVideoHeight > 0)
                   {
                       WriteFormatLog("GetIfFirstSave m_264Mp4Lib create file %s.\n", GetSavePath());
                       if (0 != m_264Mp4Lib.FileCreate(GetSavePath(), iVideoWidth, iVideoHeight, 25))
                       {
                           WriteFormatLog("m_264Mp4Lib create file  %s failed.\n", GetSavePath());
                       }
                       SetIfFirstSave(false);
                   }
               }

               if ( pData->m_llFrameTime >= GetStartTimeFlag() )
               {
                   iFlag = m_264Mp4Lib.FileWrite(MEDIA_FRAME_VIDEO, pData->m_pbH264FrameData, pData->m_iDataSize);
                   if (iFlag != 0)
                   {
                       WriteFormatLog(" m_264Mp4Lib.FileWrite = %d\n", iFlag);
                   }
               }
               else
               {
                   if (pData->m_llFrameTime < GetStartTimeFlag())
                   {
                       WriteFormatLog(" SAVING_FLAG_SAVING but pData->m_llFrameTime ( %lld )< GetStartTimeFlag( %lld )\n", pData->m_llFrameTime, GetStartTimeFlag());
                   }
                   WriteFormatLog(" SAVING_FLAG_SAVING but some thing wrong.\n", iFlag);
               }
               pData = nullptr;
               break;
           case SAVING_FLAG_SHUT_DOWN:
               if (0 != m_264Mp4Lib.FileClose())
               {
                   WriteFormatLog("m_264Mp4Lib.FileClose failed, SAVING_FLAG_SHUT_DOWN.\n", GetSavePath());
               }
               else
               {
                   WriteFormatLog("m_264Mp4Lib.FileClose , SAVING_FLAG_SHUT_DOWN.\n", GetSavePath());
               }
               SetSaveFlag(SAVING_FLAG_NOT_SAVE);
                iDataListIndex = -1;
               break;
           default:
               WriteFormatLog("MyH264Saver::processH264Data default break.\n", GetSavePath());
               break;
           }

       }

       WriteFormatLog("MyH264Saver::processH264Data finish.\n");
       return 0;
}

void MyH264Saver::SetIfExit(bool bValue)
{
    WriteFormatLog(" MyH264Saver::SetIfExit %d \n", bValue);
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif

    m_bExit = bValue;

#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

bool MyH264Saver::GetIfExit()
{
    bool bValue = false;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    bValue = m_bExit;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
    return bValue;
}

void MyH264Saver::SetSaveFlag(int iValue)
{
    WriteFormatLog(" MyH264Saver::SetSaveFlag %d \n", iValue);
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    m_iSaveH264Flag = iValue;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

int MyH264Saver::GetSaveFlag()
{
    int iValue = false;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    iValue = m_iSaveH264Flag;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
    return iValue;
}

void MyH264Saver::SetStartTimeFlag(long long iValue)
{
    WriteFormatLog(" MyH264Saver::SetTimeFlag  %lld  \n", iValue);
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    m_iTimeFlag = iValue;
	m_iTmpTime = iValue;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

long long MyH264Saver::GetStartTimeFlag()
{
    long long iValue = false;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    iValue = m_iTimeFlag;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
    return iValue;
}

void MyH264Saver::SetStopTimeFlag(long long iValue)
{
    WriteFormatLog(" MyH264Saver::SetStopTimeFlag  %lld  \n", iValue);
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    M_iStopTimeFlag = iValue;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

long long MyH264Saver::GetStopTimeFlag()
{
    long long iValue = 0;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    iValue = M_iStopTimeFlag;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
    //printf(" MyH264Saver::GetStopTimeFlag  %lld  \n", iValue);
    return iValue;
}

void MyH264Saver::SetIfFirstSave(bool bValue)
{
    WriteFormatLog(" MyH264Saver::SetIfFirstSave %d \n", bValue);
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    m_bFirstSave = bValue;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

bool MyH264Saver::GetIfFirstSave()
{
    bool bValue = false;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    bValue = m_bFirstSave;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
    return bValue;
}

void MyH264Saver::SetSavePath(const char* filePath, size_t bufLength)
{
    WriteFormatLog(" MyH264Saver::SetSavePath %s \n", filePath);
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    if (bufLength < sizeof(m_chFilePath))
    {
        memcpy(m_chFilePath, filePath, bufLength);
        m_chFilePath[bufLength] = '\0';
    }
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

const char* MyH264Saver::GetSavePath()
{
    const char* pValue = NULL;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif
    pValue = m_chFilePath;
#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
            return pValue;
}

void MyH264Saver::WriteFormatLog(const char *szfmt, ...)
{
    if(!GetLogEnable())
        return;

    memset(m_chLogBuf, 0, sizeof(m_chLogBuf));

    va_list arg_ptr;
    va_start(arg_ptr, szfmt);
    //vsnprintf_s(g_szPbString, sizeof(g_szPbString), szfmt, arg_ptr);
    //_vsnprintf(g_szPbString,  sizeof(g_szPbString),  szfmt, arg_ptr);
    vsnprintf(m_chLogBuf, sizeof(m_chLogBuf), szfmt, arg_ptr);
    va_end(arg_ptr);

    char chBuffer1[256] = {0};
    getcwd(chBuffer1,sizeof(chBuffer1));

    struct tm stimeNow;
    long long iTimeInMilliseconds = 0 ;
    Tool_GetTime(&stimeNow,& iTimeInMilliseconds);

    strcat(chBuffer1, "/XLWLog/");
    Tool_MyMakeDir(chBuffer1);

    char chLogFileName[512] = { 0 };
    sprintf(chLogFileName,"%s/Video_%d-%02d_%02d.log",
            chBuffer1,
            stimeNow.tm_year+1900,
            stimeNow.tm_mon+1,
            stimeNow.tm_mday);

    FILE *file = NULL;
    file = fopen(chLogFileName, "a+");
    //fopen_s(&file, chLogFileName, "a+");
    if (file)
    {
        fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d:%03lld] : %s\n",
                stimeNow.tm_year + 1900,
                stimeNow.tm_mon + 1,
                stimeNow.tm_mday,
                stimeNow.tm_hour,
                stimeNow.tm_min,
                stimeNow.tm_sec,
                iTimeInMilliseconds%1000,
                m_chLogBuf);
        fclose(file);
        file = NULL;
    }
}

void MyH264Saver::SetLogEnable(bool bValue)
{
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif

    m_bLogEnable = bValue;

#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif
}

bool MyH264Saver::GetLogEnable()
{
    bool bValue = false;
#ifdef WINDOWS
            EnterCriticalSection(&m_Locker);
#else
            m_Locker.lock();
#endif

    bValue = m_bLogEnable;

#ifdef WINDOWS
            LeaveCriticalSection(&m_Locker);
#else
            m_Locker.unlock();
#endif

     return bValue;
}
