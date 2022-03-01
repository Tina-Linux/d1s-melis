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

#define LIST_WHITE       { 255, 255, 255, 255 }
#define LIST_BLACK       {   0,   0,   0, 255 }
#define LIST_RED         {   0,   0, 255, 255 }
#define LIST_GREEN       {   0, 255,   0, 255 }
#define LIST_BLUE        { 255,   0,   0, 255 }

//------------------------------------------------------------------------------------------------------------
// config marco
//------------------------------------------------------------------------------------------------------------

#define SCOTT_DEBUG 1       // open debug function

//#define DOUBLE_FRAMEBUFFER


//#define  LISTMENU_DEBUG
#ifndef SIM_PC_WIN
#ifdef  LISTMENU_DEBUG
#define LISTMENU_PRINTF    __inf
#else
#define LISTMENU_PRINTF(...)
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
#define Msg(...)
#define Err(...)
#else//SIM_PC_WIN
#define LISTMENU_PRINTF __inf
#define SYSTEM_PRINTF __inf
#define ITEM_PRINTF __inf
#define POS_PRINTF __inf
#define Msg __inf
#define Err __inf
#endif
#define __HERE__
#ifndef NULL
#define NULL    0
#endif // NULL

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //___CONFIG__H__

