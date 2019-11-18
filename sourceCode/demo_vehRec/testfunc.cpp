#include "testfunc.h"
#include<stdio.h>
#include<cstring>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include"libvehrecdll.h"

int g_iHandle = -1;
#define USE_LINUX

bool Tool_MyMakeDir( const char* chImgPath)
{
    char DirName[256];
    strcpy(DirName,chImgPath);
    if((access(DirName,F_OK))!=-1)
    {
        return true;
    }

    int i,len = strlen(DirName);
    if(DirName[len-1]!='/')
    { strcat( DirName, "/" ); }
    len = strlen(DirName);
    for(i=1;i<len;i++)
    {
        if(DirName[i]=='/')
        {
            DirName[i] = 0;
            if(access(DirName,NULL) != 0)
            {
#ifdef USE_LINUX
                if(mkdir(DirName,0777) == -1)
                //if(mkdir(DirName) == -1)
                {
                    //WriteLog("mkdir error");
                    return false;
                }
#else
                if(mkdir(DirName) == -1)
                {
                    //WriteLog("mkdir error");
                    return false;
                }
#endif
            }
            DirName[i] = '/';
        }
    }
    return true;
}


int VehRec_PlateDataFunc_demo(int handle,SPlateInfoResult * plateInfo)
{
    printf("VehRec_PlateDataFunc_demo begin, handle = %d, plateinfo = %p \n", handle, plateInfo);
    if(plateInfo == NULL)
    {
        printf("VehRec_PlateDataFunc_demo, plateInfo == NULL");
        return 0;
    }

    char chPath[256] = {0};
    memset(chPath, '\0', sizeof(chPath));

    getcwd(chPath, sizeof(chPath));
    strcat(chPath, "/callback/");
    Tool_MyMakeDir(chPath);

    struct timespec timeNow;
    clock_gettime(CLOCK_MONOTONIC, &timeNow);

    char chFileName[256] = {0};
    sprintf(chFileName, "%s/handle_%d-ID_%d-receiveTime_%d-plateInfo.txt", chPath, handle, plateInfo->ID, timeNow.tv_sec);

    FILE* pFile = fopen(chFileName, "wb+");
    if(pFile != NULL)
    {
        fprintf(pFile, "ID: %d\n", plateInfo->ID);
        fprintf(pFile, "plateStr: %s\n", plateInfo->plateStr);
        fprintf(pFile, "plateColor: %s\n", plateInfo->plateColor);
        fprintf(pFile, "headpic: %s\n", plateInfo->headpic);

        fclose(pFile);
        pFile = NULL;
    }
    else
    {
        printf("VehRec_PlateDataFunc_demo, create file failed.\n");
        return 0;
    }
    printf("VehRec_PlateDataFunc_demo,finish.\n");
    return 0;
}

int VehRec_GetCarDataFun_demo(int handle,SCarInfoResult * CarInfo)
{
    printf("VehRec_GetCarDataFun,begin, handle = %d, SCarInfoResult = %p .\n", handle, CarInfo);
    if(CarInfo == NULL)
    {
        printf("VehRec_GetCarDataFun_demo, CarInfo == NULL");
        return 0;
    }

    char chPath[256] = {0};
    memset(chPath, '\0', sizeof(chPath));

    getcwd(chPath, sizeof(chPath));
    strcat(chPath, "/callback/");
    Tool_MyMakeDir(chPath);

    struct timespec timeNow;
    clock_gettime(CLOCK_MONOTONIC, &timeNow);

    char chFileName[256] = {0};
    sprintf(chFileName, "%s/handle_%d-ID_%d-receiveTime_%d-SCarInfoResult.txt", chPath, handle, CarInfo->ID, timeNow.tv_sec);

    FILE* pFile = fopen(chFileName, "wb+");
    if(pFile != NULL)
    {
        fprintf(pFile, "ID: %d\n", CarInfo->ID);
        fprintf(pFile, "plateStr: %s\n", CarInfo->plateStr);
        fprintf(pFile, "plateColor: %s\n", CarInfo->plateColor);
        fprintf(pFile, "tailPlateNump: %s\n", CarInfo->tailPlateNump);
        fprintf(pFile, "tailPalteColor: %s\n", CarInfo->tailPalteColor);

        fprintf(pFile, "carType: %d \n", CarInfo->carType);
        fprintf(pFile, "carTypeStr: %s\n", CarInfo->carTypeStr);
        fprintf(pFile, "szZhoushu: %d\n", CarInfo->szZhoushu);
        fprintf(pFile, "szLunshu: %d\n", CarInfo->szLunshu);

        fprintf(pFile, "colpic: %s\n", CarInfo->colpic);
        fprintf(pFile, "headpic: %s\n", CarInfo->headpic);
        fprintf(pFile, "tailpic: %s\n", CarInfo->tailpic);
        fprintf(pFile, "recfile: %s\n", CarInfo->recfile);

        fclose(pFile);
        pFile = NULL;
    }
    else
    {
        printf("VehRec_GetCarDataFun_demo, create file failed.\n");
        return 0;
    }
    printf("VehRec_GetCarDataFun_demo,finish.\n");
    return 0;
}


