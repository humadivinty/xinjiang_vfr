//#include "stdafx.h"
#include "CameraResult.h"
//#include "utilityTool/log4z.h"
#include "utilityTool/tool_function.h"
//#include"commondef.h"
#include<stdio.h>

typedef struct {
    long long iObjecSize;
}ResultInfo;

CameraIMG& CameraIMG::operator=( const CameraIMG& CaIMG )
{
	if (this != &CaIMG)
	{
		wImgWidth = CaIMG.wImgWidth;
		wImgHeight = CaIMG.wImgHeight;
		dwImgSize = CaIMG.dwImgSize;
		wImgType = CaIMG.wImgType;
		for (unsigned int i = 0; i< sizeof(chSavePath); i++)
		{
			chSavePath[i] = CaIMG.chSavePath[i];
		}
		if (NULL != CaIMG.pbImgData)
		{
			pbImgData = new unsigned char[CaIMG.dwImgSize];
            for (unsigned long j = 0; j< dwImgSize; j++)
			{
				pbImgData[j] = CaIMG.pbImgData[j];
			}
		}
		else
		{
			pbImgData = NULL;
		}
	}
	return *this;
}

CameraIMG::CameraIMG()
{
	wImgWidth = 0;
	wImgHeight = 0;
	pbImgData = NULL;
	dwImgSize = 0;
	for (unsigned int i = 0; i< sizeof(chSavePath); i++)
	{
		chSavePath[i] = 0;
	}
}

CameraIMG::CameraIMG( const CameraIMG& CaIMG )
{
    if (this == &CaIMG)
    {
        return;
    }
	wImgWidth = CaIMG.wImgWidth;
	wImgHeight = CaIMG.wImgHeight;
	dwImgSize = CaIMG.dwImgSize;
	wImgType = CaIMG.wImgType;
	memcpy(chSavePath, CaIMG.chSavePath, sizeof(CaIMG.chSavePath));
	if (NULL != CaIMG.pbImgData)
	{
		pbImgData = new unsigned char[CaIMG.dwImgSize];
		memcpy(pbImgData, CaIMG.pbImgData, dwImgSize);
	}
	else
	{
		pbImgData = NULL;
	}
}

CameraIMG::~CameraIMG()
{
	wImgWidth = 0;
	wImgHeight = 0;
	if (NULL != pbImgData)
	{
		delete[] pbImgData;
		pbImgData = NULL;
	}
	dwImgSize = 0;
	memset(chSavePath, 0, sizeof(chSavePath));
}
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

CameraResult::CameraResult()
{
	dwCarID = 0;
	iDeviceID = 0;
	iPlateColor = 0;
	iPlateTypeNo = 0;
    iTailPlateColor = 9;
	dw64TimeMS = 0;
	iSpeed = 0;
	iResultNo = 0;
	iVehTypeNo = 0;		//���ʹ���: ��1--1 ��������4--4�� ��1--5  ��������4--8
    iVehSizeType = 0;
	iVehBodyColorNo = 0;	
	iVehBodyDeepNo = 0;	
    iVehLogoType = 0;
	iAreaNo = 0;
	iRoadNo = 0;
	iLaneNo = 0;
	iDirection = 0;
	iWheelCount = 0;		//����
	iAxletreeCount = 0;		//����
	iAxletreeType = 0;		//����
	iAxletreeGroupCount = 2;
    iReliability = 0;
    dwReceiveTime = 0;
	fVehLenth = 0;			//����
	fDistanceBetweenAxles = 0.0;		//���
	fVehHeight = 0.0;		//����
    fVehWidth = 0.0;
    fConfidenceLevel = 0.0;
	bBackUpVeh = false;		//�Ƿ񵹳�

	//pcAppendInfo = NULL;
	//for (unsigned int i = 0; i < sizeof(pcAppendInfo); i++)
	//{
	//	pcAppendInfo[i] = 0;
	//}
    
    memset(chDeviceIp, '\0', sizeof(chDeviceIp));
    memset(chServerIP, '\0', sizeof(chServerIP));
    memset(chPlateNO, '\0', sizeof(chPlateNO));
    memset(chPlateColor, '\0', sizeof(chPlateColor));
    memset(chListNo, '\0', sizeof(chListNo));
    memset(chPlateTime, '\0', sizeof(chPlateTime));
    memset(chSignStationID, '\0', sizeof(chSignStationID));
    memset(chSignStationName, '\0', sizeof(chSignStationName));
    memset(chSignDirection, '\0', sizeof(chSignDirection));
    memset(chDeviceID, '\0', sizeof(chDeviceID));
    memset(chLaneID, '\0', sizeof(chLaneID)); 
    memset(chCarFace, '\0', sizeof(chCarFace));
    memset(chChileLogo, '\0', sizeof(chChileLogo));
    memset(pcAppendInfo, '\0', sizeof(pcAppendInfo));
    memset(chTailPlateNo, '\0', sizeof(chTailPlateNo));
    memset(chVehicleType, '\0', sizeof(chVehicleType));
    memset(chSavePath, '\0', sizeof(chSavePath));
}

