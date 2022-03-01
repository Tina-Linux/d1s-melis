subdir-ccflags-y +=			-I$(srctree)/include/melis/kernel \
				-I$(srctree)/include/melis/libs/libc \
				-I$(srctree)/include/melis/board   \
				-I$(srctree)/include/melis/module  \
				-I$(srctree)/include/melis/kernel/drivers  \
				-I$(srctree)/emodules/fm/fm_module/fm_all/  \
				-I$(srctree)/emodules/fm/ \
				-I$(srctree)/ekernel/drivers/include/hal \
				-I$(srctree)/ekernel/drivers/include/drv \
				-I$(srctree)/ekernel/core/rt-thread/include		
				
usrlibs-y +=			--start-group \
				-lsyscall  -lminic  -lpub0 \
				--end-group

MOD_NAME := fm
SUF_NAME := drv
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/drv/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += fm_module/fm_all/fm_all_drv.o
$(MOD_NAME)-objs += dev_fm.o
$(MOD_NAME)-objs += drv_fm.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += circle_buf.o

include $(MELIS_BASE)/scripts/Makefile.mods
