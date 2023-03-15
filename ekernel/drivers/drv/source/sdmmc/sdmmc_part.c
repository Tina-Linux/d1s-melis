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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <rtthread.h>
#include <blkpart.h>
#include "mbr.h"
#include <sdmmc/hal_sdhost.h>
#include <sdmmc/card.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

#define ALIGN_DOWN(x, a) __ALIGN_KERNEL((x) - ((a) - 1), (a))
#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

struct mmc_card *mmc_card_open(uint8_t card_id);
int32_t mmc_card_close(uint8_t card_id);

static struct blkpart sdmmcblk;

struct syspart
{
    char name[MAX_BLKNAME_LEN];
    u32 offset;
    u32 bytes;
};
static const struct syspart syspart[] =
{
};

static struct part all_part[16];

static void show_mbr_part(void *mbr_buf)
{
    int i = 0;

    sunxi_mbr_t *pMBR = (sunxi_mbr_t *)mbr_buf;

    printf("MBR:\r\n");
    printf("classname        name      size     offset\r\n");
    for (i = 0; i < pMBR->PartCount; i++)
    {
        int offset      = pMBR->array[i].addrlo;
        int partsize    = pMBR->array[i].lenlo;
        int secsize     = 512;

        printf("%s        %s       %d        %d\r\n",
               (const char *)pMBR->array[i].classname,
               (const char *)pMBR->array[i].name,
               partsize,
               offset);
    }
}

int mbr_part_cnt(void *mbr_buf)
{
    sunxi_mbr_t *pMBR = (sunxi_mbr_t *)mbr_buf;
    return pMBR->PartCount;
}

static int sdmmc_get_mbr(rt_device_t dev, char *buf, int len)
{
    int ret = 0;
    if (len < MBR_SIZE)
    {
        pr_err("buf too small for mbr\n");
        return -EINVAL;
    }

    pr_debug("read mbr from 0x%x\n", MBR_START_ADDRESS);
    ret = dev->read(dev, MBR_START_ADDRESS, buf, MBR_SIZE);
    return ret < 0 ? -EIO : 0;
}

static int register_part(rt_device_t dev, struct part *part)
{
    int ret = -1;
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->read = part_read;
    device->write = part_erase_without_write;
    device->control = part_control;
    device->user_data = part;

    ret = rt_device_register(device, part->name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        free(device);
        return ret;
    }
    return 0;
}

static int unregister_part(rt_device_t dev)
{
    int ret = -1;
    ret = rt_device_unregister(dev);
    if (ret != 0)
    {
        return ret;
    }
    rt_device_destroy(dev);
    return 0;
}

