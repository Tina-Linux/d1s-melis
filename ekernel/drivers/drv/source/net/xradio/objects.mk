ifeq ($(CONFIG_DRIVERS_XRADIO),y)

AW_SRCDIR = $(BUILD_DIR)

CFLAGS_COMMON += -DCONFIG_BACKEND_XRADIO
CFLAGS_COMMON += -DCONFIG_DRIVER_XRADIO
CFLAGS_COMMON += -DCONFIG_CRYPTO_INTERNAL
CFLAGS_COMMON += -DCONFIG_NO_RANDOM_POOL
CFLAGS_WPAS += -DCONFIG_NO_CONFIG_BLOBS
CFLAGS_WPAS += -DCONFIG_NO_ROAMING
CFLAGS_HOSTAPD += -DCONFIG_NO_RADIUS
CFLAGS_HOSTAPD += -DCONFIG_NO_ACCOUNTING
CFLAGS_HOSTAPD += -DCONFIG_NO_VLAN
CFLAGS_HOSTAPD += -DCONFIG_NO_CTRL_IFACE

include $(BASE)/drivers/drv/wireless/xradio/config.mk

CFLAGS +=-DCONFIG_OS_NUTTX -DCONFIG_OS_NUTTX_CM_PASS
CFLAGS +=$(CONFIG_SYMBOLS) $(CFLAGS_COMMON) $(CFLAGS_WPAS) $(CFLAGS_HOSTAPD)
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/ethernetif.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/sys/mbuf/mbuf_0.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/sys/atomic.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/kernel/os/os_mutex.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/kernel/os/os_queue.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/kernel/os/os_semaphore.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/kernel/os/os_thread.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/kernel/os/os_timer.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/os/kernel/os/os_error.o

XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/atomic.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/gpio.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/sdio.o
#XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/delay.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/xr_drv_mem.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/ffs.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/hexdump.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/driver/port/workqueue.o

ifeq ($(CONFIG_DRIVERS_XRADIO_CMD),y)
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/net_ctrl.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/net_init.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/net_sys.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sysinfo.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl/container.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl/event_queue.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl/looper.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl/observer.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl/publisher.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl/sys_ctrl.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/wifi_adapter.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/wifimanager.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/wifi_main.o



XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_ifconfig.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_util.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_wlan.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_lmac.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/cmd_wlancmd.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/frm_construct.o
XRADIO_OBJS += $(AW_SRCDIR)/drivers/drv/wireless/xradio/adapter/cmd/command.o

$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/adapter
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/adapter/net_ctrl
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/adapter/net_ctrl/sys_ctrl
$(XRADIO_OBJS):CFLAGS += -I $(BASE)/drivers/drv/wireless/xradio/adapter/cmd
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

ifeq ($(CONFIG_DRIVER_XR819),y)
LDFLAGS += -L$(BASE)/drivers/drv/wireless/xradio -lxr819
endif

ifeq ($(CONFIG_DRIVER_XR829),y)
ifeq ($(CONFIG_XR829_40M_FW),y)
LDFLAGS += -L$(BASE)/drivers/drv/wireless/xradio -lxr829_40M
else
LDFLAGS += -L$(BASE)/drivers/drv/wireless/xradio -lxr829
endif
endif

endif

OBJECTS +=$(XRADIO_OBJS)
