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


# The minimum set of files needed for lwIP.
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/lwip/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/core/ipv4/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/include/
# xr829 include files
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/kernel/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/kernel/os/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/kernel/os/RT-Thread/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/libc/sys/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/net/ethernetif/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/net/wlan/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/include/sys/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/lib/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/app/cmd/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/app/iperf/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/app/net_ctrl/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/app/net_ctrl/sys_ctrl/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/drv/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/kernel/os/RT-Thread/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/net/ethernetif/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/src/sys/
subdir-ccflags-y += 	-I$(srctree)/ekernel/drivers/drv/source/net/xr829/src/src/sys/mbuf/


subdir-ccflags-y += -D__CONFIG_OS_RTTHREAD \
		    -DLWIP_IPV4 \
		    -DLWIP_DHCP \
		    -DRT_LWIP_DHCP \
		    -DLWIP_NETIF_STATUS_CALLBACK \


obj-$(CONFIG_XR829) += src/app/net_ctrl/net_ctrl.o \
		      src/app/net_ctrl/net_init.o \
	   	      src/app/net_ctrl/net_sys.o

obj-$(CONFIG_XR829) += src/app/net_ctrl/sys_ctrl/container.o \
   		      src/app/net_ctrl/sys_ctrl/event_queue.o \
   		      src/app/net_ctrl/sys_ctrl/looper.o \
   		      src/app/net_ctrl/sys_ctrl/observer.o \
   		      src/app/net_ctrl/sys_ctrl/publisher.o \
   		      src/app/net_ctrl/sys_ctrl/sys_ctrl.o

#obj-$(CONFIG_XR829) +=src/drv/gpio.o
#obj-$(CONFIG_XR829) +=src/drv/sdio.o

obj-$(CONFIG_XR829) += src/net/ethernetif/ethernetif.o

obj-$(CONFIG_XR829) += src/kernel/os/RT-Thread/os_debug.o \
   		      src/kernel/os/RT-Thread/os_mutex.o \
   		      src/kernel/os/RT-Thread/os_queue.o \
   		      src/kernel/os/RT-Thread/os_semaphore.o \
   		      src/kernel/os/RT-Thread/os_thread.o \
   		      src/kernel/os/RT-Thread/os_timer.o


obj-$(CONFIG_XR829) += src/sys/atomic.o \
   		      src/sys/mbuf/mbuf_0.o \
   		      src/sys/mbuf/mbuf_0_mem.o

obj-$(CONFIG_XR829) += src/app/cmd/cmd_ifconfig.o \
   		      src/app/cmd/cmd_wlan.o \
   		      src/app/cmd/cmd_util.o \
   		      src/app/iperf/iperf.o \
   		      src/app/cmd/cmd_iperf.o

#obj-$(CONFIG_XR829) +=src/app/cmd/command.o
