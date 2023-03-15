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
//#include <string.h>
#include "psram.h"
#include "_psram.h"
#include "hal_psramctrl.h"

/* define for Latency
 * MR0[5]=0b(Variable Latency)
 * MR0[4:2], Latency, Max push out, Standard, Extended
 *   010        2          4           66        66
 *   011        3          6          109       109
 *   100        4(default) 8          133       133
 *   101        5         10          166       166
 *   110        6         12        200/233    200/233
 *
 *MR0 should be writen for 0x1a after power on.
*/

int32_t psram_aps64_init(struct psram_chip *chip, struct psram_ctrl *ctrl)
{
	int32_t id;
	int32_t density;

	chip->buswidth = 8;
	chip->wrap_len = PSRAMC_PAGE_SZ_1K;

	PR_DBG("========psram_sw_reset1========\n");
	psram_sw_reset(chip);

	PR_DBG("========psram_read_density========\n");
	density = Psram_Read_Mr(chip, 2);
	if ((density & 0x7) != 0x3) {
		PR_ERR("Get APS64 64Mb wrong density:0x%x\n", density);
		return -1;
	} else {
		PR_DBG("Get APS64 density success:0x%x\n", density);
	}

	PR_DBG("========Psram_Read_Id========\n");
	id = Psram_Read_Mr(chip, 1);
	if ((id & 0x0f) != 0x0d) { /* Vendor ID: 01101b(APM) */
		PR_ERR("Get APS64 wrong ID:0x%x\n", id);
		return -1;
	}
	PR_DBG("Get APS64 ID:0x%x\n", id);

	PR_DBG("========Psram_Read_die_sta========\n");
	id = Psram_Read_die_sta(chip);
	if (!id) {
		PR_ERR("APS64 BAD die:0x%x\n", id);
		return -1;
	}
	//HAL_PsramCtrl_MaxCE_LowCyc(chip->ctrl, chip->freq);

	psram_set_read_latency(chip, 0); //MR0 0x11
	psram_set_write_latency(chip, PSRAM_CHIP_OPI_APS64, 1); //MR4 0x20

	HAL_PsramCtrl_Set_CBUS_WR_LATENCY(chip->ctrl, 7 << 8);

	psram_idbus_op_cmd(chip, O_SYNC_BURST_WRITE);
	psram_idbus_op_cmd(chip, O_SYNC_BURST_READ);
#ifndef CFG_PSRAM_DISABLE_CACHE
	HAL_PsramCtrl_CacheCfg(chip->ctrl, 0);
#else
	HAL_PsramCtrl_CacheCfg(chip->ctrl, 1);
#endif
	HAL_PsramCtrl_DMACrossEnable(1);
	chip->name = "APS64";
	chip->capacity = 8 * 1024 * 1024;
#ifndef CONFIG_PSRAM_200M
	HAL_PLL_FREQ_SET(1, 192000000);
	HAL_PsramCtrl_DQS_Delay_Calibration();
#endif

	hal_udelay(1000);

	return 0;
}
