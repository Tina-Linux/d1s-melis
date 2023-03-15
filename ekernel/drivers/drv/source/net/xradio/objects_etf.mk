ifeq ($(CONFIG_DRIVERS_XRADIO),y)

AW_SRCDIR = $(BUILD_DIR)

include $(BASE)/drivers/drv/wireless/xradio/config.mk

CFLAGS += -DCONFIG_OS_NUTTX -DCONFIG_OS_NUTTX_CM_PASS
CFLAGS += -DCONFIG_KERNEL_FREERTOS -DCONFIG_ARCH_SUN8IW18P1
CFLAGS += -DCONFIG_DRIVER_XRADIO -DCONFIG_BACKEND_XRADIO -DCONFIG_DETECT_CARD
CFLAGS += -D__CONFIG_ETF -D__CONFIG_ETF_CLI

ifeq ($(CONFIG_DRIVERS_XRADIO_CMD),y)
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_util.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_etf.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/command.o

endif

$(XRADIO_OBJS):MODULE_NAME="xradio"

$(XRADIO_OBJS):CFLAGS += -I $(BASE)/include/drivers
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/include
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/os
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/os/include
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/os/include/net

$(XRADIO_OBJS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/port/
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/include/drivers/
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/include/freertos/
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9


$(XRADIO_OBJS):CFLAGS += -I $(BASE)/components/thirdparty/network/lwip/src/include
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/chip-src/sdmmc
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/include/drivers/sdmmc

$(XRADIO_OBJS):CFLAGS += -I $(BASE)/include/drivers
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/components/aw/network/tcpip
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/components/thirdparty/console
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/sunxi-platform

LDFLAGS += -L$(BASE)/drivers/drv/wireless/xradio -lxretf

OBJECTS +=$(XRADIO_OBJS)

endif

