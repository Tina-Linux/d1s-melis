/*
 * =====================================================================================
 *
 *       Filename:  melis_hdr.h
 *
 *    Description:
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 15:34:43
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-05-06 10:25:26
 *
 * =====================================================================================
 */

#ifndef __melis_header_i__
#define __melis_header_i__

#include <boot.h>

#define HIGH_MEMORY_AND_L2_SIZE  (0x7400)
#define KERN_MEMORY_AND_L2_SIZE  (0xc00)

const boot_head_t *melis_head_para_get(void);

#endif /* __melis_header_i__  */

