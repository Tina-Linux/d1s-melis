COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/aplay.o
COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/arecord.o
# COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/aduplex.o
COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/amixer.o
COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/common.o
COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/wav_parser.o
COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/card.o
COMPONENTS_AW_ALSA_UTILS_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/audio_wav.o


$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):MODULE_NAME="components-aw-alsa-utils"

#$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):CFLAGS += -DAW_ALSA_DEBUG
$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):CFLAGS += -Wfatal-errors -Werror
$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):CFLAGS += -I $(BASE)/include/
$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):CFLAGS += -I $(BASE)/include/FreeRTOS_POSIX
$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(COMPONENTS_AW_ALSA_UTILS_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9/

OBJECTS += $(COMPONENTS_AW_ALSA_UTILS_OBJECTS)
