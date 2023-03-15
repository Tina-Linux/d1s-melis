/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA,
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MAT
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

#include "psram.h"
#include "_psram.h"
#include "hal_psramctrl.h"
#include "sunxi_hal_lspsram.h"

static uint8_t standby_flag = 0;

static int32_t psram_wait_for_req(struct psram_chip *chip, struct psram_request *mrq)
{
	if (standby_flag)
		return HAL_PsramCtrl_Request(chip->ctrl, mrq, 1);
	else
		return HAL_PsramCtrl_Request(chip->ctrl, mrq, 0);
}

int32_t psram_set_read_latency(struct psram_chip *chip, uint32_t fixed)
{
	uint8_t rval;
	struct psram_request mrq = {{0}, {0}};

	/* read MR0 status */
	mrq.cmd.resp = &rval;
	mrq.data.blksz = 1;

	mrq.cmd.opcode = Mode_Reg_Read;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_READ_BYTE;
	mrq.cmd.addr = MR0;
	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	PR_DBG("%s,%d read rval:%x\n", __func__, __LINE__, rval);

	/* write MR0 to set latency*/
	rval &= ~((1 << 5) | (0x7 << 2));
	if (chip->type == PSRAM_CHIP_OPI_APS32)
		rval |= (fixed << 5) | 0x1a; //for aps32
	else if (chip->type == PSRAM_CHIP_OPI_APS64)
		rval |= (fixed << 5) | 0x11; //for aps64

	PR_DBG("%s,%d set rval:%x\n", __func__, __LINE__, rval);

	mrq.data.buff = &rval;
	mrq.cmd.opcode = Mode_Reg_Write;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_WRITE_BYTE;
	mrq.cmd.addr = MR0;
	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	return 0;
}

int32_t psram_set_write_latency(struct psram_chip *chip, uint32_t p_type, uint32_t wlc)
{
	uint8_t rval;
	struct psram_request mrq = {{0}, {0}};

	mrq.data.blksz = 1;

	mrq.cmd.opcode = Mode_Reg_Read;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_READ_BYTE;
	mrq.cmd.addr = MR4;
	mrq.cmd.resp = &rval;

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	PR_DBG("%s,%d read rval:%x\n", __func__, __LINE__, rval);

	rval &= ~(0xe0);
	if (p_type == PSRAM_CHIP_OPI_APS32)
		rval |= wlc << 6;
	else if (chip->type == PSRAM_CHIP_OPI_APS64)
		rval |= wlc << 5;

	mrq.cmd.opcode = Mode_Reg_Write;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_WRITE_BYTE;
	mrq.data.buff = &rval;

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	return 0;
}

int psram_sw_reset(struct psram_chip *chip)
{
	uint32_t value = 0;
	struct psram_request mrq = {{0}, {0}};

	// write halfword
	if (chip->type == PSRAM_CHIP_OPI_APS32 || chip->type == PSRAM_CHIP_OPI_APS64) {
		mrq.cmd.opcode = Global_Reaet;
		mrq.data.blksz = 1;
		mrq.data.blocks = 2;
		value = 0;
		mrq.data.buff = (uint8_t *)&value;
		mrq.data.flags = PSRAM_DATA_WRITE_SHORT;
	}
	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	hal_udelay(200);

	//write byte
	mrq.cmd.opcode = Global_Reaet;
	mrq.data.blksz = 1;
	mrq.data.blocks = 1;
	value = 0;
	mrq.data.buff = (uint8_t *)&value;
	mrq.data.flags = PSRAM_DATA_WRITE_BYTE;
	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;
	hal_udelay(1000);

	return 0;
}

