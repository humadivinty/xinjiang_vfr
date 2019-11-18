#include "libvehrecdll.h"
#include "utilityTool/tool_function.h"
#include "cameraModule/Camera6467_VFR.h"
#include "cameraModule/DeviceListManager.h"
#include"libHvDevice/HvDeviceCommDef.h"

char chLogPath[256] = {0};
#ifdef WINDOWS
#define WRITE_LOG(fmt, ...) Tool_WriteFormatLog("%s:: "fmt, __FUNCTION__, ##__VA_ARGS__);
#else
//#define WRITE_LOG(...) Tool_WriteFormatLog("%s:: ", __FUNCTION__, ##__VA_ARGS__);
#define WRITE_LOG(fmt,...) Tool_WriteFormatLog("%s:: " fmt, __FUNCTION__,##__VA_ARGS__);
#endif

#define BASIC_NUMBER (1000)
#define MAX_CAMERA_COUNT (10)

extern char g_chLogPath[256];

int VehRec_InitEx(char *iLogPath)
{
    Tool_SetLogPath(iLogPath);
    for (int i = 0; i < MAX_CAMERA_COUNT; i++)
    {
        BaseCamera* pCamera =DeviceListManager::GetInstance()->GetDeviceById(i);
        if (NULL != pCamera)
        {
            pCamera->SetLogPath(iLogPath);
        }
    }
    WRITE_LOG("iLogPath = %s", iLogPath);
    return 0;
}

int VehRec_Free()
{
    WRITE_LOG("begin");
    DeviceListManager::GetInstance()->ClearAllDevice();
    WRITE_LOG("finish.");
    return 0;
}

int VehRec_Connect(char *devIP, char *savepath)
{
    WRITE_LOG("begin, devIP = %s, savepath = %s", devIP, savepath);
    int iRet = -4;

    if (1 != Tool_checkIP(devIP))
    {
        WRITE_LOG("ip address is invlalid, return %d.", iRet);
        return iRet;
    }
//    if (0 != Tool_pingIpAddress(devIP))
//    {
//        WRITE_LOG("ip address is disconnect, return %d.", iRet);
//        return iRet;
//    }

    BaseCamera* pCamera = DeviceListManager::GetInstance()->GetDeviceByIpAddress(devIP);
    if (pCamera != NULL)
    {
        int iHandle = DeviceListManager::GetInstance()->GetDeviceIdByIpAddress(devIP);
        WRITE_LOG("find device %s, ID = %ld", devIP, iHandle);
        iRet = iHandle;
    }
    else
    {
        for (int i = 0; i < MAX_CAMERA_COUNT; i++)
        {
            if (NULL == DeviceListManager::GetInstance()->GetDeviceById(i))
            {
                Camera6467_VFR* pCamera = new Camera6467_VFR();
                pCamera->SetCameraIP(devIP);
                pCamera->SetLoginID(i);
                pCamera->SetSavePath(savepath, strlen(savepath));
                if(strlen(g_chLogPath) > 0)
                {
                    pCamera->SetLogPath(g_chLogPath);
                }
                if (pCamera->ConnectToCamera() == 0)
                {
                    WRITE_LOG("connect to camera success.");
                    pCamera->SetH264Callback(0,0,0, H264_RECV_FLAG_REALTIME);
                }
                else
                {
                    WRITE_LOG("connect to camera failed.");
                }
                iRet = i;
                DeviceListManager::GetInstance()->AddOneDevice(i, pCamera);
                WRITE_LOG("create camera success, device id= %d, ip = %s", i, devIP);
                break;
            }
        }
    }
    iRet = iRet >= 0 ? iRet+BASIC_NUMBER : iRet;

    WRITE_LOG("finish, return %d", iRet);
    return iRet;
}

void VehRec_DisConnect(int handle)
{
    WRITE_LOG("begin, handle = %d", handle);
    if(handle > BASIC_NUMBER)
    {
        DeviceListManager::GetInstance()->EraseDevice(handle - BASIC_NUMBER);
    }
    WRITE_LOG("finish");
}

int VehRec_SetSignal(int handle, int sig)
{
    WRITE_LOG("begin, handle = %d, sig = %d", handle, sig);
    int iRet = 0;
    WRITE_LOG("finish, return %d", iRet);
    return iRet;
}

int VehRec_SetPlateDataFun(int handle, VehRec_PlateDataFunc *pCallBack)
{
    WRITE_LOG("begin, handle = %d, VehRec_PlateDataFunc = %p", handle, pCallBack);
    int iRet = -1;
    if(handle < BASIC_NUMBER)
    {
        WRITE_LOG("handle value is invalid.");
        return iRet;
    }

    Camera6467_VFR* pCamera = (Camera6467_VFR*)DeviceListManager::GetInstance()->GetDeviceById(handle - BASIC_NUMBER);
    if (pCamera != NULL)
    {
        WRITE_LOG("find the camer and set callback.");
        pCamera->SetFrontImageCallback(pCallBack, NULL);
        iRet = 0;
    }
    else
    {
        WRITE_LOG("can not find camera by id %d", handle);
    }
    WRITE_LOG("finish, return %d", iRet);
    return iRet;
}

int VehRec_SetCarDataFun(int handle, VehRec_GetCarDataFun *pCallBack)
{
    WRITE_LOG("begin, handle = %d, VehRec_GetCarDataFun = %p", handle, pCallBack);
    int iRet = -1;
    if(handle < BASIC_NUMBER)
    {
        WRITE_LOG("handle value is invalid.");
        return iRet;
    }

    Camera6467_VFR* pCamera = (Camera6467_VFR*)DeviceListManager::GetInstance()->GetDeviceById(handle - BASIC_NUMBER);
    if (pCamera != NULL)
    {
        WRITE_LOG("find the camer and set callback.");
        pCamera->SetCompleteResultCallback(pCallBack, NULL);
        iRet = 0;
    }
    else
    {
        WRITE_LOG("can not find camera by id %d", handle);
    }
    WRITE_LOG("finish, return %d", iRet);
    return iRet;
}

int VehRec_CheckStatus (int handle,char* chDevStatus)
{
    WRITE_LOG("begin, handle = %d, chDevStatus = %p", handle, chDevStatus);
    int iRet = -1;
    if(handle < BASIC_NUMBER)
    {
        WRITE_LOG("handle value is invalid.");
        return iRet;
    }

    Camera6467_VFR* pCamera = (Camera6467_VFR*)DeviceListManager::GetInstance()->GetDeviceById(handle - BASIC_NUMBER);
    if (pCamera != NULL)
    {
        WRITE_LOG("find the camer and set callback.");
        if(0 ==pCamera->GetCamStatus())
        {
            strcpy(chDevStatus, "连接正常");
        }
        else
        {
            strcpy(chDevStatus, "连接断开");
        }
        iRet = 0;
    }
    else
    {
        WRITE_LOG("can not find camera by id %d", handle);
    }
    WRITE_LOG("finish, return %d", iRet);
    return iRet;
}