static int register_blk_device(rt_device_t dev)
{
    int ret = -1, index = 0;
    char *mbr_buf;
    struct mbr_part *mbr_part;
    struct part *part;
    unsigned int offset = 0;
    int i = 0;

    mbr_buf = malloc(MBR_SIZE);
    if (!mbr_buf)
    {
        ret = -ENOMEM;
        goto err;
    }
    memset(mbr_buf, 0, MBR_SIZE);

    dev->control(dev, BLOCK_DEVICE_CMD_GET_PAGE_SIZE, &sdmmcblk.page_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &sdmmcblk.total_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_BLOCK_SIZE, &sdmmcblk.blk_bytes);
    sdmmcblk.dev = dev;

    pr_debug("total_bytes = %lld\n", sdmmcblk.total_bytes);
    pr_debug("blk_bytes = %d\n", sdmmcblk.blk_bytes);
    pr_debug("page_bytes = %d\n", sdmmcblk.page_bytes);

#if 0
    show_mbr_part(mbr_buf);
    ret = mbr_part_cnt(mbr_buf);
    if (ret < 0)
    {
        pr_err("get part count from mbr failed\n");
        goto err;
    }

    printf("part cnt = %d\n", ret);
    sdmmcblk.n_parts = ret + ARRAY_SIZE(syspart);
    sdmmcblk.parts = malloc(sizeof(struct part) * sdmmcblk.n_parts);
    if (sdmmcblk.parts == NULL)
    {
        pr_err("allocate part array failed.\n");
        goto err;
    }
    memset(sdmmcblk.parts, 0, sizeof(struct part) * sdmmcblk.n_parts);

    for (index = 0; index < ARRAY_SIZE(syspart); index++)
    {
        part = &sdmmcblk.parts[index];
        part->blk = &sdmmcblk;
        part->bytes = syspart[index].bytes;
        part->off = syspart[index].offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", syspart[index].name);
        offset += part->bytes;
    }

    MBR *pMBR = (MBR *)mbr_buf;

    for (i = 0; i < pMBR->PartCount; i++)
    {
        part = &sdmmcblk.parts[index++];
        part->blk = &sdmmcblk;
        part->bytes = pMBR->array[i].lenlo;
        part->off = pMBR->array[i].addrlo;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", pMBR->array[i].name);
        offset += part->bytes;
    }

    sdmmcblk.parts[--index].bytes = sdmmcblk.total_bytes - offset;

    for (index = 0; index < sdmmcblk.n_parts; index++)
    {
        part = &sdmmcblk.parts[index];
        if (part->bytes % sdmmcblk.blk_bytes)
        {
            pr_err("part %s with bytes %u should align to block size %u\n",
                   part->name, part->bytes, sdmmcblk.blk_bytes);
        }
        else
        {
            register_part(dev, part);
        }
    }
    blkpart_add_list(&sdmmcblk);
#else
    sdmmcblk.n_parts = 1;
    sdmmcblk.parts = malloc(sizeof(struct part) * sdmmcblk.n_parts);
    if (sdmmcblk.parts == NULL)
    {
        pr_err("allocate part array failed.\n");
        goto err;
    }
    memset(sdmmcblk.parts, 0, sizeof(struct part) * sdmmcblk.n_parts);

    struct mmc_card *card = mmc_card_open((long)dev->user_data);

    part = &sdmmcblk.parts[index++];
    part->blk = &sdmmcblk;
    part->bytes = card->csd.capacity * 1024;
    part->off = 0;
    snprintf(part->name, MAX_BLKNAME_LEN, "%s", "SDMMC-DISK");

    register_part(dev, part);
    blkpart_add_list(&sdmmcblk);

    mmc_card_close((long)dev->user_data);
    ret = 0;
#endif
err:
    free(mbr_buf);
    return ret;
}

static int unregister_blk_device(rt_device_t dev)
{
    int ret = -1;

    rt_device_t part_dev = rt_device_find("SDMMC-DISK");
    if (!part_dev)
    {
        return ret;
    }

    ret = unregister_part(part_dev);
    if (ret != 0)
    {
        return ret;
    }
    blkpart_del_list(&sdmmcblk);
    free(sdmmcblk.parts);
    return 0;
}

int sdmmc_blkpart_init(const char *sdmmc_device_name)
{
    int ret = -1;
    rt_device_t dev = rt_device_find(sdmmc_device_name);
    if (!dev)
    {
        pr_err("the sdmmc device %s is not exist!\n", sdmmc_device_name);
        return ret;
    }

    if (dev->init)
    {
        ret = dev->init(dev);
    }

    return ret;
}

void melis_part_init(int card_id);
void melis_part_deinit(int card_id);
void melis_part_init_bootcard(int card_id);
void melis_part_deinit_bootcard(int card_id);

int mount(
    const char *device_name,
    const char *path,
    const char *filesystemtype,
    unsigned long rwflag,
    const void *data);

