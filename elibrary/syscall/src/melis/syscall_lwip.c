/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>

#include <kapi.h>
#include <syscall.h>

int lwip_socket(int domain, int type, int protocol)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_socket), (long)domain, (long)type, (long)protocol);
	return result;
}

int lwip_bind(int s, void *name, int namelen)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_bind), (long)s, (long)name, (long)namelen);
	return result;
}

int lwip_listen(int s, int backlog)
{
	int result = 0;
	result = (int)__syscall2((long)SYSCALL_LWIP(lwip_listen), (long)s, (long)backlog);
	return result;
}

int lwip_connect(int s, void *name, int namelen)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_connect), (long)s, (long)name, (long)namelen);
	return result;
}

int lwip_accept(int s, void *addr, void *addrlen)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_accept), (long)s, (long)addr, (long)addrlen);
	return result;
}

int lwip_recvfrom(int s, void *mem, int len, int flags, void *from, void *fromlen)
{
	int result = 0;
	result = (int)__syscall6((long)SYSCALL_LWIP(lwip_recvfrom), (long)s, (long)mem, (long)len, (long)flags, (long)from, (long)fromlen);
	return result;
}

int lwip_read(int s, void *mem, int len)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_read), (long)s, (long)mem, (long)len);
	return result;
}

int lwip_recv(int s, void *mem, int len, int flags)
{
	int result = 0;
	result = (int)__syscall4((long)SYSCALL_LWIP(lwip_recv), (long)s, (long)mem, (long)len, (long)flags);
	return result;
}

int lwip_sendto(int s, void *data, int size, int flags, void *to, int tolen)
{
	int result = 0;
	result = (int)__syscall6((long)SYSCALL_LWIP(lwip_sendto), (long)s, (long)data, (long)size, (long)flags, (long)to, (long)tolen);
	return result;
}

int lwip_write(int s, void *data, int size)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_write), (long)s, (long)data, (long)size);
	return result;
}

int lwip_send(int s, void *data, int size, int flags)
{
	int result = 0;
	result = (int)__syscall4((long)SYSCALL_LWIP(lwip_send), (long)s, (long)data, (long)size, (long)flags);
	return result;
}

int lwip_close(int s)
{
	int result = 0;
	result = (int)__syscall1((long)SYSCALL_LWIP(lwip_close), (long)s);
	return result;
}

int lwip_shutdown(int s, int how)
{
	int result = 0;
	result = (int)__syscall2((long)SYSCALL_LWIP(lwip_shutdown), (long)s, (long)how);
	return result;
}

int lwip_getpeername(int s, void *name, void *namelen)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_getpeername), (long)s, (long)name, (long)namelen);
	return result;
}

int lwip_getsockname(int s, void *name, void *namelen)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_getsockname), (long)s, (long)name, (long)namelen);
	return result;
}

int lwip_setsockopt(int s, int level, int optname, void *optval, int optlen)
{
	int result = 0;
	result = (int)__syscall5((long)SYSCALL_LWIP(lwip_setsockopt), (long)s, (long)level, (long)optname, (long)optval, (long)optlen);
	return result;
}

int lwip_getsockopt(int s, int level, int optname, void *optval, int optlen)
{
	int result = 0;
	result = (int)__syscall5((long)SYSCALL_LWIP(lwip_getsockopt), (long)s, (long)level, (long)optname, (long)optval, (long)optlen);
	return result;
}

int lwip_recvmsg(int s, void *message, int flags)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_recvmsg), (long)s, (long)message, (long)flags);
	return result;
}

int lwip_sendmsg(int s, void *msg, int flags)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_sendmsg), (long)s, (long)msg, (long)flags);
	return result;
}

int lwip_select(int maxfdp1, void *readset, void *writeset, void *exceptset, void *timeout)
{
	int result = 0;
	result = (int)__syscall5((long)SYSCALL_LWIP(lwip_select), (long)maxfdp1, (long)readset, (long)writeset, (long)exceptset, (long)timeout);
	return result;
}

int lwip_poll(void *fds, int nfds, int timeout)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_poll), (long)fds, (long)nfds, (long)timeout);
	return result;
}

int lwip_ioctl(int s, long cmd, void *argp)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_ioctl), (long)s, (long)cmd, (long)argp);
	return result;
}

int lwip_inet_ntop(int af, void *src, void *dst, int size)
{
	int result = 0;
	result = (int)__syscall4((long)SYSCALL_LWIP(lwip_inet_ntop), (long)af, (long)src, (long)dst, (long)size);
	return result;
}

int lwip_inet_pton(int af, void *src, void *dst)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_inet_pton), (long)af, (long)src, (long)dst);
	return result;
}

int lwip_readv(int s, void *iov, int iovcnt)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_readv), (long)s, (long)iov, (long)iovcnt);
	return result;
}

int lwip_writev(int s, void *iov, int iovcnt)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_writev), (long)s, (long)iov, (long)iovcnt);
	return result;
}

int lwip_fcntl(int s, int cmd, int val)
{
	int result = 0;
	result = (int)__syscall3((long)SYSCALL_LWIP(lwip_fcntl), (long)s, (long)cmd, (long)val);
	return result;
}

