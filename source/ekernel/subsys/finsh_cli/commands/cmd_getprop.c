#include <rtthread.h>
#include <cli_console.h>
#include "msh.h"
#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>
#include <version.h>
const char * __attribute__((weak)) melis_banner_string(void) {
    return "V_0.0.0";
}
int cmd_getprop(int argc, char *argv[])
{
	extern const char *melis_banner_string(void);
	printf("[ro.product.device]: [%s]\n[ro.product.firmware]: [%s-%s-%s]\n[ro.sys.cputype]: [%s]\n[ro.build.version.release]: [%d]\n", 
										TARGET_BOARD_TYPE, melis_banner_string(), SDK_COMPILE_BY, SDK_UTS_VERSION, CPU_TYPE, 0);
    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_getprop, getprop, getprop service);
