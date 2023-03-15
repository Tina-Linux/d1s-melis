#include <stdlib.h>
#include <interrupt.h>
#include <hal_atomic.h>
#include <hal_gpio.h>
#include <hal_dma.h>
#include <hal_cache.h>
#include <sunxi_hal_ledc.h>
#ifdef CONFIG_COMPONENTS_PM
#include <pm_devops.h>
#endif

/* define this macro when debugging is required */
/* #define LEDC_DEBUG */
#ifdef LEDC_DEBUG
#define ledc_info(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)
#else
#define ledc_info(fmt, args...)
#endif

#define led_err(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)

#define LEDC_PIN_SLEEP 0

struct ledc_config ledc_config = {
	.led_count = 3,
	.reset_ns = 84,
	.t1h_ns = 800,
	.t1l_ns = 450,
	.t0h_ns = 400,
	.t0l_ns = 850,
	.wait_time0_ns = 84,
	.wait_time1_ns = 84,
	.wait_data_time_ns = 600000,
	.output_mode = "GRB",
};

static unsigned long base_addr = LEDC_BASE;
struct sunxi_dma_chan *dma_chan;
struct sunxi_led *led;

static hal_irqreturn_t sunxi_ledc_irq_handler(void *dummy)
{
	ledc_info("=======enter irq_handler=====\n");
	struct sunxi_led *led = (struct sunxi_led *)dummy;
	unsigned int irq_status;

	irq_status = hal_ledc_get_irq_status();
	hal_ledc_clear_all_irq();

	if (irq_status & LEDC_TRANS_FINISH_INT)
		led->result = RESULT_COMPLETE;

	if (irq_status & LEDC_WAITDATA_TIMEOUT_INT)
		led->result = RESULT_ERR;

	if (irq_status & LEDC_FIFO_OVERFLOW_INT)
		led->result = RESULT_ERR;

	led->config.length = 0;
	hal_ledc_reset();

	return HAL_IRQ_OK;
}

int sunxi_led_get_config(struct ledc_config *config)
{
	*config = ledc_config;
	return 0;
}

static int ledc_clk_init(void)
{
	hal_clk_type_t	clk_type = HAL_SUNXI_CCU;
	hal_clk_id_t	mod_clk_id = CLK_LEDC;
	hal_clk_id_t	bus_clk_id = CLK_BUS_LEDC;
	hal_reset_type_t reset_type = HAL_SUNXI_RESET;
#ifdef CONFIG_ARCH_SUN20IW2
	hal_reset_id_t	reset_id = RST_LEDC;
#else
	hal_reset_id_t	reset_id = RST_BUS_LEDC;
#endif

#ifdef CONFIG_ARCH_SUN8IW18P1
	hal_clock_enable(CLK_BUS_LEDC);
#else
	led->reset = hal_reset_control_get(reset_type, reset_id);
	if (hal_reset_control_deassert(led->reset))
	{
		ledc_info("ledc reset deassert  failed!");
		return -1;
	}
	hal_reset_control_put(led->reset);

	led->mod_clk = hal_clock_get(clk_type, mod_clk_id);
	if (hal_clock_enable(led->mod_clk))
	{
		ledc_info("ledc clk enable mclk failed!");
		return -1;
	}

	led->bus_clk = hal_clock_get(clk_type, bus_clk_id);
	if (hal_clock_enable(led->bus_clk))
	{
		ledc_info("ledc clk enable mclk failed!");
		return -1;
	}
#endif

	return 0;
}

static void ledc_clk_exit(void)
{
#ifdef CONFIG_ARCH_SUN8IW18P1
	hal_clock_disable(CLK_BUS_LEDC);
#else
	hal_clock_disable(led->bus_clk);
	hal_clock_disable(led->bus_clk);
	hal_reset_control_assert(led->reset);
#endif
}

static int ledc_pinctrl_init(void)
{
	if (hal_gpio_pinmux_set_function(LEDC_PIN, LEDC_PINMUXSEL))
        {
            ledc_info("ledc pin set default function failed!");
            return -1;
        }

	return 0;
}

