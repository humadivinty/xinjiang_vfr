#ifndef CAMERA6467_H
#define CAMERA6467_H
#include "CameraResult.h"
#include <string>
#include <list>
#include <pthread.h>
//#include <comdef.h>
//#include <gdiplus.h>
//using namespace Gdiplus;
//#pragma  comment(lib, "gdiplus.lib")
#include "utilityTool/tool_function.h"
//#include "commondef.h"
#include"libAVI/MyH264Saver.h"

#define CMD_DEL_VEH_HEAD 1
#define CMD_GET_VEH_LENGTH 2
#define  CMD_DEL_ALL_VEH 3
#define CMD_GET_VHE_HEAD 4


void* Camera_StatusCheckThread(void* lpParam);

struct CameraInfo
{
    char chIP[20];                          //deviceIP
    char chDeviceID[3];
    char chStationID[7];
    char chLaneID[3];
    bool bLogEnable;
    bool bSynTimeEnable;
    bool bIsInit;
    int iDirection;
    CameraInfo()
    {
        memset(chIP, 0, 20);
        memset(chDeviceID, 0, 3);
        memset(chStationID, 0, 7);
        memset(chLaneID, 0, 3);
        bLogEnable = true;
        bSynTimeEnable = false;
        bIsInit = false;
        iDirection = 0;
    }
};

struct SaveModeInfo
{
    int iSafeModeEnable;
    char chBeginTime[260];
    char chEndTime[260];
    int iIndex;
    int iDataType;
    SaveModeInfo()
    {
        iSafeModeEnable = 0;
        iIndex = 0;
        iDataType = 0;
        memset(chBeginTime, 0, 260);
        memset(chEndTime, 0, 260);
    }
};

struct DeviceTime
{
    int iYear;
    int iMonth;
    int iDay;
    int iHour;
    int iMinutes;
    int iSecond;
    int iMilisecond;

    DeviceTime()
    {
        iYear = 0;
        iMonth = 0;
        iDay = 0;
        iHour = 0;
        iMinutes = 0;
        iSecond = 0;
        iMilisecond = 0;
    }
};

typedef struct _BasicInfo
{
    char szIP[64];              //�豸IP�������������豸����
    char szMask[64];            //�豸�������룬�����������豸����
    char szGateway[64];         //�豸���أ������������豸����
    char szMac[128];            //�豸�����ַ�������������豸����
    char szModelVersion[128]; //�豸ģ�Ͱ汾�����ֶα���
    char szSN[128];             //�豸��ţ������������豸����
    char szWorkMode[128];       //�豸����ģʽ����������PCC200��PCC600��PCC200A
    char szDevType[128];        //�豸���ͣ���������PCC200��PCC600��PCC200A
    char szDevVersion[128];  //�豸�汾����������PCC200��PCC600��PCC200A
    char szMode[128];           //�豸����ģʽ��������ģʽ�������������豸����
    char szRemark[128];    //�����ֶΣ���������PCC200��PCC600
    char szBackupVersion[128];  //���ݰ汾����������PCC200��PCC600 
    char szFPGAVersion[128]; //FPGA�汾����������PCC200��PCC600
    char szKernelVersion[128];  //Kernel�汾����������PCC200��PCC600
    char szUbootVersion[128]; //Uboot�汾����������PCC200��PCC600
    char szUBLVersion[128];//UBL�汾����������PCC200��PCC600
}BasicInfo;

class BaseCamera
{
public:
    BaseCamera();
    //Camera6467(const char* chIP, HWND*  hWnd,int Msg);
    virtual ~BaseCamera() = 0;

    //void SetList(Result_lister* ResultList);
    bool SetCameraInfo(CameraInfo& camInfo);
    void SetLoginID(int iID);

    int GetCamStatus();
    //int GetNetSatus();
    int GetLoginID();
    char* GetStationID();
    char* GetDeviceID();
    char*  GetLaneID();
    const char* GetCameraIP();
    void SetCameraIP(const char* ipAddress);
    void SetCameraIndex(int iIndex);
    bool GetDeviceTime(DeviceTime& deviceTime);

    int ConnectToCamera();
    void	InterruptionConnection();
    bool DisConnectCamera();

    virtual void ReadConfig();

    void SetLogPath(const char* path);
    void WriteFormatLog(const char* szfmt, ...);
    bool WriteLog(const char* chlog);

