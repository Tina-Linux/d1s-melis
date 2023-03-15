#
# Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.
#
# This file is part of the lwIP TCP/IP stack.
#
# Author: Adam Dunkels <adam@sics.se>
#
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/                         \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/                 \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/include/                     \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/include/ipv4/                \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/arch/include/            \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/websocket/include/  \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/compat/posix     \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/include/    \
		 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/ports/inc/    \

subdir-ccflags-y += -DMBEDTLS_ON_LWIP  \
	            -DLWIP_ERRNO_INCLUDE=1  \
	            -D__CONFIG_LWIP_V1  \

MBEDTLS_PROGRAMS_FILES=programs/wince_main.o  \
		       programs/ssl/ssl_server.o  \
		       programs/ssl/ssl_server2.o  \
		       programs/ssl/ssl_client2.o  \
		       programs/ssl/ssl_mail_client.o  \
		       programs/ssl/dtls_client.o  \
		       programs/ssl/ssl_fork_server.o  \
		       programs/ssl/ssl_pthread_server.o  \
		       programs/ssl/mini_client.o  \
		       programs/ssl/dtls_server.o  \
		       programs/ssl/ssl_client1.o  \
		       programs/x509/crl_app.o  \
		       programs/x509/cert_app.o  \
		       programs/x509/cert_req.o  \
		       programs/x509/cert_write.o  \
		       programs/x509/req_app.o  \
		       programs/util/strerror.o  \
		       programs/util/pem2der.o  \
		       programs/pkey/key_app.o  \
		       programs/pkey/rsa_decrypt.o  \
		       programs/pkey/rsa_verify.o  \
		       programs/pkey/ecdh_curve25519.o  \
		       programs/pkey/rsa_sign.o  \
		       programs/pkey/dh_server.o  \
		       programs/pkey/key_app_writer.o  \
		       programs/pkey/pk_verify.o  \
		       programs/pkey/rsa_genkey.o  \
		       programs/pkey/mpi_demo.o  \
		       programs/pkey/dh_client.o  \
		       programs/pkey/ecdsa.o  \
		       programs/pkey/pk_sign.o  \
		       programs/pkey/gen_key.o  \
		       programs/pkey/dh_genprime.o  \
		       programs/pkey/rsa_sign_pss.o  \
		       programs/pkey/rsa_verify_pss.o  \
		       programs/pkey/rsa_encrypt.o  \
		       programs/pkey/pk_decrypt.o  \
		       programs/pkey/pk_encrypt.o  \
		       programs/hash/generic_sum.o  \
		       programs/hash/hello.o  \
		       programs/aes/crypt_and_hash.o  \
		       programs/aes/aescrypt2.o  \
		       programs/test/selftest.o  \
		       programs/test/ssl_cert_test.o  \
		       programs/test/udp_proxy.o  \
		       programs/test/benchmark.o  \
		       programs/random/gen_random_havege.o  \
		       programs/random/gen_entropy.o  \
		       programs/random/gen_random_ctr_drbg.o

MBEDTLS_LIBRARY_FILES=library/cmac.o \
		      library/md.o \
		      library/sha256.o \
		      library/arc4.o \
		      library/net_sockets.o \
		      library/aes.o \
		      library/x509write_csr.o \
		      library/hmac_drbg.o \
		      library/dhm.o \
		      library/x509.o \
		      library/ripemd160.o \
		      library/version.o \
		      library/x509_crt.o \
		      library/ssl_ciphersuites.o \
		      library/base64.o \
		      library/ssl_cache.o \
		      library/ctr_drbg.o \
		      library/cipher.o \
		      library/x509_csr.o \
		      library/padlock.o \
		      library/ecp_curves.o \
		      library/sha512.o \
		      library/asn1write.o \
		      library/ssl_ticket.o \
		      library/ecjpake.o \
		      library/threading.o \
		      library/pk_wrap.o \
		      library/aesni.o \
		      library/oid.o \
		      library/asn1parse.o \
		      library/cipher_wrap.o \
		      library/x509write_crt.o \
		      library/timing.o \
		      library/ecdsa.o \
		      library/havege.o \
		      library/md_wrap.o \
		      library/memory_buffer_alloc.o \
		      library/xtea.o  \
                      library/x509_create.o  \
                      library/pem.o  \
                      library/rsa_internal.o  \
                      library/entropy.o  \
                      library/md5.o  \
                      library/ccm.o  \
                      library/md2.o  \
                      library/pk.o  \
                      library/bignum.o  \
                      library/camellia.o  \
                      library/platform.o  \
                      library/rsa.o  \
                      library/gcm.o  \
                      library/version_features.o  \
                      library/md4.o  \
                      library/pkcs5.o  \
                      library/debug.o  \
                      library/sha1.o  \
                      library/ssl_cookie.o  \
                      library/des.o  \
                      library/certs.o  \
                      library/ssl_srv.o  \
                      library/x509_crl.o  \
                      library/pkparse.o  \
                      library/ssl_tls.o  \
                      library/ecp.o  \
                      library/ecdh.o  \
                      library/error.o  \
                      library/entropy_poll.o  \
                      library/pkcs12.o  \
                      library/ssl_cli.o  \
                      library/blowfish.o  \
                      library/pkcs11.o  \
                      library/pkwrite.o \
                      library/mbedtls.o

MBEDTLS_PORTS_FILES =  \
		      ./ports/src/sha256_alt.o\
		      ./ports/src/sha512_alt.o \
		      ./ports/src/sha1_alt.o \
		      ./ports/src/md5_alt.o \
		      ./ports/src/aes_alt.o \
		      ./ports/src/tls_hardware.o \
		      ./ports/src/tls_certificate.o \
		      ./ports/src/arc4_alt.o \
		      ./ports/src/tls_client.o \
		      ./ports/src/timing_alt.o \
		      ./ports/src/des_alt.o \
		      ./ports/src/rsa_alt.o \


ifeq ($(CONFIG_USE_HWCRYPTO),y)
MBEDTLS_LIBRARY_FILES += rt_hwports/src/aes_alt.o
subdir-ccflags-y +=  -DMBEDTLS_AES_ALT \
				-I$(srctree)/ekernel/drivers/include/ \
				-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/rt_hwports/inc/

endif

obj-$(CONFIG_MBEDTLS)+=$(MBEDTLS_PROGRAMS_FILES)  \
	               $(MBEDTLS_LIBRARY_FILES)   \
	               $(MBEDTLS_PORTS_FILES) \

obj-$(CONFIG_MBEDTLS_TEST)+= yotta/data/entropy_hardware_poll.o \
                    			yotta/data/example-authcrypt/authcrypt.o \
                    			yotta/data/example-benchmark/benchmark.o \
                    			yotta/data/example-hashing/hashing.o \
                    			yotta/data/example-selftest/selftest.o

#COMPONENTS_MBEDTLS_OBJECTS=$(patsubst %.c, %.o, $(shell find $(BASE)/components/thirdparty/network/mbedtls/ -name "*.c"))

