#include <stdio.h>
#include <string.h>
#include <hal_cmd.h>
#include <osal/hal_time.h>
#include <hal_clk.h>
#include <sunxi_hal_rcosc_cali.h>

static void dump_rccal_reg(void)
{
	printf("RCCAL module related registers:\n");
	printf("CNT_TARGET: 0x%08x\n", RCOCALI_CTRL->CNT_TARGET);
	printf("DIVIDEND: 0x%08x\n", RCOCALI_CTRL->DIVIDEND);
	printf("DCXO_CNT: 0x%08x\n", RCOCALI_CTRL->DCXO_CNT);
	printf("ABN_MONITOR: 0x%08x\n", RCOCALI_CTRL->ABNORMAL_MONITOR);
	printf("INTERRUPT: 0x%08x\n", RCOCALI_CTRL->INTERRUPT);
	printf("BLE_RCOSC_CALIB_CTRL0: 0x%08x\n", PRCM->BLE_RCOSC_CALIB_CTRL0);
	printf("BLE_RCOSC_CALIB_CTRL1: 0x%08x\n", PRCM->BLE_RCOSC_CALIB_CTRL1);
}

//#define RCCAL_TEST_DEBUG
#define MAX_WAIT_CAL_TIMES 100
#define DETECT_RC_LF_CLK_FREQ_TIMES 5
#define MAX_RC_LF_FREQ 1500000
#define MIN_RC_LF_FREQ 1000000
int cmd_test_rccal(int argc, char **argv)
{
	printf("RCCAL module test\n");
	RCOCALI_InitParam cal_para;
	cal_para.cnt_n = 8192;
	cal_para.out_clk = 32000;

#ifdef RCCAL_TEST_DEBUG
	printf("before init\n");
	dump_rccal_reg();
#endif

	HAL_RcoscCali_Init(&cal_para);

#ifdef RCCAL_TEST_DEBUG
	printf("after init\n");
	dump_rccal_reg();
#endif

	RCOCALI_ConfigParam period_cal_para;
	memset(&period_cal_para, 0, sizeof(period_cal_para));
	period_cal_para.mode = PRCM_RCOSC_WK_MODE_SEL_NORMAL;
	period_cal_para.phase2_times = PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_2;
	period_cal_para.phase3_times = PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_10;
	period_cal_para.wup_time = 160;
	HAL_RcoscCali_Config(&period_cal_para);

#ifdef RCCAL_TEST_DEBUG
	printf("after configure period calibration parameter\n");
	dump_rccal_reg();
#endif

	HAL_RcoscCali_Start();

#ifdef RCCAL_TEST_DEBUG
	printf("after start calibration\n");
	dump_rccal_reg();
#endif

	int i = 0;
	uint32_t dcxo_cnt = 0;
	for (i = 0; i < MAX_WAIT_CAL_TIMES; i++)
	{
		dcxo_cnt = HAL_GET_BIT(RCOCALI_CTRL->DCXO_CNT, RCOCALI_DCXO_CNT_MASK);
		if (dcxo_cnt)
		{
			break;
		}
		hal_msleep(10);
	}

	int detect_cnt = DETECT_RC_LF_CLK_FREQ_TIMES;
	int is_test_pass = 1;
	uint32_t rc_lf_freq_arr[DETECT_RC_LF_CLK_FREQ_TIMES];

	memset(rc_lf_freq_arr, 0, sizeof(rc_lf_freq_arr));
	for (i = 0; i < detect_cnt; i++)
	{
		hal_status_t ret = hal_get_rc_lf_freq(&rc_lf_freq_arr[i]);
		if (ret != HAL_OK)
		{
			printf("hal_get_rc_lf_freq failed, ret: %d\n", ret);
			is_test_pass = 0;
			break;
		}

		printf("RC_LF frequency: %uHz\n", rc_lf_freq_arr[i]);
		if ((rc_lf_freq_arr[i] < MIN_RC_LF_FREQ) || (rc_lf_freq_arr[i] > MAX_RC_LF_FREQ))
		{
			is_test_pass = 0;
			break;
		}
		hal_msleep(200);
	}

	if (is_test_pass)
	{
		printf("RCCAL module test success!\n");
	}
	else
	{
		printf("RCCAL module test failed!\n");
	}

	HAL_RcoscCali_DeInit();
	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_rccal, test_rccal, test whether the RCCAL module work normally(it will stop RCCAL module when test complete))

int cmd_dump_rccal_reg(int argc, char **argv)
{
	dump_rccal_reg();
	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_dump_rccal_reg, dump_rccal, dump the related registers of RCCAL module)

#include <timer/platform_timer.h>

#define TIMER_BASE SUNXI_TMR_PBASE

#define TIMER_IRQ_CTRL_OFFSET 0x00

//bit7, mode, bit6:4, pre_div, bit3-bit2, clk source, bit1, reload, bit0, enable
#define TIMER_TIMER0_CTRL_OFFSET 0x10
#define TIMER_TIMER0_INTERVAL_OFFSET 0x14
#define TIMER_TIMER0_CNT_OFFSET 0x18

