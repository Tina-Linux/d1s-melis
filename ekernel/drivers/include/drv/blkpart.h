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
#ifndef __BLKPART_H__
#define __BLKPART_H__

#include <sys/types.h>
#include <stdint.h>
#include <rtthread.h>

#include <log.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SECTOR_SIZE
#undef SECTOR_SIZE
#endif
#define SECTOR_SHIFT 9
#define SECTOR_SIZE (1 << SECTOR_SHIFT)

#if 0
struct part
{
    /* public */
#define BLKPART_OFF_APPEND UINT32_MAX
    uint32_t off;
#define BLKPART_SIZ_FULL UINT32_MAX
    uint32_t bytes;
#define MAX_BLKNAME_LEN 16
    char name[MAX_BLKNAME_LEN];      /* name: UDISK */

    /* private */
    char devname[MAX_BLKNAME_LEN];   /* name: nor0p1 */
    rt_device_t phdev;
    struct part *next;
};
#endif

typedef enum
{
	STORAGE_NAND =0,
	STORAGE_SDC0,
	STORAGE_SDC1,
	STORAGE_NOR,
	STORAGE_SDC2,	
	STORAGE_SPI_NAND,
}SUNXI_BOOT_STORAGE;


struct part
{
    /* public */
#define BLKPART_OFF_APPEND UINT32_MAX
    uint64_t off;
#define BLKPART_SIZ_FULL UINT32_MAX
    uint64_t bytes;
#define MAX_BLKNAME_LEN 16
    char name[MAX_BLKNAME_LEN];      /* name: UDISK */

    /* private */
    char devname[MAX_BLKNAME_LEN];   /* name: nor0p1 */
    struct blkpart *blk;
    uint32_t n_part;
    uint32_t erase_flag;
};

struct blkpart
{
    /* public */
    const char *name;
    uint64_t total_bytes;
    uint32_t blk_bytes;
    uint32_t page_bytes;
    int (*erase)(unsigned int, unsigned int);
    int (*program)(unsigned int, char *, unsigned int);
    int (*read)(unsigned int, char *, unsigned int);

    /* if no any partition, the follow can be NULL */
    struct part *parts;
    uint32_t n_parts;

    /* private */
    int blk_cnt;
    struct part root;
    struct blkpart *next;
    uint32_t n_blk;
    rt_device_t dev;
};

typedef enum BLOCK_DEVICE_CMD_T
{
    BLOCK_DEVICE_CMD_ERASE_ALL = 0x00,
    BLOCK_DEVICE_CMD_ERASE_SECTOR,
    BLOCK_DEVICE_CMD_GET_TOTAL_SIZE,
    BLOCK_DEVICE_CMD_GET_PAGE_SIZE,
    BLOCK_DEVICE_CMD_GET_BLOCK_SIZE,
    BLOCK_DEVICE_CMD_NUM,
} BLOCK_DEVICE_CMD;

typedef enum DEVICE_PART_CMD_T
{
    DEVICE_PART_CMD_ERASE_ALL = 0x00,
    DEVICE_PART_CMD_ERASE_SECTOR,
    DEVICE_PART_CMD_GET_TOTAL_SIZE,
    DEVICE_PART_CMD_GET_BLOCK_SIZE,
    DEVICE_PART_CMD_NUM,
} DEVICE_PART_CMD;

typedef struct _blk_dev_erase_t
{
    uint32_t addr;
    uint32_t len;
} blk_dev_erase_t;

void del_blkpart(struct blkpart *blk);
int add_blkpart(struct blkpart *blk);
void blkpart_add_list(struct blkpart *blk);
void blkpart_del_list(struct blkpart *blk);

#define PARTINDEX_THE_LAST UINT32_MAX
struct blkpart *get_blkpart_by_name(const char *name);
struct part *get_part_by_index(const char *blk_name, uint32_t index);
struct part *get_part_by_name(const char *name);


#define PARTINDEX_THE_LAST UINT32_MAX
struct part *get_part_by_name(const char *name);

int remove_part_from_list(struct part *target_part);
int add_part_to_list(struct part *target_part);

int create_register_part(rt_device_t dev,
                         const char *part_name,
                         unsigned int part_offset,
                         unsigned int part_bytes);

int part_system_init(void);

rt_size_t part_erase_before_write(rt_device_t dev, rt_off_t offset, const void *data, rt_size_t size);
rt_size_t part_erase_without_write(rt_device_t dev, rt_off_t offset, const void *data, rt_size_t size);
rt_size_t part_read(rt_device_t dev, rt_off_t offset, void *data, rt_size_t size);
rt_err_t part_control(rt_device_t dev, int cmd, void *args);

#ifdef __cplusplus
}
#endif

#endif
