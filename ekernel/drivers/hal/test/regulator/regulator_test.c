#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <unistd.h>

#include <sunxi_hal_regulator.h>
#include <sunxi_hal_common.h>
#include <hal_cmd.h>
#include <hal_log.h>

#define SOC_PRCM_BASE     		  (0x40050000)
#define SOC_PRCM_DCDC_CTRL0       (SOC_PRCM_BASE+0x0004)
#define SOC_PRCM_DCDC_CTRL1       (SOC_PRCM_BASE+0x0008)
#define SOC_PRCM_DCDC_LDO_MODE    (SOC_PRCM_BASE+0x000c)
#define SOC_PRCM_RTC_LDO_CTRL     (SOC_PRCM_BASE+0x0020)
#define SOC_PRCM_EXT_LDO_CTRL     (SOC_PRCM_BASE+0x0024)
#define SOC_PRCM_TOP_LDO_CTRL     (SOC_PRCM_BASE+0x0028)
#define SOC_PRCM_AON_LDO_CTRL     (SOC_PRCM_BASE+0x0040)
#define SOC_PRCM_APP_LDO_CTRL     (SOC_PRCM_BASE+0x0044)
#define SOC_PRCM_DSP_LDO_CTRL     (SOC_PRCM_BASE+0x004c)

//test : rpccli arm hal_regulator_set_current_able

static int hal_regulator_regs_check(void)
{
	printf("[regulator] %x:%x\n", SOC_PRCM_DCDC_CTRL0, readl(SOC_PRCM_DCDC_CTRL0));
	printf("[regulator] %x:%x\n", SOC_PRCM_DCDC_CTRL1, readl(SOC_PRCM_DCDC_CTRL1));
	printf("[regulator] %x:%x\n", SOC_PRCM_DCDC_LDO_MODE, readl(SOC_PRCM_DCDC_LDO_MODE));
	printf("[regulator] %x:%x\n", SOC_PRCM_RTC_LDO_CTRL, readl(SOC_PRCM_RTC_LDO_CTRL));
	printf("[regulator] %x:%x\n", SOC_PRCM_EXT_LDO_CTRL, readl(SOC_PRCM_EXT_LDO_CTRL));
	printf("[regulator] %x:%x\n", SOC_PRCM_TOP_LDO_CTRL, readl(SOC_PRCM_TOP_LDO_CTRL));
	printf("[regulator] %x:%x\n", SOC_PRCM_AON_LDO_CTRL, readl(SOC_PRCM_AON_LDO_CTRL));
	printf("[regulator] %x:%x\n", SOC_PRCM_APP_LDO_CTRL, readl(SOC_PRCM_APP_LDO_CTRL));
	printf("[regulator] %x:%x\n", SOC_PRCM_DSP_LDO_CTRL, readl(SOC_PRCM_DSP_LDO_CTRL));
	return 0;
}

static int cmd_set_able(int argc, const char **argv)
{
	int ret;
	int id = SOC_ID_MAX;
	unsigned int flag = 0;
	struct regulator_dev regulator;
	if (argv[1])
		id = (int)*argv[1] - 48;
	if (argv[2])
		flag = (int)*argv[2] - 48;
	printf("[regulator] set regulator_en[%d]:%x, flage:%d\n", id, REGULATOR_GET(SOC_REGULATOR, id), flag);
	hal_regulator_get(REGULATOR_GET(SOC_REGULATOR, id), &regulator);
	printf("[regulator] set_able regulator_en\n");
	hal_regulator_set_able(&regulator, flag);
	printf("[regulator] set_regulator[%d]able_status:%d\n", id, flag);
	hal_regulator_regs_check();
	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_set_able, hal_regulator_set_current_able, regulator hal APIs tests)

static int cmd_get_voltage_ext(int argc, const char **argv)
{
	int ret;
	int id = SOC_ID_MAX;
	unsigned int flag = 0;
	struct regulator_dev regulator;
	if (argv[1])
		id = (int)*argv[1] - 48;
	if (argv[2])
		flag = (int)*argv[2] - 48;
	printf("[regulator] get regulator:%x\n", REGULATOR_GET(SOC_REGULATOR, id));
	hal_regulator_get(REGULATOR_GET(SOC_REGULATOR, id), &regulator);
	printf("[regulator] cat check_dedc_status\n");
	hal_regulator_get_voltage_ext(&regulator, &ret, flag);
	printf("[regulator] get_regulator[%d]status[%d]voltage:%d\n", id, flag, ret);
	hal_regulator_regs_check();
	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_get_voltage_ext, hal_regulator_get_voltage, regulator hal APIs tests)

static int cmd_set_voltage_ext(int argc, const char **argv)
{
	int ret[4] = {0};
	int ret1;
	int id = SOC_ID_MAX;
	unsigned int flag = 0;
	struct regulator_dev regulator;
	if (argv[1])
		id = (int)*argv[1] - 48;
	if (argv[2])
		flag = (int)*argv[2] - 48;
	if (argv[3])
		ret[0] = ((int)*argv[3] - 48) * 1000;
	if (argv[4])
		ret[1] = ((int)*argv[4] - 48) * 100;
	if (argv[5])
		ret[2] = ((int)*argv[5] - 48) * 10;
	if (argv[6])
		ret[3] = ((int)*argv[6] - 48) * 1;
	ret1 = (ret[0] + ret[1] + ret[2] + ret[3]) * 1000;
	printf("[regulator] set regulator:%x\n", REGULATOR_GET(SOC_REGULATOR, id));
	hal_regulator_get(REGULATOR_GET(SOC_REGULATOR, id), &regulator);
	printf("[regulator] set_regulator[%d]status[%d]voltage:%d\n", id, flag, ret1);
	hal_regulator_set_voltage_ext(&regulator, ret1, flag);
	printf("[regulator] finishe-set_regulator[%d]status[%d]voltage:%d\n", id, flag, ret1);
	hal_regulator_regs_check();
	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_set_voltage_ext, hal_regulator_set_voltage, regulator hal APIs tests)