static void ledc_pinctrl_exit(void)
{
	hal_gpio_pinmux_set_function(LEDC_PIN, LEDC_PIN_SLEEP);
}

static void ledc_dump_reg(void)
{
	ledc_info("LEDC_CTRL_REG = %0x\n", hal_readl(base_addr + LEDC_CTRL_REG));
	ledc_info("LED_T01_TIMING_CTRL_REG = %0x\n", hal_readl(base_addr + LED_T01_TIMING_CTRL_REG));
	ledc_info("LEDC_DATA_FINISH_CNT_REG = %0x\n", hal_readl(base_addr + LEDC_DATA_FINISH_CNT_REG));
	ledc_info("LED_RST_TIMING_CTRL_REG = %0x\n", hal_readl(base_addr + LED_RST_TIMING_CTRL_REG));
	ledc_info("LEDC_WAIT_TIME0_CTRL_REG = %0x\n", hal_readl(base_addr + LEDC_WAIT_TIME0_CTRL_REG));
	ledc_info("LEDC_DATA_REG = %0x\n", hal_readl(base_addr + LEDC_DATA_REG));
	ledc_info("LEDC_DMA_CTRL_REG = %0x\n",	hal_readl(base_addr + LEDC_DMA_CTRL_REG));
	ledc_info("LEDC_INTC_REG = %0x\n", hal_readl(base_addr + LEDC_INTC_REG));
	ledc_info("LEDC_INTS_REG = %0x\n", hal_readl(base_addr + LEDC_INTS_REG));
	ledc_info("LEDC_WAIT_TIME1_CTRL_REG = %0x\n", hal_readl(base_addr + LEDC_WAIT_TIME1_CTRL_REG));
	ledc_info("LEDC_FIFO_DATA0_REG = %0x\n", hal_readl(base_addr + LEDC_FIFO_DATA0_REG));
}

static void ledc_set_reset_ns(unsigned int reset_ns)
{
	unsigned int n, reg_val;
	unsigned int mask = 0x1FFF;
	unsigned int min = LEDC_RESET_TIME_MIN_NS;
	unsigned int max = LEDC_RESET_TIME_MAX_NS;

	if (reset_ns < min || reset_ns > max) {
		ledc_info("invalid parameter, reset_ns should be %u-%u!\n", min, max);
		return;
	}

	n = (reset_ns - 42) / 42;
	reg_val = hal_readl(base_addr + LED_RST_TIMING_CTRL_REG);
	reg_val &= ~(mask << 16);
	reg_val |= (n << 16);
	hal_writel(reg_val, base_addr + LED_RST_TIMING_CTRL_REG);
}

static void ledc_set_t1h_ns(unsigned int t1h_ns)
{
	unsigned int n, reg_val;
	unsigned int mask = 0x3F;
	unsigned int shift = 21;
	unsigned int min = LEDC_T1H_MIN_NS;
	unsigned int max = LEDC_T1H_MAX_NS;

	if (t1h_ns < min || t1h_ns > max) {
		ledc_info("invalid parameter, t1h_ns should be %u-%u!\n", min, max);
		return;
	}

	n = (t1h_ns - 42) / 42;
	reg_val = hal_readl(base_addr + LED_T01_TIMING_CTRL_REG);
	reg_val &= ~(mask << shift);
	reg_val |= n << shift;
	hal_writel(reg_val, base_addr + LED_T01_TIMING_CTRL_REG);
}

static void ledc_set_t1l_ns(unsigned int t1l_ns)
{
	unsigned int n, reg_val;
	unsigned int mask = 0x1F;
	unsigned int shift = 16;
	unsigned int min = LEDC_T1L_MIN_NS;
	unsigned int max = LEDC_T1L_MAX_NS;

	if (t1l_ns < min || t1l_ns > max) {
		ledc_info("invalid parameter, t1l_ns should be %u-%u!\n", min, max);
		return;
	}

	n = (t1l_ns - 42) / 42;
	reg_val = hal_readl(base_addr + LED_T01_TIMING_CTRL_REG);
	reg_val &= ~(mask << shift);
	reg_val |= n << shift;
	hal_writel(reg_val, base_addr + LED_T01_TIMING_CTRL_REG);
}

