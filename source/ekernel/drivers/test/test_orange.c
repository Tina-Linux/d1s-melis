#include <mod_orange.h>
#include <rtthread.h>
#include <log.h>
#include <orange_api_entry.h>
#include <stdlib.h>
#include <stdio.h>
#include <libc/eLIBs_az100.h>


#define LANG_CHINESES_TYPE      0x410
#define LANG_CHINESET_TYPE      0x420
#define LANG_ENGLISH_TYPE       0x400

typedef struct _theme_t
{
    __u32  id;
    __u32  size;//ԭʼ�Ĵ�С
    __u32  size_com;//�洢�Ĵ�С
    void  *buf;
} theme_t, *HTHEME_i;


typedef struct tag_HLANGLIST
{
    __u16       LangID;         //LangID�� �磺Ox400
    __u32       StringTabOff;   //�ַ�������Ӧ��ʼ��ַ��
} HLANGLIST;

typedef struct tag_HLANHEAD
{
    FILE        *fp;
    HLANGLIST   *pHLangList;    //ָ��������Ϣ����Ϣ��
    __u32       LangTabOff;     //LANG����ʼ��ַƫ����
    __u16       LangSize;       //LANG����ÿ����Ŀ��С
    __u32       LangNum;        //LANG����Ŀ����,�������������
    __u16       StringSize;     //�ַ���size
    __u32       StringNum;      //�ַ���������
    __u32       align;          //���ݱ߽����ģʽ��
    __u16       *pIdTab;
} HHEAD;
typedef void *HRES;



static __s32 font_init(void);
static __s32 font_uninit(void);
static HTHEME dsk_theme_open(__u32 theme_id);
static void *dsk_theme_hdl2buf(HTHEME handle);
static HLANG  volatile  hlang   = NULL;
static __u16  volatile  lang_ID = 0x410;//chinese
static HRES    hres;
typedef struct
{
    __s32   fb_w;
    __s32   fb_h;

    __s32   src_x;
    __s32   src_y;
    __s32   src_w;
    __s32   src_h;

    __s32   scn_x;
    __s32   scn_y;
    __s32   scn_w;
    __s32   scn_h;

    __u8    pipe;
}ui_para_layer_t;


static void  _release_theme_res(HTHEME_i htheme)
{
    if (htheme != NULL)
    {
        if (htheme->buf != NULL)
        {
            esMEMS_Bfree(htheme->buf, htheme->size);
            htheme->buf = NULL;
        }

        esMEMS_Mfree(0, htheme);
    }
}



#if 0
static GUI_MEMDEV_Handle memdev_hdl = (__s32)NULL;

static __s32 __init_memdev_create(H_LYR hlyr, __s32 x, __s32 y, __s32 w,__s32 h)
{
    if (memdev_hdl)
    {
        __err("memdev already exist...");
        return EPDK_FAIL;
    }

    if (!hlyr)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(hlyr);
    memdev_hdl = GUI_MEMDEV_Create(x, y, w, h);

    if (!memdev_hdl)
    {
        return EPDK_FAIL;
    }

    GUI_MEMDEV_Select(memdev_hdl);
    return EPDK_OK;
}

__s32 init_memdev_create(H_LYR hlyr)
{
    __s32 ret;
    RECT rect;
    GUI_LyrWinGetScnWindow(hlyr, &rect);
    rect.x = 0;
    rect.y = 0;
    ret = __init_memdev_create(hlyr, rect.x, rect.y, rect.width, rect.height);
    return ret;
}

__s32 init_memdev_delete(void)
{
    if (!memdev_hdl)
    {
        __err("memdev_hdl is null...");
        return EPDK_FAIL;
    }

    GUI_MEMDEV_CopyToLCD(memdev_hdl);
    GUI_MEMDEV_Select((__s32)NULL);
    GUI_MEMDEV_Delete(memdev_hdl);
    memdev_hdl = (__s32)NULL;
}
#endif
void install_orange(int argc,char **argv)
{
    __u32 mid_orange;
    __mp *mp_orange = NULL;
    mid_orange = esMODS_MInstall("/data/mod/orange.mod", 0);
	mp_orange = esMODS_MOpen(mid_orange, 0);
    font_init();
}
FINSH_FUNCTION_EXPORT_ALIAS(install_orange, __cmd_install_orange, install_orange);




