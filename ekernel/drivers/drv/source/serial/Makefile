subdir-ccflags-y +=	-I./lib_dbg_uart \
			-I$(srctree)/ekernel/drivers/include \
			-I$(srctree)/ekernel/core/rt-thread/include
#obj-y += sios.o
#obj-y += uart_dbg.o
#obj-y += lib_dbg_uart/format_transformed.o
#obj-y += lib_dbg_uart/lib_dbg_uart.o
#obj-y += lib_dbg_uart/lib_uart_priv.o
#obj-y += lib_dbg_uart/lib_uart_util.o
obj-y += uart_drv.o
obj-${CONFIG_LEGACY_DMS_DEV}   += uart_drv_dms.o