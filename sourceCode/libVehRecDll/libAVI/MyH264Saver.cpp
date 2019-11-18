//#include "stdafx.h"
#include "MyH264Saver.h"
#include "utilityTool/tool_function.h"
#include<string.h>
//#include <QDebug>
//#include <time.h>       //time
#include<sys/time.h>
#include<stdarg.h>

#define LIST_SIZE 512

void*  H264DataProceesor(void* lpThreadParameter)
{
    if (NULL == lpThreadParameter)
    {
        return 0;
    }
    MyH264Saver* pSaver = (MyH264Saver*)lpThreadParameter;
    pSaver->processH264Data();
}

MyH264Saver::MyH264Saver():
m_bExit(false),
m_iSaveH264Flag(0),
m_iTimeFlag(0),
M_iStopTimeFlag(TIME_FLAG_UNDEFINE),
m_iTmpTime(0),
m_lastvideoidx(-1),
m_bFirstSave(false),
//m_pLocker(NULL),
m_hThreadSaveH264(NULL)
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
#ifdef WINDOWS
	EnterCriticalSection(&m_DataListLocker);
#else
    m_DataListLocker.lock();
#endif
    if (m_lDataStructList.size() > LIST_SIZE)
    {
        m_lDataStructList.pop_front();
    }
    m_lDataStructList.push_back(pData);
#ifdef WINDOWS
	LeaveCriticalSection(&m_DataListLocker);
#else
    m_DataListLocker.unlock();
#endif
    return true;
}

