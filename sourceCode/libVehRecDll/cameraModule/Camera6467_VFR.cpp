//#include "stdafx.h"
#include "Camera6467_VFR.h"

#include "libHvDevice/HvDeviceBaseType.h"
#include "libHvDevice/HvDeviceCommDef.h"
//#include "libHvDevice/HvDeviceNew.h"
//#include "libHvDevice/HvCamera.h"
#include "utilityTool/tool_function.h"
#include"utilityTool/inifile.h"
//#include "utilityTool/log4z.h"
//#include <process.h>
#include <exception>
#include <new>
#include <time.h>
#include<stdio.h>
#include "vehcommondef.h"

#define CHECK_ARG(arg)\
	if (arg == NULL) \
	{\
    WriteFormatLog("%s is NULL", #arg); \
	return 0;\
	}

#ifndef WM_USER
#define WM_USER 0x0400
#endif

Camera6467_VFR::Camera6467_VFR() :
	BaseCamera(),
	m_dwLastCarID(-1),
    m_iTimeInvl(20),
	m_iSuperLenth(13),
	m_iResultTimeOut(1500),
    m_iResultMsg(WM_USER + 1),
	m_pResult(NULL),
    m_pBufferResult(NULL),
	m_bStatusCheckThreadExit(false),
	m_bJpegComplete(false),
    m_Camera_Plate(NULL),
    m_pUdpClient(NULL)
{
	ReadConfig();
    m_bStartSaveVideo = false;

    int iRet_thrd = pthread_create(&m_threadCheckStatus, NULL, &Camera_StatusCheckThread, (void *) this);
    m_bThreadCreateSussess = (iRet_thrd == 0) ? true :false;
    WriteFormatLog("StatusCheckThread = %d", iRet_thrd);

    iRet_thrd = pthread_create(&m_threadSendResult, NULL, &threadFunc_sendResult, (void *) this);
    m_bIfSendResultThreadCreateSuccess = (iRet_thrd == 0) ? true :false;
    WriteFormatLog("SendResultThread = %d", iRet_thrd);
}


Camera6467_VFR::~Camera6467_VFR()
{
	m_Camera_Plate = NULL;
	SetCheckThreadExit(true);
    //Tool_SafeCloseThread(m_hStatusCheckThread);
	InterruptionConnection();
	DisConnectCamera();    

	SAFE_DELETE_OBJ(m_pResult);
    SAFE_DELETE_OBJ(m_pBufferResult);
    SAFE_DELETE_OBJ(m_pUdpClient);

    if(m_bThreadCreateSussess)
    {
        pthread_join(m_threadCheckStatus, NULL);
    }

    if(m_bIfSendResultThreadCreateSuccess)
    {
        pthread_join(m_threadSendResult, NULL);
    }
}

void Camera6467_VFR::AnalysisAppendXML(CameraResult* CamResult)
{
	if (NULL == CamResult)
		return;
	if (0 != CamResult->dw64TimeMS)
	{
        ConverTimeTickToString(CamResult->dw64TimeMS, CamResult->chPlateTime, sizeof(CamResult->chPlateTime));
	}
	else
	{
        struct tm timeNow;
        long long iTimeInMilliseconds = 0;
        Tool_GetTime(&timeNow, &iTimeInMilliseconds);

        sprintf(CamResult->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d",
            timeNow.tm_year+1900,
            timeNow.tm_mon+1,
            timeNow.tm_mday,
            timeNow.tm_hour,
            timeNow.tm_min,
            timeNow.tm_sec,
            iTimeInMilliseconds%1000);
	}

	char chTemp[BUFFERLENTH] = { 0 };
	int iLenth = BUFFERLENTH;

	memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "IsTruck", chTemp, &iLenth))
	{
		if (strstr(chTemp, "客"))
		{
			CamResult->iVehTypeNo = 1;
		}
		else if (strstr(chTemp, "货"))
		{
			CamResult->iVehTypeNo = TRUCK_TYPE_1;
		}
		else
		{
			CamResult->iVehTypeNo = 0;
		}
	}

    memset(chTemp, '\0', sizeof(chTemp));
    iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "VehicleType", chTemp, &iLenth))
	{		
		CamResult->iVehTypeNo = AnalysisVelchType(chTemp);
        sprintf(CamResult->chVehicleType, "%s", chTemp);
        WriteFormatLog("AnalysisVelchType %s , get the vehicle type %d", chTemp, CamResult->iVehTypeNo);
	}

	memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "AxleCnt", chTemp, &iLenth))
	{
		int iAxleCount = 0;
		sscanf(chTemp, "%d", &iAxleCount);
		CamResult->iAxletreeCount = iAxleCount;
		//printf("the Axletree count is %d.\n", iAxleCount);
	}

    memset(chTemp, '\0', sizeof(chTemp));
    iLenth = BUFFERLENTH;
    CamResult->iWheelCount = 4;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "WheelCnt", chTemp, &iLenth))
    {
        int iWheelCount = 0;
        sscanf(chTemp, "%d", &iWheelCount);
        CamResult->iWheelCount = iWheelCount;
    }

    memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
    CamResult->iAxletreeType = AnalysisBusVehAxleType(CamResult->iWheelCount, CamResult->chPlateNO);
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "AxleType", chTemp, &iLenth))
	{
		int iAxleGroupCount = GetAlexGroupCount(chTemp);
		CamResult->iAxletreeType = AnalysisVelchAxleType(chTemp, iAxleGroupCount);
	}

	memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "Wheelbase", chTemp, &iLenth))
	{
		float fWheelbase = 0;
		sscanf(chTemp, "%f", &fWheelbase);
		CamResult->fDistanceBetweenAxles = fWheelbase;
		//printf("the Wheelbase  is %f.\n", fWheelbase);
	}
	memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "CarLength", chTemp, &iLenth))
	{
		float fCarLength = 0;
		sscanf(chTemp, "%f", &fCarLength);
		CamResult->fVehLenth = fCarLength;
		//printf("the CarLength  is %f.\n", fCarLength);
	}
	memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "CarHeight", chTemp, &iLenth))
	{
		float fCarHeight = 0;
		sscanf(chTemp, "%f", &fCarHeight);
		CamResult->fVehHeight = fCarHeight;
		//printf("the CarHeight  is %f.\n", fCarHeight);
	}
    memset(chTemp, '\0', sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "CarWidth", chTemp, &iLenth))
    {
        float fCarWidth = 0;
        sscanf(chTemp, "%f", &fCarWidth);
        CamResult->fVehWidth = fCarWidth;
        //printf("the CarHeight  is %f.\n", fCarHeight);
    }
	memset(chTemp, '\0', sizeof(chTemp));
	iLenth = BUFFERLENTH;
	if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "BackUp", chTemp, &iLenth))
	{
		CamResult->bBackUpVeh = true;
	}
    memset(chTemp, '\0', sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if(Tool_GetTextNodeFromXML(CamResult->pcAppendInfo, strlen(CamResult->pcAppendInfo), "PlateName",chTemp, iLenth ))
    {
        memset(CamResult->chPlateNO, '\0', sizeof(CamResult->chPlateNO));
        sprintf(CamResult->chPlateNO, "%s",  chTemp);
        CamResult->iPlateColor = Tool_AnalysisPlateColorNo(chTemp);
    }
}

