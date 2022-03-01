ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include/libc

lib-y += fbm.o
lib-y += pixel_format.o
lib-y += vdecoder.o
lib-y += sbm/sbmStream.o
lib-y += sbm/sbmHwProcess.o
lib-y += sbm/sbmFrameH264.o
lib-y += sbm/sbmFrameH265.o
lib-y += sbm/sbmFrameBase.o

TARGET := $(obj)/libvdecoder.a

include $(MELIS_BASE)/scripts/Makefile.rename

