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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>

#include <init.h>

#include <blkpart.h>
#include <sdmmc/hal_sdhost.h>
#include <sdmmc/card.h>
#include <sdmmc/sys/sys_debug.h>

#include <typedef.h>
#include <kapi.h>
#include "mbr.h"

#ifdef CONFIG_SUPPORT_SDMMC_CACHE
#include "sdmmc_cache.h"
#endif

#define DETECT_BY_GPIO

#ifndef CONFIG_SDC_DMA_BUF_SIZE
#define SDC_ALIGN_DMA_BUF_SIZE  (64*1024)
#else
#define SDC_ALIGN_DMA_BUF_SIZE  (CONFIG_SDC_DMA_BUF_SIZE * 1024)
#endif

#define SDXC_MAX_TRANS_LEN      SDC_ALIGN_DMA_BUF_SIZE

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))
#endif

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

void card_detect(uint32_t present, uint16_t sdc_id);

int32_t mmc_test_init(uint32_t host_id, SDC_InitTypeDef *sdc_param, uint32_t scan);
struct mmc_card *mmc_card_open(uint8_t card_id);
int32_t mmc_card_close(uint8_t card_id);
int32_t mmc_block_read(struct mmc_card *card, uint8_t *buf, uint64_t sblk, uint32_t nblk);
int32_t mmc_block_write(struct mmc_card *card, const uint8_t *buf, uint64_t sblk, uint32_t nblk);

static rt_err_t sunxi_sdmmc_init(rt_device_t dev)
{
    int ret = -1;
    int host_id = (long)dev->user_data;
    dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
	__s32 internal_card	= 0x00;

    SDC_InitTypeDef sdc_param = {0};
    sdc_param.debug_mask = (ROM_INF_MASK | \
                            ROM_WRN_MASK | ROM_ERR_MASK | ROM_ANY_MASK);
	esCFG_GetKeyValue("sdcard_global", "internal_card", (int32_t *)&internal_card, 1);
	
	if(((internal_card >> host_id) & 0x01) == 1)
	{
		sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
		printf("cd_mode CARD_ALWAYS_PRESENT!\n");
	}
	else
	{
#ifndef DETECT_BY_GPIO
    sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
#else
    sdc_param.cd_mode = CARD_DETECT_BY_GPIO_IRQ;
#endif
	}
    sdc_param.cd_cb = &card_detect;
    sdc_param.dma_use = 1;

    if (mmc_test_init(host_id, &sdc_param, 1))
    {
        dev->flag &= ~RT_DEVICE_FLAG_ACTIVATED;
        printf("init sdmmc failed!\n");
        return ret;
    }
	printf("host_id =%d!\n",host_id);

    //dev->user_data = (void *)host_id;
    return 0;
}

