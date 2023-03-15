#
# Makefile for the V4L2 core
#

#tuner-objs	:=	tuner-core.o

ifeq ($(CONFIG_VIDEO_V4L2),y)

obj-y	+=	v4l2-dev.o v4l2-ioctl.o v4l2-device.o v4l2-fh.o \
			v4l2-event.o v4l2-ctrls.o v4l2-subdev.o 

#obj-y	+=		v4l2-async.o
obj-$(CONFIG_MEDIA_CONTROLLER) += v4l2-mc.o

endif

obj-$(CONFIG_VIDEO_V4L2) += v4l2-common.o
obj-$(CONFIG_VIDEO_DV_TIMINGS) += v4l2-dv-timings.o

#obj-$(CONFIG_VIDEO_TUNER) += tuner.o

obj-$(CONFIG_V4L2_MEM2MEM_DEV) += v4l2-mem2mem.o

#obj-$(CONFIG_V4L2_FLASH_LED_CLASS) += v4l2-flash-led-class.o

obj-$(CONFIG_VIDEOBUF2_CORE) += videobuf2-core.o videobuf2-v4l2.o
obj-$(CONFIG_VIDEOBUF2_MEMOPS) += videobuf2-memops.o
obj-$(CONFIG_VIDEOBUF2_DMA_CONTIG) += videobuf2-dma-contig.o

ccflags-y += -I$(srctree)/ekernel/components/avframework/v4l2/include/media

#ccflags-y += -I$(srctree)/drivers/media/dvb-core
#ccflags-y += -I$(srctree)/drivers/media/dvb-frontends
#ccflags-y += -I$(srctree)/drivers/media/tuners

