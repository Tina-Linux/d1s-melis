/*#####################################################################
# File Describe:webclient_download_sample.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年06月16日 星期二 09时09分54秒
#====================================================================*/

#include <stdint.h>
#include <stdlib.h>
#include <rtthread.h>

#include <webclient.h>
int web_download_test(int argc, char** argv)
{
    if (argc != 3)
    {
        rt_kprintf("Please using: web_download_test <URI> <filename>\n");
        return -1;
    }

    webclient_get_file(argv[1], argv[2]);
    return 0;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(web_download_test, Get file by URI: web_download_test <URI> <filename>.);
#endif /* FINSH_USING_MSH */

