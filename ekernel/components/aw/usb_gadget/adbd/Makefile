subdir-ccflags-y +=	-I$(srctree)/include/melis/kernel/drivers \
			-I$(srctree)/include/melis/kernel \
			-I$(srctree)/include/melis/kernel/pthread \
			-I$(srctree)/include/melis/libs/libc \
			-I$(srctree)/ekernel/components \
			-I$(srctree)/ekernel/components/aw/usb_gadget/adbd \
			-I$(srctree)/ekernel/components/aw/usb_gadget/adbd/include \
			-I$(srctree)/ekernel/components/thirdparty/finsh_cli \
			-I$(srctree)/ekernel/components/aw/multi_console \
			-I$(srctree)/ekernel/components/net/rt-thread/lwip/src/ \
			-I$(srctree)/ekernel/components/net/rt-thread/lwip/src/include \
			-I$(srctree)/ekernel/components/net/rt-thread/lwip/src/arch/include \
			-I$(srctree)/emodules/mod_shell/Esh_builtins/commands/doscript \
			-I$(srctree)/ekernel/drivers/include/drv \
			-I$(srctree)/ekernel/drivers/include/hal \
			-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL -Wall -Wstrict-prototypes

EXTRA_CFLAGS+=-Wno-date-time

obj-y += adb.o
obj-y += usb.o
obj-y += adb_misc.o
obj-y += transport.o
obj-y += transport_usb.o
obj-y += service.o
obj-y += file_sync_service.o
obj-$(CONFIG_COMPONENTS_ADBD_SOCKETS) += sockets.o

obj-${CONFIG_SUBSYS_MULTI_CONSOLE} += adb_shell.o
