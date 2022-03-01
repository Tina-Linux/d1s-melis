ccflags-y +=	-I$(obj)/../include \
					-I$(srctree)/ekernel/subsys/avframework/v4l2/include \
					-I$(srctree)/include/media \
					-I$(srctree)/include/melis/misc \
					-I$(srctree)/include/melis/common \

lib-y				:= isp_math_util.o
lib-y				+= matrix.o

TARGET	:= $(obj)/../out/libisp_math.a

include $(MELIS_BASE)/scripts/Makefile.rename