static rt_size_t sunxi_sdmmc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    int err = -1;
    ssize_t ret, sz = 0;
    struct rt_device_blk_geometry geometry;
    unsigned long long total_bytes;
    char *sector_buf = NULL;
    uint8_t *data = buffer;

    struct mmc_card *card = mmc_card_open((long)dev->user_data);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return -EIO;
    }

    if (size == 0)
    {
        return 0;
    }

    memset(&geometry, 0, sizeof(struct rt_device_blk_geometry));
    err = dev->control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    if (err)
    {
        return -EIO;
    }
    total_bytes = geometry.bytes_per_sector * geometry.sector_count;

    if (pos >= total_bytes)
    {
        printf("read offset %lu over part size %llu\n", pos, total_bytes);
        return 0;
    }

    if (pos + size > total_bytes)
    {
        printf("over limit: offset %lu + size %lu over %llu\n",
               pos, size, total_bytes);
    }

    size = MIN(total_bytes - pos, size);
    pr_debug("off 0x%lx size %lu\n", pos, size);

    if (pos % geometry.bytes_per_sector || size % geometry.bytes_per_sector)
    {
        sector_buf = malloc(geometry.bytes_per_sector);
        if (!sector_buf)
        {
            return -ENOMEM;
        }
        memset(sector_buf, 0, geometry.bytes_per_sector);
    }

    /**
     * Step 1:
     * read the beginning data that not align to block size
     */
    if (pos % geometry.bytes_per_sector)
    {
        uint32_t addr, poff, len;

        addr = ALIGN_DOWN(pos, geometry.bytes_per_sector);
        poff = pos - addr;
        len = MIN(geometry.bytes_per_sector - poff, size);

        pr_debug("offset %lu not align %u, fix them before align read\n",
                 pos, geometry.bytes_per_sector);
        pr_debug("step1: read page data from addr 0x%x\n", addr);
        ret = mmc_block_read(card, sector_buf, addr / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page offset 0x%x and len %u\n",
                 poff, len);
        memcpy(data, sector_buf + poff, len);

        pos += len;
        buffer += len;
        sz += len;
        size -= len;
    }

    /**
     * Step 2:
     * read data that align to block size
     */
    while (size >= geometry.bytes_per_sector)
    {
        if (size < SDXC_MAX_TRANS_LEN)
        {
            ret = mmc_block_read(card, data, pos / 512, size / geometry.bytes_per_sector);
            if (ret)
            {
                goto err;
            }
            pos += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            data += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            sz += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            size -= geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
        }
        else
        {
            while (size > SDXC_MAX_TRANS_LEN)
            {
                ret = mmc_block_read(card, data, pos / 512, SDXC_MAX_TRANS_LEN / 512);
                if (ret)
                {
                    goto err;
                }
                size -= SDXC_MAX_TRANS_LEN;
                data += SDXC_MAX_TRANS_LEN;
                pos += SDXC_MAX_TRANS_LEN;
                sz += SDXC_MAX_TRANS_LEN;
            }
        }
    }

    /**
     * Step 3:
     * read the last data that not align to block size
     */
    if (size)
    {
        pr_debug("last size %u not align %u, read them\n", size, geometry.bytes_per_sector);

        pr_debug("step1: read page data from addr 0x%lx\n", pos);
        ret = mmc_block_read(card, sector_buf, pos / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page with len %u\n", size);
        memcpy(data, sector_buf, size);
        sz += size;
    }

    ret = 0;
    goto out;

err:
    pr_err("read failed - %d\n", (int)ret);
out:
    if (sector_buf)
    {
        free(sector_buf);
    }
    mmc_card_close((long)dev->user_data);

    return ret ? ret : sz;
}

static rt_err_t sunxi_sdmmc_open(rt_device_t dev, rt_uint16_t oflag)
{
    return 0;
}

static rt_err_t sunxi_sdmmc_close(rt_device_t dev)
{
    return 0;
}

static rt_size_t sunxi_sdmmc_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    int err = -1;
    ssize_t ret, sz = 0;
    struct rt_device_blk_geometry geometry;
    unsigned long long total_bytes;
    char *sector_buf = NULL;
    uint8_t *data = (uint8_t *)buffer;

    struct mmc_card *card = mmc_card_open((long)dev->user_data);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return -EIO;
    }

    if (size == 0)
    {
        return 0;
    }

    memset(&geometry, 0, sizeof(struct rt_device_blk_geometry));
    err = dev->control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    if (err)
    {
        return -EIO;
    }
    total_bytes = ((uint64_t)geometry.bytes_per_sector) * geometry.sector_count;

    if (pos >= total_bytes)
    {
        printf("read offset %lu over part size %llu\n", pos, total_bytes);
        return 0;
    }

    if (pos + size > total_bytes)
    {
        printf("over limit: offset %lu + size %lu over %llu\n",
               pos, size, total_bytes);
    }

    size = MIN(total_bytes - pos, size);
    pr_debug("off 0x%lx size %lu\n", pos, size);

    if (pos % geometry.bytes_per_sector || size % geometry.bytes_per_sector)
    {
        sector_buf = malloc(geometry.bytes_per_sector);
        if (!sector_buf)
        {
            return -ENOMEM;
        }
        memset(sector_buf, 0, geometry.bytes_per_sector);
    }

    /**
     * Step 1:
     * read the beginning data that not align to block size
     */
    if (pos % geometry.bytes_per_sector)
    {
        uint32_t addr, poff, len;

        addr = ALIGN_DOWN(pos, geometry.bytes_per_sector);
        poff = pos - addr;
        len = MIN(geometry.bytes_per_sector - poff, size);

        pr_debug("offset %lu not align %u, fix them before align read\n",
                 pos, geometry.bytes_per_sector);
        pr_debug("step1: read page data from addr 0x%x\n", addr);
        ret = mmc_block_write(card, sector_buf, addr / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page offset 0x%x and len %u\n",
                 poff, len);
        memcpy(data, sector_buf + poff, len);

        pos += len;
        buffer += len;
        sz += len;
        size -= len;
    }

    /**
     * Step 2:
     * read data that align to block size
     */
    while (size >= geometry.bytes_per_sector)
    {
        if (size < SDXC_MAX_TRANS_LEN)
        {
            ret = mmc_block_write(card, data, pos / 512, size / geometry.bytes_per_sector);
            if (ret)
            {
                goto err;
            }
            pos += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            data += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            sz += geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
            size -= geometry.bytes_per_sector * (size / geometry.bytes_per_sector);
        }
        else
        {
            while (size > SDXC_MAX_TRANS_LEN)
            {
                ret = mmc_block_write(card, data, pos / 512, SDXC_MAX_TRANS_LEN / 512);
                if (ret)
                {
                    goto err;
                }
                size -= SDXC_MAX_TRANS_LEN;
                data += SDXC_MAX_TRANS_LEN;
                pos += SDXC_MAX_TRANS_LEN;
                sz += SDXC_MAX_TRANS_LEN;
            }
        }
    }

    /**
     * Step 3:
     * read the last data that not align to block size
     */
    if (size)
    {
        pr_debug("last size %u not align %u, read them\n", size, geometry.bytes_per_sector);

        pr_debug("step1: read page data from addr 0x%llx\n", pos);
        ret = mmc_block_write(card, sector_buf, pos / 512, 1);
        if (ret != 0)
        {
            goto err;
        }

        pr_debug("step2: copy page data to buf with page with len %u\n", size);
        memcpy(data, sector_buf, size);
        sz += size;
    }

    ret = 0;
    goto out;