int Camera6467_VFR::AnalysisVelchType(const char* vehType)
{
	if (vehType == NULL)
	{
		return UNKOWN_TYPE;
	}
	if (strstr(vehType, "客1"))
	{
		return BUS_TYPE_1;
	}
	else if (strstr(vehType, "客2"))
	{
		return BUS_TYPE_2;
		//printf("the Vehicle type code is 2.\n");
	}
	else if (strstr(vehType, "客3"))
	{
		return BUS_TYPE_3;
	}
	else if (strstr(vehType, "客4"))
	{
		return BUS_TYPE_4;
	}
	else if (strstr(vehType, "客5"))
	{
		return BUS_TYPE_5;
	}
	else if (strstr(vehType, "货1"))
	{
		return TRUCK_TYPE_1;
	}
	else if (strstr(vehType, "货2"))
	{
		return TRUCK_TYPE_2;
	}
	else if (strstr(vehType, "货3"))
	{
		return TRUCK_TYPE_3;
	}
	else if (strstr(vehType, "货4"))
	{
		return TRUCK_TYPE_4;
	}
	else if (strstr(vehType, "货5"))
	{
		return TRUCK_TYPE_5;
	}
	else
	{
		return UNKOWN_TYPE;
	}
}

int Camera6467_VFR::GetAlexType(int ivalue)
{
	int iType = 0;
	switch (ivalue)
	{
	case 1:
		iType = 1;
		break;
	case 2:
		iType = 2;
		break;
	case 11:
		iType = 11;
		break;
	case 12:
		iType = 12;
		break;
	case 22:
		iType = 5;
		break;
	case 111:
		iType = 111;
		break;
	case 112:
		iType = 112;
		break;
	case 122:
		iType = 15;
		break;
	case 222:
		iType = 7;
		break;
	case 1112:
		iType = 114;
		break;
	case 1122:
		iType = 115;
		break;
	default:
		iType = 1;
		break;
	}
	return iType;
}

int Camera6467_VFR::GetAlexGroupCount(const char* AxleType)
{
	if (NULL == AxleType
		|| strlen(AxleType) <= 0 )
	{
		WriteFormatLog("GetAlexGroupCount, NULL == AxleType , return 0.");
		return 0;
	}
	int iGroupCout = 0;
	for (size_t i= 0; i< strlen(AxleType) ; i++)
	{
		if (AxleType[i] == '+')
		{
			iGroupCout++;
		}
		else if (AxleType[i] == '\0')
		{
			break;
		}
	}
	WriteFormatLog("GetAlexGroupCount, iGroupCout = %d.", iGroupCout);
	return iGroupCout;
}

int Camera6467_VFR::AnalysisVelchAxleType(const char* AxleType, int  iAxleGroupCount)
{
	if (NULL == AxleType
		|| strlen(AxleType) <= 0
		|| iAxleGroupCount <= 0)
	{
		WriteFormatLog("AnalysisVelchAxleType, NULL == AxleType , return 12.");
		return 12;
	}
	WriteFormatLog("AnalysisVelchAxleType, AxleType = %s, iAxleGroupCount = %d",AxleType, iAxleGroupCount);

	char chTempAxle[256] = {0};
	//strcpy_s(chTempAxle, sizeof(chTempAxle), AxleType);
	strcpy(chTempAxle,  AxleType);

	int iAxletreeType = 12;

	int iFirstValue = 0, iSeconed = 0, iThird= 0, iFourth= 0, iFifth  = 0, iSix= 0, iSeventh = 0;
	int iAxletreeType1 = 0, iAxletreeType2 = 0, iAxletreeType3 = 0, iAxletreeType4 = 0, iAxletreeType5 = 0, iAxletreeType6 = 0, iAxletreeType7 = 0;
	char chAxletreeType[32] = {0};
	int iAxleCount = iAxleGroupCount +1;
	switch (iAxleCount)
	{
	case 1:
		iAxletreeType =12;
		break;
	case 2:
		sscanf(chTempAxle,  "%d+%d", &iFirstValue, &iSeconed);
		iAxletreeType1 = GetAlexType(iFirstValue);
		iAxletreeType2 = GetAlexType(iSeconed);

		sprintf(chAxletreeType,  "%d%d", iAxletreeType1, iAxletreeType2);
		sscanf(chAxletreeType, "%d", &iAxletreeType);
		break;
	case 3:
		sscanf(chTempAxle, "%d+%d+%d", &iFirstValue, &iSeconed, &iThird);
		iAxletreeType1 = GetAlexType(iFirstValue);
		iAxletreeType2 = GetAlexType(iSeconed);
		iAxletreeType3 = GetAlexType(iThird);

		sprintf(chAxletreeType,  "%d%d%d", iAxletreeType1, iAxletreeType2, iAxletreeType3);
		sscanf(chAxletreeType, "%d", &iAxletreeType);
		break;
	case 4:
		sscanf(chTempAxle, "%d+%d+%d+%d", &iFirstValue, &iSeconed, &iThird, &iFourth);
		iAxletreeType1 = GetAlexType(iFirstValue);
		iAxletreeType2 = GetAlexType(iSeconed);
		iAxletreeType3 = GetAlexType(iThird);
		iAxletreeType4 = GetAlexType(iFourth);

		sprintf(chAxletreeType,  "%d%d%d%d", iAxletreeType1, iAxletreeType2, iAxletreeType3, iAxletreeType4);
		sscanf(chAxletreeType, "%d", &iAxletreeType);
		break;
	case 5:
		sscanf(chTempAxle, "%d+%d+%d+%d+%d", &iFirstValue, &iSeconed, &iThird, &iFourth, &iFifth);
		iAxletreeType1 = GetAlexType(iFirstValue);
		iAxletreeType2 = GetAlexType(iSeconed);
		iAxletreeType3 = GetAlexType(iThird);
		iAxletreeType4 = GetAlexType(iFourth);
		iAxletreeType5 = GetAlexType(iFifth);

		sprintf(chAxletreeType,  "%d%d%d%d%d", iAxletreeType1, iAxletreeType2, iAxletreeType3, iAxletreeType4, iAxletreeType5);
		sscanf(chAxletreeType, "%d", &iAxletreeType);
		break;
	case 6:
		sscanf(chTempAxle, "%d+%d+%d+%d+%d+%d", &iFirstValue, &iSeconed, &iThird, &iFourth, &iFifth, &iSix);
		iAxletreeType1 = GetAlexType(iFirstValue);
		iAxletreeType2 = GetAlexType(iSeconed);
		iAxletreeType3 = GetAlexType(iThird);
		iAxletreeType4 = GetAlexType(iFourth);
		iAxletreeType5 = GetAlexType(iFifth);
		iAxletreeType6 = GetAlexType(iSix);


		sprintf(chAxletreeType, "%d%d%d%d%d%d",
			iAxletreeType1,
			iAxletreeType2,
			iAxletreeType3,
			iAxletreeType4,
			iAxletreeType5, 
			iAxletreeType6);
		sscanf(chAxletreeType, "%d", &iAxletreeType);
		break;
	case 7:
		sscanf(chTempAxle, "%d+%d+%d+%d+%d+%d+%d", &iFirstValue, &iSeconed, &iThird, &iFourth, &iFifth, &iSix, &iSeventh);
		iAxletreeType1 = GetAlexType(iFirstValue);
		iAxletreeType2 = GetAlexType(iSeconed);
		iAxletreeType3 = GetAlexType(iThird);
		iAxletreeType4 = GetAlexType(iFourth);
		iAxletreeType5 = GetAlexType(iFifth);
		iAxletreeType6 = GetAlexType(iSix);
		iAxletreeType7 = GetAlexType(iSeventh);

		sprintf(chAxletreeType,"%d%d%d%d%d%d%d", 
			iAxletreeType1,
			iAxletreeType2,
			iAxletreeType3, 
			iAxletreeType4, 
			iAxletreeType5, 
			iAxletreeType6, 
			iAxletreeType7);
		sscanf(chAxletreeType, "%d", &iAxletreeType);
		break;
	default:
		WriteLog("use default AxletreeType 2");
		iAxletreeType = 12;
		break;
	}

	WriteFormatLog("AnalysisVelchAxleType,finish,  AxleType = %d", iAxletreeType);
    return iAxletreeType;
}

