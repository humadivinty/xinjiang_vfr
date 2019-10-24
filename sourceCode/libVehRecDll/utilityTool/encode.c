﻿#include "encode.h"
#include <stddef.h>
#include <iconv.h>
#include <assert.h>
#include<string.h>

#ifdef __cplusplus
extern "C"
{
#endif


int GB2312ToUTF8( char const *srcStr, char* desBuff, int desBuffLength)
{
    assert(strlen(srcStr)>0);
    unsigned int iLen = strlen(srcStr);

    iconv_t cd;
    cd = iconv_open("utf-8","gb2312");
    assert(cd != 0);
    int n=iconv(cd, (char **)&srcStr, &iLen, &desBuff, (size_t*)&desBuffLength);
    iconv_close(cd);
    return n;
}

int UTF8ToGB2312( char const *srcStr, char* desBuff, int desBuffLength)
{
    assert(strlen(srcStr)>0);
    unsigned int iLen = strlen(srcStr);

    iconv_t cd;
    cd = iconv_open("gb2312","utf-8");
    assert(cd != 0);
    int n=iconv(cd, (char **)&srcStr, (size_t*)&iLen, &desBuff, (size_t*)&desBuffLength);
    iconv_close(cd);
    return n;
}

#ifdef __cplusplus
}; //end of extern "C" {
#endif