CameraResult::CameraResult( const CameraResult& CaRESULT )
{
    if (this == &CaRESULT)
    {
        return;
    }
	dwCarID = CaRESULT.dwCarID;
	dw64TimeMS = CaRESULT.dw64TimeMS;
	iLaneNo = CaRESULT.iLaneNo;
	iPlateColor = CaRESULT.iPlateColor;
    iTailPlateColor = CaRESULT.iTailPlateColor;
	iVehTypeNo = CaRESULT.iVehTypeNo;
    iVehSizeType = CaRESULT.iVehSizeType;
	iDirection = CaRESULT.iDirection;
	iAreaNo = CaRESULT.iAreaNo;
	iVehBodyColorNo = CaRESULT.iVehBodyColorNo;
	iPlateTypeNo = CaRESULT.iPlateTypeNo;
	iVehBodyDeepNo = CaRESULT.iVehBodyDeepNo;
    iVehLogoType = CaRESULT.iVehLogoType;
	iDeviceID = CaRESULT.iDeviceID;
	iRoadNo = CaRESULT.iRoadNo;
	iSpeed = CaRESULT.iSpeed;
	iAxletreeType = CaRESULT.iAxletreeType;
	iAxletreeCount = CaRESULT.iAxletreeCount;
	iAxletreeGroupCount = CaRESULT.iAxletreeGroupCount;
	iResultNo = CaRESULT.iResultNo;
	iWheelCount = CaRESULT.iWheelCount;
    iReliability = CaRESULT.iReliability;
	fVehHeight = CaRESULT.fVehHeight;
    fVehWidth = CaRESULT.fVehWidth;
	fVehLenth = CaRESULT.fVehLenth;
	fDistanceBetweenAxles = CaRESULT.fDistanceBetweenAxles;
    fConfidenceLevel = CaRESULT.fConfidenceLevel;
	bBackUpVeh = CaRESULT.bBackUpVeh;
    dwReceiveTime = CaRESULT.dwReceiveTime;

	//pcAppendInfo = NULL;
    //strAppendInfo = CaRESULT.strAppendInfo;

    memcpy(chDeviceIp, CaRESULT.chDeviceIp, sizeof(chDeviceIp));
    memcpy(chServerIP, CaRESULT.chServerIP, sizeof(chServerIP));
    memcpy(chPlateNO, CaRESULT.chPlateNO, sizeof(chPlateNO));
    memcpy(chPlateColor, CaRESULT.chPlateColor, sizeof(chPlateColor));
    memcpy(chListNo, CaRESULT.chListNo, sizeof(chListNo));
    memcpy(chPlateTime, CaRESULT.chPlateTime, sizeof(chPlateTime));
    memcpy(chSignStationID, CaRESULT.chSignStationID, sizeof(chSignStationID));
    memcpy(chSignStationName, CaRESULT.chSignStationName, sizeof(chSignStationName));
    memcpy(chSignDirection, CaRESULT.chSignDirection, sizeof(chSignDirection));
    memcpy(chDeviceID, CaRESULT.chDeviceID, sizeof(chDeviceID));
    memcpy(chLaneID, CaRESULT.chLaneID, sizeof(chLaneID));
    memcpy(chCarFace, CaRESULT.chCarFace, sizeof(chCarFace));
    memcpy(chChileLogo, CaRESULT.chChileLogo, sizeof(chChileLogo));
    memcpy(pcAppendInfo, CaRESULT.pcAppendInfo, sizeof(pcAppendInfo));
    memcpy(chTailPlateNo, CaRESULT.chTailPlateNo, sizeof(chTailPlateNo));
    memcpy(chVehicleType, CaRESULT.chVehicleType, sizeof(chVehicleType));
    memcpy(chSavePath, CaRESULT.chSavePath, sizeof(chSavePath));

	CIMG_BestCapture = CaRESULT.CIMG_BestCapture;
	CIMG_BeginCapture = CaRESULT.CIMG_BeginCapture;
	CIMG_LastCapture = CaRESULT.CIMG_LastCapture;
	CIMG_BestSnapshot = CaRESULT.CIMG_BestSnapshot;
	CIMG_LastSnapshot = CaRESULT.CIMG_LastSnapshot;
	CIMG_PlateImage = CaRESULT.CIMG_PlateImage;
	CIMG_BinImage = CaRESULT.CIMG_BinImage;
}