err:
    pr_err("read failed - %d\n", (int)ret);
out:
    if (sector_buf)
    {
        free(sector_buf);
    }
    mmc_card_close((long)dev->user_data);

    return ret ? ret : sz;
}

static rt_err_t sunxi_sdmmc_control(rt_device_t dev, int cmd, void *args)
{
    int ret = 0;
    blk_dev_erase_t *erase_sector;
    struct rt_device_blk_geometry *geometry;
    int flag = 0;

    if (!dev)
    {
        return -EINVAL;
    }

    struct mmc_card *card = mmc_card_open((long)dev->user_data);
    if (!card)
    {
        return ret;
    }

    switch (cmd)
    {
        case BLOCK_DEVICE_CMD_ERASE_ALL:
            break;
        case BLOCK_DEVICE_CMD_ERASE_SECTOR:
            break;
        case BLOCK_DEVICE_CMD_GET_TOTAL_SIZE:
            *(uint64_t *)args = card->csd.capacity * 1024;
            ret = 0;
            break;
        case BLOCK_DEVICE_CMD_GET_PAGE_SIZE:
            *(uint32_t *)args = 512;
            ret = 0;
            break;
        case BLOCK_DEVICE_CMD_GET_BLOCK_SIZE:
            *(uint32_t *)args = 512;
            ret = 0;
            break;
        case RT_DEVICE_CTRL_BLK_GETGEOME:
            geometry = (struct rt_device_blk_geometry *)args;
            memset(geometry, 0, sizeof(struct rt_device_blk_geometry));
            geometry->block_size = 512;
            geometry->bytes_per_sector = 512;
            geometry->sector_count = (card->csd.capacity * 1024) / geometry->bytes_per_sector;
            ret = 0;
            break;
        default:
            break;
    }

    mmc_card_close((long)dev->user_data);
    return ret;
}

#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
typedef struct __DEV_SDMMC
{
    uint32_t        last_lun;               //如果为1，表示是最后一个分区,注意:必须是第一个字段
    __u32           card_no;
    __u32           boot;
    __u32           offset;
    __u32           used;
    char            name[32];
    char            major_name[16];
    char            minor_name[16];
    __hdle          hReg;
    __dev_blkinfo_t info;
    void            *align_buff;
} __dev_sdmmc_t;

static int32_t sdmmc_dev_ioctrl(__hdle hDev, uint32_t Cmd, long Aux, void *pBuffer)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)hDev;

    switch (Cmd)
    {
        case DEV_CMD_GET_INFO:
        {
            if (!pBuffer)
            {
                return EPDK_FAIL;
            }

            *((__dev_blkinfo_t *)pBuffer) = pDev->info;
            return EPDK_OK;
        }
        case DEV_IOC_USR_FLUSH_CACHE:
        {
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
            flush_block_cache(1);
#endif
            return EPDK_OK;
        }
        default:
            break;
    }
    return EPDK_FAIL;
}

