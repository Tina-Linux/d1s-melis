subdir-ccflags-y +=	-I$(srctree)/include/melis/kernel/drivers \
			-I$(srctree)/include/melis/kernel \
			-I$(srctree)/include/melis/kernel/pthread \
			-I$(srctree)/include/melis/libs/libc \
			-I$(srctree)/ekernel/subsys \
			-I$(srctree)/ekernel/subsys/adbd \
			-I$(srctree)/ekernel/subsys/adbd/include \
			-I$(srctree)/ekernel/subsys/finsh_cli \
			-I$(srctree)/ekernel/subsys/aw/multi_console \
			-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/ \
			-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include \
			-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/include \
			-I$(srctree)/emodules/mod_shell/Esh_builtins/commands/doscript \
			-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL -Wall -Wstrict-prototypes

EXTRA_CFLAGS+=-Wno-date-time

obj-y += adb.o
obj-y += usb.o
obj-y += adb_misc.o
obj-y += transport.o
obj-y += transport_usb.o
obj-y += service.o
obj-y += adb_shell.o
obj-y += file_sync_service.o
obj-$(CONFIG_COMPONENTS_ADBD_SOCKETS) += sockets.o

$(obj)/makelib: $(obj)/built-in.o
	@$(AR) -crs $(srctree)/${elibrary-libs}/3rd/adbd/libadbd.a  $(obj)/built-in.o

always     	:= makelib
