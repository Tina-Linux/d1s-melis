#include <log.h>
#include "mod_init_i.h"
#include "mini_music_scene.h"
#include "mini_music_uipara.h"

/*********************************************************************************************************************/
#ifdef NAME_MAX
#undef NAME_MAX
#endif
#define NAME_MAX 128
#define MAX_FULL_FILE_PATH_LEN          ( 1*1024 )
#define APP_TITLE_MAX 128

/*********************************************************************************************************************/
typedef struct
{
    HTHEME  h_focus;
    HTHEME  h_unfocus;
} button_htheme_t;

typedef struct
{
    H_LYR   layer;
    H_WIN   frmwin;
} __mini_music_scene_para_t;

static GUI_FONT         *font;
static char             *songname;
static button_htheme_t  *button_htheme;
static static_para_t    *static_para;
static button_para_t    *button_para;
static H_WIN            mini_manwin = NULL;

static char *__get_file_name(const char *fullpath)
{
    int     len;
    char    *p;

    __inf("fullpath = %s", fullpath);

    len = eLIBs_strlen(fullpath);
    p   = (char *)(&fullpath[len]);

    while (*p != '\\' && p >= fullpath)
    {
        p--;
    }

    p++;

    __inf("p = %s", p);
    return p;
}

static void __get_audio_info(char *filename, dsk_wkm_audio_info_t *audio_info)
{
    dsk_wkm_get_audio_info(filename, audio_info);

    if (audio_info->title[0] != '\0')
    {
        if (audio_info->title_charset == EPDK_CHARSET_ENM_UNKNOWN)
        {
            audio_info->title_charset = EPDK_CHARSET_ENM_GBK;
        }
    }
    else
    {
        eLIBs_strcpy(audio_info->title, __get_file_name(filename));
        audio_info->title_charset = EPDK_CHARSET_ENM_UTF8;
    }
}

/*********************************************************************************************************************/
static void __button_bmp_update(H_WIN parent, __s32 bmp_id, __s32 button_id)
{
    button_htheme[button_id - MINI_STC_ID_NUM].h_focus      = dsk_theme_open(bmp_id);
    button_para  [button_id - MINI_STC_ID_NUM].focus_bmp    = dsk_theme_hdl2buf(button_htheme[button_id - MINI_STC_ID_NUM].h_focus);
    button_htheme[button_id - MINI_STC_ID_NUM].h_unfocus    = dsk_theme_open(bmp_id);
    button_para  [button_id - MINI_STC_ID_NUM].unfocus_bmp  = dsk_theme_hdl2buf(button_htheme[button_id - MINI_STC_ID_NUM].h_unfocus);

    GUI_ButtonSetFocusBmp(GUI_WinGetDlgItem(parent, button_id), button_para[button_id - MINI_STC_ID_NUM].focus_bmp);

    GUI_WinUpdate(GUI_WinGetDlgItem(parent, button_id), 0);
}

static void __songname_update(__gui_msg_t *msg)
{
    __s32   index;
    char    filename[NAME_MAX];
    dsk_wkm_audio_info_t    audio_info;

    index = dsk_wkm_npl_get_cur();

    dsk_wkm_npl_index2file(index, filename);

    __get_audio_info(filename, &audio_info);

    if (dsk_wkm_get_fsm_state() == CEDAR_STAT_PLAY)
    {
        static_para[MINI_STC_ID_NAME].ftxt_color  = GUI_WHITE;
        static_para[MINI_STC_ID_NAME].uftxt_color = GUI_WHITE;
    }
    else
    {
        static_para[MINI_STC_ID_NAME].ftxt_color  = GUI_RED;
        static_para[MINI_STC_ID_NAME].uftxt_color = GUI_RED;
    }

    GUI_StaticStopMove(GUI_WinGetDlgItem(msg->h_deswin, MINI_STC_ID_NAME));
    GUI_StaticSetText(GUI_WinGetDlgItem(msg->h_deswin, MINI_STC_ID_NAME), audio_info.title);
    GUI_StaticSetCharSet(GUI_WinGetDlgItem(msg->h_deswin, MINI_STC_ID_NAME), audio_info.title_charset);
    GUI_WinUpdate(GUI_WinGetDlgItem(msg->h_deswin, MINI_STC_ID_NAME), 0);
    GUI_StaticStartMove(GUI_WinGetDlgItem(msg->h_deswin, MINI_STC_ID_NAME));
}

