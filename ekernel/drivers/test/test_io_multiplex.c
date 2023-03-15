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
#include <rtthread.h>
#include <pthread.h>

static void* test_select_main(void *args)
{
    fd_set rd;
    struct timeval tv;
    int err;

    int libc_stdio_get_console(void);
    int fd = libc_stdio_get_console();

    FD_ZERO(&rd);
    FD_SET(fd, &rd);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    err = select(fd + 1, &rd, NULL, NULL, &tv);

    if (err == 0)
    {
        printf("select time out!, %s\n", (char *)args);
    }
    else if (err == -1)
    {
        printf("fail to select!, %s\n", (char *)args);
    }
    else
    {
        printf("data is available!, %s\n", (char *)args);
    }

    return NULL;
}

static int cmd_test_select(int argc, const char **argv)
{
    pthread_t tidp1, tidp2;
    char *name1 = "tid1";
    char *name2 = "tid2";

    if ((pthread_create(&tidp1, NULL, test_select_main, (void *)name1)) == -1)
    {
        printf("create error!\n");
        return 1;
    }

    if ((pthread_create(&tidp2, NULL, test_select_main, (void *)name2)) == -1)
    {
        printf("create error!\n");
        return 1;
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_test_select, __cmd_test_select, slect test);

static int cmd_test_poll(int argc, const char **argv)
{
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_test_poll, __cmd_test_poll, poll test);
