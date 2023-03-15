EXTRA_CFLAGS += -I$(obj)/ \
        		-I$(obj)/includes   \
        		-I$(obj)/dmsdev \
        		-I$(obj)/hwscdev \
        		-I$(obj)/pwrman

obj-y                             += devman.o
obj-${CONFIG_LEGACY_HOTPLUG}  += hotplg/hotplug.o
obj-${CONFIG_LEGACY_DMS_DEV}      += dmsdev/dmsdev.o
obj-${CONFIG_LEGACY_HSWC_DEV}     += hwscdev/hwscdev.o
obj-${CONFIG_LEGACY_RAMDISK_DEV}  += ramdiskdev/ramdiskdev.o
