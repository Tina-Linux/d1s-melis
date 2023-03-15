subdir-ccflags-y +=			-I$(srctree)/include/melis/kernel \
				-I$(srctree)/include/melis/libs/libc \
				-I$(srctree)/include/melis/board   \
				-I$(srctree)/include/melis/module  \
				-I$(srctree)/include/melis/kernel/drivers  \
				-I$(srctree)/ekernel/drivers/include/hal \
				-I$(srctree)/ekernel/drivers/include/drv \
				-I$(srctree)/ekernel/components/avframework/v4l2/include/ \
				-I$(srctree)/ekernel/components/avframework/v4l2/include/uapi \
				-I$(srctree)/ekernel/components/thirdparty/mmap \
				-I$(srctree)/ekernel/drivers/hal/source/g2d_rcq  \
				-I$(srctree)/ekernel/drivers/hal/source/sunxi-di/drv_div1xx/ \
				-I$(srctree)/ekernel/drivers/hal/source/sunxi-di/include/\
				-I$(srctree)/ekernel/core/rt-thread/include		\
				-DFD_SETSIZE=1024

usrlibs-y += --start-group \
				--whole-archive -lsyscall --no-whole-archive -lminic  -lpub0 \
				--end-group

MOD_NAME := video_play
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += video_dev.o
$(MOD_NAME)-objs += video_v4l2.o
$(MOD_NAME)-objs += mod_video_play.o
$(MOD_NAME)-objs += magic.o

include $(MELIS_BASE)/scripts/Makefile.mods
