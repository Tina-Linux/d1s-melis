/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Module Loader
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : module.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-30
* Descript: module loader handing functions.
* Update  : date                auther      ver     notes
*           2011-3-30 11:14:57  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "mods.h"
#include "eelf.h"
#include <rthw.h>
#include <string.h>
#include <debug.h>
#include <sys_mems.h>
#include <kconfig.h>
#include <log.h>

__krnl_xcb_t            *esXCBTbl[EPOS_id_mumber];
#define  esMCBTbl       ((__module_mcb_t **)esXCBTbl)

#define EPDK_ROOTFS_PATH          "f:\\rootfs\\"
/*
*********************************************************************************************************
*                                       SET FCSE ID
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u8 MODS_SetFsceID(__u8 fcseid)
{
#ifdef CONFIG_ARMV5TE
    return awos_arch_set_fsce_id(fcseid);
#else
    return 0;
#endif
}

/*
*********************************************************************************************************
*                                       GET MAGIC SECTION DATA
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_GetMagicData(__hdle hLDR, __module_mgsec_t *pMagicSection)
{
    uint32_t    MagicIndex;

    //get index of magic section
    MagicIndex = LDR_GetMagicIndex(hLDR);
    if (MagicIndex == LDR_INVALID_INDEX)
    {
        return EPDK_FAIL;
    }
    //get data of magic section
    return LDR_GetSecData(hLDR, MagicIndex, (void *)pMagicSection);

}

/*
*********************************************************************************************************
*                                     LOAD SECTION DATA TO VM
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_LoadSectionData(__hdle hLDR, uint32_t Index, uint32_t maddr, uint32_t domain, void* heap)
{
    __section_rom_hdr_t ROMHdr;
    uint32_t            VMAddr;
    __bool              CreateFlag = 0;
    __bool              LoadFlag   = 0;
    __bool              ClearFlag  = 0;
    __elf_runmem        *secRecord = (__elf_runmem *)heap;
    int32_t             ret = EPDK_OK;

    //initialize section rom header
    rt_memset(&ROMHdr, 0, sizeof(ROMHdr));

    //get section rom header
    if (LDR_GetSecROMHdr(hLDR, Index, &ROMHdr) != EPDK_OK)
    {
        __err("get module file section rom header failed");
        return EPDK_FAIL;
    }
    switch (ROMHdr.Type)
    {
        case EELF_SHT_PROGBITS:
        {
            if (ROMHdr.Flags & EELF_SHF_ALLOC)
            {
                CreateFlag = 1;
                LoadFlag   = 1;
            }
            break;
        }
        case EELF_SHT_NOBITS:
        {
            if (ROMHdr.Flags & EELF_SHF_ALLOC)
            {
                //bss(zi) section
                CreateFlag = 1;
                ClearFlag  = 1;
            }
            break;
        }
        default:
        {
            //just ignore invalid section
            return EPDK_OK;
        }
    }
    //calc this section virtual address
    VMAddr = maddr + ROMHdr.VAddr;
    if (CreateFlag)
    {
        //create virtual memory space,
        //virtual address should alignd by 4k.
        //elf's head define the Addr is uint32_t
        uint32_t AlignAddr = (VMAddr) & (~(0x1000 - 1));
        uint32_t i = 0, npage = 0;

        __wrn("size %d, addr = %p.", ROMHdr.Size, ROMHdr.VAddr);
        if (awos_arch_vmem_create((rt_uint8_t *)AlignAddr, ((ROMHdr.Size + VMAddr - AlignAddr + 0xfff) >> 12), domain) != 0)
        {
            __err("create module section virtual memory space failed");
            return EPDK_FAIL;
        }
        
        if (Index == 0)
        {
            secRecord[0].sectionAddr    = AlignAddr;
            secRecord[0].npage          = (ROMHdr.Size + VMAddr - AlignAddr + 0xfff) >> 12;
        }
        
        for(i = 0; i < Index; i ++)
        {
            if(AlignAddr == secRecord[i].sectionAddr)
            {
                /* change the last value of npage*/
                secRecord[i].npage  = (ROMHdr.Size + VMAddr - AlignAddr + 0xfff) >> 12;
                break;
            }
        }
        if(i >= Index)
        {
            for(i = 0; i <= Index; i ++)
            {
                if(0 == secRecord[i].sectionAddr)
                {
                    secRecord[i].sectionAddr    = AlignAddr;
                    secRecord[i].npage          = (ROMHdr.Size + VMAddr - AlignAddr + 0xfff) >> 12;
                    break;
                }
            }
        }
    }
    if (LoadFlag == 1)
    {
        //load section data to virtual memory
        ret = LDR_GetSecData(hLDR, Index, (void *)VMAddr);
        if(EPDK_OK != ret)
        {
            __err("load section data from modules err!");
            return ret;
        }
    }
    if (ClearFlag)
    {
        //need clear this section
        rt_memset((void *)VMAddr, 0, ROMHdr.Size);
    }

    //module section load succeeded
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       SET SECTION VM BITMAP
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_SectionVMBitMapSet(__hdle hLDR, __module_mcb_t *mcb, uint32_t Index)
{
    uint32_t            BitIndex;
    __section_rom_hdr_t ROMHdr;
    uint32_t            BitNum;
    uint32_t            vBaseAddr;

    //initialize section rom header
    rt_memset(&ROMHdr, 0, sizeof(ROMHdr));

    //get section rom header
    if (LDR_GetSecROMHdr(hLDR, Index, &ROMHdr) != EPDK_OK)
    {
        __err("get section header failed");
        return EPDK_FAIL;
    }
    //vmbitmap is use for record module virtual space allocated,
    //module in melis system can't bigger then 32M,
    //a bit is to identify 1M virtual space used or not.
    switch (ROMHdr.Type)
    {
        case EELF_SHT_PROGBITS:
        case EELF_SHT_NOBITS:
        {
            if (ROMHdr.Flags & EELF_SHF_ALLOC)
            {
                if (mcb->type == EMOD_TYPE_USER)
                {
                    vBaseAddr = ROMHdr.VAddr;
                }
                else
                {
                    __u32 maddr = (EPOS_smod_startaddr + ((mcb->xcb.id - EPOS_smid_min) * EPOS_smod_segsize));
                    vBaseAddr = ROMHdr.VAddr - maddr;
                }
                BitNum = (ROMHdr.Size + 1024 * 1024 - 1) >> 20;
                BitIndex = vBaseAddr >> 20;
                BitIndex %= 32;
                while (BitNum--)
                {
                    (mcb->vmbitmap) |= (1 << BitIndex);
                    BitIndex++;
                }
            }
            break;
        }
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                      CHECK MODULE VM BITMAP IS SET
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__bool MODS_VMBitMapIsSet(uint32_t *pVMBitMap, uint32_t BitIndex)
{
    BitIndex %= 32;
    if (((*pVMBitMap) & (1 << BitIndex)) != 0)
    {
        //this bit is been set
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*                                     DESTORY MODULE VM SPACE
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_DestoryModuleVM(__module_mcb_t *pMCB)
{
    uint32_t        BitIndex = 0;
    uint32_t        VMAddr;
    __elf_runmem    *secRecord = (__elf_runmem *)(pMCB->xcb.heap);
    uint32_t        i = 0;

    //open module file
    for(i = 0; secRecord[i].sectionAddr != 0; i ++)
    {
        awos_arch_vmem_delete((rt_uint8_t *)secRecord[i].sectionAddr, secRecord[i].npage);
        secRecord[i].sectionAddr    = 0;
        secRecord[i].npage          = 0;
    }

    //clear module VM space bitmap
    pMCB->vmbitmap = 0;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     CREATE MODULE VM SPACE
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_CreateModuleVM(__hdle hLDR, __module_mcb_t *mcb)
{
    uint32_t    Index;
    uint32_t    SectionNum;
    uint32_t    MagicIndex;
    uint32_t    maddr;
    uint8_t     domain;

    //get the number of module sections
    SectionNum  = LDR_GetSecNumber(hLDR);
    if (SectionNum == 0)
    {
        __err("invalid module section number");
        return EPDK_FAIL;
    }

    //get magic section index
    MagicIndex  = LDR_GetMagicIndex(hLDR);

    //calc the base address of module
    if (mcb->type == EMOD_TYPE_USER)
    {
        //user module
        domain  = MEMS_domain_user;
        maddr   = EPOS_umod_startaddr + (mcb->xcb.id - EPOS_umid_min) * EPOS_umod_segsize;
    }
    else
    {
        //system module
        domain  = MEMS_domain_system;
        maddr   = 0;
    }

    //clear module VM space bitmap and load module sections
    mcb->vmbitmap = 0;
    for (Index = 0; Index < SectionNum; Index++)
    {
        if (Index == MagicIndex)
        {
            //just ignore magic section
            continue;
        }
        //create section virtual space and load section data
        if (MODS_LoadSectionData(hLDR, Index, maddr, domain, mcb->xcb.heap) != EPDK_OK)
        {
            //destory virtual memory allocated to system
            MODS_DestoryModuleVM(mcb);
            __err("load module section failed");
            return EPDK_FAIL;
        }
        //set section virtual memory allocated bitmap
        MODS_SectionVMBitMapSet(hLDR, mcb, Index);
    }
    //module sections succeeded
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       GET MODULE ID
*
* Description: get module id
*
* Arguments  : void
*
* Returns    : if success, return module id, else return 0.
*
* Note       :
*********************************************************************************************************
*/
uint32_t MODS_GetModuleID(__module_mgsec_t *pModMagic)
{
    uint32_t   mid;

    if (pModMagic->type == EMOD_TYPE_USER)
    {
        __err("user type module not allowd on melis3.0.");
        software_break();
        mid = EPOS_umid_min;
        while (mid <= EPOS_umid_max)
        {
            if (esMCBTbl[mid] == (__module_mcb_t *)0)
            {
                //free module id find
                break;
            }
            mid++;
        }
        if (mid > EPOS_umid_max)
        {
            //no any free module id
            return 0;
        }
    }
    else
    {
        if (pModMagic->type > (EPOS_smid_max - EPOS_smid_min))
        {
            //invalid module type
            return 0;
        }
        mid = EPOS_smid_min + pModMagic->type;
    }
    return mid;
}

/*
*********************************************************************************************************
*                                      NOTIFY MODULE BEEN INSTALLED
*
* Description: notify module is been installed
*
* Arguments  : pMCB :   module control block
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL.
*
* Note       :
*********************************************************************************************************
*/
static int32_t MODS_NotifyInstall(__module_mcb_t *pMCB)
{
    uint8_t     fcseid = 0;
    uint32_t    mid = pMCB->xcb.id;
    int32_t     ret;

    //notify module is been installed.
    if (pMCB->type == EMOD_TYPE_USER)
    {
        //set fcse to this module
        fcseid = MODS_SetFsceID(mid);
    }

    ret = (pMCB->mif.MInit)();

    if (pMCB->type == EMOD_TYPE_USER)
    {
        //restore fcse
        MODS_SetFsceID(fcseid);
    }
    return ret;
}

/*
*********************************************************************************************************
*                                            INSTALL MODULE
*
* Description: This function is used to intall a module,
*              a modules cannot be installed by an ISR.
*
* Arguments  : mfile : full name module file name
*
*
* Returns    : the module id.
*
*********************************************************************************************************
*/
uint32_t esMODS_MInstall(const char *mfile, uint8_t mode)
{
    __hdle              hLDR = NULL;
    __module_mcb_t      *mcb = NULL;
    __module_mgsec_t    ModMagic;
    uint32_t            cpu_sr = 0;
    uint32_t            i;
    uint32_t            ret;
    uint32_t            flag;
    uint8_t             mid = 0;
    int32_t             tmpfile[256] = {0};
    uint32_t            sectionNum = 0;
    static uint8_t      b_boot_cfg = 0x00;

    if (mfile == NULL)
    {
        __err("invalid module file path to install");
        return 0;
    }

    strcpy(tmpfile, mfile);

#if 0
    __s32 esCFG_GetKeyValue(char *SecName, char *KeyName, __s32 Value[], __s32 Count);
    if (0xff == b_boot_cfg)
    {
        ret = esCFG_GetKeyValue("boot_type", "boot_type", (__s32 *)&flag, 1);
        if (ret != 0)
        {
            __err("get boot_type flag failed");
            b_boot_cfg = 0;
        }
        else
        {
            b_boot_cfg = flag;
        }
    }
#endif
    flag  = b_boot_cfg;

    if (flag == 1)
    {
        char    *s;
        s = (char *)strchr((char *)tmpfile, ':');
        if (s && (tmpfile[0] != 'c'  && tmpfile[0] != 'C') && (tmpfile[0] != 'f'  && tmpfile[0] != 'F'))
        {
            char strtmp[256] = {0};
            strcpy(strtmp, EPDK_ROOTFS_PATH);
            strcat(strtmp, tmpfile + 2);
            strcpy(tmpfile, strtmp);
        }
    }

#if defined CONFIG_UDISK_UPDATE_MODS
    {
        int32_t i;
        char tempdrvfile[255] = {0};
        if (tmpfile[0] == 'c' || tmpfile[0] == 'C' || tmpfile[0] == 'd' || tmpfile[0] == 'D' || tmpfile[0] == 'e' || tmpfile[0] == 'E')
        {
            for (i = 252; i >= 0; i--)
            {
                tempdrvfile[i] = tmpfile[i];
                tempdrvfile[0] = 'e';
            }
        }
        else
        {
            for (i = 252; i >= 0; i--)
            {
                tempdrvfile[i + 2] = tmpfile[i];
            }
            tempdrvfile[0] = 'e';
            tempdrvfile[1] = ':';
        }
        hLDR = LDR_LoadFile(tempdrvfile);
        if (hLDR == NULL)
        {
            __err("load module file [%s] failed", tempdrvfile);
        }
    }
#endif
    //open module file
    if (hLDR == NULL)
    {
        hLDR = LDR_LoadFile(tmpfile);
    }

    if (hLDR == NULL)
    {
        __err("load module file [%s] failed", tmpfile);
        return 0;
    }

    //check this file is a valid module file
    if (LDR_GetFileType(hLDR) != LDR_MODULE_FILE)
    {
        __err("invalid module file [%s] to installl", tmpfile);
        goto error;
    }

    //get magic section of module
    rt_memset(&ModMagic, 0, sizeof(ModMagic));
    if (MODS_GetMagicData(hLDR, &ModMagic) != EPDK_OK)
    {
        __err("get module magic section failed");
        goto error;
    }

    //get module id, this process can't been interrupt.
    cpu_sr  = rt_hw_interrupt_disable();
    mid     = MODS_GetModuleID(&ModMagic);
    if (mid == 0)
    {
        __err("get module [%s] id failed", tmpfile);
        rt_hw_interrupt_enable(cpu_sr);
        goto error;
    }

    //install this module control block to allocated slot
    if (esMCBTbl[mid] != NULL)
    {
        __log("check, module %s already installed.", mfile);
        rt_hw_interrupt_enable(cpu_sr);
        return mid;
    }
    rt_hw_interrupt_enable(cpu_sr);
    sectionNum  = LDR_GetSecNumber(hLDR);

    //allocate memory for module control block
    mcb     = (__module_mcb_t *)rt_malloc(sizeof(__module_mcb_t) + rt_strlen(tmpfile) + 1 + sectionNum * sizeof(__elf_runmem));
    if (mcb == (__module_mcb_t *)0)
    {
        __err("allocate memory for module control block failed");
        goto error;
    }
    rt_memset(mcb, 0, sizeof(__module_mcb_t) + rt_strlen(tmpfile) + 1 + sectionNum * sizeof(__elf_runmem));

    esMCBTbl[mid]   = (__module_mcb_t *)mcb;
    //initialize module control block,
    //the module local heap have no any use,
    //all module use system heap to save a little memory.
    //by sunny at 2011-3-31 18:26:36.
    mcb->xcb.id     = mid;
    for (i = 0; i < OS_EVENT_TBL_SIZE; i++)
    {
        mcb->xcb.tcbtbl[i] = 0;
    }
    mcb->mif        = ModMagic.mif;
    mcb->type       = ModMagic.type;
    mcb->xcb.xfile  = (char*)((intptr_t)mcb + sizeof(__module_mcb_t));
    mcb->xcb.them   = 0;
    mcb->xcb.lang   = 0;
    mcb->xcb.heap   = (void*)((intptr_t)mcb + sizeof(__module_mcb_t) + rt_strlen(tmpfile) + 1);
    rt_strncpy(mcb->xcb.xfile, tmpfile, rt_strlen(tmpfile) + 1);

    //create module section virtual memory space
    if (MODS_CreateModuleVM(hLDR, mcb) != EPDK_OK)
    {
        __err("create module [%s] virtual memory space failed", tmpfile);
        goto error;
    }

    //flush i-cache
    esMEMS_FlushICache();//awos_arch_flush_icache();

    //notify module is been installed
    if (MODS_NotifyInstall(mcb) != EPDK_OK)
    {
        __err("notify module [%s] been install failed", tmpfile);
        goto error;
    }

    //module install succeeded
    __inf("module [%s] install succeeded", tmpfile);
    LDR_UnloadFile(hLDR);
    return mid;

error:
    if (mid)
    {
        esMCBTbl[mid] = (__module_mcb_t *)0;
    }
    if (hLDR)
    {
        LDR_UnloadFile(hLDR);
    }
    if (mcb)
    {
        rt_free((void *)mcb);
    }
    return 0;
}

/*
*********************************************************************************************************
*                                      NOTIFY MODULE BEEN INSTALLED
*
* Description: notify module is been installed
*
* Arguments  : pMCB :   module control block
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL.
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_NotifyUnInstall(__module_mcb_t *pMCB)
{
    uint8_t     fcseid = 0;
    uint32_t    mid = pMCB->xcb.id;
    int32_t     ret;

    //notify module is been installed.
    if (pMCB->type == EMOD_TYPE_USER)
    {
        //set fcse to this module
        fcseid = MODS_SetFsceID(mid);
    }

    ret = ((pMCB)->mif.MExit)();

    if (pMCB->type == EMOD_TYPE_USER)
    {
        //restore fcse
        MODS_SetFsceID(fcseid);
    }
    return ret;
}

/*
*********************************************************************************************************
*                                            UNINSTALL MODULE
*
* Description: This function allows you to uninstall a module.  The calling task can delete itself by
*              input EXEC_pdelself.  module allocated memory will be returned to system memory pool.
*
* Arguments  : mid  : module id
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL
*
* Notes      : before the module been uninstall, the module must been closed.
*********************************************************************************************************
*/
int32_t esMODS_MUninstall(uint8_t id)
{
    __module_mcb_t  *mcb;

#if 0
    //the module can't been delete by self
    if (id == esIDCur || id == EXEC_pidself)
    {
        return EPDK_FAIL;
    }
#endif
    //get module control block
    mcb = esMCBTbl[id];

    //notify module to uninstall
    if (MODS_NotifyUnInstall(mcb) != EPDK_OK)
    {
        __err("notify module [%s] been uninstall failed", mcb->xcb.xfile);
        return EPDK_FAIL;
    }

    //destory the virtual memory space of module allcoated
    MODS_DestoryModuleVM(mcb);

    //release allocated module control block
    rt_free((void *)mcb);
    esMCBTbl[id] = RT_NULL;

    //dump module uninstall information
    __inf("module [%s] uninstall succeeded", mcb->xcb.xfile);

    //uninstall module succeeded
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           OPEN MODULE
*
* Description: open module, the times of module can open depend on the module self,
*              system have no limit to this.
*
* Arguments  : id   : the id of module
*              mode : open mode, the mode is define by every module self.
*
* Returns    : the handle of module opened. NULL if failed.
*
* Notes      :
*********************************************************************************************************
*/
__mp *esMODS_MOpen(uint8_t id, uint8_t mode)
{
    uint32_t    fcseid;
    __mp        *mp;

    if (id == 0)
    {
        __err("invalid paramter.");
        return (__mp *)0;
    }

    if (id > EPOS_umid_max)
    {
        return (esMCBTbl[id]->mif.MOpen)(id, mode);
    }

    //set fcse to this module
    fcseid = MODS_SetFsceID(id);

    mp = (esMCBTbl[id]->mif.MOpen)(id, mode);

    //restore fcse
    MODS_SetFsceID(fcseid);

    return mp;
}

/*
*********************************************************************************************************
*                                            CLOSE MODULE
*
* Description: close a module, this is just close a open hander of module.
*
* Arguments  : mp : the handler of module open return.
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esMODS_MClose(__mp *mp)
{
    uint32_t    fcseid;
    int32_t     ret;

    if (mp == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return EPDK_FAIL;
    }

    if (mp->mid > EPOS_umid_max)
    {
        return (esMCBTbl[mp->mid]->mif.MClose)(mp);
    }

    //set fcse to this module
    fcseid = MODS_SetFsceID(mp->mid);

    ret = (esMCBTbl[mp->mid]->mif.MClose)(mp);

    //restore fcse
    MODS_SetFsceID(fcseid);

    return ret;
}

/*
*********************************************************************************************************
*                                            MODULE READ
*
* Description: read a module data.
*
* Arguments  : pdata    : buffer to store readback data
*              size     : Item size in bytes
*              n        : Maximum number of items to be read
*              mp       : the handler of module open return
*
* Returns    : the readback items number.
*
* Notes      :
*********************************************************************************************************
*/
uint32_t esMODS_MRead(void *pdata, uint32_t size, uint32_t n, __mp *mp)
{
    uint8_t     fcseid;
    uint32_t    ret;

    if (mp->mid > EPOS_umid_max)
    {
        return (esMCBTbl[mp->mid]->mif.MRead)(pdata, size, n, mp);
    }

    //set fcse to this module
    fcseid = MODS_SetFsceID(mp->mid);

    ret = (esMCBTbl[mp->mid]->mif.MRead)(pdata, size, n, mp);

    //restore fcse
    MODS_SetFsceID(fcseid);

    return ret;
}


/*
*********************************************************************************************************
*                                            MODULE WRITE
*
* Description: write data to a module.
*
* Arguments  : pdata    : buffer to store readback data
*              size     : Item size in bytes
*              n        : Maximum number of items to be write
*              mp       : the handler of module open return
*
* Returns    : the write items number.
*
* Notes      :
*********************************************************************************************************
*/
uint32_t esMODS_MWrite(const void *pdata, uint32_t size, uint32_t n, __mp *mp)
{
    uint8_t     fcseid;
    uint32_t    ret;

    if (mp->mid > EPOS_umid_max)
    {
        return (esMCBTbl[mp->mid]->mif.MWrite)(pdata, size, n, mp);
    }

    //set fcse to this module
    fcseid = MODS_SetFsceID(mp->mid);

    ret = (esMCBTbl[mp->mid]->mif.MWrite)(pdata, size, n, mp);

    //restore fcse
    MODS_SetFsceID(fcseid);

    return ret;
}

/*
*********************************************************************************************************
*                                            MODULE IO CONTROL
*
* Description: send control command to access a module.
*
* Arguments  : mp       : the handler of module open return
*              cmd      : io control command
*              aux      : the meaning is depend module command
*              pbuffer  : the meaning is depend module command
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL
*
* Notes      :
*********************************************************************************************************
*/
long esMODS_MIoctrl(__mp *mp, uint32_t cmd, int32_t aux, void *pbuffer)
{
    uint8_t  fcseid;
    long ret;

    if (mp == NULL)
    {
        __err("invalid parameter!");
        return EPDK_FAIL;
    }

    if (mp->mid > EPOS_umid_max)
    {
        return (esMCBTbl[mp->mid]->mif.MIoctrl)(mp, cmd, aux, pbuffer);
    }

    //set fcse to this module
    fcseid = MODS_SetFsceID(mp->mid);

    ret = esMCBTbl[mp->mid]->mif.MIoctrl(mp, cmd, aux, pbuffer);

    //restore fcse
    MODS_SetFsceID(fcseid);

    return ret;
}

/*
*********************************************************************************************************
*                                       MODULE SYSTEM START
*
* Description: This function is used to start module sub-system
*
* Arguments  : void
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_Init(void)
{
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       MODULE SYSTEM END
*
* Description: This function is used to end module sub-system
*
* Arguments  : void
*
* Returns    : if success, return EPDK_OK, else return EPDK_FAIL
*
* Note       :
*********************************************************************************************************
*/
int32_t MODS_Exit(void)
{
    return EPDK_OK;
}
