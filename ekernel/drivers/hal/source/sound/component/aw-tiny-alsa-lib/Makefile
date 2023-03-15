obj-y += pcm.o
obj-y += control.o
obj-y += pcm_config.o
obj-y += pcm_hw.o
obj-y += pcm_mmap.o

ifneq ($(CONFIG_KERNEL_FREERTOS), y)
obj-y += alsa_config.o
endif
ifeq ($(CONFIG_KERNEL_FREERTOS)_$(CONFIG_ARCH_DSP), y_y)
obj-y += alsa_config.o
endif

ifeq ($(CONFIG_KERNEL_FREERTOS), y)
CFLAGS += -I $(BASE)/include/
CFLAGS += -I $(BASE)/include/hal/sound/
endif
