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
#include <typedef.h>
#include <rtthread.h>
#include <mod_defs.h>
#include <stdio.h>
#include <sys/stat.h>
#include <rthw.h>
#include <kapi.h>
#include <hal_debug.h>
#include <arch.h>
#include <log.h>
#include <mman.h>

#ifdef CONFIG_DEBUG_BACKTRACE
#include <backtrace.h>
#endif

extern const unsigned long SWIHandler_entity_NLIBOPS[];
extern const unsigned long SWIHandler_entity_PTHREAD[];

#ifdef CONFIG_NETWORK
extern const unsigned long SWIHandler_entity_LWIP[];
#endif

void syscall_trap(void)
{
    __err("syscall not support! from %p.", __builtin_return_address(0));
    hal_sys_abort();
}

void esKRNL_DumpStack(void)
{
    void dump_stack_nofp(uint32_t pc, uint32_t sp, int32_t level, uint32_t *pointer, int32_t flag);
    dump_stack_nofp(0, 0, 0, NULL, 0);
}

void esSIOS_foobar(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
    __log("a = %d, b = %d, c = %d, d = %d, e = %d, f = %d.\n", a, b, c, d, e, f);
}

/*SWIHandler_SIOS_t*/
const unsigned long SWIHandler_entity_SIOS[] =
{
    (const unsigned long)syscall_trap                                   ,//0
    (const unsigned long)syscall_trap                                   ,//1
    (const unsigned long)syscall_trap                                   ,//2
    (const unsigned long)esSIOS_putstr                                  ,//3
    (const unsigned long)syscall_trap                                   ,//4
};

