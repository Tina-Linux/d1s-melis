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
#subdir-ccflags-y += -DLWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT=20000
#subdir-ccflags-y += -DLWIP_SO_LINGER=1
#subdir-ccflags-y += -DSO_REUSE=1
#subdir-ccflags-y += -DLWIP_SO_SNDTIMEO=1
#subdir-ccflags-y += -DLWIP_SO_RCVTIMEO=1
#subdir-ccflags-y += -DLWIP_PROVIDE_ERRNO=1

LWIP_PATH = $(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip

subdir-ccflags-y += 	-I$(LWIP_PATH)/src/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/include/ipv4/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/arch/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/apps/websocket/include/
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/include/compat/posix
subdir-ccflags-y += 	-I$(LWIP_PATH)/src/apps/mbedtls/include/

obj-$(CONFIG_LWIP) +=src/apps/

obj-$(CONFIG_LWIP) +=src/core/init.o \
    	src/core/def.o \
    	src/core/dns.o \
    	src/core/inet_chksum.o \
    	src/core/ip.o \
    	src/core/mem.o \
    	src/core/memp.o \
    	src/core/netif.o \
    	src/core/pbuf.o \
    	src/core/raw.o \
    	src/core/stats.o \
    	src/core/sys.o \
    	src/core/altcp.o \
    	src/core/altcp_alloc.o \
    	src/core/altcp_tcp.o \
    	src/core/tcp.o \
    	src/core/tcp_in.o \
    	src/core/tcp_out.o \
    	src/core/timeouts.o \
    	src/core/udp.o \

obj-$(CONFIG_LWIP) +=src/core/ipv4/autoip.o \
    	src/core/ipv4/dhcp.o \
    	src/core/ipv4/etharp.o \
    	src/core/ipv4/icmp.o \
    	src/core/ipv4/igmp.o \
    	src/core/ipv4/ip4_frag.o \
    	src/core/ipv4/ip4.o \
    	src/core/ipv4/ip4_addr.o \

obj-$(CONFIG_LWIP) +=src/core/ipv6/dhcp6.o \
    	src/core/ipv6/ethip6.o \
    	src/core/ipv6/icmp6.o \
    	src/core/ipv6/inet6.o \
    	src/core/ipv6/ip6.o \
    	src/core/ipv6/ip6_addr.o \
    	src/core/ipv6/ip6_frag.o \
    	src/core/ipv6/mld6.o \
    	src/core/ipv6/nd6.o \

obj-$(CONFIG_LWIP) +=src/arch/sys_arch.o \

# APIFILES: The files which implement the sequential and socket APIs.
obj-$(CONFIG_LWIP) +=src/api/api_lib.o \
    	src/api/api_msg.o \
    	src/api/err.o \
    	src/api/if_api.o \
    	src/api/netbuf.o \
    	src/api/netdb.o \
    	src/api/netifapi.o \
    	src/api/sockets.o \
    	src/api/tcpip.o \

# Files implementing various generic network interface functions
obj-$(CONFIG_LWIP) +=src/netif/ethernet.o \
    	src/netif/bridgeif.o \
    	src/netif/bridgeif_fdb.o \
    	src/netif/slipif.o \

# 6LoWPAN
obj-$(CONFIG_LWIP) +=src/netif/lowpan6_common.o \
    	src/netif/lowpan6.o \
    	src/netif/lowpan6_ble.o \
    	src/netif/zepif.o \

# PPP
obj-$(CONFIG_LWIP) +=src/netif/ppp/auth.o \
    	src/netif/ppp/ccp.o \
    	src/netif/ppp/chap-md5.o \
    	src/netif/ppp/chap_ms.o \
    	src/netif/ppp/chap-new.o \
    	src/netif/ppp/demand.o \
    	src/netif/ppp/eap.o \
    	src/netif/ppp/ecp.o \
    	src/netif/ppp/eui64.o \
    	src/netif/ppp/fsm.o \
    	src/netif/ppp/ipcp.o \
    	src/netif/ppp/ipv6cp.o \
    	src/netif/ppp/lcp.o \
    	src/netif/ppp/magic.o \
    	src/netif/ppp/mppe.o \
    	src/netif/ppp/multilink.o \
    	src/netif/ppp/ppp.o \
    	src/netif/ppp/pppapi.o \
    	src/netif/ppp/pppcrypt.o \
    	src/netif/ppp/pppoe.o \
    	src/netif/ppp/pppol2tp.o \
    	src/netif/ppp/pppos.o \
    	src/netif/ppp/upap.o \
    	src/netif/ppp/utils.o \
    	src/netif/ppp/vj.o \
    	src/netif/ppp/polarssl/arc4.o \
    	src/netif/ppp/polarssl/des.o \
    	src/netif/ppp/polarssl/md4.o \
    	src/netif/ppp/polarssl/md5.o \
    	src/netif/ppp/polarssl/sha1.o \
