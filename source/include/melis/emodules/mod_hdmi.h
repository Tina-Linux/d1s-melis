#ifndef __DRV_HDMI_H__
#define __DRV_HDMI_H__
#include <kconfig.h>
#include <const.h>

typedef struct hdmi_audio
{
    __u8    hw_intf;        /* 0:iis  1:spdif 2:pcm */
    __u16   fs_between;     /* fs */
    __u32   sample_rate;    /*sample rate*/
    __u8    clk_edge;       /* 0:*/
    __u8    ch0_en;         /* 1 */
    __u8    ch1_en;         /* 0 */
    __u8    ch2_en;         /* 0 */
    __u8    ch3_en;         /* 0 */
    __u8    word_length;    /* 32 */
    __u8    shift_ctl;      /* 0 */
    __u8    dir_ctl;        /* 0 */
    __u8    ws_pol;
    __u8    just_pol;
} hdmi_audio_t;

#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined(CONFIG_SOC_SUN20IW1)
typedef enum
{
    HDMI_CMD_SET_VIDEO_MOD,
    HDMI_CMD_GET_VIDEO_MOD,
    HDMI_CMD_OPEN,
    HDMI_CMD_CLOSE,
    HDMI_CMD_MOD_SUPPORT,
    HDMI_CMD_GET_HPD_STATUS,
    HDMI_CMD_AUDIO_ENABLE,
    HDMI_CMD_SET_AUDIO_PARA,
} __hdmi_cmd_t;
#elif defined CONFIG_SOC_SUN3IW1
typedef enum tag_HDMI_CMD
{
#if(EPDK_ARCH == EPDK_ARCH_SUNI)
    /* command cache on/off                         */
    HDMI_CMD_SET_VIDEO_MOD,
    HDMI_CMD_GET_VIDEO_MOD,
    HDMI_CMD_SET_AUDIO_PARA,
    HDMI_CMD_AUDIO_RESET_NOTIFY,            /*iis reset finish notify    */
    HDMI_CMD_CLOSE,                         /*iis reset finish notify    */

#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
    /* command cache on/off                         */
    HDMI_CMD_SET_VIDEO_MOD,
    HDMI_CMD_GET_VIDEO_MOD,
    HDMI_CMD_SET_AUDIO_PARA,
    HDMI_CMD_AUDIO_RESET_NOTIFY,            /*iis reset finish notify    */
    HDMI_CMD_CLOSE,                         /*iis reset finish notify    */
    HDMI_CMD_MOD_SUPPORT,                   /*判断某一种hdmi模式是否支持*/
    HDMI_CMD_AUDIO_ENABLE,
    HDMI_CMD_GET_HPD_STATUS,
    HDMI_CMD_OPEN,
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
    /* command cache on/off                         */
    HDMI_CMD_SET_VIDEO_MOD,
    HDMI_CMD_GET_VIDEO_MOD,
    HDMI_CMD_SET_AUDIO_PARA,
    HDMI_CMD_AUDIO_RESET_NOTIFY,            /*iis reset finish notify    */
    HDMI_CMD_CLOSE,                         /*iis reset finish notify    */
    HDMI_CMD_MOD_SUPPORT,                   /*判断某一种hdmi模式是否支持*/
    HDMI_CMD_AUDIO_ENABLE,
    HDMI_CMD_GET_HPD_STATUS,
    HDMI_CMD_OPEN,
#else

#error please select arch!

#endif
} __hdmi_cmd_t;
#endif

#endif