/*SWIHandler_KRNL_t*/
const unsigned long SWIHandler_entity_KRNL[] =
{
    (const unsigned long)esKRNL_TCreate                                ,// 0 *
    (const unsigned long)esKRNL_TDel                                   ,// 1
    (const unsigned long)esKRNL_TDelReq                                ,// 2
    (const unsigned long)syscall_trap                                  ,// 3    esKRNL_GetPrio
    (const unsigned long)syscall_trap                                  ,// 4    esKRNL_FreePrio
    (const unsigned long)syscall_trap                                  ,// 5 *  esKRNL_TaskChangePrio
    (const unsigned long)syscall_trap                                  ,// 6    esKRNL_TaskNameGet
    (const unsigned long)esKRNL_TaskNameSet                            ,// 7    esKRNL_TaskNameSet
    (const unsigned long)esKRNL_TaskResume                             ,// 8
    (const unsigned long)esKRNL_TaskSuspend                            ,// 9
    (const unsigned long)syscall_trap                                  ,// 10 *
    (const unsigned long)esKRNL_TaskQuery                              ,// 11
    (const unsigned long)esKRNL_TimeDly                                ,// 12
    (const unsigned long)syscall_trap                                  ,// 13
    (const unsigned long)esKRNL_TimeDlyResume                          ,// 14
    (const unsigned long)esKRNL_TimeGet                                ,// 15 *
    (const unsigned long)esKRNL_TimeSet                                ,// 16
    (const unsigned long)esKRNL_SemAccept                              ,// 17
    (const unsigned long)esKRNL_SemCreate                              ,// 18
    (const unsigned long)esKRNL_SemDel                                 ,// 19
    (const unsigned long)esKRNL_SemPend                                ,// 20 *
    (const unsigned long)esKRNL_SemPost                                ,// 21
    (const unsigned long)esKRNL_SemQuery                               ,// 22
    (const unsigned long)esKRNL_SemSet                                 ,// 23
    (const unsigned long)esKRNL_MboxAccept                             ,// 24
    (const unsigned long)esKRNL_MboxCreate                             ,// 25 *
    (const unsigned long)esKRNL_MboxDel                                ,// 26
    (const unsigned long)esKRNL_MboxPend                               ,// 27
    (const unsigned long)esKRNL_MboxPost                               ,// 28
    (const unsigned long)esKRNL_MboxPostOpt                            ,// 29
    (const unsigned long)esKRNL_MboxQuery                              ,// 30 *
    (const unsigned long)syscall_trap                                  ,// 31   esKRNL_MutexAccept
    (const unsigned long)esKRNL_MutexCreate                            ,// 32
    (const unsigned long)esKRNL_MutexDel                               ,// 33
    (const unsigned long)esKRNL_MutexPend                              ,// 34
    (const unsigned long)esKRNL_MutexPost                              ,// 35 *
    (const unsigned long)syscall_trap                                  ,// 36   esKRNL_MutexQuery
    (const unsigned long)esKRNL_QAccept                                ,// 37
    (const unsigned long)esKRNL_QCreate                                ,// 38
    (const unsigned long)esKRNL_QDel                                   ,// 39
    (const unsigned long)esKRNL_QFlush                                 ,// 40 *
    (const unsigned long)esKRNL_QPend                                  ,// 41
    (const unsigned long)esKRNL_QPost                                  ,// 42
    (const unsigned long)esKRNL_QPostFront                             ,// 43
    (const unsigned long)esKRNL_QPostOpt                               ,// 44
    (const unsigned long)esKRNL_QQuery                                 ,// 45 *
    (const unsigned long)esKRNL_FlagAccept                             ,// 46
    (const unsigned long)esKRNL_FlagCreate                             ,// 47
    (const unsigned long)esKRNL_FlagDel                                ,// 48
    (const unsigned long)esKRNL_FlagNameGet                            ,// 49
    (const unsigned long)esKRNL_FlagNameSet                            ,// 50 *
    (const unsigned long)esKRNL_FlagPend                               ,// 51
    (const unsigned long)esKRNL_FlagPendGetFlagsRdy                    ,// 52
    (const unsigned long)esKRNL_FlagPost                               ,// 53
    (const unsigned long)esKRNL_FlagQuery                              ,// 54
    (const unsigned long)esKRNL_TmrCreate                              ,// 55 *
    (const unsigned long)esKRNL_TmrDel                                 ,// 56
    (const unsigned long)esKRNL_TmrStart                               ,// 57
    (const unsigned long)esKRNL_TmrStop                                ,// 58
    (const unsigned long)esKRNL_TmrRemainGet                           ,// 59
    (const unsigned long)esKRNL_TmrStateGet                            ,// 60 *
    (const unsigned long)esKRNL_TmrError                               ,// 61
    (const unsigned long)esKRNL_Version                                ,// 62
    (const unsigned long)esKRNL_SchedLock                              ,// 63
    (const unsigned long)esKRNL_SchedUnlock                            ,// 64
    (const unsigned long)esKRNL_GetCallBack                            ,// 65 *
    (const unsigned long)esKRNL_CallBack                               ,// 66
    (const unsigned long)esKRNL_GetTIDCur                              ,// 67
    (const unsigned long)esKRNL_SktCreate                              ,// 68
    (const unsigned long)esKRNL_SktDel                                 ,// 69
    (const unsigned long)esKRNL_SktPend                                ,// 70 *
    (const unsigned long)esKRNL_SktPost                                ,// 71
    (const unsigned long)esKRNL_SktAccept                              ,// 72
    (const unsigned long)syscall_trap                                  ,// 73   esKRNL_SktFlush
    (const unsigned long)syscall_trap                                  ,// 74   esKRNL_SktError
    (const unsigned long)esKRNL_Time                                   ,// 75 *
#if (OS_TASK_STAT_EN > 0) && (OS_CPU_HOOKS_EN > 0)
    (const unsigned long)esKRNL_CPUStatStart                           ,// 76
    (const unsigned long)esKRNL_CPUStatStop                            ,// 77
    (const unsigned long)esKRNL_CPUStatReport                          ,// 78
#endif
    (const unsigned long)esKRNL_TaskPrefEn                             ,// 79
    (const unsigned long)esKRNL_MemLeakChk                             ,// 80 *
    (const unsigned long)syscall_trap                                  ,// 81   esKRNL_DumpStack
    (const unsigned long)esKRNL_InterruptEnable                        ,// 82
    (const unsigned long)esKRNL_InterruptDisable                       ,// 83
#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
    (const unsigned long)set_log_level                                 ,// 84
    (const unsigned long)get_log_level                                 ,// 85
#endif
#ifdef CONFIG_DEBUG_BACKTRACE
    (const unsigned long)backtrace                                     ,// 86 *
#endif
    (const unsigned long)mmap                                          ,// 87 *
    (const unsigned long)select                                        ,// 88
    (const unsigned long)esKRNL_Ioctrl                                 ,// 89 *
};