void printf_menu()
{
    char szMenu[1024] = {0};
    sprintf(szMenu,"************menu***************\n");

    strcat(szMenu, "1 VehRec_InitEx:: initialation log dir.\n");
    strcat(szMenu, "2 VehRec_Free::  release resource \n");
    strcat(szMenu, "3 VehRec_Connect:: connect to the camera.\n");
    strcat(szMenu, "4 VehRec_DisConnect:: disconnect camera.\n");
    strcat(szMenu, "5 VehRec_SetSignal:: send signal to the camera, when the car arrive or leave.\n");
    strcat(szMenu, "6 VehRec_SetPlateDataFun:: set front info callback.\n");
    strcat(szMenu, "7 VehRec_SetCarDataFun:: set complete info callback.\n");
    strcat(szMenu, "Q 退出\n");
    strcat(szMenu, "********************************\n");

    printf("%s", szMenu);
}

void SetLogPath()
{
    char chPath[256] = {0};
    memset(chPath, '\0', sizeof(chPath));

    getcwd(chPath, sizeof(chPath));
    //strcat(chPath, "/log/");

    printf("begin to call VehRec_InitEx, path = %s \n", chPath);
    int iRet = VehRec_InitEx(chPath);
    printf("VehRec_InitEx finish, return code = %d \n", iRet);
}

void FreeResource()
{
    printf("begin to call VehRec_Free\n");
    int iRet = VehRec_Free();
    printf("VehRec_InitEx finish, return code = %d \n", iRet);
}

void ConnectCamera()
{
    printf("begin to call VehRec_Connect, please input camera ip address: \n");
    char chPath[256] = {0};
    memset(chPath, '\0', sizeof(chPath));

    getcwd(chPath, sizeof(chPath));
    strcat(chPath, "/result/");

    char chIp[256] = {0};
    scanf("%s", chIp);

    int iRet = VehRec_Connect(chIp, chPath);
    g_iHandle = iRet;
    printf("VehRec_Connect(%s, %s) finish, return code = %d \n",chIp,chPath,  iRet);
}

void DisConnectCamera()
{
    printf("begin to call VehRec_DisConnect\n");
    VehRec_DisConnect(g_iHandle);
    g_iHandle = -1;
    printf("VehRec_DisConnect(%d) finish. \n", g_iHandle);
}

void SendCarMotionSignal()
{
    printf("begin to call VehRec_SetSignal, send '1' or '2' signal ? \n");
    int iSignal = 1;
    scanf("%d", &iSignal);
    int iRet = VehRec_SetSignal(g_iHandle, iSignal);
    printf("VehRec_SetSignal(%d, %d) finish, return code = %d \n", g_iHandle, iSignal, iRet);
}

void SetPlateCallback()
{
    printf("begin to set plate info callback \n");
    int iRet = VehRec_SetPlateDataFun(g_iHandle, (VehRec_PlateDataFunc*)(&VehRec_PlateDataFunc_demo));
    printf("VehRec_SetPlateDataFun(%d, %p) finish, return code = %d \n", g_iHandle, VehRec_PlateDataFunc_demo, iRet);
}

void SetCarInfoCallback()
{
    printf("begin to set car info callback \n");
    int iRet = VehRec_SetCarDataFun(g_iHandle, (VehRec_GetCarDataFun*)(&VehRec_GetCarDataFun_demo));
    printf("VehRec_SetCarDataFun(%d, %p) finish, return code = %d \n", g_iHandle, VehRec_GetCarDataFun_demo, iRet);
}

void CheckCameraStatus()
{
    printf("begin to DLL_VehRec_CheckStatus \n");
    char chStatus[256] = {0};
    memset(chStatus, '\0', sizeof(chStatus));
    int iRet = VehRec_CheckStatus (g_iHandle,chStatus);
    printf("VehRec_CheckStatus(%d) finish, return code = %d, Device Status = %s \n", g_iHandle,  iRet, chStatus);
}
