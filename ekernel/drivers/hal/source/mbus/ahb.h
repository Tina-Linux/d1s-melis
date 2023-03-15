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


#ifndef __MTOP_H
#define __MTOP_H


#include <stdint.h>
#include "platform_mbus.h"

#ifndef BIT
#define BIT(x) (1UL << (x))
#endif

#define ahb_err(fmt, args...) printf("%s()%d - "fmt, __func__, __LINE__, ##args)

#define ID0_MASK	0x7FE0
#define ID1_MASK	0x7C1F
#define ID2_MASK	0x3FF
#define ID0_HUSTER(x)	((x) << 0)
#define ID1_HUSTER(x)	((x) << 5)
#define ID2_HUSTER(x)	((x) << 10)
#define ID_NUM		3

enum ahb_id {
	AHB_ID0  = 0,
	AHB_ID1,
	AHB_ID2,
};

struct id_chan {
	uint32_t id0_used;
	uint32_t id1_used;
	uint32_t id2_used;
};

struct huster_id_chan {
	uint32_t ahb_id_huster;
	uint32_t ahb_id_num;
};



void mtop_ahb_enable(void);

uint32_t ahb_get_sum_read(enum ahb_id type);
uint32_t ahb_get_sum_write(enum ahb_id type);
uint32_t ahb_get_max_read(enum ahb_id type);
uint32_t ahb_get_max_write(enum ahb_id type);
uint32_t ahb_request_id_chan(void);
uint32_t ahb_disable_id_chan(enum ahb_id type);
uint32_t ahb_get_cpu0_ddr(uint32_t id);
uint32_t ahb_get_cpu1_ddr(uint32_t id);
uint32_t ahb_get_riscv_ddr(uint32_t id);
uint32_t ahb_get_dsp_ddr(uint32_t id);
uint32_t ahb_get_ce_ddr(uint32_t id);
uint32_t ahb_get_dma0_ddr(uint32_t id);
uint32_t ahb_get_dma1_ddr(uint32_t id);
uint32_t ahb_get_csi_ddr(uint32_t id);
uint32_t ahb_get_st0_ddr(uint32_t id);
uint32_t ahb_get_st1_ddr(uint32_t id);

void ahb_set_cpu0_huster(uint32_t id);
void ahb_set_cpu1_huster(uint32_t id);
void ahb_set_riscv_huster(uint32_t id);
void ahb_set_dsp_huster(uint32_t id);
void ahb_set_ce_huster(uint32_t id);
void ahb_set_dma0_huster(uint32_t id);
void ahb_set_dma1_huster(uint32_t id);
void ahb_set_csi_huster(uint32_t id);
void ahb_set_st0_huster(uint32_t id);
void ahb_set_st1_huster(uint32_t id);
#endif
