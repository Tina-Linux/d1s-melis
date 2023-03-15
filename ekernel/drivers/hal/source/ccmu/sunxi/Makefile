ccflags-y += -I$(obj)/

ifeq ($(CONFIG_OS_MELIS), y)
ccflags-y += -I$(srctree)/include/melis/kernel/drivers
endif

obj-y += clk.o
obj-y += clk_factors.o
obj-y += clk_periph.o

obj-$(CONFIG_ARCH_SUN8IW18) += sun8iw18p1/
obj-$(CONFIG_ARCH_SUN8IW19P1) += sun8iw19p1/
obj-$(CONFIG_ARCH_SUN8IW21P1) += sun8iw21p1/
obj-$(CONFIG_ARCH_SUN20IW3) += sun8iw21p1/
