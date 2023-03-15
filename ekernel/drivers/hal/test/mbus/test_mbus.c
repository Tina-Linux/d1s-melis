#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hal_cmd.h>
#include <sunxi_hal_mbus.h>


int cmd_mbus(int argc, char **argv)
{

	int cnt = 1;
	int ms_delay = 0;
	int windows_us;
	uint32_t cpu_value = 0, gpu_value = 0, ve_value = 0, disp_value = 0;
	uint32_t total_value = 0, di_value = 0, oth_value = 0, csi_value = 0;
	uint32_t tvd_value = 0, g2d_value = 0, iommu_value = 0, rv_value = 0;
	uint32_t dsp_value = 0, dma0_value = 0, dma1_value = 0, de_value = 0;
	uint32_t ce_value = 0, mahb_value = 0,rv_sys_value = 0;

	printf("============MBUS TEST===============\n");
	hal_mbus_pmu_enable();

	if (argc >= 2)
		cnt = atoi(argv[1]);

	if (argc >= 3)
		ms_delay = atoi(argv[2]);

	printf("the bus bandwidth occupancy status is :\n");
	while (cnt--) {
		hal_mbus_pmu_get_value(MBUS_PMU_CPU, &cpu_value);
		hal_mbus_pmu_get_value(MBUS_PMU_GPU, &gpu_value);
		hal_mbus_pmu_get_value(MBUS_PMU_RV_SYS, &rv_sys_value);
		hal_mbus_pmu_get_value(MBUS_PMU_VE, &ve_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DISP, &disp_value);
		hal_mbus_pmu_get_value(MBUS_PMU_OTH, &oth_value);
		hal_mbus_pmu_get_value(MBUS_PMU_CE, &ce_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DI, &di_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DE, &de_value);
		hal_mbus_pmu_get_value(MBUS_PMU_CSI, &csi_value);
		hal_mbus_pmu_get_value(MBUS_PMU_TVD, &tvd_value);
		hal_mbus_pmu_get_value(MBUS_PMU_G2D, &g2d_value);
		hal_mbus_pmu_get_value(MBUS_PMU_IOMMU, &iommu_value);
		hal_mbus_pmu_get_value(MBUS_PMU_RV_SYS, &rv_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DSP_SYS, &dsp_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DMA0, &dma0_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DMA1, &dma1_value);
		hal_mbus_pmu_get_value(MBUS_PMU_MAHB, &mahb_value);
		hal_mbus_pmu_get_value(MBUS_PMU_TOTAL, &total_value); //mbus calculate bw every window time, total is the max one
		hal_mbus_pmu_get_window(&windows_us);
		printf("window(us) maxbw(k) cpu      gpu      ve       disp     di       csi      tvd      g2d      iommu    rv       dsp      dma0     dma1     cd       de       mahb		others	rv_sys\n");
		printf("%-10d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d\n",\
				windows_us , total_value, cpu_value, gpu_value, ve_value, disp_value,\
				di_value, csi_value, tvd_value, g2d_value, iommu_value,\
				rv_value, dsp_value, dma0_value, dma1_value, ce_value,\
				de_value, mahb_value, oth_value,rv_sys_value);

		if (cnt && ms_delay)
			mdelay(ms_delay);
	}

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_mbus, mbus_test, Mbus hal APIs tests);

int cmd_mbus_enable(int argc, char **argv)
{
	hal_mbus_pmu_enable();

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_mbus_enable, mbus_enable, Mbus hal enable APIs tests);

int cmd_mbus_value(int argc, char **argv)
{

	int cnt = 1;
	int ms_delay = 0;
	int windows_us;
	uint32_t cpu_value = 0, gpu_value = 0, ve_value = 0, disp_value = 0;
	uint32_t total_value = 0, di_value = 0, oth_value = 0, csi_value = 0;
	uint32_t tvd_value = 0, g2d_value = 0, iommu_value = 0, rv_value = 0;
	uint32_t dsp_value = 0, dma0_value = 0, dma1_value = 0, de_value = 0;
	uint32_t ce_value = 0, mahb_value = 0,rv_sys_value = 0;

	if (argc >= 2)
		cnt = atoi(argv[1]);

	if (argc >= 3)
		ms_delay = atoi(argv[2]);

	printf("the bus bandwidth occupancy status is :\n");
	while (cnt--) {
		hal_mbus_pmu_get_value(MBUS_PMU_CPU, &cpu_value);
		hal_mbus_pmu_get_value(MBUS_PMU_GPU, &gpu_value);
		hal_mbus_pmu_get_value(MBUS_PMU_VE, &ve_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DISP, &disp_value);
		hal_mbus_pmu_get_value(MBUS_PMU_OTH, &oth_value);
		hal_mbus_pmu_get_value(MBUS_PMU_CE, &ce_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DI, &di_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DE, &de_value);
		hal_mbus_pmu_get_value(MBUS_PMU_CSI, &csi_value);
		hal_mbus_pmu_get_value(MBUS_PMU_TVD, &tvd_value);
		hal_mbus_pmu_get_value(MBUS_PMU_G2D, &g2d_value);
		hal_mbus_pmu_get_value(MBUS_PMU_IOMMU, &iommu_value);
		hal_mbus_pmu_get_value(MBUS_PMU_RV_SYS, &rv_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DSP_SYS, &dsp_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DMA0, &dma0_value);
		hal_mbus_pmu_get_value(MBUS_PMU_DMA1, &dma1_value);
		hal_mbus_pmu_get_value(MBUS_PMU_MAHB, &mahb_value);
		hal_mbus_pmu_get_value(MBUS_PMU_TOTAL, &total_value); //mbus calculate bw every window time, total is the max one
		hal_mbus_pmu_get_window(&windows_us);
		printf("window(us) maxbw(k) cpu      gpu      ve       disp     di       csi      tvd      g2d      iommu    rv       dsp      dma0     dma1     cd       de       mahb		others \n");
		printf("%-10d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d %-8d   0x%08x  %-8d %-8d %-8d %-8d %-8d %-8d %-8d \n",\
				windows_us , total_value, cpu_value, gpu_value, ve_value, disp_value,\
				di_value, csi_value, tvd_value, g2d_value, iommu_value,\
				rv_value, dsp_value, dma0_value, dma1_value, ce_value,\
				de_value, mahb_value, oth_value);

		if (cnt && ms_delay)
			mdelay(ms_delay);
	}

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_mbus_value, mbus_test_value, Mbus hal value APIs tests);
