subdir-ccflags-y +=	-I drivers/hal/source/usb/gadget
subdir-ccflags-y +=	-I drivers/indlude/drv
# subdir-ccflags-y +=	-I$(srctree)/include/melis/ekernel/pthread
ifdef CONFIG_DRIVERS_USB_GADGET_MSC
subdir-ccflags-y +=	-I drivers/hal/source/usb/gadget/function/msc
endif

obj-${CONFIG_DRIVERS_USB_GADGET_ADB} += function/adb.o
obj-${CONFIG_DRIVERS_USB_GADGET_MTP} += function/mtp.o
obj-${CONFIG_DRIVERS_USB_GADGET_NCM} += function/ncm/f_ncm.o
obj-${CONFIG_DRIVERS_USB_GADGET_MSC} += function/msc/msc.o
obj-${CONFIG_DRIVERS_USB_GADGET_MSC} += function/msc/scsi.o
obj-${CONFIG_DRIVERS_USB_GADGET_MSC} += function/msc/usb_slave_msc.o
obj-y += gadget.o
obj-y += gadget_hal.o
