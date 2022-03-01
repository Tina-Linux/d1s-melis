subdir-ccflags-y :=	-I$(srctree)/include/melis/ekernel/drivers  \
			-I$(srctree)/include/melis/elibrary/libc/misc/pub0 \
			-I$(srctree)/include/melis/emodules  \
			-I$(srctree)/include/melis/ekernel \
			-I$(srctree)/emodules/mod_jpeg_dec/macc \
			-I$(srctree)/emodules/mod_jpeg_dec \
			-I$(srctree)/emodules/mod_jpeg_dec/mjpegDecoder \
			-I$(srctree)/include/melis/libs/libc

usrlibs-y +=		-L$(srctree)/elibrary/bin/3rd  \
			--start-group \
			-lsyscall -lminic -lpub0 \
			--end-group

MOD_NAME := vcoder
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin/

$(MOD_NAME)-objs += video_coder.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += mjpegDecoder/JpegDecLib.o
$(MOD_NAME)-objs += mjpegDecoder/JpegDecoder.o
$(MOD_NAME)-objs += mjpegDecoder/mjpegDecode.o
$(MOD_NAME)-objs += macc/macc_bsp.o

include $(MELIS_BASE)/scripts/Makefile.mods
