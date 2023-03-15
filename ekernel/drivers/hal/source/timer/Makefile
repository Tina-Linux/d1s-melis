obj-y += hal_timer.o
obj-$(CONFIG_DRIVERS_TIMER) += sunxi_timer.o
obj-$(CONFIG_DRIVERS_TIMER_AVS) += hal_avs.o
obj-$(CONFIG_DRIVERS_WAKEUP_TIMER) += sunxi_wuptimer.o
obj-$(CONFIG_DRIVERS_HTIMER) += hal_htimer.o sunxi_htimer.o

ifeq ($(CONFIG_COMPONENTS_PM), y)
CFLAGS += -Icomponents/common/aw/pm/include/
endif