#define TIMER_TIMER1_CTRL_OFFSET 0x20
#define TIMER_TIMER1_INTERVAL_OFFSET 0x24
#define TIMER_TIMER1_CNT_OFFSET 0x28

#define TIMER_RELOAD_VALUE 0xFFFFFFFF

static uint64_t s_current_ticks[2];
static uint32_t s_last_cnt[2];
static uint32_t s_timer_clk_freq[2];

static void dump_timer_reg(void)
{
	printf("Timer module related registers:\n");
	printf("IRQ_CTRL: 0x%08x\n", readl(TIMER_BASE + TIMER_IRQ_CTRL_OFFSET));
	printf("TIMER0_CTRL: 0x%08x\n", readl(TIMER_BASE + TIMER_TIMER0_CTRL_OFFSET));
	printf("TIMER0_INTERVAL: 0x%08x\n", readl(TIMER_BASE + TIMER_TIMER0_INTERVAL_OFFSET));
	printf("TIMER0_CNT: 0x%08x\n", readl(TIMER_BASE + TIMER_TIMER0_CNT_OFFSET));
	printf("TIMER1_CTRL: 0x%08x\n", readl(TIMER_BASE + TIMER_TIMER1_CTRL_OFFSET));
	printf("TIMER1_INTERVAL: 0x%08x\n", readl(TIMER_BASE + TIMER_TIMER1_INTERVAL_OFFSET));
	printf("TIMER1_CNT: 0x%08x\n", readl(TIMER_BASE + TIMER_TIMER1_CNT_OFFSET));
}

static void init_timer0(void)
{
	uint32_t reg_data, reg_addr;
	//disable int
	reg_addr = TIMER_BASE + TIMER_IRQ_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data &= ~(1 << 0);
	writel(reg_data, reg_addr);

	//disable timer
	reg_addr = TIMER_BASE + TIMER_TIMER0_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data &= ~(1 << 0);
	writel(reg_data, reg_addr);

	//set interval
	reg_addr = TIMER_BASE + TIMER_TIMER0_INTERVAL_OFFSET;
	reg_data = TIMER_RELOAD_VALUE;
	writel(reg_data, reg_addr);

	//continous mode, HOSC clock, 128 div
	reg_addr = TIMER_BASE + TIMER_TIMER0_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data &= ~((1 << 7) | (0x7 << 4) | (0x3 << 2));
	reg_data |= (0x7 << 4) | (1 << 2);
	writel(reg_data, reg_addr);

	//reload and enable
	/*
	reg_addr = TIMER_BASE + TIMER_TIMER0_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data |= (1 << 1) | (1 << 0);
	writel(reg_data, reg_addr);
	*/

	s_timer_clk_freq[0] = HAL_GetHFClock() / 128;
	printf("timer 0 input clock(HOSC): %uHz\n", s_timer_clk_freq[0]);
}

static void init_timer1(void)
{
	uint32_t reg_data, reg_addr;
	//disable int
	reg_addr = TIMER_BASE + TIMER_IRQ_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data &= ~(1 << 1);
	writel(reg_data, reg_addr);

	//disable timer
	reg_addr = TIMER_BASE + TIMER_TIMER1_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data &= ~(1 << 0);
	writel(reg_data, reg_addr);

	//set interval
	reg_addr = TIMER_BASE + TIMER_TIMER1_INTERVAL_OFFSET;
	reg_data = TIMER_RELOAD_VALUE;
	writel(reg_data, reg_addr);

	//continous mode, SYS_32K clock, 1 div
	reg_addr = TIMER_BASE + TIMER_TIMER1_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data &= ~((1 << 7) | (0x7 << 4) | (0x3 << 2));
	//reg_data |= (0x7 << 4) | (1 << 2);
	writel(reg_data, reg_addr);

	//reload and enable
	/*
	reg_addr = TIMER_BASE + TIMER_TIMER1_CTRL_OFFSET;
	reg_data = readl(reg_addr);
	reg_data |= (1 << 1) | (1 << 0);
	writel(reg_data, reg_addr);
	*/
	hal_status_t ret = hal_get_rccal_output_freq(&s_timer_clk_freq[1]);
	if (ret != HAL_OK)
	{
		printf("warning: get_rccal_output_freq failed, ret: %d\n", ret);
		printf("default set rccal clk freq to 32KHz\n");
		s_timer_clk_freq[1] = 32000;
	}

	printf("timer 1 input clock(SYS_32K): %uHz\n", s_timer_clk_freq[1]);
}

static inline void start_timer(uint32_t ctrl_reg_offset)
{
	uint32_t reg_data, reg_addr;
	//reload and enable
	reg_addr = TIMER_BASE + ctrl_reg_offset;
	reg_data = readl(reg_addr);
	reg_data |= (1 << 1) | (1 << 0);
	writel(reg_data, reg_addr);
}

static inline void start_timer0(void)
{
	start_timer(TIMER_TIMER0_CTRL_OFFSET);
}

static inline void start_timer1(void)
{
	start_timer(TIMER_TIMER1_CTRL_OFFSET);
}

