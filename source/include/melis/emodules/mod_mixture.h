/*
 * =====================================================================================
 *
 *       Filename:  mod_mixture.h
 *
 *    Description:  support logo, animation, fast catback function and so on
 *
 *        Version:  1.0 
 *         Create:  2020-03-11 15:46:44
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  huangyixiu@allwinnertech.com
 *   Organization:  PDC-PD1
 *  Last Modified:  2020-03-11 15:46:44
 *
 * =====================================================================================
 */

#ifndef _MOD_MIXTURE_H_
#define _MOD_MIXTURE_H_

typedef enum
{
	STARTUP = 0,
	SHUTDOWN,
	TYPE_MAX,
}logo_execute;

typedef enum __MOD_MIXTURE_CMD
{
	MOD_MIXTURE_START = 0x0,
    MOD_MIXTURE_STOP,
    MOD_MIXTURE_QURY_FINISH,
    MOD_MIXTURE_UNKNOWN = 0xFFFF,
} __mod_mixture_cmd_set_t;

#endif

