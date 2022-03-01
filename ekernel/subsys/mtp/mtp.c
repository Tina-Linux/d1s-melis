#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "adb.h"
int g_mtpd_debug_mask = 0;
#define MTP_VERSION    "AW-V1.1.0"
static inline void mtpd_version(void)
{
    printf("mtpd version:%s, compiled on: %s %s\n", MTP_VERSION, __DATE__, __TIME__);
}

int mtpd_main(void)
{
    extern int mtp_usb_init(void);
    int ret = 0;
    int sw = 1, len;

    mtpd_version();
    /* TODO: auth,transport_port */
    ret = mtp_usb_init();
    if (ret != 0)
    {
        printf("mtpd service init failed,it may already init, please use directly.\n");
        return -1;
    }

    printf("mtpd service init successful\n");
    return ret;
}

static void usage(void)
{
    printf("Usgae: mtpd [option]\n");
    printf("-v,          mtpd version\n");
    printf("-d,          mtpd debug option\n");
    printf("-h,          mtpd help\n");
    printf("\n");
}

int mtp_init(int argc, char **argv)
{
    int ret = 0, c;

    optind = 0;
    while ((c = getopt(argc, argv, "vd:h")) != -1)
    {
        switch (c)
        {
            case 'v':
                mtpd_version();
                return 0;
            case 'd':
                g_mtpd_debug_mask = atoi(optarg);
                return 0;
            case 'h':
            default:
                usage();
                return 0;
        }
    }

    ret = mtpd_main();

    return ret;
}
int mtpd_exit(void)
{
    extern int mtp_usb_exit(void);
	mtp_usb_exit();
	return 0;
}

