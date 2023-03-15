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
#include <log.h>
#include "movie_sset.h"
//////////////////////////////////////////////////////////////////////////
////////////////////sset 配置

static __sset_item_para_t submenu_playmode[] =
{
    {
        "movie\\playmode\\rotate_all",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_ROTATE_ALL, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, RAT_PLAY_MODE_ROTATE_ALL},
        0,
        NULL,
    },
    {
        "movie\\playmode\\rotate_one",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_ROTATE_ONE, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, RAT_PLAY_MODE_ROTATE_ONE},
        0,
        NULL,
    },
    {
        "movie\\playmode\\sequence",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_SEQUENCE, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, RAT_PLAY_MODE_SEQUENCE},
        0,
        NULL,
    },
    {
        "movie\\playmode\\only_one",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_ONLY_ONE, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, RAT_PLAY_MODE_ONLY_ONCE},
        0,
        NULL,
    },
    {
        "movie\\playmode\\random",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_RANDOM, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, RAT_PLAY_MODE_RANDOM},
        0,
        NULL,
    },
    {
        //退出
        "movie\\playmode\\exit",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, RAT_PLAY_MODE_EXIT},
        0,
        NULL,
    },
};

static __sset_item_para_t submenu_channel[] =
{
    {
        "movie\\channel\\stereo",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_CHANNEL_STEREO, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_CHANNEL, AUDIO_DEV_CHANNEL_STEREO},
        0,
        NULL,
    },
    {
        "movie\\channel\\left",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_CHANNEL_LEFT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_CHANNEL, AUDIO_DEV_CHANNEL_LEFT},
        0,
        NULL,
    },
    {
        "movie\\channel\\right",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_CHANNEL_RIGHT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_CHANNEL, AUDIO_DEV_CHANNEL_RIGHT},
        0,
        NULL,
    },
    {
        //退出
        "movie\\channel\\exit",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_CHANNEL, AUDIO_DEV_CHANNEL_EXIT},
        0,
        NULL,
    },
};

static __sset_item_para_t submenu_track_temp[] =
{
    {
        "movie\\track\\track0",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_TRACK0, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 0},
        0,
        NULL,
    },
    {
        "movie\\track\\track1",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_TRACK1, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 1},
        0,
        NULL,
    },
    {
        "movie\\track\\track2",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_TRACK2, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 2},
        0,
        NULL,
    },
    {
        "movie\\track\\track3",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_TRACK3, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 3},
        0,
        NULL,
    },
    {
        "movie\\track\\track4",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_TRACK4, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 4},
        0,
        NULL,
    },
    {
        "movie\\track\\track5",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_TRACK5, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 5},
        0,
        NULL,
    },
    {
        "movie\\track\\exit",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, 6},
        0,
        NULL,
    },
};
static __sset_item_para_t submenu_track[BEETLES_TBL_SIZE(submenu_track_temp)] = {0};

static __sset_item_para_t submenu_zoom[] =
{
    {
        "movie\\zoom\\video_ratio",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_VIDEO_RATIO, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, ROBIN_ZOOM_FULL_SCREEN_VIDEO_RATIO},
        0,
        NULL,
    },
    {
        "movie\\zoom\\screen_ratio",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_SCREEN_RATIO, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO},
        0,
        NULL,
    },
    {
        "movie\\zoom\\screen_orgin",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_ORGIN_SIZE, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, ROBIN_ZOOM_ORIGIN_SIZE},
        0,
        NULL,
    },
    {
        "movie\\zoom\\zoom4r3",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_4R3, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, ROBIN_ZOOM_4R3},
        0,
        NULL,
    },
    {
        "movie\\zoom\\zoom16r9",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_16R9, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, ROBIN_ZOOM_16R9},
        0,
        NULL,
    },
    {
        //退出
        "movie\\zoom\\exit",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, ROBIN_ZOOM_EXIT},
        0,
        NULL,
    },

};

static __sset_item_para_t submenu_subtitle[] =
{
    {
        "movie\\subtitle\\bottom",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_SUB_BOTTOM, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_SUBTITLE, 0},
        0,
        NULL,
    },
    {
        "movie\\subtitle\\mid",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_SUB_MID, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_SUBTITLE, 1},
        0,
        NULL,
    },
    {
        "movie\\subtitle\\top",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            //{0,0, ID_MOVIE_SSET_CURSOR_BMP,0,0,0,0,0},
            {STRING_MOVIE_SUB_TOP, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_SUBTITLE, 2},
        0,
        NULL,
    },
    {
        //退出
        "movie\\subtitle\\exit",
        1,
        TYPE_OPT,
        {
            {ID_MOVIE_SUBMENU_FCS_BMP, ID_MOVIE_SUBMENU_UNFCS_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_SUBTITLE, 3},
        0,
        NULL,
    },
};

