#CODEC
DRIVERS_SOUND_CODECS_OBJECTS-$(CONFIG_SND_CODEC_SUNXI_RWFUNC) += $(BUILD_DIR)/drivers/hal/source/sound/codecs/sunxi_rw_func.o
DRIVERS_SOUND_CODECS_OBJECTS-$(CONFIG_SND_CODEC_DUMMY) += $(BUILD_DIR)/drivers/hal/source/sound/codecs/dummy_codec.o
DRIVERS_SOUND_CODECS_OBJECTS-$(CONFIG_SND_CODEC_AC108) += $(BUILD_DIR)/drivers/hal/source/sound/codecs/ac108.o
DRIVERS_SOUND_CODECS_OBJECTS-$(CONFIG_SND_CODEC_SUN8IW18_AUDIOCODEC) += $(BUILD_DIR)/drivers/hal/source/sound/codecs/sun8iw18-codec.o
DRIVERS_SOUND_CODECS_OBJECTS-$(CONFIG_SND_CODEC_SUN8IW19_AUDIOCODEC) += $(BUILD_DIR)/drivers/hal/source/sound/codecs/sun8iw19-codec.o


$(DRIVERS_SOUND_CODECS_OBJECTS-y):MODULE_NAME="drivers-sound"

#$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -DSNDRV_DEBUG
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -Werror -Wfatal-errors
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/drivers/chip-src/sound/core
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/include/
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/include/drivers/
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/include/freertos/
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/port/
$(DRIVERS_SOUND_CODECS_OBJECTS-y):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9

OBJECTS += $(DRIVERS_SOUND_CODECS_OBJECTS-y)
