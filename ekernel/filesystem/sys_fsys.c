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
#include "sys_fsys_i.h"
#include <kconfig.h>

/*
*********************************************************************************************************
*                                           FSYS INIT
*
* Description: file system initialize.
*
* Arguments  : NONE
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
uint32_t fsys_init(void)
{
    fsys_vpart_init();

    fsys_vfs_init();
    /* initialize dev file system */
#ifdef CONFIG_DEVFS
    fsys_devfs_init();
#endif

    /* initialize cdfs file system */
#ifdef CONFIG_CDFS
    fsys_cdfs_init();
#endif

    /* initialize udffs file system */
#ifdef CONFIG_UDFFS
    fsys_udffs_init();
#endif

    /* initialize exFAT file system */
#ifdef CONFIG_EXFAT
    fsys_exfatfs_init();
#endif

    /* initialize ntfs file system */
#ifdef CONFIG_NTFS
    fsys_ntfs_init();
#endif

    /* initialize fat file system */
#ifdef CONFIG_FAT
    fsys_fatfs_init();
#endif

    /* initialize mini file system */
#ifdef CONFIG_MINFS
    fsys_minfs_init();
#endif
    fsys_dmspart_init();
    fsys_rawpart_init();
    fsys_dospart_init();

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           FSYS EXIT
*
* Description: file system exit.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 FSYS_Exit(void)
{
    fsys_dospart_exit();
    fsys_rawpart_exit();
    fsys_dmspart_exit();

    /* exit udf file system */
#ifdef CONFIG_UDFFS
    fsys_udffs_exit();
#endif

    /* exit cdfs file system */
#ifdef CONFIG_CDFS
    fsys_cdfs_exit();
#endif

    /* exit exfat file system */
#ifdef CONFIG_EXFAT
    fsys_exfatfs_exit();
#endif

    /* exit ntfs file system */
#ifdef CONFIG_NTFS
    fsys_ntfs_exit();
#endif

    /* exit fat file system */
#ifdef CONFIG_FAT
    fsys_fatfs_exit();
#endif

    /* exit mini file system */
    fsys_minfs_exit();

    /* exit dev file system */
    fsys_devfs_exit();

    fsys_vfs_exit();
    fsys_vpart_exit();

    return EPDK_OK;
}