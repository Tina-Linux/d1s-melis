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

subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/lib/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/lib/vtls/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/projects/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/include/curl/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/tests/server/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/tests/unit/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/tests/libtest/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/packages/OS400/
subdir-ccflags-y += 	-I$(srctree)/ekernel/subsys/net/rt-thread/net_tools/netutils-master/libcurl/packages/vms/

subdir-ccflags-y += 	\
			-DHAVE_CONFIG_H \

obj-$(CONFIG_LIBCURL) += \
			./src/tool_sleep.o \
			./src/tool_operate.o \
			./src/tool_filetime.o \
			./src/tool_formparse.o \
			./src/tool_getpass.o \
			./src/tool_dirhie.o \
			./src/tool_main.o \
			./src/tool_easysrc.o \
			./src/tool_help.o \
			./src/tool_metalink.o \
			./src/tool_panykey.o \
			./src/tool_hugehelp.o \
			./src/tool_bname.o \
			./src/tool_binmode.o \
			./src/tool_helpers.o \
			./src/tool_doswin.o \
			./src/tool_parsecfg.o \
			./src/tool_cb_wrt.o \
			./src/tool_cb_rea.o \
			./src/tool_cb_see.o \
			./src/slist_wc.o \
			./src/tool_xattr.o \
			./src/tool_operhlp.o \
			./src/tool_setopt.o \
			./src/tool_progress.o \
			./src/tool_convert.o \
			./src/tool_writeout.o \
			./src/tool_strdup.o \
			./src/tool_libinfo.o \
			./src/tool_msgs.o \
			./src/tool_cfgable.o \
			./src/tool_vms.o \
			./src/tool_paramhlp.o \
			./src/tool_util.o \
			./src/tool_cb_prg.o \
			./src/tool_cb_dbg.o \
			./src/tool_getparam.o \
			./src/tool_cb_hdr.o \
			./src/tool_urlglob.o \
			./src/tool_homedir.o \
			./lib/mime.o \
			./lib/hash.o \
			./lib/asyn-ares.o \
			./lib/http_ntlm.o \
			./lib/curl_rtmp.o \
			./lib/strcase.o \
			./lib/imap.o \
			./lib/slist.o \
			./lib/ldap.o \
			./lib/curl_multibyte.o \
			./lib/curl_gethostname.o \
			./lib/hostip6.o \
			./lib/vtls/mesalink.o \
			./lib/vtls/gtls.o \
			./lib/vtls/gskit.o \
			./lib/vtls/wolfssl.o \
			./lib/vtls/mbedtls.o \
			./lib/vtls/openssl.o \
			./lib/vtls/sectransp.o \
			./lib/vtls/bearssl.o \
			./lib/vtls/schannel_verify.o \
			./lib/vtls/mbedtls_threadlock.o \
			./lib/vtls/nss.o \
			./lib/vtls/vtls.o \
			./lib/vtls/schannel.o \
			./lib/x509asn1.o \
			./lib/http_digest.o \
			./lib/http_negotiate.o \
			./lib/nwos.o \
			./lib/mprintf.o \
			./lib/strtoofft.o \
			./lib/openldap.o \
			./lib/http_proxy.o \
			./lib/transfer.o \
			./lib/rtsp.o \
			./lib/curl_fnmatch.o \
			./lib/md4.o \
			./lib/curl_sasl.o \
			./lib/urlapi.o \
			./lib/speedcheck.o \
			./lib/system_win32.o \
			./lib/hostip.o \
			./lib/parsedate.o \
			./lib/doh.o \
			./lib/gopher.o \
			./lib/md5.o \
			./lib/rand.o \
			./lib/inet_pton.o \
			./lib/ftplistparser.o \
			./lib/smtp.o \
			./lib/security.o \
			./lib/memdebug.o \
			./lib/socks_sspi.o \
			./lib/nwlib.o \
			./lib/ftp.o \
			./lib/curl_gssapi.o \
			./lib/if2ip.o \
			./lib/http_chunks.o \
			./lib/socks_gssapi.o \
			./lib/asyn-thread.o \
			./lib/dotdot.o \
			./lib/vauth/spnego_sspi.o \
			./lib/vauth/vauth.o \
			./lib/vauth/krb5_gssapi.o \
			./lib/vauth/digest_sspi.o \
			./lib/vauth/cleartext.o \
			./lib/vauth/ntlm.o \
			./lib/vauth/spnego_gssapi.o \
			./lib/vauth/cram.o \
			./lib/vauth/oauth2.o \
			./lib/vauth/krb5_sspi.o \
			./lib/vauth/digest.o \
			./lib/vauth/ntlm_sspi.o \
			./lib/timeval.o \
			./lib/psl.o \
			./lib/strerror.o \
			./lib/pingpong.o \
			./lib/fileinfo.o \
			./lib/krb5.o \
			./lib/http2.o \
			./lib/hmac.o \
			./lib/dict.o \
			./lib/hostsyn.o \
			./lib/idn_win32.o \
			./lib/curl_range.o \
			./lib/netrc.o \
			./lib/inet_ntop.o \
			./lib/strtok.o \
			./lib/hostcheck.o \
			./lib/llist.o \
			./lib/curl_path.o \
			./lib/hostasyn.o \
			./lib/curl_des.o \
			./lib/curl_ntlm_wb.o \
			./lib/socks.o \
			./lib/curl_sspi.o \
			./lib/content_encoding.o \
			./lib/getenv.o \
			./lib/curl_ctype.o \
			./lib/getinfo.o \
			./lib/curl_threads.o \
			./lib/amigaos.o \
			./lib/curl_endian.o \
			./lib/url.o \
			./lib/smb.o \
			./lib/vquic/ngtcp2.o \
			./lib/vquic/quiche.o \
			./lib/altsvc.o \
			./lib/vssh/libssh.o \
			./lib/vssh/wolfssh.o \
			./lib/vssh/libssh2.o \
			./lib/non-ascii.o \
			./lib/rename.o \
			./lib/strdup.o \
			./lib/progress.o \
			./lib/http.o \
			./lib/cookie.o \
			./lib/select.o \
			./lib/curl_get_line.o \
			./lib/curl_memrchr.o \
			./lib/wildcard.o \
			./lib/base64.o \
			./lib/pop3.o \
			./lib/splay.o \
			./lib/curl_ntlm_core.o \
			./lib/conncache.o \
			./lib/easy.o \
			./lib/escape.o \
			./lib/sendf.o \
			./lib/multi.o \
			./lib/connect.o \
			./lib/tftp.o \
			./lib/formdata.o \
			./lib/setopt.o \
			./lib/telnet.o \
			./lib/socketpair.o \
			./lib/version.o \
			./lib/share.o \
			./lib/file.o \
			./lib/nonblock.o \
			./lib/sha256.o \
			./lib/warnless.o \
			./lib/hostip4.o \
			./lib/curl_addrinfo.o \

