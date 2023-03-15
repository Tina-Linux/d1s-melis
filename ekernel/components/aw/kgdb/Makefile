ccflags-y += -I$(obj)
ccflags-y += -I$(obj)/arch/arm
ccflags-y += -I$(obj)/board/melis
ccflags-y += -I$(srctree)/ekernel/components/aw/multi_console/

obj-y += arch/arm/hardware_wbp.o
obj-y += arch/arm/arm_stub.o
obj-y += arch/arm/excep_hook.o

obj-y += breakpoint.o
obj-y += kgdb_util.o
obj-y += io_stub.o
obj-y += packet.o
obj-y += kgdb_entry.o

obj-y += board/melis/gdb_cli.o
obj-y += board/melis/serial_stub.o
obj-y += board/melis/gdb_console.o
