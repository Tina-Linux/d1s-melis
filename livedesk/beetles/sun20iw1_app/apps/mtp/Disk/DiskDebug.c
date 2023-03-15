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
#include "Disk.h"
typedef struct
{
    const char *name;
    uint32_t value;
} ValueEntry;

static const ValueEntry sDiskType[] =
{
    { "ext4",       DISK_FS_TYPE_EXT4 },
    { "vfat",       DISK_FS_TYPE_FAT },
    { "fuse",       DISK_FS_TYPE_FUSE },
    { "proc",       DISK_FS_TYPE_PROC },
    { "sysfs",      DISK_FS_TYPE_SYSFS },
    { "cgroup",     DISK_FS_TYPE_CGROUPFS },
    { "tmpfs",      DISK_FS_TYPE_TMPFS },
    { "debugfs",        DISK_FS_TYPE_DEBUGFS },
    { "devtmpfs",       DISK_FS_TYPE_DEVTMPFS },
    { "devpts",     DISK_FS_TYPE_DEVPTS },
    { "rootfs",     DISK_FS_TYPE_ROOTFS },
    { "squashfs",       DISK_FS_TYPE_SQUASHFS },
};

static const char *getValueName(uint32_t value, const ValueEntry *table)
{
    const ValueEntry *entry = table;
    while (entry->name)
    {
        if (entry->value == value)
        {
            return entry->name;
        }
        entry++;
    }
    return "UNKNOWN";
}
const char *getDiskType(DiskType type)
{
    return getValueName(type, sDiskType);
}
