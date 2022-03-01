ccflags-y  =

MOD_NAME := ftest
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/staging

usrlibs-y += -L$(srctree) \
			   --start-group \
			   -lsyscall -lminic\
			   --end-group

objy += magic.o
objy += mod_test.o

$(MOD_NAME)-objs := $(objy)

include $(MELIS_BASE)/scripts/Makefile.mods
