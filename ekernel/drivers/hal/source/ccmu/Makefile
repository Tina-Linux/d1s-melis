ccflags-y += -I$(obj)/

ifeq ($(CONFIG_OS_MELIS), y)
 ccflags-y += -I$(srctree)/include/melis/kernel/drivers
endif

obj-y += hal_clk.o hal_reset.o

obj-$(CONFIG_DRIVERS_SUNXI_CLK) += sunxi/
obj-$(CONFIG_DRIVERS_SUNXI_CCU) += sunxi-ng/
