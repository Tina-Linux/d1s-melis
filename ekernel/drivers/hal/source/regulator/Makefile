ifeq ($(CONFIG_OS_MELIS), y)
ccflags-y += -I$(srctree)/include/melis/kernel/drivers
ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include
endif

subdir-ccflags-y += -I drivers/include -I$(obj)/

obj-y += axp_twi.o axp.o axp2101.o
obj-$(CONFIG_SOC_SUN8IW19P1) += sun8iw19p1/core.o
obj-$(CONFIG_ARCH_SUN20IW2) += sun20iw2p1/core.o
