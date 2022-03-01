/*#####################################################################
# File Describe:webclient_upload_sample.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年06月16日 星期二 09时11分08秒
#====================================================================*/

#include <stdint.h>
#include <stdlib.h>
#include <rtthread.h>

#include <webclient.h>
int web_upload_test(int argc, char** argv)
{
    if (argc != 4)
    {
        rt_kprintf("Please using: web_upload_test <URI> <filename> <data>\n");
        return -1;
    }

    webclient_post_file(argv[1], argv[2], argv[3]);
    return 0;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(web_upload_test, Get file by URI: web_upload_test <URI> <filename> <data>.);
#endif /* FINSH_USING_MSH */