int umount(const char *specialfile);
static int register_blk_bootcard_device(rt_device_t dev);
void mount_sdmmc_filesystem(int card_id)
{
#ifdef CONFIG_RT_USING_DFS_ELMFAT
    int ret = -1;
    rt_device_t dev = rt_device_find("sdmmc");
    if (!dev)
    {
        pr_err("the sdmmc device [sdmmc] is not exist!\n");
        return;
    }

    ret = register_blk_device(dev);
    if (ret == 0)
    {
        ret = mount("SDMMC-DISK", "/sdmmc", "elm", 0, 0);
        if (ret != 0)
        {
            unregister_blk_device(dev);
            return;
        }
    }
#elif defined CONFIG_MELIS_LEGACY_DRIVER_MAN
    int ret = -1, value = 0;
	int need_check = 0;
	int card_product_used = -1;
	
    ret = esCFG_GetKeyValue("target", "storage_type", (int32_t *)&value, 1);
	if (value == STORAGE_SDC0 && card_id == 0) {
		need_check = 1;
	} else if (value == STORAGE_SDC1 && card_id == 1){
		need_check = 1;
	} else {
		need_check = 0;
	}
	if (need_check)
	{
#if defined CONFIG_ROOTFS_LITTLEFS
        rt_device_t dev = rt_device_find("sdmmc");
        register_blk_bootcard_device(dev);
#elif defined CONFIG_ROOTFS_MINFS
        melis_part_init_bootcard(card_id);
#endif
    }
	ret = esCFG_GetKeyValue("card_product", "card_product_used", (int32_t *)&card_product_used, 1);
	if (need_check == 0 || (card_product_used && ret == 0))
	{
    	melis_part_init(card_id);
	}
#else
#warning "Not filesystem support!"
#endif
    return;
}

void unmount_sdmmc_filesystem(int card_id)
{
#ifdef CONFIG_RT_USING_DFS_ELMFAT
    int ret = -1;

    rt_device_t dev = rt_device_find("sdmmc");
    if (!dev)
    {
        pr_err("the sdmmc device [sdmmc] is not exist!\n");
        return;
    }

    ret = umount("/sdmmc");
    if (ret == 0)
    {
        unregister_blk_device(dev);
    }
#elif defined CONFIG_MELIS_LEGACY_DRIVER_MAN
    int ret = -1, value = 0;
	int need_check = 0;

    /* usbc enable */
    ret = esCFG_GetKeyValue("target", "storage_type", (int32_t *)&value, 1);
	if (value == STORAGE_SDC0 && card_id == 0) {
		printk("boot from sdio\n");
		need_check = 1;
	} else if (value == STORAGE_SDC1 && card_id == 1){
		need_check = 1;
	} else {
		need_check = 0;
	}
    melis_part_deinit(card_id);
	if (need_check)
    {
    	melis_part_deinit_bootcard(card_id);
	}
#else
#warning "Not filesystem support!"
#endif
    return;
}

#if defined CONFIG_LOAD_DATA_TO_MEM_FROM_STORAGE

char *pmem_for_data = NULL;
struct part *pdata_part = NULL;

static rt_size_t sunxi_sdmmc_read_from_memory(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    /*only load CUDISK part to the memory for debug, so kick out the part->off*/
    //printf(" pos=%d, off=%d\r\n", pos, pdata_part->off);
    pos -= pdata_part->off;
    rt_memcpy(buffer, pmem_for_data + pos, size);
    return size;
}

static rt_size_t sunxi_sdmmc_write_to_memory(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    /*only load CUDISK part to the memory for debug on fpgboard, so kick out the part->off*/
    pos -= pdata_part->off;
    rt_memcpy(pmem_for_data + pos, buffer, size);
    return size;
}


#endif

