#include <iostream>
#include<stdio.h>
#include"vehcommondef.h"
#include "testfunc.h"

using namespace std;

int main(int argc, char *argv[])
{

    bool bExit = false;
    while(!bExit)
    {
        printf_menu();
        printf("input a text:\n");
        char ch;
        scanf("%s", &ch);
        switch(ch)
        {
        case '1':
            SetLogPath();
            break;
        case '2':
            FreeResource();
            break;
        case '3':
            ConnectCamera();
            break;
        case '4':
            DisConnectCamera();
            break;
        case '5':
            SendCarMotionSignal();
            break;
        case '6':
            SetPlateCallback();
            break;
        case '7':
            SetCarInfoCallback();
            break;
        case 'q':
        case 'Q':
            FreeResource();
            bExit = true;
            break;
        default:
            continue;
            break;
        }
    }

    return 0;
}