/*SWIHandler_MEMS_t*/
const unsigned long SWIHandler_entity_MEMS[] =
{
    (const unsigned long)esMEMS_Palloc                                  ,//0
    (const unsigned long)esMEMS_Pfree                                   ,//1.
    (const unsigned long)esMEMS_VA2PA                                   ,//2.
    (const unsigned long)esMEMS_VMCreate                                ,//3.
    (const unsigned long)esMEMS_VMDelete                                ,//4.
    (const unsigned long)esMEMS_HeapCreate                              ,//5
    (const unsigned long)esMEMS_HeapDel                                 ,//6
    (const unsigned long)esMEMS_Malloc                                  ,//7
    (const unsigned long)esMEMS_Mfree                                   ,//8
    (const unsigned long)esMEMS_Realloc                                 ,//9
    (const unsigned long)esMEMS_Calloc                                  ,//10
    (const unsigned long)esMEMS_Balloc                                  ,//11
    (const unsigned long)esMEMS_Bfree                                   ,//12
    (const unsigned long)esMEMS_Info                                    ,//13
    (const unsigned long)esMEMS_CleanDCache                             ,//14
    (const unsigned long)esMEMS_CleanFlushDCache                        ,//15
    (const unsigned long)esMEMS_CleanFlushCache                         ,//16
    (const unsigned long)esMEMS_FlushDCache                             ,//17
    (const unsigned long)esMEMS_FlushICache                             ,//18
    (const unsigned long)esMEMS_FlushCache                              ,//19
    (const unsigned long)esMEMS_CleanDCacheRegion                       ,//20
    (const unsigned long)esMEMS_CleanFlushDCacheRegion                  ,//21
    (const unsigned long)esMEMS_CleanFlushCacheRegion                   ,//22
    (const unsigned long)esMEMS_FlushDCacheRegion                       ,//23
    (const unsigned long)esMEMS_FlushICacheRegion                       ,//24
    (const unsigned long)esMEMS_FlushCacheRegion                        ,//25
    (const unsigned long)esMEMS_VMalloc                                 ,//26
    (const unsigned long)esMEMS_VMfree                                  ,//27
    (const unsigned long)esMEMS_FreeMemSize                             ,//28
    (const unsigned long)esMEMS_TotalMemSize                            ,//29
    (const unsigned long)esMEMS_LockICache                              ,//30
    (const unsigned long)esMEMS_LockDCache                              ,//31
    (const unsigned long)esMEMS_UnlockICache                            ,//32
    (const unsigned long)esMEMS_UnlockDCache                            ,//33
    (const unsigned long)esMEMS_GetIoVAByPA                             ,//34
    (const unsigned long)esMEMS_PhyAddrConti                            ,//35
    (const unsigned long)esMEMS_CleanInvalidateCacheAll                 ,//36
};

/*SWIHandler_FSYS_t*/
const unsigned long SWIHandler_entity_FSYS[] =
{
    (const unsigned long)esFSYS_clearpartupdateflag                      ,// 0 *
    (const unsigned long)esFSYS_closedir                                 ,// 1
    (const unsigned long)esFSYS_fclose                                   ,// 2
    (const unsigned long)esFSYS_fd2file                                  ,// 3
    (const unsigned long)esFSYS_ferrclr                                  ,// 4
    (const unsigned long)esFSYS_ferror                                   ,// 5 *
    (const unsigned long)esFSYS_file2fd                                  ,// 6
    (const unsigned long)esFSYS_fioctrl                                  ,// 7
    (const unsigned long)esFSYS_fopen                                    ,// 8
    (const unsigned long)esFSYS_format                                   ,// 9
    (const unsigned long)esFSYS_fread                                    ,// 10 *
    (const unsigned long)esFSYS_fsdbg                                    ,// 11
    (const unsigned long)esFSYS_fseek                                    ,// 12
    (const unsigned long)esFSYS_fseekex                                  ,// 13
    (const unsigned long)esFSYS_fsreg                                    ,// 14
    (const unsigned long)esFSYS_fstat                                    ,// 15 *
    (const unsigned long)esFSYS_fsunreg                                  ,// 16
    (const unsigned long)esFSYS_fsync                                    ,// 17
    (const unsigned long)esFSYS_ftell                                    ,// 18
    (const unsigned long)esFSYS_ftellex                                  ,// 19
    (const unsigned long)esFSYS_ftruncate                                ,// 20 *
    (const unsigned long)esFSYS_fwrite                                   ,// 21
    (const unsigned long)esFSYS_getfscharset                             ,// 22
    (const unsigned long)esFSYS_mkdir                                    ,// 23
    (const unsigned long)esFSYS_mntfs                                    ,// 24
    (const unsigned long)esFSYS_mntparts                                 ,// 25 *
    (const unsigned long)esFSYS_open                                     ,// 26
    (const unsigned long)esFSYS_opendir                                  ,// 27
    (const unsigned long)esFSYS_partfslck                                ,// 28
    (const unsigned long)esFSYS_partfsunlck                              ,// 29
    (const unsigned long)esFSYS_pclose                                   ,// 30 *
    (const unsigned long)esFSYS_pdreg                                    ,// 31
    (const unsigned long)esFSYS_pdunreg                                  ,// 32
    (const unsigned long)esFSYS_perrclr                                  ,// 33
    (const unsigned long)esFSYS_perror                                   ,// 34
    (const unsigned long)esFSYS_pioctrl                                  ,// 35 *
    (const unsigned long)esFSYS_popen                                    ,// 36
    (const unsigned long)esFSYS_pread                                    ,// 37
    (const unsigned long)esFSYS_premove                                  ,// 38
    (const unsigned long)esFSYS_prename                                  ,// 39
    (const unsigned long)esFSYS_pwrite                                   ,// 40 *
    (const unsigned long)esFSYS_querypartupdateflag                      ,// 41
    (const unsigned long)esFSYS_readdir                                  ,// 42
    (const unsigned long)esFSYS_remove                                   ,// 43
    (const unsigned long)esFSYS_rename                                   ,// 44
    (const unsigned long)esFSYS_rewinddir                                ,// 45 *
    (const unsigned long)esFSYS_rmdir                                    ,// 46
    (const unsigned long)esFSYS_setfs                                    ,// 47
    (const unsigned long)esFSYS_statfs                                   ,// 48
    (const unsigned long)esFSYS_statpt                                   ,// 49
    (const unsigned long)esFSYS_umntparts                                ,// 50 *
    (const unsigned long)esFSYS_umntfs                                   ,// 51
};