static __hdle sdmmc_dev_open(void *open_arg, __u32 Mode)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)open_arg;

    return pDev;
}

static __s32 sdmmc_dev_close(__hdle hDev)
{
    return 0;
}

__u32 sdmmc_dev_phy_read(void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)hDev;
    __u32 nblk = n;
    __u32 offset = 0;
    int readn = 0;
    int error;

    struct mmc_card *card = mmc_card_open(pDev->card_no);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return readn;
    }
    if (n < SDXC_MAX_TRANS_LEN / 512)
    {
        error = mmc_block_read(card, pBuffer, blk, n);
        if (error < 0)
        {
            mmc_card_close(pDev->card_no);
            return readn;
        }
        readn += n;
    }
    else
    {
        while (nblk > SDXC_MAX_TRANS_LEN / 512)
        {
            error = mmc_block_read(card, pBuffer + offset, blk, SDXC_MAX_TRANS_LEN / 512);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return readn;
            }
            nblk -= SDXC_MAX_TRANS_LEN / 512;
            blk += SDXC_MAX_TRANS_LEN / 512;
            offset += SDXC_MAX_TRANS_LEN;
            readn += SDXC_MAX_TRANS_LEN / 512;
        }
        if (nblk > 0)
        {
            error = mmc_block_read(card, pBuffer + offset, blk, nblk);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return readn;
            }
            readn += nblk;
        }
    }
    mmc_card_close(pDev->card_no);
    return readn;
}

__u32 sdmmc_dev_phy_write(const void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
    __dev_sdmmc_t *pDev = (__dev_sdmmc_t *)hDev;
    __u32 nblk = n;
    __u32 offset = 0;
    int error = 0;
    int writen = 0;

    struct mmc_card *card = mmc_card_open(pDev->card_no);
    if (card == NULL)
    {
        printf("mmc open fail\n");
        return 0;
    }
    if (n < SDXC_MAX_TRANS_LEN / 512)
    {
        error = mmc_block_write(card, pBuffer, blk, n);
        if (error < 0)
        {
            mmc_card_close(pDev->card_no);
            return writen;
        }
        writen += n;
    }
    else
    {
        while (nblk > SDXC_MAX_TRANS_LEN / 512)
        {
            error = mmc_block_write(card, pBuffer + offset, blk, SDXC_MAX_TRANS_LEN / 512);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return writen;
            }
            nblk -= SDXC_MAX_TRANS_LEN / 512;
            blk += SDXC_MAX_TRANS_LEN / 512;
            offset += SDXC_MAX_TRANS_LEN;
            writen += SDXC_MAX_TRANS_LEN / 512;
        }
        if (nblk > 0)
        {
            error = mmc_block_write(card, pBuffer + offset, blk, nblk);
            if (error < 0)
            {
                mmc_card_close(pDev->card_no);
                return writen;
            }
            writen += nblk;
        }
    }
    mmc_card_close(pDev->card_no);
    return writen;
}

__u32 sdmmc_dev_read(void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
    blk += ((__dev_sdmmc_t *)hDev)->offset;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    if (sdmmc_cache_read(pBuffer, (int)blk, (int)n, 512, hDev) == 0)
    {
        return n;
    }
    return 0;
#else
    return sdmmc_dev_phy_read(pBuffer, blk, n, hDev);
#endif
}

__u32 sdmmc_dev_write(const void *pBuffer, __u32 blk, __u32 n, __hdle hDev)
{
    blk += ((__dev_sdmmc_t *)hDev)->offset;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    if (sdmmc_cache_write((void *)pBuffer, (int)blk, (int)n, 512, hDev) == 0)
    {
        return n;
    }
    return 0;
#else
    return sdmmc_dev_phy_write(pBuffer, blk, n, hDev);
#endif
}

__dev_devop_t sdmmc_dev_op =
{
    sdmmc_dev_open,
    sdmmc_dev_close,
    sdmmc_dev_read,
    sdmmc_dev_write,
    sdmmc_dev_ioctrl
};
#define SDC_COUNT                       2

__dev_sdmmc_t *pDev[SDC_COUNT] = {NULL};