# \
			./c-ares-1.16.1/acountry.o \
			./c-ares-1.16.1/adig.o \
			./c-ares-1.16.1/ahost.o \
			./c-ares-1.16.1/ares_android.o \
			./c-ares-1.16.1/ares_cancel.o \
			./c-ares-1.16.1/ares__close_sockets.o \
			./c-ares-1.16.1/ares_create_query.o \
			./c-ares-1.16.1/ares_data.o \
			./c-ares-1.16.1/ares_destroy.o \
			./c-ares-1.16.1/ares_expand_name.o \
			./c-ares-1.16.1/ares_expand_string.o \
			./c-ares-1.16.1/ares_fds.o \
			./c-ares-1.16.1/ares_freeaddrinfo.o \
			./c-ares-1.16.1/ares_free_hostent.o \
			./c-ares-1.16.1/ares_free_string.o \
			./c-ares-1.16.1/ares_getaddrinfo.o \
			./c-ares-1.16.1/ares_getenv.o \
			./c-ares-1.16.1/ares_gethostbyaddr.o \
			./c-ares-1.16.1/ares_gethostbyname.o \
			./c-ares-1.16.1/ares__get_hostent.o \
			./c-ares-1.16.1/ares_getnameinfo.o \
			./c-ares-1.16.1/ares_getopt.o \
			./c-ares-1.16.1/ares_getsock.o \
			./c-ares-1.16.1/ares_init.o \
			./c-ares-1.16.1/ares_library_init.o \
			./c-ares-1.16.1/ares_llist.o \
			./c-ares-1.16.1/ares_mkquery.o \
			./c-ares-1.16.1/ares_nowarn.o \
			./c-ares-1.16.1/ares_options.o \
			./c-ares-1.16.1/ares_parse_aaaa_reply.o \
			./c-ares-1.16.1/ares_parse_a_reply.o \
			./c-ares-1.16.1/ares__parse_into_addrinfo.o \
			./c-ares-1.16.1/ares_parse_mx_reply.o \
			./c-ares-1.16.1/ares_parse_naptr_reply.o \
			./c-ares-1.16.1/ares_parse_ns_reply.o \
			./c-ares-1.16.1/ares_parse_ptr_reply.o \
			./c-ares-1.16.1/ares_parse_soa_reply.o \
			./c-ares-1.16.1/ares_parse_srv_reply.o \
			./c-ares-1.16.1/ares_parse_txt_reply.o \
			./c-ares-1.16.1/ares_platform.o \
			./c-ares-1.16.1/ares_process.o \
			./c-ares-1.16.1/ares_query.o \
			./c-ares-1.16.1/ares__readaddrinfo.o \
			./c-ares-1.16.1/ares__read_line.o \
			./c-ares-1.16.1/ares_search.o \
			./c-ares-1.16.1/ares_send.o \
			./c-ares-1.16.1/ares__sortaddrinfo.o \
			./c-ares-1.16.1/ares_strcasecmp.o \
			./c-ares-1.16.1/ares_strdup.o \
			./c-ares-1.16.1/ares_strerror.o \
			./c-ares-1.16.1/ares_strsplit.o \
			./c-ares-1.16.1/ares_timeout.o \
			./c-ares-1.16.1/ares__timeval.o \
			./c-ares-1.16.1/ares_version.o \
			./c-ares-1.16.1/ares_writev.o \
			./c-ares-1.16.1/bitncmp.o \
			./c-ares-1.16.1/inet_net_pton.o \
			./c-ares-1.16.1/inet_ntop.o \
			./c-ares-1.16.1/windows_port.o \



