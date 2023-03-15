/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.


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
#include <sunxi_hal_common.h>
#include "platform_mbus.h"
#include "stdint.h"
#include "ahb.h"


static unsigned long reg_base = AHB_MONT_BASE;
static uint32_t ahb_id_used[ID_NUM];


uint32_t ahb_get_sum_read(enum ahb_id type)
{
	unsigned value;

	switch (type) {
		case AHB_ID0 :
			value = hal_readl(reg_base + AHB_MONT_RD_BW_SUM_ID0);
			break;
		case AHB_ID1 :
			value = hal_readl(reg_base + AHB_MONT_RD_BW_SUM_ID1);
			break;
		case AHB_ID2 :
			value = hal_readl(reg_base + AHB_MONT_RD_BW_SUM_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", type);
			return -1;
	}

	return value;
}

uint32_t ahb_get_sum_write(enum ahb_id type)
{
	unsigned value;

	switch (type) {
		case AHB_ID0 :
			value = hal_readl(reg_base + AHB_MONT_WR_BW_SUM_ID0);
			break;
		case AHB_ID1 :
			value = hal_readl(reg_base + AHB_MONT_WR_BW_SUM_ID1);
			break;
		case AHB_ID2 :
			value = hal_readl(reg_base + AHB_MONT_WR_BW_SUM_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", type);
			return -1;
	}

	return value;
}

uint32_t ahb_get_max_read(enum ahb_id type)
{
	unsigned value;

	switch (type) {
		case AHB_ID0 :
			value = hal_readl(reg_base + AHB_MONT_MAX_RD_BW_ID0);
			break;
		case AHB_ID1 :
			value = hal_readl(reg_base + AHB_MONT_MAX_RD_BW_ID1);
			break;
		case AHB_ID2 :
			value = hal_readl(reg_base + AHB_MONT_MAX_RD_BW_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", type);
			return -1;
	}

	return value;
}

uint32_t ahb_get_max_write(enum ahb_id type)
{
	unsigned value;

	switch (type) {
		case AHB_ID0 :
			value = hal_readl(reg_base + AHB_MONT_MAX_WR_BW_ID0);
			break;
		case AHB_ID1 :
			value = hal_readl(reg_base + AHB_MONT_MAX_WR_BW_ID1);
			break;
		case AHB_ID2 :
			value = hal_readl(reg_base + AHB_MONT_MAX_WR_BW_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", type);
			return -1;
	}

	return value;
}

uint32_t ahb_get_sum_total_read(void)
{
	unsigned value;

	value = hal_readl(reg_base + AHB_MONT_RD_BW_SUM_TOTAL);

	return value;
}

uint32_t ahb_get_sum_total_write(void)
{
	unsigned value;

	value = hal_readl(reg_base + AHB_MONT_WR_BW_SUM_TOTAL);
	return value;
}

uint32_t ahb_get_max_total_read(void)
{
	unsigned value;

	value = hal_readl(reg_base + AHB_MONT_MAX_RD_BW_TOTAL);

	return value;
}

uint32_t ahb_get_max_total_write(void)
{
	unsigned value;

	value = hal_readl(reg_base + AHB_MONT_MAX_WR_BW_TOTAL);

	return value;
}

uint32_t ahb_request_id_chan(void)
{
	int i;

	for (i = 0; i < ID_NUM; i++) {

		if (!ahb_id_used[i]) {
			ahb_id_used[i] = 1;
			return i;
		}
	}

	ahb_err("ahb:the ahb id is busy\n");

	return -1;
}

uint32_t ahb_disable_id_chan(enum ahb_id type)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (type) {
		case AHB_ID0 :
			value &= ID0_MASK;
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			hal_writel(0x3, reg_base+AHB_MONT_CTRL);
			hal_writel(0x1, reg_base+AHB_MONT_CTRL);
			ahb_id_used[0] = 0;
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			hal_writel(0x3, reg_base+AHB_MONT_CTRL);
			hal_writel(0x1, reg_base+AHB_MONT_CTRL);
			ahb_id_used[1] = 0;
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			hal_writel(0x3, reg_base+AHB_MONT_CTRL);
			hal_writel(0x1, reg_base+AHB_MONT_CTRL);
			ahb_id_used[2] = 0;
			break;
		default:
			ahb_err("not support ahb id type: %d\n", type);
			return -1;
	}

	return 0;
}

void ahb_set_cpu0_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(CPU0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(CPU0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(CPU0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_cpu1_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(CPU1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(CPU1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(CPU1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_riscv_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(RISCV_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(RISCV_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(RISCV_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_dsp_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(DSP_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(DSP_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(DSP_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_ce_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(CE_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(CE_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(CE_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_dma0_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(DMA0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(DMA0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(DMA0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_dma1_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(DMA1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(DMA1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(DMA1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_csi_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(CSI_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(CSI_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(CSI_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_st0_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(ST0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(ST0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(ST0_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

void ahb_set_st1_huster(uint32_t id)
{
	unsigned value;
	value = hal_readl(reg_base + AHB_MONT_MST_ID);
	switch (id) {
		case AHB_ID0 :
			value &= ID0_MASK;
			value |= ID0_HUSTER(ST1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID1 :
			value &= ID1_MASK;
			value |= ID1_HUSTER(ST1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		case AHB_ID2 :
			value &= ID2_MASK;
			value |= ID2_HUSTER(ST1_HUSTER);
			hal_writel(value, reg_base+AHB_MONT_MST_ID);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
	}
}

uint32_t ahb_get_cpu0_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;
}

uint32_t ahb_get_cpu1_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_riscv_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_dsp_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_ce_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_dma0_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_dma1_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_csi_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_st0_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_max_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}
uint32_t ahb_get_st1_ddr(uint32_t id)
{
	uint32_t addr = 0;
	switch (id) {
		case AHB_ID0 :
			addr = ahb_get_sum_read(AHB_ID0);
			addr += ahb_get_sum_write(AHB_ID0);
			break;
		case AHB_ID1 :
			addr = ahb_get_sum_read(AHB_ID1);
			addr += ahb_get_sum_write(AHB_ID1);
			break;
		case AHB_ID2 :
			addr = ahb_get_sum_read(AHB_ID2);
			addr += ahb_get_sum_write(AHB_ID2);
			break;
		default:
			ahb_err("not support ahb id type: %d\n", id);
			return -1;
	}

	return addr;

}


void mtop_ahb_enable(void)
{

	unsigned reg_val = 0;

	reg_val |= BIT(0);
	hal_writel(reg_val, reg_base + AHB_MONT_CTRL);

	reg_val |= BIT(1);
	hal_writel(reg_val, reg_base + AHB_MONT_CTRL);

	reg_val &= BIT(0);
	hal_writel(reg_val, reg_base + AHB_MONT_CTRL);

	hal_writel(MONT_PRD_NUM, reg_base + AHB_MONT_PRD);
}