static __s32 __draw_backgrd(H_WIN win)
{
    __miniplay_uipara_t *ui_para;
    void    *backgrd_theme;
    HTHEME  htheme;

    ui_para = miniplay_get_uipara();

    if (!ui_para)
    {
        __wrn("get ui para fail");
        return -1;
    }

    htheme  =  dsk_theme_open(0);

    backgrd_theme   = dsk_theme_hdl2buf(htheme);

    GUI_LyrWinSel(GUI_WinGetLyrWin(win));

    GUI_BMP_DrawEx(backgrd_theme, ui_para->para_bg.x, ui_para->para_bg.y);

    dsk_theme_close(htheme);
    return 0;
}

static H_WIN __item_songname_create(H_WIN parent, static_para_t *s_static_para, __miniplay_ui_para_item_t *position)
{
    __gui_ctlwincreate_para_t ctlwin_create_info;
    H_WIN   static_win;

    ctlwin_create_info.hParent         = parent;
    ctlwin_create_info.spClassName     = CTRL_STATIC;
    ctlwin_create_info.spCaption       = NULL;
    ctlwin_create_info.dwExStyle       = WS_NONE;
    ctlwin_create_info.dwStyle         = WS_VISIBLE;
    ctlwin_create_info.name            = "mini songname";
    ctlwin_create_info.attr            = s_static_para;
    ctlwin_create_info.id              = MINI_STC_ID_NAME;
    ctlwin_create_info.ctl_rect.x      = position->x;
    ctlwin_create_info.ctl_rect.y      = position->y;
    ctlwin_create_info.ctl_rect.width  = position->width;
    ctlwin_create_info.ctl_rect.height = position->height;

    static_win = GUI_CtrlWinCreate(&ctlwin_create_info);

    GUI_StaticStartMove(static_win);

    return static_win;
}

static H_WIN __item_button_create(H_WIN parent, button_para_t *s_button_para, __s32 item_id, __miniplay_ui_para_item_t *position)
{
    __gui_ctlwincreate_para_t   ctlwin_create_info;

    ctlwin_create_info.hParent         = parent;
    ctlwin_create_info.spClassName     = CTRL_BUTTON;
    ctlwin_create_info.spCaption       = NULL;
    ctlwin_create_info.dwExStyle       = WS_NONE;
    ctlwin_create_info.dwStyle         = WS_VISIBLE;
    ctlwin_create_info.name            = NULL;
    ctlwin_create_info.attr            = s_button_para;
    ctlwin_create_info.id              = item_id;
    ctlwin_create_info.ctl_rect.x      = position->x;
    ctlwin_create_info.ctl_rect.y      = position->y;
    ctlwin_create_info.ctl_rect.width  = position->width;
    ctlwin_create_info.ctl_rect.height = position->height;

    return (GUI_CtrlWinCreate(&ctlwin_create_info));
}

static __s32 __cmd_handle(__gui_msg_t *msg)
{
    switch (LOSWORD(msg->dwAddData1))
    {
        case MINI_STC_ID_NAME:
        {
            break;
        }

        case MINI_BTN_ID_PRE:
        {
            if (HISWORD(msg->dwAddData1) == BN_CLICKED)
            {
                __s32   index;
                char    filename[NAME_MAX];

                index   = dsk_wkm_npl_get_cur();

                index++;

                if (index >= dsk_wkm_npl_get_total_count())
                {
                    index = 0;
                }

                dsk_wkm_npl_index2file(index, filename);

                dsk_wkm_play_file(filename);

                //更新歌名
                __songname_update(msg);
            }

            break;
        }

        case MINI_BTN_ID_NEXT:
        {
            if (HISWORD(msg->dwAddData1) == BN_CLICKED)
            {
                __s32   index;
                char    filename[NAME_MAX];

                index   = dsk_wkm_npl_get_cur();

                index--;

                if (index < 0)
                {
                    index = dsk_wkm_npl_get_total_count() - 1;
                }

                dsk_wkm_npl_index2file(index, filename);

                dsk_wkm_play_file(filename);

                //更新歌名
                __songname_update(msg);
            }

            break;
        }

        case MINI_BTN_ID_PLAYPAUSE:
        {
            if (HISWORD(msg->dwAddData1) == BN_CLICKED)
            {
                if (dsk_wkm_get_fsm_state() == CEDAR_STAT_PAUSE)
                {
                    dsk_wkm_resume_play();

                    __button_bmp_update(msg->h_deswin, 0, MINI_BTN_ID_PLAYPAUSE);
                }
                else if (dsk_wkm_get_fsm_state() == CEDAR_STAT_PLAY)
                {
                    dsk_wkm_pause();

                    __button_bmp_update(msg->h_deswin, 0, MINI_BTN_ID_PLAYPAUSE);
                }

                //更新歌名
                __songname_update(msg);
            }

            break;
        }

        case MINI_BTN_ID_EXIT:
        {
            if (HISWORD(msg->dwAddData1) == BN_CLICKED)
            {
                scene_delete();
            }

            break;
        }

        case MINI_BTN_ID_QUIT:
            break;

        default:
            break;
    }

    return 0;
}