int32_t psram_set_drv(struct psram_chip *chip, uint32_t drv)
{
	uint8_t rval;
	struct psram_request mrq = {{0}, {0}};

	mrq.data.blksz = 1; /* byte */

	mrq.cmd.opcode = Mode_Reg_Read;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_READ_BYTE;
	mrq.cmd.addr = MR0;
	mrq.cmd.resp = &rval;

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	rval &= ~(0x3);
	rval |= drv;

	mrq.cmd.opcode = Mode_Reg_Write;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_WRITE_BYTE;
	mrq.data.buff = &rval;

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	return 0;
}

int32_t Psram_Read_Id(struct psram_chip *chip)
{
	uint8_t p_id = 0;
	struct psram_request mrq = {{0}, {0}};

	mrq.cmd.resp = &p_id;
	mrq.data.blksz = 1; /* byte */
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_READ_BYTE;
	mrq.cmd.flags = PSRAM_ADDR_PRESENT;

	switch (chip->type) {
	case PSRAM_CHIP_OPI_APS32:
		mrq.cmd.opcode = Mode_Reg_Read;
		mrq.cmd.addr = MR1;
		break;
	case PSRAM_CHIP_OPI_APS64:
		mrq.cmd.opcode = Mode_Reg_Read;
		mrq.cmd.addr = MR1;
		break;
	default:
		break;
	}

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	return p_id;
}

int32_t Psram_Read_Mr(struct psram_chip *chip, uint32_t mreg)
{
	uint16_t p_id[5];
	struct psram_request mrq = {{0}, {0}};

	mrq.cmd.resp = (uint8_t *)p_id;
	mrq.data.blksz = 1; /* byte */
	mrq.cmd.flags = PSRAM_ADDR_PRESENT;

	switch (chip->type) {
	case PSRAM_CHIP_OPI_APS32:
		mrq.cmd.opcode = Mode_Reg_Read;
		mrq.data.flags = PSRAM_DATA_READ_BYTE;
		mrq.cmd.addr = mreg;
		mrq.data.blocks = 2;
		break;
	case PSRAM_CHIP_OPI_APS64:
		mrq.cmd.opcode = Mode_Reg_Read;
		mrq.data.flags = PSRAM_DATA_READ_SHORT;
		mrq.cmd.addr = mreg;
		mrq.data.blocks = 2;
		break;
	default:
		break;
	}

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	return p_id[0];
}

int32_t Psram_Read_Info(struct psram_chip *chip)
{
	uint8_t mode_regs[8] = {0};
	struct psram_request mrq = {{0}, {0}};

	mrq.data.blksz = 1; /* byte */
	mrq.data.blocks = 2;
	mrq.data.flags = PSRAM_DATA_READ_SHORT;
	mrq.cmd.flags = PSRAM_ADDR_PRESENT;

	switch (chip->type) {
	case PSRAM_CHIP_OPI_APS32:
		mrq.cmd.opcode = Mode_Reg_Read;
		break;
	case PSRAM_CHIP_OPI_APS64:
		mrq.cmd.opcode = Mode_Reg_Read;
		break;
	default:
		break;
	}

	mrq.cmd.flags = PSRAM_ADDR_PRESENT;
	for (int i = 0; i < 4; i++) {
		mrq.cmd.resp = &mode_regs[i*2];
		mrq.cmd.addr = i*2;
		if (psram_wait_for_req(chip, &mrq) != 0)
			return -1;
	}
	return 0;
}

int32_t psram_read_16byte_test(struct psram_chip *chip)
{
	uint8_t mode_regs[8] = {0};
	struct psram_request mrq = {{0}, {0}};

	mrq.data.blksz = 4;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_READ_SHORT;

	mrq.cmd.flags = PSRAM_ADDR_PRESENT;
	mrq.cmd.opcode = Mode_Reg_Read;
	mrq.cmd.addr = MR0;
	mrq.cmd.resp = &mode_regs[0];

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	return 0;
}

