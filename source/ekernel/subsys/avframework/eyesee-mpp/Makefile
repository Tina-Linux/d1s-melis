ifeq ($(CONFIG_SOC_SUN8IW19P1), y)
chipDir := sun8iw19p1
else
  $(warning ===eyesee-mpp do not support this chip===)
endif

ifeq ($(CONFIG_SOC_SUN8IW19P1), y)
  subdir-ccflags-y += -DAWCHIP=0x1817#AW_V459, must keep sync with []plat_defines.h]
else
  $(warning ===eyesee-mpp do not support this chip 2===)
  subdir-ccflags-y += -DAWCHIP=0x1817#AW_V459
endif

ifeq ($(CONFIG_mpp_compile_static_lib), y)
  subdir-ccflags-y += -DADEC_CONFIG_COMPILE_STATIC_LIB=1
else
  subdir-ccflags-y += -DADEC_CONFIG_COMPILE_STATIC_LIB=0
endif

ifeq ($(CONFIG_imx317), y)
    subdir-ccflags-y += -DSENSOR_IMX317=1
else ifeq ($(CONFIG_imx278), y)
    subdir-ccflags-y += -DSENSOR_IMX278=1
else ifeq ($(CONFIG_imx386), y)
    subdir-ccflags-y += -DSENSOR_IMX386=1
else ifeq ($(CONFIG_sc2232), y)
    subdir-ccflags-y += -DSENSOR_SC2232=1
else ifeq ($(CONFIG_imx335), y)
    subdir-ccflags-y += -DSENSOR_IMX335=1
else ifeq ($(CONFIG_gc2053), y)
    subdir-ccflags-y += -DSENSOR_GC2053=1
else ifeq ($(CONFIG_c2398), y)
    subdir-ccflags-y += -DSENSOR_C2398=1
endif

ifeq ($(CONFIG_mpp_fwrite_method_vfs), y)
  subdir-ccflags-y += -DCDXCFG_FILE_SYSTEM=0#OPTION_FILE_SYSTEM_LINUX_VFS
else ifeq ($(CONFIG_mpp_fwrite_method_directIO),y)
  subdir-ccflags-y += -DCDXCFG_FILE_SYSTEM=2#OPTION_FILE_SYSTEM_DIRECT_IO
else
  subdir-ccflags-y += -DCDXCFG_FILE_SYSTEM=0
endif

ifeq ($(CONFIG_mpp_hw_display), y)
  subdir-ccflags-y += -DCDXCFG_HW_DISPLAY=1#OPTION_HW_DISPLAY_ENABLE
else
  subdir-ccflags-y += -DCDXCFG_HW_DISPLAY=0#OPTION_HW_DISPLAY_DISABLE
endif

ifeq ($(CONFIG_mpp_vi), y)
  subdir-ccflags-y += -DMPPCFG_VI=1#OPTION_VI_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_VI=0#OPTION_VI_DISABLE
endif

ifeq ($(CONFIG_mpp_vo), y)
  subdir-ccflags-y += -DMPPCFG_VO=1#OPTION_VO_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_VO=0#OPTION_VO_DISABLE
endif

ifeq ($(CONFIG_mpp_textenc), y)
  subdir-ccflags-y += -DMPPCFG_TEXTENC=1#OPTION_TEXTENC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_TEXTENC=0#OPTION_TEXTENC_DISABLE
endif

ifeq ($(CONFIG_mpp_venc), y)
  subdir-ccflags-y += -DMPPCFG_VENC=1#OPTION_VENC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_VENC=0#OPTION_VENC_DISABLE
endif

ifeq ($(CONFIG_mpp_vdec), y)
  subdir-ccflags-y += -DMPPCFG_VDEC=1#OPTION_VDEC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_VDEC=0#OPTION_VDEC_DISABLE
endif

ifeq ($(CONFIG_mpp_aio), y)
  subdir-ccflags-y += -DMPPCFG_AIO=1#OPTION_AIO_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_AIO=0#OPTION_AIO_DISABLE
endif

ifeq ($(CONFIG_mpp_aenc), y)
  subdir-ccflags-y += -DMPPCFG_AENC=1#OPTION_AENC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_AENC=0#OPTION_AENC_DISABLE
endif

ifeq ($(CONFIG_mpp_adec), y)
  subdir-ccflags-y += -DMPPCFG_ADEC=1#OPTION_ADEC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_ADEC=0#OPTION_ADEC_DISABLE
endif

ifeq ($(CONFIG_mpp_muxer), y)
  subdir-ccflags-y += -DMPPCFG_MUXER=1#OPTION_MUXER_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_MUXER=0#OPTION_MUXER_DISABLE
endif

ifeq ($(CONFIG_mpp_demuxer), y)
  subdir-ccflags-y += -DMPPCFG_DEMUXER=1#OPTION_DEMUXER_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_DEMUXER=0#OPTION_DEMUXER_DISABLE
endif

ifeq ($(CONFIG_mpp_ise), y)
  subdir-ccflags-y += -DMPPCFG_ISE=1#OPTION_ISE_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_ISE=0#OPTION_ISE_DISABLE
endif

ifeq ($(CONFIG_mpp_ise_mo), y)
  subdir-ccflags-y += -DMPPCFG_ISE_MO=1#OPTION_ISE_MO_ENABLE
