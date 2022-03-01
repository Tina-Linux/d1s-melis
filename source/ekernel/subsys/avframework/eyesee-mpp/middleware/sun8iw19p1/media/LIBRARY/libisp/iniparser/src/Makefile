ccflags-y +=	-I$(obj)/../../include \
					-I$(srctree)/include/melis/misc \
					-I$(srctree)/include/melis/common \

lib-y				:= iniparser.o
lib-y				+= dictionary.o

TARGET	:= $(obj)/../../out/libiniparser.a

include $(MELIS_BASE)/scripts/Makefile.rename
