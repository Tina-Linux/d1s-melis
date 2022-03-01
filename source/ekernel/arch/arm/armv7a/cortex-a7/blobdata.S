/*
 * ===========================================================================================
 *
 *       Filename:  blobdata.S
 *
 *    Description:  include blob data to image.
 *
 *        Version:  Melis3.0 
 *         Create:  2019-11-21 18:03:56
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-12-10 11:32:51
 *
 * ===========================================================================================
 */
#include <kconfig.h>
    .section .blobdata,#alloc
    .globl  blob_input_ramdisk_start
blob_input_ramdisk_start:
/*    .incbin "workspace/suniv/ramfs/ramdisk.img"*/
    .globl  blob_input_ramdisk_end
blob_input_ramdisk_end: 

#ifdef CONFIG_OF
    .globl  blob_input_dtb_start
blob_input_dtb_start:
    .incbin "workspace/suniv/ramfs/sunxi-v459.dtb"
    .globl  blob_input_dtb_end
blob_input_dtb_end: 
#endif
    .end
