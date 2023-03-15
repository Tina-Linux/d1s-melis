ccflags-y += 	-I$(srctree)/ekernel/components/avframework/v4l2/include \
		-I$(srctree)/ekernel/components/avframework/v4l2/include/uapi \
		-I$(srctree)/ekernel/drivers/hal/source/osal/include \
		-I$(srctree)/ekernel/components/avframework/v4l2/include/media \
		-I$(srctree)/ekernel/drivers/hal/source/ccmu \
		-I$(srctree)/ekernel/drivers/include/hal

obj-$(CONFIG_VIN_SENSOR_OV5640)			+= ov5640.o
obj-$(CONFIG_VIN_SENSOR_imx386)			+= imx386_mipi.o
obj-$(CONFIG_VIN_SENSOR_C2398)			+= C2398_mipi.o
obj-$(CONFIG_VIN_SENSOR_gc2053)			+= gc2053_mipi.o
obj-$(CONFIG_VIN_SENSOR_tw2866)			+= tw2866.o
obj-$(CONFIG_VIN_SENSOR_tp9950)			+= tp9950.o
obj-y += sensor_helper.o