    bool TakeCapture();
    bool SynTime();
    bool SynTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int MilientSecond);
    bool SaveImgToDisk(char* chImgPath, unsigned char* pImgData, unsigned long dwImgSize);
    //bool SaveImgToDisk(char* chImgPath, unsigned char* pImgData, unsigned long dwImgSize, int iWidth, int iHeight, int iType = 0);

    void SaveResultToBufferPath(CameraResult* pResult);

    //���豸�����ض����ֻ���к�С���˶Խ���Ч
    bool SenMessageToCamera(int iMessageType, int& iReturnValue, int& iErrorCode, int iArg = 0);

    //�����豸�ĵ����ַ���С����ɫ
    bool SetOverlayVedioFont(int iFontSize, int iColor);
    bool SetOverlayVideoText(int streamId, char* overlayText, int textLength);
    bool SetOverlayVideoTextPos(int streamId, int posX, int posY);
    bool SetOverlayVideoTextEnable(int streamId, bool enable);
    bool SetOverlayTimeEnable(int streamID, bool enable);
    bool SetOverlayTimeFormat(int streamId, int iformat);
    bool SetOverlayTimePos(int streamId, int posX, int posY);

    //��ȡ�豸��Ӳ���汾��Ϣ
    bool GetHardWareInfo(BasicInfo& info);

    void setSaveBufferPath(const char* path, size_t bufLength);

    //��Ƶ����
    bool SetH264Callback(int iStreamID, unsigned long long  dwBeginTime, unsigned long long  dwEndTime, unsigned long  RecvFlag);
    bool SetH264CallbackNULL(int iStreamID, unsigned long  RecvFlag);

    bool StartToSaveAviFile(int iStreamID, const char* fileName, long long  beginTimeTick = 0);
    bool StopSaveAviFile(int iStreamID, long long TimeFlag = 0);

    void setVideoAdvanceTime(int iTime);
    int getVideoAdvanceTime();

    void setVideoDelayTime(int iTime);
    int getVideoDelayTime();

    void SetSavePath(const char* path, size_t bufLen);

    bool ConverTimeTickToString(unsigned long long timeTick, char* buff, size_t bufLen);
protected:
    void* m_hHvHandle;
    void* m_hWnd;
    //HWND m_hWnd;
    int m_iMsg;
    int m_iConnectMsg;
    int m_iDisConMsg;
    int m_iConnectStatus;
    int m_iLoginID;
    int m_iCompressQuality;
    int m_iDirection;
    int m_iIndex;

    int m_video;
    int m_iVideoAdvanceTime;
    int m_iVideoDelayTime;
    bool m_bVideoLogEnable;

    long long m_curH264Ms;

    bool m_bLogEnable;
    bool m_bSynTime;
    bool m_bThreadCreateSussess;

    char m_chDeviceID[3];
    char m_chStationID[7];
    char m_chLaneID[3];
    char m_chBufferPath[256];
    char m_chLogPath[256];
    char m_chSavePath[256];

    std::string m_strIP;
    std::string m_strDeviceID;

    SaveModeInfo m_SaveModelInfo;

    //CRITICAL_SECTION m_csLog;
    ThreadMutex m_csLog;

    ThreadMutex m_csFuncCallback;
    pthread_t m_threadCheckStatus;

    MyH264Saver m_h264Saver;
protected:

    void ReadHistoryInfo();
    void WriteHistoryInfo(SaveModeInfo& SaveInfo);


    inline void CopyDataToIMG(CameraIMG& DestImg,
        unsigned char* pImgData,
        unsigned long width,
        unsigned long height, 
        unsigned long dataLenth, 
        unsigned short wImgType)
    {
        if (NULL != DestImg.pbImgData)
        {
            delete[] DestImg.pbImgData;
            DestImg.pbImgData = NULL;
        }

        DestImg.pbImgData = new(std::nothrow) unsigned char[dataLenth];
        DestImg.dwImgSize = 0;
        if (NULL != DestImg.pbImgData)
        {
            memcpy(DestImg.pbImgData, pImgData, dataLenth);
            DestImg.wImgWidth = width;
            DestImg.wImgHeight = height;
            DestImg.dwImgSize = dataLenth;
            DestImg.wImgType = wImgType;
        }
    }