else 
  subdir-ccflags-y += -DMPPCFG_ISE_MO=0#OPTION_ISE_MO_DISABLE
endif

ifeq ($(CONFIG_mpp_ise_gdc), y)
  subdir-ccflags-y += -DMPPCFG_ISE_GDC=1#OPTION_ISE_GDC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_ISE_GDC=0#OPTION_ISE_GDC_DISABLE
endif

ifeq ($(CONFIG_mpp_ise_bi), y)
  subdir-ccflags-y += -DMPPCFG_ISE_BI=1#OPTION_ISE_BI_ENABLE
else 
  subdir-ccflags-y += -DMPPCFG_ISE_BI=0#OPTION_ISE_BI_DISABLE
endif

ifeq ($(CONFIG_mpp_ise_bi_soft), y)
  subdir-ccflags-y += -DMPPCFG_ISE_BI_SOFT=1#OPTION_ISE_BI_SOFT_ENABLE
else 
  subdir-ccflags-y += -DMPPCFG_ISE_BI_SOFT=0#OPTION_ISE_BI_SOFT_DISABLE
endif

ifeq ($(CONFIG_mpp_ise_sti), y)
  subdir-ccflags-y += -DMPPCFG_ISE_STI=1#OPTION_ISE_STI_ENABLE
else 
  subdir-ccflags-y += -DMPPCFG_ISE_STI=0#OPTION_ISE_STI_DISABLE
endif

ifeq ($(CONFIG_mpp_eis), y)
  subdir-ccflags-y += -DMPPCFG_EIS=1#OPTION_EIS_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_EIS=0#OPTION_EIS_DISABLE
endif

ifeq ($(CONFIG_mpp_motion_detect_soft), y)
  subdir-ccflags-y += -DMPPCFG_MOTION_DETECT_SOFT=1#OPTION_MOTION_DETECT_SOFT_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_MOTION_DETECT_SOFT=0#OPTION_MOTION_DETECT_SOFT_DISABLE
endif

ifeq ($(CONFIG_mpp_vlpr), y)
  subdir-ccflags-y += -DMPPCFG_VLPR=1#OPTION_VLPR_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_VLPR=0#OPTION_VLPR_DISABLE
endif

ifeq ($(CONFIG_mpp_adas_detect), y)
  subdir-ccflags-y += -DMPPCFG_ADAS_DETECT=1#OPTION_ADAS_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_ADAS_DETECT=0#OPTION_ADAS_DISABLE
endif

ifeq ($(CONFIG_mpp_adas_detect_v2), y)
  subdir-ccflags-y += -DMPPCFG_ADAS_DETECT_V2=1#OPTION_ADAS_V2_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_ADAS_DETECT_V2=0#OPTION_ADAS_V2_DISABLE
endif

ifeq ($(CONFIG_mpp_bdii), y)
  subdir-ccflags-y += -DMPPCFG_BDII=1#OPTION_BDII_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_BDII=0#OPTION_BDII_DISABLE
endif

ifeq ($(CONFIG_mpp_eveface), y)
  subdir-ccflags-y += -DMPPCFG_EVEFACE=1#OPTION_EVEFACE_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_EVEFACE=0#OPTION_EVEFACE_DISABLE
endif

ifeq ($(CONFIG_mpp_aec), y)
  subdir-ccflags-y += -DMPPCFG_AEC=1#OPTION_AEC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_AEC=0#OPTION_AEC_DISABLE
endif

ifeq ($(CONFIG_mpp_softdrc), y)
  subdir-ccflags-y += -DMPPCFG_SOFTDRC=1#OPTION_SOFTDRC_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_SOFTDRC=0#OPTION_SOFTDRC_DISABLE
endif

ifeq ($(CONFIG_mpp_ans), y)
  subdir-ccflags-y += -DMPPCFG_ANS=1#OPTION_ANS_ENABLE
else
  subdir-ccflags-y += -DMPPCFG_ANS=0#OPTION_ANS_DISABLE
endif

ifeq ($(CONFIG_mpp_ans_libwebrtc), y)
  subdir-ccflags-y += -DMPPCFG_ANS_LIB=0#SELECT ANS LIBRARY FROM WEBRTC
endif

ifeq ($(CONFIG_mpp_ans_liblstm), y)
  subdir-ccflags-y += -DMPPCFG_ANS_LIB=1#SELECT ANS LIBRARY FROM LSTM
endif

ifeq ($(CONFIG_mpp_uvc), y)
  subdir-ccflags-y += -DMPPCFG_UVC
endif

#ifeq ($(CONFIG_mpp_iommu), y)
#  export USE_IOMMU := true#for libcedarc
#else
#  export USE_IOMMU := false#for libcedarc
#endif

ifeq ($(CONFIG_mpp_systrace), y)
  subdir-ccflags-y += -DMPPCFG_SYSTRACE
endif
obj-$(CONFIG_PACKAGE_eyesee_mpp_system) += system/public/
obj-$(CONFIG_PACKAGE_eyesee_mpp_middleware) += middleware/sun8iw19p1/
obj-$(CONFIG_PACKAGE_eyesee_mpp_framework) += framework/sun8iw19p1/