static void ledc_set_t0h_ns(unsigned int t0h_ns)
{
	unsigned int n, reg_val;
	unsigned int mask = 0x1F;
	unsigned int shift = 6;
	unsigned int min = LEDC_T0H_MIN_NS;
	unsigned int max = LEDC_T0H_MAX_NS;

	if (t0h_ns < min || t0h_ns > max) {
		ledc_info("invalid parameter, t0h_ns should be %u-%u!\n", min, max);
		return;
	}

	n = (t0h_ns - 42) / 42;
	reg_val = hal_readl(base_addr + LED_T01_TIMING_CTRL_REG);
	reg_val &= ~(mask << shift);
	reg_val |= n << shift;
	hal_writel(reg_val, base_addr + LED_T01_TIMING_CTRL_REG);
}

static void ledc_set_t0l_ns(unsigned int t0l_ns)
{
	unsigned int n, reg_val;
	unsigned int min = LEDC_T0L_MIN_NS;
	unsigned int max = LEDC_T0L_MAX_NS;

	if (t0l_ns < min || t0l_ns > max) {
		ledc_info("invalid parameter, t0l_ns should be %u-%u!\n", min, max);
		return;
	}

	n = (t0l_ns - 42) / 42;
	reg_val = hal_readl(base_addr + LED_T01_TIMING_CTRL_REG);
	reg_val &= ~0x3F;
	reg_val |= n;
	hal_writel(reg_val, base_addr + LED_T01_TIMING_CTRL_REG);
}

static void ledc_set_wait_time0_ns(unsigned int wait_time0_ns)
{
	unsigned int n, reg_val;
	unsigned int min = LEDC_WAIT_TIME0_MIN_NS;
	unsigned int max = LEDC_WAIT_TIME0_MAX_NS;

	if (wait_time0_ns < min || wait_time0_ns > max) {
		ledc_info("invalid parameter, wait_time0_ns should be %u-%u!\n", min, max);
		return;
	}

	n = (wait_time0_ns - 42) / 42;
	reg_val = (1 << 8) | n;
	hal_writel(reg_val, base_addr + LEDC_WAIT_TIME0_CTRL_REG);
}

static void ledc_set_wait_time1_ns(unsigned long long wait_time1_ns)
{
	//unsigned long tmp;
	unsigned long long max = LEDC_WAIT_TIME1_MAX_NS;
	unsigned int min = LEDC_WAIT_TIME1_MIN_NS;
	unsigned int n, reg_val;

	if (wait_time1_ns < min || wait_time1_ns > max) {
		ledc_info("invalid parameter, wait_time1_ns should be %u-%llu!\n", min, max);
		return;
	}

	n = wait_time1_ns / 42;
	//tmp = wait_time1_ns;
	//n = div_u64(tmp, 42);
	n -= 1;
	reg_val = (1 << 31) | n;
	hal_writel(reg_val, base_addr + LEDC_WAIT_TIME1_CTRL_REG);
}

static void ledc_set_wait_data_time_ns(unsigned int wait_data_time_ns)
{
	unsigned int mask = 0x1FFF;
	unsigned int shift = 16;
	unsigned int reg_val = 0;
	unsigned int n, min, max;

	min = LEDC_WAIT_DATA_TIME_MIN_NS;
	max = LEDC_WAIT_DATA_TIME_MAX_NS_IC;

	if (wait_data_time_ns < min || wait_data_time_ns > max) {
		ledc_info("invalid parameter, wait_data_time_ns should be %u-%u!\n",
				min, max);
		return;
	}

	n = (wait_data_time_ns - 42) / 42;
	reg_val &= ~(mask << shift);
	reg_val |= (n << shift);
	hal_writel(reg_val, base_addr + LEDC_DATA_FINISH_CNT_REG);
}

