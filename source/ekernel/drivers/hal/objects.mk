#DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_EFUSE) += $(BUILD_DIR)/drivers/chip-hal/hal_efuse.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_EFUSE) += $(BUILD_DIR)/drivers/hal/source/efuse/efuse.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_EFUSE) += $(BUILD_DIR)/drivers/hal/source/efuse/hal_efuse.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SPI) += $(BUILD_DIR)/drivers/hal/source/spi/hal_spi.o

DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SPINOR) += \
	$(BUILD_DIR)/drivers/hal/source/spinor/hal_spinor.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/core.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_gd.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_mxic.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_winbond.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_esmt.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_xtx.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_fm.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_xmc.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_puya.o \
	$(BUILD_DIR)/drivers/hal/source/spinor/factory_zetta.o

DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SPINOR_CACHE) += $(BUILD_DIR)/drivers/hal/source/spinor/cache.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SPINOR_WRITE_LOCK) += $(BUILD_DIR)/drivers/hal/source/spinor/wrlock.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_LEDC) += $(BUILD_DIR)/drivers/hal/source/ledc/hal_ledc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_LRADC) += $(BUILD_DIR)/drivers/hal/source/lradc/hal_lradc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_GPADC) += $(BUILD_DIR)/drivers/hal/source/gpadc/hal_gpadc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_PWM) += $(BUILD_DIR)/drivers/hal/source/pwm/hal_pwm.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_GPIO) += $(BUILD_DIR)/drivers/hal/source/gpio/hal_gpio.o
ifdef CONFIG_ARCH_SUN8IW18P1
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_GPIO) += $(BUILD_DIR)/drivers/hal/source/gpio/sun8iw18/gpio-sun8iw18.o
endif
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_DMA) += $(BUILD_DIR)/drivers/hal/source/dma/hal_dma.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_REGULATOR) += $(BUILD_DIR)/drivers/hal/source/regulator/sun8iw18p1/core.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_THERMAL) += $(BUILD_DIR)/drivers/hal/source/thermal/hal_thermal.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_UART) += $(BUILD_DIR)/drivers/hal/source/uart/hal_uart.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_RTC) += $(BUILD_DIR)/drivers/hal/source/rtc/hal_rtc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_RTC) += $(BUILD_DIR)/drivers/hal/source/rtc/rtc-lib.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_WATCHDOG) += $(BUILD_DIR)/drivers/hal/source/watchdog/hal_watchdog.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/ccmu/hal_clk.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/ccmu/hal_reset.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/ccmu/sunxi/clk.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/ccmu/sunxi/clk_factors.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/ccmu/sunxi/clk_periph.o
ifdef CONFIG_ARCH_SUN8IW18P1
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/ccmu/sunxi/sun8iw18p1/clk_sun8iw18.o
endif
DRIVERS_HAL_OBJECTS-y += $(BUILD_DIR)/drivers/hal/source/timer/hal_timer.o
DRIVERS_HAL_OBJECTS-y += $(BUILD_DIR)/drivers/hal/source/timer/sunxi_timer.o

# sdmmc
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/core.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/hal_sdpin.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/hal_sdhost.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/mmc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/quirks.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/sd.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/sdio.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/sdio_irq.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/test.o
#DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/cmd/cmd_sd.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/osal/os/FreeRTOS/os_debug.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/osal/os/FreeRTOS/os_mutex.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/osal/os/FreeRTOS/os_queue.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/osal/os/FreeRTOS/os_semaphore.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/osal/os/FreeRTOS/os_thread.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_SDMMC) += $(BUILD_DIR)/drivers/hal/source/sdmmc/osal/os/FreeRTOS/os_timer.o

DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CE) += $(BUILD_DIR)/drivers/hal/source/ce/hal_ce.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CE) += $(BUILD_DIR)/drivers/hal/source/ce/ce_common.o

DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_USB) += $(BUILD_DIR)/drivers/hal/source/usb/udc/hal_udc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_USB) += $(BUILD_DIR)/drivers/hal/source/usb/include/platform_usb.o
ifdef CONFIG_ARCH_SUN8IW18P1
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/usb/platform/sun8iw18/usb_sun8iw18.o
endif
ifdef CONFIG_ARCH_SUN8IW19
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/usb/platform/sun8iw19/sun8iw19.o
endif
ifdef CONFIG_SOC_SUN20IW1
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/usb/platform/sun20iw1/usb_sun20iw1.o
endif
ifdef CONFIG_ARCH_SUN8IW20
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_CCMU) += $(BUILD_DIR)/drivers/hal/source/usb/platform/sun20iw1/usb_sun20iw1.o
endif

DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_MBUS) += $(BUILD_DIR)/drivers/hal/source/mbus/hal_mbus.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_MBUS) += $(BUILD_DIR)/drivers/hal/source/mbus/mbus.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_HTIMER) += $(BUILD_DIR)/drivers/hal/source/timer/hal_htimer.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_HTIMER) += $(BUILD_DIR)/drivers/hal/source/timer/sunxi_htimer.o
DRIVERS_HAL_OBJECTS-$(CONFIG_DRIVERS_TWI) += $(BUILD_DIR)/drivers/hal/source/twi/hal_twi.o


DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_CCMU) += $(BUILD_DIR)/drivers/hal/test/ccmu/test_ccmu.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_CE) += $(BUILD_DIR)/drivers/hal/test/ce/test_ce.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_DMA) += $(BUILD_DIR)/drivers/hal/test/dma/test_dma.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_GPADC) += $(BUILD_DIR)/drivers/hal/test/gpadc/test_gpadc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_GPIO) += $(BUILD_DIR)/drivers/hal/test/gpio/test_gpio.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_PWM) += $(BUILD_DIR)/drivers/hal/test/pwm/test_pwm.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_EFUSE) += $(BUILD_DIR)/drivers/hal/test/efuse/test_efuse.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_SPI) += $(BUILD_DIR)/drivers/hal/test/spi/test_spi.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_SPINOR) += $(BUILD_DIR)/drivers/hal/test/spinor/test_spinor.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_THERMAL) += $(BUILD_DIR)/drivers/hal/test/thermal/test_thermal.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_TWI) += $(BUILD_DIR)/drivers/hal/test/twi/test_twi.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_UART) += $(BUILD_DIR)/drivers/hal/test/uart/test_uart.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_RTC) += $(BUILD_DIR)/drivers/hal/test/rtc/test_rtc.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_UDC) += $(BUILD_DIR)/drivers/hal/test/usb/udc/usb_msg.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_UDC) += $(BUILD_DIR)/drivers/hal/test/usb/udc/ed_test.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_UDC) += $(BUILD_DIR)/drivers/hal/test/usb/udc/main.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_TIMER) += $(BUILD_DIR)/drivers/hal/test/timer/test_timer.o
DRIVERS_HAL_OBJECTS-$(CONFIG_HAL_TEST_HCI) += $(BUILD_DIR)/drivers/hal/test/usb/host/test_hci.o
DRIVERS_HAL_OBJECTS-y += $(BUILD_DIR)/drivers/hal/source/common/dma_alloc.o

$(DRIVERS_HAL_OBJECTS-y):MODULE_NAME="Driver-Hal"

$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/hal/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/drivers/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/freertos/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/drivers/chip-src/uart/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/drivers/chip-src/ccmu/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/port/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9

# sdmmc
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/hal/sdmmc/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/hal/sdmmc/hal/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/hal/sdmmc/sys/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/hal/sdmmc/osal/
$(DRIVERS_HAL_OBJECTS-y):CFLAGS += -I $(BASE)/include/hal/sdmmc/osal/FreeRTOS/

include $(BASE)/drivers/hal/source/sound/objects.mk

OBJECTS += $(DRIVERS_HAL_OBJECTS-y)

