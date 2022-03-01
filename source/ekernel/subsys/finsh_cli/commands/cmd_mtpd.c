#include <rtthread.h>
#include <cli_console.h>
#include "msh.h"
#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>

int cmd_mtpd(int argc, char *argv[])
{
    int mtpd_main(int argc, char **argv);
    mtpd_main(argc, argv);
    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_mtpd, mtpd, mtpd service);