bool MyH264Saver::StartSaveH264(long long beginTimeStamp, const char* pchFilePath)
{
    //qDebug()<< "StartSaveH264 :"<< "beginTimeStamp "<< beginTimeStamp;
    SetSaveFlag(SAVING_FLAG_SAVING);
    SetStopTimeFlag(TIME_FLAG_UNDEFINE);
    SetIfFirstSave(true);
    SetTimeFlag(beginTimeStamp);
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
//void*  MyH264Saver::H264DataProceesor(void* lpThreadParameter)
//{
//    if (NULL == lpThreadParameter)
//    {
//        return 0;
//    }
//    MyH264Saver* pSaver = (MyH264Saver*)lpThreadParameter;
//    pSaver->processH264Data();
//}
#endif

unsigned long MyH264Saver::processH264Data_0()
{
    WriteFormatLog("MyH264Saver::processH264Data begin.\n");
    int  iSaveFlag = 0;
    int iVideoWidth = -1;
    int iVideoHeight = -1;

    int iFlag = 1;

    long long iCurrentFrameTimeFlag = -1;
    long long iLastTimeFlag = -1;
    long long iVideoStopTimeFlag = -1;

    long long iLastSaveFlag = -1;
    long long iVideoBeginTimeFlag = -1;
    long long iTimeNowFlag = -1;

    int iLastIndex = -1;
     int iCurrentFrameIndex = -1;
    while (!GetIfExit())
    {
        //Sleep(50);
#ifdef WINDOWS
		EnterCriticalSection(&m_DataListLocker);
#else
        m_DataListLocker.lock();
#endif
        if (m_lDataStructList.size() <= 0)
        {
#ifdef WINDOWS
        LeaveCriticalSection(&m_DataListLocker);
#else
        m_DataListLocker.unlock();
#endif
            //printf("MyH264Saver::processH264Data m_lDataStructList.size() <= 0 \n");
            usleep (10*1000);
            continue;
        }
#ifdef WINDOWS
		LeaveCriticalSection(&m_DataListLocker);
#else
        m_DataListLocker.unlock();
#endif
        char buf[256] = { 0 };
        iVideoStopTimeFlag = GetStopTimeFlag();
        iVideoBeginTimeFlag = GetTimeFlag();
        iTimeNowFlag = Tool_GetTickCount();

//        if( iStopTimeFlag != TIME_FLAG_UNDEFINE
//                &&  iTimeNowFlag > iStopTimeFlag )
//        {
//            //qDebug()<< "GetStopTimeFlag "  <<GetStopTimeFlag() << "< "<< "GetTickCount" <<GetTickCount();
//            iSaveFlag = SAVING_FLAG_SHUT_DOWN;
//            SetStopTimeFlag(TIME_FLAG_UNDEFINE);
//        }
//        else
        {
            iSaveFlag = GetSaveFlag();
        }

        if(iSaveFlag != iLastSaveFlag)
        {
            WriteFormatLog("stop time flag change, lastStopTimeFlag = %lld, current Stop time flag = %lld, system time Now = %lld,  video beginTIme = %lld, save flag = %d\n",
                           iLastSaveFlag,
                           iVideoStopTimeFlag,
                           iTimeNowFlag,
                           iVideoBeginTimeFlag,
                           iSaveFlag );
            iLastSaveFlag = iSaveFlag;
        }

        std::shared_ptr<CustH264Struct > pData = nullptr;
        switch (iSaveFlag)
        {
        case SAVING_FLAG_NOT_SAVE:
            usleep (10*1000);
            //printf("MyH264Saver::processH264Data SAVING_FLAG_NOT_SAVE, break \n");
            break;
        case SAVING_FLAG_SAVING:
            //printf("MyH264Saver::processH264Data SAVING_FLAG_SAVING \n",GetSavePath() );
#ifdef WINDOWS
            EnterCriticalSection(&m_DataListLocker);
#else
            m_DataListLocker.lock();
#endif

            pData = m_lDataStructList.front();
            m_lDataStructList.pop_front();

#ifdef WINDOWS
            LeaveCriticalSection(&m_DataListLocker);
#else
            m_DataListLocker.unlock();
#endif

            iCurrentFrameTimeFlag = pData->m_llFrameTime;
            iCurrentFrameIndex = pData->index;
            //iStopTimeFlag = GetTimeFlag();
            if (iCurrentFrameTimeFlag < iVideoBeginTimeFlag)
            {
                WriteFormatLog("iCurrentTimeFlag (%lld) < iVideoBeginTimeFlag (%lld), erase frame, index = %d",
                               iCurrentFrameTimeFlag,
                               iVideoBeginTimeFlag,
                               iCurrentFrameIndex);
                pData = nullptr;
                continue;
            }
            else
            {
                if( iVideoStopTimeFlag != TIME_FLAG_UNDEFINE
                        && iCurrentFrameTimeFlag >= iVideoStopTimeFlag)
                {
                    WriteFormatLog(" iStopTimeFlag != TIME_FLAG_UNDEFINE  && iCurrentTimeFlag (%lld) >= iStopTimeFlag (%lld), change saveFlag ",
                                   iCurrentFrameTimeFlag,
                                   iVideoStopTimeFlag);
                    SetSaveFlag(SAVING_FLAG_SHUT_DOWN);

                    iLastTimeFlag = -1;
                    iLastIndex = -1;
                }
                else
                {
                    if(iLastTimeFlag == -1
                            || iLastIndex == -1
                            || ( ( iCurrentFrameTimeFlag > iLastTimeFlag) && ( iCurrentFrameIndex == iLastIndex + 1  )   )
                            || ( ( iCurrentFrameTimeFlag > iLastTimeFlag) && ( iCurrentFrameIndex == 0  ) && iLastIndex == VIDEO_FRAME_LIST_SIZE  )
                            )
                    {
//                        WriteFormatLog("iCurrentTimeFlag (%lld) > iVideoBeginTimeFlag (%lld),and < video stop time(%lld) save frame, index = %d",
//                                       iCurrentFrameTimeFlag,
//                                       iVideoBeginTimeFlag,
//                                       iVideoStopTimeFlag,
//                                       iCurrentFrameIndex);

                        iLastTimeFlag = iCurrentFrameTimeFlag;
                        iLastIndex = iCurrentFrameIndex;
                    }
                    else
                    {
//                        WriteFormatLog("iCurrentTimeFlag (%lld) > iVideoBeginTimeFlag (%lld),and < video stop time(%lld) but nosave frame, index = %d",
//                                       iCurrentFrameTimeFlag,
//                                       iVideoBeginTimeFlag,
//                                       iVideoStopTimeFlag,
//                                       iCurrentFrameIndex);
                        pData = nullptr;
                    }
                }
            }

//#ifdef WINDOWS
//            LeaveCriticalSection(&m_DataListLocker);
//#else
//            m_DataListLocker.unlock();
//#endif

            //pData = m_lDataStructList.front();

            if (pData == nullptr)
            {
                //printf("MyH264Saver::processH264Data SAVING_FLAG_SAVING  pData == nullptr\n" );
                break;
            }

            iVideoWidth = pData->m_iWidth;
            iVideoHeight = pData->m_iHeight;
            if (GetIfFirstSave())
            {
                if (!m_264AviLib.IsNULL())
                {
                    WriteFormatLog("m_264AviLib.IsNULL == false, close file.\n");
                    m_264AviLib.close();
                }
                if (iVideoWidth > 0
                    && iVideoHeight > 0)
                {
                    WriteFormatLog("m_264AviLib.setAviInfo , %s.\n",GetSavePath() );
                    m_264AviLib.setAviInfo((char*)GetSavePath(), iVideoWidth, iVideoHeight, 25, "H264");
                    SetIfFirstSave(false);
                }
            }

            if (!m_264AviLib.IsNULL()
                && pData->m_llFrameTime > GetTimeFlag()
               )
            {
                iFlag =   m_264AviLib.writeFrame((char*)pData->m_pbH264FrameData, pData->m_iDataSize, pData->m_isIFrame);
                if(iFlag != 0)
                {
                    WriteFormatLog(" m_264AviLib.writeFrame = %d\n", iFlag);
                }
                //SetIfFirstSave(false);
            }
            pData = nullptr;
            break;
        case SAVING_FLAG_SHUT_DOWN:
            if (!m_264AviLib.IsNULL())
            {
				m_iTmpTime = 0;
                m_264AviLib.close();
                //SetSaveFlag(SAVING_FLAG_NOT_SAVE);
                WriteFormatLog("m_264AviLib.close SAVING_FLAG_SHUT_DOWN.\n",GetSavePath() );
            }
            else
            {
                WriteFormatLog("MyH264Saver::processH264Data SAVING_FLAG_SHUT_DOWN m_264AviLib.IsNULL.\n",GetSavePath() );
            }
            SetSaveFlag(SAVING_FLAG_NOT_SAVE);
            break;
        default:
            printf("MyH264Saver::processH264Data default break.\n",GetSavePath() );
            break;
        }       

    }

     WriteFormatLog("MyH264Saver::processH264Data finish.\n");
     return 0;
}

unsigned long MyH264Saver::processH264Data()
{
    WriteFormatLog("MyH264Saver::processH264Data begin.\n");
      int  iSaveFlag = 0;
      int iVideoWidth = -1;
      int iVideoHeight = -1;

      int iFlag = 1;

      long long iCurrentFrameTimeFlag = -1;
      long long iLastTimeFlag = -1;
      long long iVideoStopTimeFlag = -1;

      long long iLastSaveFlag = -1;
      long long iVideoBeginTimeFlag = -1;
      long long iTimeNowFlag = -1;

      int iLastIndex = -1;
      int iCurrentFrameIndex = -1;
      while (!GetIfExit())
      {
          //Sleep(50);
  #ifdef WINDOWS
          EnterCriticalSection(&m_DataListLocker);
  #else
          m_DataListLocker.lock();
  #endif
          if (m_lDataStructList.size() <= 0)
          {
  #ifdef WINDOWS
              LeaveCriticalSection(&m_DataListLocker);
  #else
              m_DataListLocker.unlock();
  #endif
              //printf("MyH264Saver::processH264Data m_lDataStructList.size() <= 0 \n");
              usleep(10 * 1000);
              //Sleep(10);
              continue;
          }
  #ifdef WINDOWS
          LeaveCriticalSection(&m_DataListLocker);
  #else
          m_DataListLocker.unlock();
  #endif
          char buf[256] = { 0 };
          //iVideoStopTimeFlag = GetStopTimeFlag();
          iVideoBeginTimeFlag = GetTimeFlag();
          iTimeNowFlag = Tool_GetTickCount();

          //        if( iStopTimeFlag != TIME_FLAG_UNDEFINE
          //                &&  iTimeNowFlag > iStopTimeFlag )
          //        {
          //            //qDebug()<< "GetStopTimeFlag "  <<GetStopTimeFlag() << "< "<< "GetTickCount" <<GetTickCount();
          //            iSaveFlag = SAVING_FLAG_SHUT_DOWN;
          //            SetStopTimeFlag(TIME_FLAG_UNDEFINE);
          //        }
          //        else
          {
              iSaveFlag = GetSaveFlag();
          }

          if (iSaveFlag != iLastSaveFlag)
          {
              WriteFormatLog("stop time flag change, lastStopTimeFlag = %lld, current Stop time flag = %lld, system time Now = %lld,  video beginTIme = %lld, save flag = %d\n",
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
  #ifdef WINDOWS
              EnterCriticalSection(&m_DataListLocker);
  #else
              m_DataListLocker.lock();
  #endif

              //pData = m_lDataStructList.front();
              //m_lDataStructList.pop_front();

              for (auto r = m_lDataStructList.begin(); r != m_lDataStructList.end(); )
              {
                  pData = (*r);
                  iCurrentFrameTimeFlag = pData->m_llFrameTime;
                  iCurrentFrameIndex = pData->index;
                  //iStopTimeFlag = GetTimeFlag();
                  if (iCurrentFrameTimeFlag < iVideoBeginTimeFlag)
                  {
                      WriteFormatLog("iCurrentFrameTimeFlag (%lld) < iVideoBeginTimeFlag (%lld), erase frame, index = %d",
                          iCurrentFrameTimeFlag,
                          iVideoBeginTimeFlag,
                          iCurrentFrameIndex);
                      pData = nullptr;
                      r = m_lDataStructList.erase(r);
                      //r++;
                      continue;
                  }
                  else
                  {
                      iVideoStopTimeFlag = GetStopTimeFlag();
                      if (iVideoStopTimeFlag != TIME_FLAG_UNDEFINE
                          && iCurrentFrameTimeFlag >= iVideoStopTimeFlag)
                      {
                          WriteFormatLog(" iStopTimeFlag != TIME_FLAG_UNDEFINE  && iCurrentTimeFlag (%lld) >= iStopTimeFlag (%lld), change saveFlag ",
                              iCurrentFrameTimeFlag,
                              iVideoStopTimeFlag);
                          SetSaveFlag(SAVING_FLAG_SHUT_DOWN);

                          iLastTimeFlag = -1;
                          iLastIndex = -1;

                          break;
                      }
                      else
                      {
                          if (iLastTimeFlag == -1
                              || iLastIndex == -1
                              || ((iCurrentFrameTimeFlag > iLastTimeFlag) && (iCurrentFrameIndex == iLastIndex + 1))
                              || ((iCurrentFrameTimeFlag > iLastTimeFlag) && (iCurrentFrameIndex == 0) && iLastIndex == VIDEO_FRAME_LIST_SIZE)
                              )
                          {
                              WriteFormatLog("iCurrentFrameTimeFlag (%lld) > iVideoBeginTimeFlag (%lld),and < video stop time(%lld) save frame, index = %d, iLastTimeFlag = %lld, iLastIndex = %d",
                                  iCurrentFrameTimeFlag,
                                  iVideoBeginTimeFlag,
                                  iVideoStopTimeFlag,
                                  iCurrentFrameIndex,
                                  iLastTimeFlag,
                                  iLastIndex);

                              iLastTimeFlag = iCurrentFrameTimeFlag;
                              iLastIndex = iCurrentFrameIndex;

                              break;
                          }
                          else
                          {
                              WriteFormatLog("iCurrentFrameTimeFlag (%lld) > iVideoBeginTimeFlag (%lld),and < video stop time(%lld) but nosave frame, index = %d, iLastTimeFlag = %lld, iLastIndex = %d",
                                  iCurrentFrameTimeFlag,
                                  iVideoBeginTimeFlag,
                                  iVideoStopTimeFlag,
                                  iCurrentFrameIndex,
                                  iLastTimeFlag,
                                  iLastIndex);
                              pData = nullptr;

                              r++;
                          }
                      }

                  }
              }

  #ifdef WINDOWS
              LeaveCriticalSection(&m_DataListLocker);
  #else
              m_DataListLocker.unlock();
  #endif

              if (pData == nullptr)
              {
                  //printf("MyH264Saver::processH264Data SAVING_FLAG_SAVING  pData == nullptr\n" );
                  break;
              }

              iVideoWidth = pData->m_iWidth;
              iVideoHeight = pData->m_iHeight;
              if (GetIfFirstSave())
              {
                  if (!m_264AviLib.IsNULL())
                  {
                      iLastIndex = -1;
                      WriteFormatLog("m_264AviLib.IsNULL == false, close file.\n");
                      m_264AviLib.close();
                  }
                  if (iVideoWidth > 0
                      && iVideoHeight > 0)
                  {
                      WriteFormatLog("m_264AviLib.setAviInfo , %s.\n", GetSavePath());
                      m_264AviLib.setAviInfo((char*)GetSavePath(), iVideoWidth, iVideoHeight, 25, "H264");
                      SetIfFirstSave(false);
                  }
              }

              if (!m_264AviLib.IsNULL()
                  && pData->m_llFrameTime > GetTimeFlag()
                  )
              {
                  iFlag = m_264AviLib.writeFrame((char*)pData->m_pbH264FrameData, pData->m_iDataSize, pData->m_isIFrame);
                  if (iFlag != 0)
                  {
                      WriteFormatLog(" m_264AviLib.writeFrame = %d\n", iFlag);
                  }
                  //SetIfFirstSave(false);
              }
              pData = nullptr;
              break;
          case SAVING_FLAG_SHUT_DOWN:
              if (!m_264AviLib.IsNULL())
              {
                  m_iTmpTime = 0;
                  m_264AviLib.close();
                  //SetSaveFlag(SAVING_FLAG_NOT_SAVE);
                  WriteFormatLog("m_264AviLib.close SAVING_FLAG_SHUT_DOWN.\n", GetSavePath());
              }
              else
              {
                  WriteFormatLog("MyH264Saver::processH264Data SAVING_FLAG_SHUT_DOWN m_264AviLib.IsNULL.\n", GetSavePath());
              }
              SetSaveFlag(SAVING_FLAG_NOT_SAVE);
              break;
          default:
              WriteFormatLog("MyH264Saver::processH264Data default break.\n", GetSavePath());
              break;
          }

      }

      WriteFormatLog("MyH264Saver::processH264Data finish.\n");
      return 0;
}

unsigned long MyH264Saver::processH264Data_old()
{
    WriteFormatLog("MyH264Saver::processH264Data begin.\n");
    int  iSaveFlag = 0;
    int iVideoWidth = -1;
    int iVideoHeight = -1;

    int iFlag = 1;

    long long iCurrentFrameTimeFlag = -1;
    long long iLastTimeFlag = -1;
    long long iVideoStopTimeFlag = -1;

    long long iLastSaveFlag = -1;
    long long iVideoBeginTimeFlag = -1;
    long long iTimeNowFlag = -1;

    int iLastIndex = -1;
     int iCurrentFrameIndex = -1;
    while (!GetIfExit())
    {
        //Sleep(50);
#ifdef WINDOWS
        EnterCriticalSection(&m_DataListLocker);
#else
        m_DataListLocker.lock();
#endif
        if (m_lDataStructList.size() <= 0)
        {
#ifdef WINDOWS
        LeaveCriticalSection(&m_DataListLocker);
#else
        m_DataListLocker.unlock();
#endif
            //printf("MyH264Saver::processH264Data m_lDataStructList.size() <= 0 \n");
            usleep (10*1000);
            continue;
        }
#ifdef WINDOWS
        LeaveCriticalSection(&m_DataListLocker);
#else
        m_DataListLocker.unlock();
#endif
        char buf[256] = { 0 };
        iVideoStopTimeFlag = GetStopTimeFlag();
        iVideoBeginTimeFlag = GetTimeFlag();
        iTimeNowFlag = Tool_GetTickCount();

//        if( iStopTimeFlag != TIME_FLAG_UNDEFINE
//                &&  iTimeNowFlag > iStopTimeFlag )
//        {
//            //qDebug()<< "GetStopTimeFlag "  <<GetStopTimeFlag() << "< "<< "GetTickCount" <<GetTickCount();
//            iSaveFlag = SAVING_FLAG_SHUT_DOWN;
//            SetStopTimeFlag(TIME_FLAG_UNDEFINE);
//        }
//        else
        {
            iSaveFlag = GetSaveFlag();
        }

        if(iSaveFlag != iLastSaveFlag)
        {
            WriteFormatLog("stop time flag change, lastStopTimeFlag = %lld, current Stop time flag = %lld, system time Now = %lld,  video beginTIme = %lld, save flag = %d\n",
                           iLastSaveFlag,
                           iVideoStopTimeFlag,
                           iTimeNowFlag,
                           iVideoBeginTimeFlag,
                           iSaveFlag );
            iLastSaveFlag = iSaveFlag;
        }

        std::shared_ptr<CustH264Struct > pData = nullptr;
        switch (iSaveFlag)
        {
        case SAVING_FLAG_NOT_SAVE:
            usleep (10*1000);
            //printf("MyH264Saver::processH264Data SAVING_FLAG_NOT_SAVE, break \n");
            break;
        case SAVING_FLAG_SAVING:
            //printf("MyH264Saver::processH264Data SAVING_FLAG_SAVING \n",GetSavePath() );
#ifdef WINDOWS
            EnterCriticalSection(&m_DataListLocker);
#else
            m_DataListLocker.lock();
#endif
            for (auto r = m_lDataStructList.begin(); r != m_lDataStructList.end(); )
            {
                iCurrentFrameTimeFlag = (*r)->m_llFrameTime;
                iCurrentFrameIndex = (*r)->index;
                //iStopTimeFlag = GetTimeFlag();
                if (iCurrentFrameTimeFlag < iVideoBeginTimeFlag)
                {
                    WriteFormatLog("iCurrentTimeFlag (%lld) < iVideoBeginTimeFlag (%lld), erase frame, index = %d",
                                   iCurrentFrameTimeFlag,
                                   iVideoBeginTimeFlag,
                                   iCurrentFrameIndex);
                    r = m_lDataStructList.erase(r);
                    //printf("(*r)->m_llFrameTime = %lld, GetTimeFlag() = %lld \n",(*r)->m_llFrameTime, GetTimeFlag());
                    //r++;
                }
                else
                {
//                    if ( (*r)->m_llFrameTime > m_iTmpTime
//                            || ( (*r)->m_llFrameTime == m_iTmpTime && ( (*r)->index == m_lastvideoidx + 1 || ((*r)->index == 0 && m_lastvideoidx == 400))))
//                    {
//                        pData = (*r);
//                        m_iTmpTime = (*r)->m_llFrameTime;
//                        m_lastvideoidx = (*r)->index;
//                        break;
//                    }
                    if( iVideoStopTimeFlag != TIME_FLAG_UNDEFINE
                            && iCurrentFrameTimeFlag >= iVideoStopTimeFlag)
                    {
                        WriteFormatLog(" iStopTimeFlag != TIME_FLAG_UNDEFINE  && iCurrentTimeFlag (%lld) >= iStopTimeFlag (%lld), change saveFlag ",
                                       iCurrentFrameTimeFlag,
                                       iVideoStopTimeFlag);
                        SetSaveFlag(SAVING_FLAG_SHUT_DOWN);

                        pData = (*r);
                        iLastTimeFlag = -1;
                        iLastIndex = -1;

                        break;
                    }

                    if(iLastTimeFlag == -1
                            || iLastIndex == -1
                            || ( ( iCurrentFrameTimeFlag > iLastTimeFlag) && ( iCurrentFrameIndex == iLastIndex + 1  )   )
                            || ( ( iCurrentFrameTimeFlag > iLastTimeFlag) && ( iCurrentFrameIndex == 0  ) && iLastIndex == 400  )
                            )
                    {
                        WriteFormatLog("iCurrentTimeFlag (%lld) > iVideoBeginTimeFlag (%lld),and < video stop time(%lld) save frame, index = %d",
                                       iCurrentFrameTimeFlag,
                                       iVideoBeginTimeFlag,
                                       iVideoStopTimeFlag,
                                       iCurrentFrameIndex);
                        pData = (*r);
                        iLastTimeFlag = iCurrentFrameTimeFlag;
                        iLastIndex = iCurrentFrameIndex;
                        break;
                    }
                    else
                    {
                        WriteFormatLog("iCurrentTimeFlag (%lld) > iVideoBeginTimeFlag (%lld),and < video stop time(%lld) but nosave frame, index = %d",
                                       iCurrentFrameTimeFlag,
                                       iVideoBeginTimeFlag,
                                       iVideoStopTimeFlag,
                                       iCurrentFrameIndex);
                    }
                    r++;
                }
            }

#ifdef WINDOWS
            LeaveCriticalSection(&m_DataListLocker);
#else
            m_DataListLocker.unlock();
#endif

            if (pData == nullptr)
            {
                //printf("MyH264Saver::processH264Data SAVING_FLAG_SAVING  pData == nullptr\n" );
                break;
            }

            iVideoWidth = pData->m_iWidth;
            iVideoHeight = pData->m_iHeight;
            if (GetIfFirstSave())
            {
                if (!m_264AviLib.IsNULL())
                {
                    WriteFormatLog("m_264AviLib.IsNULL == false, close file.\n");
                    m_264AviLib.close();
                }
                if (iVideoWidth > 0
                    && iVideoHeight > 0)
                {
                    WriteFormatLog("m_264AviLib.setAviInfo , %s.\n",GetSavePath() );
                    m_264AviLib.setAviInfo((char*)GetSavePath(), iVideoWidth, iVideoHeight, 25, "H264");
                    SetIfFirstSave(false);
                }
            }

            if (!m_264AviLib.IsNULL()
                && pData->m_llFrameTime > GetTimeFlag()
               )
            {
                iFlag =   m_264AviLib.writeFrame((char*)pData->m_pbH264FrameData, pData->m_iDataSize, pData->m_isIFrame);
                if(iFlag != 0)
                {
                    WriteFormatLog(" m_264AviLib.writeFrame = %d\n", iFlag);
                }
                //SetIfFirstSave(false);
            }
            pData = nullptr;
            break;
        case SAVING_FLAG_SHUT_DOWN:
            if (!m_264AviLib.IsNULL())
            {
                m_iTmpTime = 0;
                m_264AviLib.close();
                //SetSaveFlag(SAVING_FLAG_NOT_SAVE);
                WriteFormatLog("m_264AviLib.close SAVING_FLAG_SHUT_DOWN.\n");
            }
            else
            {
                WriteFormatLog("MyH264Saver::processH264Data SAVING_FLAG_SHUT_DOWN m_264AviLib.IsNULL.\n" );
            }
            SetSaveFlag(SAVING_FLAG_NOT_SAVE);
            break;
        default:
            WriteFormatLog("MyH264Saver::processH264Data default break.\n" );
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

void MyH264Saver::SetTimeFlag(long long iValue)
{
    WriteFormatLog(" MyH264Saver::SetTimeFlag %lld \n", iValue);
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

long long MyH264Saver::GetTimeFlag()
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
    WriteFormatLog(" MyH264Saver::SetStopTimeFlag %lld \n", iValue);
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
    //printf(" MyH264Saver::GetStopTimeFlag %lld \n", iValue);
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
    //return;

    static char g_szString[10240] = { 0 };
    memset(g_szString, 0, sizeof(g_szString));

    va_list arg_ptr;
    va_start(arg_ptr, szfmt);
    //vsnprintf_s(g_szPbString, sizeof(g_szPbString), szfmt, arg_ptr);
    //_vsnprintf(g_szPbString,  sizeof(g_szPbString),  szfmt, arg_ptr);
    vsnprintf(g_szString, sizeof(g_szString), szfmt, arg_ptr);
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
                g_szString);
        fclose(file);
        file = NULL;
    }
}
