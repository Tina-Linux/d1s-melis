#include <rtthread.h>
#include <cli_console.h>
#include "msh.h"
#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>

int cmd_adbd(int argc, char *argv[])
{
    int adbd_init(int argc, char **argv);
    adbd_init(argc, argv);
    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_adbd, adbd, adbd service);