static int register_blk_bootcard_device(rt_device_t dev)
{
    int ret = -1, index = 0;
    char *mbr_buf;
    struct mbr_part *mbr_part;
    struct part *part;
    unsigned int offset = 0;
    int i = 0;
	/*define the bootcard's two part in front, do not use boot0card.bin which start from 16th sector*/
	static const struct syspart syspartcard[] =
	{
        {"none",    0,              20*1024*1024},
        {"mbr",     20*1024*1024,   SUNXI_MBR_SIZE/*16*1024*1024*/}/*16K *4 ,but only read one copie*/
	};

	/*read the mbr*/
    mbr_buf = malloc(syspartcard[1].bytes);
    if (!mbr_buf)
    {
        ret = -ENOMEM;
        goto err;
    }
    memset(mbr_buf, 0, syspartcard[1].bytes);

    dev->control(dev, BLOCK_DEVICE_CMD_GET_PAGE_SIZE, &sdmmcblk.page_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &sdmmcblk.total_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_BLOCK_SIZE, &sdmmcblk.blk_bytes);
    sdmmcblk.dev = dev;/*sdmmc device*/

	/*read mbr*/
    ret = dev->read(dev, syspartcard[1].offset, mbr_buf, syspartcard[1].bytes);

    printf("total_bytes = %ld\r\n", sdmmcblk.total_bytes);
    printf("blk_bytes = %d\r\n", sdmmcblk.blk_bytes);
    printf("page_bytes = %d\r\n", sdmmcblk.page_bytes);

    show_mbr_part(mbr_buf);
    ret = mbr_part_cnt(mbr_buf);
    if (ret < 0)
    {
        pr_err("get part count from mbr failed\n");
        goto err;
    }

    printf("part cnt = %d\r\n", ret);
    sdmmcblk.n_parts = ret + ARRAY_SIZE(syspartcard);
    sdmmcblk.parts = malloc(sizeof(struct part) * sdmmcblk.n_parts);
    if (sdmmcblk.parts == NULL)
    {
        pr_err("allocate part array failed.\r\n");
        goto err;
    }
    memset(sdmmcblk.parts, 0, sizeof(struct part) * sdmmcblk.n_parts);

    for (index = 0; index < ARRAY_SIZE(syspartcard); index++)
    {
        part = &sdmmcblk.parts[index];
        part->blk = &sdmmcblk;
        part->bytes = syspartcard[index].bytes;
        part->off = syspartcard[index].offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", syspartcard[index].name);
        offset += part->bytes;
    }

    sunxi_mbr_t *pMBR = (sunxi_mbr_t *)mbr_buf;

    for (i = 0; i < pMBR->PartCount; i++)
    {
        part = &sdmmcblk.parts[index++];
        part->blk = &sdmmcblk;
        part->bytes = pMBR->array[i].lenlo * sdmmcblk.page_bytes/*512*/;
        part->off = pMBR->array[i].addrlo * sdmmcblk.page_bytes/*512*/ + syspartcard[1].offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", pMBR->array[i].name);
        offset += part->bytes;
    }

    sdmmcblk.parts[--index].bytes = sdmmcblk.total_bytes - offset;



    for (index = 0; index < sdmmcblk.n_parts; index++)
    {
        part = &sdmmcblk.parts[index];
        if (part->bytes % sdmmcblk.blk_bytes)
        {
            pr_err("part %s with bytes %u should align to block size %u\n",
                   part->name, part->bytes, sdmmcblk.blk_bytes);
        }
        else
        {
            #if defined CONFIG_LOAD_DATA_TO_MEM_FROM_STORAGE
                /*only load CUDISK part to the memory for debug*/
                if(0 == rt_strcmp(part->name, "ROOTFS"))
                {
                    pmem_for_data = rt_malloc_align(part->bytes, 1024);
                    if(pmem_for_data)
                    {
                        rt_size_t ret = 0;
                        ret = dev->read(dev, part->off,  pmem_for_data,   part->bytes);
                        if(ret != part->bytes)
                        {
                            printf("the bytes number is not right that CUDISK read from sd\r\n");
                        }
                        dev->read   = sunxi_sdmmc_read_from_memory;
                        dev->write  = sunxi_sdmmc_write_to_memory;
                        printf("pmem_for_data = 0x%p CUDISK off=%d bytes=%d\r\n", pmem_for_data, part->off, part->bytes);
                        pdata_part = part;
                    }
                    else
                    {
                        printf("pmem_for_data failed!\r\n");
                    }
                }
            #endif
            register_part(dev, part);
        }
    }
	mount("CUDISK", "/data", "lfs",0, NULL);
    blkpart_add_list(&sdmmcblk);

err:
    free(mbr_buf);
    return ret;
}
