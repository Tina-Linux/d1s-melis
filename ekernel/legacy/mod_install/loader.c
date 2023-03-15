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
#include "loader.h"
#include "eelf.h"
#include <sys_fsys.h>
#include <log.h>
#include <dfs_posix.h>

#include <hal_mem.h>

extern int             fioctrl(FILE* fp, int Cmd, void *args);

/*
*********************************************************************************************************
*                                       LOAD STARDARD ELF FILE
*
* Description: load stardard elf format file
*
* Arguments  : hFile    :   handler of elf file
*
* Returns    : handler of open elf file if succeeded, NULL if failed.
*
* Note       :
*********************************************************************************************************
*/
open_elf_t *LDR_LoadELFFile(__hdle hFile)
{
    open_elf_t       *pOPENELF;
    __elf32_head_t    ELFHdr;       //52 byte
    __elf32_shead_t   SecHdr;       //40 byte
    __magic_common_t  Magic;        //64 byte
    uint32_t       MagicIdx;
    uint32_t       Offset;
    uint32_t       FileType;
    uint32_t       Idx;
    char             *shstrtbl;

    memset(&ELFHdr, 0x00, sizeof(__elf32_head_t));
    memset(&SecHdr, 0x00, sizeof(__elf32_shead_t));
    memset(&Magic,  0x00, sizeof(__magic_common_t));

    //seek to file begin
    //esFSYS_fseek(hFile, 0, SEEK_SET);
    fseek(hFile, 0, SEEK_SET);
    //esFSYS_fread(&ELFHdr, sizeof(__elf32_head_t), 1, hFile);
    fread(&ELFHdr, sizeof(__elf32_head_t), 1, hFile);
    if ((ELFHdr.ident[EI_MAG0] != ELFMAG0) ||
        (ELFHdr.ident[EI_MAG1] != ELFMAG1) ||
        (ELFHdr.ident[EI_MAG2] != ELFMAG2) ||
        (ELFHdr.ident[EI_MAG3] != ELFMAG3))
    {
        __err("invalid elf file to loader!");
        return NULL;
    }

    //load string table section
    Offset = ELFHdr.shoff + ELFHdr.shentsize * ELFHdr.shstrndx;
    //esFSYS_fseek(hFile, Offset, SEEK_SET);
    fseek(hFile, Offset, SEEK_SET);
    //esFSYS_fread(&SecHdr, ELFHdr.shentsize, 1, hFile);
    fread(&SecHdr, ELFHdr.shentsize, 1, hFile);
    shstrtbl = hal_malloc(SecHdr.size);
    if (shstrtbl == NULL)
    {
        __err("allocate buffer for elf file string table failed");
        return NULL;
    }

    //load string table
    //esFSYS_fseek(hFile, SecHdr.offset, SEEK_SET);
    fseek(hFile, SecHdr.offset, SEEK_SET);
    //esFSYS_fread((void *)shstrtbl, SecHdr.size, 1, hFile);
    fread((void *)shstrtbl, SecHdr.size, 1, hFile);

    //find magic section
    MagicIdx = LDR_INVALID_INDEX;
    FileType = LDR_INVALID_FILE;
    for (Idx = 0; Idx < ELFHdr.shnum; Idx++)
    {
        Offset = ELFHdr.shoff + ELFHdr.shentsize * Idx;
        //esFSYS_fseek(hFile, Offset, SEEK_SET);
        fseek(hFile, Offset, SEEK_SET);
        //esFSYS_fread(&SecHdr, ELFHdr.shentsize, 1, hFile);
        fread(&SecHdr, ELFHdr.shentsize, 1, hFile);
        if (strncmp(&shstrtbl[SecHdr.name], "MAGIC", 5) == 0)
        {
            //esFSYS_fseek(hFile, SecHdr.offset, SEEK_SET);
            //esFSYS_fread(&Magic, sizeof(Magic), 1, hFile);
            fseek(hFile, SecHdr.offset, SEEK_SET);
			fread(&Magic, sizeof(Magic), 1, hFile);
            if (strncmp(Magic.magic, "ePDK.mod", 8) == 0)
            {
                //module file
                FileType = LDR_MODULE_FILE;
            }
            else if (strncmp(Magic.magic, "ePDK.exf", 8) == 0)
            {
                //process file
                FileType = LDR_PROCESS_FILE;
            }
            //magic section found
            MagicIdx = Idx;
            break;
        }
    }

    //free string table
    hal_free(shstrtbl);
    if (MagicIdx == LDR_INVALID_INDEX || FileType == LDR_INVALID_FILE)
    {
        __err("invalid elf file to loader");
        return NULL;
    }

    //valid standard elf file for melis system
    pOPENELF = hal_malloc(sizeof(open_elf_t));
    if (pOPENELF == NULL)
    {
        __err("allocate memory for open elf file failed");
        return NULL;
    }

    //initialize open elf file handler
    pOPENELF->IsMFSROMELF = 0;
    pOPENELF->FileType    = FileType;
    pOPENELF->hFile       = hFile;
    pOPENELF->shoff       = ELFHdr.shoff;
    pOPENELF->MagicIdx    = MagicIdx;
    pOPENELF->SecNum      = ELFHdr.shnum;

    //load stardard elf file succeeded
    return pOPENELF;
}

