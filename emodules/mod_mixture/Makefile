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

MOD_NAME := mixture
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += mod_mixture.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += startup_check/startup_check.o 
$(MOD_NAME)-objs += bmp_parser/bmp.o 
$(MOD_NAME)-objs += bmp_parser/Parse_Picture.o

include $(MELIS_BASE)/scripts/Makefile.mods

