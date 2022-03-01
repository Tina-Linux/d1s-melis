subdir-ccflags-y +=

ifeq ($(CONFIG_MOD_CEDAR_ENABLE), y)
endif
ifeq ($(CONFIG_MOD_WILLOW_ENABLE), y)
endif

ifeq ($(CONFIG_MOD_DISPLAY_ENABLE), y)
obj-m += mod_display/
endif

ifeq ($(CONFIG_MOD_ORANGE_ENABLE), y)
obj-m += mod_orange/
endif

ifeq ($(CONFIG_MOD_CHARSET_ENABLE), y)
obj-m += mod_charset/
endif

ifeq ($(CONFIG_FINSH_EXTERN_SCRIPT), y)
obj-m += mod_test/
endif

#add this judgement temporarily by swb-date-210305, could be change in the future
ifeq ($(CONFIG_SOC_SUN20IW1), y)
obj-m += fm/
endif

ifeq ($(CONFIG_MOD_MIXTURE_ENABLE), y)
obj-m += mod_mixture/
endif

ifeq ($(CONFIG_MOD_JPEG_CODEC_ENABLE), y)
obj-m += mod_jpeg_dec/
endif