CameraResult::~CameraResult()
{
	dwCarID = 0;
	iDeviceID = 0;
	iPlateColor = 0;
	iPlateTypeNo = 0;
	dw64TimeMS = 0;
	iSpeed = 0;
	iResultNo = 0;
	iVehTypeNo = 0;		//���ʹ���: ��1--1 ��������4--4�� ��1--5  ��������4--8
	iVehBodyColorNo = 0;	
	iVehBodyDeepNo = 0;	
    iVehLogoType = 0;
	iAreaNo = 0;
	iRoadNo = 0;
	iLaneNo = 0;
	iDirection = 0;
	iWheelCount = 0;		//����
	iAxletreeCount = 0;		//����
	iAxletreeGroupCount = 0;
	iAxletreeType = 0;		//����
    iReliability = 0;
	fVehLenth = 0;			//����
	fDistanceBetweenAxles = 0.0;		//���
	fVehHeight = 0.0;		//����
	bBackUpVeh = false;		//�Ƿ񵹳�

	memset(chDeviceIp, 0, sizeof(chDeviceIp));
	memset(chPlateNO, 0, sizeof(chPlateNO));
	memset(chPlateColor, 0, sizeof(chPlateColor));
	memset(chListNo, 0, sizeof(chListNo));
	memset(chPlateTime, 0, sizeof(chPlateTime));
	memset(chSignStationID, 0, sizeof(chSignStationID));
	memset(chSignStationName, 0, sizeof(chSignStationName));
	//memset(pcAppendInfo, 0, sizeof(pcAppendInfo));
	memset(chLaneID, 0, sizeof(chLaneID));
	memset(chDeviceID, 0, sizeof(chDeviceID));
}

void CopyCImgDataToFile(CameraIMG& imageStruct, FILE* pFile)
{
    if (NULL == pFile || imageStruct.dwImgSize <= 0)
    {
        return;
    }
    if (imageStruct.dwImgSize > 0 && NULL != imageStruct.pbImgData)
    {
        fwrite(imageStruct.pbImgData, sizeof(unsigned char), imageStruct.dwImgSize, pFile);
    }
}

bool CameraResult::SerializationToDisk(const char* chFilePath)
{
    if (NULL == chFilePath)
    {
        return false;
    }
    printf("SerializationToDisk::begin , path= %s", chFilePath);
    ResultInfo info;
    info.iObjecSize = sizeof(ResultInfo)+sizeof(CameraResult)+CIMG_BestSnapshot.dwImgSize +
        CIMG_LastSnapshot.dwImgSize +
        CIMG_BeginCapture.dwImgSize +
        CIMG_BestCapture.dwImgSize +
        CIMG_LastCapture.dwImgSize +
        CIMG_PlateImage.dwImgSize +
        CIMG_BinImage.dwImgSize;

    FILE* pfile = NULL;
    //errno_t errCode;
    //_set_errno(0);
    //errCode = fopen_s(&pfile, chFilePath, "wb");
    //if (errCode != 0)
    //{
    //    printf("SerializationToDisk:: open file failed., error code = %d", errCode);
    //    return false;
    //}
	pfile = fopen(chFilePath, "wb");
    if (!pfile)
    {
        //printf("SerializationToDisk:: open file failed ,pfile== NULL, error code = %d", errCode);
		printf("SerializationToDisk:: open file failed ,pfile== NULL");
        return false;
    }
    fwrite(&info, sizeof(unsigned char), sizeof(ResultInfo), pfile);
    fwrite(this, sizeof(unsigned char), sizeof(CameraResult), pfile);

    CopyCImgDataToFile(CIMG_BestSnapshot, pfile);
    CopyCImgDataToFile(CIMG_LastSnapshot, pfile);
    CopyCImgDataToFile(CIMG_BeginCapture, pfile);
    CopyCImgDataToFile(CIMG_BestCapture, pfile);
    CopyCImgDataToFile(CIMG_LastCapture, pfile);
    CopyCImgDataToFile(CIMG_PlateImage, pfile);
    CopyCImgDataToFile(CIMG_BinImage, pfile);    

    //fflush(pfile);
    fclose(pfile);
    pfile = NULL;

    printf("SerializationToDisk::finish, path = %s", chFilePath);
    return true;
}