/*SWIHandler_EXEC_t*/
const unsigned long SWIHandler_entity_EXEC[] =
{
    (const unsigned long)syscall_trap       ,//esEXEC_PCreate,
    (const unsigned long)syscall_trap       ,//esEXEC_PDel,
    (const unsigned long)syscall_trap       ,//esEXEC_PDelReq,
    (const unsigned long)syscall_trap       ,//esEXEC_Run,
};

/*SWIHandler_MODS_t*/
const unsigned long SWIHandler_entity_MODS[] =
{
    (const unsigned long)esMODS_MInstall,
    (const unsigned long)esMODS_MUninstall,
    (const unsigned long)esMODS_MOpen,
    (const unsigned long)esMODS_MClose,
    (const unsigned long)esMODS_MRead,
    (const unsigned long)esMODS_MWrite,
    (const unsigned long)esMODS_MIoctrl,
};

/*SWIHandler_RESM_t*/
const unsigned long SWIHandler_entity_RESM[] =
{
    (const unsigned long)esRESM_ROpen,
    (const unsigned long)esRESM_RClose,
    (const unsigned long)esRESM_RRead,
    (const unsigned long)esRESM_RSeek,
};

/*SWIHandler_INT_t*/
const unsigned long SWIHandler_entity_INT[] =
{
    (const unsigned long)esINT_InsISR,
    (const unsigned long)esINT_UniISR,
    (const unsigned long)esINT_InsFIR,
    (const unsigned long)esINT_UniFIR,
    (const unsigned long)esINT_SetIRQPrio,
    (const unsigned long)esINT_DisableINT,
    (const unsigned long)esINT_EnableINT,
};

/*SWIHandler_DMA_t*/
const unsigned long SWIHandler_entity_DMA[] =
{
    (const unsigned long)esDMA_Request,
    (const unsigned long)esDMA_Release,
    (const unsigned long)esDMA_Setting,
    (const unsigned long)esDMA_Start,
    (const unsigned long)esDMA_Restart,
    (const unsigned long)esDMA_Stop,
    (const unsigned long)esDMA_QueryStat,
    (const unsigned long)esDMA_QuerySrc,
    (const unsigned long)esDMA_QueryDst,
    (const unsigned long)esDMA_QueryRestCount,
    (const unsigned long)esDMA_ChangeMode,
    (const unsigned long)esDMA_EnableINT,
    (const unsigned long)esDMA_DisableINT,
    (const unsigned long)esDMA_RegDmaHdler,
    (const unsigned long)esDMA_UnregDmaHdler,
    (const unsigned long)esDMA_Information,
};

/*SWIHandler_TIME_t*/
const unsigned long SWIHandler_entity_TIME[] =
{
    (const unsigned long)syscall_trap               ,//esTIME_RequestTimer          ,
    (const unsigned long)syscall_trap               ,//esTIME_ReleaseTimer          ,
    (const unsigned long)syscall_trap               ,//esTIME_StartTimer            ,
    (const unsigned long)syscall_trap               ,//esTIME_StopTimer             ,
    (const unsigned long)syscall_trap               ,//esTIME_QuerryTimer           ,
    (const unsigned long)esTIME_GetTime             ,
    (const unsigned long)esTIME_SetTime             ,
    (const unsigned long)esTIME_GetDate             ,
    (const unsigned long)esTIME_SetDate             ,
    (const unsigned long)esTIME_RequestAlarm        ,
    (const unsigned long)esTIME_ReleaseAlarm        ,
    (const unsigned long)esTIME_StartAlarm          ,
    (const unsigned long)esTIME_StopAlarm           ,
    (const unsigned long)esTIME_QuerryAlarm         ,
    (const unsigned long)esTIME_RequestCntr         ,//esTIME_RequestCntr           ,
    (const unsigned long)esTIME_ReleaseCntr         ,//esTIME_ReleaseCntr           ,
    (const unsigned long)esTIME_StartCntr           ,//esTIME_StartCntr             ,
    (const unsigned long)esTIME_StopCntr            ,//esTIME_StopCntr              ,
    (const unsigned long)esTIME_PauseCntr           ,//esTIME_PauseCntr             ,
    (const unsigned long)esTIME_ContiCntr           ,//esTIME_ContiCntr             ,
    (const unsigned long)esTIME_SetCntrValue        ,//esTIME_SetCntrValue          ,
    (const unsigned long)esTIME_QuerryCntr          ,//esTIME_QuerryCntr            ,
    (const unsigned long)esTIME_SetCntrPrescale     ,//esTIME_SetCntrPrescale       ,
    (const unsigned long)esTIME_QuerryCntrStat      ,//esTIME_QuerryCntrStat        ,

};