static void ledc_set_length(unsigned int length)
{
	unsigned int reg_val;

	if (length == 0)
		return;

	reg_val = hal_readl(base_addr + LEDC_CTRL_REG);
	reg_val &= ~(0x1FFF << 16);
	reg_val |=  length << 16;
	hal_writel(reg_val, base_addr + LEDC_CTRL_REG);

	reg_val = hal_readl(base_addr + LED_RST_TIMING_CTRL_REG);
	reg_val &= ~0x3FF;
	reg_val |= length - 1;
	hal_writel(reg_val, base_addr + LED_RST_TIMING_CTRL_REG);
}

static void ledc_set_output_mode(const char *str)
{
	unsigned int val = 0;
	unsigned int mask = 0x7;
	unsigned int shift = 6;
	unsigned int reg_val ;
	if (str != NULL) {
		if (!strncmp(str, "GRB", 3))
			val = LEDC_OUTPUT_GRB;
		else if (!strncmp(str, "GBR", 3))
			val = LEDC_OUTPUT_GBR;
		else if (!strncmp(str, "RGB", 3))
			val = LEDC_OUTPUT_RGB;
		else if (!strncmp(str, "RBG", 3))
			val = LEDC_OUTPUT_RBG;
		else if (!strncmp(str, "BGR", 3))
			val = LEDC_OUTPUT_BGR;
		else if (!strncmp(str, "BRG", 3))
			val = LEDC_OUTPUT_BRG;
		else
			return;
	} else {
	}

	reg_val = hal_readl(base_addr + LEDC_CTRL_REG);
	reg_val &= ~(mask << shift);
	reg_val |= val;
	hal_writel(reg_val, base_addr + LEDC_CTRL_REG);
}

static void ledc_disable_irq(unsigned int mask)
{
	unsigned int reg_val = 0;

	reg_val = hal_readl(base_addr + LEDC_INTC_REG);
	reg_val &= ~mask;
	hal_writel(reg_val, base_addr + LEDC_INTC_REG);
}

static void ledc_enable_irq(unsigned int mask)
{
	unsigned int reg_val = 0;

	reg_val = hal_readl(base_addr + LEDC_INTC_REG);
	reg_val |= mask;
	hal_writel(reg_val, base_addr + LEDC_INTC_REG);
}

static void ledc_set_dma_mode(void)
{
	unsigned int reg_val = 0;

	reg_val |= 1 << 5;
	hal_writel(reg_val, base_addr + LEDC_DMA_CTRL_REG);
}

static void ledc_set_cpu_mode(void)
{
	unsigned int reg_val = 0;

	reg_val &= ~(1 << 5);
	hal_writel(reg_val, base_addr + LEDC_DMA_CTRL_REG);
}

static void ledc_clear_all_irq(void)
{
	unsigned int reg_val;

	reg_val = hal_readl(base_addr + LEDC_INTS_REG);
	reg_val |= 0x1F;
	hal_writel(reg_val, base_addr + LEDC_INTS_REG);
}

static unsigned int ledc_get_irq_status(void)
{
	return hal_readl(base_addr + LEDC_INTS_REG);
}

static void ledc_soft_reset(void)
{
	unsigned int reg_val;

	reg_val = hal_readl(base_addr + LEDC_CTRL_REG);
	reg_val |= 1 << 1;
	hal_writel(reg_val, base_addr + LEDC_CTRL_REG);
}

static void ledc_reset_en(void)
{
	unsigned int reg_val;

	reg_val = hal_readl(base_addr + LEDC_CTRL_REG);
	reg_val |= 1 << 10;
	hal_writel(reg_val, base_addr + LEDC_CTRL_REG);
}

static void ledc_set_data(unsigned int data)
{
	hal_writel(data, base_addr + LEDC_DATA_REG);
}

static void ledc_enable(void)
{
	unsigned int reg_val;

	reg_val = hal_readl(base_addr + LEDC_CTRL_REG);
	reg_val |= 1;
	hal_writel(reg_val, base_addr + LEDC_CTRL_REG);
}

