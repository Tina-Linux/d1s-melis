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

//#include <stdio.h>
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

int32_t psram_aps32_init(struct psram_chip *chip, struct psram_ctrl *ctrl)
{
	int32_t id;
	int32_t density[2];

	chip->buswidth = 8;
	chip->wrap_len = PSRAMC_PAGE_SZ_1K;

	PR_DBG("========psram_sw_reset1========\n");
	psram_sw_reset(chip);
	PR_DBG("========psram_sw_reset2========\n");
	psram_sw_reset(chip);

	PR_DBG("========psram_read_density========\n");
	density[0] = Psram_Read_Mr(chip, 1);
	density[1] = Psram_Read_Mr(chip, 2);
	if (!(density[0] & HAL_BIT(5)) && ((density[1] & 0x7) == 0x1)) {
		PR_DBG("Get APS32 density success\n");
	} else {
		PR_ERR("Get APS32 32Mb wrong density:MR1(0x%x),MR2(0x%x)\n",
				density[0], density[1]);
		return -1;
	}

	PR_DBG("========Psram_Read_Id========\n");
	id = Psram_Read_Id(chip);
	if ((id & 0x0f) != 0x0d) { /* Vendor ID: 01101b(APM) */
		PR_ERR("Get APS32 wrong ID:0x%x\n", id);
		return -1;
	}

	PR_DBG("Get APS32 ID:0x%x\n", id);

	PR_DBG("========Psram_Read_Info========\n");
	Psram_Read_Info(chip);
	PR_DBG("========Psram_Read_die_sta========\n");
	id = Psram_Read_die_sta(chip);
	if (!id) {
		PR_ERR("APS32 BAD die:0x%x\n", id);
		return -1;
	}

	//HAL_PsramCtrl_MaxCE_LowCyc(chip->ctrl, chip->freq);

	PR_DBG("========psram_set_read_latency========\n");
	if (chip->freq >= 66000000)
		psram_set_read_latency(chip, 0); //MR0 0x1a

	PR_DBG("========psram_set_write_latency========\n");
	if (chip->freq >= 166000000) {
		psram_set_write_latency(chip, PSRAM_CHIP_OPI_APS32, 2); //MR4 0x80
		HAL_PsramCtrl_Set_SBUS_WR_LATENCY(chip->ctrl, 2);
		HAL_PsramCtrl_Set_CBUS_WR_LATENCY(chip->ctrl, 2 << 8);
	} else {
		psram_set_write_latency(chip, PSRAM_CHIP_OPI_APS32, 0); //MR4 0x00
		HAL_PsramCtrl_Set_CBUS_WR_LATENCY(chip->ctrl, 0 << 8);
	}

#if 0
	psram_set_drv(chip, 1);
	PR_DBG("===========psram_read_16byt_test =============\n");
	HAL_UDelay(2 * 1000 * 1000);
	psram_read_16byte_test(chip);
#endif

	psram_idbus_op_cmd(chip, O_SYNC_WRITE);
	psram_idbus_op_cmd(chip, O_SYNC_READ);
#ifndef CFG_PSRAM_DISABLE_CACHE
	HAL_PsramCtrl_CacheCfg(chip->ctrl, 0);
#else
	HAL_PsramCtrl_CacheCfg(chip->ctrl, 1);
#endif
	HAL_PsramCtrl_DMACrossEnable(1);

	chip->name = "APS32";
	chip->capacity = 4 * 1024 * 1024;

	hal_udelay(1000);

	HAL_PsramCtrl_DQS_Delay_Calibration();

	return 0;
}