int32_t Psram_Read_die_sta(struct psram_chip *chip)
{
	uint8_t p_id;
	uint16_t p_id_raw = 0;
	struct psram_request mrq = {{0}, {0}};

	mrq.cmd.resp = (uint8_t *)&p_id_raw;
	mrq.data.blksz = 1; /* byte */

	switch (chip->type) {
	case PSRAM_CHIP_OPI_APS32:
		mrq.cmd.opcode = Mode_Reg_Read;
		mrq.data.blocks = 1;
		mrq.data.flags = PSRAM_DATA_READ_BYTE;
		mrq.cmd.addr = MR2;
		break;
	case PSRAM_CHIP_OPI_APS64:
		mrq.cmd.opcode = Mode_Reg_Read;
		mrq.data.blocks = 1;
		mrq.data.flags = PSRAM_DATA_READ_BYTE;
		mrq.cmd.addr = MR2;
		break;
	default:
		break;
	}

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	p_id = (p_id_raw >> 7) & 0x1; /* bit7: 1:PASS, 0:FAIL */

	return p_id;
}

int32_t psram_enter_hsleep_mode(struct psram_chip *chip)
{
	uint8_t rval;
	struct psram_request mrq = {{0}, {0}};

	mrq.data.blksz = 1; /* byte */

	mrq.cmd.opcode = Mode_Reg_Write;
	mrq.data.blocks = 1;
	mrq.data.flags = PSRAM_DATA_WRITE_BYTE;
	mrq.cmd.addr = MR6;
	rval= 0xF0;
	mrq.data.buff = &rval;

	if (psram_wait_for_req(chip, &mrq) != 0)
		return -1;

	PR_DBG("%s,%d success\n", __func__, __LINE__);

	return 0;
}

void psram_idbus_op_cmd(struct psram_chip *chip, uint32_t opcmd)
{
	uint32_t write = 0, opcfg;

	switch (opcmd) {
		//O_SYNC_WRITEa
	case O_SYNC_WRITE:
		write = 1;
		opcfg = Sync_Write << PSRAMC_S_RW_CFG_RW_COM_SEND_SHIFT;
			//| PSRAMC_CBUS_CMD_8BIT | PSRAMC_CBUS_ADDR_8BIT | PSRAMC_CBUS_DATA_8BIT;
		break;
		//O_SYNC_READ
	case O_SYNC_READ:
		opcfg = Sync_Read << PSRAMC_S_RW_CFG_RW_COM_SEND_SHIFT;
			//| PSRAMC_CBUS_CMD_8BIT | PSRAMC_CBUS_ADDR_8BIT | PSRAMC_CBUS_DATA_8BIT;
		break;
	case O_SYNC_BURST_READ:
		opcfg = Sync_Burst_Read << 24 |
			PSRAMC_CBUS_CMD_8BIT | PSRAMC_CBUS_ADDR_8BIT | PSRAMC_CBUS_DATA_8BIT;
		break;
	case O_SYNC_BURST_WRITE:
		write = 1;
		opcfg = Sync_Burst_Write << PSRAMC_S_RW_CFG_RW_COM_SEND_SHIFT |
			PSRAMC_CBUS_CMD_8BIT | PSRAMC_CBUS_ADDR_8BIT | PSRAMC_CBUS_DATA_8BIT;
		break;
	default:
		PR_ERR("%s,%d unkonw cmd:%d\n", __func__, __LINE__, opcmd);
		return ;
	}

	if (write)
		chip->cbus_wcmd = opcfg >> PSRAMC_S_RW_CFG_RW_COM_SEND_SHIFT;
	else
		chip->cbus_rcmd = opcfg >> PSRAMC_S_RW_CFG_RW_COM_SEND_SHIFT;

	HAL_SET_BIT(PSRAM_CTRL->MEM_COM_CFG, HAL_BIT(5));
	HAL_SET_BIT(PSRAM_CTRL->MEM_COM_CFG, HAL_BIT(6));
	HAL_SET_BIT(PSRAM_CTRL->MEM_COM_CFG, HAL_BIT(16));

	HAL_PsramCtrl_IDbusCfg(chip->ctrl, write, opcfg);
}

