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

# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/lwip/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/include/ipv4/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/arch/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/websocket/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/websocket/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/include/compat/posix/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/include/
subdir-ccflags-y += 	-I$(srctree)/ekernel/components/thirdparty/net/rt-thread/lwip/src/apps/mbedtls/include/mbedtls/


subdir-ccflags-y += -DAW_DEBUG_LEVEL_NONE  \
	            -DLWIP_ERRNO_INCLUDE=1 \
	            -DLWIP_PROVIDE_ERRNO=1

obj-$(CONFIG_WEBSOCKET) +=src/rws_string.o \
			  src/rws_list.o \
			  src/rws_socketpub.o \
			  src/rws_socketpriv.o \
			  src/librws.o \
			  src/rws_thread.o \
			  src/rws_frame.o \
			  src/rws_memory.o \
			  src/rws_error.o \
			  src/rws_common.o \
