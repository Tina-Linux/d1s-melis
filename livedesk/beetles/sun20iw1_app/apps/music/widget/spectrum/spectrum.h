/*
************************************************************************************************************************
*                                                         MUSIC
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : spectrum.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.02
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.11.02       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __spectrum_h
#define  __spectrum_h


/* control class name */
#define CTRL_SPECTRUM                   "CTRL_SPECTRUM"


/* spectrum style */


/* spectrum notify message */
typedef enum
{
    STN_KEY = 0,
    STN_
} stn_e;


/* attr for a spectrum */
typedef struct tag_spectrum_para_t
{
    __s32  min; //0
    __s32  max; //16
    void  *particle_bmp_buf;
    __u32  particle_nr;
    void *buoy_bmp_buf;
} spectrum_para_t;



extern __bool RegisterSpectrumControl(void);
extern __bool UnRegisterSpectrumControl(void);
extern __s32  SPECTRUM_SetValue(H_WIN hwin, __s32 *value_array);


#endif     //  ifndef __spectrum_h

/* end of spectrum.h */
