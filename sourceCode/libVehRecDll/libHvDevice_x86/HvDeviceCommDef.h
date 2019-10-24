#ifndef _HVDEVICECOMMDEF_H
#define _HVDEVICECOMMDEF_H

//pic TYPE
#define IMAGE_TYPE_UNKNOWN                  0xffff0100
#define IMAGE_TYPE_JPEG_NORMAL            0xffff0101
#define IMAGE_TYPE_JPEG_CAPTURE          0xffff0102
#define IMAGE_TYPE_JPEG_LPR                    0xffff0103

//video type
#define VIDEO_TYPE_UNKNOWN                  0xffff0200
#define VIDEO_TYPE_H264_NORMAL_I          0xffff0201
#define VIDEO_TYPE_H264_NORMAL_P        0xffff0202
#define VIDEO_TYPE_H264_HISTORY_I        0xffff0203
#define VIDEO_TYPE_H264_HISTORY_P       0xffff0205
//#define VIDEO_TYPE_JPEG_HISTROY          0xffff0204
#define VIDEO_TYPE_JPEG_HISTORY         0xffff0204
//record type
#define RECORD_TYPE_UNKNOWN             0xffff0300
#define RECORD_TYPE_NORMAL                 0xffff0301
#define RECORD_TYPE_HISTORY                0xffff0302
#define RECORD_TYPE_STRING                  0xffff0303
#define RECORD_TYPE_INLEFT                    0xffff0304

//connect status
#define CONN_STATUS_UNKNOWN             0xffff0400
#define CONN_STATUS_NORMAL                 0xffff0401
#define CONN_STATUS_DISCONN               0xffff0402
#define CONN_STATUS_RECONN                 0xffff0403
#define CONN_STATUS_RECVDONE            0xffff0404

//以下两种状态为主动连接特有
#define CONN_STATUS_CONNFIRST           0xffff0405	/**< 初始连接*/
#define CONN_STATUS_CONNOVERTIME        0xffff0406	/**< 连接超时*/

/* 请求结果类型*/
#define RESULT_RECV_FLAG_REALTIME					0xffff0500  /**< 请求实时结果 */
#define RESULT_RECV_FLAG_HISTORY					0xffff0501  /**< 请求历史结果 */
#define RESULT_RECV_FLAG_HISTROY_ONLY_PECCANCY		0xffff0502  /**< 请求历史违章结果 */

/* 结果标志*/
#define RESULT_FLAG_INVAIL			0xffff0600  /**< 无效结果数据 */
#define RESULT_FLAG_VAIL			0xffff0601  /**< 有效结果数据 */
#define RESULT_FLAG_HISTROY_END		0xffff0602  /**< 历史结果结束标志 */

/* 请求H264类型*/
#define H264_RECV_FLAG_REALTIME				0xffff0700  /**< 请求实时视频 */
#define H264_RECV_FLAG_HISTORY				0xffff0701  /**< 请求历史视频 */

/* H264标志*/
#define H264_FLAG_INVAIL			0xffff0800  /**< 无效视频数据 */
#define H264_FLAG_VAIL				0xffff0801  /**< 有效视频数据 */
#define H264_FLAG_HISTROY_END		0xffff0802  /**< 历史结果结束标志 */

/* 请求MJPEG类型*/
#define MJPEG_RECV_FLAG_DEBUG				0xffff0900  /**< 接收调试码流 */
#define MJPEG_RECV_FLAG_REALTIME			0xffff0901  /**< 接收实时JPEG流*/

/* MJPEG标志*/
#define MJPEG_FLAG_INVAIL			0xffff0A00  /**< 无效MJPEG */
#define MJPEG_FLAG_VAIL				0xffff0A01  /**< 有效MJPEG帧 */

//device TYPE
#define DEV_TYPE_UNKNOWN		0x00000000 /**< 未知设备 */
#define DEV_TYPE_HVCAM_200W		0x00000001 /**< 200万一体机 */
#define DEV_TYPE_HVCAM_500W		0x00000002 /**< 500万一体机 */
#define DEV_TYPE_HVSIGLE		0x00000004 /**< 高清2.0识别器 */
#define DEV_TYPE_HVCAM_SINGLE	0x00000008 /**< 简化版一体机 */
#define DEV_TYPE_HVCAMERA       0x00000010 /**< 纯相机 */
#define DEV_TYPE_HVMERCURY		0x00000020  /**< 水星设备 */
#define DEV_TYPE_HVEARTH		0x00000040  /**< 地球设备 */
#define DEV_TYPE_HVVENUS		0x00000080  /**< 金星设备 */
#define DEV_TYPE_HVJUPITER		0x00000100  /**< 木星设备 */
#define DEV_TYPE_ITTS	        0x00000200  /**< ITTS设备 */
#define DEV_TYPE_SATURN	        0x00000400  /**< 土星设备 */
#define DEV_TYPE_VFR	        0x00000800  /**< 车型分类设备 */
#define DEV_TYPE_SATURN2        0x00001000  /**< 土星2设备 */
#define DEV_TYPE_ALL            0x0000ffff	/**< 所有设备 */

#define INVALID_SOCKET (int)(~0)


#define PLATE_RECT_BEST_SNAPSHOT           0x0001        /**< 最清晰识别图车牌坐标 */
#define PLATE_RECT_LAST_SNAPSHOT           0x0002        /**< 最后识别图车牌坐标 */
#define PLATE_RECT_BEGIN_CAPTURE           0x0003        /**< 开始抓拍图车牌坐标 */
#define PLATE_RECT_BEST_CAPTURE            0x0004        /**< 最清晰抓拍图车牌坐标 */
#define PLATE_RECT_LAST_CAPTURE            0x0005        /**< 最后抓拍图车牌坐标 */

#define FACE_RECT_BEST_SNAPSHOT            0x0006        /**< 最清晰识别图人脸信息 */
#define FACE_RECT_LAST_SNAPSHOT            0x0007        /**< 最后识别图人脸信息 */
#define FACE_RECT_BEGIN_CAPTURE            0x0008        /**< 开始抓拍图人脸信息 */
#define FACE_RECT_BEST_CAPTURE             0x0009        /**< 最清晰抓拍图人脸信息 */
#define FACE_RECT_LAST_CAPTURE             0x000A        /**< 最后抓拍图人脸信息 */

/* 通讯协议版本号 */
typedef enum _PROTOCOL_VERSION
{
	PROTOCOL_VERSION_1,
	PROTOCOL_VERSION_2,
	PROTOCOL_VERSION_MERCURY,
	PROTOCOL_VERSION_EARTH
} PROTOCOL_VERSION;




#endif // _HVDEVICECOMMDEF_H
