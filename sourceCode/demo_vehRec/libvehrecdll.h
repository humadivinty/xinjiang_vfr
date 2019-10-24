#ifndef LIBVEHRECDLL_H
#define LIBVEHRECDLL_H

//#include "libvehrecdll_global.h"
#include "vehcommondef.h"



#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief VehRec_InitEx
 * @details 初始化动态库
 * @param iLogPath  日志目录最大长度260字节
 * @return  0		成功
                   -1		输入参数错误
                   -3		加载依赖动态库失败
 * @attention 只需要调用1次，并且不能在线程中调用
 */
int WINAPI VehRec_InitEx(char *iLogPath);

/**
 * @brief VehRec_Free
 * @details 释放动态库
 * @return 0		成功
 * @attention 只需要调用1次，并且不能在线程中调用
 */
int WINAPI VehRec_Free();

/**
 * @brief VehRec_Connect
 * @details 连接车型设备
 * @param devIP     设备IP地址格式"X.X.X.X"
 * @param savepath  文件保存路径，最大长度100个字节
                                   车辆正面照, 车辆尾部照, 车辆侧面照和车身录像等信息在此路径下存储，根据配置定期清理，并且确保可以存储一天的数据
 * @return  ＞=1000设备句柄连接设备成功
                    -1		输入参数错误
                    -2		查找可用设备节点失败
                    -4		连接设备失败
 * @attention
 */
int WINAPI VehRec_Connect(char *devIP,char *savepath);

/**
 * @brief VehRec_DisConnect
 * @details 断开车型设备连接
 * @param handle   handle	设备句柄
 */
void WINAPI VehRec_DisConnect(int handle);

/**
 * @brief VehRec_SetSignal
 * @details    车辆驶入和驶离车辆分离器时的触发信号
 * @param handle    设备句柄
 * @param sig   1-车头到达车辆分离器位置；
                         2-车尾离开车辆分离器位置；
 * @return  0		成功
                 -1		输入参数错误
                 -2		查找可用设备节点失败
                 -3		失败
 * @attention   每辆车的车头驶入车辆分离器位置时调用一次。
                        每辆车的车尾驶出车辆分离器位置时调用一次。
 */
int WINAPI VehRec_SetSignal(int handle,int sig);

/**
 * @brief VehRec_SetPlateDataFun
 * @details 设置获取车头信息回调函数
 * @param handle    设备句柄
 * @param pCallBack 获取车头车牌信息的回调函数
 * @return  0		成功
                 -1		输入参数错误
                 -2		查找可用设备节点失败
                 -3		设置失败
 * @attention   可以不调用这个函数，只调用VehRec_SetGetCarDataFun，一次获取完整的数据
 */
int WINAPI VehRec_SetPlateDataFun(int handle,VehRec_PlateDataFunc*pCallBack);

/**
 * @brief VehRec_SetCarDataFun
 * @details 获取车辆数据的回调函数
 * @param handle    handle	设备句柄
 * @param pCallBack     获取车信息的回调函数
 * @return  0		成功
                 -1		输入参数错误
                 -2		查找可用设备节点失败
                 -3		失败
 * @attention
 */
int WINAPI VehRec_SetCarDataFun(int handle,VehRec_GetCarDataFun*pCallBack);

#ifdef __cplusplus
}
#endif

#endif // LIBVEHRECDLL_H