static H_LYR ui_layer_create(void)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0xff,                                              /* alpha_val */
        1,                                              /* pipe      */
        11,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };

    __layerwincreate_para_t lyrcreate_info =
    {
        "test layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    ui_para_layer_t layer_para =
    {
        400,480,
        0,0,400,480,
        0,0,400,480,
        1
    };
    fb.size.width           = layer_para.fb_w;
    fb.size.height          = layer_para.fb_h;
    lstlyr.src_win.x        = 0;//layer_para.src_x;
    lstlyr.src_win.y        = 0;//layer_para.src_y;
    lstlyr.src_win.width    = layer_para.src_w;
    lstlyr.src_win.height   = layer_para.src_h;
    lstlyr.scn_win.x        = layer_para.scn_x;
    lstlyr.scn_win.y        = layer_para.scn_y;
    lstlyr.scn_win.width    = layer_para.scn_w;
    lstlyr.scn_win.height   = layer_para.scn_h;
    lstlyr.pipe             = layer_para.pipe;
    lstlyr.fb               = &fb;

    lyrcreate_info.lyrpara  = &lstlyr;
	__log("hbar fb %d %d gui_dir=%d",fb.size.width,fb.size.height, GUI_GetScnDir());
	__log("hbar src %d %d %d %d",lstlyr.src_win.x,lstlyr.src_win.y,lstlyr.src_win.width,lstlyr.src_win.height);
	__log("hbar scn %d %d %d %d",lstlyr.scn_win.x,lstlyr.scn_win.y,lstlyr.scn_win.width,lstlyr.scn_win.height);

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("headbar layer create error !");
    }

    return layer;
}

static H_LYR ui_layer_create2(void)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0xff,                                              /* alpha_val */
        1,                                              /* pipe      */
        13,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };

    __layerwincreate_para_t lyrcreate_info =
    {
        "test layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    ui_para_layer_t layer_para =
    {
        800,480,
        0,0,800,480,
        0,0,800,480,
        1
    };
    fb.size.width           = layer_para.fb_w;
    fb.size.height          = layer_para.fb_h;
    lstlyr.src_win.x        = 0;//layer_para.src_x;
    lstlyr.src_win.y        = 0;//layer_para.src_y;
    lstlyr.src_win.width    = layer_para.src_w;
    lstlyr.src_win.height   = layer_para.src_h;
    lstlyr.scn_win.x        = layer_para.scn_x;
    lstlyr.scn_win.y        = layer_para.scn_y;
    lstlyr.scn_win.width    = layer_para.scn_w;
    lstlyr.scn_win.height   = layer_para.scn_h;
    lstlyr.pipe             = layer_para.pipe;
    lstlyr.fb               = &fb;

    lyrcreate_info.lyrpara  = &lstlyr;
	__log("hbar fb %d %d gui_dir=%d",fb.size.width,fb.size.height, GUI_GetScnDir());
	__log("hbar src %d %d %d %d",lstlyr.src_win.x,lstlyr.src_win.y,lstlyr.src_win.width,lstlyr.src_win.height);
	__log("hbar scn %d %d %d %d",lstlyr.scn_win.x,lstlyr.scn_win.y,lstlyr.scn_win.width,lstlyr.scn_win.height);

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("headbar layer create error !");
    }

    return layer;
}

//#define SFT_FONT

GUI_FONT    *SWFFont = NULL;

static __s32 font_init(void)
{
static __u8 flag = 0;
if(!flag)
{
    flag = 1;
#ifdef SFT_FONT
	SWFFont = GUI_SFT_CreateFont(16, "/mnt/E/font16.sft");
    if (SWFFont == NULL)
    {
        __err("create font fail...\n");
        return EPDK_FAIL;
    }

    GUI_SetFont(SWFFont);

#else
    GUI_TTF_ATTR  pTTF_ATTR;
    pTTF_ATTR.height = 30;
    strcpy(&pTTF_ATTR.filepath,"/mnt/E/PingFang_Medium.ttf");

	SWFFont = GUI_TTF_CreateFont(&pTTF_ATTR);


#endif
}

    return EPDK_OK;
}

static __s32 font_uninit(void)
{
#ifdef SFT_FONT
    if (SWFFont)
    {
        GUI_SetFont(GUI_GetDefaultFont());
        GUI_SFT_ReleaseFont(SWFFont);
        SWFFont = NULL;
    }
#else
    GUI_TTF_Done(SWFFont);
#endif

    return EPDK_OK;
}