/*SWIHandler_SVC_t*/
const unsigned long SWIHandler_entity_SVC[] =
{
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)syscall_trap,
    (const unsigned long)esSVC_ResourceReq,
    (const unsigned long)esSVC_ResourceRel,
};

/*SWIHandler_DEV_t*/
const unsigned long SWIHandler_entity_DEV[] =
{
    (const unsigned long)esDEV_Plugin,
    (const unsigned long)esDEV_Plugout,
    (const unsigned long)esDEV_DevReg,
    (const unsigned long)esDEV_DevUnreg,
    (const unsigned long)esDEV_Open,
    (const unsigned long)esDEV_Close,
    (const unsigned long)esDEV_Read,
    (const unsigned long)esDEV_Write,
    (const unsigned long)esDEV_Ioctl,
    (const unsigned long)esDEV_Lock,
    (const unsigned long)esDEV_Unlock,
    (const unsigned long)esDEV_Insmod,
    (const unsigned long)esDEV_Unimod,
    (const unsigned long)rt_device_register,
    (const unsigned long)rt_device_unregister,
};

/*SWIHandler_KSRV_t*/
const unsigned long SWIHandler_entity_KSRV[] =
{
    (const unsigned long)esKSRV_SysInfo,
    (const unsigned long)esKSRV_Get_Display_Hld ,//NULL,
    (const unsigned long)esKSRV_Save_Display_Hld ,//NULL,
    (const unsigned long)esKSRV_SendMsgEx,
    (const unsigned long)esKSRV_GetMsg,
    (const unsigned long)esKSRV_GetVersion,
    (const unsigned long)esKSRV_Random,      //esKSRV_Random,
    (const unsigned long)esKSRV_Reset,      //esKSRV_Reset,
    (const unsigned long)syscall_trap,      //esKSRV_GetSocID,
    (const unsigned long)syscall_trap,      //esKSRV_PowerOff,
    (const unsigned long)esKSRV_SendMsg,
    (const unsigned long)syscall_trap,      //esKSRV_GetTargetPara,
    (const unsigned long)syscall_trap,      //esKSRV_EnableWatchDog,
    (const unsigned long)syscall_trap,      //esKSRV_DisableWatchDog,
    (const unsigned long)syscall_trap,      //esKSRV_ClearWatchDog,
    (const unsigned long)syscall_trap,      //esKSRV_EnableWatchDogSrv,
    (const unsigned long)syscall_trap,      //esKSRV_DisableWatchDogSrv,
    (const unsigned long)esKSRV_memcpy,     //esKSRV_memcpy,
    (const unsigned long)esKSRV_GetLowMsg,
    (const unsigned long)esKSRV_GetHighMsg,
    (const unsigned long)syscall_trap,      //esKSRV_GetPara,
    (const unsigned long)syscall_trap,      //esKSRV_GetDramCfgPara,
    (const unsigned long)esKSRV_memset,     //esKSRV_memset,
    (const unsigned long)syscall_trap,      //esKSRV_GetAddPara,
    (const unsigned long)esKSRV_Save_Mixture_Hld,      //esKSRV_close_logo,
    (const unsigned long)esKSRV_Get_Mixture_Hld,      //esKSRV_release_logo_buf,
};

/*SWIHandler_PINS_t*/
const unsigned long SWIHandler_entity_PINS[] =
{
    (const unsigned long)esPINS_PinGrpReq,
    (const unsigned long)esPINS_PinGrpRel,
    (const unsigned long)esPINS_GetPinGrpStat,
    (const unsigned long)esPINS_GetPinStat,
    (const unsigned long)esPINS_SetPinStat,
    (const unsigned long)esPINS_SetPinIO,
    (const unsigned long)esPINS_SetPinPull,
    (const unsigned long)esPINS_SetPinDrive,
    (const unsigned long)esPINS_ReadPinData,
    (const unsigned long)esPINS_WritePinData,
    (const unsigned long)esPINS_EnbaleInt,
    (const unsigned long)esPINS_DisbaleInt,
    (const unsigned long)esPINS_QueryInt,
    (const unsigned long)esPINS_SetIntMode,
    (const unsigned long)esPINS_RegIntHdler,
    (const unsigned long)esPINS_UnregIntHdler,
    (const unsigned long)esPINS_ClearPending,
};