void melis_part_init(int card_id)
{
    int aux = 0;
    struct mmc_card *card;

    if (pDev[card_id] != NULL)
    {
        if (pDev[card_id]->hReg != NULL)
        {
            esDEV_DevUnreg(pDev[card_id]->hReg);
        }
        free(pDev[card_id]);
        pDev[card_id] = NULL;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
        discard_block_cache();
#endif
    }
	aux = card_id;

    card = mmc_card_open(card_id);
    if (card == NULL)
    {
        __err("mmc open fail");
        return;
    }

    pDev[card_id] = (__dev_sdmmc_t *)malloc(sizeof(__dev_sdmmc_t));
    if (NULL == pDev[card_id])
    {
        __err("allocate memory failed");
        mmc_card_close(card_id);
        return;
    }

#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    block_cache_manager_init(CONFIG_SDMMC_CACHE_SIZE);
#endif

    memset(pDev[card_id], 0, sizeof(__dev_sdmmc_t));
    snprintf(pDev[card_id]->name, 32, "%s:%d", "SDMMC-DISK", aux);
    __log("sdcard device name: %s", pDev[card_id]->name);

    pDev[card_id]->card_no          = aux;
    pDev[card_id]->boot             = 0;
    pDev[card_id]->offset           = 0;
    pDev[card_id]->used             = 0;
    pDev[card_id]->info.hiddennum   = 0;
    pDev[card_id]->info.headnum     = 2;
    pDev[card_id]->info.secnum      = 4;
    pDev[card_id]->info.partsize    = card->csd.capacity * 2;
    pDev[card_id]->info.secsize     = 512;

    pDev[card_id]->hReg = esDEV_DevReg("DISK", pDev[card_id]->name, &sdmmc_dev_op, (void *)pDev[card_id]);
    if (NULL == pDev[card_id]->hReg)
    {
        __err("sdcard register partition failed");
    }

    mmc_card_close(card_id);
}

void melis_part_deinit(int card_id)
{
    if (!pDev[card_id])
    {
        return;
    }
    esDEV_DevUnreg(pDev[card_id]->hReg);
    free(pDev[card_id]);
    pDev[card_id] = NULL;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    discard_block_cache();
#endif
}
#include "part_efi.h"
#define MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)
#define SUNXI_MBR_MAGIC			    "softw411"
#define GPT_ENTRY_SIZE			128
typedef struct tag_CRC32_DATA
{
    __u32 CRC;
    __u32 CRC_32_Tbl[256];
} CRC32_DATA_t;

__dev_sdmmc_t   *pDevSys[SDC_COUNT][MAX_PART_COUNT] = {NULL};
__u32 calc_crc32(const unsigned char *buffer, __u32 length)
{
    __u32 i, j;
    CRC32_DATA_t crc32;
    __u32 CRC32 = 0xffffffff;
    crc32.CRC = 0;

    for ( i = 0; i < 256; ++i)
    {
        crc32.CRC = i;
        for ( j = 0; j < 8 ; ++j)
        {
            if (crc32.CRC & 1)
                crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
            else
                crc32.CRC >>= 1;
        }
        crc32.CRC_32_Tbl[i] = crc32.CRC;
    }

    CRC32 = 0xffffffff;
    for ( i = 0; i < length; ++i)
    {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32 >> 8);
    }

    return CRC32^0xffffffff;
}

void sunxi_get_logical_offset_param(int storage_type, u32 *logic_offset,
				    int *total_sectors)
{
	/*
	 * for nand, no phyread available, so this is not relevant
	 * for mmc, part offset is physical offset, offset should be taken care in start value, refer by logic_offset here
	 * for nor, part offset is logical offset, offset should be taken care in end value, refer by total_sectors here
	 */
	if (storage_type == STORAGE_SDC0 || storage_type == STORAGE_SDC1 ||
	    storage_type == STORAGE_SDC2) {
		*logic_offset = MMC_LOGICAL_OFFSET;
	} else {
		*logic_offset = 0;
	}
#if 0
	*total_sectors = sunxi_sprite_size();
#ifdef CONFIG_SUNXI_SPINOR
	if (storage_type == STORAGE_NOR) {
		*total_sectors -= SPINOR_LOGICAL_OFFSET;
	}
#endif
#endif
}