void CopyCImgDataFromBuffer(CameraIMG& imageStruct, unsigned char* pBuffer)
{
    if (imageStruct.dwImgSize > 0)
    {
        imageStruct.pbImgData = new unsigned char[imageStruct.dwImgSize];
        memcpy(imageStruct.pbImgData, pBuffer, imageStruct.dwImgSize);
    }
    else
    {
        imageStruct.pbImgData = NULL;
    }
}

bool CameraResult::SerializationFromDisk(const char* fileName)
{
    if (NULL == fileName || strlen(fileName) <= 0)
    {
        printf("SerializationFromDisk finish, fileName length is invalid.");
        return false;
    }
    printf("SerializationFromDisk begin, fileName = %s", fileName);
    ///// �����л�����
    size_t iFileSize = Tool_GetFile_size(fileName);
    unsigned char* pFileBuffer = new unsigned char[iFileSize + 1];
    if (!Tool_LoadFile(fileName, (void*)pFileBuffer, iFileSize))
    {
        printf("SerializationFromDisk Tool_LoadFile  failed.");

        SAFE_DELETE_ARRAY(pFileBuffer);
        return false;
    }

    unsigned char* pData = pFileBuffer;
    ResultInfo info;
    memcpy(&info, pData, sizeof(ResultInfo));
    pData += sizeof(ResultInfo);

    if (info.iObjecSize == iFileSize)
    {
        //LOGFMTD("DeSerializationResult:: info.iObjecSize == iFileSize");
    }
    else
    {
        printf("DeSerializationResult:: info.iObjecSize %I64u != iFileSize %u", info.iObjecSize, iFileSize);

        pData = NULL;
        SAFE_DELETE_ARRAY(pFileBuffer);
        //DeleteFile(fileName);
        remove(fileName);
        return false;
    }
    memset(this, 0, sizeof(CameraResult));
    memcpy(this, pData, sizeof(CameraResult));
    pData += sizeof(CameraResult);

    CopyCImgDataFromBuffer(CIMG_BestSnapshot, pData);
    pData += CIMG_BestSnapshot.dwImgSize;

    CopyCImgDataFromBuffer(CIMG_LastSnapshot, pData);
    pData += CIMG_LastSnapshot.dwImgSize;

    CopyCImgDataFromBuffer(CIMG_BeginCapture, pData);
    pData += CIMG_BeginCapture.dwImgSize;

    CopyCImgDataFromBuffer(CIMG_BestCapture, pData);
    pData += CIMG_BestCapture.dwImgSize;

    CopyCImgDataFromBuffer(CIMG_LastCapture, pData);
    pData += CIMG_LastCapture.dwImgSize;

    CopyCImgDataFromBuffer(CIMG_PlateImage, pData);
    pData += CIMG_PlateImage.dwImgSize;

    CopyCImgDataFromBuffer(CIMG_BinImage, pData);
    pData += CIMG_BinImage.dwImgSize;

    //DeleteFile(fileName);
    remove(fileName);
    pData = NULL;
    SAFE_DELETE_ARRAY(pFileBuffer);
    printf("SerializationFromDisk finish, fileName = %s", fileName);
    return true;
}

