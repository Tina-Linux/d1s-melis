/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_syn_op.h
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
#ifndef  __robin_syn_op_h
#define  __robin_syn_op_h



typedef enum
{
    SYN_OP_RET_OK             = 0,
    SYN_OP_RET_SEND_CMD_ERR,
    SYN_OP_RET_TO_QUIT_ROBIN,
    SYN_OP_RET_CEDAR_FEEDBACK_ERR,
    SYN_OP_RET_OVERTIME,
    SYN_OP_RET_INT_FROM_NEW_FILE,

    SYN_OP_RET_ERR = -1
} syn_op_ret_e;



extern __s32  syn_op_pause(void);
extern __s32  syn_op_play(void);
extern __s32  syn_op_play_file(const char *file, const __cedar_tag_inf_t *tag_info_p);
extern __s32  syn_op_stop(void);
extern __s32  syn_op_ff(void);
extern __s32  syn_op_rr(void);
extern __s32  syn_op_jump(__u32 time);



#endif     //  ifndef __robin_syn_op_h

/* end of robin_syn_op.h */