/*
*******************************************************************************
*********************
*Name        : dsk_theme_hdl2buf
*Prototype   : void *dsk_theme_hdl2buf( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the pointer to the buffer of a theme. NULL means error.
*Description : get the pointer to the buffer of a theme.
*Other       :
*******************************************************************************
*********************
*/
static void *dsk_theme_hdl2buf(HTHEME handle)
{
    __s32     size;
    __s32     ret;
    void     *pbuf;
    HTHEME_i  htheme = (HTHEME_i) handle;

    if (handle == 0)
    {
        __log("handle is null, handle to buf failed!");
        return NULL;
    }

    if (htheme->buf != NULL)
    {
        return htheme->buf;
    }

    htheme->buf = (void *)esMEMS_Balloc(htheme->size);

    if (htheme->buf == NULL)
    {
        __log("Error in alloc size %x.", htheme->size);
        return NULL;
    }

    pbuf = esMEMS_Balloc(htheme->size_com);

    if (NULL == pbuf)
    {
        __log("*************mem not enough***********");
        esMEMS_Bfree(htheme->buf, htheme->size);
        htheme->buf = NULL;
        return NULL;
    }

    ret = GetRes(hres, 0, htheme->id, pbuf, htheme->size_com);

    if (-1 == ret)
    {
        __log("***********GetRes fail***********");
        esMEMS_Bfree(htheme->buf, htheme->size);
        esMEMS_Bfree(pbuf, htheme->size_com);
        htheme->buf = NULL;
        pbuf = NULL;
        return NULL;
    }

    if (EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        __s32 ret;
        __log("az100");
        ret = AZ100_Uncompress(pbuf, htheme->size_com, htheme->buf, htheme->size);

        if (EPDK_FAIL == ret)
        {
            __log("***********AZ100_Uncompress fail***********");
            esMEMS_Bfree(htheme->buf, htheme->size);
            esMEMS_Bfree(pbuf, htheme->size_com);
            htheme->buf = NULL;
            pbuf = NULL;
            return NULL;
        }
    }
    else
    {
        memcpy(htheme->buf, pbuf, htheme->size_com);
    }

    esMEMS_Bfree(pbuf, htheme->size_com);
    pbuf = NULL;
    return htheme->buf;
}


static HTHEME dsk_theme_open(__u32 theme_id)
{
    __s32     ret;
    HTHEME_i  htheme = NULL;
    void     *pbuf;

    if (hres == NULL)
    {
        return 0;
    }

    htheme = (HTHEME_i) esMEMS_Malloc(0, sizeof(theme_t));

    if (htheme == NULL)
    {
        __log("Error in allocating memory.");
        return 0;
    }

    memset(htheme, 0, sizeof(theme_t));
    htheme->id = theme_id;
    htheme->size_com = GetResSize(hres, 0, theme_id);
    __log("size = %d",htheme->size_com);
    pbuf = esMEMS_Balloc(htheme->size_com);

    if (NULL == pbuf)
    {
        __log("*************mem not enough, theme_id=%d***********", theme_id);
        goto error;
    }

    ret = GetRes(hres, 0, theme_id, pbuf, htheme->size_com);

    __log("hres=%x,theme_id=%d,pbuf=%x,htheme->size_com=%d",hres,theme_id, pbuf, htheme->size_com);
    if (-1 == ret)
    {
        __log("***********GetRes fail***********");
        goto error;
    }

    if (EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        __log("xx az100.....");
        htheme->size = AZ100_GetUncompressSize(pbuf, htheme->size_com);
    }
    else//δѹ��
    {
        htheme->size = htheme->size_com;
    }

    if (htheme->size == 0)
    {
        __log("Fail in getting the size of res %u.", htheme->id);
        goto error;
    }

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return (HTHEME) htheme;
error:
    _release_theme_res(htheme);

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return 0;
}


