/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                File system module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsys_debug.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-8
* Descript: debug sevices for file system.
* Update  : date                auther      ver     notes
*           2011-3-8 14:26:42   Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "fsys_debug.h"
#include "fsys_libs.h"
#include "rtthread.h"
#include "elibs_stdio.h"

#if     FSYS_DEBUG_ON

debug_para_msg_t debug_para;

__s32 esFSYS_fsdbg(const char *cmd, const char *para)
{
    if ((rt_strncmp(cmd, "log", 3) == 0) || (rt_strncmp(cmd, "LOG", 3) == 0))
    {
        fs_log_parse_option(para);
    }
    else if ((rt_strncmp(cmd, "dump", 4) == 0) || (rt_strncmp(cmd, "DUMP", 4) == 0))
    {
        fs_dump_parse_option(para);
    }
    else if ((rt_strncmp(cmd, "timer", 5) == 0) || (rt_strncmp(cmd, "TIMER", 5) == 0))
    {
        fs_timer_parse_option(para);
    }
    else if ((rt_strncmp(cmd, "fatchunk", 8) == 0) || (rt_strncmp(cmd, "FATCHUNK", 8) == 0))
    {
        ES_FILE *fatfile;

        fatfile = esFSYS_fopen(para, "r");
        if (!fatfile)
        {
            __inf("open file %s fail!", para);
        }
        else
        {
            esFSYS_fioctrl(fatfile, FS_IOC_USR_DEBUG_FATCHUNK, 0, (void *)para);
            esFSYS_fclose(fatfile);
        }
    }
    else if ((rt_strncmp(cmd, "showopenfiles", 13) == 0) || (rt_strncmp(cmd, "SHOWOPENFILES", 13) == 0))
    {
        __debug_dump_openfiles();
    }
    else if ((rt_strncmp(cmd, "fmt.", 4) == 0) || (rt_strncmp(cmd, "FMT.", 4) == 0))
    {
        esFSYS_format(para, cmd + 4, 0);
    }
    else if ((rt_strncmp(cmd, "help", 4) == 0) || (rt_strncmp(cmd, "HELP", 4) == 0))
    {
        __inf("==================================================================================================");
        __inf("=                                     Filesystem debuger helper.                                 =");
        __inf("=                                          2008.12.30@chenlm                                     =");
        __inf("=------------------------------------------------------------------------------------------------=");
        __inf("= fs <log>   <[+/-]debug>     : turn on / off debug print                                        =");
        __inf("= fs <log>   <[+/-]verbose>   : turn on / off more detail print                                  =");
        __inf("= fs <log>   <[+/-]quiet>     : turn on / off a little print                                     =");
        __inf("= fs <log>   <[+/-]trace0>    : turn on / off vfs trace print for key functions                  =");
        __inf("= fs <log>   <[+/-]trace1>    : turn on / off part trace print for key functions                 =");
        __inf("= fs <log>   <[+/-]trace2>    : turn on / off dev trace print for key functions                  =");
        __inf("= fs <log>   <[+/-]trace01>   : turn on / off fs trace print for key functions                   =");
        __inf("= fs <log>   <[+/-]line>      : turn on / off line print                                         =");
        __inf("= fs <log>   <[+/-]objname>   : turn on / off object name print like file name                   =");
        __inf("= fs <log>   <[+/-]function>  : turn on / off func name print                                    =");
        __inf("= fs <log>   <[+/-]tracef:xxx>: turn on / off and add/del specific func trace                    =");
        __inf("= fs <log>   <traceinfo>      : show the state of trace                                          =");
        __inf("=                                                                                                =");
        __inf("= fs <timer> [on/off]         : turn on/off the timer                                            =");
        __inf("= fs <timer> [request]        : get system timer                                                 =");
        __inf("= fs <timer> [release]        : release system timer                                             =");
        __inf("= fs <timer> [0x--------]     : set timer hdle by 0x--------                                     =");
        __inf("=                                                                                                =");
        __inf("= fs <dump>  <+dmem:name,offset,len>  : add memory dump item to show mem content (hex)           =");
        __inf("= fs <dump>  <-dmem>          : clear all memory dump items                                      =");
        __inf("= fs <dump>  <[+/-]sbi>       : turn on / off show sb inode info                                 =");
        __inf("= fs <dump>  <[+/-]lrui>      : turn on / off show lru inode info                                =");
        __inf("= fs <dump>  <[+/-]sbd>       : turn on / off show sb dentry info                                =");
        __inf("= fs <dump>  <[+/-]lrud>      : turn on / off show lru dentry info                               =");
        __inf("= fs <dump>  <[+/-]lruf>      : turn on / off show lru and cid fat cache info                    =");
        __inf("= fs <dump>  <[+/-]freep>     : turn on / off show free page info                                =");
        __inf("= fs <dump>  <[+/-]lrup>      : turn on / off show lru page info                                 =");
        __inf("= fs <dump>  <[+/-]bhlist>    : turn on / off show bh list info                                  =");
        __inf("= fs <dump>  <[+/-]bhinfo>    : turn on / off show glance of bhs                                 =");
        __inf("= fs <dump>  <[+/-]pinfo>     : turn on / off show glance of pages                               =");
        __inf("= fs <dump>  <[+/-]allon>     : turn on / off the high priorit dump on-shower                    =");
        __inf("= fs <dump>  <[+/-]alloff>    : turn on / off the high priorit dump off-shower                   =");
        __inf("= fs <dump>  <[+/-]detail>    : turn on / off show more detail infos for dump                    =");
        __inf("=                                                                                                =");
        __inf("= fs <fatchunk> <filename>    : show the fat cluster chunks                                      =");
        __inf("= fs <showopenfiles>          : show current open files and dirs                                 =");
        __inf("=                                                                                                =");
        __inf("= fs <ins>   <[fat]/[ntfs]>   : install file system by hand (please wait)                        =");
        __inf("= fs <unins> <[fat]/[ntfs]>   : uninstall file system by hand (please wait)                      =");
        __inf("= fs <fmt.[fat/ntfs]> <part>  : format the partition by fat or ntfs                              =");
        __inf("==================================================================================================");
        __inf("");
    }
    else
    {
        __inf("invalid input.");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

#else

__s32 esFSYS_fsdbg(const char *cmd, const char *para)
{
    return EPDK_FAIL;
}

#endif  /* FSYS_DEBUG_ON */