/*SWIHandler_CLK_t*/
const unsigned long SWIHandler_entity_CLK[] =
{
    (const unsigned long)esCLK_SetSrcFreq           ,
    (const unsigned long)esCLK_GetSrcFreq           ,
    (const unsigned long)esCLK_OpenMclk             ,
    (const unsigned long)esCLK_CloseMclk            ,
    (const unsigned long)esCLK_MclkRegCb            ,
    (const unsigned long)esCLK_MclkUnregCb          ,
    (const unsigned long)esCLK_SetMclkSrc           ,
    (const unsigned long)esCLK_GetMclkSrc           ,
    (const unsigned long)esCLK_SetMclkDiv           ,
    (const unsigned long)esCLK_GetMclkDiv           ,
    (const unsigned long)esCLK_MclkOnOff            ,
    (const unsigned long)esCLK_MclkAssert           ,
    (const unsigned long)esCLK_MclkDeassert         ,
    (const unsigned long)esCLK_MclkReset            ,
    (const unsigned long)esCLK_MclkGetRstStatus     ,
    (const unsigned long)esCLK_SysInfo              ,
    (const unsigned long)esCLK_ModInfo              ,
    (const unsigned long)esCLK_GetRound_Rate        ,
};

/*SWIHandler_MEM_t*/
const unsigned long SWIHandler_entity_MEM[] =
{
    (const unsigned long)syscall_trap,      //esMEM_DramWakeup,
    (const unsigned long)syscall_trap,      //esMEM_DramSuspend,
    (const unsigned long)esMEM_RegDramAccess,
    (const unsigned long)esMEM_UnRegDramAccess,
    (const unsigned long)syscall_trap,      //esMEM_MasterSet,
    (const unsigned long)syscall_trap,      //esMEM_MasterGet,
    (const unsigned long)syscall_trap,      //esMEM_RequestDramUsrMode,
    (const unsigned long)syscall_trap,      //esMEM_ReleaseDramUsrMode,
    (const unsigned long)syscall_trap,      //esMEM_BWEnable,
    (const unsigned long)syscall_trap,      //esMEM_BWDisable,
    (const unsigned long)syscall_trap,      //esMEM_BWGet,
    (const unsigned long)esMEM_SramReqBlk,
    (const unsigned long)esMEM_SramRelBlk,
    (const unsigned long)esMEM_SramSwitchBlk,
};

/*SWIHandler_HID_t*/
const unsigned long SWIHandler_entity_HID[] =
{
    (const unsigned long)syscall_trap,      //NULL,
    (const unsigned long)syscall_trap,      //NULL,
    (const unsigned long)syscall_trap,      //NULL,
};

/*SWIHandler_PWRMAN_t*/
const unsigned long SWIHandler_entity_PWM[] =
{
    (const unsigned long)esPWRMAN_ReqPwrmanMode    ,
    (const unsigned long)esPWRMAN_RelPwrmanMode    ,
    (const unsigned long)esPWRMAN_EnterStandby     ,
    (const unsigned long)esPWRMAN_UsrEventNotify   ,
    (const unsigned long)esPWRMAN_LockCpuFreq      ,
    (const unsigned long)esPWRMAN_UnlockCpuFreq    ,
    (const unsigned long)esPWRMAN_RegDevice        ,
    (const unsigned long)esPWRMAN_UnregDevice      ,
    (const unsigned long)esPWRMAN_GetStandbyPara   ,
    (const unsigned long)esPWRMAN_SetStandbyMode   ,
};

/*SWIHandler_CHS_t*/
const unsigned long SWIHandler_entity_CHS[] =
{
    (const unsigned long)esCHS_Uni2Char,
    (const unsigned long)esCHS_Char2Uni,
    (const unsigned long)esCHS_GetChUpperTbl,
    (const unsigned long)esCHS_GetChLowerTbl,
};

/*SWIHandler_MSTUB_t*/
const unsigned long SWIHandler_entity_MSTUB[] =
{
    (const unsigned long)syscall_trap       ,//esMSTUB_RegFuncTbl,
    (const unsigned long)syscall_trap       ,//esMSTUB_UnRegFuncTbl,
    (const unsigned long)syscall_trap       ,//esMSTUB_GetFuncEntry,
};

/*SWIHandler_INPUT_t*/
const unsigned long SWIHandler_entity_INPUT[] =
{
    (const unsigned long)esINPUT_RegDev,
    (const unsigned long)esINPUT_UnregDev,
    (const unsigned long)esINPUT_LdevGrab           ,//esINPUT_LdevGrab,
    (const unsigned long)esINPUT_LdevRelease,
    (const unsigned long)esINPUT_SendEvent,
    (const unsigned long)esINPUT_LdevFeedback,
    (const unsigned long)esINPUT_GetLdevID,
    (const unsigned long)esINPUT_LdevCtl,
};