/*
*********************************************************************************************************
*                                    GET ELF FILE SECTION ROM HEADER
*
* Description: get elf file section rom header
*
* Arguments  : pOPENELF : open elf file handler
*              Index    : the index of target section
*              pROMHDR  : pointer of rom header
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed
*
* Note       :
*********************************************************************************************************
*/
int32_t LDR_GetELFFileSecROMHdr(open_elf_t *pOPENELF, uint32_t Index, __section_rom_hdr_t *pROMHdr)
{
    __elf32_shead_t     SecHdr;   //40 byte
    uint32_t         Offset;

    //load section header
    Offset = pOPENELF->shoff + Index * sizeof(__elf32_shead_t);
    //esFSYS_fseek(pOPENELF->hFile, Offset, SEEK_SET);
    //esFSYS_fread(&SecHdr, sizeof(__elf32_shead_t), 1, pOPENELF->hFile);
    fseek(pOPENELF->hFile, Offset, SEEK_SET);
	fread(&SecHdr, sizeof(__elf32_shead_t), 1, pOPENELF->hFile);

    //initialize section rom header
    pROMHdr->Size  = SecHdr.size;
    pROMHdr->VAddr = SecHdr.addr;
    pROMHdr->Type  = SecHdr.type;
    pROMHdr->Flags = SecHdr.flags;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                    GET ELF FILE SECTION ROM HEADER
*
* Description: load elf file section rom header
*
* Arguments  : pOPENELF : open elf file handler
*              Index    : the index of target section
*              pROMHDR  : pointer of rom header
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed
*
* Note       :
*********************************************************************************************************
*/
int32_t LDR_GetELFFileSecData(open_elf_t *pOPENELF, uint32_t Index, void *pData)
{
    __elf32_shead_t     SecHdr;   //40 byte
    uint32_t         Offset;

    //load section header
    Offset = pOPENELF->shoff + Index * sizeof(__elf32_shead_t);
    //esFSYS_fseek(pOPENELF->hFile, Offset, SEEK_SET);
    //esFSYS_fread(&SecHdr, sizeof(__elf32_shead_t), 1, pOPENELF->hFile);
    fseek(pOPENELF->hFile, Offset, SEEK_SET);
	fread(&SecHdr, sizeof(__elf32_shead_t), 1, pOPENELF->hFile);

    //load section data
    //esFSYS_fseek(pOPENELF->hFile, SecHdr.offset, SEEK_SET);
    //if (esFSYS_fread(pData, 1, SecHdr.size, pOPENELF->hFile) != SecHdr.size)
    fseek(pOPENELF->hFile, SecHdr.offset, SEEK_SET);
	if (fread(pData, 1, SecHdr.size, pOPENELF->hFile) != SecHdr.size)
    {
        __wrn("read section data failed");
        return EPDK_FAIL;
    }

    //load section data succeeded
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           LOAD ELF FILE
*
* Description: load one elf file
*
* Arguments  : filepath : the full path of elf file
*
* Returns    : handler of open elf file if succeeded, NULL if failed.
*
* Note       :
*********************************************************************************************************
*/
__hdle LDR_LoadFile(const char *filepath)
{
    FILE            *hFile;
    uint32_t        MagicIdx;
    open_elf_t      *pOPENELF = NULL;
    unsigned long   args[3] = {0, 0, 0};

    __log("filepath = %s.", filepath);
    if (filepath == NULL)
    {
        __err("invalid file name for elf loader");
        return NULL;
    }
    //hFile = esFSYS_fopen(filepath, "rb");
    hFile = fopen(filepath, "rb");
    if (hFile == NULL)
    {
        __err("open file [%s] failed", filepath);
        return NULL;
    }
    //check is valid minfs elf file,
    //try to minsf romldr command to get magic index.
    //by sunny at 2011-4-8 13:29:42.
    //if (esFSYS_fioctrl(hFile, ROMLDR_IOC_GET_MAGIC_INDEX, 0, &MagicIdx) == EPDK_OK)
    args[0] = 0;
    args[1] = (unsigned long)(&MagicIdx);
    if (fioctrl(hFile, ROMLDR_IOC_GET_MAGIC_INDEX, (void*)args) == EPDK_OK)
    {
        __magic_common_t magic;

        memset(&magic, 0x00, sizeof(magic));
        //valid minfs elf file
        pOPENELF = hal_malloc(sizeof(open_elf_t));
        if (pOPENELF == NULL)
        {
            __err("allocate memory for open elf file failed");
            //esFSYS_fclose(hFile);
            fclose((FILE*)hFile);
            return NULL;
        }

        memset(pOPENELF, 0x00, sizeof(open_elf_t));
        pOPENELF->IsMFSROMELF   = 1;
        pOPENELF->FileType      = LDR_INVALID_FILE;
        pOPENELF->hFile         = (__hdle)hFile;
        pOPENELF->shoff         = 0;          //no use for minfs elf file
        pOPENELF->MagicIdx      = MagicIdx;

        //get section number
        args[0] = 0;
        args[1] = (unsigned long)(&(pOPENELF->SecNum));
		fioctrl(hFile, ROMLDR_IOC_GET_SECTION_NUM, (void *)args);
        //esFSYS_fioctrl(hFile, ROMLDR_IOC_GET_SECTION_NUM, 0, (void *)(&(pOPENELF->SecNum)));

        args[0] = MagicIdx;
        args[1] = (unsigned long)(&magic);
        //get magic section data
		fioctrl(hFile, ROMLDR_IOC_GET_SECTION_DATA, (void *)args);
        //esFSYS_fioctrl(hFile, ROMLDR_IOC_GET_SECTION_DATA, MagicIdx, &magic);

        if (strncmp(magic.magic, "ePDK.mod", 8) == 0)
        {
            //module file
            pOPENELF->FileType = LDR_MODULE_FILE;
        }
        else if (strncmp(magic.magic, "ePDK.exf", 8) == 0)
        {
            //process file
            pOPENELF->FileType = LDR_PROCESS_FILE;
        }
        else
        {
            __err("module format cant recongnized!");
        }
        return (__hdle)pOPENELF;
    }
    else
    {
        //try to load file as standard elf file format
        pOPENELF = LDR_LoadELFFile((__hdle)hFile);
        if (pOPENELF == NULL)
        {
            //close file handler
            //esFSYS_fclose(hFile);
            fclose((FILE*)hFile);
            return NULL;
        }

        //load elf file succeeded
        return (__hdle)pOPENELF;
    }

    return NULL;
}

/*
*********************************************************************************************************
*                                       UNLOAD ELF FILE
*
* Description: unload one elf file.
*
* Arguments  : hLDR : hanlder of open elf file
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed
*
* Note       :
*********************************************************************************************************
*/
int32_t LDR_UnloadFile(__hdle hLDR)
{
    open_elf_t  *pOPENELF;

    if (hLDR == NULL)
    {
        __wrn("invalid handler to unload elf file");
        return EPDK_FAIL;
    }
    pOPENELF = (open_elf_t *)hLDR;

    //close file handler
    //esFSYS_fclose(pOPENELF->hFile);
    fclose(pOPENELF->hFile);

    //release open elf structure
    hal_free(pOPENELF);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       GET ELF FILE TYPE
*
* Description: get the file type : PROCESS or MODULE.
*
* Arguments  : hLDR : hanlder of open elf file
*
* Returns    : the file type of elf.
*
* Note       :
*********************************************************************************************************
*/
uint32_t LDR_GetFileType(__hdle hLDR)
{
    open_elf_t  *pOPENELF;

    if (hLDR == NULL)
    {
        __wrn("invalid handler to get file type");
        return LDR_INVALID_FILE;
    }
    pOPENELF = (open_elf_t *)hLDR;

    return pOPENELF->FileType;
}

/*
*********************************************************************************************************
*                                       GET THE MAGIC INDEX
*
* Description: get the index of magic section.
*
* Arguments  : hLDR : hanlder of open elf file
*
* Returns    : the index of magic section
*
* Note       :
*********************************************************************************************************
*/
uint32_t LDR_GetMagicIndex(__hdle hLDR)
{
    open_elf_t  *pOPENELF;

    if (hLDR == NULL)
    {
        __wrn("invalid handler to get magic index");
        return LDR_INVALID_INDEX;
    }
    pOPENELF = (open_elf_t *)hLDR;

    return pOPENELF->MagicIdx;
}

/*
*********************************************************************************************************
*                                       GET THE SECTION NUMBER
*
* Description: get the section number of elf file.
*
* Arguments  : hLDR : hanlder of open elf file
*
* Returns    : the sections number.
*
* Note       :
*********************************************************************************************************
*/
uint32_t LDR_GetSecNumber(__hdle hLDR)
{
    open_elf_t  *pOPENELF;

    if (hLDR == NULL)
    {
        __wrn("invalid handler to get section number");
        return 0;
    }
    pOPENELF = (open_elf_t *)hLDR;

    return pOPENELF->SecNum;
}

/*
*********************************************************************************************************
*                                       LOAD SECTION DATA
*
* Description: load one section data.
*
* Arguments  : hLDR     : hanlder of open elf file
*              Index    : the index of load section
*              pData    : pointer of buffer to store data
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed
*
* Note       :
*********************************************************************************************************
*/
int32_t LDR_GetSecData(__hdle hLDR, uint32_t Index, void *pData)
{
    open_elf_t      *pOPENELF;
    unsigned long   args[3] = {0, 0, 0};

    if (hLDR == NULL)
    {
        __wrn("invalid handler to get section data");
        return EPDK_FAIL;
    }
    pOPENELF = (open_elf_t *)hLDR;

    if (Index >= pOPENELF->SecNum)
    {
        __err("invalid section index to get section data");
        return EPDK_FAIL;
    }

    if (pOPENELF->IsMFSROMELF)
    {
        args[0] = Index;
        args[1] = (unsigned long)(pData);
	    return  fioctrl(pOPENELF->hFile, ROMLDR_IOC_GET_SECTION_DATA, (void*)args);
    }
    //stardard elf file format access
    return LDR_GetELFFileSecData(pOPENELF, Index, pData);
}

/*
*********************************************************************************************************
*                                       GET SECTION ROM HEADER
*
* Description: get one section rom header.
*
* Arguments  : hLDR     : hanlder of open elf file
*              Index    : the index of load section
*              pROMHdr    : pointer of buffer to rom header
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed
*
* Note       :
*********************************************************************************************************
*/
int32_t LDR_GetSecROMHdr(__hdle hLDR, uint32_t Index, __section_rom_hdr_t *pROMHdr)
{
    open_elf_t      *pOPENELF;
    unsigned long   args[3] = {0, 0, 0};

    if (hLDR == NULL)
    {
        __wrn("invalid handler to get section rom header");
        return EPDK_FAIL;
    }
    pOPENELF = (open_elf_t *)hLDR;

    if (Index >= pOPENELF->SecNum)
    {
        __wrn("invalid section index to get section rom header");
        return EPDK_FAIL;
    }

    if (pOPENELF->IsMFSROMELF)
    {
        args[0] = (unsigned long)Index;
        args[1] = (unsigned long)pROMHdr;
	    return  fioctrl(pOPENELF->hFile, ROMLDR_IOC_GET_SECTION_HDR, (void*)args);
    }
    //stardard elf file format access
    return LDR_GetELFFileSecROMHdr(pOPENELF, Index, pROMHdr);
}
