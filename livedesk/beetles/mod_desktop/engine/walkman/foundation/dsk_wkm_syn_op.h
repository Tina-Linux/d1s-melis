/*
************************************************************************************************************************
*                                                        dsk_walkman
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_syn_op.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.22
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
* Gary.Wang      2009.11.22       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  ____dsk_wkm_syn_op_h
#define  ____dsk_wkm_syn_op_h



typedef enum
{
    SYN_OP_RET_OK             = 0,
    SYN_OP_RET_SEND_CMD_ERR,
    SYN_OP_RET_TO_QUIT_DSK_WKM,
    SYN_OP_RET_CEDAR_FEEDBACK_ERR,
    SYN_OP_RET_OVERTIME,
    SYN_OP_RET_INT_FROM_NEW_FILE,

    SYN_OP_RET_ERR = -1
} syn_op_ret_e;



extern syn_op_ret_e  __dsk_wkm_syn_op_play_file(const char *file);
extern syn_op_ret_e  __dsk_wkm_syn_op_stop(void);
extern syn_op_ret_e  __dsk_wkm_syn_op_pause(void);
extern syn_op_ret_e  __dsk_wkm_syn_op_resume_play(void);
extern syn_op_ret_e  __dsk_wkm_syn_op_ff(void);
extern syn_op_ret_e  __dsk_wkm_syn_op_rr(void);
extern syn_op_ret_e  __dsk_wkm_syn_op_jump(__u32 time);



#endif     //  ifndef ____dsk_wkm_syn_op_h

/* end of dsk_wkm_syn_op.h */
