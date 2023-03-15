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
#include "cdc_log.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <dfs_posix.h>

#include <CdcSysinfo.h>

#define NODE_DDR_FREQ    "/sys/class/devfreq/sunxi-ddrfreq/cur_freq"
static int readNodeValue(const char *node, char * strVal, size_t size)
{
    int ret = -1;
    int fd = open(node, O_RDONLY);
    if (fd >= 0)
    {
        read(fd, strVal, size);
        close(fd);
        ret = 0;
    }
    return ret;
}

//* get current ddr frequency, if it is too slow, we will cut some spec off.
int CdcGetDramFreq(void)
{
    CEDARC_UNUSE(CdcGetDramFreq);

    char freq_val[8] = {0};
    if (!readNodeValue(NODE_DDR_FREQ, freq_val, 8))
    {
        return atoi(freq_val);
    }

    // unknow freq
    return -1;
}


int CdcGetSysinfoChipId(void)
{
    int ret = 0;
    int dev = 0;
    char buf[16]={0};

    dev = open("/dev/sunxi_soc_info", O_RDONLY);
    if (dev < 0)
    {
        logv("cannot open /dev/sunxi_soc_info\n");
        return  SI_CHIP_UNKNOWN;
    }

    ret = ioctl(dev, 3, buf);
    if(ret < 0)
    {
        loge("ioctl err!\n");
        close(dev);
        return  SI_CHIP_UNKNOWN;
    }

    logd("%s\n", buf);

    close(dev);

    if(!strncmp(buf, "00000000", 8) ||
       !strncmp(buf, "00000081", 8))
    {
        ret = SI_CHIP_H3;
    }
    else if(!strncmp(buf, "00000042", 8) ||
            !strncmp(buf, "00000083", 8))
    {
        ret = SI_CHIP_H2PLUS;
    }
    else if(!strcmp(buf, "H2"))    // deprecated
    {
        ret = SI_CHIP_H2;
    }
    else if(!strcmp(buf, "H3s"))// deprecated
    {
        ret = SI_CHIP_H3s;
    }
    else
    {
        ret = SI_CHIP_UNKNOWN;
    }
    return ret;
}
