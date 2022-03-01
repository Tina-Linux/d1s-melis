/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_disp_output.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.10.10
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
* Gary.Wang      2009.10.10       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_disp_output_c
#define  __robin_disp_output_c

#include <emodules/mod_display.h>
#include "robin_i.h"


/***************************************************************************************************
*Name        : robin_get_screen_size
*Prototype   : __s32  robin_get_screen_size( __u32 *width_p, __u32 *height_p )
*Arguments   : width_p   output. width.
*              height_p  output. height.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the width and height of the screen.
*Other       :
***************************************************************************************************/
__s32  robin_get_screen_size(__u32 *width_p, __u32 *height_p)
{
    if (width_p == NULL || height_p == NULL)
    {
        return -1;
    }

    *width_p  = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    *height_p = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    return 0;
}



/***************************************************************************************************
*Name        : robin_get_disp_output_type
*Prototype   : __disp_output_type_t  robin_get_disp_output_type( void )
*Arguments   : void
*Return      : the current display output type, such as tv, lcd and so on.
*Description : get the current display output type.
*Other       :
***************************************************************************************************/
enum disp_output_type  robin_get_disp_output_type(void)
{
    return (enum disp_output_type)g_fioctrl(robin_hdis, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);
}



#endif     //  ifndef __robin_disp_output_c

/* end of robin_disp_output.c */
