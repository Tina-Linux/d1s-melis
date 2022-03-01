/*
************************************************************************************************************************
*                                                        CUCKOO
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : misc.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2008.11.08
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.11.08       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __COMMON_MISC_H__
#define  __COMMON_MISC_H__

#include "beetles_app.h"
#include "APP_Mem.h"
#include "touch_adapter.h"
#include "APP_Debug.h" //这个要放在最后


#define DE_INFO do{__msg("DE_INFO:L%d(%s):\n", __LINE__, __FILE__);pt_de_info();}while(0)
#define _Symbol2Str(str)                    #str
#define Symbol2Str(str)                     _Symbol2Str(str)
#define PT_MEMINF                           (eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),esMEMS_Info())

//虚拟键
#define VK_NULL                             0x0fffffff
#define VK_ESCAPE                           GUI_MSG_KEY_MENU
#define VK_MENU                             GUI_MSG_KEY_LONGMENU
#define VK_ENTER                            GUI_MSG_KEY_ENTER
#define VK_LEFT                             GUI_MSG_KEY_LEFT
#define VK_RIGHT                            GUI_MSG_KEY_RIGHT
#define VK_POP_VOLUME                       GUI_MSG_KEY_LONGVADD
#define VK_SUBMENU                          GUI_MSG_KEY_VADD
#define VK_SELECT                           GUI_MSG_KEY_VADD
#define VK_PREV                             GUI_MSG_KEY_PREV
#define VK_NEXT                             GUI_MSG_KEY_NEXT
#define VK_PLAY_PAUSE                       GUI_MSG_KEY_PLAY_PAUSE

#define VK_SELECT_REPEAT                    GUI_MSG_KEY_LONGVADD
#define VK_LEFT_REPEAT                      GUI_MSG_KEY_LONGLEFT
#define VK_RIGHT_REPEAT                     GUI_MSG_KEY_LONGRIGHT

///////macro const
#define MAX_FILE_NAME_LEN                   (256 * 3)

//mem
void *My_Palloc(uint32_t npage, uint32_t mode);
void  My_Pfree(void *mblk, uint32_t npage);
void *My_Balloc(uint32_t size);
void  My_Bfree(void *mblk, uint32_t size);
void *My_Balloc(uint32_t size);
void  My_Mfree(void *heap, void *f);
void *My_Malloc(void *heap, uint32_t size);

//macro function
#define app_sprintf                         eLIBs_sprintf
#define ZeroMemory(_p, _size)               eLIBs_memset((_p), 0, (_size))
#define ZeroTypeMem(_p, _type)              ZeroMemory(_p, sizeof(_type))

#define BallocType(_p, _type)               APP_NewMem_T_B(_p, _type)
#define BFreeType(_p, _type)                APP_DelMem_T_B(_p, _type)

#ifndef ValueInRange //touch_adapter.h
#define ValueInRange(_min, _v, _max)        ((_min) <= (_v) && (_v) < (_max))
#endif

#ifndef ValueBetweenRange //touch_adapter.h
#define ValueBetweenRange(_min, _v, _max)   ((_min) <= (_v) && (_v) <= (_max))
#endif

#define DATA_OF_ADRR(t, p, x, y, w)         (*( (t*)(p)+(y)*(w)+(x) ))

//消息
#ifndef NOTIFY_MSG
#define NOTIFY_MSG(mid, src, des, d1, d2)   \
    do{                                     \
        __gui_msg_t  exmsg;                 \
        exmsg.id            = mid;          \
        exmsg.h_srcwin      = src;          \
        exmsg.h_deswin      = des;          \
        exmsg.dwAddData1    = (uint32_t)d1; \
        exmsg.dwAddData2    = (uint32_t)d2; \
        GUI_SendNotifyMessage(&exmsg);      \
    }while(0)
#endif

#ifndef SEND_MSG
#define SEND_MSG(mid, src, des, d1, d2)     \
    do{                                     \
        __gui_msg_t  exmsg;                 \
        exmsg.id            = mid;          \
        exmsg.h_srcwin      = src;          \
        exmsg.h_deswin      = des;          \
        exmsg.dwAddData1    = (uint32_t)d1; \
        exmsg.dwAddData2    = (uint32_t)d2; \
        GUI_SendMessage(&exmsg);            \
    }while(0)
#endif

#ifndef POST_MSG
#define POST_MSG(mid, src, des, d1, d2)     \
    do{                                     \
        __gui_msg_t  exmsg;                 \
        exmsg.id            = mid;          \
        exmsg.h_srcwin      = src;          \
        exmsg.h_deswin      = des;          \
        exmsg.dwAddData1    = (uint32_t)d1; \
        exmsg.dwAddData2    = (uint32_t)d2; \
        GUI_PostMessage(&exmsg);            \
    }while(0)
#endif

// 内存设备特殊处理
#define GUI_MEMDEV_CopyToLCD_Ex(_h)        {GUI_SetDrawMode(LCD_DRAWMODE_NORMAL); GUI_MEMDEV_CopyToLCD(_h);}
//////////////////////////////////////////////////////////////////////////
//图层
#define _app_show_layer(_layer)             do {GUI_LyrWinSetSta(_layer, GUI_LYRWIN_STA_ON); GUI_LyrWinSetTop(_layer);} while (0);

#define _app_hide_layer(_layer)             GUI_LyrWinSetSta(_layer, GUI_LYRWIN_STA_SUSPEND)

#define app_show_layer                      _app_show_layer
#define app_hide_layer                      _app_hide_layer

#define _app_show_focus_wnd(_wnd, _layer)  {app_show_layer(_layer); GUI_WinSetFocusChild(_wnd);}
#define app_show_focus_wnd(_wnd)            _app_show_focus_wnd(_wnd, GUI_WinGetLyrWin(_wnd))
#define app_hide_wnd(_wnd)                  app_hide_layer(GUI_WinGetLyrWin(_wnd))

//注册表
#define _get_reg_pointer(_p, _t, _e)        (_p = (_t *)dsk_reg_get_para_by_app(_e))
#define get_reg_pointer(_p, _t, _e)         _get_reg_pointer(_p, _t, _e) //{_get_reg_pointer(_p, _t, _e); __msg("!!! reg(%d) = %x\n", _e, _p);}
#define update_to_reg()                     dsk_reg_flush()

//res string
#define _get_lang_res(_id, _pbuf)           dsk_langres_get_menu_text(_id, _pbuf, get_menu_len(_id) + 1)
#define get_lang_res(_id, _pbuf)            {_get_lang_res(_id, _pbuf);/* __msg("load str ( %s )\n", _pbuf);*/}

