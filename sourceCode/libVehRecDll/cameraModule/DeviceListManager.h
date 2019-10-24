#pragma once
#include <memory>
#include <map>
//#include <mutex>                // std::mutex, std::unique_lock

#include "Camera6467_plate.h"
#include "utilityTool/tool_function.h"

//#define  OBJ_TYPE Camera6467_plate
#define  OBJ_TYPE BaseCamera

class DeviceListManager
{
public:
    static DeviceListManager* GetInstance();
public:
    size_t GetDeviceCount();
    void ClearAllDevice();
    void AddOneDevice(int iCamId, OBJ_TYPE* pCamera);
	OBJ_TYPE* GetDeviceById(int iCamId);
	OBJ_TYPE* GetDeviceByIpAddress(const char* ipAddress);
    int GetDeviceIdByIpAddress(const char* ipAddress);
    bool FindIfExsit(int camId);
    bool EraseDevice(int camId);

private:
    DeviceListManager();
    ~DeviceListManager();

private:
    std::map<int , OBJ_TYPE* > m_CameraMap;
#ifdef WINDOWS
    CRITICAL_SECTION  m_csLock; // »¥³âËø.
#else
    ThreadMutex m_csLock;
#endif
};