/*
static uint16_t u16PsramCheckSum = 0;
static __inline uint16_t psram_data_checksum(void)
{
	return image_checksum16((__PSRAM_BASE, (uint32_t)__PSRAM_LENGTH);
}*/

static struct psram_chip *_chip_priv;

struct psram_chip *psram_chip_get_para(void)
{
	return _chip_priv;
}

void psram_suspend(struct psram_chip *lpsram_para)
{
	standby_flag = 1;
	HAL_MODIFY_REG(PSRAM_CTRL->MEM_COM_CFG, 0x1, 0);
	switch (lpsram_para->type) {
	case PSRAM_CHIP_OPI_APS32:
		psram_set_write_latency(lpsram_para, PSRAM_CHIP_OPI_APS32, 0);
		break;
	case PSRAM_CHIP_OPI_APS64:
		psram_set_write_latency(lpsram_para, PSRAM_CHIP_OPI_APS64, 0);
		break;
	default:
		break;
	}

	HAL_PsramCtrl_Set_SBUS_WR_LATENCY(lpsram_para->ctrl, 0 << 8);
#ifndef CONFIG_PSRAM_200M
	HAL_PsramCtrl_ConfigCCMU(192000000);
#else
	HAL_PsramCtrl_ConfigCCMU(200000000);
#endif
	hal_udelay(10);
	/* HAL_PsramCtrl_Set_DQS_Delay_Cal(192000000); */
	/* HAL_PsramCtrl_MaxCE_LowCyc(lpsram_para->ctrl, 96000000); */
	HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_TIM_CFG, PSRAMC_CS_OUTP_DHCYC_MASK, PSRAMC_CS_OUTP_DHCYC(3));
	hal_udelay(10);
	psram_enter_hsleep_mode(lpsram_para);
	standby_flag = 0;
}

#define PSRAM_FREQ (1920000000)
static int psram_init_after_suspend(struct psram_chip *lpsram_para)
{
	struct psram_chip *chip = lpsram_para;
	struct psram_ctrl *ctrl = chip->ctrl;

	PSRAMCtrl_InitParam cfg;
	PSRAMChip_InitParam psram_chip_cfg;

	cfg.freq = PSRAM_FREQ;
	cfg.rdata_w = 1;
	cfg.p_type = ctrl->p_type;

	HAL_PsramCtrl_Close(ctrl);
	if (HAL_PsramCtrl_Init(ctrl, &cfg)) {
		PR_ERR("psram ctrl init faild!\n");
		return -1;
	}
	HAL_PsramCtrl_Open(0);

	psram_chip_cfg.freq = PSRAM_FREQ;
	psram_chip_cfg.p_type = ctrl->p_type;

	if (psram_init(chip, ctrl, &psram_chip_cfg)) {
		PR_ERR("psram_init init faild!\n");
		return -1;
	}
	PR_DBG("psram init after suspend success!\n");

	return 0;
}

int psram_resume(struct psram_chip *lpsram_para)
{
	int32_t ret = 0;

	standby_flag = 1;
	ret = psram_init_after_suspend(lpsram_para);
	standby_flag = 0;

	return ret;
}

void psram_chip_init(struct psram_chip *chip, struct psram_ctrl *ctrl, PSRAMChip_InitParam *param)
{
	chip->type = param->p_type;
	chip->ctrl = ctrl;
	chip->freq = param->freq;

	_chip_priv = chip;

}

