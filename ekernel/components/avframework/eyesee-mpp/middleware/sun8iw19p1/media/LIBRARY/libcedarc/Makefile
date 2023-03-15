cedartop := $(obj)
export cedartop

subdir-ccflags-y += -I$(cedartop)/vencoder/base/include \
		-I$(cedartop)/vdecoder/include \
		-I$(cedartop)/vdecoder \
		-I$(cedartop)/base/include \
		-I$(cedartop)/include \
		-DCONFIG_ENABLE_DECODER_H264 \
		-DCONFIG_ENABLE_DECODER_H265 \
		-DCONFIG_ENABLE_DECODER_MJPEGPLUS

ifeq ($(CONFIG_ENABLE_MPP), y)
subdir-ccflags-y += -DCONF_SIMPLE_PARSER_CASE
else
subdir-ccflags-y += -DCONFIG_ENABLE_DECODER_AVS \
		-DCONFIG_ENABLE_DECODER_MJPEG \
		-DCONFIG_ENABLE_DECODER_MPEG2 \
		-DCONFIG_ENABLE_DECODER_MPEG4DIVX311 \
		-DCONFIG_ENABLE_DECODER_MPEG4H263 \
		-DCONFIG_ENABLE_DECODER_MPEG4NORMAL \
		-DCONFIG_ENABLE_DECODER_MPEG4VP6 \
		-DCONFIG_ENABLE_DECODER_VP8 \
		-DCONFIG_ENABLE_DECODER_WMV3
endif

ifneq ($(filter y, $(CONFIG_mpp_compile_static_lib) $(CONFIG_MOD_CEDAR_ENABLE)),)
subdir-ccflags-y += -DCONFIG_COMPILE_STATIC_LIB
endif

subdir-ccflags-y += -Wno-format

#ref to OPTION_LOG_LEVEL_DETAIL,[cdc_config.h]
subdir-ccflags-y += -UCONFIG_LOG_LEVEL
ifeq ($(CONFIG_LOG_DEFAULT_LEVEL), 0)
subdir-ccflags-y += -DCONFIG_LOG_LEVEL=0#OPTION_LOG_LEVEL_CLOSE
else ifneq ($(filter $(CONFIG_LOG_DEFAULT_LEVEL), 1 2),)
subdir-ccflags-y += -DCONFIG_LOG_LEVEL=1#OPTION_LOG_LEVEL_ERROR
else ifeq ($(CONFIG_LOG_DEFAULT_LEVEL), 3)
subdir-ccflags-y += -DCONFIG_LOG_LEVEL=2#OPTION_LOG_LEVEL_WARNING
else ifeq ($(CONFIG_LOG_DEFAULT_LEVEL), 4)
subdir-ccflags-y += -DCONFIG_LOG_LEVEL=3#OPTION_LOG_LEVEL_DEFAULT
else ifeq ($(CONFIG_LOG_DEFAULT_LEVEL), 5)
subdir-ccflags-y += -DCONFIG_LOG_LEVEL=4#OPTION_LOG_LEVEL_DETAIL
endif

ifneq ($(filter y, $(CONFIG_mpp_venc) $(CONFIG_mpp_vdec) $(CONFIG_MOD_CEDAR_ENABLE)),)
obj-$(CONFIG_MOD_CEDAR_ENABLE) += adapter/
obj-y += base/
obj-y += memory/
endif
obj-$(CONFIG_mpp_venc) += vencoder/base/
obj-$(CONFIG_mpp_venc) += vencoder/
obj-$(filter y, $(CONFIG_mpp_vdec) $(CONFIG_MOD_CEDAR_ENABLE)) += vdecoder/

