obj-y += hal_psramctrl.o \
	 psram_core.o \
	 psram_opi32.o \
	 psram_opi64.o

ifeq ($(CONFIG_KERNEL_FREERTOS), y)
CFLAGS += -I $(BASE)/include/hal/sdmmc/hal
endif
