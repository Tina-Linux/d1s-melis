#include <log.h>
#include"ebook_sset.h"

static reg_ebook_para_t *reg_ebook_para = 0;


/* 背景音乐选项*/
__sset_item_para_t ebook_bg_music_array[ ] =
{
    {
        "ebook\\bg_music\\open",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_ON, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_MUSIC, BG_MUSIC_OPEN},

        0,
        NULL,
    },
    {
        "ebook\\bg_music\\close",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_OFF, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_MUSIC, BG_MUSIC_CLOSE},

        0,
        NULL,
    }
};

/* 字体颜色选项*/
__sset_item_para_t ebook_font_color_array[ ] =
{

    {
        "ebook\\font_color\\black",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_BLACK, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_FCOLOR, FONT_COLOR_BLACK},

        0,
        NULL,
    },

    {
        "ebook\\font_color\\green",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_GREEN, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_FCOLOR, FONT_COLOR_GREEN},

        0,
        NULL,
    },
    {
        "ebook\\font_color\\gray",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_BLUE, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_FCOLOR, FONT_COLOR_BLUE},

        0,
        NULL,
    },

    {
        "ebook\\font_color\\exit",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_FCOLOR, FONT_COLOR_EXIT},

        0,
        NULL,
    }
};

/* 自动播放选项*/
__sset_item_para_t ebook_auto_play_array[ ] =
{
    {
        "ebook\\auto_play\\stop",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_OFF, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_AUTO, AUTO_TIME_0},

        0,
        NULL,
    },
    {
        "ebook\\auto_play\\fast",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_FAST, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_AUTO, AUTO_TIME_10},

        0,
        NULL,
    },
    {
        "ebook\\auto_play\\middle",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_MIDDLE, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_AUTO, AUTO_TIME_20},

        0,
        NULL,
    },
    {
        "ebook\\auto_play\\slow",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_SLOW, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_AUTO, AUTO_TIME_30},

        0,
        NULL,
    },
    {
        "ebook\\auto_play\\exit",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_AUTO, AUTO_TIME_EXIT},

        0,
        NULL,
    }


};

/* tts 选项*/
__sset_item_para_t ebook_tts_array[ ] =
{
    {
        "ebook\\tts\\on",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_ON, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_TTS, TTS_ON},

        0,
        NULL,
    },
    {
        "ebook\\tts\\off",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_OFF, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_TTS, TTS_OFF},

        0,
        NULL,
    },
    {
        "ebook\\tts\\exit",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_ROOT_EXIT, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_TTS, TTS_EXIT},

        0,
        NULL,
    }


};

__sset_item_para_t ebook_obmk_array[ ] = //..
{



    {
        "ebook\\openbmk\\lab1",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_LABEL1, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, LABEL1},

        0,
        NULL,
    },
    {
        "ebook\\openbmk\\lab2",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_LABEL2, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, LABEL2},

        0,
        NULL,
    },
    {
        "ebook\\openbmk\\lab3",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_LABEL3, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, LABEL3},

        0,
        NULL,
    },

    {
        "ebook\\openbmk\\lab4",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_LABEL4, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, LABEL4},

        0,
        NULL,
    },
    {
        "ebook\\openbmk\\lab5",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_LABEL5, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, LABEL5},

        0,
        NULL,
    },
    {
        "ebook\\openbmk\\lab6",
        1,
        TYPE_OPT,

        {
            {0, 0, ID_EBOOK_SELECT_OPT_BMP, 0, 0, 0, 0, 0},
            {STRING_EBOOK_LABEL6, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, LABEL6},

        0,
        NULL,
    },

};




