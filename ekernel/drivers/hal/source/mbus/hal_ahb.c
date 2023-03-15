/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY¡¯S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS¡¯SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY¡¯S TECHNOLOGY.


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

#include <stdint.h>
#include <stdio.h>
#include <hal_clk.h>

#include "sunxi_hal_ahb.h"
#include "ahb.h"

struct huster_id_chan  id_chan[HUSTER_ID_NUM];

hal_ahb_status_t hal_ahb_request_id_chan(struct huster_id_chan *huster_chan)
{
	if (!huster_chan->ahb_id_huster) {
		huster_chan->ahb_id_num = ahb_request_id_chan();
		if (huster_chan->ahb_id_num < 0) {
			printf("the ahb channel is busy\n");
			return HAL_AHB_STATUS_BUSY_CHANNEL;
		}
		huster_chan->ahb_id_huster = 1;
	}

	return HAL_AHB_STATUS_OK;
}

hal_ahb_status_t hal_ahb_huster_get_value(enum ahb_huster type, unsigned int *value)
{
	uint32_t id_num;
	int ret;

	switch (type) {
		case AHB_CPU0 :
			if (!id_chan[0].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[0]);
				if (ret)
					return ret;
				ahb_set_cpu0_huster(id_chan[0].ahb_id_num);
			}
			*value = ahb_get_cpu0_ddr(id_chan[0].ahb_id_num);
			break;
		case AHB_CPU1 :
			if (!id_chan[1].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[1]);
				if (ret)
					return ret;
				ahb_set_cpu1_huster(id_chan[1].ahb_id_num);
			}
			*value = ahb_get_cpu1_ddr(id_chan[1].ahb_id_num);
			break;
		case AHB_RISCV :
			if (!id_chan[2].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[2]);
				if (ret)
					return ret;
				ahb_set_riscv_huster(id_chan[2].ahb_id_num);
			}

			*value = ahb_get_riscv_ddr(id_chan[2].ahb_id_num);
			break;
		case AHB_DSP :
			if (!id_chan[3].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[3]);
				if (ret)
					return ret;
				ahb_set_dsp_huster(id_chan[3].ahb_id_num);
			}
			*value = ahb_get_dsp_ddr(id_chan[3].ahb_id_num);
			break;
		case AHB_CE :
			if (!id_chan[4].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[4]);
				if (ret)
					return ret;
				ahb_set_ce_huster(id_chan[4].ahb_id_num);
			}
			*value = ahb_get_ce_ddr(id_chan[4].ahb_id_num);
			break;
		case AHB_DMA0 :
			if (!id_chan[5].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[5]);
				if (ret)
					return ret;
				ahb_set_dma0_huster(id_chan[5].ahb_id_num);
			}
			*value = ahb_get_dma0_ddr(id_chan[5].ahb_id_num);
			break;
		case AHB_DMA1 :
			if (!id_chan[6].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[6]);
				if (ret)
					return ret;
				ahb_set_dma1_huster(id_chan[6].ahb_id_num);
			}
			*value = ahb_get_dma1_ddr(id_chan[6].ahb_id_num);
			break;
		case AHB_CSI :
			if (!id_chan[7].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[7]);
				if (ret)
					return ret;
				ahb_set_csi_huster(id_chan[7].ahb_id_num);
			}
			*value = ahb_get_csi_ddr(id_chan[7].ahb_id_num);
			break;
		case AHB_ST0 :
			if (!id_chan[8].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[8]);
				if (ret)
					return ret;
				ahb_set_st0_huster(id_chan[8].ahb_id_num);
			}
			*value = ahb_get_st0_ddr(id_chan[8].ahb_id_num);
			break;
		case AHB_ST1 :
			if (!id_chan[9].ahb_id_huster) {
				ret = hal_ahb_request_id_chan(&id_chan[9]);
				if (ret)
					return ret;
				ahb_set_st1_huster(id_chan[9].ahb_id_num);
			}
			*value = ahb_get_st1_ddr(id_chan[9].ahb_id_num);
			break;
		default :
			ahb_err("not support mbus type, %d\n", type);
			return HAL_AHB_STATUS_ERROR_PARAMETER;
	}

	return HAL_AHB_STATUS_OK;
}

hal_ahb_status_t hal_ahb_huster_enable(void)
{
	hal_clk_type_t clk_type = HAL_SUNXI_CCU;
	hal_clk_id_t clk_id = CLK_BUS_MONITOR;
	hal_clk_status_t ret;
	hal_clk_t clk;

	clk = hal_clock_get(clk_type, clk_id);
	ret = hal_clock_enable(clk);
	if (ret != HAL_CLK_STATUS_OK){
		ahb_err("monitor clock enable failed.\n");
		return HAL_AHB_STATUS_ERROR;
	}
	else{
		mtop_ahb_enable();
		return HAL_AHB_STATUS_OK;
	}
}

hal_ahb_status_t hal_ahb_disable_id_chan(enum ahb_huster type)
{

	switch (type) {
		case AHB_CPU0:
			ahb_disable_id_chan(id_chan[0].ahb_id_num);
			id_chan[0].ahb_id_huster = 0;
			break;
		case AHB_CPU1:
			ahb_disable_id_chan(id_chan[1].ahb_id_num);
			id_chan[1].ahb_id_huster = 0;
			break;
		case AHB_RISCV:
			ahb_disable_id_chan(id_chan[2].ahb_id_num);
			id_chan[2].ahb_id_huster = 0;
			break;
		case AHB_DSP:
			ahb_disable_id_chan(id_chan[3].ahb_id_num);
			id_chan[3].ahb_id_huster = 0;
			break;
		case AHB_CE:
			ahb_disable_id_chan(id_chan[4].ahb_id_num);
			id_chan[4].ahb_id_huster = 0;
			break;
		case AHB_DMA0:
			ahb_disable_id_chan(id_chan[5].ahb_id_num);
			id_chan[5].ahb_id_huster = 0;
			break;
		case AHB_DMA1:
			ahb_disable_id_chan(id_chan[6].ahb_id_num);
			id_chan[6].ahb_id_huster = 0;
			break;
		case AHB_CSI:
			ahb_disable_id_chan(id_chan[7].ahb_id_num);
			id_chan[7].ahb_id_huster = 0;
			break;
		case AHB_ST0:
			ahb_disable_id_chan(id_chan[8].ahb_id_num);
			id_chan[8].ahb_id_huster = 0;
			break;
		case AHB_ST1:
			ahb_disable_id_chan(id_chan[9].ahb_id_num);
			id_chan[9].ahb_id_huster = 0;
			break;
		default:
			ahb_err("not support ahb type, %d\n", type);
			return HAL_AHB_STATUS_ERROR_PARAMETER;
	}

	return HAL_AHB_STATUS_OK;
}
