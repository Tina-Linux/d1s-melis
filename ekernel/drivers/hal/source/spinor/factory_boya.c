/* all about boya factory */

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <hal_timer.h>

#include "inter.h"

#define NOR_BOYA_QE_BIT BIT(1)
#define NOR_BOYA_CMD_RDSR2 0x35
#define NOR_BOYA_CMD_WRSR2 0x31

static struct nor_info idt_boya[] =
{
    {
        .model = "BOYA25Q64A",
        .id = {0x68, 0x40, 0x17},
        .total_size = SZ_8M,
        /* .flag = SUPPORT_GENERAL */
	.flag = SUPPORT_ALL_ERASE_BLK,
    },
    {
        .model = "BOYA25Q32B",
        .id = {0x68, 0x40, 0x16},
        .total_size = SZ_4M,
        /* .flag = SUPPORT_GENERAL */
	.flag = SUPPORT_ALL_ERASE_BLK,
    },
    {
        .model = "BOYA25Q80A",
        .id = {0x68, 0x40, 0x14},
        .total_size = SZ_1M,
        /* .flag = SUPPORT_GENERAL */
	.flag = SUPPORT_ALL_ERASE_BLK,
    },
    {
        .model = "BOYA25D40",
        .id = {0x68, 0x40, 0x13},
        .total_size = SZ_512K,
        /* .flag = SUPPORT_GENERAL */
	.flag = SUPPORT_ALL_ERASE_BLK,
    },
};

static int nor_boya_quad_mode(struct nor_flash *unused)
{
    int ret;
    unsigned char cmd[3];
    char reg[2] = {0};

    cmd[0] = NOR_BOYA_CMD_RDSR2;
    ret = nor_transfer(1, cmd, 1, reg, 2);
    if (ret) {
        SPINOR_ERR("read status register2 fail\n");
        return ret;
    }

    if (reg[1] & NOR_BOYA_QE_BIT)
        return 0;

    ret = nor_write_enable();
    if (ret)
        return ret;

    cmd[0] = NOR_BOYA_CMD_WRSR2;
    cmd[1] = reg[1] | NOR_BOYA_QE_BIT;
    ret = nor_transfer(2, cmd, 2, NULL, 0);
    if (ret) {
        SPINOR_ERR("set status register fail\n");
        return ret;
    }

    if (nor_wait_ready(0, 500)) {
        SPINOR_ERR("wait set qd mode failed\n");
        return -EBUSY;
    }

	reg[0] = 0;
	reg[1] = 0;
    cmd[0] = NOR_BOYA_CMD_RDSR2;
    ret = nor_transfer(1, cmd, 1, reg, 2);
    if (ret) {
        SPINOR_ERR("read status register2 fail\n");
        return ret;
    }


    if (!(reg[1] & NOR_BOYA_QE_BIT)) {
        SPINOR_ERR("set gd QE failed\n");
        return -EINVAL;
    }
    return 0;
}

static struct nor_factory nor_boya = {
    .factory = FACTORY_BOYA,
    .idt = idt_boya,
    .idt_cnt = sizeof(idt_boya),

    .init = NULL,
    .deinit = NULL,
    .init_lock = NULL,
    .deinit_lock = NULL,
    .lock = NULL,
    .unlock = NULL,
    .islock = NULL,
    .set_quad_mode = nor_boya_quad_mode,
    .set_4bytes_addr = NULL,
};

int nor_register_factory_boya(void)
{
    return nor_register_factory(&nor_boya);
}