/* tbar 选项*/
__sset_item_para_t ebook_tbar_array[ ] =
{
#if 0
    {
        "epdf\\exit",
        1,
        TYPE_TBAR,              /* 退出选项 */

        {
            {ID_EBOOK_EXIT_F_BMP, ID_EBOOK_EXIT_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_RETURN, STRING_EBOOK_RETURN},
            0,
            0,
            0,
            0,
        },

        {EBOOK_SET_EXIT, 0},

        0,
        NULL,
    },
    {
        "epdf\\music",
        1,
        TYPE_TBAR,              /* 音乐快捷 */

        {
            {ID_EBOOK_MUSIC_F_BMP, ID_EBOOK_MUSIC_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_MUSIC, STRING_EBOOK_MUSIC},
            0,
            0,
            0,
            0,
        },

        {EBOOK_SET_MUSIC, 0},

        0,//sizeof(ebook_bg_music_array)/sizeof(__sset_item_para_t),
        NULL,//ebook_bg_music_array,
    },
#endif
    {
        "epdf\\fcolor",
        1,
        TYPE_TBAR,              /* 字体 颜色 */

        {
            {ID_EBOOK_FONTCOLOR_F_BMP, ID_EBOOK_FONTCOLOR_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_FONT, STRING_EBOOK_FONT},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_FCOLOR, EBOOK_SSET_NC_ID_UNKNOW},

        sizeof(ebook_font_color_array) / sizeof(__sset_item_para_t),
        ebook_font_color_array,
    },
    {
        "epdf\\auto_play",
        1,
        TYPE_TBAR,              /* 自动播放选项 */

        {
            {ID_EBOOK_AUTO_F_BMP, ID_EBOOK_AUTO_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_SPEED, STRING_EBOOK_SPEED},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_AUTO, EBOOK_SSET_NC_ID_UNKNOW},

        sizeof(ebook_auto_play_array) / sizeof(__sset_item_para_t),
        ebook_auto_play_array,
    },
#if 0

    {
        "epdf\\tts",
        1,
        TYPE_TBAR,              /* 自动播放选项 */

        {
            {ID_EBOOK_TTS_F_BMP, ID_EBOOK_TTS_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_TTS, STRING_EBOOK_TTS},
            0,
            0,
            0,
            0,
        },

        {EBOOK_SET_TTS, EBOOK_SSET_NC_ID_UNKNOW},

        sizeof(ebook_tts_array) / sizeof(__sset_item_para_t),
        ebook_tts_array,
    },
#endif


#if 1
    {
        "epdf\\goto_page",
        1,
        TYPE_TBAR,              /* 退出选项 */

        {
            {ID_EBOOK_GOTO_PAGE_F_BMP, ID_EBOOK_GOTO_PAGE_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_GOTO, STRING_EBOOK_GOTO},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_GOTO_PAGE, 0},

        0,
        NULL,
    },
    //  {
    //      "epdf\\rotate",
    //      1,
    //      TYPE_TBAR,              /* 自动选项 */
    //
    //      {
    //          {ID_EPDF_AUTO_F_BMP,ID_EPDF_AUTO_UF_BMP, 0,0,0,0,0,0},
    //          {0,0},
    //          0,
    //          0,
    //          0,
    //          0,
    //      },
    //
    //      {EPDF_SET_ROTATE,0},
    //
    //      0,
    //      NULL,
    //  },

    {
        //....
        "epdf\\openbmk",
        1,
        TYPE_TBAR,              /* 打开书签选项 */

        {
            {ID_EBOOK_BMARK_F_BMP, ID_EBOOK_BMARK_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_OBMK, STRING_EBOOK_OBMK},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_OBMK, EBOOK_SSET_NC_ID_UNKNOW},

        sizeof(ebook_obmk_array) / sizeof(__sset_item_para_t),
        ebook_obmk_array,
    },
    {
        "epdf\\savebmk",
        1,
        TYPE_TBAR,              /* 保存书签选项 */

        {
            {ID_EBOOK_FONTCOLOR_F_BMP, ID_EBOOK_FONTCOLOR_UF_BMP, 0, 0, 0, 0, 0, 0},
            {STRING_EBOOK_SBMK, STRING_EBOOK_SBMK},
            0,
            0,
            0,
            0,
            {0}
        },

        {EBOOK_SET_SBMK, 0},

        0,
        NULL,
    },
#endif
    //  {
    //      "epdf\\bcolor",
    //      1,
    //      TYPE_TBAR,              /* 颜色 */
    //
    //      {
    //          {ID_EPDF_BCOLOR_F_BMP,ID_EPDF_BCOLOR_UF_BMP, 0,0,0,0,0,0},
    //          {0,0},
    //          0,
    //          0,
    //          0,
    //          0,
    //      },
    //
    //      {0,0},
    //
    //      sizeof(bcolor_array)/sizeof(__sset_item_para_t),
    //      bcolor_array,
    //  },
    //  {
    //      "epdf\\prev",
    //      1,
    //      TYPE_TBAR,              /* 上一页 */
    //
    //      {
    //          {ID_EPDF_PREV_F_BMP,ID_EPDF_PREV_UF_BMP, 0,0,0,0,0,0},
    //          {0,0},
    //          0,
    //          0,
    //          0,
    //          0,
    //      },
    //
    //      {EPDF_SET_PREV,0},
    //
    //      0,
    //      NULL,
    //  },
    //  {
    //      "epdf\\next",
    //      1,
    //      TYPE_TBAR,              /* 下一页 */
    //
    //      {
    //          {ID_EPDF_NEXT_F_BMP,ID_EPDF_NEXT_UF_BMP, 0,0,0,0,0,0},
    //          {0,0},
    //          0,
    //          0,
    //          0,
    //          0,
    //      },
    //
    //      {EPDF_SET_NEXT,0},
    //
    //      0,
    //      NULL,
    //  },
};

