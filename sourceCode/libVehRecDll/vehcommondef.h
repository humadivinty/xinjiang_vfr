#ifndef VEHCOMMONDEF_H
#define VEHCOMMONDEF_H

#ifndef WINAPI
#define WINAPI
#endif

//#pragma pack (1)
typedef struct
{
    int		ID;					//ID号

    //////////////车头车牌信息///////
    char		plateStr[20];			//车头车牌号码，可能是空或无牌车
    char		plateColor[5];			//车头车牌颜色，包括蓝、白、黑、黄、绿、无

    //////////////车尾车牌信息///////
    char		tailPlateNump[20];		//车尾车牌号码
    char		tailPalteColor[5];		//车尾车牌颜色

    //////////////车型信息////////////
    int		carType;				//车型结果[数值型]，客车:1-4,
    //货车:11-16，专项作业:21-26
    char		carTypeStr[64];		//车型结果[字符串]，客1-客4，货1-货6，专1-专6
    int		szZhoushu;			//轴数
    int		szLunshu;				//轮数

    //////////////图片和视频//////////
    char		colpic[260];			//车辆侧面图绝对路径
    char		headpic[260];			//车头大图或车牌图绝对路径
    char		tailpic[260];			//车尾大图或车牌图绝对路径
    char		recfile[260];			//录像文件绝对路径
}SCarInfoResult;

typedef struct
{
    int		ID;					//ID号

    char		plateStr[20];			//车头车牌号码，可能是空或无牌车
    char		plateColor[5];			//车头车牌颜色，包括蓝、白、黑、黄、绿、无
    char		headpic[260];			//车头大图或车牌图绝对路径
}SPlateInfoResult;
//#pragma pack ()
/**
 *功能	获取车头信息的回调函数
 *传入参数      handle	设备句柄
 *传出参数      pCarData车辆车头信息
 *返回值        0		成功
 *               -1		失败
 */
typedef int (WINAPI *VehRec_PlateDataFunc)(int ,SPlateInfoResult *);

/**
 *功能	获取车辆数据的回调函数
 *传入参数      handle	设备句柄
 *传出参数      pCarData	车数据信息结构体
 *返回值        0		成功
 *               -1		失败
 */
typedef int (WINAPI *VehRec_GetCarDataFun)(int ,SCarInfoResult *);

#endif // VEHCOMMONDEF_H
