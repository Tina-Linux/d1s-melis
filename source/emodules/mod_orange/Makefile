subdir-ccflags-y +=	-I$(srctree)/include/melis/kernel/drivers  \
			-I$(srctree)/include/melis/kernel  \
			-I$(srctree)/include/melis/module  \
			-I$(srctree)/include/melis/misc    \
			-I$(srctree)/include/melis/libs/libc  \
			-I$(srctree)/include/melis/module/mod_orange/gui/win \
			-I$(srctree)/include/melis/module/mod_orange/gui/core\
			-I$(srctree)/emodules/mod_orange   \
			-I$(srctree)/emodules/mod_orange/lds  \
			-I$(srctree)/emodules/mod_orange/GUI  \
			-I$(srctree)/emodules/mod_orange/GUI/message \
			-I$(srctree)/emodules/mod_orange/GUI/widget  \
			-I$(srctree)/emodules/mod_orange/GUI/core    \
			-I$(srctree)/emodules/mod_orange/GUI/core/cc  \
			-I$(srctree)/emodules/mod_orange/GUI/core/font  \
			-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL -DUSED_BY_ORANGEMOD -DEPDK_OS=0

usrlibs-y +=	--start-group \
		-lsyscall -lminic -llzma -lpub0 \
		--end-group  

MOD_NAME := orange
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += mod_orange.o
$(MOD_NAME)-objs += orange_mod_api.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += built-in.o
obj-y := GUI/ ResReader/
include $(MELIS_BASE)/scripts/Makefile.mods