void __open_res(void)
{
    button_htheme[MINI_BTN_ID_PRE - MINI_STC_ID_NUM].h_focus          = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_PRE - MINI_STC_ID_NUM].h_unfocus        = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_NEXT - MINI_STC_ID_NUM].h_focus         = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_NEXT - MINI_STC_ID_NUM].h_unfocus       = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_PLAYPAUSE - MINI_STC_ID_NUM].h_focus    = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_PLAYPAUSE - MINI_STC_ID_NUM].h_unfocus  = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_EXIT - MINI_STC_ID_NUM].h_focus         = dsk_theme_open(0);
    button_htheme[MINI_BTN_ID_EXIT - MINI_STC_ID_NUM].h_unfocus       = dsk_theme_open(0);
}

void __release_res(void)
{
    __u32   i;

    for (i = MINI_BTN_ID_PRE; i <= MINI_BTN_ID_EXIT; i++)
    {
        dsk_theme_close(button_htheme[i - MINI_STC_ID_NUM].h_focus);
        dsk_theme_close(button_htheme[i - MINI_STC_ID_NUM].h_unfocus);
    }
}

void __button_para_init(void)
{
    __u32   i;

    for (i = MINI_BTN_ID_PRE; i <= MINI_BTN_ID_EXIT; i++)
    {
        button_para  [i - MINI_STC_ID_NUM].focus_bmp      = dsk_theme_hdl2buf(button_htheme[i - MINI_STC_ID_NUM].h_focus);
        button_para  [i - MINI_STC_ID_NUM].unfocus_bmp    = dsk_theme_hdl2buf(button_htheme[i - MINI_STC_ID_NUM].h_unfocus);
        button_para  [i - MINI_STC_ID_NUM].bmp_pos.x      = 0;
        button_para  [i - MINI_STC_ID_NUM].bmp_pos.y      = 0;
        button_para  [i - MINI_STC_ID_NUM].bk_color       = 0x0;
    }
}

void __static_para_init(void)
{
    __s32   index = -1;
    char    filename[NAME_MAX];
    dsk_wkm_audio_info_t    audio_info;

    index = dsk_wkm_npl_get_cur();

    dsk_wkm_npl_index2file(index, filename);

    __get_audio_info(filename, &audio_info);

    static_para[MINI_STC_ID_NAME].draw_font   = font;

    if (dsk_wkm_get_fsm_state() == CEDAR_STAT_PLAY)
    {
        static_para[MINI_STC_ID_NAME].ftxt_color    = GUI_WHITE;
        static_para[MINI_STC_ID_NAME].uftxt_color   = GUI_WHITE;
    }
    else
    {
        static_para[MINI_STC_ID_NAME].ftxt_color    = GUI_RED;
        static_para[MINI_STC_ID_NAME].uftxt_color   = GUI_RED;
    }

    static_para[MINI_STC_ID_NAME].text              = songname;
    static_para[MINI_STC_ID_NAME].draw_code         = audio_info.title_charset;
    static_para[MINI_STC_ID_NAME].txt_align         = GUI_TA_HCENTER;
    static_para[MINI_STC_ID_NAME].bk_color          = 0x66e196a1;//0x00a196e1   abgr
    static_para[MINI_STC_ID_NAME].alpha_en          = 1;

    eLIBs_strncpy(static_para[MINI_STC_ID_NAME].text, audio_info.title, NAME_MAX);
}

