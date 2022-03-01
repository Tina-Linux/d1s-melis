
ifeq ($(CONFIG_COMPONENTS_AW_ALSA_LIB), y)
	include $(BASE)/drivers/hal/source/sound/component/aw-alsa-lib/objects.mk
endif
ifeq ($(CONFIG_COMPONENTS_AW_ALSA_UTILS), y)
	include $(BASE)/drivers/hal/source/sound/component/aw-alsa-utils/objects.mk
endif
