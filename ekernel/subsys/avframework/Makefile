subdir-ccflags-y += 	-D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DUSE_STD_LOG -D__OS_LINUX \
			-Wno-strict-prototypes -Wno-type-limits -Wno-multichar -Wno-missing-braces \
			-Wno-shift-negative-value -Wno-char-subscripts -Wno-discarded-qualifiers

obj-y += v4l2/lib/
obj-$(CONFIG_ENABLE_MPP) 	+= eyesee-mpp/
obj-$(CONFIG_CSI_VIN) 		+= v4l2/drivers/media/platform/sunxi-vin/modules/sensor/

ifneq ($(CONFIG_SDK_RELEASE),y)
obj-$(CONFIG_MEDIA_SUPPORT) 	+= v4l2/drivers/media/
endif