//gui bmp res 相关
#define GUI_BMPRES_Draw(_hTheme, _x, _y)    GUI_BMP_Draw(dsk_theme_hdl2buf(_hTheme), _x, _y)

//load bmp res
typedef struct tag_load_bmpres_type
{
    void    *data;
    HTHEME  handle;
} load_bmp_res_t;

#define ResHandle2Data(_res)                ((_res).data)
#define ResHandle2Handle(_res)              ((_res).handle)

#define _create_bmp_res(_id, _res)                      \
    do                                                  \
    {                                                   \
        APP_dsk_theme_open((_res).handle, _id);         \
        (_res).data = dsk_theme_hdl2buf((_res).handle); \
        DEBUG_BMP_DATA((_res).data);                    \
    } while(0)

#define _destroy_bmp_res(_res)              do {APP_dsk_theme_close((_res).handle);} while(0)

#define GUI_BMP_RES_Draw(_res, _x, _y)      GUI_BMP_Draw(ResHandle2Data(_res), _x, _y)
#define GUI_BMP_RES_GetYSize(_res)          GUI_BMP_GetYSize(ResHandle2Data(_res))
#define GUI_BMP_RES_GetXSize(_res)          GUI_BMP_GetXSize(ResHandle2Data(_res))

#define create_bmp_res(_id, _res)                   \
    do                                              \
    {                                               \
        __msg(" load res bmp %s (%d)", # _id, _id); \
        _create_bmp_res(_id, _res);                 \
    } while(0)

#define destroy_bmp_res(_res)               _destroy_bmp_res(_res)

#define BMP_RES_NULL                        -1
//////////////////////////////////////////////////////////////////////////

typedef struct _cuckoo_time_t
{
    uint32_t    hour;
    uint32_t    minute;
    uint32_t    second;
} cuckoo_time_t;


//add by libaiao, 2011.5.8;
//TIME_S,表示只显示秒; TIME_MS, 表示显示分秒; TIME_HMS,表示显示时，分,秒
//AUTO_HMS,根据时间自动转换，最少M,S
typedef enum time_format_s
{
    TIME_S          ,
    TIME_MS         ,
    TIME_HMS        ,
    TIME_AUTO_HMS   ,
} time_format_e;

extern void     uint2str(uint32_t input, char *str);   //注意该函数只能转换为两位输出.

extern void     time2time(uint32_t second_time, cuckoo_time_t *format_time);  ////declaraed by libaiao,2011.5.8
//extern void  uint2str( uint32_t input, char * str);  //declaraed by libaiao,2011.5.8

extern void     time2str(uint32_t ms, char *str);


extern void     filesize2str(uint32_t size, char *str);                                    //add by libaiao, 2011.5.8
extern void     time2str_by_format(uint32_t ms, char *str, time_format_e format);      //add by libaiao, 2011.5.8

extern int32_t  GetClippedString(GUI_RECT *region, char *src_str, char *dst_str, const char *append);//add by libaiao
extern uint32_t get_memory_info(char *title, void *pExtPara);          //add by libaiao,2011.6.26


#define app_print_memory_usage(title, pExtPara)     __here__;           \
    __msg("\n");                                                        \
    __msg("=================title: %s=====================\n", title);  \
    get_memory_info(title, pExtPara)


#define __memory_usage(title, pExtPara)     app_print_memory_usage(title, pExtPara)


//传入24时制小时值，传出12时制小时值，返回0为am，非0为pm
extern int32_t  hour24_to_hour12(int32_t *hour);

//查询app_name名称的应用是否存在
//查询背景音乐是否存在可以用is_app_exist(APP_MUSIC)
extern int32_t  is_app_exist(char *app_name);

int32_t     APP_BMP_DrawEx(H_LYR hlyr, const void *pFileData, int x, int y, int x0, int y0, int x1, int y1);

int32_t     test_flush_nor(int32_t i);

//设置标题
int32_t     app_set_title(int32_t res_id);

//设置默认背景颜色
int32_t     com_set_backcolor(uint32_t hlay, uint32_t color);

//libaiao, 2011.5.15
//根据指定长度从src中截取字符串
//返回值为窃取字符串的长度(byte为单位)
int32_t     GUI_GetStringByWidth(char *dst, const char GUI_UNI_PTR *src, uint32_t width);

void        pull_down_gpio(uint32_t port, uint32_t port_num);

void        pull_up_gpio(uint32_t port, uint32_t port_num);

int32_t     get_gpio_status(uint32_t port, uint32_t port_num);

int         pt_de_info(void);


#endif     //  ifndef __misc_h
