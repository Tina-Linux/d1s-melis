#***************************************************************************
#                                  _   _ ____  _
#  Project                     ___| | | |  _ \| |
#                             / __| | | | |_) | |
#                            | (__| |_| |  _ <| |___
#                             \___|\___/|_| \_\_____|
#
# Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at https://curl.haxx.se/docs/copyright.html.
#
# You may opt to use, copy, modify, merge, publish, distribute and/or sell
# copies of the Software, and permit persons to whom the Software is
# furnished to do so, under the terms of the COPYING file.
#
# This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
# KIND, either express or implied.
#
###########################################################################

#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/lwip
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/include/
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/include/ipv4/
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/include/
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/websocket/include/
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/compat/posix
#subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/apps/mbedtls/include/

subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/include/curl/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/lib/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/lib/vauth/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/lib/vquic/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/lib/vssh/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/lib/vtls/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/curl/curl_v7.67.0/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/libssh2/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl2rtt-latest/libssh2/include/

subdir-ccflags-y += 	\
			-DHAVE_CONFIG_H \

obj-$(CONFIG_LIBCURL2RTT) += \
			    ./curl/curl_v7.67.0/src/tool_convert.o \
			    ./curl/curl_v7.67.0/src/tool_urlglob.o \
			    ./curl/curl_v7.67.0/src/tool_getparam.o \
			    ./curl/curl_v7.67.0/src/tool_main.o \
			    ./curl/curl_v7.67.0/src/tool_doswin.o \
			    ./curl/curl_v7.67.0/src/tool_parsecfg.o \
			    ./curl/curl_v7.67.0/src/tool_dirhie.o \
			    ./curl/curl_v7.67.0/src/tool_util.o \
			    ./curl/curl_v7.67.0/src/tool_strdup.o \
			    ./curl/curl_v7.67.0/src/tool_metalink.o \
			    ./curl/curl_v7.67.0/src/tool_cb_wrt.o \
			    ./curl/curl_v7.67.0/src/tool_setopt.o \
			    ./curl/curl_v7.67.0/src/tool_cfgable.o \
			    ./curl/curl_v7.67.0/src/tool_cb_hdr.o \
			    ./curl/curl_v7.67.0/src/tool_bname.o \
			    ./curl/curl_v7.67.0/src/tool_vms.o \
			    ./curl/curl_v7.67.0/src/tool_paramhlp.o \
			    ./curl/curl_v7.67.0/src/tool_cb_rea.o \
			    ./curl/curl_v7.67.0/src/tool_homedir.o \
			    ./curl/curl_v7.67.0/src/slist_wc.o \
			    ./curl/curl_v7.67.0/src/tool_helpers.o \
			    ./curl/curl_v7.67.0/src/tool_cb_dbg.o \
			    ./curl/curl_v7.67.0/src/tool_writeout.o \
			    ./curl/curl_v7.67.0/src/tool_cb_see.o \
			    ./curl/curl_v7.67.0/src/tool_progress.o \
			    ./curl/curl_v7.67.0/src/tool_filetime.o \
			    ./curl/curl_v7.67.0/src/tool_sleep.o \
			    ./curl/curl_v7.67.0/src/tool_getpass.o \
			    ./curl/curl_v7.67.0/src/tool_xattr.o \
			    ./curl/curl_v7.67.0/src/tool_cb_prg.o \
			    ./curl/curl_v7.67.0/src/tool_easysrc.o \
			    ./curl/curl_v7.67.0/src/tool_libinfo.o \
			    ./curl/curl_v7.67.0/src/tool_help.o \
			    ./curl/curl_v7.67.0/src/tool_msgs.o \
			    ./curl/curl_v7.67.0/src/tool_hugehelp.o \
			    ./curl/curl_v7.67.0/src/tool_formparse.o \
			    ./curl/curl_v7.67.0/src/tool_binmode.o \
			    ./curl/curl_v7.67.0/src/tool_operhlp.o \
			    ./curl/curl_v7.67.0/src/tool_panykey.o \
			    ./curl/curl_v7.67.0/src/tool_operate.o \
			    ./curl/curl_v7.67.0/lib/escape.o \
			    ./curl/curl_v7.67.0/lib/transfer.o \
			    ./curl/curl_v7.67.0/lib/krb5.o \
			    ./curl/curl_v7.67.0/lib/nwlib.o \
			    ./curl/curl_v7.67.0/lib/vquic/ngtcp2.o \
			    ./curl/curl_v7.67.0/lib/vquic/quiche.o \
			    ./curl/curl_v7.67.0/lib/rtsp.o \
			    ./curl/curl_v7.67.0/lib/md4.o \
			    ./curl/curl_v7.67.0/lib/base64.o \
			    ./curl/curl_v7.67.0/lib/setopt.o \
			    ./curl/curl_v7.67.0/lib/strcase.o \
			    ./curl/curl_v7.67.0/lib/curl_ntlm_wb.o \
			    ./curl/curl_v7.67.0/lib/system_win32.o \
			    ./curl/curl_v7.67.0/lib/openldap.o \
			    ./curl/curl_v7.67.0/lib/curl_ntlm_core.o \
			    ./curl/curl_v7.67.0/lib/getenv.o \
			    ./curl/curl_v7.67.0/lib/parsedate.o \
			    ./curl/curl_v7.67.0/lib/ldap.o \
			    ./curl/curl_v7.67.0/lib/conncache.o \
			    ./curl/curl_v7.67.0/lib/hostip.o \
			    ./curl/curl_v7.67.0/lib/altsvc.o \
			    ./curl/curl_v7.67.0/lib/x509asn1.o \
			    ./curl/curl_v7.67.0/lib/curl_multibyte.o \
			    ./curl/curl_v7.67.0/lib/strtok.o \
			    ./curl/curl_v7.67.0/lib/select.o \
			    ./curl/curl_v7.67.0/lib/pop3.o \
			    ./curl/curl_v7.67.0/lib/curl_gssapi.o \
			    ./curl/curl_v7.67.0/lib/curl_sasl.o \
			    ./curl/curl_v7.67.0/lib/http_negotiate.o \
			    ./curl/curl_v7.67.0/lib/smtp.o \
			    ./curl/curl_v7.67.0/lib/sendf.o \
			    ./curl/curl_v7.67.0/lib/slist.o \
			    ./curl/curl_v7.67.0/lib/curl_get_line.o \
			    ./curl/curl_v7.67.0/lib/http_digest.o \
			    ./curl/curl_v7.67.0/lib/curl_threads.o \
			    ./curl/curl_v7.67.0/lib/tftp.o \
			    ./curl/curl_v7.67.0/lib/doh.o \
			    ./curl/curl_v7.67.0/lib/timeval.o \
			    ./curl/curl_v7.67.0/lib/amigaos.o \
			    ./curl/curl_v7.67.0/lib/socks_sspi.o \
			    ./curl/curl_v7.67.0/lib/smb.o \
			    ./curl/curl_v7.67.0/lib/netrc.o \
			    ./curl/curl_v7.67.0/lib/cookie.o \
			    ./curl/curl_v7.67.0/lib/sha256.o \
			    ./curl/curl_v7.67.0/lib/memdebug.o \
			    ./curl/curl_v7.67.0/lib/nonblock.o \
			    ./curl/curl_v7.67.0/lib/curl_path.o \
			    ./curl/curl_v7.67.0/lib/content_encoding.o \
			    ./curl/curl_v7.67.0/lib/fileinfo.o \
			    ./curl/curl_v7.67.0/lib/curl_ctype.o \
			    ./curl/curl_v7.67.0/lib/vtls/nss.o \
			    ./curl/curl_v7.67.0/lib/vtls/schannel.o \
			    ./curl/curl_v7.67.0/lib/vtls/vtls.o \
			    ./curl/curl_v7.67.0/lib/vtls/schannel_verify.o \
			    ./curl/curl_v7.67.0/lib/vtls/gskit.o \
			    ./curl/curl_v7.67.0/lib/vtls/sectransp.o \
			    ./curl/curl_v7.67.0/lib/vtls/gtls.o \
			    ./curl/curl_v7.67.0/lib/vtls/polarssl_threadlock.o \
			    ./curl/curl_v7.67.0/lib/vtls/wolfssl.o \
			    ./curl/curl_v7.67.0/lib/vtls/polarssl.o \
			    ./curl/curl_v7.67.0/lib/vtls/openssl.o \
			    ./curl/curl_v7.67.0/lib/vtls/mesalink.o \
			    ./curl/curl_v7.67.0/lib/vtls/mbedtls.o \
			    ./curl/curl_v7.67.0/lib/progress.o \
			    ./curl/curl_v7.67.0/lib/http2.o \
			    ./curl/curl_v7.67.0/lib/curl_des.o \
			    ./curl/curl_v7.67.0/lib/connect.o \
			    ./curl/curl_v7.67.0/lib/hostip4.o \
			    ./curl/curl_v7.67.0/lib/http_ntlm.o \
			    ./curl/curl_v7.67.0/lib/vssh/libssh2.o \
			    ./curl/curl_v7.67.0/lib/vssh/libssh.o \
			    ./curl/curl_v7.67.0/lib/gopher.o \
			    ./curl/curl_v7.67.0/lib/hash.o \
			    ./curl/curl_v7.67.0/lib/idn_win32.o \
			    ./curl/curl_v7.67.0/lib/if2ip.o \
			    ./curl/curl_v7.67.0/lib/version.o \
			    ./curl/curl_v7.67.0/lib/http.o \
			    ./curl/curl_v7.67.0/lib/curl_gethostname.o \
			    ./curl/curl_v7.67.0/lib/asyn-ares.o \
			    ./curl/curl_v7.67.0/lib/hostsyn.o \
			    ./curl/curl_v7.67.0/lib/curl_endian.o \
			    ./curl/curl_v7.67.0/lib/speedcheck.o \
			    ./curl/curl_v7.67.0/lib/psl.o \
			    ./curl/curl_v7.67.0/lib/curl_range.o \
			    ./curl/curl_v7.67.0/lib/telnet.o \
			    ./curl/curl_v7.67.0/lib/dict.o \
			    ./curl/curl_v7.67.0/lib/nwos.o \
			    ./curl/curl_v7.67.0/lib/inet_pton.o \
			    ./curl/curl_v7.67.0/lib/multi.o \
			    ./curl/curl_v7.67.0/lib/wildcard.o \
			    ./curl/curl_v7.67.0/lib/strtoofft.o \
			    ./curl/curl_v7.67.0/lib/splay.o \
			    ./curl/curl_v7.67.0/lib/dotdot.o \
			    ./curl/curl_v7.67.0/lib/share.o \
			    ./curl/curl_v7.67.0/lib/warnless.o \
			    ./curl/curl_v7.67.0/lib/rand.o \
			    ./curl/curl_v7.67.0/lib/curl_memrchr.o \
			    ./curl/curl_v7.67.0/lib/strerror.o \
			    ./curl/curl_v7.67.0/lib/ftp.o \
			    ./curl/curl_v7.67.0/lib/curl_rtmp.o \
			    ./curl/curl_v7.67.0/lib/http_proxy.o \
			    ./curl/curl_v7.67.0/lib/asyn-thread.o \
			    ./curl/curl_v7.67.0/lib/imap.o \
			    ./curl/curl_v7.67.0/lib/formdata.o \
			    ./curl/curl_v7.67.0/lib/pingpong.o \
			    ./curl/curl_v7.67.0/lib/socketpair.o \
			    ./curl/curl_v7.67.0/lib/url.o \
			    ./curl/curl_v7.67.0/lib/hostcheck.o \
			    ./curl/curl_v7.67.0/lib/security.o \
			    ./curl/curl_v7.67.0/lib/curl_addrinfo.o \
			    ./curl/curl_v7.67.0/lib/hostip6.o \
			    ./curl/curl_v7.67.0/lib/file.o \
			    ./curl/curl_v7.67.0/lib/non-ascii.o \
			    ./curl/curl_v7.67.0/lib/mprintf.o \
			    ./curl/curl_v7.67.0/lib/http_chunks.o \
			    ./curl/curl_v7.67.0/lib/ftplistparser.o \
			    ./curl/curl_v7.67.0/lib/strdup.o \
			    ./curl/curl_v7.67.0/lib/socks.o \
			    ./curl/curl_v7.67.0/lib/urlapi.o \
			    ./curl/curl_v7.67.0/lib/curl_sspi.o \
			    ./curl/curl_v7.67.0/lib/mime.o \
			    ./curl/curl_v7.67.0/lib/curl_fnmatch.o \
			    ./curl/curl_v7.67.0/lib/inet_ntop.o \
			    ./curl/curl_v7.67.0/lib/getinfo.o \
			    ./curl/curl_v7.67.0/lib/hmac.o \
			    ./curl/curl_v7.67.0/lib/hostasyn.o \
			    ./curl/curl_v7.67.0/lib/socks_gssapi.o \
			    ./curl/curl_v7.67.0/lib/vauth/digest.o \
			    ./curl/curl_v7.67.0/lib/vauth/spnego_gssapi.o \
			    ./curl/curl_v7.67.0/lib/vauth/ntlm.o \
			    ./curl/curl_v7.67.0/lib/vauth/cleartext.o \
			    ./curl/curl_v7.67.0/lib/vauth/vauth.o \
			    ./curl/curl_v7.67.0/lib/vauth/ntlm_sspi.o \
			    ./curl/curl_v7.67.0/lib/vauth/spnego_sspi.o \
			    ./curl/curl_v7.67.0/lib/vauth/digest_sspi.o \
			    ./curl/curl_v7.67.0/lib/vauth/krb5_sspi.o \
			    ./curl/curl_v7.67.0/lib/vauth/oauth2.o \
			    ./curl/curl_v7.67.0/lib/vauth/cram.o \
			    ./curl/curl_v7.67.0/lib/vauth/krb5_gssapi.o \
			    ./curl/curl_v7.67.0/lib/llist.o \
			    ./curl/curl_v7.67.0/lib/easy.o \
			    ./curl/curl_v7.67.0/lib/md5.o \
			    ./libssh2/src/crypt.o \
			    ./libssh2/src/kex.o \
			    ./libssh2/src/channel.o \
			    ./libssh2/src/pem.o \
			    ./libssh2/src/packet.o \
			    ./libssh2/src/session.o \
			    ./libssh2/src/wincng.o \
			    ./libssh2/src/publickey.o \
			    ./libssh2/src/agent.o \
			    ./libssh2/src/hostkey.o \
			    ./libssh2/src/sftp.o \
			    ./libssh2/src/version.o \
			    ./libssh2/src/global.o \
			    ./libssh2/src/knownhost.o \
			    ./libssh2/src/transport.o \
			    ./libssh2/src/libgcrypt.o \
			    ./libssh2/src/openssl.o \
			    ./libssh2/src/keepalive.o \
			    ./libssh2/src/misc.o \
			    ./libssh2/src/mac.o \
			    ./libssh2/src/userauth.o \
			    ./libssh2/src/comp.o \
			    ./libssh2/src/scp.o \
			    ./libssh2/src/mbedtls.o \
			    ./libssh2/src/os400qc3.o \
