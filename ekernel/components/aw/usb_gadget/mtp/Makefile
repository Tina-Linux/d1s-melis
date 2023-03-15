subdir-ccflags-y +=	-I$(srctree)/include/melis/kernel/drivers \
			-I$(srctree)/include/melis/kernel \
			-I$(srctree)/include/melis/kernel/pthread \
			-I$(srctree)/include/melis/libs/libc \
			-I$(srctree)/ekernel/components \
			-I$(srctree)/ekernel/components/finsh_cli \
			-I$(srctree)/ekernel/drivers/include/drv \
			-I$(srctree)/ekernel/drivers/include/hal \
			-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL -Wall -Wstrict-prototypes

EXTRA_CFLAGS+=-Wno-date-time

obj-y += mtp.o
obj-y += usb.o
