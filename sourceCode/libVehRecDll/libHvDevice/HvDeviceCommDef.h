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

//��������״̬Ϊ������������
#define CONN_STATUS_CONNFIRST           0xffff0405	/**< ��ʼ����*/
#define CONN_STATUS_CONNOVERTIME        0xffff0406	/**< ���ӳ�ʱ*/

/* ����������*/
#define RESULT_RECV_FLAG_REALTIME					0xffff0500  /**< ����ʵʱ��� */
#define RESULT_RECV_FLAG_HISTORY					0xffff0501  /**< ������ʷ��� */
#define RESULT_RECV_FLAG_HISTROY_ONLY_PECCANCY		0xffff0502  /**< ������ʷΥ�½�� */

/* �����־*/
#define RESULT_FLAG_INVAIL			0xffff0600  /**< ��Ч������� */
#define RESULT_FLAG_VAIL			0xffff0601  /**< ��Ч������� */
#define RESULT_FLAG_HISTROY_END		0xffff0602  /**< ��ʷ���������־ */

/* ����H264����*/
#define H264_RECV_FLAG_REALTIME				0xffff0700  /**< ����ʵʱ��Ƶ */
#define H264_RECV_FLAG_HISTORY				0xffff0701  /**< ������ʷ��Ƶ */

/* H264��־*/
#define H264_FLAG_INVAIL			0xffff0800  /**< ��Ч��Ƶ���� */
#define H264_FLAG_VAIL				0xffff0801  /**< ��Ч��Ƶ���� */
#define H264_FLAG_HISTROY_END		0xffff0802  /**< ��ʷ���������־ */

/* ����MJPEG����*/
#define MJPEG_RECV_FLAG_DEBUG				0xffff0900  /**< ���յ������� */
#define MJPEG_RECV_FLAG_REALTIME			0xffff0901  /**< ����ʵʱJPEG��*/

/* MJPEG��־*/
#define MJPEG_FLAG_INVAIL			0xffff0A00  /**< ��ЧMJPEG */
#define MJPEG_FLAG_VAIL				0xffff0A01  /**< ��ЧMJPEG֡ */

//device TYPE
#define DEV_TYPE_UNKNOWN		0x00000000 /**< δ֪�豸 */
#define DEV_TYPE_HVCAM_200W		0x00000001 /**< 200��һ��� */
#define DEV_TYPE_HVCAM_500W		0x00000002 /**< 500��һ��� */
#define DEV_TYPE_HVSIGLE		0x00000004 /**< ����2.0ʶ���� */
#define DEV_TYPE_HVCAM_SINGLE	0x00000008 /**< �򻯰�һ��� */
#define DEV_TYPE_HVCAMERA       0x00000010 /**< ����� */
#define DEV_TYPE_HVMERCURY		0x00000020  /**< ˮ���豸 */
#define DEV_TYPE_HVEARTH		0x00000040  /**< �����豸 */
#define DEV_TYPE_HVVENUS		0x00000080  /**< �����豸 */
#define DEV_TYPE_HVJUPITER		0x00000100  /**< ľ���豸 */
#define DEV_TYPE_ITTS	        0x00000200  /**< ITTS�豸 */
#define DEV_TYPE_SATURN	        0x00000400  /**< �����豸 */
#define DEV_TYPE_VFR	        0x00000800  /**< ���ͷ����豸 */
#define DEV_TYPE_SATURN2        0x00001000  /**< ����2�豸 */
#define DEV_TYPE_ALL            0x0000ffff	/**< �����豸 */

#define INVALID_SOCKET (int)(~0)


#define PLATE_RECT_BEST_SNAPSHOT           0x0001        /**< ������ʶ��ͼ�������� */
#define PLATE_RECT_LAST_SNAPSHOT           0x0002        /**< ���ʶ��ͼ�������� */
#define PLATE_RECT_BEGIN_CAPTURE           0x0003        /**< ��ʼץ��ͼ�������� */
#define PLATE_RECT_BEST_CAPTURE            0x0004        /**< ������ץ��ͼ�������� */
#define PLATE_RECT_LAST_CAPTURE            0x0005        /**< ���ץ��ͼ�������� */

#define FACE_RECT_BEST_SNAPSHOT            0x0006        /**< ������ʶ��ͼ������Ϣ */
#define FACE_RECT_LAST_SNAPSHOT            0x0007        /**< ���ʶ��ͼ������Ϣ */
#define FACE_RECT_BEGIN_CAPTURE            0x0008        /**< ��ʼץ��ͼ������Ϣ */
#define FACE_RECT_BEST_CAPTURE             0x0009        /**< ������ץ��ͼ������Ϣ */
#define FACE_RECT_LAST_CAPTURE             0x000A        /**< ���ץ��ͼ������Ϣ */

/* ͨѶЭ��汾�� */
typedef enum _PROTOCOL_VERSION
{
	PROTOCOL_VERSION_1,
	PROTOCOL_VERSION_2,
	PROTOCOL_VERSION_MERCURY,
	PROTOCOL_VERSION_EARTH
} PROTOCOL_VERSION;




#endif // _HVDEVICECOMMDEF_H