//ui layer create
//layer open and close
//GUI_SFT_CreateFont
//GUI_TTF_CreateFont
//UI zoom
//UI rotate
//UI bmp show
//line show
//color set
//circle show
//string show
//memdev
//alpha blending
//res loading lang and theme sft
//show UI res
void orangetest(int argc, char **argv)
{
    H_LYR  layer;
    __hdle fh;
    __u32 fb_addr = NULL;
    __mp *mp_display = NULL;
    __u32 pic_w,pic_h;
    __u32 mid_orange;
    __mp *mp_orange = NULL;



	mp_display = esKSRV_Get_Display_Hld();

    layer = ui_layer_create();

   GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_ON);

   GUI_LyrWinSel(layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();

  //  init_memdev_create(layer);//OK

   GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
   //GUI_LyrWinSetAlpha(layer,100);
   // GUI_OpenAlphaBlend();
   #if 0
   GUI_SetBkColor(GUI_WHITE);
   __log("eee");

   GUI_SetColor(GUI_YELLOW);
   __log("rrr");

   GUI_DrawLine(0,0,800,480);
   __log("tttt");

   GUI_SetColor(GUI_RED);
   GUI_DrawLine(0,50,800,50);

   GUI_DispStringAt("Allwinner",200,100);
   __log("yyy");

   GUI_DrawCircle(200,100,50);
   __log("uuu");

   GUI_SetColor(GUI_RED);
   GUI_FillCircle(300,100 ,30);
   __log("iivi");
   #endif
  // GUI_SetColor(GUI_YELLOW);
  // GUI_FillRect(600,0,50,50);

#if 0
    pic_h = 400;
    pic_w = 240;
	fb_addr = rt_malloc(pic_h*pic_w*4);
	rt_memset(fb_addr, 0,pic_h*pic_w*4);
	fh = fopen("/mnt/E/red400240.bmp","r");
	__log("file open ok fh = %p",fh);
    fread((void *)fb_addr, pic_h*pic_w*4, 1, fh);
    __log("BMP  x size = %d   y size = %d",GUI_BMP_GetXSize(fb_addr),GUI_BMP_GetYSize(fb_addr));
   GUI_BMP_Draw(fb_addr,0,0);
      __log("eeeeeeeee");
   //GUI_BMP_Draw_Rotate(fb_addr + 54, 0,0,pic_h,pic_w);
#endif


   __log("tsttttwwwvniiiwww");

   GUI_SetBkColor(0);
   GUI_SetColor(GUI_RED);
   GUI_DispStringAt("Allwinner. D1s test",50,300);

  //  GUI_CloseAlphaBlend();
  //  init_memdev_delete(); //ok
   __log("ssoossoi");
//GUI_LyrWinFlushFB(layer);


}
FINSH_FUNCTION_EXPORT_ALIAS(orangetest, __cmd_orangetest, orangetest);




void orangetest2(int argc, char **argv)
{
    H_LYR  layer2;
    __hdle fh;
    __u32 fb_addr = NULL;
    __mp *mp_display = NULL;
    __u32 pic_w,pic_h;
    HHEAD       *pHHead;
    char buff[20];
    void *themebuf;
    GUI_RECT rect = {300,20,380,100};
    pHHead = (HHEAD *)malloc(sizeof(HHEAD));
   // themebuf = malloc(200*200*4);
  //  memset(themebuf,0xff,200*200*4);
	mp_display = esKSRV_Get_Display_Hld();
  //  font_init();
      pHHead = Lang_Open("/mnt/E/lang.bin",0);
      hres = OpenRes("/mnt/E/theme.bin", 0);

    __log("pHHead :");
    __log("fp :%p",pHHead->fp);
    __log("pHLangList :%p",pHHead->pHLangList);
    __log("LangTabOff :%d",pHHead->LangTabOff);
    __log("LangSize :%d",pHHead->LangSize);
    __log("LangNum :%d",pHHead->LangNum);
    __log("StringSize :%d",pHHead->StringSize);
    __log("StringNum :%d",pHHead->StringNum);
    __log("align :%d",pHHead->align);
    __log("pIdTab :%p",pHHead->pIdTab);

    layer2 = ui_layer_create2();

   GUI_LyrWinSetSta(layer2, GUI_LYRWIN_STA_ON);

   GUI_LyrWinSel(layer2);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();


   GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

   GUI_SetBkColor(0);
  // GUI_SetColor(GUI_RED);
   //GUI_DispStringAt("Allwinner. D1s test 222",50,300);
   __log("ssoossoi");
    #if 1

   //theme  and  lang
   //lang
       //GUI_OpenAlphaBlend();

         // GUI_BMP_Draw_Rotate(themebuf,200,0,200,200);

        Lang_GetString(pHHead, LANG_ENGLISH_TYPE, 0x3ec, buff, 100);
        __log("string buff = %s",buff);
        Lang_GetString(pHHead, LANG_CHINESES_TYPE, 0x3ec, buff, 20);
        GUI_SetColor(GUI_RED);
        GUI_DispStringInRect(buff, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        rect.x0 = 300;
        rect.x1 = 380;
        rect.y0 = 100;
        rect.y1 = 150;
        Lang_GetString(pHHead, LANG_CHINESET_TYPE, 0x3ec, buff, 20);
        GUI_DispStringInRect(buff, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //theme
    __log("11111");
      //  GUI_BMP_Draw(dsk_theme_hdl2buf(dsk_theme_open(35)), 0,0);

   //     GUI_BMP_Draw_Rotate(themebuf,0,0,200,200);
            __log("2222");
    #endif

}
FINSH_FUNCTION_EXPORT_ALIAS(orangetest2, __cmd_orangetest2, orangetest2);