/*SWIHandler_CONFIG_t*/
const unsigned long SWIHandler_entity_CONFIG[] =
{
    (const unsigned long)esCFG_GetKeyValue,
    (const unsigned long)esCFG_GetSecKeyCount,
    (const unsigned long)esCFG_GetSecCount,
    (const unsigned long)esCFG_GetGPIOSecKeyCount,
    (const unsigned long)esCFG_GetGPIOSecData,
    (const unsigned long)esCFG_Init_Ex,
    (const unsigned long)esCFG_Exit_Ex,
    (const unsigned long)esCFG_GetKeyValue_Ex,
    (const unsigned long)esCFG_GetSecKeyCount_Ex,
    (const unsigned long)esCFG_GetSecCount_Ex,
    (const unsigned long)esCFG_GetGPIOSecKeyCount_Ex,
    (const unsigned long)esCFG_GetGPIOSecData_Ex,
};

#ifdef CONFIG_KASAN

#define DEFINE_SYSCALL_FUNCTION(size) \
	void k__asan_load##size(unsigned long addr) \
    {  \
        __asan_load##size(addr); \
	} \
   	void k__asan_load##size##_noabort(unsigned long addr) \
    {  \
        __asan_load##size##_noabort(addr); \
	} \
    void k__asan_store##size(unsigned long addr) \
    {  \
        __asan_store##size(addr); \
	} \
   	void k__asan_store##size##_noabort(unsigned long addr) \
    {  \
        __asan_store##size##_noabort(addr); \
	}

DEFINE_SYSCALL_FUNCTION(1);
DEFINE_SYSCALL_FUNCTION(2);
DEFINE_SYSCALL_FUNCTION(4);
DEFINE_SYSCALL_FUNCTION(8);
DEFINE_SYSCALL_FUNCTION(16);

void k__asan_loadN(unsigned long addr, size_t size)
{
    __asan_loadN(addr, size);
}

void k__asan_loadN_noabort(unsigned long addr, size_t size)
{
    __asan_loadN(addr, size);
}

void k__asan_storeN(unsigned long addr, size_t size)
{
    __asan_loadN(addr, size);
}

void k__asan_storeN_noabort(unsigned long addr, size_t size)
{
    __asan_loadN(addr, size);
}

void k__asan_poison_stack_memory(const void* addr, size_t size)
{
    __asan_poison_stack_memory(addr, size);
}

void k__asan_unpoison_stack_memory(const void* addr, size_t size)
{
    __asan_poison_stack_memory(addr, size);
}

void k__asan_alloca_poison(const void* addr, size_t size)
{
    __asan_alloca_poison(addr, size);
}

void k__asan_alloca_unpoison(const void* addr, size_t size)
{
    __asan_allocas_unpoison(addr, size);
}

void k__asan_handle_no_return(void)
{
	__asan_handle_no_return();
}

void k__asan_register_globals(void *globals, size_t size)
{
    __asan_register_globals(globals, size);
}

void k__asan_unregister_globals(void *globals, size_t size)
{
    __asan_unregister_globals(globals, size);
}

const unsigned long SWIHandler_entity_KASANOPS[] =
{
    (const unsigned long) k__asan_load1                  ,
    (const unsigned long) k__asan_load2                  ,
    (const unsigned long) k__asan_load4                  ,
    (const unsigned long) k__asan_load8                  ,
    (const unsigned long) k__asan_load16                 ,
    (const unsigned long) k__asan_loadN                  ,
    (const unsigned long) k__asan_store1                 ,
    (const unsigned long) k__asan_store2                 ,
    (const unsigned long) k__asan_store4                 ,
    (const unsigned long) k__asan_store8                 ,
    (const unsigned long) k__asan_store16                ,
    (const unsigned long) k__asan_storeN                 ,
    (const unsigned long) k__asan_load1_noabort          ,
    (const unsigned long) k__asan_load2_noabort          ,
    (const unsigned long) k__asan_load4_noabort          ,
    (const unsigned long) k__asan_load8_noabort          ,
    (const unsigned long) k__asan_load16_noabort         ,
    (const unsigned long) k__asan_loadN_noabort          ,
    (const unsigned long) k__asan_store1_noabort         ,
    (const unsigned long) k__asan_store2_noabort         ,
    (const unsigned long) k__asan_store4_noabort         ,
    (const unsigned long) k__asan_store8_noabort         ,
    (const unsigned long) k__asan_store16_noabort        ,
    (const unsigned long) k__asan_storeN_noabort         ,
    (const unsigned long) k__asan_poison_stack_memory    ,
    (const unsigned long) k__asan_alloca_poison          ,
    (const unsigned long) k__asan_alloca_unpoison        ,
	(const unsigned long) k__asan_handle_no_return       ,
	(const unsigned long) k__asan_register_globals       ,
	(const unsigned long) k__asan_unregister_globals     ,
};
#endif

