/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : pwm_script.h
* Version: V1.0
* By     : kevin.z
* Date   : 2009-5-26 9:09
*********************************************************************************************************
*/
#ifndef _PWRMAN_SCRIPT_H_
#define _PWRMAN_SCRIPT_H_
#include <typedef.h>

#define PWRMAN_SCRIPT_DEBUG     (0)
#define KEY_WORD_MAX_SIZE       (32)

#define TABLE           '\t'
#define SPACE           ' '
#define NEWLINE         '\n'
#define ENTER           '\r'
#define REMARK          ';'

typedef enum __PARS_SCRIPT_RESULT
{
    SCRIPT_PARSE_ERR_NONE   = 0,
    SCRIPT_PARSE_ERR_EOF    = 1,
} __pars_script_result_t;

typedef struct __PWRMAN_SCRIPT_FUNC_MAP
{
    char        keystr[KEY_WORD_MAX_SIZE];
    int32_t     (*func)(void);
} __pwrman_script_func_mapt_t;


typedef struct __PWRMAN_SCRIPT_PSR
{
    uint8_t     *buf_start;
    uint32_t    buf_size;
    uint8_t     *script_end;
    uint8_t     *cur_ptr;
    uint8_t     string[KEY_WORD_MAX_SIZE + 8];
} __pwrman_script_psr_t;


#endif  /* _PWRMAN_SCRIPT_H_ */



