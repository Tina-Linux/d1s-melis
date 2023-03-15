# This file is indended to be included in end-user CMakeLists.txt
# include(/path/to/Filelists.o \make)
# It assumes the variable srctree is defined pointing to the
# root path of lwIP sources.
#
# This file is NOT designed (on purpose) to be used as cmake
# subdir via add_subdirectory()
# The intention is to provide greater flexibility to users to
# create their own targets using the *_srcS variables.

# LWIP_VERSION_RC is set to LWIP_RC_RELEASE for official releases
# LWIP_VERSION_RC is set to LWIP_RC_DEVELOPMENT for Git versions
# Numbers 1..31 are reserved for release candidates
#
LWIP_PATH = $(srctree)/ekernel/components/thirdparth/net/rt-thread/lwip
SDMMC_INC_PATH = $(srctree)/ekernel/drivers/include/hal/sdmmc

# sdmmc
subdir-ccflags-y += 	-I$(SDMMC_INC_PATH)/
subdir-ccflags-y += 	-I$(SDMMC_INC_PATH)/sys/
subdir-ccflags-y += 	-I$(SDMMC_INC_PATH)/hal
subdir-ccflags-y += 	-I$(SDMMC_INC_PATH)/osal
subdir-ccflags-y += 	-I$(SDMMC_INC_PATH)/os
subdir-ccflags-y += 	-I$(SDMMC_INC_PATH)/osal/RT-Thread
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/hal/source/sdmmc

# The minimum set of files needed for lwIP.
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/include/lwip/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/core/ipv4/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/arch/include/

subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/brcm_wifi_libs/inc

subdir-ccflags-y += -DMHD_WLCMD -DMHD_JOIN_STATE -DMHD_BUS_AWAKE

#subdir-ccflags-y += -DUSE_STATIC_IP
#subdir-ccflags-y += -DUSE_POLLING_MODE


### MHD test ###
subdir-ccflags-y += -DMHD_API_RELEASE -DMHD_API_TEST -DMHD_PING_TEST -DMHD_IPERF_TEST

obj-$(CONFIG_AP6203) += src/bcm_wifi.o \
			src/bcm_wifi_test.o \
			src/error_codes.o \
			src/bcm_cmd.o \
			src/ifconfig.o \
			src/host_gpio.o \
			src/nvram_ap6203bm.o \
			src/sdio_wrappers.o \
			libs/liblwip_mhd_rtthread.a

