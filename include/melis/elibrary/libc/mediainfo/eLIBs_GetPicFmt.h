/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2007-2009, SoftWinners,
*                                               All Rights Reserved
*
* File   : eLIBs_GetPicFmt.h
* Version: V1.0
* By     :
* Modify : kevin.z.m, 2009-11-26 19:45
*********************************************************************************************************
*/
#ifndef _ELIBS_GET_PIC_FORMAT_H_
#define _ELIBS_GET_PIC_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __PIC_FORMAT_INFO
{
    int  temp;
} __pic_format_info_t;



/*
************************************************************************************************************************
*                       GET PICTURE FORMAT
*
*Description: get picture format.
*
*Arguments  : file      full path of picture file.
*             format    file format information.
*
*Return     : result.
*               1:get picture format succssed.
*               0:get picture format failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetPicFormat(const char *file, int *format);


/*
************************************************************************************************************************
*                       GET PICTURE INFORMATION
*
*Description: get picture information.
*
*Arguments  : file      full path of picture file.
*             format    file information.
*
*Return     : result.
*               1:get picture information succssed.
*               0:get picture information failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetPicInfo(const char *file, __pic_format_info_t *inf);

#ifdef __cplusplus
}
#endif

#endif //_ELIBS_GET_PIC_FORMAT_H_