int Camera6467_VFR::AnalysisBusVehAxleType(int WheelCount, const char *plateNumber)
{
    if(WheelCount <= 4 || plateNumber == NULL)
        return 11;
    if(WheelCount > 4 || strstr(plateNumber, "黄") != NULL)
    {
        return 12;
    }
    else
    {
        return 11;
    }
}

bool Camera6467_VFR::CheckIfSuperLength(CameraResult* CamResult)
{
	CHECK_ARG(CamResult);
	if (CamResult->fVehLenth > m_iSuperLenth)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Camera6467_VFR::CheckIfBackUpVehicle(CameraResult* CamResult)
{
	CHECK_ARG(CamResult);
	if (CamResult->bBackUpVeh)
	{
		return true;
	}
	else
	{
		return false;
    }
}

int Camera6467_VFR::GetTimeInterval()
{
    int iIntervel = 30;
    m_csFuncCallback.lock();
    iIntervel = m_iTimeInvl;
    m_csFuncCallback.unlock();
    return iIntervel;
}

void Camera6467_VFR::SetFrontImageCallback(void *pFunc, void *pUser)
{
    m_csFuncCallback.lock();
    pFrontImageCallbackFunc = pFunc;
    pFrontImageUserdata = pUser;
    m_csFuncCallback.unlock();
}

void Camera6467_VFR::SetCompleteResultCallback(void *pFunc, void *pUser)
{
    m_csFuncCallback.lock();
    pCompleteResultCallbackFunc = pFunc;
    pCompleteResultUserdata = pUser;
    m_csFuncCallback.unlock();
}

bool Camera6467_VFR::SendFrontImageByCallback(CameraResult *CamResult)
{
    bool bRet = false;
    WriteFormatLog("SendFrontImageByCallback");
    m_csFuncCallback.lock();
    if(pFrontImageCallbackFunc != NULL)
    {
       m_csFuncCallback.unlock();

        SPlateInfoResult plateResult;
        memset(&plateResult, 0, sizeof(SPlateInfoResult));
        plateResult.ID = CamResult->dwCarID;
        char chPlateNo[20] = {0};
        char chColor[5] = {0};
        Tool_ProcessPlateNo(CamResult->chPlateNO, chPlateNo, sizeof(chPlateNo), chColor, sizeof(chColor));
        sprintf(plateResult.plateStr, "%s", chPlateNo);
        memset(plateResult.plateColor, '\0', sizeof(plateResult.plateColor));
        strcpy(plateResult.plateColor,chColor );
        CameraIMG* pImage = &(CamResult->CIMG_BeginCapture);
        if(pImage->dwImgSize > 0)
        {
            strcpy(plateResult.headpic,pImage->chSavePath );
        }
        WriteFormatLog("FrontImageCallback begin, func = %p ",pFrontImageCallbackFunc);
       // m_csFuncCallback.lock();
        ((VehRec_PlateDataFunc)pFrontImageCallbackFunc)(GetLoginID(), &plateResult);
        //m_csFuncCallback.unlock();

        WriteFormatLog("FrontImageCallback finish.");
        bRet = true;
    }
    else
    {
        m_csFuncCallback.unlock();
        WriteFormatLog("FrontImageCallback is NULL.");
    }
    WriteFormatLog("SendFrontImageByCallback finish");
    return bRet;
}

bool Camera6467_VFR::SendCompleteResultByCallback(CameraResult *CamResult)
{
    bool bRet = false;
    WriteFormatLog("SendCompleteResultByCallback");
    m_csFuncCallback.lock();
    if(pCompleteResultCallbackFunc != NULL)
    {
        m_csFuncCallback.unlock();
        WriteFormatLog("process complete result.\n");
        SCarInfoResult completeResult;
        memset(&completeResult, 0, sizeof(SCarInfoResult));
        completeResult.ID = CamResult->dwCarID;

        char chPlateNo[20] = {0};
        char chColor[5] = {0};
        Tool_ProcessPlateNo(CamResult->chPlateNO, chPlateNo, sizeof(chPlateNo), chColor, sizeof(chColor));
        WriteFormatLog("Tool_ProcessPlateNo finish, chPlateNo = %s, chColor = %s \n", chPlateNo, chColor);
        sprintf(completeResult.plateStr, "%s", chPlateNo);
        memset(completeResult.plateColor, '\0', sizeof(completeResult.plateColor));
        strcpy(completeResult.plateColor,chColor );

        memset(chPlateNo, '\0', sizeof(chPlateNo));
        memset(chColor, '\0', sizeof(chColor));
        Tool_ProcessPlateNo(CamResult->chTailPlateNo, chPlateNo, sizeof(chPlateNo), chColor, sizeof(chColor));
        sprintf(completeResult.tailPlateNump, "%s", chPlateNo);
        memset(completeResult.tailPalteColor, '\0', sizeof(completeResult.tailPalteColor));
        strcpy(completeResult.tailPalteColor,chColor );
        WriteFormatLog("tailPlateNump finish, chPlateNo = %s, chColor = %s \n", chPlateNo, chColor);

        completeResult.carType = CamResult->iVehTypeNo;
        completeResult.szZhoushu = CamResult->iAxletreeCount;
        completeResult.szLunshu = CamResult->iWheelCount;
        if(strlen(CamResult->chVehicleType) > 0)
        {
            memset(completeResult.carTypeStr, '\0', sizeof(completeResult.carTypeStr));
            sprintf(completeResult.carTypeStr,"%s", CamResult->chVehicleType);
        }
        WriteFormatLog("begin to process begin CIMG_BeginCapture.\n");
        CameraIMG* pImage = &(CamResult->CIMG_BeginCapture);
        if(pImage->dwImgSize > 0)
        {
            //Front
            strcpy(completeResult.headpic,pImage->chSavePath );
        }
        WriteFormatLog("begin to process begin CIMG_BestCapture.\n");
        pImage = &(CamResult->CIMG_BestCapture);
        if(pImage->dwImgSize > 0)
        {
            //body
            strcpy(completeResult.colpic,pImage->chSavePath );
        }
        WriteFormatLog("begin to process begin CIMG_LastCapture.\n");
        pImage = &(CamResult->CIMG_LastCapture);
        if(pImage->dwImgSize > 0)
        {
            //tail
            strcpy(completeResult.tailpic,pImage->chSavePath );
        }
        WriteFormatLog("begin to process begin chSavePath.\n");
        if(strlen(CamResult->chSavePath ) > 0)
        {
            strcpy(completeResult.recfile, CamResult->chSavePath);
        }

        WriteFormatLog("CompleteResult begin. ,func = %p", pCompleteResultCallbackFunc);

        //m_csFuncCallback.lock();
        ((VehRec_GetCarDataFun)pCompleteResultCallbackFunc)(GetLoginID(), &completeResult);
        //m_csFuncCallback.unlock();

        WriteFormatLog("CompleteResult finish.");
        bRet = true;
    }
    else
    {
        m_csFuncCallback.unlock();
        WriteFormatLog("CompleteResult Callback is NULL.");
    }
    WriteFormatLog("SendCompleteResultByCallback finish");
    return bRet;
}

void Camera6467_VFR::ReadConfig()
{
	char chTemp[MAX_PATH] = { 0 };
	int iTempValue = 0;

	iTempValue = 15;
	Tool_ReadIntValueFromConfigFile(INI_FILE_NAME, "Filter", "SuperLongVehicleLenth", iTempValue);
	m_iSuperLenth = iTempValue > 0 ? iTempValue : 15;

    iTempValue = 1500;
    Tool_ReadIntValueFromConfigFile(INI_FILE_NAME, "Filter", "ResultTimeOut", iTempValue);
    m_iResultTimeOut = iTempValue > 0 ? iTempValue : 1500;

	BaseCamera::ReadConfig();
}

void Camera6467_VFR::SetCheckThreadExit(bool bExit)
{
    m_csFuncCallback.lock();
	m_bStatusCheckThreadExit = bExit;
    m_csFuncCallback.unlock();
}

bool Camera6467_VFR::GetCheckThreadExit()
{
	bool bExit = false;
    m_csFuncCallback.lock();
	bExit = m_bStatusCheckThreadExit;
    m_csFuncCallback.unlock();
	return bExit;
}

bool Camera6467_VFR::OpenPlateCamera(const char* ipAddress)
{
	CHECK_ARG(ipAddress);

	WriteFormatLog("OpenPlateCamera %s begin.", ipAddress);
	//m_Camera_Plate = std::make_shared<Camera6467_plate>();
	m_Camera_Plate = new Camera6467_plate();
	m_Camera_Plate->SetCameraIP(ipAddress);
	if (0 == m_Camera_Plate->ConnectToCamera())
	{
		WriteFormatLog("OpenPlateCamera %s success.", ipAddress);
		return true;
	}
	else
	{
		WriteFormatLog("OpenPlateCamera %s failed.", ipAddress);
		return false;
	}
}

CameraResult* Camera6467_VFR::GetFrontResult()
{
	try
	{
		CameraResult* pResultPlate = NULL;
		CameraResult* pResultVFR = NULL;
		bool bGetPlateNo = false;
		if (m_Camera_Plate != NULL)
		{
			CameraResult* pTemp = m_Camera_Plate->GetOneResult();
			if (pTemp != NULL)
			{
				//pResultPlate = std::shared_ptr<CameraResult>(pTemp);
				pResultPlate = pTemp;
				bGetPlateNo = true;
				pTemp = NULL;
			}
			else
			{
				bGetPlateNo = false;
			}
		}
		else
		{
			if (!m_resultList.empty())
			{
				m_resultList.front(pResultPlate);
			}
		}
		if (pResultPlate != NULL)
		{
			if (bGetPlateNo)
			{
				WriteFormatLog("Get one result from plate camera , plate : %s.", pResultPlate->chPlateNO);
				WriteFormatLog("GetFrontResult, plate No list:\n");
				BaseCamera::WriteLog(m_resultList.GetAllPlateString().c_str());
				int iIndex = -1;
				if (strstr(pResultPlate->chPlateNO, "无"))
				{
					WriteFormatLog("GetFrontResult, current plate no is 无车牌, use first result .");
					iIndex = -1;
				}
				else
				{
					iIndex = m_resultList.GetPositionByPlateNo(pResultPlate->chPlateNO);
				}
				if (-1 != iIndex)
				{
					WriteFormatLog("find the result from list, index = %d.", iIndex);
					pResultVFR = m_resultList.GetOneByIndex(iIndex);
				}
				else
				{
					if (!m_resultList.empty())
					{
						WriteFormatLog("can not find result from list, Get first result.");
						m_resultList.front(pResultVFR);
					}
					else
					{
						WriteFormatLog("can not find result from list, the list is empty.");
					}
				}
				SAFE_DELETE_OBJ(pResultPlate);
			}
			else
			{
				pResultPlate = NULL;
				WriteFormatLog("can not get result from plate camera, get from VFR list.");
				if (!m_resultList.empty())
				{
					WriteFormatLog("Get first result.");
					m_resultList.front(pResultVFR);
				}
				else
				{
					WriteFormatLog("The list is empty.");
				}
			}
		}
		else
		{
			WriteFormatLog("Can not get result from  camera .");
		}
		if (pResultVFR == NULL)
		{
			return NULL;
		}
		else
		{
			return new CameraResult(*pResultVFR);
		}		
	}
	catch (std::bad_exception& e)
	{
		printf("GetFrontResult, bad_exception, error msg = %s", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (std::bad_alloc& e)
	{
		printf("GetFrontResult, bad_alloc, error msg = %s", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (std::exception& e)
	{
		printf("GetFrontResult, exception, error msg = %s.", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (void*)
	{
		printf("GetFrontResult,  void* exception");
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (...)
	{
		printf("GetFrontResult,  unknown exception");
		return NULL;
		//return std::make_shared<CameraResult>();
	}
}

CameraResult*  Camera6467_VFR::GetFrontResultByPlateNo(const char* plateNo)
{
	try
	{
		WriteFormatLog("GetFrontResultByPlateNo , plateNo = %s", plateNo);
		//std::shared_ptr<CameraResult> tempResult;
		CameraResult*  pTempResult =NULL;
		if (m_resultList.empty())
		{
			WriteFormatLog("GetFrontResultByPosition , resultList is empty, return null.");
			return pTempResult;
		}
		int iIndex = -1;
		if (plateNo == NULL
			|| strlen(plateNo) == 0
			|| strstr(plateNo, "无"))
		{
			WriteFormatLog("GetFrontResult, current plate no is 无车牌, use first result .");
			iIndex = -1;
		}
		else
		{
			iIndex = m_resultList.GetPositionByPlateNo(plateNo);
		}
		if (-1 != iIndex)
		{
			WriteFormatLog("find the result from list, index = %d.", iIndex);
			pTempResult = m_resultList.GetOneByIndex(iIndex);
		}
		else
		{
			if (!m_resultList.empty())
			{
				WriteFormatLog("can not find result from list, Get first result.");
				m_resultList.front(pTempResult);
			}
			else
			{
				WriteFormatLog("can not find result from list, the list is empty.");
			}
		}
		if (pTempResult == NULL)
		{
			return NULL;
		}
		else
		{
			return new CameraResult(*pTempResult);
		}		
	}
	catch (std::bad_exception& e)
	{
		WriteFormatLog("GetFrontResultByPlateNo, bad_exception, error msg = %s", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (std::bad_alloc& e)
	{
		WriteFormatLog("GetFrontResultByPlateNo, bad_alloc, error msg = %s", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (std::exception& e)
	{
		WriteFormatLog("GetFrontResultByPlateNo, exception, error msg = %s.", e.what());

		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (void*)
	{
		WriteFormatLog("GetFrontResultByPlateNo,  void* exception");
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (...)
	{
		WriteFormatLog("GetFrontResultByPlateNo,  unknown exception");
		return NULL;
		//return std::make_shared<CameraResult>();
	}
}

CameraResult* Camera6467_VFR::GetFrontResultByPosition(int position)
{
	try
	{
		WriteFormatLog("GetFrontResultByPosition , position = %d", position);
		//std::shared_ptr<CameraResult> pTempResult;
		CameraResult* pTempResult = NULL;
		if (m_resultList.empty())
		{
			WriteFormatLog("GetFrontResultByPosition , resultList is empty, return null.");
			return pTempResult;
		}
		if (position <= 0)
		{
            WriteFormatLog("GetFrontResultByPosition ,finish, return first.");
			pTempResult = GetFrontResult();
            return pTempResult;
		}
		else
		{
			if (position >= m_resultList.size())
			{
				WriteFormatLog("GetFrontResultByPosition , position : %d is larger than  resultList size %d, get the last one.",
					position,
					m_resultList.size());
				pTempResult = m_resultList.GetOneByIndex(m_resultList.size() - 1);
			}
			else
			{
				pTempResult = m_resultList.GetOneByIndex(position);
			}
            WriteFormatLog("GetFrontResultByPosition ,finish");
            if (pTempResult == NULL)
            {
                return NULL;
            }
            else
            {
                return new CameraResult(*pTempResult);
            }
		}
	}
	catch (std::bad_exception& e)
	{
		printf("GetFrontResultByPosition, bad_exception, error msg = %s", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (std::bad_alloc& e)
	{
		printf("GetFrontResultByPosition, bad_alloc, error msg = %s", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (std::exception& e)
	{
		printf("GetFrontResultByPosition, exception, error msg = %s.", e.what());
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (void*)
	{
		printf("GetFrontResultByPosition,  void* exception");
		return NULL;
		//return std::make_shared<CameraResult>();
	}
	catch (...)
	{
		printf("GetFrontResultByPosition,  unknown exception");
		return NULL;
		//return std::make_shared<CameraResult>();
    }
}

std::string Camera6467_VFR::GetAllVehTypeList()
{
    return m_resultList.GetAllVehTypeString();
}

void Camera6467_VFR::DeleteFrontResult(const char* plateNo)
{
	try
	{
		WriteFormatLog("DeleteFrontResult, plate no = %p", plateNo);
		bool bHasPlateNo = false;
		if (NULL != plateNo && strlen(plateNo) > 0)
		{
			bHasPlateNo = true;
		}
		std::string strPlateNo;
		if (bHasPlateNo)
		{
			strPlateNo = plateNo;
			WriteFormatLog("DeleteFrontResult, has plate no : %s", plateNo);
		}
		else
		{            
			if (  m_Camera_Plate != NULL  )
			{
				WriteFormatLog("DeleteFrontResult, get from plate camera:");
				CameraResult* pResult = m_Camera_Plate->GetOneResult();
				if (NULL != pResult)
				{
					strPlateNo = pResult->chPlateNO;
					SAFE_DELETE_OBJ(pResult);
					WriteFormatLog("DeleteFrontResult, plate no : %s", strPlateNo.c_str());
				}
				else
				{
					WriteFormatLog("DeleteFrontResult,can not get from plate camera.");
				}
			}
			else
			{
				WriteFormatLog("DeleteFrontResult, cant not get plate number from plate camera.");
			}
		}

		if (strPlateNo.empty())
		{
			WriteFormatLog("DeleteFrontResult, cant not get plate number , so delete front result.");

            m_resultList.DeleteToPosition(0);
		}
		else
		{
//			if (std::string::npos == strPlateNo.find("无"))
			{
				int iPosition = m_resultList.GetPositionByPlateNo(strPlateNo.c_str());
				WriteFormatLog("DeleteFrontResult, GetPositionByPlateNo %d.", iPosition);
				m_resultList.DeleteToPosition(iPosition);
			}
//			else
//			{
//				WriteFormatLog("DeleteFrontResult, current plate  %s == ‘无车牌’, do nothing.", strPlateNo.c_str());
//			}
		}
		WriteFormatLog("DeleteFrontResult, final list:");
		BaseCamera::WriteLog(m_resultList.GetAllPlateString().c_str());

		WriteFormatLog("DeleteFrontResult, finish");
	}
	catch (std::bad_exception& e)
	{
		printf("DeleteFrontResult, bad_exception, error msg = %s", e.what());
	}
	catch (std::bad_alloc& e)
	{
		printf("DeleteFrontResult, bad_alloc, error msg = %s", e.what());
	}
	catch (std::exception& e)
	{
		printf("DeleteFrontResult, exception, error msg = %s.", e.what());
	}
	catch (void*)
	{
		printf("DeleteFrontResult,  void* exception");
	}
	catch (...)
	{
		printf("DeleteFrontResult,  unknown exception");
	}
}

void Camera6467_VFR::ClearALLResult()
{
	WriteFormatLog("ClearALLResult begin.");
	m_resultList.ClearALLResult();
	WriteFormatLog("ClearALLResult finish.");
}

size_t Camera6467_VFR::GetResultListSize()
{
	WriteFormatLog("GetResultListSize begin.");
	size_t iSize = m_resultList.size();
	WriteFormatLog("GetResultListSize finish, size = %d.", iSize);
	return iSize;
}

int Camera6467_VFR::RecordInfoBegin(unsigned long dwCarID)
{
	try
	{
		WriteFormatLog("RecordInfoBegin, dwCarID = %lu", dwCarID);

		SAFE_DELETE_OBJ(m_pResult);
		//m_Result = std::make_shared<CameraResult>();
		if (NULL == m_pResult)
		{
			m_pResult = new CameraResult();
		}
		CHECK_ARG(m_pResult);

		m_pResult->dwCarID = dwCarID;

		WriteFormatLog("RecordInfoBegin, finish.");
		return 0;
	}
	catch (std::bad_exception& e)
	{
		WriteFormatLog("RecordInfoBegin, bad_exception, error msg = %s", e.what());
		return 0;
	}
	catch (std::bad_alloc& e)
	{
		WriteFormatLog("RecordInfoBegin, bad_alloc, error msg = %s", e.what());
		return 0;
	}
	catch (std::exception& e)
	{
		WriteFormatLog("RecordInfoBegin, exception, error msg = %s.", e.what());
		return 0;
	}
	catch (void*)
	{
		WriteFormatLog("Camera6467_VFR::RecordInfoBegin,  void* exception");
		return 0;
	}
	catch (...)
	{
		WriteFormatLog("Camera6467_VFR::DeleteFrontResult,  unknown exception");
		return 0;
	}
}

int Camera6467_VFR::RecordInfoEnd(unsigned long dwCarID)
{
	try
	{
		WriteFormatLog("RecordInfoEnd, dwCarID = %lu", dwCarID);
		CHECK_ARG(m_pResult);

        if(m_bStartSaveVideo)
        {
            //sleep(1);
            struct tm TimeNow;
            long long  iCurrentTime = 0;
            Tool_GetTime(&TimeNow, &iCurrentTime);
            StopSaveAviFile(0, iCurrentTime + getVideoDelayTime() * 1000);

            WriteFormatLog("StopSaveAviFile , iCurrentTime = %lld, delay time = %d",iCurrentTime,  getVideoDelayTime());
            m_bStartSaveVideo = false;
        }

		if (dwCarID == m_pResult->dwCarID)
		{
			if (CheckIfBackUpVehicle(m_pResult))
			{
				WriteFormatLog("current result is reversing car, drop this result.");
			}
			else
			{
				if (CheckIfSuperLength(m_pResult))
				{
					WriteFormatLog("current length %f is larger than max length %d, clear list first.", m_pResult->fVehLenth, m_iSuperLenth);
					m_resultList.ClearALLResult();
				}
				WriteFormatLog("push one result to list, current list plate NO:\n");
				if (!m_resultList.empty())
				{
					BaseCamera::WriteLog(m_resultList.GetAllPlateString().c_str());
				}
				else
				{
					WriteFormatLog("list is empty.");
				}
				if (m_resultList.size() >= MAX_VFR_LENGTH)
				{
					WriteFormatLog("current VFR result length is larger than %d, remove the first one.",  MAX_VFR_LENGTH);
//					CameraResult* pTempResult = NULL;
//                    m_resultList.front(pTempResult);
                    m_resultList.pop_front();
//					SAFE_DELETE_OBJ(pTempResult);
				}

				//std::shared_ptr<CameraResult> pResult(m_pResult);
                bool bSameWithLastOne = false;
				if (m_dwLastCarID == dwCarID)
				{
                    bSameWithLastOne = true;
					WriteFormatLog("current car ID  %lu is  same wit last carID %lu, replace the last one.", dwCarID, m_dwLastCarID);
                    CameraResult* pTempResult = m_pBufferResult;
                    //m_resultList.back(pTempResult);

                    if (pTempResult
                        && strlen(pTempResult->chSavePath) > 0)
                    {
                        WriteFormatLog("last car ID  %lu , avi fileName = %s.", pTempResult->dwCarID, pTempResult->chSavePath);
                        memset(m_pResult->chSavePath, '\0', sizeof(m_pResult->chSavePath));
                        strcpy(m_pResult->chSavePath, pTempResult->chSavePath);
                    }

					m_resultList.pop_back();	
                    //SAFE_DELETE_OBJ(pTempResult);
				}
				else
				{
                    //SaveResultToBufferPath(m_pResult);
                    SendFrontImageByCallback(m_pResult);
					m_dwLastCarID = dwCarID;
                }
                SAFE_DELETE_OBJ(m_pBufferResult);
                m_pBufferResult = new CameraResult(*m_pResult);

                m_resultList.push_back(m_pResult);
                m_pResult = NULL;

				WriteFormatLog("after push, list plate NO:\n");
                BaseCamera::WriteLog(m_resultList.GetAllPlateString().c_str());

                if(m_pUdpClient != NULL
                        && !bSameWithLastOne)
                {
                    long iBuf = 0x01020304;
                    if(m_pUdpClient->SendMsgToServer((char*)&iBuf, sizeof(long)))
                    {
                        WriteFormatLog("Send Msg To UDP Server success, content = 0x01020304");
                    }
                    else
                    {
                        WriteFormatLog("Send Msg To UDP Server failed, content = 0x01020304");
                    }
                }

			}
		}
		else
		{
			WriteFormatLog("current car ID  %lu is not same wit result carID %lu.", dwCarID, m_pResult->dwCarID);
		}
		WriteFormatLog("RecordInfoEnd, finish");
		return 0;
	}
	catch (std::bad_exception& e)
	{
		WriteFormatLog("RecordInfoEnd, bad_exception, error msg = %s", e.what());
		return 0;
	}
	catch (std::bad_alloc& e)
	{
		WriteFormatLog("RecordInfoEnd, bad_alloc, error msg = %s", e.what());
		return 0;
	}
	catch (std::exception& e)
	{
		WriteFormatLog("RecordInfoEnd, exception, error msg = %s.", e.what());
		return 0;
	}
	catch (void*)
	{
		WriteFormatLog("RecordInfoEnd,  void* exception");
		return 0;
	}
	catch (...)
	{
		WriteFormatLog("RecordInfoEnd, unknown exception");
		return 0;
	}
}

int Camera6467_VFR::RecordInfoPlate(unsigned long dwCarID,
                                    const char* pcPlateNo,
                                    const char* pcAppendInfo,
                                    unsigned long dwRecordType,
                                    unsigned long long dw64TimeMS)
{
	try
	{
        WriteFormatLog("RecordInfoPlate, dwCarID = %lu, plateNo = %s, dwRecordType= %x, dw64TimeMS= %llu",
			dwCarID,
			pcPlateNo,
			dwRecordType,
			dw64TimeMS);
		BaseCamera::WriteLog(pcAppendInfo);
		CHECK_ARG(m_pResult);

		if (dwCarID == m_pResult->dwCarID)
		{
            struct tm receiveTime;
            Tool_GetTime(&receiveTime, &m_pResult->dwReceiveTime);


            m_pResult->dw64TimeMS = m_pResult->dw64TimeMS == 0? dw64TimeMS : m_pResult->dw64TimeMS ;
			//strcpy_s(m_pResult->chPlateNO, sizeof(m_pResult->chPlateNO), pcPlateNo);
			strcpy(m_pResult->chPlateNO,  pcPlateNo);
			if (strlen(pcAppendInfo) < sizeof(m_pResult->pcAppendInfo))
			{
				//strcpy_s(m_pResult->pcAppendInfo, sizeof(m_pResult->pcAppendInfo), pcAppendInfo);
				strcpy(m_pResult->pcAppendInfo, pcAppendInfo);
				AnalysisAppendXML(m_pResult);
			}
			else
			{
				WriteFormatLog("strlen(pcAppendInfo)< sizeof(m_pResult->pcAppendInfo), can not AnalysisAppendXML.");
			}

            char chAviPath[256] = {0};
            //strPlateTime = strPlateTime.substr(0, 8);
            sprintf(chAviPath,  "%s/%s/", m_chSavePath, m_pResult->chPlateTime );
            Tool_MyMakeDir(chAviPath);

            if(m_dwLastCarID != dwCarID)
            {
                memset(chAviPath, '\0', sizeof(chAviPath));
                //sprintf(chAviPath, "%s\\%s\\%lu-%llu.avi", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);
                sprintf(chAviPath, "%s/%s/%lu-%llu.avi", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);
                //qDebug()<<"chAviPath ="<<chAviPath;
                StartToSaveAviFile(0, chAviPath, getVideoAdvanceTime()*1000);
                memset(m_pResult->chSavePath, '\0', sizeof(m_pResult->chSavePath));
                memcpy(m_pResult->chSavePath, chAviPath, strlen(chAviPath));

                m_bStartSaveVideo = true;
                WriteFormatLog("current car ID  %lu , avi fileName = %s.", dwCarID, chAviPath);
            }
            else
            {
                WriteFormatLog("current car ID  %lu , is same with last carID %lu.", dwCarID, m_dwLastCarID);
            }
		}
		else
		{
			WriteFormatLog("current car ID  %lu is not same wit result carID %lu.", dwCarID, m_pResult->dwCarID);
		}

		WriteFormatLog("RecordInfoPlate, finish.");
		return 0;
	}
	catch (std::bad_exception& e)
	{
		printf("RecordInfoPlate, bad_exception, error msg = %s", e.what());
		return 0;
	}
	catch (std::bad_alloc& e)
	{
		printf("RecordInfoPlate, bad_alloc, error msg = %s", e.what());
		return 0;
	}
	catch (std::exception& e)
	{
		printf("RecordInfoPlate, exception, error msg = %s.", e.what());
		return 0;
	}
	catch (void*)
	{
		printf("RecordInfoPlate,  void* exception");
		return 0;
	}
	catch (...)
	{
		printf("RecordInfoPlate, unknown exception");
		return 0;
	}
}

int Camera6467_VFR::RecordInfoBigImage(unsigned long dwCarID,
                                       unsigned short wImgType,
                                       unsigned short wWidth,
                                       unsigned short wHeight,
                                       unsigned char* pbPicData,
                                       unsigned long dwImgDataLen,
                                       unsigned long dwRecordType,
                                       unsigned long long dw64TimeMS)
{
    WriteFormatLog("RecordInfoBigImage, dwCarID = %lu, wImgType = %u, wWidth= %u, wHeight= %u, \
                   dwImgDataLen= %lu, dwRecordType = %x, dw64TimeMS = %llu.",
            dwCarID,
                   wImgType,
                   wWidth,
                   wHeight,
                   dwImgDataLen,
                   dwRecordType,
                   dw64TimeMS);

    CHECK_ARG(m_pResult);

    char* pSavePath = NULL;
    if(m_pResult->dw64TimeMS == 0)
    {
        m_pResult->dw64TimeMS = dw64TimeMS;
        ConverTimeTickToString(dw64TimeMS, m_pResult->chPlateTime, sizeof(m_pResult->chPlateTime));
    }
    //printf("chPlateTime = %s,\ndwCarID=%lu,dw64TimeMS= %llu \n", m_pResult->chPlateTime, dwCarID,m_pResult->dw64TimeMS );

    std::string strPath(m_chSavePath);
    strPath.append("/").append(m_pResult->chPlateTime).append("/");
    Tool_MyMakeDir(strPath.c_str());

    if (dwCarID == m_pResult->dwCarID)
    {
        if (wImgType == RECORD_BIGIMG_BEST_SNAPSHOT)
        {
            WriteFormatLog("RecordInfoBigImage BEST_SNAPSHO  ");

            CopyDataToIMG(m_pResult->CIMG_BestSnapshot, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);

            pSavePath = m_pResult->CIMG_BestSnapshot.chSavePath;
            //sprintf_s(pSavePath, 256, "%s\\%s\\%llu-%s-front.jpg",
            //    m_chImageDir,
            //    m_pResult->chPlateTime,
            //    m_pResult->dw64TimeMS,
            //    m_pResult->chPlateNO);
            sprintf(pSavePath, "%s/%s/%lu-%llu-front.jpg", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);

        }
        else if (wImgType == RECORD_BIGIMG_LAST_SNAPSHOT)
        {
            WriteFormatLog("RecordInfoBigImage LAST_SNAPSHOT  ");

            CopyDataToIMG(m_pResult->CIMG_LastSnapshot, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);

            pSavePath = m_pResult->CIMG_LastSnapshot.chSavePath;
            //sprintf_s(pSavePath, 256, "%s\\%s\\%lu-%llu-front.jpg",
            //    m_chImageDir,
            //    m_pResult->chPlateTime,
            //    dwCarID,
            //    m_pResult->dw64TimeMS);
            sprintf(pSavePath, "%s/%s/%lu-%llu-front.jpg", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);
        }
        else if (wImgType == RECORD_BIGIMG_BEGIN_CAPTURE)
        {
            WriteFormatLog("RecordInfoBigImage BEGIN_CAPTURE  ");

            CopyDataToIMG(m_pResult->CIMG_BeginCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);

            pSavePath = m_pResult->CIMG_BeginCapture.chSavePath;
            //sprintf_s(pSavePath, 256, "%s\\%s\\%lu-%llu-front.jpg",
            //    m_chImageDir,
            //    m_pResult->chPlateTime,
            //    dwCarID,
            //    m_pResult->dw64TimeMS);
            sprintf(pSavePath, "%s/%s/%lu-%llu-front.jpg", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);

        }
        else if (wImgType == RECORD_BIGIMG_BEST_CAPTURE)
        {
            WriteFormatLog("RecordInfoBigImage BEST_CAPTURE  ");

            CopyDataToIMG(m_pResult->CIMG_BestCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);

            pSavePath = m_pResult->CIMG_BestCapture.chSavePath;
            //sprintf_s(pSavePath, 256, "%s\\%s\\%lu-%llu-side.jpg",
            //    m_chImageDir,
            //    m_pResult->chPlateTime,
            //    dwCarID,
            //    m_pResult->dw64TimeMS);
            sprintf(pSavePath, "%s/%s/%lu-%llu-side.jpg", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);

        }
        else if (wImgType == RECORD_BIGIMG_LAST_CAPTURE)
        {
            WriteFormatLog("RecordInfoBigImage LAST_CAPTURE  ");

            CopyDataToIMG(m_pResult->CIMG_LastCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);

            pSavePath = m_pResult->CIMG_LastCapture.chSavePath;
            //sprintf_s(pSavePath, 256, "%s\\%s\\%lu-%llu-tail.jpg",
            //    m_chImageDir,
            //    m_pResult->chPlateTime,
            //    dwCarID,
            //    m_pResult->dw64TimeMS);
            sprintf(pSavePath,"%s/%s/%lu-%llu-tail.jpg", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);
        }
        else
        {
            WriteFormatLog("RecordInfoBigImage other Image, put it to  LAST_CAPTURE .");
            CopyDataToIMG(m_pResult->CIMG_LastCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);

            pSavePath = m_pResult->CIMG_LastCapture.chSavePath;
            //sprintf_s(pSavePath, 256, "%s\\%s\\%lu-%llu-tail.jpg",
            //    m_chImageDir,
            //    m_pResult->chPlateTime,
            //    dwCarID,
            //    m_pResult->dw64TimeMS);
            sprintf(pSavePath, "%s/%s/%lu-%llu-tail.jpg", m_chSavePath, m_pResult->chPlateTime, dwCarID, m_pResult->dw64TimeMS);
        }
        Tool_SaveFile(pbPicData, dwImgDataLen,pSavePath);
    }
    else
    {
        WriteFormatLog("current car ID  %lu is not same wit result carID %lu.", dwCarID, m_pResult->dwCarID);
    }

    WriteFormatLog("RecordInfoBigImage , finish.");
    return 0;
}

int Camera6467_VFR::RecordInfoSmallImage(unsigned long dwCarID,
                                         unsigned short wWidth,
                                         unsigned short wHeight,
                                         unsigned char* pbPicData,
                                         unsigned long dwImgDataLen,
                                         unsigned long dwRecordType,
                                         unsigned long long dw64TimeMS)
{
	WriteFormatLog("RecordInfoSmallImage, dwCarID = %lu, wWidth= %u, wHeight= %u, \
                   dwImgDataLen= %lu, dwRecordType = %x, dw64TimeMS = %llu.",
				   dwCarID,
				   wWidth,
				   wHeight,
				   dwImgDataLen,
				   dwRecordType,
				   dw64TimeMS);

	CHECK_ARG(m_pResult);

	WriteFormatLog("RecordInfoSmallImage , finish.");
	return 0;

    //int iBuffLen = MAX_IMG_SIZE;
	if (m_pResult->dwCarID == dwCarID)
	{
        CopyDataToIMG(m_pResult->CIMG_PlateImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0x006);
	}
    else
    {
        WriteFormatLog("current car ID  %lu is not same wit result carID %lu.", dwCarID, m_pResult->dwCarID);
    }

	WriteFormatLog("RecordInfoSmallImage, finish.");
	return 0;
}

int Camera6467_VFR::RecordInfoBinaryImage(unsigned long dwCarID,
                                          unsigned short wWidth,
                                          unsigned short wHeight,
                                          unsigned char* pbPicData,
                                          unsigned long dwImgDataLen,
                                          unsigned long dwRecordType,
                                          unsigned long long dw64TimeMS)
{
	WriteFormatLog("RecordInfoBinaryImage, dwCarID = %lu, wWidth= %u, wHeight= %u, \
                   dwImgDataLen= %lu, dwRecordType = %x, dw64TimeMS = %llu.",
				   dwCarID,
				   wWidth,
				   wHeight,
				   dwImgDataLen,
				   dwRecordType,
				   dw64TimeMS);
	CHECK_ARG(m_pResult);

	if (dwCarID == m_pResult->dwCarID)
	{
		CopyDataToIMG(m_pResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
	}
	else
	{
		WriteFormatLog("current car ID  %lu is not same wit result carID %lu.", dwCarID, m_pResult->dwCarID);
	}
	WriteFormatLog("RecordInfoBinaryImage, finish.");
	return 0;
}

int Camera6467_VFR::DeviceJPEGStream(unsigned char* pbImageData,
                                     unsigned long dwImageDataLen,
                                     unsigned long dwImageType,
                                     const char* szImageExtInfo)
{
	static int iCout = 0;
	if (iCout++ > 100)
	{
		WriteFormatLog("DeviceJPEGStream, pbImageData = %p, plateNo = %s, dwImageDataLen= %lu, dwImageType= %lu",
			pbImageData,
			dwImageDataLen,
			dwImageType);

		iCout = 0;
	}

	return 0;

    m_csResult.lock();

	m_bJpegComplete = false;

	m_CIMG_StreamJPEG.dwImgSize = dwImageDataLen;
	m_CIMG_StreamJPEG.wImgWidth = 1920;
	m_CIMG_StreamJPEG.wImgHeight = 1080;
	if (NULL == m_CIMG_StreamJPEG.pbImgData)
	{
		m_CIMG_StreamJPEG.pbImgData = new unsigned char[MAX_IMG_SIZE];
		memset(m_CIMG_StreamJPEG.pbImgData, 0, MAX_IMG_SIZE);
	}
	if (m_CIMG_StreamJPEG.pbImgData)
	{
		memset(m_CIMG_StreamJPEG.pbImgData, 0, MAX_IMG_SIZE);
		memcpy(m_CIMG_StreamJPEG.pbImgData, pbImageData, dwImageDataLen);
		m_bJpegComplete = true;
	}
    m_csResult.unlock();

	return 0;
}

void Camera6467_VFR::CheckStatus()
{
	int iLastStatus = -1;
    long long iLastTick = Tool_GetTickCount(), iCurrentTick = Tool_GetTickCount();
	int iFirstConnctSuccess = -1;

	while (!GetCheckThreadExit())
	{
        usleep(10*1000);
        iCurrentTick = Tool_GetTickCount();
		int iTimeInterval = GetTimeInterval();
		if ((iCurrentTick - iLastTick) >= (iTimeInterval * 1000))
		{
			int iStatus = GetCamStatus();
			if (iStatus != iLastStatus)
			{
				if (iStatus == 0)
				{
					//if (iStatus != iLastStatus)
					//{
					//	pThis->SendConnetStateMsg(true);
					//}
					//SendConnetStateMsg(true);
					WriteLog("设备连接正常.");
					iFirstConnctSuccess = 0;
				}
				else
				{
					//SendConnetStateMsg(false);
					WriteLog("设备连接失败, 尝试重连");

					if (iFirstConnctSuccess == -1)
					{
                        //pThis->ConnectToCamera();
                        ConnectToCamera();
					}
				}
			}
			iLastStatus = iStatus;

			iLastTick = iCurrentTick;
		}
    }
}

void Camera6467_VFR::SendResult()
{
    WriteFormatLog("SendResultThreadFunc begin.");
    DWORD dwLastSendCarID = 0;

    struct tm timeNow;
    long long iTimeNow = 0;

    while (!GetCheckThreadExit())
    {
        usleep(100 * 1000);
        if (!m_resultList.empty())
        {
            CameraResult* pResult = NULL;
            //std::shared_ptr<CameraResult> pResult;
            m_resultList.front(pResult);
            if (NULL == pResult)
            {
                continue;
            }
             Tool_GetTime(&timeNow, & iTimeNow);

            int iTimeDelay = iTimeNow - pResult->dwReceiveTime;
            if (checkIfHasThreePic(pResult)
                || iTimeDelay >= getResultWaitTime())
            {
                if (pResult->dwCarID != dwLastSendCarID)
                {
                    printf("SendResult:: SendCompleteResultByCallback \n");
                    SendCompleteResultByCallback(pResult);

                    dwLastSendCarID = pResult->dwCarID;
                }

                 m_resultList.DeleteToPosition(0);
            }

            pResult = NULL;
        }
    }
    WriteFormatLog("SendResultThreadFunc finish.");
}

void Camera6467_VFR::SetUdpServerPort(int port)
{
    SAFE_DELETE_OBJ(m_pUdpClient);
    m_pUdpClient = new C_UdpClient(port);
    if(m_pUdpClient
            && m_pUdpClient->ConnectToServer())
    {
        WriteFormatLog("SetUdpServerPort %d success.", port);
    }
    else
    {
        WriteFormatLog("SetUdpServerPort %d failed.", port);
    }
}

bool Camera6467_VFR::checkIfHasThreePic(CameraResult *result)
{
    if (result->CIMG_BeginCapture.dwImgSize > 0
        && result->CIMG_BestCapture.dwImgSize > 0
        && result->CIMG_LastCapture.dwImgSize > 0)
    {
        return true;
    }
    return false;
}

int Camera6467_VFR::getResultWaitTime()
{
    int  bTimeOut = 1500;
    m_csFuncCallback.lock();
    bTimeOut = m_iResultTimeOut;
    m_csFuncCallback.unlock();
    return bTimeOut;
}
