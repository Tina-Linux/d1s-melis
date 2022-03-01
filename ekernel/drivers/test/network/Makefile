subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/core/
subdir-ccflags-y +=     -I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/lwip/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/include/ipv4/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/websocket/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/compat/posix
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/mbedtls/include/

obj-$(CONFIG_LOOP_TEST) += loopif_test/loopif_test.o \
		           loopif_test/socket_test/loopif_socket_test.o \

obj-$(CONFIG_SOCKET_TEST) += socket/service.o \
	                   socket/client.o \

obj-$(CONFIG_TCPIP_TEST) += tcpip/tcpip_adapter.o
