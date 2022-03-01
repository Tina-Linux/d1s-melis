/*
 * ===========================================================================================
 *
 *       Filename:  barrier.h
 *
 *    Description:  barry definition for memory access.
 *
 *        Version:  Melis3.0
 *         Create:  2019-01-08 15:51:34
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-10-28 17:36:08
 *
 * ===========================================================================================
 */

#ifndef __BARRIER_H_
#define __BARRIER_H_

#define isb(option)             __asm__ __volatile__ ("isb " #option : : : "memory")
#define dsb(option)             __asm__ __volatile__ ("dsb " #option : : : "memory")
#define dmb(option)             __asm__ __volatile__ ("dmb " #option : : : "memory")

#define barrier()               __asm__ __volatile__("": : :"memory")

#endif
