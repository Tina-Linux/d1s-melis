#include <rtthread.h>
#include <log.h>
#include <mod_display.h>
#include <stdlib.h>
#include <stdio.h>
#include <mod_video_play.h>
#include "sunxi_di.h"
#include <dfs_posix.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>

__u32 mid = 0;
__s32 *mp = 0;
static int cmd_video_sharpness(int argc, char **argv)
{
#if 1
    static int count = 0;
    esMODS_MIoctrl(mp, VIDEO_PLAY_SET_SHARPNESS, count, 0);

    count++;
    if(count > 7)
    {
        count = 0;
    }
    return 0;
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_video_sharpness, __cmd_video_sharpness, cmd_video_sharpness);

static int cmd_video_contrast(int argc, char **argv)
{
#if 1
    static int contrast = 0;

    esMODS_MIoctrl(mp, VIDEO_PLAY_SET_CONTRAST, contrast, 0);

    contrast += 50;
    if(contrast > 250)
    {
        contrast = 0;
    }
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_video_contrast, __cmd_video_contrast, cmd_video_contrast);


static int cmd_video_hue(int argc, char **argv)
{
#if 1
    static int hue = 0;

    esMODS_MIoctrl(mp, VIDEO_PLAY_SET_HUE, hue, 0);

    hue += 50;
    if(hue > 250)
    {
        hue = 0;
    }
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_video_hue, __cmd_video_hue, cmd_video_hue);


static int cmd_video_saturation(int argc, char **argv)
{
#if 1
    static int saturation = 0;

    esMODS_MIoctrl(mp, VIDEO_PLAY_SET_SATURATION, saturation, 0);

    saturation += 50;
    if(saturation > 250)
    {
        saturation = 0;
    }

    return 0;
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_video_saturation, __cmd_video_saturation, cmd_video_saturation);


static int cmd_video_test(int argc, char **argv)
{
#if 1
    // __u32 mid = 0;
    // __s32 *mp = 0;
    mid = esMODS_MInstall("d:\\mod\\video_play.mod",0);
    mp = esMODS_MOpen(mid, 0);
    printk("ioctrl ");
    int  ret  = 0;
    // ret = esMODS_MIoctrl(mp, VIDEO_PLAY_SET_CHANNEL, 1, 0);
    // printk("ioctrl VIDEO_PLAY_SET_CHANNEL ret = %d\n,ret");

    ret = esMODS_MIoctrl(mp, VIDEO_PLAY_START, 0, 0);
    printk("ioctrl ret = %d\n,ret");
    // esKRNL_TimeDly(1000);
    // __err("--------------xuan zhuan---------------");
    //esMODS_MIoctrl(mp, VIDEO_PLAY_SET_SHARPNESS, 5, 0);
    //esMODS_MIoctrl(mp, VIDEO_PLAY_SET_SATURATION, 255, 0);
    // esMODS_MIoctrl(mp, VIDEO_PLAY_SET_ROTATE_ANGLE, VIDEO_PLAY_ROTATE_HFLIP, 0);//旋转90°
    // esKRNL_TimeDly(500);
    // esMODS_MIoctrl(mp, VIDEO_PLAY_STOP, 0, 0);

    // __log("befor close ");
    // esMODS_MClose(mp);
    // esKRNL_TimeDly(10);
    //  __log("end of close ");
    // esMODS_MUninstall(mid);
    // esKRNL_TimeDly(10);
    // __log("end of esMODS_MUninstall ");

    return 0;
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_video_test, __cmd_video_test, video_test);


static int cmd_video_test_close(int argc, char **argv)
{
#if 1

    // __u32 mid = 0;
    // __s32 *mp = 0;
    // mid = esMODS_MInstall("d:\\mod\\video_play.mod",0);
    // mp = esMODS_MOpen(mid, 0);
    // printk("ioctrl ");
    // int  ret  = 0;
    // // ret = esMODS_MIoctrl(mp, VIDEO_PLAY_SET_CHANNEL, 1, 0);
    // // printk("ioctrl VIDEO_PLAY_SET_CHANNEL ret = %d\n,ret");

    // ret = esMODS_MIoctrl(mp, VIDEO_PLAY_START, 0, 0);
    // printk("ioctrl ret = %d\n,ret");
    // esKRNL_TimeDly(1000);
    // __err("--------------xuan zhuan---------------");
    //esMODS_MIoctrl(mp, VIDEO_PLAY_SET_SHARPNESS, 5, 0);
    //esMODS_MIoctrl(mp, VIDEO_PLAY_SET_SATURATION, 255, 0);
    // esMODS_MIoctrl(mp, VIDEO_PLAY_SET_ROTATE_ANGLE, VIDEO_PLAY_ROTATE_HFLIP, 0);//旋转90°
    // esKRNL_TimeDly(500);
    if(mp == NULL)
        return 0;
    esMODS_MIoctrl(mp, VIDEO_PLAY_STOP, 0, 0);

    // __log("befor close ");
    esMODS_MClose(mp);
    esKRNL_TimeDly(10);
     __log("end of close ");
    esMODS_MUninstall(mid);
    esKRNL_TimeDly(10);
    __log("end of esMODS_MUninstall ");

    return 0;
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_video_test_close, __cmd_video_test_close, cmd_video_test_close);