obj-$(CONFIG_LIBCURL_TEST) += \
			./docs/examples/https.o \
			./docs/examples/dns_test.o \
			./docs/examples/rtsp.o \
			./docs/examples/telnet.o \
			./docs/examples/ftpget.o \

#			\
			./docs/examples/rtsp.o \
			./docs/examples/xmlstream.o \
			./docs/examples/imap-list.o \
			./docs/examples/imap-append.o \
			./docs/examples/pop3-noop.o \
			./docs/examples/pop3-ssl.o \
			./docs/examples/imap-copy.o \
			./docs/examples/smtp-mail.o \
			./docs/examples/pop3-stat.o \
			./docs/examples/httpput.o \
			./docs/examples/ftpgetinfo.o \
			./docs/examples/pop3-multi.o \
			./docs/examples/sftpuploadresume.o \
			./docs/examples/simplessl.o \
			./docs/examples/imap-store.o \
			./docs/examples/imap-examine.o \
			./docs/examples/smtp-multi.o \
			./docs/examples/externalsocket.o \
			./docs/examples/smtp-vrfy.o \
			./docs/examples/curlx.o \
			./docs/examples/persistent.o \
			./docs/examples/synctime.o \
			./docs/examples/opensslthreadlock.o \
			./docs/examples/urlapi.o \
			./docs/examples/chkspeed.o \
			./docs/examples/sftpget.o \
			./docs/examples/ftp-wildcard.o \
			./docs/examples/parseurl.o \
			./docs/examples/multi-app.o \
			./docs/examples/http3.o \
			./docs/examples/sampleconv.o \
			./docs/examples/smtp-mime.o \
			./docs/examples/debug.o \
			./docs/examples/pop3-list.o \
			./docs/examples/imap-create.o \
			./docs/examples/certinfo.o \
			./docs/examples/progressfunc.o \
			./docs/examples/postit2-formadd.o \
			./docs/examples/ephiperfifo.o \
			./docs/examples/sessioninfo.o \
			./docs/examples/http-post.o \
			./docs/examples/sendrecv.o \
			./docs/examples/sepheaders.o \
			./docs/examples/imap-lsub.o \
			./docs/examples/imap-ssl.o \
			./docs/examples/ftpupload.o \
			./docs/examples/simplepost.o \
			./docs/examples/imap-delete.o \
			./docs/examples/htmltidy.o \
			./docs/examples/multi-uv.o \
			./docs/examples/threaded-ssl.o \
			./docs/examples/threaded-shared-conn.o \
			./docs/examples/curlgtk.o \
			./docs/examples/multi-double.o \
			./docs/examples/ghiper.o \
			./docs/examples/imap-search.o \
			./docs/examples/imap-multi.o \
			./docs/examples/httpcustomheader.o \
			./docs/examples/http3-present.o \
			./docs/examples/anyauthput.o \
			./docs/examples/ftpgetresp.o \
			./docs/examples/imap-tls.o \
			./docs/examples/pop3-authzid.o \
			./docs/examples/multi-single.o \
			./docs/examples/imap-authzid.o \
			./docs/examples/ftpsget.o \
			./docs/examples/multi-debugcallback.o \
			./docs/examples/imap-noop.o \
			./docs/examples/multithread.o \
			./docs/examples/resolve.o \
			./docs/examples/postit2.o \
			./docs/examples/post-callback.o \
			./docs/examples/cacertinmem.o \
			./docs/examples/fopen.o \
			./docs/examples/smtp-ssl.o \
			./docs/examples/smtp-tls.o \
			./docs/examples/getinmemory.o \
			./docs/examples/hiperfifo.o \
			./docs/examples/multi-post.o \
			./docs/examples/pop3-uidl.o \
			./docs/examples/sslbackend.o \
			./docs/examples/href_extractor.o \
			./docs/examples/10-at-a-time.o \
			./docs/examples/getredirect.o \
			./docs/examples/http2-download.o \
			./docs/examples/smooth-gtk-thread.o \
			./docs/examples/evhiperfifo.o \
			./docs/examples/ftpget.o \
			./docs/examples/cookie_interface.o \
			./docs/examples/smtp-authzid.o \
			./docs/examples/getinfo.o \
			./docs/examples/smtp-expn.o \
			./docs/examples/shared-connection-cache.o \
			./docs/examples/simple.o \
			./docs/examples/pop3-tls.o \
			./docs/examples/usercertinmem.o \
			./docs/examples/altsvc.o \
			./docs/examples/crawler.o \
			./docs/examples/pop3-retr.o \
			./docs/examples/http2-pushinmemory.o \
			./docs/examples/fileupload.o \
			./docs/examples/imap-fetch.o \
			./docs/examples/pop3-top.o \
			./docs/examples/postinmemory.o \
			./docs/examples/http2-upload.o \
			./docs/examples/ftpuploadfrommem.o \
			./docs/examples/ftpuploadresume.o \
			./docs/examples/multi-poll.o \
			./docs/examples/multi-event.o \
			./docs/examples/http2-serverpush.o \
			./docs/examples/multi-formadd.o \
			./docs/examples/url2file.o \
			./docs/examples/pop3-dele.o \

