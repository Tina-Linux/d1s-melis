ccflags-y +=	-I$(obj)/../include \
				-I$(srctree)/ekernel/subsys/avframework/v4l2/include \
				-I$(srctree)/ekernel/subsys/avframework/v4l2/os_support/include \
				-I$(srctree)/include/media \
				-I$(srctree)/include/melis/misc \
				-I$(srctree)/include/melis/common \
				-I$(srctree)/ekernel/subsys/libc/mmap

lib-y				:= isp_dev.o
lib-y				+= isp_v4l2_helper.o
lib-y				+= media.o
lib-y				+= video.o

TARGET	:= $(obj)/../out/libisp_dev.a

include $(MELIS_BASE)/scripts/Makefile.rename
