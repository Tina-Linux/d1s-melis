#include <rtthread.h>
#include <cli_console.h>

static int cmd_exit(int argc, char *argv[])
{
    cli_console_current_task_destory();
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_exit, __cmd_exit, Exit current console);
