
#include "music_menu_ui.h"

/*static const res_info_t  res_btn_id[DIGITAL_SONG_BUTTON_MAX] =
{
    {STRING_KARAOKE_COMFIRM,   ID_KARAOKE_BUTTON0_UNFCS_BMP,ID_KARAOKE_BUTTON0_FCS_BMP},
    {STRING_KARAOKE_RETURN,    ID_KARAOKE_BUTTON0_UNFCS_BMP,ID_KARAOKE_BUTTON0_FCS_BMP},
    {STRING_KARAOKE_PREPAGE,   ID_KARAOKE_BUTTON0_UNFCS_BMP,ID_KARAOKE_BUTTON0_FCS_BMP},
    {STRING_KARAOKE_NEXTPAGE,  ID_KARAOKE_BUTTON0_UNFCS_BMP,ID_KARAOKE_BUTTON0_FCS_BMP}
};

static RECT             res_btn_pos[DIGITAL_SONG_BUTTON_MAX] =
{
    {50      ,294-52, 150, 57},
    {230     , 294-52, 150, 57},
    {410     , 294-52, 150, 57},
    {590     , 294-52, 150, 57},
};


static const res_bmp_info_t  res_num_id[DIGITAL_SONG_NUM_MAX] =
{
    {ID_KARAOKE_0_UNFCS_BMP,ID_KARAOKE_0_FCS_BMP},
    {ID_KARAOKE_1_UNFCS_BMP,ID_KARAOKE_1_FCS_BMP},
    {ID_KARAOKE_2_UNFCS_BMP,ID_KARAOKE_2_FCS_BMP},
    {ID_KARAOKE_3_UNFCS_BMP,ID_KARAOKE_3_FCS_BMP},
    {ID_KARAOKE_4_UNFCS_BMP,ID_KARAOKE_4_FCS_BMP},
    {ID_KARAOKE_5_UNFCS_BMP,ID_KARAOKE_5_FCS_BMP},
    {ID_KARAOKE_6_UNFCS_BMP,ID_KARAOKE_6_FCS_BMP},
    {ID_KARAOKE_7_UNFCS_BMP,ID_KARAOKE_7_FCS_BMP},
    {ID_KARAOKE_8_UNFCS_BMP,ID_KARAOKE_8_FCS_BMP},
    {ID_KARAOKE_9_UNFCS_BMP,ID_KARAOKE_9_FCS_BMP},

};
static RECT     res_num_pos[DIGITAL_SONG_NUM_MAX] =
{
    {35-13,  100-52, 65,  50},
    {109-13, 100-52, 65,  50},
    {183-13, 100-52, 65,  50},
    {256-13, 100-52, 65,  50},
    {330-13, 100-52, 65,  50},
    {404-13, 100-52, 65,  50},
    {478-13, 100-52, 65,  50},
    {552-13, 100-52, 65,  50},
    {626-13, 100-52, 65,  50},
    {699-13, 100-52, 65,  50},
};

static const res_bmp_info_t  res_other_id[DIGITAL_SONG_OTHER_MAX] =
{
    {ID_KARAOKE_INPUT_BOX_BMP,ID_KARAOKE_INPUT_BOX_BMP},
    {ID_KARAOKE_DEL_NUM_BMP,ID_KARAOKE_DEL_NUM_BMP},

};

static RECT     res_other_pos[DIGITAL_SONG_OTHER_MAX] =
{
    {564-13, 61-52,  134, 31},
    {701-13, 61-52,  61,  31},
};

static RECT     res_bg_pos[3] =
{
    {0,     0,      773, 309},   //frm bg
    {38-13, 153-52, 722, 126},   //listbar bg
    {270,   10,     400, 31},    //title
};
*/
/*
__s32 init_digital_song_res( pdigital_song_ui_t pui )
{
    __u32   i;

    for( i = 0; i < DIGITAL_SONG_OTHER_MAX; i++ )
    {
        pui->other_bmp[i].unfocus = dsk_theme_open(res_other_id[i].unfocus);
        pui->other_bmp[i].focus   =dsk_theme_open(res_other_id[i].focus);
        //__msg("pui->other_bmp[i].unfocus = %x\n", pui->other_bmp[i].unfocus);
        //__msg("pui->other_bmp[i].focus = %x\n", pui->other_bmp[i].focus);

    }
    //≥ı ºªØICON
    for( i = 0; i < DIGITAL_SONG_NUM_MAX; i++ )
    {
        pui->num_bmp[i].unfocus = dsk_theme_open(res_num_id[i].unfocus);
        pui->num_bmp[i].focus = dsk_theme_open(res_num_id[i].focus);
        //__msg("pui->num_bmp[i].unfocus = %x\n", pui->num_bmp[i].unfocus);
        //__msg("pui->num_bmp[i].focus = %x\n", pui->num_bmp[i].focus);

    }

    for( i = 0; i < DIGITAL_SONG_BUTTON_MAX; i++ )
    {
        pui->btn_bmp[i].unfocus = dsk_theme_open(res_btn_id[i].unfocus);
        pui->btn_bmp[i].focus = dsk_theme_open(res_btn_id[i].focus);

        //__msg("pui->btn_bmp[i].unfocus = %x\n", pui->btn_bmp[i].unfocus);
        //__msg("pui->btn_bmp[i].focus = %x\n", pui->btn_bmp[i].focus);
        dsk_langres_get_menu_text(res_btn_id[i].lang, pui->lang[i], GUI_NAME_MAX);
    }


    pui->h_frm_bg       = dsk_theme_open(ID_KARAOKE_KARAOKE_BG_BMP);
    pui->h_listbar_bg   = dsk_theme_open(ID_KARAOKE_SONG_LISTBG_BMP);
    pui->h_listbar_unfcs= dsk_theme_open(ID_KARAOKE_LIST_UNFCS_BMP);
    pui->h_listbar_fcs  = dsk_theme_open(ID_KARAOKE_LIST_FCS_BMP);

    //__msg("pui->h_listbar_unfcs = %x\n", pui->h_listbar_unfcs);
    //__msg("pui->h_listbar_fcs = %x\n", pui->h_listbar_fcs);

    return EPDK_OK;
}
__s32 uninit_digital_song_res( pdigital_song_ui_t pui )
{
    __u32   i;
    for( i=0; i< DIGITAL_SONG_OTHER_MAX; i++ )
    {
        if( pui->other_bmp[i].unfocus )
        {
            dsk_theme_close( pui->other_bmp[i].unfocus );
            pui->other_bmp[i].unfocus = NULL;
        }
        if( pui->other_bmp[i].focus )
        {
            dsk_theme_close( pui->other_bmp[i].focus );
            pui->other_bmp[i].focus = NULL;
        }
    }
    for( i=0; i< DIGITAL_SONG_NUM_MAX; i++ )
    {
        if( pui->num_bmp[i].unfocus )
        {
            dsk_theme_close( pui->num_bmp[i].unfocus );
            pui->num_bmp[i].unfocus = NULL;
        }
        if( pui->num_bmp[i].focus )
        {
            dsk_theme_close( pui->num_bmp[i].focus );
            pui->num_bmp[i].focus = NULL;
        }
    }

    for( i=0; i< DIGITAL_SONG_BUTTON_MAX; i++ )
    {
        if( pui->btn_bmp[i].unfocus )
        {
            dsk_theme_close( pui->btn_bmp[i].unfocus);
            pui->btn_bmp[i].unfocus = NULL;
        }
        if( pui->btn_bmp[i].focus )
        {
            dsk_theme_close( pui->btn_bmp[i].focus );
            pui->btn_bmp[i].focus = NULL;
        }
        pui->lang[i][0]= '\0';
    }
    if(pui->h_frm_bg)
    {
        dsk_theme_close( pui->h_frm_bg);
        pui->h_frm_bg = NULL;
    }
    if(pui->h_listbar_bg)
    {
        dsk_theme_close( pui->h_listbar_bg);
        pui->h_listbar_bg = NULL;
    }
    if(pui->h_listbar_unfcs)
    {
        dsk_theme_close( pui->h_listbar_unfcs);
        pui->h_listbar_unfcs = NULL;
    }
    if(pui->h_listbar_fcs)
    {
        dsk_theme_close( pui->h_listbar_fcs);
        pui->h_listbar_fcs = NULL;
    }
    return EPDK_OK;
}


void digital_song_draw_bg( pdigital_song_ui_t pui)
{
    __s16 i = 0;
    GUI_RECT rect;
    char str[64];
    //__msg("++++++digital_song_draw_bg++++++\n");

    if( pui->h_frm_bg != NULL)
    {
        void *pbmp = NULL;
        pbmp = dsk_theme_hdl2buf(pui->h_frm_bg);
        GUI_BMP_Draw( pbmp, res_bg_pos[0].x, res_bg_pos[0].y );
    }

    if( pui->h_listbar_bg != NULL )
    {
        void *pbmp =NULL;
        pbmp = dsk_theme_hdl2buf(pui->h_listbar_bg);
        GUI_BMP_Draw( pbmp, res_bg_pos[1].x, res_bg_pos[1].y);
    }
    eLIBs_memclr(str, sizeof(str));
     dsk_langres_get_menu_text(STRING_KARAOKE_DIGIT_SELECT, str,sizeof(str));
    //__msg("str = %s\n", str);
    GUI_DispStringAt(str,res_bg_pos[2].x, res_bg_pos[2].y);

}



void draw_digital_song_btn ( pdigital_song_ui_t pui , __s32 index, FOCUSE_STATE focus )
{
    void* pbmp =NULL;

   // __msg("    ++++++index = %d ++++++\n", index);
   if( focus == FOCUSE_NORMAL || focus == FOCUSE_OVER )
   {
        __msg("pui->btn_bmp[index].unfocus = %x\n", pui->btn_bmp[index].unfocus);
        if( pui->btn_bmp[index].unfocus != NULL )
        {
            pbmp = dsk_theme_hdl2buf(pui->btn_bmp[index].unfocus);
            GUI_BMP_Draw( pbmp, res_btn_pos[index].x, res_btn_pos[index].y );

        }
    }
   else
   {
        __msg("pui->btn_bmp[index].focus = %x\n", pui->btn_bmp[index].focus);
         if( pui->btn_bmp[index].focus != NULL )
        {
            pbmp = dsk_theme_hdl2buf(pui->btn_bmp[index].focus);
            GUI_BMP_Draw( pbmp, res_btn_pos[index].x, res_btn_pos[index].y );
        }
   }
   if( pui->lang[index][0] == '\0' )
        dsk_langres_get_menu_text(res_btn_id[index].lang, pui->lang[index], GUI_NAME_MAX);
    GUI_DispStringAt(pui->lang[index],res_btn_pos[index].x+50,res_btn_pos[index].y+20);

}


void draw_digital_song_num ( pdigital_song_ui_t pui, __s32 index, FOCUSE_STATE focus )
{
    void* pbmp =NULL;

   // __msg("    ++++++index = %d ++++++\n", index);
   if( focus == FOCUSE_NORMAL || focus == FOCUSE_OVER )
   {
        __msg("pui->num_bmp[index] = %x\n", pui->num_bmp[index]);
        if( pui->num_bmp[index].unfocus != NULL )
        {
            pbmp = dsk_theme_hdl2buf(pui->num_bmp[index].unfocus);
            GUI_BMP_Draw( pbmp, res_num_pos[index].x, res_num_pos[index].y );

        }
    }
   else
   {
        __msg("pui->num_bmp[index] = %x\n", pui->num_bmp[index]);
         if( pui->num_bmp[index].focus != NULL )
        {
            pbmp = dsk_theme_hdl2buf(pui->num_bmp[index].focus);
            GUI_BMP_Draw( pbmp, res_num_pos[index].x, res_num_pos[index].y );
        }
   }
}


void draw_digital_input_icon ( pdigital_song_ui_t pui )
{
     void* pbmp =NULL;
    if( pui->other_bmp[DIGITAL_SONG_INPUT].unfocus != NULL )
    {
        pbmp = dsk_theme_hdl2buf(pui->other_bmp[DIGITAL_SONG_INPUT].unfocus);
        GUI_BMP_Draw( pbmp, res_other_pos[DIGITAL_SONG_INPUT].x, res_other_pos[DIGITAL_SONG_INPUT].y );
    }
}



void draw_digital_input_del ( pdigital_song_ui_t pui, __s32 index, FOCUSE_STATE focus )
{
    void* pbmp = NULL;

   // __msg("    ++++++index = %d ++++++\n", index);
   if( focus == FOCUSE_NORMAL || focus == FOCUSE_OVER )
   {
        __msg("pui->other_bmp[index] = %x\n", pui->other_bmp[index]);
        if( pui->other_bmp[index].unfocus != NULL )
        {
            pbmp = dsk_theme_hdl2buf(pui->other_bmp[index].unfocus);
            GUI_BMP_Draw( pbmp, res_other_pos[index].x, res_other_pos[index].y );

        }
    }
   else
   {
        __msg("pui->other_bmp[index] = %x\n", pui->other_bmp[index]);
         if( pui->other_bmp[index].focus != NULL )
        {
            pbmp = dsk_theme_hdl2buf(pui->other_bmp[index].focus);
            GUI_BMP_Draw( pbmp, res_other_pos[index].x, res_other_pos[index].y);
        }
   }
}

void draw_digital_listbar_fcs_unfcs( pdigital_song_ui_t pui, FOCUSE_STATE focus )
{
    void* pbmp = NULL;

    __msg("    ++++++index = %d ++++++\n", index);
   if( focus == FOCUSE_NORMAL || focus == FOCUSE_OVER )
   {

        if(pui->h_listbar_unfcs)
        {
            pbmp = dsk_theme_hdl2buf(pui->h_listbar_unfcs);
            GUI_BMP_Draw( pbmp, res_other_pos[index].x, res_other_pos[index].y);
        }
    }
   else
   {
        __msg("pui->other_bmp[index] = %x\n", pui->other_bmp[index]);

   }
}
*/