typedef enum
{
    movie_sset_return = 0,
    movie_sset_pre,
    movie_sset_play,
    movie_sset_next,
    movie_sset_zoom,
    movie_sset_subtitle,
    movie_sset_playmode,
    movie_sset_channel,
    movie_sset_track,
    movie_sset_tvout,
    movie_sset_ab,
} movie_sset_mainmenu_e;

//主菜单
static __sset_item_para_t mainmenu[] =
{
    {
        "movie\\return",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_RETURN_F_BMP, ID_MOVIE_SSET_RETURN_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_RETURN, STRING_MOVIE_RETURN},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_RETURN, 0},
        0,
        NULL,
    },
    {
        "movie\\pre",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_PREV_F_BMP, ID_MOVIE_SSET_PREV_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_PRE, STRING_MOVIE_PRE},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PRE, 0},
        0,
        NULL,
    },
    {
        "movie\\playstate",
        1,
        TYPE_TBAR,
        {
            //{ID_MOVIE_SSET_PAUSE_F_BMP,ID_MOVIE_SSET_PLAY_UF_BMP,ID_MOVIE_SSET_PAUSE_UF_BMP ,ID_MOVIE_SSET_PLAY_F_BMP,0,0,0,0},
            {ID_MOVIE_SSET_PLAY_F_BMP, ID_MOVIE_SSET_PLAY_UF_BMP, ID_MOVIE_SSET_PAUSE_UF_BMP, ID_MOVIE_SSET_PAUSE_F_BMP, 0, 0, 0, 0},
            {STRING_MOVIE_PLAY, STRING_MOVIE_STOP},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAY, 0},
        0,
        NULL,
    },
    {
        "movie\\next",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_NEXT_F_BMP, ID_MOVIE_SSET_NEXT_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_NEXT, STRING_MOVIE_NEXT},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_NEXT, 0},
        0,
        NULL,
    },
    {
        "movie\\zoom",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_SCREENSET_F_BMP, ID_MOVIE_SSET_SCREENSET_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_ZOOM, STRING_MOVIE_ZOOM},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_ZOOM, MOVIE_SSET_NC_ID_UNKNOW},

        BEETLES_TBL_SIZE(submenu_zoom),
        submenu_zoom,
    },
    {
        "movie\\subtitle",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_SUBTITLE_F_BMP, ID_MOVIE_SSET_SUBTITLE_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_SUBTITLE, STRING_MOVIE_SUBTITLE},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_SUBTITLE, MOVIE_SSET_NC_ID_UNKNOW},

        BEETLES_TBL_SIZE(submenu_subtitle),
        submenu_subtitle,
    },
    {
        "movie\\playmode",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_PLAY_MODE_F_BMP, ID_MOVIE_SSET_PLAY_MODE_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_REPEAT, STRING_MOVIE_REPEAT},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_PLAYMODE, MOVIE_SSET_NC_ID_UNKNOW},

        BEETLES_TBL_SIZE(submenu_playmode),
        submenu_playmode,
    },
    {
        "movie\\channel",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_CHANNEL_F_BMP, ID_MOVIE_SSET_CHANNEL_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_CHANNEL, STRING_MOVIE_CHANNEL},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_CHANNEL, MOVIE_SSET_NC_ID_UNKNOW},

        BEETLES_TBL_SIZE(submenu_channel),
        submenu_channel,
    },
    {
        "movie\\track",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_TRACK_F_BMP, ID_MOVIE_SSET_TRACK_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_TRACK, STRING_MOVIE_TRACK},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_TRACK, MOVIE_SSET_NC_ID_UNKNOW},
        BEETLES_TBL_SIZE(submenu_track),
        submenu_track,
    },
    {
        "movie\\AB",
        1,
        TYPE_TBAR,
        {
            {ID_MOVIE_SSET_PLAY_MODE_F_BMP, ID_MOVIE_SSET_PLAY_MODE_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_MOVIE_AB, STRING_MOVIE_AB},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {MOVIE_SSET_NC_AB, 0},

        0,
        NULL,
    }
};

//sset配置入口
static __sset_item_para_t movie_sset_arry[] =
{
    {
        "movie",                 // 名称
        1,                       // 是否有效
        TYPE_NORMAL,             // 风格
        {
            {0, 0, 0, 0, 0, 0, 0, 0}, // 图标资源ID
            {0, 0},              // 文本资源ID
            0,                   // 当前值
            0,                   // 最大值
            0,                   // 最小值
            0,                   // OPT选中标示
            {NULL},                // 文本数据
        },
        {0, 0},                  // 消息码

        BEETLES_TBL_SIZE(mainmenu),   // 本项子菜单中的条目数
        mainmenu,                     // 本项子菜单地址
    },
};

