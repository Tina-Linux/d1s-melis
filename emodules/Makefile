subdir-ccflags-y +=

ifeq ($(CONFIG_MOD_SLIB), y)
endif

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

ifeq ($(CONFIG_MOD_UPDATE), y)
obj-m += mod_update/
endif

#add this judgement temporarily by swb-date-210305, could be change in the future
ifeq ($(CONFIG_MOD_FM), y)
obj-m += fm/
endif

ifeq ($(CONFIG_MOD_MIXTURE_ENABLE), y)
obj-m += mod_mixture/
endif

ifeq ($(CONFIG_MOD_JPEG_CODEC_ENABLE), y)
endif

ifeq ($(CONFIG_MOD_VIDEO_PLAY_ENABLE), y)
obj-m += mod_video_play/
endif