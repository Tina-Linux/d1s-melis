/*
 * =====================================================================================
 *
 *       Filename:  loader.h
 *
 *    Description:  loader.c header file.
 *
 *        Version:  Melis3.0
 *         Create:  2018-03-05 16:44:11
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-09-28 10:06:34
 *
 * =====================================================================================
 */

#ifndef __LOADER_I_H__
#define __LOADER_I_H__
#include <typedef.h>
#include <rtthread.h>
#include <common/romldr.h>

//open elf file structure
typedef struct __OPEN_ELF
{
    __hdle          hFile;          //file handler
    rt_bool_t       IsMFSROMELF;    //this elf file is store in minfs rom
    rt_uint32_t     FileType;       //this loader able file type
    rt_uint32_t     shoff;          //section header offset
    rt_uint32_t     MagicIdx;       //index of magic section
    rt_uint32_t     SecNum;         //the number of sectiions
} open_elf_t;

//common magic section structure,
//this structure size must be larger
//then __module_mgsec_t and __exec_mgsec_t.
//by sunny at 2011-4-8 13:47:29.
typedef struct __MAGIC_COMMON
{
    char    magic[8];
    char    reserved[56];
} __magic_common_t;

//legal load able file types of melis
#define LDR_INVALID_FILE    0
#define LDR_PROCESS_FILE    1
#define LDR_MODULE_FILE     2

//invalid index value
#define LDR_INVALID_INDEX   0xffffffff

__hdle  LDR_LoadFile(const char *filepath);
rt_int32_t   LDR_UnloadFile(__hdle hLDR);
rt_uint32_t  LDR_GetFileType(__hdle hLDR);
rt_uint32_t  LDR_GetMagicIndex(__hdle hLDR);
rt_uint32_t  LDR_GetSecNumber(__hdle hLDR);
rt_int32_t   LDR_GetSecData(__hdle hLDR, rt_uint32_t Index, void *pData);
rt_int32_t   LDR_GetSecROMHdr(__hdle hFile, rt_uint32_t Index, __section_rom_hdr_t *pHdr);

#endif  //__LOADER_I_H__
