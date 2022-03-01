
#ifndef EBOOK_NEW_SSET_H
#define EBOOK_NEW_SSET_H

#include "app_ebook_i.h"

/*背景音乐开关切换*/
typedef enum     tag_bg_music_switch_e
{
    BG_MUSIC_OPEN = 0,
    BG_MUSIC_CLOSE,
} __ebook_bg_music_switch_e;

/*字体颜色设置*/
typedef enum      tag_font_color_e
{
    FONT_COLOR_BLACK = 0,
    FONT_COLOR_GREEN,
    FONT_COLOR_BLUE,
    //..FONT_COLOR_BLACK,
    FONT_COLOR_EXIT,  //退出
} __ebook_font_color_t;

/* 自动切换时间 */
typedef enum tag_book_auto_e
{
    AUTO_TIME_0 = 0,    // 停止自动播放
    AUTO_TIME_10,       // 10s自动切换
    AUTO_TIME_20,       // 20s自动切换
    AUTO_TIME_30,       // 30s自动切换
    AUTO_TIME_EXIT, //退出
} __ebook_autotime_e;

/*tts 开关*/
typedef enum  tag_book_tts_e
{
    TTS_ON = 0,
    TTS_OFF,
    TTS_EXIT,
} __ebook_tts_e;

/*电子书签*/
typedef enum      tag_open_bmk_e
{
    LABEL1 = 0,
    LABEL2,
    LABEL3,
    LABEL4,
    LABEL5,
    LABEL6

} __ebook_obmk_t;


typedef enum tag_EBOOK_SSET_MSG
{
    EBOOK_SET_EXIT  = 1,        //退出消息码
    EBOOK_SET_MUSIC,            //背景音乐开关消息码
    EBOOK_SET_FCOLOR,           //字体颜色设置
    EBOOK_SET_AUTO,         //自动播放
    EBOOK_SET_TTS,              //tts 开关选项
    EBOOK_SET_GOTO_PAGE,        //页面跳转
    EBOOK_SET_BRIGHT,
    EBOOK_SET_PREV,
    EBOOK_SET_NEXT,
    EBOOK_SET_ZOOMIN,
    EBOOK_SET_ZOOMOUT,
    EBOOK_SET_OBMK,
    EBOOK_SET_SBMK,
    EBOOK_SET_BMK_OPEN,
    EBOOK_SET_BMK_ADD,
    EBOOK_SET_BCOLOR,
    EBOOK_SET_ROTATE,
    EBOOK_SET_TIME,

} __ebook_sset_msg_t;

#define EBOOK_SSET_NC_ID_UNKNOW    0xFF


__sset_create_para_t *Ebook_SSET_Build_CreatePara(H_WIN h_win);
__s32 ebook_sset_init_focus_item_by_reg(__sset_create_para_t *sset_para);


#endif  /*EBOOK_NEW_SSET_H*/
