ifeq ($(CONFIG_SOC_SUN8IW19P1), y)
  subdir-ccflags-y += -DCONF_NEW_BDMV_STREAM \
                    -DCONF_H265_4K \
                    -DCONF_KERNEL_VER=310 \
                    -DCONF_GPU_MALI \
                    -DCONF_KERN_BITWIDE=32 \
                    -DCONF_VE_PHY_OFFSET=0x40000000 \
                    -Wall \
                    -DAWP_DEBUG
else
  $(warning ===eyesee-mpp do not support this chip. libcedarx/libcore/base===)
endif

obj-y += \
    AwMessageQueue.o \
    AwPool.o \
    cdx_log.o \
    CdxBitReader.o \
    CdxBuffer.o \
    CdxDebug.o \
    CdxISOLang.o \
    CdxKeyedVector.o \
    CdxList.o \
    CdxMessage.o \
    CdxMeta.o \
    CdxParseUrl.o \
    CdxQueue.o \
    CdxSocketUtil.o \
    CdxStrUtil.o \
    CdxSysinfo.o \
    CdxTime.o \
    SmartDnsService.o