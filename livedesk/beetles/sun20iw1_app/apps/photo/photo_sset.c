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
#include "photo_sset.h"
#include "dtv_func_interface.h"
//////////////////////////////////////////////////////////////////////////
////////////////////sset 配置
//子菜单--播放特效
__sset_item_para_t submenu_slideshow[] =
{
    {
        //默认无切换效果
        "photo\\slideshow\\default",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_DEFAULT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, DEFAULT_SWITCH},
        0,
        NULL,
    },
    {
        //淡入淡出
        "photo\\slideshow\\fade_in_out",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_FADE_IN_FADE_OUT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, FADE_IN_OUT},
        0,
        NULL,
    },
    {
        //水平百叶窗
        "photo\\slideshow\\persian_blind_h",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_PERSIAN_BLINDS_H, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, PERSIAN_BLIND_H},
        0,
        NULL,
    },
    {
        //垂直百叶窗
        "photo\\slideshow\\persian_blind_v",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_PERSIAN_BLINDS_V, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, PERSIAN_BLIND_V},
        0,
        NULL,
    },
    {
        //向上滑动
        "photo\\slideshow\\slid_up",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SLIDE_UP, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, SLID_UP},
        0,
        NULL,
    },
    {
        //向下滑动
        "photo\\slideshow\\slid_down",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SLIDE_DOWN, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, SLID_DOWN},
        0,
        NULL,
    },
    {
        //向左滑动
        "photo\\slideshow\\slid_left",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SLIDE_LEFT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, SLID_LEFT},
        0,
        NULL,
    },
    {
        //向右滑动
        "photo\\slideshow\\slid_right",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SLIDE_RIGHT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, SLID_RIGHT},
        0,
        NULL,
    },
    {
        //向上展开
        "photo\\slideshow\\stretch_up",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_STRETCH_UP, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, STRETCH_UP},
        0,
        NULL,
    },
    {
        //向下展开
        "photo\\slideshow\\stretch_down",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_STRETCH_DOWN, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, STRETCH_DOWN},
        0,
        NULL,
    },
    {
        //向左展开
        "photo\\slideshow\\stretch_left",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_STRETCH_LEFT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, STRETCH_LEFT},
        0,
        NULL,
    },
    {
        //向右展开
        "photo\\slideshow\\stretch_right",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_STRETCH_RIGHT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, STRETCH_RIGHT},
        0,
        NULL,
    },
    {
        //马赛克
        "photo\\slideshow\\mosaic",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_MOSAIC, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, MOSAIC},
        0,
        NULL,
    },
    {
        //缩小
        "photo\\slideshow\\room_in",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_ZOOM_OUT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, ROOM_IN},
        0,
        NULL,
    },
    {
        //放大
        "photo\\slideshow\\room_out",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_ZOOM_IN, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, ROOM_OUT},
        0,
        NULL,
    },
    {
        //随机
        "photo\\slideshow\\fade_in_out",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_RANDOM_SEL, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, ROOM_RANDOM},
        0,
        NULL,
    },
    {
        //退出
        "photo\\slideshow\\exit",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_SLIDESHOW, ROOM_EXIT},
        0,
        NULL,
    },
};

//子菜单--播放速度
__sset_item_para_t submenu_interval[] =
{
    {
        // fast
        "photo\\interval\\fast",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_FAST, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_INTERVAL, PHOTO_SPEEDFLAG_FAST},
        0,
        NULL,
    },
    {
        // middle
        "photo\\interval\\middle",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_MIDDLE, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_INTERVAL, PHOTO_SPEEDFLAG_MIDDLE},
        0,
        NULL,
    },
    {
        // slow
        "photo\\interval\\slow",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SLOW, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_INTERVAL, PHOTO_SPEEDFLAG_SLOW},
        0,
        NULL,
    },
    {
        // exit
        "photo\\interval\\exit",
        1,
        TYPE_OPT,
        {
            {0, 0, ID_PHOTO_ICON_LIST_SEL_BMP, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_INTERVAL, PHOTO_SPEEDFLAG_EXIT},
        0,
        NULL,
    },
    /*{// user define
        "photo\\interval\\user",
        1,
        TYPE_SPIN,
        {
            {0,0,0,ID_PHOTO_BTN_SUB_F_BMP,ID_PHOTO_BTN_ADD_F_BMP,ID_PHOTO_BTN_SUB_N_BMP,ID_PHOTO_BTN_ADD_N_BMP,0},
            {0,0},
            10,   // 当前值
            59,   // 最大值
            1,    // 最小值
            0,
            NULL,
        },
        {PHOTO_SSET_NC_INTERVAL_USER, 0},
        0,
        NULL,
    },*/
};

