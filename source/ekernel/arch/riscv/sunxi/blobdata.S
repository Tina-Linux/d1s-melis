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
 *  Last Modified:  2020-08-04 09:21:42
 *
 * ===========================================================================================
 */
    .section .text.sbi
    .globl  blob_input_sbi_start
blob_input_sbi_start:
    .incbin CONFIG_SBI_PATH
    .globl  blob_input_sbi_end
blob_input_sbi_end:
    .end
