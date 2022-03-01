/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                execute sub-system
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exec.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-1
* Descript: execute sub-system.
* Update  : date                auther      ver     notes
*           2011-4-1 18:54:41   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include <rthw.h>
#include <log.h>
#include <debug.h>
#include <string.h>
#include <kapi.h>
#include <sys_mems.h>
#include "exec.h"
#include "eelf.h"
#include "loader.h"
#include "mods.h"

extern __krnl_xcb_t   *esXCBTbl[EPOS_id_mumber];
#define  esPCBTbl     ((__exec_pcb_t **)esXCBTbl)

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
int32_t EXEC_GetMagicData(__hdle hLDR, __exec_mgsec_t *pMagicSection)
{
    uint32_t MagicIndex;

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
int32_t EXEC_LoadSectionData(__hdle hLDR, uint32_t paddr, uint32_t Index, void* heap)
{
    __section_rom_hdr_t ROMHdr;
    __bool              CreateFlag = 0;
    __bool              LoadFlag   = 0;
    __bool              ClearFlag  = 0;
    uint32_t            VMAddr;
    __elf_runmem        *secRecord  = (__elf_runmem *)heap;

    //initialize section rom header
    rt_memset(&ROMHdr, 0, sizeof(ROMHdr));

    //get section rom header
    if (LDR_GetSecROMHdr(hLDR, Index, &ROMHdr) != EPDK_OK)
    {
        __err("get process file section rom header failed");
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
    VMAddr = paddr + ROMHdr.VAddr;
    if (CreateFlag)
    {
        //create virtual memory space,
        //virtual address should alignd by one page.
        __u32 AlignAddr = (VMAddr) & (~(0x1000 - 1));
        uint32_t i = 0, npage = 0;

        __log("size %d, addr = %p.", ROMHdr.Size, ROMHdr.VAddr);
        if (awos_arch_vmem_create((rt_uint8_t *)AlignAddr, ((ROMHdr.Size + VMAddr - AlignAddr + 0xfff) >> 12), MEMS_domain_user) != EPDK_OK)
        {
            __err("create process section virtual memory space failed");
            return EPDK_FAIL;
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
            for(i = 0; i < Index; i ++)
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
        __log("VMAddr = 0x%08x.", VMAddr);
        LDR_GetSecData(hLDR, Index, (void *)VMAddr);
    }
    if (ClearFlag)
    {
        //need clear this section
        rt_memset((void *)VMAddr, 0, ROMHdr.Size);
    }

    //process section load succeeded
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     CREATE PROCESS VM SPACE
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
int32_t EXEC_CreateProcessVM(__hdle hLDR, __exec_pcb_t *pcb)
{
    uint32_t    Index;
    uint32_t    SectionNum;
    uint32_t    MagicIndex;
    uint32_t    paddr;

    //get the number of process sections
    SectionNum = LDR_GetSecNumber(hLDR);
    if (SectionNum == 0)
    {
        __wrn("invalid process section number");
        return EPDK_FAIL;
    }

    //get magic section index
    MagicIndex = LDR_GetMagicIndex(hLDR);

    //calculate the process address
    paddr = pcb->xcb.id * EPOS_proc_segsize;

    //load process sections
    for (Index = 0; Index < SectionNum; Index++)
    {
        if (Index == MagicIndex)
        {
            //just ignore magic section
            continue;
        }
        //create section virtual space and load section data
        if (EXEC_LoadSectionData(hLDR, paddr, Index, pcb->xcb.heap) != EPDK_OK)
        {
            __elf_runmem        *secRecord  = (__elf_runmem *)(pcb->xcb.heap);
            uint32_t            i = 0;

            //release allcotes virtual memory system
            for(i = 0; secRecord[i].sectionAddr != 0; i ++)
            {
                awos_arch_vmem_delete((rt_uint8_t *)secRecord[i].sectionAddr, secRecord[i].npage);
                secRecord[i].sectionAddr    = 0;
                secRecord[i].npage          = 0;
            }

            __err("load process section failed");
            return EPDK_FAIL;
        }
    }
    //process sections succeeded
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                       ALLOCATE FREE PROCESS ID
*
* Description: allocate process id
*
* Arguments  : void
*
* Returns    : if success, return process id, else return 0.
*
* Note       :
*********************************************************************************************************
*/
uint8_t EXEC_GetProcessID(void)
{
    uint8_t pid;

    pid = EPOS_pid_min;
    while (pid <= EPOS_pid_max)
    {
        if (esPCBTbl[pid] == (__exec_pcb_t *)0)
        {
            //free process id find
            return pid;
        }
        pid++;
    }
    //allocate pid failed
    __wrn("no free process to allocated");
    return 0;
}

/*
**********************************************************************************************************************
*                                            Delete a Process(application)
*
* Description: This function allows you to delete a App.  The calling task process delete itself by
*              input EXEC_pdelself.  App's memory will be returned to system memory pool.
*
* Arguments  : pid  process id,当pid为EXEC_pdelself时，为delete self
*
* Returns    :
*
* Notes      :
**********************************************************************************************************************
*/
int8_t esEXEC_PDel(uint8_t id)
{
    struct rt_object_information *information;
    struct rt_list_node *node;
    rt_thread_t         temp;
    rt_uint32_t         fcse;
    __exec_pcb_t        *pcb = esPCBTbl[id];
    uint32_t            i = 0;
    __elf_runmem        *secRecord = (__elf_runmem *)(pcb->xcb.heap);

    // only used for shell delete itself.
    asm volatile("mrc p15, 0, %0, c13, c0, 0" : "=r"(fcse) : : "memory");

    if (id == EXEC_pidself)
    {
        id = (fcse >> 25);
    }

    if ((id != 1) || ((fcse >> 25) != 1))
        while (1);

    pcb = esPCBTbl[id];

    for(i = 0; secRecord[i].sectionAddr != 0; i ++)
    {
        awos_arch_vmem_delete((rt_uint8_t *)secRecord[i].sectionAddr, secRecord[i].npage);
        secRecord[i].sectionAddr    = 0;
        secRecord[i].npage    = 0;
    }

    rt_free((void *)esPCBTbl[id]);
    esPCBTbl[id]    = RT_NULL;

    information     = rt_object_get_information(RT_Object_Class_Thread);
    RT_ASSERT(information != RT_NULL);
    for (node  = information->object_list.next; node != &(information->object_list); node  = node->next)
    {
        struct rt_object *object;
        object  = rt_list_entry(node, struct rt_object, list);
        temp    = (rt_thread_t)object;

        if ((rt_uint32_t)temp->entry  < (32 * 1024 * 1024))
        {
            if (temp != rt_thread_self())
            {
                rt_thread_delete(temp);
            }
        }
    }

    rt_thread_delete(rt_thread_self());
    rt_schedule();
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                            Delete a Process(application)
*
* Description: This function allows you to delete a App.  The calling task can delete itself by
*              input EXEC_pdelself.  App's memory will be returned to system memory pool.
*
* Arguments  : pid  process id
*                   当pid为EXEC_pdelself时，为delete self
*
* Returns    :
*
* Notes      :
**********************************************************************************************************************
*/
void cb_EXEC_PDel(int32_t u_arg, int32_t s_arg)
{
    esEXEC_PDel(((uint32_t)u_arg & 0xff));
    return;
}

int8_t esEXEC_PDelReq(uint8_t pid)
{
    __epos_kmsg_t *msg_tdel;

    msg_tdel = (__epos_kmsg_t *)rt_malloc(sizeof(__epos_kmsg_t));
    if (msg_tdel)
    {
        //向系统邮箱发一个删除请求的命令
        msg_tdel->target       = KMSG_TGT_CALLBACK;
        msg_tdel->message      = 0;
        msg_tdel->prio         = KMSG_PRIO_LOW;
        msg_tdel->l.cb         = cb_EXEC_PDel;
        msg_tdel->h.cb_u_arg   = (__s32)pid;
        esKSRV_SendMsgEx((void *)msg_tdel);

        return EPDK_OK;
    }
    return EPDK_FAIL;
}


/*
**********************************************************************************************************************
*                                               exec_main_thread
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      : 所有的进程的main函数都被这个线程调用，输入参数和返回参数都由此线程处理
*
**********************************************************************************************************************
*/
static void exec_main_thread(void *p_arg)
{
    uint32_t    fcseid;
    uint32_t    cpu_sr;
    uint8_t     prio = 30;
    __exec_mainthread_para_t *para;

    para = (__exec_mainthread_para_t *)p_arg;

    if (para == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return;
    }
    //切换进入用户段
    cpu_sr = rt_hw_interrupt_disable();

    asm volatile("mrc p15, 0, %0, c13, c0, 0" : "=r"(fcseid) : : "memory");
    asm volatile("mcr p15, 0, %0, c13, c0, 0" : :"r"(para->id << 25): "memory");

    rt_hw_interrupt_enable(cpu_sr);

    //call main function of the process, and store return value to pcb
    rt_err_t rt_thread_nameset(rt_thread_t thread, char *name);
    rt_thread_nameset(rt_thread_self(), esPCBTbl[para->id]->xcb.xfile);
    rt_thread_control(rt_thread_self(), RT_THREAD_CTRL_CHANGE_PRIORITY, &prio);

    /* shell entry can be null! so weired anyway. */
    //if (para->main)
    {
        esPCBTbl[para->id]->ret = para->main(para->p_arg);
    }

    //退回系统态
    asm volatile("mcr p15, 0, %0, c13, c0, 0 " : : "r"(fcseid) : "memory");

    rt_free(para);

    //如果sem不为空，表示有用户在等待此进程结束
    if (esPCBTbl[para->id]->retsem)
    {
        rt_sem_release(esPCBTbl[para->id]->retsem);
    }

    rt_thread_delete(rt_thread_self());
    rt_schedule();

    return;
}


/*
**********************************************************************************************************************
*                                            esEXEC_Run
*
* Description: This function is used to start a process. A process cannot be created by an ISR.
*
* Arguments  : pfilename     full name process file name
*              p_arg         process main function's arg
*              mode.bit0     0: normal
*                            1: wait main function ret before return
*              ret           return value of main function
*
* Returns    : process handle
*               if mode.bit0 equ. 1, return NULL, ret is the return value of main function
*               if mode.bit0 equ. 0, if return NULL, means create process fail
**********************************************************************************************************************
*/
uint8_t esEXEC_PCreate(const char *filename, void *p_arg, uint32_t mode, uint32_t *ret)
{
    uint8_t         pid = 0;
    __hdle          hLDR = NULL;
    __exec_mgsec_t  ExecMagic;
    __exec_pcb_t    *pcb = NULL;
    uint32_t        cpu_sr = 0;
    uint32_t        i;
    uint32_t        sectionNum = 0;

    if (filename == NULL)
    {
        __err("invalid process file path to create");
        return 0;
    }

    //allocate a process id, this process can't been interrupt.
    cpu_sr  = rt_hw_interrupt_disable();
    pid     = EXEC_GetProcessID();
    if (pid == 0)
    {
        __err("allocate process [%s] id failed", filename);
        rt_hw_interrupt_enable(cpu_sr);
        goto error;
    }
    //ocur this process control block first
    esPCBTbl[pid]   = (__exec_pcb_t *)0xffffffff;
    rt_hw_interrupt_enable(cpu_sr);

    //load process file
    hLDR    = LDR_LoadFile(filename);
    if (hLDR == NULL)
    {
        __err("load process file [%s] failed", filename);
        goto error;
    }

    //check this file is a valid process file
    if (LDR_GetFileType(hLDR) != LDR_PROCESS_FILE)
    {
        __err("invalid process file [%s] to installl", filename);
        goto error;
    }
    sectionNum  = LDR_GetSecNumber(hLDR);

    //allocate memory for process control block
    pcb = (__exec_pcb_t *)rt_malloc(sizeof(__exec_pcb_t) + rt_strlen(filename) + 1 + sectionNum * sizeof(__elf_runmem));
    if (pcb == (__exec_pcb_t *)0)
    {
        __err("allocate memory for process control block failed");
        goto error;
    }

    rt_memset(pcb, 0, sizeof(__exec_pcb_t) + rt_strlen(filename) + 1 + sectionNum * sizeof(__elf_runmem));

    //get magic section of process
    rt_memset(&ExecMagic, 0, sizeof(ExecMagic));
    if (EXEC_GetMagicData(hLDR, &ExecMagic) != EPDK_OK)
    {
        __err("get process magic section failed");
        goto error;
    }

    //initialize process control block,
    //the process local heap have no any use,
    //all process use system heap to save a little memory.
    //by sunny at 2011-3-31 21:48:05.
    pcb->ret    = EPDK_OK;
    pcb->xcb.id = pid;
    for (i = 0; i < OS_EVENT_TBL_SIZE; i++)
    {
        pcb->xcb.tcbtbl[i] = 0;
    }

    pcb->xcb.xfile  = (char*)((intptr_t)pcb + sizeof(__exec_pcb_t));
    strcpy(pcb->xcb.xfile, (char *)filename);
    pcb->xcb.them   = 0;
    pcb->xcb.lang   = 0;
    pcb->xcb.heap   = (void*)((intptr_t)pcb + sizeof(__exec_pcb_t) + rt_strlen(filename) + 1);

    //register this process control block to system
    esPCBTbl[pid]   = pcb;

    //create process section virtual memory space
    if (EXEC_CreateProcessVM(hLDR, pcb) != EPDK_OK)
    {
        __err("create process [%s] virtual memory space failed", filename);
        goto error;
    }

    //flush i-cache
    awos_arch_flush_icache();

    //close process file handle
    LDR_UnloadFile(hLDR);

    //start process
    {
        __exec_mainthread_para_t *para;
        para = (__exec_mainthread_para_t *)rt_malloc(sizeof(__exec_mainthread_para_t));
        if (para == NULL)
        {
            __err("fatal error, malloc buffer failure.");
            goto error;
        }

        rt_memset(para, 0x00, sizeof(__exec_mainthread_para_t));
        para->main  = ExecMagic.main;
        para->p_arg = p_arg;
        para->id    = pid;
        if (mode & EXEC_CREATE_WAIT_RET)
        {
            //wait main function to return，need create a sem to pending
            pcb->retsem = rt_sem_create("proc_sem", 0, 0);
        }
        else
        {
            pcb->retsem = RT_NULL;
        }
        if (esKRNL_TCreate(exec_main_thread, para, ExecMagic.mtskstksize, (pid << 8) | ExecMagic.mtskprio) != 0)
        {
            if (mode & EXEC_CREATE_WAIT_RET)
            {
                rt_sem_take(pcb->retsem, RT_WAITING_FOREVER);
                rt_sem_delete(pcb->retsem);
                if (ret)
                {
                    *ret    = pcb->ret;
                }
                //process create and exit succeeded
                __log("process [%s] create and exit succeeded", filename);
                return 0;
            }
            else
            {
                //process create succeeded
                __log("process [%s] create succeeded", filename);
                return pid;
            }
        }
        rt_free(para);
        rt_sem_delete(pcb->retsem);
    }

    //process create failed
error:
    if (pid)
    {
        esPCBTbl[pid] = (__exec_pcb_t *)0;
    }
    if (hLDR)
    {
        LDR_UnloadFile(hLDR);
    }
    if (pcb)
    {
        rt_free((void *)pcb);
    }

    __log("process [%s] created failed!", filename);
    return 0;
}

/*
*********************************************************************************************************
*                                       esexec_run
*
* description:
*
* arguments  :
*
* returns    :
*
* note       :
*********************************************************************************************************
*/
int8_t esEXEC_Run(const char *pfilename, void *p_arg, uint32_t mode, uint32_t *ret)
{
    return esEXEC_PCreate(pfilename, p_arg, mode, ret);
}

/*
**********************************************************************************************************************
*                                            start exec sub-system
*
* description: this function is used to start exec sub-system
*
* arguments  : void
*
* returns    : if success, return EPDK_OK, else return epdk_fail
**********************************************************************************************************************
*/
int32_t EXEC_Init(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                       end exec sub-system
*
* description: this function is used to end exec sub-system
*
* arguments  : void
*
* returns    : if success, return EPDK_OK, else return epdk_fail
**********************************************************************************************************************
*/
int32_t EXEC_Exit(void)
{
    return EPDK_OK;
}
