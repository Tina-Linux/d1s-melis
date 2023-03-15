asflags-y += -I$(srctree)/ekernel/drivers/hal/source/sound/component/aw-alsa-utils \
             -DCONFIG_BUILTIN_WAV_FILE

obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS) += common.o
obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS) += wav_parser.o
obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS) += card.o
obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS_APLAY) += aplay.o
obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS_ARECORD) += arecord.o
obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS_AMIXER) += amixer.o
obj-$(CONFIG_COMPONENTS_AW_ALSA_UTILS_ALOOP) += aloop.o
obj-$(CONFIG_BUILTIN_WAV_FILE) += audio_wav/audio_wav.o
