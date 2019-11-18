#ifndef TOOL_FUNCTION_H
#define TOOL_FUNCTION_H
#include <pthread.h>
#include <unistd.h>
#include<string>

#define SAFE_DELETE_OBJ(obj) \
if (NULL != obj)                                  \
{                                           \
    delete obj;                        \
    obj = NULL;                      \
}

#define SAFE_DELETE_ARRAY(arrayObj) \
if (NULL != arrayObj)                                  \
{                                           \
    delete[] arrayObj;                        \
    arrayObj = NULL;                      \
}

#define  BUFFERLENTH 256
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct ThreadMutex
{
    ThreadMutex()
    {
        pthread_mutex_init(&mtx, NULL);
    }
    ~ThreadMutex()
    {
        pthread_mutex_destroy(&mtx);
    }
    inline void lock()
    {

        pthread_mutex_lock(&mtx);
       //    printf("Enter %d\n", ++i);
    }
    inline void unlock()
    {
        pthread_mutex_unlock(&mtx);
     //     printf("Leave %d\n", ++t);
    }
    pthread_mutex_t mtx;
 //   int i;
 //   int t;
}ThreadMutex;

bool Tool_MyMakeDir( const char* chImgPath);
void Tool_SetLogPath(const char* path);
bool Tool_libWriteLog( const char* chlog);
void Tool_WriteFormatLog(const char* szfmt, ...);

bool Tool_trim(char * szOutput, const char *szInput);
bool Tool_ReadCommand(char* fileName, char* chCommand);
long Tool_GetFile_size(const char* filename);
bool Tool_IsFileExist(const char* FilePath);
long long Tool_GetTickCount();
bool Tool_SaveFile(void* pData, int length, char* chFileName);
bool Tool_LoadFile(const char* fileName, void* pBuffer, size_t& inputOutputFileSize);
void Tool_GetTime(struct tm* timeNow,long long* piTimeInMilliseconds);
const char* Tool_GetCurrentPath();

void Tool_ReadIntValueFromConfigFile(const char* IniFileName, const char* nodeName, const char* keyName, int&keyValue);
void Tool_ReadKeyValueFromConfigFile(const char* IniFileName, const char* nodeName, const char* keyName, char* keyValue, int bufferSize);
void Tool_WriteKeyValueFromConfigFile(const char* INIFileName, const char* nodeName, const char* keyName, char* keyValue, int bufferSize);

bool Tool_GetDataFromAppenedInfo(char *pszAppendInfo, std::string strItemName, char *pszRstBuf, int *piRstBufLen);

bool Tool_DimCompare(const char *szSrcPlateNo, const char *szDesPlateNo);

int Tool_AnalysisPlateColorNo(const char* szPlateNo);

void InitConfigFile();
int Tool_checkIP(const char* p);

int Tool_pingIpAddress(const char* ipAddress);

int Tool_ProcessPlateNo(const char* srcPlateNum, char *buffer, size_t bufLen, char * colorBuf, size_t coBufLen);


//************************************
// Method:        GetTextNodeFromXML
// Describe:        获取XML 节点的字符节点内容 如 <PlateName>  无车牌</PlateName> 中的无车牌
// FullName:      GetTextNodeFromXML
// Access:          public
// Returns:        bool
// Returns Describe: 成功返回 true ， 失败返回 false
// Parameter:    const char * XmlInfoBuf：  传入的XML字符串
// Parameter:    size_t xmlLength ： XML 字符串长度
// Parameter:    const char * NodeName ： 节点名
// Parameter:    char * ValueBuf ：输入输出参数， 值的缓冲区
// Parameter:    size_t bufLength：输入参数， 缓冲区大小
//************************************
bool Tool_GetTextNodeFromXML(const char* XmlInfoBuf, size_t xmlLength, const char* NodeName, char* ValueBuf, size_t bufLength);

bool Tool_GetDataAttributFromAppenedInfo(char *pszAppendInfo, std::string strItemName, std::string  strAttributeName,char *pszRstBuf, int *piRstBufLen);
#endif // TOOL_FUNCTION_H
