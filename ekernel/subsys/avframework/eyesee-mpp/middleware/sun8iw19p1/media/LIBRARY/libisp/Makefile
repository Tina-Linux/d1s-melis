subdir-ccflags-y +=	-I$(obj)/include \
					-I$(srctree)/ekernel/subsys/avframework/v4l2/include \
					-I$(srctree)/ekernel/subsys/avframework/v4l2/os_support/include \
					-I$(srctree)/include/media \
					-I$(srctree)/include/melis/misc \
					-I$(srctree)/include/melis/common \

obj-y	+= iniparser/src/
obj-y	+= isp_cfg/
obj-y	+= isp_dev/
obj-y	+= isp_math/

#usrlibs-y += 	-L$(srctree)/${elibrary-libs} \
#				-L$(srctree)/ekernel/subsys/avframework/libisp/out \
#				--start-group \
#				-liniparser \
#				-lisp_algo \
#				-lisp_ini \
#				-lisp_dev \
#				-lisp_math \
#				--end-group

obj-y			+= libisp.o
libisp-y		:= isp.o
libisp-y		+= isp_events/events.o
libisp-y		+= isp_manage/isp_helper.o \
				   isp_manage/isp_manage.o
libisp-y		+= isp_tuning/isp_tuning.o
libisp-y		+= isp_test.o
libisp-y		+= vin_isp_test.o
#libisp-y		+= vin_test.o
