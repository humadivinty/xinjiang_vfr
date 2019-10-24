#ifndef TESTFUNC_H
#define TESTFUNC_H

void  printf_menu();

void SetLogPath();
void FreeResource();
void ConnectCamera();
void DisConnectCamera();
void SendCarMotionSignal();
void SetPlateCallback();
void SetCarInfoCallback();
bool Tool_MyMakeDir( const char* chImgPath);
#endif // TESTFUNC_H