static void  hal_ledc_set_time(struct ledc_config *ledc)
{
	ledc_set_reset_ns(ledc->reset_ns);
	ledc_set_t1h_ns(ledc->t1h_ns);
	ledc_set_t1l_ns(ledc->t1l_ns);
	ledc_set_t0h_ns(ledc->t0h_ns);
	ledc_set_t0l_ns(ledc->t0l_ns);
	ledc_set_wait_time0_ns(ledc->wait_time0_ns);
	ledc_set_wait_time1_ns(ledc->wait_time1_ns);
	ledc_set_wait_data_time_ns(ledc->wait_data_time_ns);
}

void hal_ledc_dma_callback(void *para)
{
	printf("dma callback\n");
}

void hal_ledc_trans_data(struct ledc_config *ledc)
{
	int i;
	unsigned long int size;
	unsigned int mask = 0;
	struct dma_slave_config slave_config;

	mask = LEDC_TRANS_FINISH_INT_EN | LEDC_WAITDATA_TIMEOUT_INT_EN
		| LEDC_FIFO_OVERFLOW_INT_EN | LEDC_GLOBAL_INT_EN;
	if (ledc->length <= SUNXI_LEDC_FIFO_DEPTH) {
		ledc_info("trans data by CPU mode\n");
		mask |= LEDC_FIFO_CPUREQ_INT_EN;
		ledc_reset_en();
		hal_ledc_set_time(ledc);
		ledc_set_output_mode(ledc->output_mode);
		ledc_set_cpu_mode();
		ledc_set_length(ledc->length);
		ledc_enable_irq(mask);

		for(i = 0; i < ledc->length; i++)
			ledc_set_data(ledc->data[i]);

		ledc_enable();
	} else {
		ledc_info("trans data by DMA mode\n");
		mask &= ~LEDC_FIFO_CPUREQ_INT_EN;

		ledc_reset_en();
		size = ledc->length * 4;

		hal_dcache_clean((unsigned long)ledc->data, sizeof(ledc->data));

		slave_config.direction = DMA_MEM_TO_DEV;
		slave_config.src_addr = (unsigned long)(ledc->data);
		slave_config.dst_addr = (uint32_t)(base_addr + LEDC_DATA_REG);
		slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		slave_config.src_maxburst = DMA_SLAVE_BURST_16;
		slave_config.dst_maxburst = DMA_SLAVE_BURST_16;
		slave_config.slave_id = sunxi_slave_id(DRQDST_LEDC, DRQSRC_SDRAM);
		hal_dma_slave_config(dma_chan, &slave_config);

		hal_dma_prep_device(dma_chan, slave_config.dst_addr, slave_config.src_addr, size, DMA_MEM_TO_DEV);

		//dma_chan->callback = ledc_dma_callback;
		hal_dma_start(dma_chan);

		hal_ledc_set_time(ledc);
		ledc_set_output_mode(ledc->output_mode);
		ledc_set_length(ledc->length);
		ledc_set_dma_mode();
		ledc_enable_irq(mask);
		ledc_enable();
	}
}

void hal_ledc_clear_all_irq(void)
{
	ledc_clear_all_irq();
}

unsigned int hal_ledc_get_irq_status(void)
{
	return ledc_get_irq_status();
}

void hal_ledc_reset(void)
{
	ledc_disable_irq(LEDC_TRANS_FINISH_INT_EN | LEDC_WAITDATA_TIMEOUT_INT_EN
			| LEDC_FIFO_OVERFLOW_INT_EN | LEDC_GLOBAL_INT_EN | LEDC_GLOBAL_INT_EN);

	if (dma_chan)
	{
		hal_dma_stop(dma_chan);
	}
	ledc_soft_reset();
}

#ifdef CONFIG_COMPONENTS_PM
static inline void sunxi_ledc_save_regs(struct sunxi_led *led)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_ledc_regs_offset); i++)
		led->regs_backup[i] = readl(base_addr + sunxi_ledc_regs_offset[i]);
}

