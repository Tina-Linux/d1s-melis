/*
*******************************************************************************************************************
*                                                           Graphic Library
*                                                   the graphic library support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name:       config.h
*
*Author:          William Wang
*
*Version :        1.1.0
*
*Date:            2008-10-13
*
*Description :    ListMenu Framework config head file
*
*Others :         None at present.
*
* History :
*
* <Author>          <time>              <version>     <description>
*
* William Wang     2008-10-13    1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef ___CONFIG__H__
#define ___CONFIG__H__      1





//------------------------------------------------------------------------------------------------------------
// define constant
//------------------------------------------------------------------------------------------------------------
#ifndef OK
#define OK                  0
#endif //OK

#ifndef TRUE
#define TRUE                    1
#endif //OK

#ifndef FALSE
#define FALSE                   0
#endif //OK

#define TXT_WHITE       { 255, 255, 255, 255 }
#define TXT_BLACK       {   0,   0,   0, 255 }
#define TXT_RED         {   0,   0, 255, 255 }
#define TXT_GREEN       {   0, 255,   0, 255 }
#define TXT_BLUE        { 255,   0,   0, 255 }

//------------------------------------------------------------------------------------------------------------
// config marco
//------------------------------------------------------------------------------------------------------------

#define SCOTT_DEBUG 1       // open debug function

//#define DOUBLE_FRAMEBUFFER


//#define  TXTMENU_DEBUG
#ifndef SIM_PC_WIN
#ifdef  TXTMENU_DEBUG
#define TXTMENU_PRINTF    __inf
#else
#define TXTMENU_PRINTF(...)
#endif


//#define  SYSTEM_DEBUG

#ifdef  SYSTEM_DEBUG
#define SYSTEM_PRINTF    __inf
#else
#define SYSTEM_PRINTF(...)
#endif

//#define  ITEM_DEBUG

#ifdef  ITEM_DEBUG
#define ITEM_PRINTF    __inf
#else
#define ITEM_PRINTF(...)
#endif


//#define  POS_DEBUG

#ifdef  POS_DEBUG
#define POS_PRINTF    __inf
#else
#define POS_PRINTF(...)
#endif

#define __HERE__
#define Msg(...)
#define Err(...)
#ifndef NULL
#define NULL    0
#endif // NULL
#else

#ifdef  TXTMENU_DEBUG
#define TXTMENU_PRINTF    __inf
#else
#define TXTMENU_PRINTF   __inf
#endif


//#define  SYSTEM_DEBUG

#ifdef  SYSTEM_DEBUG
#define SYSTEM_PRINTF    __inf
#else
#define SYSTEM_PRINTF  __inf
#endif

//#define  ITEM_DEBUG

#ifdef  ITEM_DEBUG
#define ITEM_PRINTF    __inf
#else
#define ITEM_PRINTF   __inf
#endif


//#define  POS_DEBUG

#ifdef  POS_DEBUG
#define POS_PRINTF    __inf
#else
#define POS_PRINTF  __inf
#endif

#define __HERE__
#define Msg
#define Err
#ifndef NULL
#define NULL    0
#endif // NULL

#endif
//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //___CONFIG__H__