int32_t psram_init(struct psram_chip *chip, struct psram_ctrl *ctrl, PSRAMChip_InitParam *param)
{
	int32_t ret = -1;

	chip->type = param->p_type;
	chip->ctrl = ctrl;
	chip->freq = param->freq;

	switch (chip->type) {
	case PSRAM_CHIP_OPI_APS32:
		ret = psram_aps32_init(chip, ctrl);
		if (ret == 0)
			PR_DBG("Psram aps32 init successfully!\n");
		break;
	case PSRAM_CHIP_OPI_APS64:
		ret = psram_aps64_init(chip, ctrl);
		if (ret == 0)
			PR_DBG("Psram aps64 init successfully!\n");
		break;
	default:
		break;
	}

	//HAL_PsramCtrl_Set_Address_Field(ctrl, 0, PSRAM_START_ADDR, PSRAM_END_ADDR, 0);
	if (chip->type == PSRAM_CHIP_OPI_APS32)
		HAL_PsramCtrl_Set_Address_Field(ctrl, 0, PSRAM_START_ADDR, 0x083fffff, 0); //aps32 addr ranges
	else if (chip->type == PSRAM_CHIP_OPI_APS64)
		HAL_PsramCtrl_Set_Address_Field(ctrl, 0, PSRAM_START_ADDR, 0x087fffff, 0); //aps64 addr ranges
	hal_udelay(10);

	_chip_priv = chip;

	PR_DBG("%s @:%p ret:%d\n", __func__, chip, ret);

	return ret;
}

int32_t psram_deinit(struct psram_chip *chip)
{
	_chip_priv = NULL;
	memset(chip, 0, sizeof(struct psram_chip));
	return 0;
}

void dump_psram_reg(void)
{
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->MEM_COM_CFG, PSRAM_CTRL->MEM_COM_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->OPI_COM_CFG, PSRAM_CTRL->OPI_COM_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->CACHE_CFG, PSRAM_CTRL->CACHE_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->MEN_AC_CFG, PSRAM_CTRL->MEN_AC_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->C_READ_CFG, PSRAM_CTRL->C_READ_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->C_WRITE_CFG, PSRAM_CTRL->C_WRITE_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->S_RW_CFG, PSRAM_CTRL->S_RW_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->S_ADDR_CFG, PSRAM_CTRL->S_ADDR_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->S_WR_NUM, PSRAM_CTRL->S_WR_NUM);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->S_RD_NUM, PSRAM_CTRL->S_RD_NUM);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->FIFO_TRIG_LEV, PSRAM_CTRL->FIFO_TRIG_LEV);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->INT_EN_REG, PSRAM_CTRL->INT_EN_REG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->INT_STA_REG, PSRAM_CTRL->INT_STA_REG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->DEBUG_STA, PSRAM_CTRL->DEBUG_STA);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_FORCE_CFG, PSRAM_CTRL->PSRAM_FORCE_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_COM_CFG, PSRAM_CTRL->PSRAM_COM_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_LAT_CFG, PSRAM_CTRL->PSRAM_LAT_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_TIM_CFG, PSRAM_CTRL->PSRAM_TIM_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG, PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_ADDR[0].START_ADDR, PSRAM_CTRL->PSRAM_ADDR[0].START_ADDR);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_ADDR[0].END_ADDR, PSRAM_CTRL->PSRAM_ADDR[0].END_ADDR);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_ADDR[0].BIAS_ADDR, PSRAM_CTRL->PSRAM_ADDR[0].BIAS_ADDR);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_CLK_OUT_DLY_CFG, PSRAM_CTRL->PSRAM_CLK_OUT_DLY_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->PSRAM_MISC_CFG, PSRAM_CTRL->PSRAM_MISC_CFG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->C_W_BYTE_NUM, PSRAM_CTRL->C_W_BYTE_NUM);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->S_WDATA_REG, PSRAM_CTRL->S_WDATA_REG);
	PR_DBG("0x%08x:0x%08x\n", &PSRAM_CTRL->S_RDATA_REG, PSRAM_CTRL->S_RDATA_REG);
}

#ifndef CONFIG_PSRAM_200M
#define PSRAM_FREQ (1920000000)
#else
#define PSRAM_FREQ (2000000000)
#endif

static struct psram_chip chip = {0};

