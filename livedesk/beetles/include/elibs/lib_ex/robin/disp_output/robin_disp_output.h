/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_disp_output.h
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
#ifndef  __robin_disp_output_h
#define  __robin_disp_output_h

/*
typedef enum
{
    DISP_OUTPUT_TYPE_NONE   = 0,
    DISP_OUTPUT_TYPE_LCD    = 1,
    DISP_OUTPUT_TYPE_TV     = 2,
    DISP_OUTPUT_TYPE_HDMI   = 4,
    DISP_OUTPUT_TYPE_VGA    = 8,
    DISP_OUTPUT_TYPE_LCD_TV  = 16,
} __disp_output_type_t;

*/
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
//__s32  robin_get_screen_size(__u32 *width_p, __u32 *height_p);

/***************************************************************************************************
*Name        : robin_get_disp_output_type
*Prototype   : __disp_output_type_t  robin_get_disp_output_type( void )
*Arguments   : void
*Return      : the current display output type, such as tv, lcd and so on.
*Description : get the current display output type.
*Other       :
***************************************************************************************************/
//__disp_output_type_t  robin_get_disp_output_type(void);


#endif     //  ifndef __robin_disp_output_h

/* end of robin_disp_output.h */
