subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/aw/multi_console/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/include/ipv4/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/websocket/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/compat/posix
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/mbedtls/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/mbedtls/include/mbedtls/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/mbedtls/ports/inc/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/optparse-master
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/webclient/inc/

EXTRA_CFLAGS += -Wno-date-time
obj-y +=optparse-master/optparse.o
obj-$(CONFIG_NETTOOLS_OPTPARSE) += optparse-master/example/optparse_demo.o

#network app libcurl
obj-$(CONFIG_LIBCURL) +=libcurl/
obj-$(CONFIG_LIBCURL2RTT) +=libcurl2rtt-latest/

#network app ping
obj-$(CONFIG_PING) +=ping/ping.o \

#network app wget
obj-$(CONFIG_WGET) +=mini_wget/download_mult.o \
	             mini_wget/download_single.o \

#network app webclient
obj-$(CONFIG_WEBCLIENT) +=webclient/src/webclient.o \
			  webclient/src/webclient_file.o \

obj-$(CONFIG_WEBCLIENT_TEST) +=webclient/samples/webclient_get_sample.o \
			       webclient/samples/webclient_post_sample.o \
			       webclient/samples/webclient_download_sample.o \
			       webclient/samples/webclient_upload_sample.o \

#network app iperf
obj-$(CONFIG_IPERF) +=iperf/iperf.o \

#network app ntp
obj-$(CONFIG_NTP) +=ntp/ntp.o \

#network app netio
obj-$(CONFIG_NETIO) +=netio/netio.o \

#network app tcpdump
obj-$(CONFIG_TCPDUMP) +=tcpdump/tcpdump.o \

#network app telnet
obj-$(CONFIG_TELNET) +=telnet/telnet.o telnet/telnet_console.o \

#network app tftp
obj-$(CONFIG_TFTP) +=tftp/tftp_client.o \
    				  tftp/tftp_server.o \
    				  tftp/tftp_port.o \
    				  tftp/tftp_xfer.o \

#network app httpclient
obj-$(CONFIG_HTTPCLIENT) +=httpclient/user_http.o \
