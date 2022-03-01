/*
************************************************************************************************************************
*                                                     display output
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_display_output.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.02
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
* Gary.Wang      2010.09.02        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_DISPLAY_OUTPUT_H__
#define  __DSK_DISPLAY_OUTPUT_H__
#include <kconfig.h>
#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_display_get_format
*Prototype   : __lion_disp_format_e  dsk_display_get_format( void )
*Arguments   : void
*Return      : the current display format.
*Description : get the current display format.
*Other       :
****************************************************************************************************
*/
extern __lion_disp_format_e  dsk_display_get_format(void);

/*
****************************************************************************************************
*Name        : dsk_display_set_format
*Prototype   : __s32 dsk_display_set_format( __lion_disp_format_e type );
*Arguments   : type       input. display format.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : switch the display format.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_set_format(__lion_disp_format_e type);

/*
****************************************************************************************************
*Name        : dsk_display_get_output_type
*Prototype   : __u32  dsk_display_get_output_type( void )
*Arguments   : void
*Return      : the current display output type: TV, HDMI, and so on.
*Description : get the current display output type.
*Other       :
****************************************************************************************************
*/
extern __u32  dsk_display_get_output_type(void);


/*
****************************************************************************************************
*Name        : dsk_display_dskfmt2epdkfmt
*Prototype   : __s32  dsk_display_dskfmt2epdkfmt( __lion_disp_format_e format, __disp_output_type_t *output_p )
*Arguments   : format   input. the output format defined in desktop.
*              output_p output. the output type defined in ePDK.
*Return      : the format corresponding to the output type defined in ePDK.
*Description : convert the output format defined in desktop to the output format defined in ePDK.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_display_dskfmt2epdkfmt(__lion_disp_format_e format, enum disp_output_type *output_p);

/*
****************************************************************************************************
*Name        : dsk_display_epdkfmt2dskfmt
*Prototype   : __lion_disp_format_e  dsk_display_epdkfmt2dskfmt( __disp_output_type_t output, __s32 format );
*Arguments   : output   input. the output type defined in ePDK.
*              format   input. the format corresponding to the output type defined in ePDK. if the
*                       output type is LCD, this parameter may be ignored.
*Return      :
*Description : convert the output format defined in ePDK to the output format defined in desktop.
*Other       :
****************************************************************************************************
*/
extern __lion_disp_format_e  dsk_display_epdkfmt2dskfmt(enum disp_output_type output, __s32 format);

/*
****************************************************************************************************
*Name        : dsk_display_on
*Prototype   : __s32  dsk_display_on( __disp_output_type_t output )
*Arguments   : output     input. the display output type.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : open the display output.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_display_on(enum disp_output_type output);

/*
****************************************************************************************************
*Name        : dsk_display_off
*Prototype   : __s32  dsk_display_off( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : close the current display output.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_display_off(void);




#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_OUTPUT_H__

/* end of dsk_display_output.h */
