subdir-ccflags-y +=	-I drivers/include -I drivers/hal/source/spilcd/soc
subdir-ccflags-y += -I drivers/hal/source/spilcd/lcd_fb
subdir-ccflags-y += -I drivers/hal/source/spilcd/lcd_fb/panels

ifeq ($(CONFIG_OS_MELIS), y)
subdir-ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include
endif

obj-y += lcd_fb/
obj-y += soc/
