#obj-$(CONFIG_CSI_VIN) += modules/sensor/
obj-$(CONFIG_CSI_VIN) += modules/actuator/
obj-$(CONFIG_CSI_VIN) += 			vin_io.o
#vin_io-$(CONFIG_CSI_CCI)			:= vin-cci/csi_cci_reg.o
#vin_io-$(CONFIG_CSI_CCI)			+= vin-cci/bsp_cci.o
vin_io-y					+= vin-cci/cci_helper.o
#vin_io-y					+= vin-cci/sunxi_cci.o
vin_io-y					+= utility/vin_supply.o
vin_io-y					+= utility/vin_os.o
#vin_io-y					+= modules/sensor/sensor-compat-ioctl32.o
#vin_io-y					+= modules/sensor/sensor_helper.o

obj-$(CONFIG_CSI_VIN) += 			vin_v4l2.o
vin_v4l2-y					:= vin-csi/sunxi_csi.o
vin_v4l2-y					+= vin-csi/parser_reg.o
vin_v4l2-y					+= vin-mipi/sunxi_mipi.o
vin_v4l2-y					+= vin-mipi/bsp_mipi_csi.o
vin_v4l2-y					+= vin-isp/sunxi_isp.o
vin_v4l2-y					+= vin-isp/isp521/isp521_reg_cfg.o

vin_v4l2-y					+= vin-vipp/sunxi_scaler.o
vin_v4l2-y					+= vin-vipp/vipp_reg.o
vin_v4l2-y					+= modules/flash/flash.o
vin_v4l2-y					+= utility/bsp_common.o
vin_v4l2-y					+= utility/config.o
vin_v4l2-y					+= utility/cfg_op.o
vin_v4l2-y					+= vin-stat/vin_h3a.o
vin_v4l2-y					+= vin-video/vin_video.o
vin_v4l2-y					+= vin-video/vin_core.o
vin_v4l2-y					+= vin-video/dma_reg.o
vin_v4l2-y					+= top_reg.o
vin_v4l2-y					+= vin.o

vin_v4l2-$(CONFIG_CSI_VIN_TEST)			+= vin_test/mplane_image/csi_test_mplane_mmap.o
vin_v4l2-$(CONFIG_sample_driverVipp) 		+= sample_driverVipp/sample_driverVipp.o
