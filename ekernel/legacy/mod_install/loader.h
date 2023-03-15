/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __LOADER_I_H__
#define __LOADER_I_H__

#include <stdbool.h>

#include <typedef.h>
#include <rtthread.h>
#include <common/romldr.h>

//open elf file structure
typedef struct __OPEN_ELF
{
    __hdle          hFile;          //file handler
    bool         IsMFSROMELF;    //this elf file is store in minfs rom
    uint32_t     FileType;       //this loader able file type
    uint32_t     shoff;          //section header offset
    uint32_t     MagicIdx;       //index of magic section
    uint32_t     SecNum;         //the number of sectiions
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
int32_t   LDR_UnloadFile(__hdle hLDR);
uint32_t  LDR_GetFileType(__hdle hLDR);
uint32_t  LDR_GetMagicIndex(__hdle hLDR);
uint32_t  LDR_GetSecNumber(__hdle hLDR);
int32_t   LDR_GetSecData(__hdle hLDR, uint32_t Index, void *pData);
int32_t   LDR_GetSecROMHdr(__hdle hFile, uint32_t Index, __section_rom_hdr_t *pHdr);

#endif  //__LOADER_I_H__