int gpt_convert_to_sunxi_mbr(void *sunxi_mbr_buf, char *gpt_buf, int storage_type)
{
	u32 data_len	  = 0;
	char *pbuf	    = gpt_buf;
	gpt_entry *pgpt_entry = NULL;
	char *gpt_entry_start = NULL;
	int PartCount	 = 0;
	int pos		      = 0;
	int i, j;
	int crc32_total;
	int mbr_size = 0;
	u64 start_sector;
	int total_sectors;
	u32 logic_offset = 0;

	/* mbr_size = 256; [> hardcode, TODO: fixit <] */
	/* mbr_size = mbr_size * (1024/512); */

	sunxi_mbr_t *sunxi_mbr = (sunxi_mbr_t *)sunxi_mbr_buf;
	memset(sunxi_mbr, 0, sizeof(sunxi_mbr_t));

	sunxi_mbr->version = 0x00000200;
	memcpy(sunxi_mbr->magic, SUNXI_MBR_MAGIC, 8);
	sunxi_mbr->copy = 1;

	data_len = 0;
	data_len += 512; /* 0 to gpt->head */
	data_len += 512; /* gpt->head to gpt_entry */
	gpt_entry_start = (pbuf + data_len);
	while (1) {
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (pos)*GPT_ENTRY_SIZE);
		if ((long)(pgpt_entry->starting_lba) > 0) {
			//printf("gpt pos:%d s:0x%llx e:0x%llx\n", pos, pgpt_entry->starting_lba, pgpt_entry->ending_lba);
			PartCount++;
		} else
			break;

		pos++;
	}
	sunxi_mbr->PartCount = PartCount;
	//printf("PartCount = %d\n", PartCount);
	//sunxi_mbr->PartCount += 1; //need UDISK
	//printf("fix, now PartCount = %d\n", sunxi_mbr->PartCount);

	for (i = sunxi_mbr->PartCount - 1; i >= 0; i--) {
		/* udisk is the first part */
		/* pos = (i == sunxi_mbr->PartCount-1) ? 0: i+1; */
		pos	= i;
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (pos)*GPT_ENTRY_SIZE);

		sunxi_mbr->array[i].lenhi = ((pgpt_entry->ending_lba - pgpt_entry->starting_lba + 1) >> 32) & 0xffffffff;
		sunxi_mbr->array[i].lenlo = ((pgpt_entry->ending_lba - pgpt_entry->starting_lba + 1) >> 0) & 0xffffffff;
		if (i == sunxi_mbr->PartCount - 1) {
			sunxi_mbr->array[i].lenhi = 0;
			sunxi_mbr->array[i].lenlo = 0;
		};
		//printf("%d starting_lba:%llx ending_lba:%llx\n", i, pgpt_entry->starting_lba, pgpt_entry->ending_lba);
		if (i == 0) {
			/* mbr_size = pgpt_entry->ending_lba - 511; */
			sunxi_get_logical_offset_param(storage_type, &logic_offset, &total_sectors);
			mbr_size = pgpt_entry->starting_lba - logic_offset;
			//printf("mbr sector size:%x\n", mbr_size);
		}

		if (pgpt_entry->attributes.fields.type_guid_specific == 0x6000)
			sunxi_mbr->array[i].ro = 1;
		else if (pgpt_entry->attributes.fields.type_guid_specific == 0x8000)
			sunxi_mbr->array[i].ro = 0;

		strcpy((char *)sunxi_mbr->array[i].classname, "DISK");
		memset(sunxi_mbr->array[i].name, 0, 16);

		for (j = 0; j < 16; j++) {
			if (pgpt_entry->partition_name[j])
				sunxi_mbr->array[i].name[j] = pgpt_entry->partition_name[j];
			else
				break;
		}
	}

	for (i = 0; i < sunxi_mbr->PartCount; i++) {
		if (i == 0) {
			sunxi_mbr->array[i].addrhi = 0;
			sunxi_mbr->array[i].addrlo = mbr_size;
		} else {
			start_sector = sunxi_mbr->array[i - 1].addrlo;
			start_sector |= (u64)sunxi_mbr->array[i - 1].addrhi << 32;
			start_sector += sunxi_mbr->array[i - 1].lenlo;

			sunxi_mbr->array[i].addrlo = (u32)(start_sector & 0xffffffff);
			sunxi_mbr->array[i].addrhi = (u32)((start_sector >> 32) & 0xffffffff);
		}
		/*printf("i=%d, addrhi=%d addrlo=0x%x, lenhi=0x%x, lenlo=0x%x, name=%s\n", i, sunxi_mbr->array[i].addrhi,
		       sunxi_mbr->array[i].addrlo, sunxi_mbr->array[i].lenhi, sunxi_mbr->array[i].lenlo, sunxi_mbr->array[i].name);*/
	}

	crc32_total      = calc_crc32((const unsigned char *)(sunxi_mbr_buf + 4), SUNXI_MBR_SIZE - 4);
	sunxi_mbr->crc32 = crc32_total;

	return 0;
}