static __s32 __cb_frmwin(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __miniplay_uipara_t *ui_para;
            GUI_TTF_ATTR    attr;

            //memory allocate
            songname = (char *)esMEMS_Malloc(0, sizeof(char) * NAME_MAX);

            if (!songname)
            {
                __wrn("songname malloc error");
                return EPDK_FAIL;
            }

            eLIBs_memset(songname, 0, sizeof(char)*NAME_MAX);

            button_htheme = (button_htheme_t *)esMEMS_Malloc(0, sizeof(button_htheme_t) * (MINI_BTN_ID_NUM - MINI_STC_ID_NUM));

            if (!button_htheme)
            {
                __wrn("button_htheme malloc error");
                return EPDK_FAIL;
            }

            eLIBs_memset(button_htheme, 0, sizeof(button_htheme_t) * (MINI_BTN_ID_NUM - MINI_STC_ID_NUM));

            button_para = (button_para_t *)esMEMS_Malloc(0, sizeof(button_para_t) * (MINI_BTN_ID_NUM - MINI_STC_ID_NUM));

            if (!button_para)
            {
                __wrn("button_para malloc error");
                return EPDK_FAIL;
            }

            eLIBs_memset(button_para, 0, sizeof(button_para_t) * (MINI_BTN_ID_NUM - MINI_STC_ID_NUM));

            static_para = (static_para_t *)esMEMS_Malloc(0, sizeof(static_para_t) * (MINI_STC_ID_NUM - 0));

            if (!static_para)
            {
                __wrn("static_para malloc error");
                return EPDK_FAIL;
            }

            eLIBs_memset(static_para, 0, sizeof(static_para_t) * (MINI_STC_ID_NUM - 0));

            //create font
            //font  = GUI_SFT_CreateFont(SMALL_FONT_SIZE, SMALL_FONT_FILE);

            eLIBs_strcpy(attr.filepath, BEETLES_APP_ROOT"res\\fonts\\font.ttf");

            attr.height = 21;

            font = GUI_TTF_CreateFont(&attr);

            //打开图片资源句柄
            __open_res();

            //button para
            __button_para_init();

            //static para
            __static_para_init();

            ui_para     = miniplay_get_uipara();

            //background
            __draw_backgrd(msg->h_deswin);

            //song name
            __item_songname_create(msg->h_deswin, &static_para[MINI_STC_ID_NAME - 0], &(ui_para->para_songname));

            /*create pre*/
            __item_button_create(msg->h_deswin, &button_para[MINI_BTN_ID_PRE - MINI_STC_ID_NUM], MINI_BTN_ID_PRE, &(ui_para->para_pre));

            /*create next*/
            __item_button_create(msg->h_deswin, &button_para[MINI_BTN_ID_NEXT - MINI_STC_ID_NUM], MINI_BTN_ID_NEXT, &(ui_para->para_next));

            /*create playpause*/
            if (dsk_wkm_get_fsm_state() == CEDAR_STAT_PLAY)
            {
                __item_button_create(msg->h_deswin, &button_para[MINI_BTN_ID_PLAYPAUSE - MINI_STC_ID_NUM], MINI_BTN_ID_PLAYPAUSE, &(ui_para->para_play));
            }
            else
            {
                __item_button_create(msg->h_deswin, &button_para[MINI_BTN_ID_PLAYPAUSE - MINI_STC_ID_NUM], MINI_BTN_ID_PLAYPAUSE, &(ui_para->para_play));
            }

            /*create exit*/
            __item_button_create(msg->h_deswin, &button_para[MINI_BTN_ID_EXIT - MINI_STC_ID_NUM], MINI_BTN_ID_EXIT, &(ui_para->para_exit));
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __release_res();

            esMEMS_Mfree(0, songname);
            esMEMS_Mfree(0, button_htheme);
            esMEMS_Mfree(0, button_para);
            esMEMS_Mfree(0, static_para);

            //release font
            GUI_SetFont(GUI_GetDefaultFont());

            //GUI_SFT_ReleaseFont(font);
            GUI_TTF_Done(font);

            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
            return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            __cmd_handle(msg);
            return 0;
        }

        case MUSIC_CMD_AUTO_PLAY_NEXT:
        {
            __songname_update(msg);
            return EPDK_OK;
        }

        //case MUSIC_CMD_ERROR:
        //case MUSIC_CMD_STATE_CHANGE:
        //{
        //  return EPDK_OK;
        //}

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN __mini_frmwin_create(H_WIN parent, H_LYR layer)
{
    __gui_framewincreate_para_t framewin_para;
    __miniplay_uipara_t         *ui_para;
    H_WIN                       frmwin;

    ui_para = miniplay_get_uipara();

    if (!ui_para)
    {
        __wrn("get ui para fail");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name          = "mini frmwin",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.hHosting      = parent;
    framewin_para.FrameWinProc  = __cb_frmwin;
    framewin_para.rect.x        = ui_para->para_frmwin.x;
    framewin_para.rect.y        = ui_para->para_frmwin.y;
    framewin_para.rect.width    = ui_para->para_frmwin.width;
    framewin_para.rect.height   = ui_para->para_frmwin.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = NULL;
    framewin_para.hLayer        = layer;

    frmwin = GUI_FrmWinCreate(&framewin_para);

    return frmwin;
}


static H_WIN __mini_lyrwin_create(__s32 x, __s32 y)
{
    __layerwincreate_para_t lyrcreate_info;
    __miniplay_uipara_t     *ui_para;

    FB pBarFB =
    {
        {256, 128},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,         //mode
        0,                                   //ck_mode
        0,                                   //alpha_en
        255,                                 //alpha_val
        0,                                   //pipe
        9,                                //prio
        {0, 0, 256, 128},                    //scr_win
        {0, 0, 256, 128},                    //src_win
        DISP_LAYER_OUTPUT_CHN_DE_CH1,        //channel
        NULL                                 //fb
    };

    ui_para = miniplay_get_uipara();

    if (!ui_para)
    {
        __wrn("get ui para fail");
        return NULL;
    }

    eLIBs_memset(&lyrcreate_info, 0x0, sizeof(__layerwincreate_para_t));

    pBarFB.size.width       = ui_para->para_lyrwin.fb_size.width;
    pBarFB.size.height      = ui_para->para_lyrwin.fb_size.height;
    lstlyr.scn_win.x        = x;
    lstlyr.scn_win.y        = y;
    lstlyr.scn_win.width    = ui_para->para_lyrwin.scn_rect.width;
    lstlyr.scn_win.height   = ui_para->para_lyrwin.scn_rect.height;
    lstlyr.src_win.x        = ui_para->para_lyrwin.src_rect.x;
    lstlyr.src_win.y        = ui_para->para_lyrwin.src_rect.y;
    lstlyr.src_win.width    = ui_para->para_lyrwin.src_rect.width;
    lstlyr.src_win.height   = ui_para->para_lyrwin.src_rect.height;
    lstlyr.fb               = &pBarFB;
    lyrcreate_info.name     = "miniplay layer";
    lyrcreate_info.lyrpara  = &lstlyr;
    lyrcreate_info.initsta  = GUI_LYRWIN_STA_SLEEP;
    lyrcreate_info.type     = GUI_LYRWIN_SPRITE;

    return (GUI_LyrWinCreate(&lyrcreate_info));
}

__s32 __cb_auto_play_next(void *arg_p)
{
    NOTIFY_MSG(MUSIC_CMD_AUTO_PLAY_NEXT, 0, (H_WIN)arg_p, 0, 0);
    return 0;
}

static __s32 __mini_main_proc(__gui_msg_t  *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __mini_music_create_para_t  *create_para;
            __mini_music_scene_para_t   *scene_para;

            create_para = (__mini_music_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            scene_para  = (__mini_music_scene_para_t *)esMEMS_Malloc(0, sizeof(__mini_music_scene_para_t));

            if (!scene_para)
            {
                __wrn("scene para malloc error");
                return EPDK_FAIL;
            }

            eLIBs_memset(scene_para, 0, sizeof(__mini_music_scene_para_t));

            GUI_WinSetAddData(msg->h_deswin, (unsigned long)scene_para);

            scene_para->layer  = __mini_lyrwin_create(create_para->x, create_para->y);
            scene_para->frmwin = __mini_frmwin_create(msg->h_deswin, scene_para->layer);

            GUI_LyrWinSetSta(scene_para->layer, GUI_LYRWIN_STA_ON);

            GUI_LyrWinSetTop(scene_para->layer);

            //GUI_WinSetFocusChild(scene_para->frmwin);
            //注册自动播放回调

            dsk_wkm_set_cb(DSK_WKM_EVENT_AUTO_PLAY_NEXT, esKRNL_GetCallBack(__cb_auto_play_next), (void *)scene_para->frmwin);
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            __mini_music_scene_para_t *scene_para = (__mini_music_scene_para_t *)GUI_WinGetAddData(msg->h_deswin);

            //注销自动播放回调
            dsk_wkm_set_cb(DSK_WKM_EVENT_AUTO_PLAY_NEXT, NULL, NULL);

            GUI_LyrWinDelete(scene_para->layer);
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN scene_create(__mini_music_create_para_t *create_para)
{
    H_WIN                       hManWin;
    __gui_manwincreate_para_t   create_info;

    if (mini_manwin)
    {
        return mini_manwin;
    }

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.attr            = create_para;
    create_info.hParent         = create_para->parent;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__mini_main_proc);
    create_info.name            = "mini music";

    hManWin                     = GUI_ManWinCreate(&create_info);

    if (hManWin == NULL)
    {
        __wrn("mini music: create man win failed!");
        return NULL;
    }

    mini_manwin = hManWin;

    return hManWin;
}

__s32 scene_delete()
{
    __s32 ret;

    if (mini_manwin)
    {
        ret = GUI_ManWinDelete(mini_manwin);
        mini_manwin = NULL;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

__bool scene_exist(void)
{
    if (mini_manwin)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

