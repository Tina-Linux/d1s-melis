ifeq ($(CONFIG_OS_MELIS), y)
subdir-ccflags-y +=	-I$(srctree)/ekernel/drivers/include \
			-I$(srctree)/ekernel/core/rt-thread/include \
			-I$(srctree)/ekernel/drivers/drv/source/disp2/soc
endif

ifeq ($(CONFIG_KERNEL_FREERTOS), y)
subdir-ccflags-y +=	-I drivers/hal/test/disp2 -I drivers/hal/source/disp2
endif

obj-y += disp_mem.o disp_layer_cfg.o

obj-y += disp_layer_alpha_test.o
obj-y += disp_layer_scal_test.o
obj-y += disp_layer_format_test.o
obj-y += disp_lbc_test.o