/*SWIHandler_SWIT_t*/
const unsigned long melis_syscall_table[] =
{
    (const unsigned long)SWIHandler_entity_SIOS         ,   // void *SWIHandler_SIOS        ;
    (const unsigned long)SWIHandler_entity_KRNL         ,   // void *SWIHandler_KRNL        ;
    (const unsigned long)SWIHandler_entity_MEMS         ,   // void *SWIHandler_MEMS        ;
    (const unsigned long)SWIHandler_entity_FSYS         ,   // void *SWIHandler_FSYS        ;
    (const unsigned long)SWIHandler_entity_EXEC         ,   // void *SWIHandler_EXEC        ;
    (const unsigned long)SWIHandler_entity_MODS         ,   // void *SWIHandler_MODS        ;
    (const unsigned long)SWIHandler_entity_RESM         ,   // void *SWIHandler_RESM        ;
    (const unsigned long)SWIHandler_entity_INT          ,   // void *SWIHandler_INT         ;
    (const unsigned long)SWIHandler_entity_DMA          ,   // void *SWIHandler_DMA         ;
    (const unsigned long)SWIHandler_entity_TIME         ,   // void *SWIHandler_TIME        ;
    (const unsigned long)NULL                           ,   // void *SWIHandler_IPIPE       ;
    (const unsigned long)NULL                           ,   // void *SWIHandler_PWRS        ;
    (const unsigned long)NULL                           ,   // void *SWIHandler_ERRS        ;
    (const unsigned long)SWIHandler_entity_SVC          ,   // void *SWIHandler_SVC         ;
    (const unsigned long)SWIHandler_entity_DEV          ,   // void *SWIHandler_DEV         ;
    (const unsigned long)SWIHandler_entity_KSRV         ,   // void *SWIHandler_KSRV        ;
    (const unsigned long)SWIHandler_entity_PINS         ,   // void *SWIHandler_PINS        ;
    (const unsigned long)SWIHandler_entity_CLK          ,   // void *SWIHandler_CLK         ;
    (const unsigned long)SWIHandler_entity_MEM          ,   // void *SWIHandler_MEM         ;
    (const unsigned long)SWIHandler_entity_HID          ,   // void *SWIHandler_HID         ;
    (const unsigned long)SWIHandler_entity_PWM          ,   // void *SWIHandler_PWM         ;
    (const unsigned long)SWIHandler_entity_CHS          ,   // void *SWIHandler_CHS         ;
    (const unsigned long)SWIHandler_entity_MSTUB        ,   // void *SWIHandler_MSTUB       ;
    (const unsigned long)SWIHandler_entity_INPUT        ,   // void *SWIHandler_INPUT       ;
    (const unsigned long)SWIHandler_entity_CONFIG       ,   // void *SWIHandler_CONFIG      ;
    (const unsigned long)SWIHandler_entity_PTHREAD      ,   // void *SWIHandler_PTHREAD     ;
    (const unsigned long)SWIHandler_entity_NLIBOPS      ,   // void *SWIHandler_NLIBFOPS    ;

#ifdef CONFIG_NETWORK
    (const unsigned long)SWIHandler_entity_LWIP         ,   // void *SWIHandler_LWIP        ;
#endif

#ifdef CONFIG_KASAN
    (const unsigned long)SWIHandler_entity_KASANOPS     ,   // void *SWIHandler_KASANOPS    ;
#endif
};

#ifdef CONFIG_RISCV
unsigned long esSYSCALL_function(irq_regs_t *regs, void *reserved)
{
    unsigned long child_addr_offset  = ((regs->x17 >> 8) & 0xFFul) * sizeof(unsigned long);//   a7
    unsigned long func_addr_offset   = ((regs->x17 >> 0) & 0xFFul) * sizeof(unsigned long);//   a7
    unsigned long child_array_addr   = *((long*)((long)(&melis_syscall_table) + child_addr_offset)); // L P 64 mode or I L P 3 2 mode
    unsigned long pfunc_addr         = child_array_addr ? (child_array_addr + func_addr_offset) : 0;

    __pSWI_t func = (__pSWI_t)((0 != pfunc_addr) ? (*((unsigned long*)pfunc_addr)) : 0);

    if(func)
    {
        regs->x1    = regs->epc + 4;
        regs->epc  = (*((unsigned long*)pfunc_addr));
    }
    else
    {
        printf("fatal error, invalid syscall\n");
        while(1);
    }
    return 0;
}
#endif
