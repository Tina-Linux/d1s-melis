subdir-ccflags-y += -I drivers/include
subdir-ccflags-y += -I drivers/hal/source/disp2/soc

ifeq ($(CONFIG_OS_MELIIS), y)
subdir-ccflags-y += -I $(srctree)/ekernel/core/rt-thread/include
endif

obj-y += disp/
obj-y += soc/
obj-$(CONFIG_TV_DISP2_SUNXI) += tv/