int platform_psram_chip_config(void)
{
	struct psram_ctrl *ctrl = NULL;
	PSRAMCtrl_InitParam cfg;
	PSRAMChip_InitParam psram_chip_cfg;
	uint32_t p_type;

	cfg.freq = PSRAM_FREQ;
	cfg.rdata_w = 1;

#ifdef CONFIG_PSRAM_CHIP_OPI_APS64
	p_type = PSRAM_CHIP_OPI_APS64;
#else
	p_type = PSRAM_CHIP_OPI_APS32;
#endif
	cfg.p_type = p_type;

	ctrl = HAL_PsramCtrl_Create(0, &cfg);
	if (!ctrl) {
		PR_ERR("psram create faile\n");
		return -1;
	}

	ctrl = HAL_PsramCtrl_Open(0);
	if (!ctrl) {
		PR_ERR("psram open faile\n");
			goto out2;
	}

	psram_chip_cfg.freq = PSRAM_FREQ;
	psram_chip_cfg.p_type = p_type;

	psram_chip_init(&chip, ctrl, &psram_chip_cfg);

	PR_DBG("%s:%d\n", __func__, __LINE__);

	return 0;

out2:
	HAL_PsramCtrl_Deinit(ctrl);
out3:
	HAL_PsramCtrl_Destory(ctrl);
	PR_ERR("psram chip init faild!\n");

	return -1;

}

int platform_psram_init(void)
{
	uint8_t p_type = 0;
	int i = 0;

	struct psram_ctrl *ctrl = NULL;
	PSRAMCtrl_InitParam cfg;
	PSRAMChip_InitParam psram_chip_cfg;

	cfg.freq = PSRAM_FREQ;
	cfg.rdata_w = 1;

	for (i = 0; i < 2; i++) {
		switch (i) {
		case 0:
			p_type = PSRAM_CHIP_OPI_APS64;
			break;
		case 1:
			p_type = PSRAM_CHIP_OPI_APS32;
			break;
		}

		cfg.p_type = p_type;

		PR_DBG("begin to creat struct and malloc\n");
		ctrl = HAL_PsramCtrl_Create(0, &cfg);
		if (!ctrl) {
			PR_ERR("psram create faile\n");
			return -1;
		}

		PR_DBG("HAL_PsramCtrl_Init\n");
		if (HAL_PsramCtrl_Init(ctrl, &cfg)) {
			PR_ERR("psram ctrl init faild!\n");
			goto out3;
		}

		PR_DBG("HAL_PsramCtrl_Open\n");
		ctrl = HAL_PsramCtrl_Open(0);
		if (!ctrl) {
			PR_ERR("psram open faile\n");
			goto out2;
		}

		psram_chip_cfg.freq = PSRAM_FREQ;
		psram_chip_cfg.p_type = p_type;

		PR_DBG("psram_init\n");
		if (psram_init(&chip, ctrl, &psram_chip_cfg)) {
			HAL_PsramCtrl_Close(ctrl);
			HAL_PsramCtrl_Deinit(ctrl);
			HAL_PsramCtrl_Destory(ctrl);
		} else {
			PR_DBG("psram chip %s init ok!, freq %d\n", chip.name, chip.freq);
			break;
		}
	}

	/*if (load_psram_bin_and_set_addr(&chip)) {
		PR_ERR("load psram bin faild!\n");
		goto out;
	} */
	PR_DBG("load psram bin ok\n");
	PR_DBG("Init psram controller ok\n");

	/* dump_psram_reg(); */

	//psram_boot0_stress_test();
	if (p_type == PSRAM_CHIP_OPI_APS64)
		return 8;
	else if (p_type == PSRAM_CHIP_OPI_APS32)
		return 4;
	else
		return 0;
//out:
	//psram_deinit(&chip);
//out1:
	//HAL_PsramCtrl_Close(ctrl);
out2:
	HAL_PsramCtrl_Deinit(ctrl);
out3:
	HAL_PsramCtrl_Destory(ctrl);
	PR_ERR("psram chip init faild!\n");

	return -1;
}
