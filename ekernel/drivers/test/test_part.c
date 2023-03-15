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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include <blkpart.h>

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

#define HEXDUMP_LINE_CHR_CNT 16
static int hexdump(const unsigned char *buf, int bytes)
{
    unsigned char line[HEXDUMP_LINE_CHR_CNT] = {0};
    int addr;

    for (addr = 0; addr < bytes; addr += HEXDUMP_LINE_CHR_CNT)
    {
        int len = MIN(bytes - addr, HEXDUMP_LINE_CHR_CNT), i;

        memcpy(line, buf + addr, len);
        memset(line + len, 0, HEXDUMP_LINE_CHR_CNT - len);

        /* print addr */
        printf("0x%.8X: ", addr);
        /* print hex */
        for (i = 0; i < HEXDUMP_LINE_CHR_CNT; i++)
        {
            if (i < len)
            {
                printf("%.2X ", line[i]);
            }
            else
            {
                printf("   ");
            }
        }
        /* print char */
        printf("|");
        for (i = 0; i < HEXDUMP_LINE_CHR_CNT; i++)
        {
            if (i < len)
            {
                if (line[i] >= 0x20 && line[i] <= 0x7E)
                {
                    printf("%c", line[i]);
                }
                else
                {
                    printf(".");
                }
            }
            else
            {
                printf(" ");
            }
        }
        printf("|\n");
    }
    return 0;
}


static int cmd_part_read(int argc, const char **argv)
{
    int ret;
    int size = 0;
    unsigned int offset;
    char *err = NULL;

    if (argc < 4)
    {
        printf("Unsage: part_read part_name offset size.\n");
        return -1;
    }

    rt_device_t dev = rt_device_find(argv[1]);
    if (!dev)
    {
        printf("can not find device\n");
        ret = -1;
        goto out;
    }

    offset = strtoul(argv[2], &err, 0);
    if (*err)
    {
        ret = -1;
        goto out;
    }

    size = strtoul(argv[3], &err, 0);
    if (*err || size > 1024 * 512)
    {
        ret = -1;
        goto out;
    }

    char *buf = (char *)malloc(size);
    if (!buf)
    {
        printf("allocate memnory failed\n");
        ret = -1;;
        goto out;
    }
    memset(buf, 0, size);

    printf("read data:\n");
    ret = dev->read(dev, offset, buf, size);
    if (ret <= 0)
    {
        printf("spinor read data failed\n");
        ret = -1;
        goto error;
    }
    hexdump(buf, size);

error:
    free(buf);
out:
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_part_read, __cmd_part_read, part read test);

static int cmd_part_erase(int argc, const char **argv)
{
    int ret;
    int size = 0;
    unsigned int offset;
    char *err = NULL;

    if (argc < 4)
    {
        printf("Unsage: part_erase part_name offset size.\n");
        return -1;
    }

    rt_device_t dev = rt_device_find(argv[1]);
    if (!dev)
    {
        printf("can not find device\n");
        ret = -1;
        goto out;
    }

    offset = strtoul(argv[2], &err, 0);
    if (*err)
    {
        ret = -1;
        goto out;
    }

    size = strtoul(argv[3], &err, 0);
    if (*err || size > 1024 * 512)
    {
        ret = -1;
        goto out;
    }

    blk_dev_erase_t erase_sector;
    memset(&erase_sector, 0, sizeof(blk_dev_erase_t));
    erase_sector.addr = offset;
    erase_sector.len = size;
    ret = dev->control(dev, BLOCK_DEVICE_CMD_ERASE_SECTOR, &erase_sector);
    if (ret)
    {
        printf("spinor erase data failed\n");
        ret = -1;
        goto error;
    }

error:
out:
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_part_erase, __cmd_part_erase, part erase test);

static int cmd_part_write(int argc, const char **argv)
{
    int ret;
    int size = 0;
    unsigned int offset;
    char *err = NULL;
    int value = 0;

    if (argc < 5)
    {
        printf("Unsage: part_write part_name offset size value.\n");
        return -1;
    }

    rt_device_t dev = rt_device_find(argv[1]);
    if (!dev)
    {
        printf("can not find device\n");
        ret = -1;
        goto out;
    }

    offset = strtoul(argv[2], &err, 0);
    if (*err)
    {
        ret = -1;
        goto out;
    }

    size = strtoul(argv[3], &err, 0);
    if (*err || size > 1024 * 512)
    {
        ret = -1;
        goto out;
    }

    value = strtoul(argv[4], &err, 0);
    if (*err)
    {
        ret = -1;
        goto out;
    }

    char *buf = (char *)malloc(size);
    if (!buf)
    {
        printf("allocate memnory failed\n");
        ret = -1;;
        goto out;
    }
    memset(buf, value, size);

    ret = dev->write(dev, offset, buf, size);
    if (ret <= 0)
    {
        printf("spinor write data failed\n");
        ret = -1;
        goto error;
    }

error:
    free(buf);
out:
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_part_write, __cmd_part_write, part write test);
