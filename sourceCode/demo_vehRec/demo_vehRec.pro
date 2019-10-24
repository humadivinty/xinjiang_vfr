TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    testfunc.cpp

HEADERS += \
    libvehrecdll.h \
    vehcommondef.h \
    testfunc.h
