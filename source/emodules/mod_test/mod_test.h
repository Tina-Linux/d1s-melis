/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Flash Module
*
*                                    (c) Copyright 2010-2014, wfsun China
*                                             All Rights Reserved
*
* File    : mod_slib_i.h
* By      : wfsun
* Version : v1.0
* Date    : 2011-1-18
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef __MOD_SLIB_I_H__
#define __MOD_SLIB_I_H__

#include <typedef.h>
#include <mod_defs.h>

int32_t ftest_MInit(void);
int32_t ftest_MExit(void);
__mp *ftest_MOpen(uint32_t mid, uint32_t mode);
int32_t ftest_MClose(__mp *mp);
uint32_t ftest_MRead(void *pdata, uint32_t size, uint32_t n, __mp *mp);
uint32_t ftest_MWrite(const void *pdata, uint32_t size, uint32_t n, __mp *mp);
int32_t ftest_MIoctrl(__mp *mp, uint32_t cmd, int32_t aux, void *pbuffer);

#endif  //__MOD_SLIB_I_H__