//读注册表来相关初始化数据
static void Movie_InitSetting(__movie_sset_list_para_t *sset_list_para)
{
    {
        __s32 i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(submenu_playmode) ; i++)
        {
            if (submenu_playmode[i].item_msg.msg_value == sset_list_para->playmode)
            {
                submenu_playmode[i].item_attr.opt = 1;
            }
            else
            {
                submenu_playmode[i].item_attr.opt = 0;
            }
        }
    }
    {
        __s32 i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(submenu_zoom) ; i++)
        {
            if (submenu_zoom[i].item_msg.msg_value == sset_list_para->zoom)
            {
                submenu_zoom[i].item_attr.opt = 1;
            }
            else
            {
                submenu_zoom[i].item_attr.opt = 0;
            }
        }
    }
    {
        __s32 i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(submenu_channel) ; i++)
        {
            if (submenu_channel[i].item_msg.msg_value == sset_list_para->channel)
            {
                submenu_channel[i].item_attr.opt = 1;
            }
            else
            {
                submenu_channel[i].item_attr.opt = 0;
            }
        }
    }
    {
        __s32 i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(submenu_subtitle) ; i++)
        {
            if (submenu_subtitle[i].item_msg.msg_value == sset_list_para->sub_pos)
            {
                submenu_subtitle[i].item_attr.opt = 1;
            }
            else
            {
                submenu_subtitle[i].item_attr.opt = 0;
            }
        }
    }
    {
        __s32 i;
        __s32 track_index;
        __s32 track_num;
        __audstream_profile_t   track_para;
        robin_get_track_info(&track_para);
        __msg("track_para.nAudStrmNum=%d\n", track_para.nAudStrmNum);
        track_index = robin_get_track_index();
        track_num = track_para.nAudStrmNum;
#if 0

        if (track_num < 0 || track_num > BEETLES_TBL_SIZE(submenu_track))
        {
            track_num = BEETLES_TBL_SIZE(submenu_track);
        }

#else

        if (track_num < 0 || track_num > BEETLES_TBL_SIZE(submenu_track_temp) - 1)
        {
            track_num = BEETLES_TBL_SIZE(submenu_track_temp) - 1;
        }

#endif

        if (track_index < 0 || track_index >= track_num)
        {
            track_index = 0;
        }

        mainmenu[movie_sset_track].n = track_num + 1;
        /*for exit*/
        eLIBs_memset(submenu_track, 0x00, sizeof(submenu_track));
        eLIBs_memcpy(submenu_track, submenu_track_temp, sizeof(submenu_track));
        eLIBs_memcpy(&submenu_track[track_num], &submenu_track_temp[BEETLES_TBL_SIZE(submenu_track_temp) - 1], sizeof(__sset_item_para_t));

        for (i = 0 ; i < track_num ; i++)
        {
            if (submenu_track[i].item_msg.msg_value == track_index)
            {
                submenu_track[i].item_attr.opt = 1;
            }
            else
            {
                submenu_track[i].item_attr.opt = 0;
            }
        }
    }
}

//创建及初始化SSET创建参数实例
__sset_create_para_t *Movie_SSET_Build_CreatePara(H_WIN hManwin, __u32 sub_id
        , __movie_sset_list_para_t *sset_list_para)
{
    __sset_create_para_t *create_para;
    create_para = esMEMS_Balloc(sizeof(__sset_create_para_t));

    if (!create_para)
    {
        __msg("esMEMS_Balloc fail...\n");
        return NULL;
    }

    eLIBs_memset(create_para, 0, sizeof(__sset_create_para_t));
    Movie_InitSetting(sset_list_para);
    create_para->name       = "movie_sset";
    create_para->sid        = sub_id; //
    create_para->pwin       = hManwin;
    create_para->attr       = movie_sset_arry;
    create_para->bfont      = SWFFont;
    create_para->sfont      = SWFFont;
    create_para->icon_res   = NULL;
    create_para->icon_style = 0;
    create_para->lang_res   = NULL;
    create_para->lang_style = NULL;
    create_para->draw_code  = get_language_charset();
    create_para->time_speed = SSET_DEFAULLT_TIME_SPEED;
    __inf("draw_code = %d\n", create_para->draw_code);
    return create_para;
}

//销毁SSET创建参数实例
__s32 Movie_SSET_Destroy_CreatePara(__sset_create_para_t *create_para)
{
    if (create_para)
    {
        esMEMS_Bfree(create_para, sizeof(__sset_create_para_t));
    }

    return EPDK_OK;
}