void melis_part_init_bootcard(int card_id)
{
    uint32_t            capacity = 0;
    struct mmc_card     *card;
    sunxi_mbr_t         *pMBR = NULL;
    sunxi_mbr_t         *temp_buf = NULL;
    int32_t             ret = 0;
    uint32_t            offset = 0;
    int32_t             i = 0;

    /*step 01: open mmc-card to get the card's capacity*/
    card    = mmc_card_open(card_id);

    if (card == NULL)
    {
        __err("mmc open fail");
        ret = -ENODEV;
        goto err;
    }

    capacity    = card->csd.capacity * 2;

    mmc_card_close(card_id);

    /*step 02: ready for reading card-type-mbr, size is 16KBytes */
    /*ready pDevSys handler */
    pDevSys[card_id][0]   = (__dev_sdmmc_t *)rt_malloc(sizeof(__dev_sdmmc_t));

    if (NULL == pDevSys[card_id][0])
    {
        __err("allocate memory for pDev manager failed");
        ret = -ENOMEM;
        goto err;
    }

    rt_memset((void*)pDevSys[card_id][0], 0,  sizeof(__dev_sdmmc_t));

    pDevSys[card_id][0]->card_no    = card_id;

    /*ready pMBR memory */
    pMBR    = (sunxi_mbr_t *)rt_malloc(16384);
    if (!pMBR)
    {
        __err("allocate memory for mbr failed");
        ret = -ENOMEM;
        goto err;
    }
    rt_memset((void*)pMBR, 0, 16384);
    temp_buf	= (sunxi_mbr_t *)rt_malloc(16384);
    if (!temp_buf)
    {
        __err("allocate memory for mbr test failed");
	    ret = -ENOMEM;
	    goto err;
    }
    rt_memset((void*)temp_buf, 0, 16384);

    /*step 03: read mbr */
    ret = sdmmc_dev_read((void*)temp_buf, 40960/*unit is sector(512bytes)*/, 32/*16kbytes*/, pDevSys[card_id][0]);

    if(rt_strncmp(temp_buf->magic,SUNXI_MBR_MAGIC,8))
    {
        gpt_convert_to_sunxi_mbr((void *)pMBR,(char *)temp_buf,STORAGE_SDC0);
    }
    else
    {
        __log("PhoenixCard burn is mbr");
        rt_memcpy(pMBR,temp_buf,16384);
    }

    /*step 04: realloc part structure depending on the mbr part count*/
    pDevSys[card_id][0] = (__dev_sdmmc_t *)rt_realloc(pDevSys[card_id][0], (pMBR->PartCount) * sizeof(__dev_sdmmc_t));
    if (NULL == pDevSys[card_id][0])
    {
        __err("1re-allocate memory for pDev manager failed %d",(pMBR->PartCount) * sizeof(__dev_sdmmc_t));
        ret = -ENOMEM;
        goto err;
    }
    rt_memset((void*)pDevSys[card_id][0], 0, (pMBR->PartCount) * sizeof(__dev_sdmmc_t));	
    /*step 05: start to process mbr information and esDEV_DEVReg partition to the DMS*/
    for (i = 0; i < pMBR->PartCount; i++)
    {
        pDevSys[card_id][i] = (__dev_sdmmc_t *)((unsigned long)(pDevSys[card_id][0]) + i * sizeof(__dev_sdmmc_t));

        pDevSys[card_id][i]->card_no        = card_id;
        pDevSys[card_id][i]->boot           = 0;
        pDevSys[card_id][i]->offset         = 40960 + pMBR->array[i].addrlo; /*unit is sector(512bytes), add boot0card's offset*/
        pDevSys[card_id][i]->used           = 0;
        pDevSys[card_id][i]->info.hiddennum = 0;
        pDevSys[card_id][i]->info.headnum   = 2;
        pDevSys[card_id][i]->info.secnum    = 4;
        pDevSys[card_id][i]->info.partsize  = pMBR->array[i].lenlo; /*unit is sector(512bytes)*/
        pDevSys[card_id][i]->info.secsize   = 512;

        rt_memcpy(pDevSys[card_id][i]->name, pMBR->array[i].name, sizeof(pMBR->array[i].name));

        if (i == pMBR->PartCount - 1)/*caculate the last partition udisk's size(unit is sector)*/
        {
            pDevSys[card_id][i]->info.partsize = capacity - pDevSys[card_id][i]->offset;
			if (pDevSys[card_id][i]->info.partsize >= (20*1024*1024)/512)
			{
				pDevSys[card_id][i]->info.partsize = (20*1024*1024)/512;
			}
        }
        /*step 06: only register the ROOTFS, because bootcard's UDISK partition is register in melis_part_init function*/
        if((i == pMBR->PartCount - 2) || (i == pMBR->PartCount - 1))
        {
            pDevSys[card_id][i]->hReg   = esDEV_DevReg("DISK", pDevSys[card_id][i]->name, &sdmmc_dev_op, (void *)pDevSys[card_id][i]);

            if (NULL == pDevSys[card_id][i]->hReg)
            {
                __err("sdcard register [%s] partition failed", pDevSys[card_id][i]->name);
            }
        }
    }

    goto out;
    /* not release the pDev memory*/

err:
    if(pDevSys[card_id][0])
    {
        rt_free(pDevSys[card_id][0]);
        pDevSys[card_id][0] = NULL;
    }

out:
    if(pMBR)
    {
        rt_free(pMBR);
        pMBR    = NULL;
    }
	if(temp_buf)
    {
        rt_free(temp_buf);
        temp_buf    = NULL;
    }
    return ;
}

