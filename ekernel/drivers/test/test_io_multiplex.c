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
