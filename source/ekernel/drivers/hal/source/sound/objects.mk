DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_core.o
DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_dma.o
DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_pcm.o
DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_misc.o
DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_io.o
DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_ctl.o
DRIVERS_SOUND_OBJECTS += $(BUILD_DIR)/drivers/hal/source/sound/core/snd_params.o

include $(BASE)/drivers/hal/source/sound/codecs/objects.mk
include $(BASE)/drivers/hal/source/sound/platform/objects.mk
include $(BASE)/drivers/hal/source/sound/component/objects.mk

$(DRIVERS_SOUND_OBJECTS):MODULE_NAME="drivers-sound"

#$(DRIVERS_SOUND_OBJECTS):CFLAGS += -DSNDRV_DEBUG
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -Werror -Wfatal-errors
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/drivers/hal/source/sound/core
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/include/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/include/arch/mach/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/include/freertos/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/port/
$(DRIVERS_SOUND_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9

OBJECTS += $(DRIVERS_SOUND_OBJECTS)