static inline void stop_timer(uint32_t ctrl_reg_offset)
{
	uint32_t reg_data, reg_addr;
	//disable timer
	reg_addr = TIMER_BASE + ctrl_reg_offset;
	reg_data = readl(reg_addr);
	reg_data &= ~(1 << 0);
	writel(reg_data, reg_addr);
}

static inline void stop_timer0(void)
{
	stop_timer(TIMER_TIMER0_CTRL_OFFSET);
}

static inline void stop_timer1(void)
{
	stop_timer(TIMER_TIMER1_CTRL_OFFSET);
}

static inline uint32_t read_timer(uint32_t cnt_reg_offset)
{
	return TIMER_RELOAD_VALUE - readl(TIMER_BASE + cnt_reg_offset);
}

static inline uint64_t get_timer_ticks(uint32_t timer_id)
{
	uint32_t now;

	if (timer_id == 0)
	{
		now = read_timer(TIMER_TIMER0_CNT_OFFSET);
	}
	else if (timer_id == 1)
	{
		now = read_timer(TIMER_TIMER1_CNT_OFFSET);
	}
	else
	{
		printf("warning: invalid timer_id(%u)\n", timer_id);
		return 0;
	}

	if (now >= s_last_cnt[timer_id])
		s_current_ticks[timer_id] += (now - s_last_cnt[timer_id]);
	else
		s_current_ticks[timer_id] += (TIMER_RELOAD_VALUE - s_last_cnt[timer_id]) + now;

	s_last_cnt[timer_id] = now;

	return s_current_ticks[timer_id];
}

static uint64_t get_timer0_ticks(void)
{
	return get_timer_ticks(0);
}

static uint64_t get_timer1_ticks(void)
{
	return get_timer_ticks(1);
}

#define TS_PRECISION_100_US

#ifdef TS_PRECISION_100_US
#define TEST_TIME_FACTOR 10000
#else
#define TEST_TIME_FACTOR 1000
#endif

#include <errno.h>

static void print_rccal_precision_cmd_usage(void)
{
	printf("Usage: test_rccal_precision <test_time>\n");
	printf("       the unit of parameter 'test_time' is second\n");
}

int cmd_test_rccal_precision(int argc, const char *argv[])
{
	char *ptr = NULL;
	errno = 0;

	if (argc != 2)
	{
		printf("invalid input parameter num(%d)!\n", argc);
		print_rccal_precision_cmd_usage();
		return 0;
	}
	unsigned long test_time = strtoul(argv[1], &ptr, 10);
	if (errno || (ptr && *ptr != '\0'))
	{
		printf("invalid input parameter('%s')!\n", argv[1]);
		print_rccal_precision_cmd_usage();
		return 0;
	}

	printf("test time: %lus\n", test_time);
	test_time *= TEST_TIME_FACTOR;

	init_timer0();
	init_timer1();
	uint64_t timer0_ticks = 0, timer1_ticks = 0;
	uint64_t timer0_ts = 0, timer1_ts = 0, timer_ts_diff = 0;

	s_last_cnt[0] = 0;
	s_last_cnt[1] = 0;
	s_current_ticks[0] = 0;
	s_current_ticks[1] = 0;

	//dump_timer_reg();
	start_timer0();
	start_timer1();
	//wait timer reload
	udelay(10*1000);
	//dump_timer_reg();
	while (1)
	{
		timer0_ticks = get_timer0_ticks();
		timer1_ticks = get_timer1_ticks();
#ifdef TS_PRECISION_100_US
		timer0_ts = timer0_ticks * 10000 / s_timer_clk_freq[0];
		timer1_ts = timer1_ticks * 10000 / s_timer_clk_freq[1];
		printf("timer0 ts: %lu.%lums, ticks: %lu\n", timer0_ts/10, timer0_ts%10, timer0_ticks);
		printf("timer1 ts: %lu.%lums, ticks: %lu\n", timer1_ts/10, timer1_ts%10, timer1_ticks);
#else
		timer0_ts = timer0_ticks * 1000 / s_timer_clk_freq[0];
		timer1_ts = timer1_ticks * 1000 / s_timer_clk_freq[1];
		printf("timer0 ts: %lums, ticks: %lu\n", timer0_ts, timer0_ticks);
		printf("timer1 ts: %lums, ticks: %lu\n", timer1_ts, timer1_ticks);
#endif
		if (timer0_ts > timer1_ts)
		{
			timer_ts_diff = timer0_ts - timer1_ts;
		}
		else
		{
			timer_ts_diff = timer1_ts - timer0_ts;
		}
#ifdef TS_PRECISION_100_US
		printf("timer ts diff: %lu.%lums\n\n", timer_ts_diff/10, timer_ts_diff%10);
#else
		printf("timer ts diff: %lums\n\n", timer_ts_diff);
#endif
		udelay(500*1000);

		if (timer0_ts > test_time)
		{
			dump_timer_reg();
			printf("\n");
			break;
		}
	}

	stop_timer0();
	stop_timer1();

	printf("RCCAL precision test complete\n");
	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_rccal_precision, test_rccal_precision, test the precision of RCCAL output clock);
