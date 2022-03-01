COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/control.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_config.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_hw.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_direct.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_dsnoop.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_dmix.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_asym.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_mmap.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_generic.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_plugin_generic.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_route.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_softvol.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_misc.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_linear.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_extplug.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_plug.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_rate.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_file.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_multi.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_params.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/pcm_rate_linear.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/external_resample/speexrate/rate_speexrate.o
COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-lib/external_resample/speexrate/resample_speexdsp.o
#COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/components/aw/sound/aw-alsa-lib/external_resample/awrate/doResample.o
#COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/components/aw/sound/aw-alsa-lib/external_resample/awrate/rate_awrate.o
#COMPONENTS_AW_ALSA_LIB_OBJECTS += $(BUILD_DIR)/components/aw/sound/aw-alsa-lib/external_resample/awrate/audiomix.o


ifeq ($(CONFIG_COMPONENTS_USB_GADGET_ADB_FORWARD),y)
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -DSUPPORT_ADB_FORWARD
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -I $(BASE)/components/thirdparty/network/lwip/src/include
endif

ifeq ($(CONFIG_AW_ALSA_PLUGINS_SONA_AUDIOAEF), y)
COMPONENTS_AW_ALSA_LIB_OBJECTS += \
	$(BUILD_DIR)/components/aw/sound/aw-alsa-lib/thirdparty/sona_audioaef/pcm_sona_audioaef.o
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -DAW_ALSA_PLUGINS_SONA_AUDIOAEF
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -I $(BASE)/components/thirdparty/sona_audioaef
LDFLAGS += -L$(BASE)/components/thirdparty/sona_audioaef/lib
LDFLAGS += -lsonaaef
endif

$(COMPONENTS_AW_ALSA_LIB_OBJECTS):MODULE_NAME="components-aw-alsa-lib"

#$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -DAW_ALSA_LIB_API_TEST

#$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -DAW_ALSA_DEBUG
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -Wfatal-errors -Werror
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -I $(BASE)/include/
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9/
$(COMPONENTS_AW_ALSA_LIB_OBJECTS):CFLAGS += -I $(BASE)/components/aw/sound/aw-alsa-lib/

OBJECTS += $(COMPONENTS_AW_ALSA_LIB_OBJECTS)
