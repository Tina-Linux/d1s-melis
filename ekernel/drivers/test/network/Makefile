LWIP_PATH = $(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip

subdir-ccflags-y += 	-I$(LWIP_PATH)/src/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/arch/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/core/
subdir-ccflags-y +=     -I$(LWIP_PATH)/src/include/lwip/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/include/ipv4/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/arch/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/apps/websocket/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/include/compat/posix
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/apps/mbedtls/include/

obj-$(CONFIG_LOOP_TEST) += loopif_test/loopif_test.o \
		           loopif_test/socket_test/loopif_socket_test.o \

obj-$(CONFIG_SOCKET_TEST) += socket/service.o \
	                   socket/client.o \

obj-$(CONFIG_TCPIP_TEST) += tcpip/tcpip_adapter.o