//主菜单
static __sset_item_para_t mainmenu[] =
{
    {
        // ID_MMENU_EXIT
        "photo\\exit",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_EXIT_F_BMP, ID_PHOTO_BTN_EXIT_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_RETURN, STRING_PHOTO_RETURN},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_EXIT, 0},

        0,
        NULL,
    },
    {
        // ID_MMENU_BG_MUSIC
        "photo\\BGMusic",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_BGMUSICON_F_BMP, ID_PHOTO_BTN_BGMUSICON_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_MUSIC, STRING_PHOTO_MUSIC},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_BGMUSIC, 0},

        0,
        NULL,
    },
#if 1
    {
        // ID_MMENU_ZOOM
        "photo\\zoom",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_ZOOM_F_BMP, ID_PHOTO_BTN_ZOOM_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_ZOOM, STRING_PHOTO_ZOOM},
            0,
            0,
            0,
            0,
            NULL,
        },
        {PHOTO_SSET_NC_ZOOM, 0},

        0,
        NULL,
    },
    {
        // ID_MMENU_SLIDESHOW
        "photo\\slide show",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_SLIDESHOW_F_BMP, ID_PHOTO_BTN_SLIDESHOW_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SLIDE_EFFICT, STRING_PHOTO_SLIDE_EFFICT},
            0,
            0,
            0,
            0,
            NULL,
        },
        {PHOTO_SSET_NC_SLIDESHOW, PHOTO_SSET_NC_ID_UNKNOW},
        BEETLES_TBL_SIZE(submenu_slideshow),
        submenu_slideshow,
    },
#endif
    {
        // ID_MMENU_FILEINFO
        "photo\\fileinfo",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_FILEINFO_F_BMP, ID_PHOTO_BTN_FILEINFO_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_INFO, STRING_PHOTO_INFO},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_FILEINFO, 0},

        0,
        NULL,
    },
    {
        // ID_MMENU_AUTOPLAY
        "photo\\autoplay",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_AUTOPLAY_F_BMP, ID_PHOTO_BTN_AUTOPLAY_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_PLAY, STRING_PHOTO_PLAY},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_AUTOPLAY, 0},

        0,
        NULL,
    },
    {
        // ID_MMENU_INTERVAL
        "photo\\interval",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_INTERVAL_F_BMP, ID_PHOTO_BTN_INTERVAL_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_SPEED, STRING_PHOTO_SPEED},
            0,
            0,
            0,
            0,
            {NULL},
        },
        {PHOTO_SSET_NC_INTERVAL, PHOTO_SSET_NC_ID_UNKNOW},

        BEETLES_TBL_SIZE(submenu_interval),
        submenu_interval,
    },
#if 1
    {
        // ID_MMENU_SAVE_BG
        "photo\\save_bg",
        1,
        TYPE_TBAR,
        {
            {ID_PHOTO_BTN_SETBG_F_BMP, ID_PHOTO_BTN_SETBG_N_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_PHOTO_BACKGROUND, STRING_PHOTO_BACKGROUND},
            0,
            0,
            0,
            0,
            NULL,
        },
        {PHOTO_SSET_NC_SAVE_BG, 0},

        0,
        NULL,
    },
#endif
};