public:
    static int  RecordInfoBeginCallBack(void* pUserData, unsigned long dwCarID)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->RecordInfoBegin(dwCarID);
    };
    virtual int RecordInfoBegin(unsigned long dwCarID) = 0;

    static int  RecordInfoEndCallBack(void* pUserData, unsigned long dwCarID)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->RecordInfoEnd(dwCarID);
    };
    virtual int RecordInfoEnd(unsigned long dwCarID) = 0;

    static int  RecordInfoPlateCallBack(void* pUserData,
        unsigned long dwCarID,
        const char* pcPlateNo,
        const char* pcAppendInfo,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->RecordInfoPlate(dwCarID, pcPlateNo, pcAppendInfo, dwRecordType, dw64TimeMS);
    }
    virtual int RecordInfoPlate(unsigned long dwCarID,
        const char* pcPlateNo,
        const char* pcAppendInfo,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS) = 0;

    static int  RecordInfoBigImageCallBack(void* pUserData,
        unsigned long dwCarID,
        unsigned short  wImgType,
        unsigned short  wWidth,
        unsigned short  wHeight,
        unsigned char* pbPicData,
        unsigned long dwImgDataLen,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->RecordInfoBigImage(dwCarID, wImgType, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
    }
    virtual int RecordInfoBigImage(unsigned long dwCarID,
        unsigned short  wImgType,
        unsigned short  wWidth,
        unsigned short  wHeight,
        unsigned char* pbPicData,
        unsigned long dwImgDataLen,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS) = 0;

    static int  RecordInfoSmallImageCallBack(void* pUserData,
        unsigned long dwCarID,
        unsigned short wWidth,
        unsigned short wHeight,
        unsigned char* pbPicData,
        unsigned long dwImgDataLen,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->RecordInfoSmallImage(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
    }
    virtual int RecordInfoSmallImage(unsigned long dwCarID,
        unsigned short wWidth,
        unsigned short wHeight,
        unsigned char* pbPicData,
        unsigned long dwImgDataLen,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS) = 0;

    static int  RecordInfoBinaryImageCallBack(void* pUserData,
        unsigned long dwCarID,
        unsigned short wWidth,
        unsigned short wHeight,
        unsigned char* pbPicData,
        unsigned long dwImgDataLen,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->RecordInfoBinaryImage(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
    }
    virtual int RecordInfoBinaryImage(unsigned long dwCarID,
        unsigned short wWidth,
        unsigned short wHeight,
        unsigned char* pbPicData,
        unsigned long dwImgDataLen,
        unsigned long dwRecordType,
        unsigned long long dw64TimeMS) = 0;

    static int  JPEGStreamCallBack(
        void* pUserData,
        unsigned char* pbImageData,
        unsigned long dwImageDataLen,
        unsigned long dwImageType,
        const char* szImageExtInfo
        )
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->DeviceJPEGStream(pbImageData, dwImageDataLen, dwImageType, szImageExtInfo);

    };
    virtual int DeviceJPEGStream(
        unsigned char* pbImageData,
        unsigned long dwImageDataLen,
        unsigned long dwImageType,
        const char* szImageExtInfo) = 0;

    static int HVAPI_CALLBACK_H264_EX(
        void* pUserData,
        unsigned long  dwVedioFlag,
        unsigned long  dwVideoType,
        unsigned long  dwWidth,
        unsigned long  dwHeight,
        unsigned long long  dw64TimeMS,
        unsigned char* pbVideoData,
        unsigned long  dwVideoDataLen,
        const char* szVideoExtInfo
        )
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->handleH264Frame(
            dwVedioFlag,
            dwVideoType,
            dwWidth,
            dwHeight,
            dw64TimeMS,
            pbVideoData,
            dwVideoDataLen,
            szVideoExtInfo);
    };

    int handleH264Frame(
        unsigned long  dwVedioFlag,
        unsigned long  dwVideoType,
        unsigned long  dwWidth,
        unsigned long  dwHeight,
        unsigned long long  dw64TimeMS,
        unsigned char* pbVideoData,
        unsigned long  dwVideoDataLen,
        const char* szVideoExtInfo
        );

    static int HVAPI_CALLBACK_H264_linux( void* pUserData,
                                          unsigned char* pbVideoData,
                                          unsigned long dwVIdeoDataLen,
                                          unsigned long dwVIdeoType,
                                          const char* szVideoExtInfo)
    {
        if (pUserData == NULL)
            return 0;

        BaseCamera* pThis = (BaseCamera*)pUserData;
        return pThis->handleH264_linux( pbVideoData,
                                         dwVIdeoDataLen,
                                         dwVIdeoType,
                                         szVideoExtInfo);
    }

    int handleH264_linux(unsigned char* pbVideoData,
                         unsigned long dwVIdeoDataLen,
                         unsigned long dwVIdeoType,
                         const char* szVideoExtInfo);

    virtual void CheckStatus();
};

#endif