static inline void sunxi_ledc_restore_regs(struct sunxi_led *led)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_ledc_regs_offset); i++)
		writel(led->regs_backup[i], base_addr + sunxi_ledc_regs_offset[i]);
}

static int hal_ledc_resume(struct pm_device *dev, suspend_mode_t mode)
{
	int ret = 0;

	if (ledc_clk_init())
	{
		led_err("ledc clk init failed \n");
		ret = -1;
		goto err0;
	}

	if (ledc_pinctrl_init())
	{
		led_err("ledc pinctrl init failed \n");
		ret = -1;
		goto err1;
	}
	sunxi_ledc_restore_regs(led);
	hal_enable_irq(SUNXI_IRQ_LEDC);
	ledc_info("hal ledc resume");

	return 0;

err1:
	ledc_clk_exit();
err0:
	return ret;
}

static int hal_ledc_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	hal_disable_irq(SUNXI_IRQ_LEDC);
	sunxi_ledc_save_regs(led);
	ledc_pinctrl_exit();
	ledc_clk_exit();
	ledc_info("hal ledc suspend");
	return 0;
}

struct pm_devops pm_ledc_ops = {
	.suspend = hal_ledc_suspend,
	.resume = hal_ledc_resume,
};

struct pm_device pm_ledc = {
	.name = "sunxi_ledc",
	.ops = &pm_ledc_ops,
};
#endif

int hal_ledc_init(void)
{
	ledc_info("hal_led_init\n");

	led = malloc(sizeof(struct sunxi_led));
	if (NULL == led) {
		led_err("sunxi led malloc err\n");
		return -1;
	}

	sunxi_led_get_config(&led->config);

	led->config.data = malloc(sizeof(unsigned int) * led->config.led_count);
	if (NULL == led->config.data) {
		led_err("sunxi led config data malloc err\n");
		goto err1;
	}

	if (ledc_clk_init())
	{
		led_err("ledc clk init failed \n");
	}

	if (ledc_pinctrl_init())
	{
		led_err("ledc pinctrl init failed \n");
	}

	hal_dma_chan_request(&dma_chan);

	if (hal_request_irq(SUNXI_IRQ_LEDC, sunxi_ledc_irq_handler, "ledc", led) < 0)
	{
		led_err("ledc request irq failed \n");
		goto errirq;
	}

	hal_enable_irq(SUNXI_IRQ_LEDC);

#ifdef CONFIG_COMPONENTS_PM
	pm_devops_register(&pm_ledc);
#endif

	ledc_info("hal_led_init success\n");

	return 0;

errirq:
	free(led->config.data);
err1:
	free(led);

	return -1;
}

void hal_ledc_deinit(void)
{
#ifdef CONFIG_COMPONENTS_PM
	pm_devops_unregister(&pm_ledc);
#endif
	hal_disable_irq(SUNXI_IRQ_LEDC);
	hal_free_irq(SUNXI_IRQ_LEDC);
	hal_dma_chan_free(dma_chan);
	ledc_pinctrl_exit();
	ledc_clk_exit();
	free(led->config.data);
	free(led);
}

int sunxi_set_all_led(unsigned int brightness)
{
	int i;

	led->config.length = led->config.led_count;
	for(i = 0;i < led->config.led_count;i++)
		led->config.data[i] = brightness;

	hal_ledc_trans_data(&led->config);

	return 0;
}

int sunxi_set_led_brightness(int led_num, unsigned int brightness)
{
	u32 reg_val;

	if (NULL == led) {
		led_err("err : ledc is not init\n");
		return -1;
	}

	if (led_num > led->config.led_count) {
		led_err("has not the %d led\n", led_num);
		return -1;
	}

	led->config.length = 1;
	led->config.data[led_num-1] = brightness;

	hal_ledc_trans_data(&led->config);

	reg_val = hal_ledc_get_irq_status();
	ledc_info("ledc interrupt status reg is %x", reg_val);

	return 0;
}
