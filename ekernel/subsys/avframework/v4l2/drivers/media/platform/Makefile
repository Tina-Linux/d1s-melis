#
# Makefile for the video capture/playback device drivers.
#

obj-$(CONFIG_SUNXI_ISE) += sunxi-ise/
obj-$(CONFIG_SUNXI_EISE) += sunxi-eise/


obj-$(CONFIG_SUNXI_EVE)		+= sunxi-eve/
obj-$(CONFIG_VIDEO_SUNXI_VIN)		+= sunxi-vin/
obj-$(CONFIG_SENSOR_POWER)  		+= sunxi-vin/modules/sensor_power/
obj-$(CONFIG_VIDEO_SUNXI_TVD)		+= sunxi-tvd/

subdir-ccflags-y += -I$(srctree)/ekernel/subsys/libc/mmap/
