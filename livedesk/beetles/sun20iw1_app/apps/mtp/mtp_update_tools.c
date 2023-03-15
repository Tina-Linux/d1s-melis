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
#include <dfs_posix.h>
#include "elibs_stdio.h"
#include "elibs_string.h"

typedef struct
{
    uint32_t action;    /* 0:add, 1:remove, 2:update */
    uint32_t type;      /* 0:file, 2:dir */
    uint32_t srcPathLen;    /* src path buffer length */
    uint32_t destPathLen;   /* dest path buffer length */
    char *path;     /* object path */
} mtp_command_t;
#define MTP_FIFO_NAME   "/tmp/.mtp_fifo"


int mtp_tools_send_command(ES_FILE * fp, uint32_t action, uint32_t type, const char *spath, const char *dpath)
{
    int ret;
    mtp_command_t *command;
    size_t spathLen, dpathLen, pathLen = 0;
    size_t command_size;

    if (!spath)
    {
        eLIBs_printf("spath is NULL!\n");
        return -1;
    }

    spathLen = eLIBs_strlen(spath) + 1;
    dpathLen = (dpath != NULL) ? (eLIBs_strlen(dpath) + 1) : 0;
    pathLen = spathLen + dpathLen;
    command_size = sizeof(mtp_command_t) + pathLen;

#if 0
    eLIBs_printf("action:%u, type:%u, spath:%p, dpath:%p\n", action, type, spath, dpath);
    if (spathLen > 1)
    {
        eLIBs_printf("spath:%s\n", spath);
    }
    if (dpathLen > 1)
    {
        eLIBs_printf("dpath:%s\n", dpath);
    }
#endif

    command = esMEMS_Malloc(0, command_size);
    command->action = action;
    command->type = type;
    command->srcPathLen =  spathLen;
    command->destPathLen =  dpathLen;
    if (spathLen > 1)
    {
        command->path = (char *)&command[1];
        eLIBs_strcpy(command->path, spath);
    }
    else
    {
        eLIBs_printf("spath error: %s\n", spath);
        esMEMS_Mfree(0, command);
        return -1;
    }
    if (dpathLen > 1)
    {
        eLIBs_strcpy(command->path + spathLen, dpath);
    }

    ret = eLIBs_fwrite(command, sizeof(char), command_size, fp);
    if (ret != command_size)
    {
        eLIBs_printf("write failed, command size:%zu, but only write %d\n",
               command_size, ret);
        esMEMS_Mfree(0, command);
        return -1;
    }
    esMEMS_Mfree(0, command);
	eLIBs_fclose(fp);
    return 0;
}


static void usage(void)
{
    eLIBs_printf("MtpTools usage:\n");
    eLIBs_printf("MtpTools [function] [path]\n");
    eLIBs_printf(" -f, --function      function opion, contains add,remove,update,cut,copy\n");
    eLIBs_printf(" -t, --type          object type, FILE or DIR\n");
    eLIBs_printf(" -s, --spath         object src path\n");
    eLIBs_printf(" -d, --dpath         object dest path, specify dest path of object which will be cut or copy\n");
    eLIBs_printf(" -h, --help          show help\n");
    eLIBs_printf("\n");
    eLIBs_printf("example:\n");
    eLIBs_printf("MtpTools -f add -t FILE -s /mnt/UDISK/test\n");
    eLIBs_printf("\n");
   	return ;
}

int mtp_update(int argc, char *argv[])
{
    ES_FILE * fp = NULL;
    enum
    {
        MTP_TOOLS_FUNCTION_ADD = 0,
        MTP_TOOLS_FUNCTION_REMOVE,
        MTP_TOOLS_FUNCTION_UPDATE,
        MTP_TOOLS_FUNCTION_CUT,
        MTP_TOOLS_FUNCTION_COPY,
    } func = MTP_TOOLS_FUNCTION_ADD;
    enum
    {
        MTP_TOOLS_TYPE_FILE = 0,
        MTP_TOOLS_TYPE_DIR,
    } type = MTP_TOOLS_TYPE_FILE;
    char *spath = NULL;
    char *dpath = NULL;

    while (1)
    {
//        const struct option long_options[] =
//        {
//            {"function", required_argument, NULL, 'f'},
//            {"type", required_argument, NULL, 't'},
//            {"spath", required_argument, NULL, 's'},
//            {"dpath", required_argument, NULL, 'd'},
//            {"help", no_argument, NULL, 'h'},
//        };
        int option_index = 0;
        int c = 0;

//        c = getopt_long(argc, argv, "f:t:s:d:h", long_options, &option_index);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'h':
                usage();
                break;
            case 'f':
#if 0
                if (!strcmp(optarg, "add"))
                {
                    func = MTP_TOOLS_FUNCTION_ADD;
                }
                else if (!strcmp(optarg, "remove"))
                {
                    func = MTP_TOOLS_FUNCTION_REMOVE;
                }
                else if (!strcmp(optarg, "update"))
                {
                    func = MTP_TOOLS_FUNCTION_UPDATE;
                }
                else if (!strcmp(optarg, "cut"))
                {
                    func = MTP_TOOLS_FUNCTION_CUT;
                }
                else if (!strcmp(optarg, "copy"))
                {
                    func = MTP_TOOLS_FUNCTION_COPY;
                }
                else
                {
                    eLIBs_printf("invalid function:[%s]\n", optarg);
                    usage();
                }
#endif
                break;
            case 't':
#if 0
                if (!strcmp(optarg, "FILE"))
                {
                    type = MTP_TOOLS_TYPE_FILE;
                }
                else if (!strcmp(optarg, "DIR"))
                {
                    type = MTP_TOOLS_TYPE_DIR;
                }
                else
                {
                    eLIBs_printf("invalid type:[%s]\n", optarg);
                    usage();
                }
#endif
                break;
            case 's':
#if 0
                if (!optarg)
                {
                    usage();
                }
                spath = strdup(optarg);
#endif
                break;
            case 'd':
#if 0
                if (!optarg)
                {
                    usage();
                }
                dpath = strdup(optarg);
#endif
                break;
            default:
                usage();
                break;
        }
    }

#if 0
    if (optind < 2)
    {
        usage();
    }
#endif
    if (!spath)
    {
        eLIBs_printf("need spath!\n");
        usage();
    }
    mtp_tools_send_command(fp, func, type, spath, dpath);
    return 0;
}
