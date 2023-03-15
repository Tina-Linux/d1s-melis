subdir-ccflags-y :=	-I$(srctree)/include/melis/kernel/drivers  \
			-I$(srctree)/include/melis/arch/chip \
			-I$(srctree)/include/melis/module  \
			-I$(srctree)/include/melis/kernel \
			-I$(srctree)/include/melis/board \
			-I$(srctree)/include/melis/libs/libc	\
			-I$(srctree)/ekernel/components/aw/blkpart	\
			-I$(srctree)/ekernel/drivers/include/drv	\

usrlibs-y +=		-L$(srctree)/${elibrary-libs}/3rd  \
			--start-group \
			-lsyscall  -lminic -lpub0  -llzma \
			--end-group

MOD_NAME := update
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += sunxi_update.o
$(MOD_NAME)-objs += sunxi_flash.o
$(MOD_NAME)-objs += sunxi_partition.o
$(MOD_NAME)-objs += sunxi_update_misc.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += clear_fw.o
$(MOD_NAME)-objs += image/encode.o
$(MOD_NAME)-objs += image/image.o
include $(MELIS_BASE)/scripts/Makefile.mods

