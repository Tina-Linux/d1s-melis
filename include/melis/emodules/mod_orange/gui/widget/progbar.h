/*
************************************************************************************************************************
*                                                         Progbar

*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : progbar.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.10.09
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
* Gary.Wang      2009.10.09       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __progbar_h
#define  __progbar_h


/* control class name */
#define CTRL_PROGBAR                     "CTRL_PROGBAR"


/* progbar style */
#define PGBS_VERTICAL                    ( 1 << 16 )


typedef enum
{
    PROGBAR_TEXT_TYPE_NONE = 0,         // don't show text
    PROGBAR_TEXT_TYPE_PERCENT,          // current percent
    PROGBAR_TEXT_TYPE_VALUE,            // current value

    PROGBAR_TEXT_TYPE_UNKNOWN = -1
} progbar_text_type_e;


/* attr for a progbar */
typedef struct tag_progbar_para_t
{
    void *head_bmp_buf;           // can be free after control is created
    void *tail_bmp_buf;           // can be free after control is created
    void *frame_bmp_buf;          // can be free after control is created
    __s32 min;
    __s32 max;
    __s32 value;
    progbar_text_type_e text_type;
    __u32 text_color;
    GUI_FONT *font;               // can be NULL. NULL means showing no text.
} progbar_para_t;



extern __bool RegisterProgbarControl(void);

extern __s32  PROGBAR_SetRange(H_WIN hwin, __s32 min, __s32 max);
extern __s32  PROGBAR_SetValue(H_WIN hwin, __s32 value);
extern __s32  PROGBAR_GetMin(H_WIN hwin);
extern __s32  PROGBAR_GetMax(H_WIN hwin);
extern __s32  PROGBAR_GetValue(H_WIN hwin);



#endif     //  ifndef __progbar_h

/* end of progbar.h */
