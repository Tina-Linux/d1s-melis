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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <sys/types.h>

#ifdef __ANDROID__
#include <cutils/properties.h>
#endif


#include "cdc_log.h"
#include "CdcUtil.h"

int CdcIonGetMemType(void)
{
    MEMORY_TYPE eMemoryType = MEMORY_NORMAL;

#ifdef CONF_USE_IOMMU
    eMemoryType = MEMORY_IOMMU;
#else

    #ifdef __ANDROID__
    char prop_value[512];
    property_get("ro.kernel.iomem.type", prop_value, "0xaf01");
    logv("++++ prop_value: %s", prop_value);
    if(strcmp(prop_value, "0xaf10")==0)
    {
        eMemoryType = MEMORY_IOMMU;
    }
    #endif
#endif

    return eMemoryType;
}
unsigned long CdcIonGetPhyAdr(int fd, uintptr_t handle)
{
    if(CdcIonGetMemType() == MEMORY_IOMMU)
    {
        return 1;
    }
    else
    {
        int ret = 0;
        ion_custom_data_t custom_data;
        cdc_sunxi_phys_data phys_data;
        memset(&phys_data, 0, sizeof(cdc_sunxi_phys_data));
        CEDARC_UNUSE(phys_data.size);
        custom_data.aw_cmd = ION_IOC_SUNXI_PHYS_ADDR;
        phys_data.handle = (aw_ion_user_handle_t)handle;
        custom_data.aw_arg = (unsigned long)&phys_data;
        ret = ioctl(fd, AW_MEM_ION_IOC_CUSTOM, &custom_data);
        if(ret < 0)
        {
            loge("CdcIonGetPhyAdr AW_MEM_ION_IOC_CUSTOM error\n");
            return 0;
        }
        return phys_data.phys_addr;
    }
}

int CdcIonGetFd(int fd, uintptr_t handle)
{
    int ret;
    ion_fd_data_t fd_data;
    fd_data.handle = (aw_ion_user_handle_t)handle;
    ret = ioctl(fd, AW_MEM_ION_IOC_MAP, &fd_data);
    if(ret)
    {
        loge("CdcIonGetFd map other dev's handle err, ret %d, dmabuf fd 0x%08x\n",
                                                                ret, fd_data.aw_fd);
        return -1;
    }
    return fd_data.aw_fd;
}

int CdcIonImport(int fd, int share_fd, aw_ion_user_handle_t *ion_handle)
{
    int ret;
    ion_fd_data_t fd_data;

    memset(&fd_data, 0, sizeof(ion_fd_data_t));
    fd_data.aw_fd = share_fd;
    ret = ioctl(fd, AW_MEM_ION_IOC_IMPORT, &fd_data);
    if(ret)
    {
        loge("CdcIonImport get ion_handle err, ret %d\n",ret);
        return -1;
    }

    *ion_handle = fd_data.handle;
    return 0;
}

int CdcIonFree(int fd, aw_ion_user_handle_t ion_handle)
{
    int ret;
    ion_handle_data_t sIonHandleData;

    memset(&sIonHandleData, 0, sizeof(ion_handle_data_t));
    sIonHandleData.handle = ion_handle;
    ret = ioctl(fd, AW_MEM_ION_IOC_FREE, &sIonHandleData);
    if(ret)
    {
        loge("CdcIonFree free ion_handle err, ret %d\n",ret);
        return -1;
    }
    return 0;
}

int CdcIonOpen(void)
{
    int fd = open("/dev/ion", O_RDWR);
    if (fd < 0)
        loge("open /dev/ion failed!\n");
    return fd;
}

int CdcIonClose(int fd)
{
    int ret = close(fd);
    if (ret < 0)
    {
        loge("CdcIonClose failed with code %d: %s\n", ret, strerror(errno));
        return -errno;
    }
    return ret;
}

#if 0
int CdcIonMmap(int buf_fd, size_t len, unsigned char **pVirAddr)
{
    CEDARC_UNUSE(CdcIonMmap);
    /* mmap to user */
    *pVirAddr = mmap(NULL, len, \
        PROT_READ|PROT_WRITE, MAP_SHARED, buf_fd, 0);
    if(MAP_FAILED == pVirAddr)
    {
        loge("CdcIonMmap failed: %s\n", strerror(errno));
        return -errno;
    }

    return 0;
}

int CdcIonUnmap(size_t len, unsigned char *pVirAddr)
{
    CEDARC_UNUSE(CdcIonUnmap);
    int ret;
    ret = munmap((void*)pVirAddr, len);
    if(ret)
    {
        loge("CdcIonUnmap failed: %s\n", strerror(errno));
        return -errno;
    }

    return 0;
}
#endif

unsigned long CdcIonGetTEEAdr(int fd, uintptr_t handle)
{
    int ret = 0;
    cdc_sunxi_phys_data tee_data;
    ion_custom_data_t custom_data;

    memset(&tee_data, 0, sizeof(cdc_sunxi_phys_data));
    tee_data.handle = (aw_ion_user_handle_t)handle;
    CEDARC_UNUSE(tee_data.size);
    custom_data.aw_cmd = ION_IOC_SUNXI_TEE_ADDR;
    custom_data.aw_arg = (unsigned long)&tee_data;
    ret = ioctl(fd, AW_MEM_ION_IOC_CUSTOM, &custom_data);
    if(ret)
    {
        loge("ION_IOC_CUSTOM get TEE addr err, ret %d\n", ret);
        return 0;
    }
    return tee_data.phys_addr;
}