__sset_item_para_t ebook_sset_arry[ ] =
{
    {
        "ebook",
        1,
        TYPE_NORMAL,

        {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0},
            0,
            0,
            0,
            0,
            {0}
        },

        {0, 0},

        sizeof(ebook_tbar_array) / sizeof(__sset_item_para_t),
        ebook_tbar_array,
    }
};

//根据注册表初始化sset组件中的各个选项
__s32 ebook_sset_init_focus_item_by_reg(__sset_create_para_t *sset_para)
{
    __s32   i ;
    __s32   ret ;
    reg_ebook_para = dsk_reg_get_para_by_app(REG_APP_EBOOK);

    if (!reg_ebook_para)
    {
        __msg("ebook read reg error .........\n");
        return EPDK_FAIL;
    }

    ret = is_app_exist(APP_MUSIC);  //查询当前音乐是否存在

    if (EPDK_TRUE == ret)    //音乐存在
    {
        ebook_bg_music_array[BG_MUSIC_OPEN].item_attr.opt = 1;
    }
    else
    {
        ebook_bg_music_array[BG_MUSIC_CLOSE].item_attr.opt = 1;
    }

    /*TTS */
    //..reg_ebook_para->tts_switch=0;//..
    switch (reg_ebook_para->tts_switch)
    {
        case 0:
        {
            ebook_tts_array[0].item_attr.opt = 1;
            ebook_tts_array[1].item_attr.opt = 0;
            break;
        }

        case  1:
        {
            ebook_tts_array[0].item_attr.opt = 0;
            ebook_tts_array[1].item_attr.opt = 1;
            break;
        }

        default:
        {
            ebook_tts_array[0].item_attr.opt = 1;
            ebook_tts_array[1].item_attr.opt = 0;
            break;
        }
    }

    //字体颜色
    //reg_ebook_para->text_colour = GUI_BLACK;//..
    if (reg_ebook_para->text_colour == APP_COLOR_BLACK)
    {
        ebook_font_color_array[FONT_COLOR_BLACK].item_attr.opt = 1;
        ebook_font_color_array[FONT_COLOR_GREEN].item_attr.opt = 0;
        ebook_font_color_array[FONT_COLOR_BLUE].item_attr.opt = 0;
    }
    else if (reg_ebook_para->text_colour == APP_COLOR_GREEN)
    {
        ebook_font_color_array[FONT_COLOR_GREEN].item_attr.opt = 1;
        ebook_font_color_array[FONT_COLOR_BLUE].item_attr.opt = 0;
        ebook_font_color_array[FONT_COLOR_BLACK].item_attr.opt = 0;
    }
    else if (reg_ebook_para->text_colour == GUI_BLUE)
    {
        ebook_font_color_array[FONT_COLOR_BLUE].item_attr.opt = 1;
        ebook_font_color_array[FONT_COLOR_BLACK].item_attr.opt = 0;
        ebook_font_color_array[FONT_COLOR_GREEN].item_attr.opt = 0;
    }
    else
    {
        ebook_font_color_array[FONT_COLOR_GREEN].item_attr.opt = 0;
        ebook_font_color_array[FONT_COLOR_BLUE].item_attr.opt = 0;
        ebook_font_color_array[FONT_COLOR_BLACK].item_attr.opt = 1;
        reg_ebook_para->text_colour = APP_COLOR_BLACK ;
    }

    //自动播放
    __wrn("...................reg_ebook_para->switch_time=%d/n", reg_ebook_para->switch_time);

    if (reg_ebook_para->switch_time == 10)   //10s 自动播放
    {
        ebook_auto_play_array[AUTO_TIME_10].item_attr.opt = 1;
        ebook_auto_play_array[AUTO_TIME_20].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_30].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_0].item_attr.opt = 0;
    }
    else if (reg_ebook_para->switch_time == 20)
    {
        ebook_auto_play_array[AUTO_TIME_20].item_attr.opt = 1;
        ebook_auto_play_array[AUTO_TIME_30].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_10].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_0].item_attr.opt = 0;
    }
    else if (reg_ebook_para->switch_time == 30)
    {
        ebook_auto_play_array[AUTO_TIME_30].item_attr.opt = 1;
        ebook_auto_play_array[AUTO_TIME_10].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_20].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_0].item_attr.opt = 0;
    }
    else
    {
        ebook_auto_play_array[AUTO_TIME_0].item_attr.opt = 1;
        ebook_auto_play_array[AUTO_TIME_30].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_10].item_attr.opt = 0;
        ebook_auto_play_array[AUTO_TIME_20].item_attr.opt = 0;
        reg_ebook_para->switch_time = 0 ;
    }

    //电子书签
    __wrn("reg_ebook_para->e_mark=%d\n", reg_ebook_para->e_mark);

    if (reg_ebook_para->e_mark == LABEL1)
    {
        ebook_obmk_array[LABEL1].item_attr.opt = 1;
        ebook_obmk_array[LABEL2].item_attr.opt = 0;
        ebook_obmk_array[LABEL3].item_attr.opt = 0;
        ebook_obmk_array[LABEL4].item_attr.opt = 0;
        ebook_obmk_array[LABEL5].item_attr.opt = 0;
        ebook_obmk_array[LABEL6].item_attr.opt = 0;
    }
    else if (reg_ebook_para->e_mark == LABEL2)
    {
        ebook_obmk_array[LABEL1].item_attr.opt = 0;
        ebook_obmk_array[LABEL2].item_attr.opt = 1;
        ebook_obmk_array[LABEL3].item_attr.opt = 0;
        ebook_obmk_array[LABEL4].item_attr.opt = 0;
        ebook_obmk_array[LABEL5].item_attr.opt = 0;
        ebook_obmk_array[LABEL6].item_attr.opt = 0;
    }
    else if (reg_ebook_para->e_mark == LABEL3)
    {
        ebook_obmk_array[LABEL1].item_attr.opt = 0;
        ebook_obmk_array[LABEL2].item_attr.opt = 0;
        ebook_obmk_array[LABEL3].item_attr.opt = 1;
        ebook_obmk_array[LABEL4].item_attr.opt = 0;
        ebook_obmk_array[LABEL5].item_attr.opt = 0;
        ebook_obmk_array[LABEL6].item_attr.opt = 0;
    }
    else if (reg_ebook_para->e_mark == LABEL4)
    {
        ebook_obmk_array[LABEL1].item_attr.opt = 0;
        ebook_obmk_array[LABEL2].item_attr.opt = 0;
        ebook_obmk_array[LABEL3].item_attr.opt = 0;
        ebook_obmk_array[LABEL4].item_attr.opt = 1;
        ebook_obmk_array[LABEL5].item_attr.opt = 0;
        ebook_obmk_array[LABEL6].item_attr.opt = 0;
    }
    else if (reg_ebook_para->e_mark == LABEL5)
    {
        ebook_obmk_array[LABEL1].item_attr.opt = 0;
        ebook_obmk_array[LABEL2].item_attr.opt = 0;
        ebook_obmk_array[LABEL3].item_attr.opt = 0;
        ebook_obmk_array[LABEL4].item_attr.opt = 0;
        ebook_obmk_array[LABEL5].item_attr.opt = 1;
        ebook_obmk_array[LABEL6].item_attr.opt = 0;
    }
    else if (reg_ebook_para->e_mark == LABEL6)
    {
        ebook_obmk_array[LABEL1].item_attr.opt = 0;
        ebook_obmk_array[LABEL2].item_attr.opt = 0;
        ebook_obmk_array[LABEL3].item_attr.opt = 0;
        ebook_obmk_array[LABEL4].item_attr.opt = 0;
        ebook_obmk_array[LABEL5].item_attr.opt = 0;
        ebook_obmk_array[LABEL6].item_attr.opt = 1;
    }

    return EPDK_OK;
}

//创建及初始化SSET创建参数实例
__sset_create_para_t *Ebook_SSET_Build_CreatePara(H_WIN h_win)
{
    __sset_create_para_t *create_para;
    create_para = (__sset_create_para_t *)My_Malloc(0, sizeof(__sset_create_para_t));

    if (!create_para)
    {
        __msg("memory alloc fail.........\n");
        return (__sset_create_para_t *)NULL;
    }

    eLIBs_memset(create_para, 0, sizeof(__sset_create_para_t));
    //Photo_ParaReset();
    create_para->name       = "ebook_sset";
    create_para->sid        = EBOOK_SSET_SCENE_ID;
    create_para->pwin       = h_win;
    create_para->attr       = ebook_sset_arry;
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


