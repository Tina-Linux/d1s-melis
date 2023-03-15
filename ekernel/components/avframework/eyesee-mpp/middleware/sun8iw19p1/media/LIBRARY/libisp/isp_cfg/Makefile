subdir-ccflags-y +=	-I$(obj)/../include \
					-I$(srctree)/ekernel/components/avframework/v4l2/include \
					-I$(srctree)/include/media \
					-I$(srctree)/include/melis/misc \
					-I$(srctree)/include/melis/common

lib-y				:= isp_ini_parse.o

TARGET	:= $(obj)/../out/libisp_ini.a

include $(MELIS_BASE)/scripts/Makefile.rename
