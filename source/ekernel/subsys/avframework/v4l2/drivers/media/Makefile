media-objs	:= media-device.o media-devnode.o media-entity.o

obj-$(CONFIG_MEDIA_SUPPORT) += media.o
obj-$(CONFIG_VIDEO_DEV) += v4l2-core/

subdir-ccflags-y += -I$(srctree)/ekernel/subsys/avframework/v4l2/include/media \
	-I$(srctree)/ekernel/subsys/avframework/v4l2/include \
	-I$(srctree)/ekernel/subsys/avframework/v4l2/include/uapi \
	-I$(srctree)/ekernel/drivers/hal/source/ccmu \
	-Wno-address-of-packed-member

obj-y += platform/

# NNA drivers
obj-$(CONFIG_SUNXI_NNA) += sunxi_nna/

ifneq ($(CONFIG_SDK_RELEASE),y)
$(obj)/makelib: $(obj)/built-in.o
	@$(AR) -crs $(srctree)/${elibrary-libs}/3rd/v4l2/libv4l2.a  $(obj)/built-in.o

always     	:= makelib
endif
