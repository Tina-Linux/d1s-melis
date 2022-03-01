#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include <sunxi_drv_spinor.h>

static int init_flag = 0;

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

#define HEXDUMP_LINE_CHR_CNT 16
int hexdump(const unsigned char *buf, int bytes)
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

static int cmd_spinor_test(int argc, const char **argv)
{
    int ret;
    rt_device_t dev = rt_device_find("spinor");
    if (!dev)
    {
        printf("can not find device\n");
        ret = -1;
        goto out;
    }

    if (init_flag == 0)
    {
        dev->init(dev);
        init_flag = 1;
    }
    char *buf = (char *)malloc(512);
    if (buf == 0)
    {
        printf("allocate memnory failed\n");
        ret = -1;;
        goto out;
    }
    memset(buf, 0, 512);

    printf("read data:\n");
    ret = dev->read(dev, 3 * 1024 * 1024, buf, 512);
    if (ret <= 0)
    {
        printf("spinor read data failed\n");
        ret = -1;
        goto error;
    }
    hexdump(buf, 512);
    printf("\n");

    printf("erase data:\n");
    blk_dev_erase_t erase_sector;
    memset(&erase_sector, 0, sizeof(blk_dev_erase_t));
    erase_sector.addr = 3 * 1024 * 1024;
    erase_sector.len = 4 * 1024;
    ret = dev->control(dev, BLOCK_DEVICE_CMD_ERASE_SECTOR, &erase_sector);
    if (ret)
    {
        printf("spinor erase data failed\n");
        ret = -1;
        goto error;
    }

    printf("write data:\n");
    memset(buf, 0xa5, 512);
    ret = dev->write(dev, 3 * 1024 * 1024, buf, 512);
    if (ret <= 0)
    {
        printf("spinor write data failed\n");
        ret = -1;
        goto error;
    }

    printf("after write data:\n");
    memset(buf, 0xa5, 512);
    ret = dev->read(dev, 3 * 1024 * 1024, buf, 512);
    if (ret <= 0)
    {
        printf("spinor read data failed\n");
        ret = -1;
        goto error;
    }
    hexdump(buf, 512);

error:
    free(buf);
out:
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_spinor_test, __cmd_spinor_test, spinor_test);
