/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY��S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS��SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY��S TECHNOLOGY.
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

#include "dbi_sun8iw20.h"

struct sunxi_dbi_dev *dbi_dev[2];


bool dbi_dev_set_base(u32 sel, uintptr_t base)
{
	dbi_dev[sel] = (struct sunxi_dbi_dev *)base;

	return true;
}

bool set_chip_select_control(u32 sel)
{
	dbi_dev[sel]->trans_ctrl.dwval = 0x00;

	dbi_dev[sel]->trans_ctrl.bits.sdm = 1;
	dbi_dev[sel]->trans_ctrl.bits.dhb = 1;
	dbi_dev[sel]->trans_ctrl.bits.ss_owner = 1;
	dbi_dev[sel]->trans_ctrl.bits.spol = 1;

	return true;
}

bool is_master_mode(u32 sel)
{
	return dbi_dev[sel]->gc.bits.mode;
}

bool set_master_counter(u32 sel, u32 tx_len)
{
	dbi_dev[sel]->mbc.bits.mbc = tx_len;
	dbi_dev[sel]->mwtc.bits.mwtc = tx_len;
	dbi_dev[sel]->bcc.bits.stc = tx_len;

	return true;
}

bool config_dbi_enable_mode(u32 sel, u32 mode)
{
	dbi_dev[sel]->dbi_ctrl.bits.soft_tri = mode;

	return true;
}

bool config_dbi_output_clk_mode(u32 sel, u32 mode)
{

	dbi_dev[sel]->dbi_ctrl.bits.output_clk_mode = mode;

	return true;
}

bool config_dbi_output_format(u32 sel, u32 format)
{
	if (format <= DBI_RGB888) {
		dbi_dev[sel]->dbi_ctrl0.bits.output_format = format;
		return true;
	}

	return false;
}

bool set_dcx(u32 sel, u32 dcx)
{
	dbi_dev[sel]->dbi_ctrl.bits.dcx_data = dcx;
	return true;
}

bool config_dbi_data_src_sel(u32 sel, u32 arg)
{
	dbi_dev[sel]->dbi_ctrl.bits.data_src_sel = arg;
	return true;
}

bool set_dbi_en_mode(u32 sel, u32 mode)
{
	dbi_dev[sel]->dbi_ctrl.bits.mode_sel = mode;
	return true;
}

bool dbi_irq_enable(u32 sel, enum dbi_mode_type mode_type)
{
	switch (mode_type) {
	case DBI_MODE_TYPE_RX: {
		dbi_dev[sel]->dbi_int.bits.rd_done_int_en = 1;
		}
	case DBI_MODE_TYPE_TX: {
		dbi_dev[sel]->dbi_int.bits.dbi_fifo_empty_int_en = 1;
		}
	default : {
		 return false;
		}
	}

	return true;
}

bool dbi_irq_disable(u32 sel, enum dbi_mode_type mode_type)
{
	switch (mode_type) {
	case DBI_MODE_TYPE_RX: {
		dbi_dev[sel]->dbi_int.bits.rd_done_int_en = 0;
		}
	case DBI_MODE_TYPE_TX: {
		dbi_dev[sel]->dbi_int.bits.dbi_fifo_empty_int_en = 0;
		}
	default : {
		 return false;
		}
	}

	return true;
}

void default_config(u32 sel)
{
	dbi_dev[sel]->dbi_ctrl.bits.data_src_sel = 0;
}
