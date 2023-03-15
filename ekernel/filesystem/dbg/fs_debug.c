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
#include "fsys_debug.h"
#include "fsys_libs.h"
#include "rtthread.h"
#include "elibs_stdio.h"

#if     FSYS_DEBUG_ON

debug_para_msg_t debug_para;

__s32 esFSYS_fsdbg(const char *cmd, const char *para)
{
    if ((strncmp(cmd, "log", 3) == 0) || (strncmp(cmd, "LOG", 3) == 0))
    {
        fs_log_parse_option(para);
    }
    else if ((strncmp(cmd, "dump", 4) == 0) || (strncmp(cmd, "DUMP", 4) == 0))
    {
        fs_dump_parse_option(para);
    }
    else if ((strncmp(cmd, "timer", 5) == 0) || (strncmp(cmd, "TIMER", 5) == 0))
    {
        fs_timer_parse_option(para);
    }
    else if ((strncmp(cmd, "fatchunk", 8) == 0) || (strncmp(cmd, "FATCHUNK", 8) == 0))
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
    else if ((strncmp(cmd, "showopenfiles", 13) == 0) || (strncmp(cmd, "SHOWOPENFILES", 13) == 0))
    {
        __debug_dump_openfiles();
    }
    else if ((strncmp(cmd, "fmt.", 4) == 0) || (strncmp(cmd, "FMT.", 4) == 0))
    {
        esFSYS_format(para, cmd + 4, 0);
    }
    else if ((strncmp(cmd, "help", 4) == 0) || (strncmp(cmd, "HELP", 4) == 0))
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
