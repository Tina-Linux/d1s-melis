subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/include/ipv4/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/arch/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/websocket/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/compat/posix
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/include/

#network app ping
#obj-$(CONFIG_PING) +=ping/ping.o \

#network app iperf
#obj-$(CONFIG_IPERF) +=lwiperf/lwiperf.o \

#network app sntp
obj-$(CONFIG_SNTP) +=sntp/sntp.o \

#network app smtp
obj-$(CONFIG_SMTP) +=smtp/smtp.o \

#network app mdns
obj-$(CONFIG_MDNS) +=mdns/mdns.o \

#network app tftp
obj-$(CONFIG_TFTP) +=tftp/tftp_server.o \

#network app mqtt
obj-$(CONFIG_MQTT) +=mqtt/mqtt.o \

#network app tls
obj-$(CONFIG_ALTCP_TLS) +=altcp_tls/altcp_tls_mbedtls.o \
    	altcp_tls/altcp_tls_mbedtls_mem.o \

#network app snmp
obj-$(CONFIG_SNMP) +=snmp/snmp_asn1.o \
    	snmp/snmpv3_mbedtls.o \
    	snmp/snmp_core.o \
    	snmp/snmp_mib2.o \
    	snmp/snmp_mib2_icmp.o \
    	snmp/snmp_mib2_interfaces.o \
    	snmp/snmp_mib2_ip.o \
    	snmp/snmp_mib2_snmp.o \
    	snmp/snmp_mib2_system.o \
    	snmp/snmp_mib2_tcp.o \
    	snmp/snmp_mib2_udp.o \
    	snmp/snmp_snmpv2_framework.o \
    	snmp/snmp_snmpv2_usm.o \
    	snmp/snmp_msg.o \
    	snmp/snmpv3.o \
    	snmp/snmp_netconn.o \
    	snmp/snmp_pbuf_stream.o \
    	snmp/snmp_raw.o \
    	snmp/snmp_scalar.o \
    	snmp/snmp_table.o \
    	snmp/snmp_threadsync.o \
    	snmp/snmp_traps.o \

#network app netbios
obj-$(CONFIG_NETBIOSNS) +=netbiosns/netbiosns.o \

#network app websocket
obj-$(CONFIG_WEBSOCKET) +=websocket/

#network app mbedtls
obj-$(CONFIG_MBEDTLS) +=mbedtls/

#network app cyassl
obj-$(CONFIG_CYASSL) +=cyassl/

#network app HTTP server + client
obj-$(CONFIG_HTTP) +=http/altcp_proxyconnect.o \
    http/fs.o \
    http/http_client.o \
    http/httpd.o \
