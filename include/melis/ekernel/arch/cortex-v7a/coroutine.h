/*
 * ===========================================================================================
 *
 *       Filename:  coroutine.h
 *
 *    Description:  coroutine header file
 *
 *        Version:  Melis3.0
 *         Create:  2019-01-30 11:41:02
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-01-30 13:21:17
 *
 * ===========================================================================================
 */
#ifndef __MELIS_COROUTINE__
#define __MELIS_COROUTINE__
#include <stdint.h>

struct context_from_t
{
    void *fctx;
    void *priv;
};

void *make_fcontext(void *stack, size_t size, void (*func)(struct context_from_t));
struct context_from_t jump_fcontext(void *fctx, void *priv);

#endif