CameraResult& CameraResult::operator=(const CameraResult& CaRESULT)
{
    if (this == &CaRESULT)
    {
        return *this;
    }
	dwCarID = CaRESULT.dwCarID;
	dw64TimeMS = CaRESULT.dw64TimeMS;
	iLaneNo = CaRESULT.iLaneNo;
	iPlateColor = CaRESULT.iPlateColor;
    iTailPlateColor = CaRESULT.iTailPlateColor;
	iVehTypeNo = CaRESULT.iVehTypeNo;
    iVehSizeType = CaRESULT.iVehSizeType;
	iDirection = CaRESULT.iDirection;
	iAreaNo = CaRESULT.iAreaNo;
	iVehBodyColorNo = CaRESULT.iVehBodyColorNo;
	iPlateTypeNo = CaRESULT.iPlateTypeNo;
	iVehBodyDeepNo = CaRESULT.iVehBodyDeepNo;
    iVehLogoType = CaRESULT.iVehLogoType;
	iDeviceID = CaRESULT.iDeviceID;
	iRoadNo = CaRESULT.iRoadNo;
	iSpeed = CaRESULT.iSpeed;
	iAxletreeType = CaRESULT.iAxletreeType;
	iAxletreeCount = CaRESULT.iAxletreeCount;
	iAxletreeGroupCount = CaRESULT.iAxletreeGroupCount;
	iResultNo = CaRESULT.iResultNo;
	iWheelCount = CaRESULT.iWheelCount;
    iReliability = CaRESULT.iReliability;
	fVehHeight = CaRESULT.fVehHeight;
    fVehWidth = CaRESULT.fVehWidth;
	fVehLenth = CaRESULT.fVehLenth;
	fDistanceBetweenAxles = CaRESULT.fDistanceBetweenAxles;
    fConfidenceLevel = CaRESULT.fConfidenceLevel;
	bBackUpVeh = CaRESULT.bBackUpVeh;
    dwReceiveTime = CaRESULT.dwReceiveTime;

	//pcAppendInfo = NULL;
    //strAppendInfo = CaRESULT.strAppendInfo;

    memcpy(chDeviceIp, CaRESULT.chDeviceIp, sizeof(chDeviceIp));
    memcpy(chServerIP, CaRESULT.chServerIP, sizeof(chServerIP));
    memcpy(chPlateNO, CaRESULT.chPlateNO, sizeof(chPlateNO));
    memcpy(chPlateColor, CaRESULT.chPlateColor, sizeof(chPlateColor));
    memcpy(chListNo, CaRESULT.chListNo, sizeof(chListNo));
    memcpy(chPlateTime, CaRESULT.chPlateTime, sizeof(chPlateTime));
    memcpy(chSignStationID, CaRESULT.chSignStationID, sizeof(chSignStationID));
    memcpy(chSignStationName, CaRESULT.chSignStationName, sizeof(chSignStationName));
    memcpy(chSignDirection, CaRESULT.chSignDirection, sizeof(chSignDirection));
    memcpy(chDeviceID, CaRESULT.chDeviceID, sizeof(chDeviceID));
    memcpy(chLaneID, CaRESULT.chLaneID, sizeof(chLaneID));
    memcpy(chCarFace, CaRESULT.chCarFace, sizeof(chCarFace));
    memcpy(chChileLogo, CaRESULT.chChileLogo, sizeof(chChileLogo));
    memcpy(pcAppendInfo, CaRESULT.pcAppendInfo, sizeof(pcAppendInfo));
    memcpy(chTailPlateNo, CaRESULT.chTailPlateNo, sizeof(chTailPlateNo));
    memcpy(chVehicleType, CaRESULT.chVehicleType, sizeof(chVehicleType));
    memcpy(chSavePath, CaRESULT.chSavePath, sizeof(chSavePath));

	CIMG_BestCapture = CaRESULT.CIMG_BestCapture;
	CIMG_BeginCapture = CaRESULT.CIMG_BeginCapture;
	CIMG_LastCapture = CaRESULT.CIMG_LastCapture;
	CIMG_BestSnapshot = CaRESULT.CIMG_BestSnapshot;
	CIMG_LastSnapshot = CaRESULT.CIMG_LastSnapshot;
	CIMG_PlateImage = CaRESULT.CIMG_PlateImage;
	CIMG_BinImage = CaRESULT.CIMG_BinImage;

	return *this;
}