void melis_part_deinit_bootcard(int card_id)
{
    int i = 0;

    if (!pDevSys[card_id][0])
    {
        return;
    }
    for (i = 0; i < MAX_PART_COUNT; i++)
    {
        if(pDevSys[card_id][0]->hReg)
        {
            esDEV_DevUnreg(pDevSys[card_id][0]->hReg);
        }
    }
    free(pDevSys[card_id][0]);
    pDevSys[card_id][0] = NULL;
#ifdef CONFIG_SUPPORT_SDMMC_CACHE
    discard_block_cache();
#endif
}

#endif
static int init_sdmmc_device(rt_device_t device, void *usr_data, char *dev_name)
{
	int ret = -1;
	//struct rt_device *device;

	device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }
    device->init = sunxi_sdmmc_init;
    device->open = sunxi_sdmmc_open;
    device->close = sunxi_sdmmc_close;
    device->read = sunxi_sdmmc_read;
    device->write = sunxi_sdmmc_write;
    device->control = sunxi_sdmmc_control;
    /* device->user_data = (void *)&sunxi_hal_sdmmc_driver; */
	device->user_data	= usr_data;

    ret = rt_device_register(device, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_device_destroy(device);
        return ret;
    }
    int sdmmc_blkpart_init(const char *name);
    ret = sdmmc_blkpart_init(dev_name);

    return ret;
}


int sunxi_driver_sdmmc_init(void)
{
    int ret = -1;
	int i = 0;
	rt_device_t device0, device1;
	void *usr_data;
	__s32 used_card_no	= 0x01;

	ret = esCFG_GetKeyValue("sdcard_global", "used_card_no", (int32_t *)&used_card_no, 1);
	if (ret)
	{
		used_card_no = 0x00;
		__err("get	card no failed, card no: %d", used_card_no);
		return ret;
	}

	for (i = 0; i < 2; ++i)
	{
		if (((used_card_no >> i) & 0x01) == 0)
		{
			continue;
		}
		if(i == 0)
		{
			usr_data = (void *)0;
			ret = init_sdmmc_device(device0,usr_data ,"sdmmc");
		}
		if(i == 1)
		{
			usr_data = (void *)1;
			ret = init_sdmmc_device(device1,usr_data ,"sdmmc1");
		}
	}

    return ret;
}

int sunxi_driver_sdmmc_deinit(void)
{
    void shutdown_block_cache(void);
    shutdown_block_cache();
    return 0;
}

device_initcall(sunxi_driver_sdmmc_init);
