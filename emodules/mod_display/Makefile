subdir-ccflags-y :=	-I$(srctree)/include/melis/kernel/drivers  \
			-I$(srctree)/include/melis/arch/chip \
			-I$(srctree)/include/melis/module  \
			-I$(srctree)/include/melis/kernel \
			-I$(srctree)/include/melis/board \
			-I$(srctree)/include/melis/libs/libc	\

usrlibs-y +=		-L$(srctree)/${elibrary-libs}/3rd  \
			--start-group \
			-lsyscall  -lminic -lpub0  -llzma \
			--end-group

MOD_NAME := display
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += mod_display.o
$(MOD_NAME)-objs += magic.o

include $(MELIS_BASE)/scripts/Makefile.mods

