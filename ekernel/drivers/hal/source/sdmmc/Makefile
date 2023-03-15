
ifeq ($(CONFIG_OS_MELIS), y)
INC_PREFIX = drivers/
subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc/osal/RT-Thread
endif

ifeq ($(CONFIG_KERNEL_FREERTOS), y)
INC_PREFIX =
subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc/osal/FreeRTOS
endif

subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc
subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc/hal
subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc/sys
subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc/osal
subdir-ccflags-y += -I $(INC_PREFIX)include/hal/sdmmc/os
subdir-ccflags-y += -Werror

obj-y += osal/
obj-y += core.o
obj-y += hal_sdpin.o
obj-y += hal_sdhost.o
obj-y += mmc.o
obj-y += quirks.o
obj-y += sd.o
obj-y += sdio.o
obj-y += test.o
obj-y += sdio_irq.o
obj-y += cmd/cmd_sd.o