//sset配置入口
__sset_item_para_t photo_sset_arry[] =
{
    {
        "photo",                 // 名称
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

//重置为默认值
static void Photo_ParaReset(void)
{
    __s32 i;
#if 0

    for (i = mainmenu[ID_MMENU_SLIDESHOW].n; i--;)
    {
        submenu_slideshow[i].item_attr.opt = 0;
    }

#endif
    //  submenu_slideshow[DEFAULT_SWITCH].item_attr.opt = 1;

    for (i = mainmenu[ID_MMENU_INTERVAL].n; i--;)
    {
        submenu_interval[i].item_attr.opt = 0;
    }

    //  submenu_interval[PHOTO_SPEEDFLAG_MIDDLE].item_attr.opt = 1;
    return;
}

static void Photo_InitSetting(void)
{
    //读注册表来相关初始化数据
    __s32 val;
    get_photo_value(val, photo_get_effect);

    if (ValueBetweenRange(0, val, ROOM_OUT + 1))
    {
        submenu_slideshow[val].item_attr.opt = 1;
    }
    else
    {
        ASSERT(0);
        submenu_slideshow[DEFAULT_SWITCH].item_attr.opt = 1;
    }

    get_photo_value(val, photo_get_speedflag);

    if (ValueBetweenRange(PHOTO_SPEEDFLAG_FAST, val, PHOTO_SPEEDFLAG_SLOW/*PHOTO_SPEEDFLAG_MANNUAL*/))//TODO:
    {
        submenu_interval[val].item_attr.opt = 1;
    }
    else
    {
        submenu_interval[PHOTO_SPEEDFLAG_FAST].item_attr.opt = 1;//..
    }
}
void Photo_sset_process_for_tvout(void)
{
    static __u8 is_ready_process = EPDK_FALSE;
    __u8 Index = 0, total_item = BEETLES_TBL_SIZE(mainmenu);

    if (is_ready_process == EPDK_TRUE)
    {
        return ;
    }

    for (Index = 2; Index + 2 < total_item; Index ++)
    {
        eLIBs_memset(&mainmenu[Index], 0x00, sizeof(__sset_item_para_t));
        eLIBs_memcpy(&mainmenu[Index], &mainmenu[Index + 2], sizeof(__sset_item_para_t));
    }

    photo_sset_arry[0].n -= 2;
    is_ready_process = EPDK_TRUE;
    return ;
}

//创建及初始化SSET创建参数实例
__sset_create_para_t *Photo_SSET_Build_CreatePara(H_WIN h_win)
{
    __sset_create_para_t *create_para;
    BallocType(create_para, __sset_create_para_t);
    ZeroTypeMem(create_para, __sset_create_para_t);
    Photo_ParaReset();
    create_para->name       = "photo_sset";
    create_para->sid        = ID_PHOTO_SCENE_SSET; // 1
    create_para->pwin       = h_win;

    if (is_on_tv())//TV-OUT
    {
        Photo_sset_process_for_tvout();
    }

    create_para->attr       = photo_sset_arry;
    create_para->bfont      = SWFFont;
    create_para->sfont      = SWFFont;
    create_para->icon_res   = NULL; // TODO:to remove ?
    create_para->icon_style = 0;    // TODO:to remove ?
    create_para->lang_res   = NULL; // TODO:to remove ?
    create_para->lang_style = NULL; // TODO:to remove ?
    create_para->draw_code  = get_language_charset();
    create_para->time_speed = SSET_DEFAULLT_TIME_SPEED;
    __inf("draw_code = %d\n", create_para->draw_code);
    Photo_InitSetting();
    return create_para;
}

//销毁SSET创建参数实例
// __s32 Photo_SSET_Destroy_CreatePara(__sset_create_para_t* create_para)
// {
//  BFreeType(create_para, __sset_create_para_t);
//
//     return EPDK_OK;
// }

// __s32 Subtitle_ListCreate(void)
// {
//  __subtitle_profile_t    subtitle_para;
//  __sset_item_para_t*     sset_data;
//  __s32 itemnum, i;
//
//  robin_get_subtitle_list(&subtitle_para);
//
//  __inf("subtitle_para.nSubtitleStrmNum = %d\n", subtitle_para.nSubtitleStrmNum);
//  if (subtitle_para.nSubtitleStrmNum == 0)
//      itemnum = 1;
//  else
//      itemnum = subtitle_para.nSubtitleStrmNum;
//
//  /*动态创建创建SSET参数*/
//  sset_data = (__sset_item_para_t*)esMEMS_Balloc(sizeof(__sset_item_para_t) * itemnum);
//  eLIBs_memset(sset_data, 0x0, sizeof(__sset_item_para_t) * itemnum);
//
//  for (i = 0; i < itemnum; i++)
//  {
//      app_sprintf(sset_data[i].name, "movie\\subtitle\\subtitle%d", i);
//      sset_data[i].item_sta           = 1;
//      sset_data[i].item_type          = TYPE_OPT;
//      sset_data[i].item_msg.msg_id    = PHOTO_SSET_NC_ZOOM;
//      sset_data[i].item_msg.msg_value = i;
//      sset_data[i].n                  = 0;
//      sset_data[i].childarray         = NULL;
//      sset_data[i].item_attr.icon[2] = ID_MOVIE_STARSTAR_BMP;
//      if (subtitle_para.nSubtitleStrmNum == 0)
//          eLIBs_strncpy(sset_data[i].item_attr.text, "no sub", 128);
//      else if (subtitle_para.nSubtitleStrmNum == 1)
//          eLIBs_strncpy(sset_data[i].item_attr.text, "subtitle0", 128);
//      else
//          eLIBs_strncpy(sset_data[i].item_attr.text, (char*)subtitle_para.SubtitleStreamList[i].lang, 128);
//
//      if (i == robin_get_subtitle_index())
//      {
//          __inf("opt select i = %d\n", i);
//          sset_data[i].item_attr.opt = 1;
//      }
//  }
//
//  SSET_Insert("movie\\subtitle", sset_data, itemnum, SSET_INSERT_CHLD);
//  esMEMS_Bfree(sset_data, sizeof(__sset_item_para_t) * itemnum);
//
//  return 0;
//
// }
