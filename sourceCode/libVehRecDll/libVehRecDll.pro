#-------------------------------------------------
#
# Project created by QtCreator 2019-10-16T09:46:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = libVehRecDll
TEMPLATE = lib

DEFINES += LIBVEHRECDLL_LIBRARY

SOURCES += libvehrecdll.cpp \
    cameraModule/BaseCamera.cpp \
    cameraModule/Camera6467_plate.cpp \
    cameraModule/Camera6467_VFR.cpp \
    cameraModule/CameraResult.cpp \
    cameraModule/ResultListManager.cpp \
    tinyxml/tinystr.cpp \
    tinyxml/tinyxml.cpp \
    tinyxml/tinyxmlerror.cpp \
    tinyxml/tinyxmlparser.cpp \
    utilityTool/tool_function.cpp \
    utilityTool/encode.c \
    utilityTool/inifile.c \
    c_udpclient.cpp \
    libAVI/cAviLib.cpp \
    libAVI/CusH264Struct.cpp \
    libAVI/MyH264Saver.cpp \
    libAVI/avilib.c \
    cameraModule/DeviceListManager.cpp \
    cameraModule/BaseCamera.cpp \
    cameraModule/Camera6467_plate.cpp \
    cameraModule/Camera6467_VFR.cpp \
    cameraModule/CameraResult.cpp \
    cameraModule/DeviceListManager.cpp \
    cameraModule/ResultListManager.cpp \
    libMp4/cMp4Encode.cpp \
    libAVI/MyH264Vector.cpp

HEADERS += libvehrecdll.h\
        libvehrecdll_global.h \
    vehcommondef.h \
    cameraModule/BaseCamera.h \
    cameraModule/Camera6467_plate.h \
    cameraModule/Camera6467_VFR.h \
    cameraModule/CameraResult.h \
    cameraModule/ResultListManager.h \
    libHvDevice/HVAPI_HANDLE_CONTEXT_EX.h \
    libHvDevice/HvDeviceBaseType.h \
    libHvDevice/HvDeviceCommDef.h \
    libHvDevice/HvDeviceEx.h \
    libHvDevice/swerror.h \
    tinyxml/tinystr.h \
    tinyxml/tinyxml.h \
    utilityTool/encode.h \
    utilityTool/inifile.h \
    utilityTool/tool_function.h \
    c_udpclient.h \
    libAVI/avilib.h \
    libAVI/cAviLib.h \
    libAVI/CusH264Struct.h \
    libAVI/MyH264Saver.h \
    cameraModule/DeviceListManager.h \
    cameraModule/ThreadSafeList.h \
    cameraModule/BaseCamera.h \
    cameraModule/Camera6467_plate.h \
    cameraModule/Camera6467_VFR.h \
    cameraModule/CameraResult.h \
    cameraModule/DeviceListManager.h \
    cameraModule/ResultListManager.h \
    cameraModule/ThreadSafeList.h \
    libMp4/include/mp4v2/chapter.h \
    libMp4/include/mp4v2/file.h \
    libMp4/include/mp4v2/file_prop.h \
    libMp4/include/mp4v2/general.h \
    libMp4/include/mp4v2/isma.h \
    libMp4/include/mp4v2/itmf_generic.h \
    libMp4/include/mp4v2/itmf_tags.h \
    libMp4/include/mp4v2/mp4v2.h \
    libMp4/include/mp4v2/platform.h \
    libMp4/include/mp4v2/project.h \
    libMp4/include/mp4v2/project.h.in \
    libMp4/include/mp4v2/sample.h \
    libMp4/include/mp4v2/streaming.h \
    libMp4/include/mp4v2/track.h \
    libMp4/include/mp4v2/track_prop.h \
    libMp4/cMp4Encode.h \
    libAVI/MyH264Vector.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    libHvDevice/HvDevice.lib \
    libHvDevice/libHvdevice.so
