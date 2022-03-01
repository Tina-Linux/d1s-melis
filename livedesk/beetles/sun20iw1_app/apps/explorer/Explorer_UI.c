#include "beetles_app.h"
#include "Explorer.h"
#include "Explorer_UI.h"
#include "Explorer_List.h"
#include <misc/fb.h>
#include <log.h>

//libaiao


#define C_DATE_Y_SPACE              18
#define C_DEL_FILE_PECENT_AREA_X    0
#define C_DEL_FILE_PECENT_AREA_Y    (55+110+1)
#define C_DEL_FILE_PECENT_AREA_W    (130)
#define C_DEL_FILE_PECENT_AREA_H    (60)

// 左侧文件类型中，显示每种媒体对应的字符，如"music","ebook等"  坐标是相对这个媒体对应的图片而言
#define   C_FILE_TYPE_TXT_X 30
#define   C_FILE_TYPE_TXT_Y     2
#define   C_FILE_TYPE_TXT_W   80
#define   C_FILE_TYPE_TXT_H    22


static explorer_viewer_ui_t  explr_viewer_ui_480_640 =
{
    //layer window
    {
        PIXEL_COLOR_ARGB8888,       //fmt
        { 800, 480 - 46 },               //fb size
        { 0, 0, 800, 480 - 46 },         //src rect
        { 0, 46, 800, 480 -  46 },         //scn rect
        { 0 },                      //reserved
    },

    //menuview attribute
    { 0, 0, 800, 480 -  46 },             //menuview(frame window)
    { 0, 0, 800, 480 },             //browser bg
    { 0, 0, 800, 20  },              //browser bg top
    { 0, 0, 0,  0 },                 //browser bg bottom
    { 0, 0, 0,  0 },              //browser bg left

    { 300,  0,  10, 434},            //scroll bg rect
    { 300,  0,  10, 404},            //scroll rect
    0,  30,     0,                        //scroll head/body/tail height

    {0, 0, 150, 106 + 2}, //sd coor
    {150, 0, 150, 106 + 2}, //usb coor
    {447, 125 - 46, 246, 294}, //cp_mov_dialog

    { 0, 46 + 2, 800, 108 * 4},    //(the whole listbar rectangle)

    //mediatype item attribute
    {
        {0, 0, 300, 432}, //ListBar Area
        {0, 0, 300, 106 + 2}, //item rect
        {4, (106 + 2 - 42) / 2, 42, 42}, //icon rect
        {20 + 42, 5, 300 - (4 + 42 + 20), 106 + 2 - 10 }, //text rect
    },

    //list item attribute，以下坐标为点对于listbar rectangle的相对坐标
    {
        { 800 - 490, 0, 490, 54 * 8},           //iListBar Area
        //{ 0, 0, 490, 106+2 },                 //item rect
        { 0, 0, 490, 54 },                  //item rect
        { 0, 0, 490, 2 },                   //item_top_line;
        { 0, 0, 490, 2 },                   //item_bottom_line;
        { 0, 2, 2, 38},                     //item_dividing_line;

        //  { 4, (106 + 2 - 42) / 2, 42, 42 },                    //icon rect
        { 4, (54 - 42) / 2, 42, 42},                    //icon rect
        //{ 4, (106 + 2 - 42) / 2, 42, 42 },                    //miniature rect, 缩略图 UI 参数
        { 4, (54 - 42) / 2, 42, 42 },                   //miniature rect, 缩略图 UI 参数

        { 42 + 20, 0, 490 - (4 + 42 + 20), 54}, //text rect

        //downside two not use ,now
        { 7, 2, 254, 30 },                  //focus icon rect(select picture)
        { 2, 99, 65, 65},                   //媒体类型图标位置(显示在左边区域的大图标)

        //need to change
        {24, 5, 106 + 2, 20 },                  //title rect, only for file manager, not use any more
        //  {42 + 20, 106 + 2 - 32, 100, 32 },              //file size rect
        {60, 54 * 4, 300 - 60, 54},         //file size rect
        //  {42 + 20 + 100 + 80 / 2, 106 + 2 - 32, 150, 32},            //file create time rect
        {60, 54 * 4 + 54, 300 - 60, 54},        //file create time rect
        {0, 0, 150, 34 },                   //number rect
        {0, 164, 127, 81}                           //file_info_area_rect
    },

    //do not use, now
    //square item attribute，以下坐标为点对于mendu的相对坐标
    {
        { 0, 1, 800, 243},                  //(the whole listbar rectangle)
        { 0, 0, 160, 81 },                  //item rect in listbar
        { 8, 0, 112, 81 },                  //focus rect
        { 11, 3, 150, 60 },                 //icon rect
        { 11, 3, 150, 60 },                 //miniature rect
        { 13, 3 + 60 + 2, 160, 16},         //title rect
        //  { 0, 8, 390, 230},                  //(the whole listbar rectangle)
        //   { 0, 0, 130, 76 },                 //item rect in listbar
        //   { 8, 0, 114, 76 },                 //focus rect
        //   { 11, 3, 108, 55 },                //icon rect
        //   { 11, 3, 108, 55 },                //miniature rect
        //    { 15, 3+55 + 1 , 100, 16},        //title rect
    },

    //icon table
    {
        //ID                                                            res handle,     res buffer

        { NULL,           NULL,                  NULL },    //调色板

        { ID_EXPLORER_SD_F_BMP,           NULL,                  NULL }, //SD F
        { ID_EXPLORER_SD_UF_BMP,           NULL,                 NULL }, //SD UF
        { ID_EXPLORER_USB_F_BMP,           NULL,                 NULL }, // USB F
        { ID_EXPLORER_USB_UF_BMP,           NULL,                NULL }, //USB UF

        { ID_EXPLORER_CP_MOV_BMP,           NULL,                NULL },
        { ID_EXPLORER_CP_MOV_F_BMP,           NULL,                  NULL },

        { ID_EXPLORER_BOTTOM_BMP,           NULL,                NULL },    //背景图

        { ID_EXPLORER_USB_ICON_FOCUSED_BMP,             NULL,            NULL },    // USB

        { ID_EXPLORER_SD_ICON_FOCUSED_BMP,          NULL,            NULL },    // SD


        { NULL,                                          NULL,           NULL },    //
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    // 没法识别文件
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    //

        //文件管理左侧图标
        { ID_EXPLORER_VIDEO_FILE_BMP/*ID_EXPLORER_ALL_FILE_BMP*/,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    //


        { ID_EXPLORER_VIDEO_FILE_BMP,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    //

        { ID_EXPLORER_MUSIC_FILE_BMP,                   NULL,        NULL },    // music文件标志
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的

        { ID_EXPLORER_PHOTO_FILE_BMP,               NULL,       NULL },     // picture文件标志
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的

        { ID_EXPLORER_EBOOK_FILE_BMP,               NULL,                NULL },    // Ebook文件标志
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的

        { ID_EXPLORER_GAME_FILE_BMP,                NULL,                NULL },    // Game文件标志
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的


        {ID_EXPLORER_LISTBAR_ITEM_F_BMP,                NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_UF_BMP,           NULL,                NULL },    //listbar unfocus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_ROLL_BMP,         NULL,                NULL },    //listbar roll item 背景图

        {ID_EXPLORER_LEFT_LIST_F_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_F_BMP*/,               NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LEFT_LIST_UF_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_UF_BMP*/,         NULL,                NULL },    //listbar unfocus item 背景图


        {NULL,                                      NULL,                NULL },    //

        { ID_EXPLORER_LIST_SCROLL_BG_BMP,                    NULL,                   NULL },    // scroll bar 背景图
        { NULL,                  NULL,                   NULL },    // scroll bar 头部icon
        { ID_EXPLORER_LIST_SCROLL_BMP,                   NULL,                   NULL },    // scroll bar , 滑块，高度必须为1
        { NULL,                  NULL,                   NULL },    // scroll bar , 滑块，高度必须为1

        {ID_EXPLORER_LEFT_LIST_MOVIE_F_BMP,         NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MOVIE_UF_BMP,            NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MUSIC_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_MUSIC_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_F_BMP,          NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_UF_BMP,         NULL,               NULL},
        {ID_EXPLORER_FILE_TYPE_BG_BMP,              NULL,               NULL},

        {ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_GAME_ICON_FOCUSED_BMP,         NULL,               NULL},
        {ID_EXPLORER_FOLDER_ICON_BMP,               NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGER_BMP,              NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGE_ICON_BMP,          NULL,               NULL},

        {ID_EXPLORER_RETURN_F_BMP,                  NULL,               NULL},
        {ID_EXPLORER_RETURN_UF_BMP,                 NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_F_BMP,             NULL,               NULL},
        {ID_EXPLORER_INFO_BG_BMP,                   NULL,               NULL},
        {ID_EXPLORER_ERROR_BMP,                     NULL,               NULL},
        {NULL,                                      NULL,               NULL}

    },

    {
        { 0, 0, 126, 480 -  46},
        {
            {{2, 1, 126, 26}, {2, 137, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 1, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 28, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 55, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 218, 126, 26}, {2, 82, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 109, 126, 26}, {2, 109, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}}
        },
        {5, 5, 19, 19},         //文件管理图标
        {30, 5, 80, 20},        //显示文件字符坐标
        {87, 100, 30, 30},      //删除按钮坐标位置
        {0, 0, 30, 30},         //记录返去图标坐标的
        {(126 - 120) / 2, 120, 120, 90}         //图片对应的缩略图坐标

    }
}
#if 0
{
    //layer window
    {
        PIXEL_COLOR_ARGB8888,       //fmt
        { 640, 360 - 46},               //fb size
        { 0, 0, 640, 360 - 46},         //src rect
        { 0, 46 + 60, 640, 360 -  46},         //scn rect
        { 0 },                      //reserved
    },

    //menuview attribute
    { 0, 0, 640, 360 -  46},             //menuview(frame window)
    { 0, 0, 640, 360 },             //browser bg
    { 0, 0, 640, 20  },              //browser bg top
    { 0, 0, 0,  0 },                 //browser bg bottom
    { 0, 0, 0,  0 },              //browser bg left

    { 300,  0,  10, 434},            //scroll bg rect
    { 300,  0,  10, 404},            //scroll rect
    0,  30,     0,                        //scroll head/body/tail height

    {0, 0, 150, 106 + 2}, //sd coor
    {150, 0, 150, 106 + 2}, //usb coor
    {447, 125 - 46, 246, 294}, //cp_mov_dialog

    { 0, 46 + 2, 640/2, 108 * 4/2},    //(the whole listbar rectangle)

    //mediatype item attribute
    {
        {0, 0, 300, 432}, //ListBar Area
        {0, 0, 300, 106 + 2}, //item rect
        {4, (106 + 2 - 42) / 2, 42, 42}, //icon rect
        {20 + 42, 5, 300 - (4 + 42 + 20), 106 + 2 - 10 }, //text rect
    },

    //list item attribute，以下坐标为点对于listbar rectangle的相对坐标
    {
        { 640 - 330, 0, 330, 54 * 6},           //iListBar Area
        //{ 0, 0, 330, 106+2 },                 //item rect
        { 0, 0, 330, 54 },                  //item rect
        { 0, 0, 330, 2 },                   //item_top_line;
        { 0, 0, 330, 2 },                   //item_bottom_line;
        { 0, 2, 2, 38},                     //item_dividing_line;

        //  { 4, (106 + 2 - 42) / 2, 42, 42 },                    //icon rect
        { 4, (54 - 42) / 2, 42, 42},                    //icon rect
        //{ 4, (106 + 2 - 42) / 2, 42, 42 },                    //miniature rect, 缩略图 UI 参数
        { 4, (54 - 42) / 2, 42, 42 },                   //miniature rect, 缩略图 UI 参数

        { 42 + 20, 0, 490 - (4 + 42 + 20), 54}, //text rect

        //downside two not use ,now
        { 7, 2, 254, 30 },                  //focus icon rect(select picture)
        { 2, 99, 65, 65},                   //媒体类型图标位置(显示在左边区域的大图标)

        //need to change
        {24, 5, 106 + 2, 20 },                  //title rect, only for file manager, not use any more
        //  {42 + 20, 106 + 2 - 32, 100, 32 },              //file size rect
        {60, 54 * 4, 300 - 60, 54},         //file size rect
        //  {42 + 20 + 100 + 80 / 2, 106 + 2 - 32, 150, 32},            //file create time rect
        {60, 54 * 4 + 54, 300 - 60, 54},        //file create time rect
        {0, 0, 150, 34 },                   //number rect
        {0, 164, 127, 81}                           //file_info_area_rect
    },

    //do not use, now
    //square item attribute，以下坐标为点对于mendu的相对坐标
    {
        { 0, 1, 640, 243},                  //(the whole listbar rectangle)
        { 0, 0, 160, 81 },                  //item rect in listbar
        { 8, 0, 112, 81 },                  //focus rect
        { 11, 3, 150, 60 },                 //icon rect
        { 11, 3, 150, 60 },                 //miniature rect
        { 13, 3 + 60 + 2, 160, 16},         //title rect
        //  { 0, 8, 390, 230},                  //(the whole listbar rectangle)
        //   { 0, 0, 130, 76 },                 //item rect in listbar
        //   { 8, 0, 114, 76 },                 //focus rect
        //   { 11, 3, 108, 55 },                //icon rect
        //   { 11, 3, 108, 55 },                //miniature rect
        //    { 15, 3+55 + 1 , 100, 16},        //title rect
    },

    //icon table
    {
        //ID                                                            res handle,     res buffer

        { NULL,           NULL,                  NULL },    //调色板

        { ID_EXPLORER_SD_F_BMP,           NULL,                  NULL }, //SD F
        { ID_EXPLORER_SD_UF_BMP,           NULL,                 NULL }, //SD UF
        { ID_EXPLORER_USB_F_BMP,           NULL,                 NULL }, // USB F
        { ID_EXPLORER_USB_UF_BMP,           NULL,                NULL }, //USB UF

        { ID_EXPLORER_CP_MOV_BMP,           NULL,                NULL },
        { ID_EXPLORER_CP_MOV_F_BMP,           NULL,                  NULL },

        { ID_EXPLORER_BOTTOM_BMP,           NULL,                NULL },    //背景图

        { ID_EXPLORER_USB_ICON_FOCUSED_BMP,             NULL,            NULL },    // USB

        { ID_EXPLORER_SD_ICON_FOCUSED_BMP,          NULL,            NULL },    // SD


        { NULL,                                          NULL,           NULL },    //
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    // 没法识别文件
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    //

        //文件管理左侧图标
        { ID_EXPLORER_VIDEO_FILE_BMP/*ID_EXPLORER_ALL_FILE_BMP*/,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    //


        { ID_EXPLORER_VIDEO_FILE_BMP,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    //

        { ID_EXPLORER_MUSIC_FILE_BMP,                   NULL,        NULL },    // music文件标志
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的

        { ID_EXPLORER_PHOTO_FILE_BMP,               NULL,       NULL },     // picture文件标志
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的

        { ID_EXPLORER_EBOOK_FILE_BMP,               NULL,                NULL },    // Ebook文件标志
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的

        { ID_EXPLORER_GAME_FILE_BMP,                NULL,                NULL },    // Game文件标志
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的


        {ID_EXPLORER_LISTBAR_ITEM_F_BMP,                NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_UF_BMP,           NULL,                NULL },    //listbar unfocus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_ROLL_BMP,         NULL,                NULL },    //listbar roll item 背景图

        {ID_EXPLORER_LEFT_LIST_F_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_F_BMP*/,               NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LEFT_LIST_UF_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_UF_BMP*/,         NULL,                NULL },    //listbar unfocus item 背景图


        {NULL,                                      NULL,                NULL },    //

        { ID_EXPLORER_LIST_SCROLL_BG_BMP,                    NULL,                   NULL },    // scroll bar 背景图
        { NULL,                  NULL,                   NULL },    // scroll bar 头部icon
        { ID_EXPLORER_LIST_SCROLL_BMP,                   NULL,                   NULL },    // scroll bar , 滑块，高度必须为1
        { NULL,                  NULL,                   NULL },    // scroll bar , 滑块，高度必须为1

        {ID_EXPLORER_LEFT_LIST_MOVIE_F_BMP,         NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MOVIE_UF_BMP,            NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MUSIC_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_MUSIC_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_F_BMP,          NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_UF_BMP,         NULL,               NULL},
        {ID_EXPLORER_FILE_TYPE_BG_BMP,              NULL,               NULL},

        {ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_GAME_ICON_FOCUSED_BMP,         NULL,               NULL},
        {ID_EXPLORER_FOLDER_ICON_BMP,               NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGER_BMP,              NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGE_ICON_BMP,          NULL,               NULL},

        {ID_EXPLORER_RETURN_F_BMP,                  NULL,               NULL},
        {ID_EXPLORER_RETURN_UF_BMP,                 NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_F_BMP,             NULL,               NULL},
        {ID_EXPLORER_INFO_BG_BMP,                   NULL,               NULL},
        {ID_EXPLORER_ERROR_BMP,                     NULL,               NULL},
        {NULL,                                      NULL,               NULL}

    },

    {
        { 0, 0, 126, 480 -  46},
        {
            {{2, 1, 126, 26}, {2, 137, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 1, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 28, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 55, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 218, 126, 26}, {2, 82, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 109, 126, 26}, {2, 109, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}}
        },
        {5, 5, 19, 19},         //文件管理图标
        {30, 5, 80, 20},        //显示文件字符坐标
        {87, 100, 30, 30},      //删除按钮坐标位置
        {0, 0, 30, 30},         //记录返去图标坐标的
        {(126 - 120) / 2, 120, 120, 90}         //图片对应的缩略图坐标

    }
}
#endif
;


static explorer_viewer_ui_t  explr_viewer_ui_800_480 =
{
    //layer window
    {
        PIXEL_COLOR_ARGB8888,       //fmt
        { 800, 480 - 46 },               //fb size
        { 0, 0, 800, 480 - 46 },         //src rect
        { 0, 46, 800, 480 -  46 },         //scn rect
        { 0 },                      //reserved
    },

    //menuview attribute
    { 0, 0, 800, 480 -  46 },             //menuview(frame window)
    { 0, 0, 800, 480 },             //browser bg
    { 0, 0, 800, 20  },              //browser bg top
    { 0, 0, 0,  0 },                 //browser bg bottom
    { 0, 0, 0,  0 },              //browser bg left

    { 300,  0,  10, 434},            //scroll bg rect
    { 300,  0,  10, 404},            //scroll rect
    0,  30,     0,                        //scroll head/body/tail height

    {0, 0, 150, 106 + 2}, //sd coor
    {150, 0, 150, 106 + 2}, //usb coor
    {447, 125 - 46, 246, 294}, //cp_mov_dialog

    { 0, 46 + 2, 800, 108 * 4},    //(the whole listbar rectangle)

    //mediatype item attribute
    {
        {0, 0, 300, 432}, //ListBar Area
        {0, 0, 300, 106 + 2}, //item rect
        {4, (106 + 2 - 42) / 2, 42, 42}, //icon rect
        {20 + 42, 5, 300 - (4 + 42 + 20), 106 + 2 - 10 }, //text rect
    },

    //list item attribute，以下坐标为点对于listbar rectangle的相对坐标
    {
        { 800 - 490, 0, 490, 54 * 8},           //iListBar Area
        //{ 0, 0, 490, 106+2 },                 //item rect
        { 0, 0, 490, 54 },                  //item rect
        { 0, 0, 490, 2 },                   //item_top_line;
        { 0, 0, 490, 2 },                   //item_bottom_line;
        { 0, 2, 2, 38},                     //item_dividing_line;

        //  { 4, (106 + 2 - 42) / 2, 42, 42 },                    //icon rect
        { 4, (54 - 42) / 2, 42, 42},                    //icon rect
        //{ 4, (106 + 2 - 42) / 2, 42, 42 },                    //miniature rect, 缩略图 UI 参数
        { 4, (54 - 42) / 2, 42, 42 },                   //miniature rect, 缩略图 UI 参数

        { 42 + 20, 0, 490 - (4 + 42 + 20), 54}, //text rect

        //downside two not use ,now
        { 7, 2, 254, 30 },                  //focus icon rect(select picture)
        { 2, 99, 65, 65},                   //媒体类型图标位置(显示在左边区域的大图标)

        //need to change
        {24, 5, 106 + 2, 20 },                  //title rect, only for file manager, not use any more
        //  {42 + 20, 106 + 2 - 32, 100, 32 },              //file size rect
        {60, 54 * 4, 300 - 60, 54},         //file size rect
        //  {42 + 20 + 100 + 80 / 2, 106 + 2 - 32, 150, 32},            //file create time rect
        {60, 54 * 4 + 54, 300 - 60, 54},        //file create time rect
        {0, 0, 150, 34 },                   //number rect
        {0, 164, 127, 81}                           //file_info_area_rect
    },

    //do not use, now
    //square item attribute，以下坐标为点对于mendu的相对坐标
    {
        { 0, 1, 800, 243},                  //(the whole listbar rectangle)
        { 0, 0, 160, 81 },                  //item rect in listbar
        { 8, 0, 112, 81 },                  //focus rect
        { 11, 3, 150, 60 },                 //icon rect
        { 11, 3, 150, 60 },                 //miniature rect
        { 13, 3 + 60 + 2, 160, 16},         //title rect
        //  { 0, 8, 390, 230},                  //(the whole listbar rectangle)
        //   { 0, 0, 130, 76 },                 //item rect in listbar
        //   { 8, 0, 114, 76 },                 //focus rect
        //   { 11, 3, 108, 55 },                //icon rect
        //   { 11, 3, 108, 55 },                //miniature rect
        //    { 15, 3+55 + 1 , 100, 16},        //title rect
    },

    //icon table
    {
        //ID                                                            res handle,     res buffer

        { NULL,           NULL,                  NULL },    //调色板

        { ID_EXPLORER_SD_F_BMP,           NULL,                  NULL }, //SD F
        { ID_EXPLORER_SD_UF_BMP,           NULL,                 NULL }, //SD UF
        { ID_EXPLORER_USB_F_BMP,           NULL,                 NULL }, // USB F
        { ID_EXPLORER_USB_UF_BMP,           NULL,                NULL }, //USB UF

        { ID_EXPLORER_CP_MOV_BMP,           NULL,                NULL },
        { ID_EXPLORER_CP_MOV_F_BMP,           NULL,                  NULL },

        { ID_EXPLORER_BOTTOM_BMP,           NULL,                NULL },    //背景图

        { ID_EXPLORER_USB_ICON_FOCUSED_BMP,             NULL,            NULL },    // USB

        { ID_EXPLORER_SD_ICON_FOCUSED_BMP,          NULL,            NULL },    // SD


        { NULL,                                          NULL,           NULL },    //
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    // 没法识别文件
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    //

        //文件管理左侧图标
        { ID_EXPLORER_VIDEO_FILE_BMP/*ID_EXPLORER_ALL_FILE_BMP*/,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    //


        { ID_EXPLORER_VIDEO_FILE_BMP,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    //

        { ID_EXPLORER_MUSIC_FILE_BMP,                   NULL,        NULL },    // music文件标志
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的

        { ID_EXPLORER_PHOTO_FILE_BMP,               NULL,       NULL },     // picture文件标志
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的

        { ID_EXPLORER_EBOOK_FILE_BMP,               NULL,                NULL },    // Ebook文件标志
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的

        { ID_EXPLORER_GAME_FILE_BMP,                NULL,                NULL },    // Game文件标志
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的


        {ID_EXPLORER_LISTBAR_ITEM_F_BMP,                NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_UF_BMP,           NULL,                NULL },    //listbar unfocus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_ROLL_BMP,         NULL,                NULL },    //listbar roll item 背景图

        {ID_EXPLORER_LEFT_LIST_F_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_F_BMP*/,               NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LEFT_LIST_UF_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_UF_BMP*/,         NULL,                NULL },    //listbar unfocus item 背景图


        {NULL,                                      NULL,                NULL },    //

        { ID_EXPLORER_LIST_SCROLL_BG_BMP,                    NULL,                   NULL },    // scroll bar 背景图
        { NULL,                  NULL,                   NULL },    // scroll bar 头部icon
        { ID_EXPLORER_LIST_SCROLL_BMP,                   NULL,                   NULL },    // scroll bar , 滑块，高度必须为1
        { NULL,                  NULL,                   NULL },    // scroll bar , 滑块，高度必须为1

        {ID_EXPLORER_LEFT_LIST_MOVIE_F_BMP,         NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MOVIE_UF_BMP,            NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MUSIC_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_MUSIC_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_F_BMP,          NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_UF_BMP,         NULL,               NULL},
        {ID_EXPLORER_FILE_TYPE_BG_BMP,              NULL,               NULL},

        {ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_GAME_ICON_FOCUSED_BMP,         NULL,               NULL},
        {ID_EXPLORER_FOLDER_ICON_BMP,               NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGER_BMP,              NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGE_ICON_BMP,          NULL,               NULL},

        {ID_EXPLORER_RETURN_F_BMP,                  NULL,               NULL},
        {ID_EXPLORER_RETURN_UF_BMP,                 NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_F_BMP,             NULL,               NULL},
        {ID_EXPLORER_INFO_BG_BMP,                   NULL,               NULL},
        {ID_EXPLORER_ERROR_BMP,                     NULL,               NULL},
        {NULL,                                      NULL,               NULL}

    },

    {
        { 0, 0, 126, 480 -  46},
        {
            {{2, 1, 126, 26}, {2, 137, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 1, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 28, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 55, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 218, 126, 26}, {2, 82, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 109, 126, 26}, {2, 109, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}}
        },
        {5, 5, 19, 19},         //文件管理图标
        {30, 5, 80, 20},        //显示文件字符坐标
        {87, 100, 30, 30},      //删除按钮坐标位置
        {0, 0, 30, 30},         //记录返去图标坐标的
        {(126 - 120) / 2, 120, 120, 90}         //图片对应的缩略图坐标

    }
};

//720*480
static explorer_viewer_ui_t  explr_viewer_ui_720_480 =
{
    //layer window
    {
        PIXEL_COLOR_ARGB8888,       //fmt
        { 720, 480 - 46 },               //fb size
        { 0, 0, 720, 480 - 46 },         //src rect
        { 0, 46, 720, 480 -  46 },         //scn rect
        { 0 },                      //reserved
    },

    //menuview attribute
    { 0, 0, 720, 480 -  46 },             //menuview(frame window)
    { 0, 0, 720, 480 },             //browser bg
    { 0, 0, 720, 20  },              //browser bg top
    { 0, 0, 0,  0 },                 //browser bg bottom
    { 0, 0, 0,  0 },              //browser bg left

    { 300,  0,  10, 434},            //scroll bg rect
    { 300,  0,  10, 404},            //scroll rect
    0,  30,     0,                        //scroll head/body/tail height



    {0, 0, 150, 106 + 2}, //sd coor
    {150, 0, 150, 106 + 2}, //usb coor
    {447 - 40, 125 - 46, 246, 294}, //cp_mov_dialog

    { 0, 46 + 2, 720, 108 * 4},    //(the whole listbar rectangle)

    //mediatype item attribute
    {
        {0, 106 + 2, 300, 108 * 3}, //ListBar Area
        {0, 0, 300, 106 + 2}, //item rect
        {4 + 40, (106 + 2 - 42) / 2, 42, 42}, //icon rect
        {20 + 42 + 40, 5, 300 - (4 + 42 + 20 + 40), 106 + 2 - 10 }, //text rect
    },

    //list item attribute，以下坐标为点对于listbar rectangle的相对坐标
    {
        { 720 - 410, 0, 410, 108 * 4},              //iListBar Area
        { 0, 0, 410, 106 + 2 },                 //item rect
        { 0, 0, 410, 2 },                   //item_top_line;
        { 0, 0, 410, 2 },                   //item_bottom_line;
        { 0, 2, 2, 38},                     //item_dividing_line;

        { 4, (106 + 2 - 42) / 2, 42, 42 },                  //icon rect
        { 4, (106 + 2 - 42) / 2, 42, 42 },                  //miniature rect, 缩略图 UI 参数

        { 42 + 20, 5, 410 - (4 + 42 + 20) - 40, 106 + 2 - 10 }, //text rect

        //downside two not use ,now
        { 7, 2, 254, 30 },                  //focus icon rect(select picture)
        { 2, 99, 65, 65},                   //媒体类型图标位置(显示在左边区域的大图标)

        //need to change
        {24, 5, 106 + 2, 20 },                  //title rect, only for file manager, not use any more
        {42 + 20, 106 + 2 - 32, 100, 32 },              //file size rect
        {42 + 20 + 100 + 80 / 2, 106 + 2 - 32, 150, 32},            //file create time rect
        {0, 0, 150, 34 },                   //number rect
        {0, 164, 127, 81}                           //file_info_area_rect
    },

    //do not use, now
    //square item attribute，以下坐标为点对于mendu的相对坐标
    {
        { 0, 1, 720, 243},                  //(the whole listbar rectangle)
        { 0, 0, 160, 81 },                  //item rect in listbar
        { 8, 0, 112, 81 },                  //focus rect
        { 11, 3, 150, 60 },                 //icon rect
        { 11, 3, 150, 60 },                 //miniature rect
        { 13, 3 + 60 + 2, 160, 16},         //title rect
        //  { 0, 8, 390, 230},                  //(the whole listbar rectangle)
        //   { 0, 0, 130, 76 },                 //item rect in listbar
        //   { 8, 0, 114, 76 },                 //focus rect
        //   { 11, 3, 108, 55 },                //icon rect
        //   { 11, 3, 108, 55 },                //miniature rect
        //    { 15, 3+55 + 1 , 100, 16},        //title rect
    },

    //icon table
    {
        //ID                                                            res handle,     res buffer

        { NULL,           NULL,                  NULL },    //调色板

        { ID_EXPLORER_SD_F_BMP,           NULL,                  NULL }, //SD F
        { ID_EXPLORER_SD_UF_BMP,           NULL,                 NULL }, //SD UF
        { ID_EXPLORER_USB_F_BMP,           NULL,                 NULL }, // USB F
        { ID_EXPLORER_USB_UF_BMP,           NULL,                NULL }, //USB UF

        { ID_EXPLORER_CP_MOV_BMP,           NULL,                NULL },
        { ID_EXPLORER_CP_MOV_F_BMP,           NULL,                  NULL },

        { ID_EXPLORER_BOTTOM_BMP,           NULL,                NULL },    //背景图

        { ID_EXPLORER_USB_ICON_FOCUSED_BMP,             NULL,            NULL },    // USB

        { ID_EXPLORER_SD_ICON_FOCUSED_BMP,          NULL,            NULL },    // SD


        { NULL,                                          NULL,           NULL },    //
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    // 没法识别文件
        { ID_EXPLORER_UNKNOWN_FILE_FOCUSED_BMP,  NULL,           NULL },    //

        //文件管理左侧图标
        { ID_EXPLORER_VIDEO_FILE_BMP/*ID_EXPLORER_ALL_FILE_BMP*/,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP/*ID_EXPLORER_ALL_ICON_FOCUSED_BMP*/,       NULL,        NULL },    //


        { ID_EXPLORER_VIDEO_FILE_BMP,                   NULL,        NULL },    //  video文件标志
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    // video文件icon图标,与文件名一起的
        { ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,       NULL,        NULL },    //

        { ID_EXPLORER_MUSIC_FILE_BMP,                   NULL,        NULL },    // music文件标志
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的
        { ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,       NULL,        NULL },    // music文件icon图标,与文件名一起的

        { ID_EXPLORER_PHOTO_FILE_BMP,               NULL,       NULL },     // picture文件标志
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的
        { ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,       NULL,       NULL },     // picture文件icon图标,与文件名一起的

        { ID_EXPLORER_EBOOK_FILE_BMP,               NULL,                NULL },    // Ebook文件标志
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的
        { ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,       NULL,                NULL },    // Ebook文件icon图标,与文件名一起的

        { ID_EXPLORER_GAME_FILE_BMP,                NULL,                NULL },    // Game文件标志
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的
        { ID_EXPLORER_GAME_ICON_FOCUSED_BMP,        NULL,                NULL },    // game文件icon图标,与文件名一起的


        {ID_EXPLORER_LISTBAR_ITEM_F_BMP,                NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LISTBAR_ITEM_UF_BMP,           NULL,                NULL },    //listbar unfocus item 背景图

        {ID_EXPLORER_LEFT_LIST_F_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_F_BMP*/,               NULL,                NULL },    //listbar focus item 背景图
        {ID_EXPLORER_LEFT_LIST_UF_BMP/*ID_EXPLORER_MEDIATYPE_LISTBAR_ITEM_UF_BMP*/,         NULL,                NULL },    //listbar unfocus item 背景图


        {NULL,                                      NULL,                NULL },    //

        { ID_EXPLORER_LIST_SCROLL_BG_BMP,                    NULL,                   NULL },    // scroll bar 背景图
        { NULL,                  NULL,                   NULL },    // scroll bar 头部icon
        { ID_EXPLORER_LIST_SCROLL_BMP,                   NULL,                   NULL },    // scroll bar , 滑块，高度必须为1
        { NULL,                  NULL,                   NULL },    // scroll bar , 滑块，高度必须为1

        {ID_EXPLORER_LEFT_LIST_MOVIE_F_BMP,         NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MOVIE_UF_BMP,            NULL,               NULL},      //窗口中左侧显示的文件类型图标
        {ID_EXPLORER_LEFT_LIST_MUSIC_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_MUSIC_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_PHOTO_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_F_BMP,         NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_EBOOK_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_F_BMP,          NULL,               NULL},
        {ID_EXPLORER_LEFT_LIST_GAME_UF_BMP,         NULL,               NULL},
        {ID_EXPLORER_FILE_TYPE_BG_BMP,              NULL,               NULL},

        {ID_EXPLORER_VIDEO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_MUSIC_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_PHOTO_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_EBOOK_ICON_FOCUSED_BMP,        NULL,               NULL},
        {ID_EXPLORER_GAME_ICON_FOCUSED_BMP,         NULL,               NULL},
        {ID_EXPLORER_FOLDER_ICON_BMP,               NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGER_BMP,              NULL,               NULL},
        {ID_EXPLORER_FILE_MANAGE_ICON_BMP,          NULL,               NULL},

        {ID_EXPLORER_RETURN_F_BMP,                  NULL,               NULL},
        {ID_EXPLORER_RETURN_UF_BMP,                 NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_UF_BMP,            NULL,               NULL},
        {ID_EXPLORER_DELETE_ICON_F_BMP,             NULL,               NULL},
        {ID_EXPLORER_INFO_BG_BMP,                   NULL,               NULL},
        {ID_EXPLORER_ERROR_BMP,                     NULL,               NULL},
        {NULL,                                      NULL,               NULL}

    },

    {
        { 0, 0, 126, 480 -  46},
        {
            {{2, 1, 126, 26}, {2, 137, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 1, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 164, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 28, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 191, 126, 26}, {2, 218, 126, 26}, {2, 55, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 218, 126, 26}, {2, 82, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}},
            {{2, 1, 126, 26}, {2, 28, 126, 26}, {2, 55, 126, 26}, {2, 82, 126, 26}, {2, 109, 126, 26}, {2, 109, 126, 135}, {22, 48, 65, 65}, {87, 31, 34, 30}}
        },
        {5, 5, 19, 19},         //文件管理图标
        {30, 5, 80, 20},        //显示文件字符坐标
        {87, 100, 30, 30},      //删除按钮坐标位置
        {0, 0, 30, 30},         //记录返去图标坐标的
        {(126 - 120) / 2, 120, 120, 90}         //图片对应的缩略图坐标

    }
};




//获得整个explorer AP设计的UI参数
explorer_viewer_ui_t *explorer_get_viewer_ui_param(void)
{
    RECT screen_size;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        return &(explr_viewer_ui_800_480);
    }
    else if((screen_size.width == 480) && (screen_size.height == 640))
    {
        //__log("cccccccccasaf...set screen_size.width == 480) && (screen_size.height == 640");
        return &(explr_viewer_ui_480_640);
    }
    else
    {
        return &(explr_viewer_ui_720_480);
    }
}

RECT *explorer_get_listview_ui_param(void)
{
    RECT screen_size;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        return &(explr_viewer_ui_800_480.listview);
    }
    else //if(TVOUT_NTSC == tv_out_status)
    {
        return &(explr_viewer_ui_720_480.listview);
    }
}

//获得list 中 item 设计的UI参数
explorer_list_item_ui_param_t *explorer_get_list_item_ui_param(void)
{
    RECT screen_size;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        return &(explr_viewer_ui_800_480.list_item_ui_param);
    }
    else
    {
        return &(explr_viewer_ui_720_480.list_item_ui_param);
    }
}

//获得list 中 square 设计的UI参数
explorer_square_item_ui_param_t *explorer_get_square_item_ui_param(void)
{
    RECT screen_size;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        return &(explr_viewer_ui_800_480.square_item_ui_param);
    }
    else //if(TVOUT_NTSC == tv_out_status)
    {
        return &(explr_viewer_ui_720_480.square_item_ui_param);
    }
}


__u32 explorer_get_listbar_pagenum(explr_list_para_t *list_para)
{
    __u32 pagenum = 0, col, row;
    __u32 item_width, item_height, lbar_width, lbar_height;
    explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();            //获得整个AP的UI设计参数

    if (list_para->view_mode == EXPLR_LIST_MODE)
    {
        item_width = ui_param->list_item_ui_param.item_rect.width;                      //根据list or square模式不同
        item_height = ui_param->list_item_ui_param.item_rect.height;
        lbar_width = ui_param->list_item_ui_param.ListBarArea.width;
        lbar_height = ui_param->list_item_ui_param.ListBarArea.height;
    }
    else if (list_para->view_mode == EXPLR_SQUARE_MODE)
    {
        item_width = ui_param->square_item_ui_param.item_rect.width;
        item_height = ui_param->square_item_ui_param.item_rect.height;
        lbar_width = ui_param->square_item_ui_param.ListBarArea.width;
        lbar_height = ui_param->square_item_ui_param.ListBarArea.height;
    }
    else
    {
        item_width = ui_param->list_item_ui_param.item_rect.width;
        item_height = ui_param->list_item_ui_param.item_rect.height;
        lbar_width = ui_param->list_item_ui_param.ListBarArea.width;
        lbar_height = ui_param->list_item_ui_param.ListBarArea.height;
    }

    col         = lbar_width  / item_width;
    row         = lbar_height / item_height;
    pagenum     = col * row;
    return pagenum;
}
/*
************************************************************************************************************************
*Function   :         void* explorer_get_listview_icon_res(__u32 index)
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
void *explorer_get_listview_icon_res(__u32 index)
{
    explorer_icon_data_t *icon_table = NULL;
    RECT screen_size;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        icon_table = explr_viewer_ui_800_480.icon_table;
    }
    else
    {
        icon_table = explr_viewer_ui_720_480.icon_table;
    }

    //__msg("----icon_id: %d, res_id: %d\n", index, icon_table[index].res_id);
    if (icon_table == NULL)
    {
        return NULL;
    }
    else if (icon_table[index].res_id == 0)
    {
        return NULL;
    }
    else if (icon_table[index].res_handle == NULL)
    {
        icon_table[index].res_handle = theme_open(icon_table[index].res_id);
        icon_table[index].res_buffer = theme_hdl2buf(icon_table[index].res_handle);
        return icon_table[index].res_buffer;
    }
    else
    {
        //--------------------------------------------------------------------------
        //加这个判断仅仅是为了避开listbar, 684行，GUI_BitStringDrawEx()冲掉该图片buffer
        //后续应彻底根除
        //if(index == ID_EXP_LONG_STR_SCROLL_BG)
        /*
        {
            theme_close(icon_table[index].res_handle);
            icon_table[index].res_handle = theme_open(icon_table[index].res_id);
            icon_table[index].res_buffer = theme_hdl2buf(icon_table[index].res_handle);
            return icon_table[index].res_buffer;
        }
        */
        //--------------------------------------------------------------------------
        return icon_table[index].res_buffer;
    }
}

/*
************************************************************************************************************************
*Function   :         __s32 explorer_free_listview_icon_res(void)
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 explorer_free_listview_icon_res(void)
{
    __s32 i = 0;
    explorer_icon_data_t *icon_table = NULL;
    RECT screen_size;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        icon_table = explr_viewer_ui_800_480.icon_table;
    }
    else
    {
        icon_table = explr_viewer_ui_720_480.icon_table;
    }

    if (icon_table == NULL)
    {
        return EPDK_FAIL;
    }
    else
    {
        for (i = 0; i < BROWSER_ICON_MAX; i++)
        {
            if (icon_table[i].res_handle != NULL)
            {
                theme_close(icon_table[i].res_handle);
                icon_table[i].res_handle = NULL;
                icon_table[i].res_buffer = NULL;
            }
        }

        return EPDK_OK;
    }
}

//获得Explorer 窗口layer size
__s32 explorer_get_screen_rect(RECT *screen_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();
    screen_rect->x = ui_param->layer.scn_rect.x;
    screen_rect->y = ui_param->layer.scn_rect.y;
    screen_rect->width = ui_param->layer.scn_rect.width;
    screen_rect->height = ui_param->layer.scn_rect.height;
    return EPDK_OK;
}


#if (EXPLORER_SUPPORT_MINIATURE == 1)
//获得缩略图区域(according to the view mode of list)
__s32 explorer_get_item_miniature_rect(explr_list_para_t *para, RECT *miniature_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();

    switch (para->view_mode)
    {
        case EXPLR_LIST_MODE:
            miniature_rect->x = ui_param->list_item_ui_param.miniature_rect.x;
            miniature_rect->y = ui_param->list_item_ui_param.miniature_rect.y;
            miniature_rect->width = ui_param->list_item_ui_param.miniature_rect.width;
            miniature_rect->height = ui_param->list_item_ui_param.miniature_rect.height;
            break;

        case EXPLR_SQUARE_MODE:
            miniature_rect->x = ui_param->square_item_ui_param.miniature_rect.x;
            miniature_rect->y = ui_param->square_item_ui_param.miniature_rect.y;
            miniature_rect->width = ui_param->square_item_ui_param.miniature_rect.width;
            miniature_rect->height = ui_param->square_item_ui_param.miniature_rect.height;
            break;
    }

    return EPDK_OK;
}
#endif

//获得显示文本区域
__s32 explorer_get_mediatype_item_text_rect(explr_list_para_t *para, RECT *text_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();
    text_rect->x = ui_param->mediatype_list_item_ui_param.text_rect.x;
    text_rect->y = ui_param->mediatype_list_item_ui_param.text_rect.y;
    text_rect->width = ui_param->mediatype_list_item_ui_param.text_rect.width;
    text_rect->height = ui_param->mediatype_list_item_ui_param.text_rect.height;
    return EPDK_OK;
}



//获得显示文本区域
__s32 explorer_get_item_text_rect(explr_list_para_t *para, RECT *text_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();

    switch (para->view_mode)
    {
        case EXPLR_LIST_MODE:
            text_rect->x = ui_param->list_item_ui_param.text_rect.x;
            text_rect->y = ui_param->list_item_ui_param.text_rect.y;
            text_rect->width = ui_param->list_item_ui_param.text_rect.width;
            text_rect->height = ui_param->list_item_ui_param.text_rect.height;
            break;

        case EXPLR_SQUARE_MODE:
            text_rect->x = ui_param->square_item_ui_param.title_rect.x;
            text_rect->y = ui_param->square_item_ui_param.title_rect.y;
            text_rect->width = ui_param->square_item_ui_param.title_rect.width;
            text_rect->height = ui_param->square_item_ui_param.title_rect.height;
            break;

        default:
            break;
    }

    return EPDK_OK;
}

//获得mediatype 条目开始的图标显示区域
__s32 explorer_get_mediatype_item_icon_rect(explr_list_para_t *para, RECT *icon_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();
    {
        icon_rect->x = ui_param->mediatype_list_item_ui_param.icon_rect.x;
        icon_rect->y = ui_param->mediatype_list_item_ui_param.icon_rect.y;
        icon_rect->width = ui_param->mediatype_list_item_ui_param.icon_rect.width;
        icon_rect->height = ui_param->mediatype_list_item_ui_param.icon_rect.height;
    }
    return EPDK_OK;
}



//获得条目开始的图标显示区域
__s32 explorer_get_item_icon_rect(explr_list_para_t *para, RECT *icon_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();

    switch (para->view_mode)
    {
        case EXPLR_LIST_MODE:
            icon_rect->x = ui_param->list_item_ui_param.icon_rect.x;
            icon_rect->y = ui_param->list_item_ui_param.icon_rect.y;
            icon_rect->width = ui_param->list_item_ui_param.icon_rect.width;
            icon_rect->height = ui_param->list_item_ui_param.icon_rect.height;
            break;

        case EXPLR_SQUARE_MODE:
            icon_rect->x = ui_param->square_item_ui_param.icon_rect.x;
            icon_rect->y = ui_param->square_item_ui_param.icon_rect.y;
            icon_rect->width = ui_param->square_item_ui_param.icon_rect.width;
            icon_rect->height = ui_param->square_item_ui_param.icon_rect.height;
            break;

        default:
            break;
    }

    return EPDK_OK;
}


//获得焦点条目图标
__s32 explorer_get_item_focus_icon_rect(explr_list_para_t *para, RECT *focus_icon_rect)
{
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();

    switch (para->view_mode)
    {
        case EXPLR_LIST_MODE:
            focus_icon_rect->x = ui_param->list_item_ui_param.focus_icon_rect.x;
            focus_icon_rect->y = ui_param->list_item_ui_param.focus_icon_rect.y;
            focus_icon_rect->width = ui_param->list_item_ui_param.focus_icon_rect.width;
            focus_icon_rect->height = ui_param->list_item_ui_param.focus_icon_rect.height;
            break;

        case EXPLR_SQUARE_MODE:
            focus_icon_rect->x = ui_param->square_item_ui_param.focus_rect.x;
            focus_icon_rect->y = ui_param->square_item_ui_param.focus_rect.y;
            focus_icon_rect->width = ui_param->square_item_ui_param.focus_rect.width;
            focus_icon_rect->height = ui_param->square_item_ui_param.focus_rect.height;
            break;

        default:
            break;
    }

    return EPDK_OK;
}

__s32 explorer_file_type_res_init(explr_list_para_t *list_para)
{
    list_para->h_file_type_res.file_type_bg_bmp = theme_open(ID_EXPLORER_FILE_TYPE_BG_BMP);
    list_para->h_return_bmp = theme_open(ID_EXPLORER_RETURN_UF_BMP);
    list_para->h_file_type_res.file_type_big_ebook_bmp = theme_open(ID_EXPLORER_EBOOK_FILE_BMP);
    list_para->h_file_type_res.file_type_big_game_bmp = theme_open(ID_EXPLORER_GAME_FILE_BMP);
    list_para->h_file_type_res.file_type_big_movie_bmp = theme_open(ID_EXPLORER_VIDEO_FILE_BMP);
    list_para->h_file_type_res.file_type_big_music_bmp = theme_open(ID_EXPLORER_MUSIC_FILE_BMP);
    list_para->h_file_type_res.file_type_big_photo_bmp = theme_open(ID_EXPLORER_PHOTO_FILE_BMP);
    list_para->h_file_type_res.file_type_ebook_f_bmp = theme_open(ID_EXPLORER_LEFT_LIST_EBOOK_F_BMP);
    list_para->h_file_type_res.file_type_ebook_uf_bmp = theme_open(ID_EXPLORER_LEFT_LIST_EBOOK_UF_BMP);
    list_para->h_file_type_res.file_type_game_f_bmp = theme_open(ID_EXPLORER_LEFT_LIST_GAME_F_BMP);
    list_para->h_file_type_res.file_type_game_uf_bmp = theme_open(ID_EXPLORER_LEFT_LIST_GAME_UF_BMP);
    list_para->h_file_type_res.file_type_movie_f_bmp = theme_open(ID_EXPLORER_LEFT_LIST_MOVIE_F_BMP);
    list_para->h_file_type_res.file_type_movie_uf_bmp = theme_open(ID_EXPLORER_LEFT_LIST_MOVIE_UF_BMP);
    list_para->h_file_type_res.file_type_music_f_bmp = theme_open(ID_EXPLORER_LEFT_LIST_MUSIC_F_BMP);
    list_para->h_file_type_res.file_type_music_uf_bmp = theme_open(ID_EXPLORER_LEFT_LIST_MUSIC_UF_BMP);
    list_para->h_file_type_res.file_type_photo_f_bmp = theme_open(ID_EXPLORER_LEFT_LIST_PHOTO_F_BMP);
    list_para->h_file_type_res.file_type_photo_uf_bmp = theme_open(ID_EXPLORER_LEFT_LIST_PHOTO_UF_BMP);
    return EPDK_OK;
}

#if (EXPLORER_SUPPORT_MINIATURE == 1)
// 左侧文件类型中，显示当前图片的大的缩略图
__s32 explorer_draw_pic_miniature_bmp(explr_list_para_t *list_para, __u32 item_index)
{
    void *pic_buf = NULL;
    __s32 picX = 0, picY = 0;
    explorer_viewer_ui_t *ui_param;
    GUI_RECT gui_rect ;
    __s32 ret = EPDK_FAIL ;
    char  str_photo[128] = { 0 } ;
    GUI_MEMDEV_Handle   draw_mem;
    OS_SEM_DATA         pdata;

    if (RAT_MEDIA_TYPE_PIC != list_para->media_type)
    {
        return EPDK_FAIL ;
    }

    if (list_para->enter_sd_usb_flag == 0)  //在sd/usb选择窗口时，直接返回
    {
        return EPDK_FAIL ;
    }

    __here__ ;
    GUI_SetFont(SWFFont);
    GUI_SetColor(GUI_BLUE);
    GUI_UC_SetEncodeUTF8();
    ui_param = explorer_get_viewer_ui_param();
    gui_rect.x0 = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.x ;
    gui_rect.y0 = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.y ;
    gui_rect.x1 = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.width ;
    gui_rect.y1 = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.height ;
    GUI_LyrWinSel(list_para->list_layer);
    draw_mem = GUI_MEMDEV_Create(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
    GUI_MEMDEV_Select(draw_mem);
    //画背景
    picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.x ;
    picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.y ;
    pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
    GUI_BMP_Draw(pic_buf, picX, picY);
    __here__ ;
    {
        char FilePath[RAT_MAX_FULL_PATH_LEN] = { 0 };
        int ret = 0;
        rat_miniature_para_t in_para;
        rat_pic_info_t out_para;
        RECT miniature_rect;
        ret = GetListItemFileFullPath(list_para, item_index, FilePath);

        if (ret == EPDK_FAIL)
        {
            __msg("get list item full path fail\n");
            return EPDK_FAIL;
        }

        miniature_rect.x = ui_param->file_type_uiparam.pic_miniature_bmp_rect.x ;
        miniature_rect.y = ui_param->file_type_uiparam.pic_miniature_bmp_rect.y ;
        miniature_rect.width = ui_param->file_type_uiparam.pic_miniature_bmp_rect.width ;
        miniature_rect.height = ui_param->file_type_uiparam.pic_miniature_bmp_rect.height - 22 ;
        eLIBs_strcpy(in_para.file, FilePath);
        in_para.format = PIXEL_COLOR_ARGB8888;
        in_para.width = miniature_rect.width;
        in_para.height = miniature_rect.height;
        in_para.mode = 0;   // 1为拉伸模式
        __here__ ;

        if (list_para->big_miniature_buf == NULL)
        {
            __msg("list_para->big_miniature_buf is null \n");
            return EPDK_FAIL;
        }

        eLIBs_memset(list_para->big_miniature_buf, 0, list_para->big_miniature_size);
        eLIBs_memset(&out_para, 0, sizeof(out_para));
        out_para.miniature.buf = list_para->big_miniature_buf ;
        //ret = rat_get_pic_info(&in_para, &out_para);
        ret = rat_get_pic_info_ex(&in_para, &out_para, list_para);

        if (EPDK_OK == ret)
        {
            FB fb;
            eLIBs_memset(&fb, 0, sizeof(FB));
            fb.size.width = in_para.width;
            fb.size.height = in_para.height;
            fb.addr[0] = out_para.miniature.buf;;
            fb.fmt.type = FB_TYPE_RGB;
            fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
            fb.fmt.fmt.rgb.br_swap = 0;
            fb.fmt.fmt.rgb.pixseq = (__rgb_seq_t)0;
            __msg("start to draw current pic miniature \n ");
            GUI_BitString_Draw(&fb,     miniature_rect.x, miniature_rect.y);
        }
        else
        {
            char str_tips[128] ;
            dsk_langres_get_menu_text(STRING_EXPLR_GET_PIC_INFO_FAIL, str_tips, sizeof(str_tips)) ;
            GUI_DispStringInRectWrap(str_tips, &gui_rect, GUI_TA_BOTTOM | GUI_TA_HCENTER, GUI_WRAPMODE_WORD);
        }
    }
    __here__ ;
    //画返回图标
    picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].return_bmp_rect.x +
           ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.x ;
    picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].return_bmp_rect.y +
           ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.y;
    pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
    GUI_BMP_Draw(pic_buf, picX, picY);
    //记录返去图标坐标
    ui_param->file_type_uiparam.return_bmp_rect.x = picX ;
    ui_param->file_type_uiparam.return_bmp_rect.y = picY ;
    //画photo
    picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_photo_rect.x ;
    picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_photo_rect.y ;
    pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_FOCUS_ICON_PHOTO);
    GUI_BMP_Draw(pic_buf, picX, picY);
    gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
    gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
    gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
    gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
    dsk_langres_get_menu_text(STRING_EXPLR_PHOTO_MEDIA, str_photo, sizeof(str_photo)) ;
    __msg("\n");
    __msg("str_photo=%s\n", str_photo);
    __msg("\n");
    GUI_DispStringInRect(str_photo, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    __here__ ;
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    __here__;
    draw_mem = NULL;
    return EPDK_OK;
}
#endif
#if 0
__s32 explorer_draw_file_type(explr_list_para_t *list_para, rat_media_type_t media_type)
{
    void *pic_buf = NULL;
    __s32 picX = 0, picY = 0;
    explorer_viewer_ui_t *ui_param;
    GUI_RECT gui_rect ;
    __s32 ret = EPDK_FAIL ;
    GUI_MEMDEV_Handle   draw_mem = NULL ;
    __s32 game_card = EPDK_FALSE;
    char str_ebook[128] = { 0 }, str_movie[128] = { 0 }, str_music[128] = { 0 }, str_photo[128] = { 0 }, str_game[128] = { 0 } ;
    ui_param = explorer_get_viewer_ui_param();

    if ((rat_is_encryption_partition_Insert(NULL, 1))
        && (CFG_get_game_and_dict_flag(ID_HOME_GAME)))
    {
        game_card = EPDK_TRUE;
    }

    GUI_LyrWinSel(list_para->list_layer) ;
    dsk_langres_get_menu_text(STRING_EXPLR_EBOOK_MEDIA, str_ebook, sizeof(str_ebook)) ;
    dsk_langres_get_menu_text(STRING_EXPLR_PHOTO_MEDIA, str_photo, sizeof(str_photo)) ;
    dsk_langres_get_menu_text(STRING_EXPLR_MOVIE_MEDIA, str_movie, sizeof(str_movie)) ;
    dsk_langres_get_menu_text(STRING_EXPLR_MUSIC_MEDIA, str_music, sizeof(str_music)) ;

    if (EPDK_TRUE == game_card)
    {
        dsk_langres_get_menu_text(STRING_EXPLR_GAME_MEDIA,  str_game, sizeof(str_game)) ;
    }

    draw_mem = GUI_MEMDEV_Create(ui_param->file_type_uiparam.file_type_rect.x, ui_param->file_type_uiparam.file_type_rect.y, ui_param->file_type_uiparam.file_type_rect.width, ui_param->file_type_uiparam.file_type_rect.height);

    if (!draw_mem)
    {
        __msg("memory device create fail \n");
    }

    GUI_MEMDEV_Select(draw_mem);
    ui_param = explorer_get_viewer_ui_param();
    //__wrn("not need draw background...\n");
    //pic_buf = explorer_get_listview_icon_res( ID_EXP_BACKGROUND);
    gui_rect.x0 = ui_param->file_type_uiparam.file_type_rect.x;
    gui_rect.y0 = ui_param->file_type_uiparam.file_type_rect.y;
    gui_rect.x1 = gui_rect.x0 + ui_param->file_type_uiparam.file_type_rect.width - 1 ;
    gui_rect.y1 = gui_rect.y0 + ui_param->file_type_uiparam.file_type_rect.height - 1 ;
    __msg("gui_rect.x0 = %d %d %d %d\n", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1) ;

    //  ret = APP_BMP_DrawEx( list_para->list_layer, pic_buf, gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1 );   //用背景色去擦除该区域
    //if( EPDK_FAIL == ret )
    //{
    //  __msg( "APP_BMP_DrawEx fail \n" );
    //}
    switch (media_type)
    {
        case RAT_MEDIA_TYPE_VIDEO:
        {
            //画music
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_music_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_music_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MUSIC);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_music, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画背景
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_bg_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画movie
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_movie_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_movie_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_FOCUS_ICON_MOVIE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_BLUE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_movie, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //对应媒体的大图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_big_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_bg_rect.x;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_big_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_VIDEO_FILE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画返回图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].return_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].return_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //记录返去图标坐标
            ui_param->file_type_uiparam.return_bmp_rect.x = picX ;
            ui_param->file_type_uiparam.return_bmp_rect.y = picY ;
            //画photo
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_photo_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_photo_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_PHOTO);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_photo, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画ebook
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_ebook_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_ebook_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_EBOOK);
            GUI_SetColor(GUI_WHITE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_ebook, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);

            //画game
            if (EPDK_TRUE == game_card)
            {
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_game_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_GAME);
                GUI_SetColor(GUI_WHITE);
                GUI_BMP_Draw(pic_buf, picX, picY);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_game, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
            else
            {
                HTHEME h_theme;
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MOVIE].file_type_game_rect.y ;
                h_theme = theme_open(ID_EXPLORER_FILE_TYPE_TXT_BMP);
                pic_buf = theme_hdl2buf(h_theme);
                GUI_SetColor(GUI_WHITE);
                GUI_BMP_Draw(pic_buf, picX, picY);
            }
        }
        break;

        case RAT_MEDIA_TYPE_PIC:
        {
            //画music
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_music_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_music_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MUSIC);
            GUI_SetColor(GUI_WHITE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_music, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画背景
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画movie
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_movie_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_movie_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MOVIE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_movie, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //对应媒体的大图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_big_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.x;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_big_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_PHOTO_FILE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画返回图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].return_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].return_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //记录返去图标坐标
            ui_param->file_type_uiparam.return_bmp_rect.x = picX ;
            ui_param->file_type_uiparam.return_bmp_rect.y = picY ;
            //画photo
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_photo_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_photo_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_FOCUS_ICON_PHOTO);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_BLUE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_photo, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画ebook
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_ebook_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_ebook_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_EBOOK);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_ebook, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);

            //画game
            if (EPDK_TRUE == game_card)
            {
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_game_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_GAME);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_game, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
            else
            {
                HTHEME h_theme;
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_PHOTO].file_type_game_rect.y ;
                h_theme = theme_open(ID_EXPLORER_FILE_TYPE_TXT_BMP);
                pic_buf = theme_hdl2buf(h_theme);
                GUI_SetColor(GUI_WHITE);
                GUI_BMP_Draw(pic_buf, picX, picY);
            }
        }
        break;

        case RAT_MEDIA_TYPE_AUDIO:
        {
            //画背景
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_bg_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画music
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_music_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_music_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_FOCUS_ICON_MUSIC);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_BLUE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_music, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画movie
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_movie_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_movie_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MOVIE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_movie, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //对应媒体的大图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_big_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_bg_rect.x;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_big_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_MUSIC_FILE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画返回图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].return_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].return_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //记录返去图标坐标
            ui_param->file_type_uiparam.return_bmp_rect.x = picX ;
            ui_param->file_type_uiparam.return_bmp_rect.y = picY ;
            //画photo
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_photo_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_photo_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_PHOTO);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_photo, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画ebook
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_ebook_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_ebook_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_EBOOK);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_ebook, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);

            //画game
            if (EPDK_TRUE == game_card)
            {
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_game_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_GAME);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_game, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
            else
            {
                HTHEME h_theme;
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_game_rect.y ;
                h_theme = theme_open(ID_EXPLORER_FILE_TYPE_TXT_BMP);
                pic_buf = theme_hdl2buf(h_theme);
                GUI_SetColor(GUI_WHITE);
                GUI_BMP_Draw(pic_buf, picX, picY);
            }
        }
        break;

        case RAT_MEDIA_TYPE_EBOOK:
        {
            //画music
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_music_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_music_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MUSIC);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_music, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画背景
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_bg_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画movie
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_movie_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_movie_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MOVIE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_movie, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //对应媒体的大图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_big_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_bg_rect.x;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_big_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_EBOOK_FILE);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //画返回图标
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].return_bmp_rect.x +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_bg_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].return_bmp_rect.y +
                   ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_bg_rect.y;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
            GUI_BMP_Draw(pic_buf, picX, picY);
            //记录返去图标坐标
            ui_param->file_type_uiparam.return_bmp_rect.x = picX ;
            ui_param->file_type_uiparam.return_bmp_rect.y = picY ;
            //画photo
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_photo_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_photo_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_PHOTO);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_photo, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //画ebook
            picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_ebook_rect.x ;
            picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_ebook_rect.y ;
            pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_FOCUS_ICON_EBOOK);
            GUI_BMP_Draw(pic_buf, picX, picY);
            GUI_SetColor(GUI_BLUE);
            gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
            gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
            gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
            gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
            GUI_DispStringInRect(str_ebook, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);

            //画game
            if (EPDK_TRUE == game_card)
            {
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_game_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_GAME);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_game, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
            else
            {
                HTHEME h_theme;
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_EBOOK].file_type_game_rect.y ;
                h_theme = theme_open(ID_EXPLORER_FILE_TYPE_TXT_BMP);
                pic_buf = theme_hdl2buf(h_theme);
                GUI_SetColor(GUI_WHITE);
                GUI_BMP_Draw(pic_buf, picX, picY);
            }
        }
        break;

        case RAT_MEDIA_TYPE_GAME:
        {
            if (EPDK_TRUE == game_card)
            {
                //画背景
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_bg_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_bg_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
                GUI_BMP_Draw(pic_buf, picX, picY);
                //对应媒体的大图标
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_big_bmp_rect.x +
                       ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_bg_rect.x;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_big_bmp_rect.y +
                       ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_bg_rect.y;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_GAME_FILE);
                GUI_BMP_Draw(pic_buf, picX, picY);
                //画返回图标
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].return_bmp_rect.x +
                       ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_bg_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].return_bmp_rect.y +
                       ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_bg_rect.y;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
                GUI_BMP_Draw(pic_buf, picX, picY);
                //记录返去图标坐标
                ui_param->file_type_uiparam.return_bmp_rect.x = picX ;
                ui_param->file_type_uiparam.return_bmp_rect.y = picY ;
                //music
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_music_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_music_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MUSIC);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_music, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                //画movie
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_movie_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_movie_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_MOVIE);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_movie, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                //画photo
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_photo_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_photo_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_PHOTO);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_photo, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                //画ebook
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_ebook_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_ebook_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_UNFOCUS_ICON_EBOOK);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_WHITE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_ebook, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                //画game
                picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_game_rect.x ;
                picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_GAME].file_type_game_rect.y ;
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_FOCUS_ICON_GAME);
                GUI_BMP_Draw(pic_buf, picX, picY);
                GUI_SetColor(GUI_BLUE);
                gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
                gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
                gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
                gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
                GUI_DispStringInRect(str_game, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
        break;

        case RAT_MEDIA_TYPE_FOLDER:
            //pic_buf = explorer_get_listview_icon_res(ID_EXP_FOLDER);              //draw select border
            break;

        case RAT_MEDIA_TYPE_ALL:
            explorer_file_manager_draw(list_para, media_type);
            break;

        case RAT_MEDIA_TYPE_UNKNOWN:                                                //
            //pic_buf = explorer_get_listview_icon_res(ID_EXP_UNKNOWN_FILE);            //draw select border
            break;

        case RAT_MEDIA_TYPE_USB_DEVICE:
            //pic_buf = explorer_get_listview_icon_res(ID_EXP_USB);         //draw select border
            break;

        case RAT_MEDIA_TYPE_SD_DEVICE:
            //pic_buf = explorer_get_listview_icon_res(ID_EXP_SD);          //draw select border
            break;

        default:
            break;
    }

    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}
#endif

__s32 explorer_draw_FileTypeIcon(explr_list_para_t *list_para, rat_media_type_t media_type)
{
    void *pic_buf = NULL;
    __s32 picX = 0, picY = 0;
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();
    //GUI_LyrWinSel(list_para->BG_layer);
    //com_set_palette_by_id(ID_EXPLORER_PALETTE_BMP);
    //ret = explorer_get_item_focus_icon_rect(list_para, &FocusIconRect);
    picX = ui_param->ListBarArea.x + ui_param->list_item_ui_param.icon_rect.x ;
    picY = ui_param->ListBarArea.y + ui_param->list_item_ui_param.icon_rect.y ;

    switch (media_type)
    {
        case RAT_MEDIA_TYPE_VIDEO:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_VIDEO_ICON_FOCUSED);            //draw select border
            break;

        case RAT_MEDIA_TYPE_PIC:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_PHOTO_ICON_FOCUSED);            //draw select border
            break;

        case RAT_MEDIA_TYPE_AUDIO:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_MUSIC_ICON_FOCUSED);            //draw select border
            break;

        case RAT_MEDIA_TYPE_EBOOK:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_EBOOK_ICON_FOCUSED);            //draw select border
            break;

        case RAT_MEDIA_TYPE_GAME:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_GAME_ICON_FOCUSED);         //draw select border
            break;

        case RAT_MEDIA_TYPE_FOLDER:
            //pic_buf = explorer_get_listview_icon_res(ID_EXP_FOLDER);              //draw select border
            break;

        case RAT_MEDIA_TYPE_ALL:
        case RAT_MEDIA_TYPE_UNKNOWN:                                                //
            pic_buf = explorer_get_listview_icon_res(ID_EXP_UNKNOWN_FILE);          //draw select border
            break;

        case RAT_MEDIA_TYPE_USB_DEVICE:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_USB_ICON_FOCUSED);          //draw select border
            break;

        case RAT_MEDIA_TYPE_SD_DEVICE:
            pic_buf = explorer_get_listview_icon_res(ID_EXP_SD_ICON_FOCUSED);           //draw select border
            break;

        default:
            break;
    }

    //picW =  GUI_BMP_GetXSize(pic_buf);                                        //获得图片宽
    //picH =  GUI_BMP_GetYSize(pic_buf);                                        //获得图片高度
    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, picX, picY);
    }

    return EPDK_OK;
}

__s32 explorer_file_manager_draw(explr_list_para_t *list_para, rat_media_type_t media_type)
{
    void *pic_buf = NULL;
    __s32 picX = 0, picY = 0;
    explorer_viewer_ui_t *ui_param;
    char str[128] = { 0 } ;
    GUI_RECT  gui_rect ;
    ui_param = explorer_get_viewer_ui_param();

    if (RAT_MEDIA_TYPE_ALL != media_type)
    {
        __msg("It's file manager  break ...........\n");
    }

    //画背景
    picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_music_rect.x ;
    picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_music_rect.y ;
    pic_buf =  explorer_get_listview_icon_res(ID_EXP_FILE_TYPE_BG);
    GUI_BMP_Draw(pic_buf, picX, picY);
    //画文件管理器图标
    picX = ui_param->file_type_uiparam.file_manage_icon_rect.x ;
    picY = ui_param->file_type_uiparam.file_manage_icon_rect.y ;
    pic_buf =  explorer_get_listview_icon_res(ID_FILE_MAGAGER_LITTLE_ICON);
    GUI_BMP_Draw(pic_buf, picX, picY);
    dsk_langres_get_menu_text(STRING_EXPLR_MANAGER_TITLE, str, sizeof(str)) ;   // 画文件管理字符
    GUI_SetColor(GUI_BLUE);
    gui_rect.x0 = C_FILE_TYPE_TXT_X + picX ;
    gui_rect.y0 = C_FILE_TYPE_TXT_Y + picY ;
    gui_rect.x1 = gui_rect.x0 + C_FILE_TYPE_TXT_W ;
    gui_rect.y1 = gui_rect.y0 + C_FILE_TYPE_TXT_H ;
    GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    //画大图标
    picX = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_music_rect.x ;
    picY = ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_music_rect.y ;
    pic_buf =  explorer_get_listview_icon_res(ID_FILE_MAGAGER_BIG_ICON);
    GUI_BMP_Draw(pic_buf, picX + ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_big_bmp_rect.x, picY + ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].file_type_big_bmp_rect.y);
    //画返回图标
    pic_buf =  explorer_get_listview_icon_res(ID_EXP_RETURN_UNFOCUS);
    GUI_BMP_Draw(pic_buf, picX + ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].return_bmp_rect.x, picY + ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].return_bmp_rect.y);
    //记录返去图标坐标
    ui_param->file_type_uiparam.return_bmp_rect.x =  picX + ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].return_bmp_rect.x ;
    ui_param->file_type_uiparam.return_bmp_rect.y =  picY + ui_param->file_type_uiparam.file_type_pos[FILE_TYPE_MUSIC].return_bmp_rect.y ;

    //GUI_BMP_Draw(theme_hdl2buf(list_para->h_return_bmp),picX + ui_param->file_type_uiparam.return_bmp_rect.x,picY + ui_param->file_type_uiparam.return_bmp_rect.y);
    //画文件删除图标
    if (list_para->enter_sd_usb_flag == 1)
    {
        pic_buf =  explorer_get_listview_icon_res(ID_EXP_DELETE_FILE_UF_ICON);
        GUI_BMP_Draw(pic_buf, ui_param->file_type_uiparam.delete_file_icon_rect.x,  ui_param->file_type_uiparam.delete_file_icon_rect.y);
    }

    return EPDK_OK;
}
__s32 explorer_clear_filesize_area(explr_list_para_t *list_para)
{
#if 0
    explorer_viewer_ui_t *ui_param;
    GUI_RECT rect;
    __u32 file_size;
    void *p_buf ;
    GUI_MEMDEV_Handle   draw_mem;
    ui_param = explorer_get_viewer_ui_param();
    p_buf = explorer_get_listview_icon_res(ID_EXP_INFO_BG);
    rect.x0 = ui_param->list_item_ui_param.file_size_rect.x;
    rect.y0 = ui_param->list_item_ui_param.file_size_rect.y;
    rect.x1 = rect.x0 + ui_param->list_item_ui_param.file_size_rect.width;
    rect.y1 = rect.y0 + ui_param->list_item_ui_param.file_size_rect.height;
    draw_mem = GUI_MEMDEV_Create(rect.x0, rect.y0, rect.x1, rect.y1);
    GUI_MEMDEV_Select(draw_mem);
    GUI_BMP_Draw(p_buf, rect.x0, rect.y0);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    //  APP_BMP_DrawEx( list_para->list_layer, p_buf, rect.x0, rect.y0, rect.x0, rect.y0, rect.x1, rect.y1 );   //用背景色去擦除该区域
    //GUI_ClearRect(rect.x0,rect.y0, rect.x1, rect.y1);
#endif
    return EPDK_OK;
}
__s32 explorer_draw_FileSize(__lbar_draw_para_t *draw_param, __s32 file_size, explr_list_para_t *list_para)
{
    GUI_RECT rect = {0};
    char temp_array[24] = {0};
    explorer_viewer_ui_t *ui_param = NULL;
    char string_detail_prompt[128] = {0};
    filesize2str(file_size, temp_array);
    __wrn("draw_param->index:%d\r\n", draw_param->index);
    __wrn("file size:%s\r\n", temp_array);
    ui_param = explorer_get_viewer_ui_param();
    dsk_langres_get_menu_text(STRING_EXPLR_FILE_TIME, string_detail_prompt, sizeof(string_detail_prompt));
    eLIBs_strcat(string_detail_prompt, temp_array);
    __wrn("display size:%s\r\n", string_detail_prompt);
    rect.x0 = ui_param->list_item_ui_param.file_size_rect.x;
    rect.y0 = ui_param->list_item_ui_param.file_size_rect.y;
    rect.x1 = rect.x0 + ui_param->list_item_ui_param.file_size_rect.width;
    rect.y1 = rect.y0 + ui_param->list_item_ui_param.file_size_rect.height;
    GUI_ClearRectEx(&rect);
    GUI_DispStringInRect(string_detail_prompt, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    return EPDK_OK;
}

//时间转换为字符串函数(年月日字符串)
__s32 mtime_to_date_string(ES_TIMESTAMP *m_time, char *string)
{
    char temp_str[20] = { 0 };
    temp_str[0]   = m_time->year / 1000 + '0';
    temp_str[1]   = (m_time->year % 1000) / 100 + '0';
    temp_str[2]   = (m_time->year % 100) / 10 + '0';
    temp_str[3]   = m_time->year % 10 + '0';
    temp_str[4]   = 0;
    eLIBs_strcpy(string, temp_str);
    eLIBs_strcat(string, "-");
    eLIBs_memset(temp_str, 0, sizeof(temp_str));
    //get_menu_text(STRING_EXPLR_TIME_YEAR, temp_str, sizeof(temp_str));
    //eLIBs_strcat(string, temp_str);

    //  if(m_time->month > 10)
    if (m_time->month >= 10)
    {
        temp_str[0]   = m_time->month / 10 + '0';
        temp_str[1]   = m_time->month % 10 + '0';
        temp_str[2]   = 0;
    }
    else
    {
        temp_str[0]   = m_time->month + '0';
        temp_str[1]   = 0;
    }

    eLIBs_strcat(string, temp_str);
    eLIBs_strcat(string, "-");
    eLIBs_memset(temp_str, 0, sizeof(temp_str));
    //get_menu_text(STRING_EXPLR_TIME_MONTH, temp_str, sizeof(temp_str));
    //eLIBs_strcat(string, temp_str);

    //if(m_time->day > 10)
    if (m_time->day >= 10)
    {
        temp_str[0]   = m_time->day / 10 + '0';
        temp_str[1]   = m_time->day % 10 + '0';
        temp_str[2]   = 0;
    }
    else
    {
        temp_str[0]   = m_time->day + '0';
        temp_str[1]   = 0;
    }

    eLIBs_strcat(string, temp_str);
    //eLIBs_memset(temp_str, 0, sizeof(temp_str));
    //get_menu_text(STRING_EXPLR_TIME_DAY, temp_str, sizeof(temp_str));
    //eLIBs_strcat(string, temp_str);
    return EPDK_OK;
}
__s32 mtime_to_time_string(ES_TIMESTAMP *m_time, char *string)
{
    string[0]   = m_time->hour / 10 + '0';
    string[1]   = m_time->hour % 10 + '0';
    string[2]   = ':';
    string[3]  = m_time->minute / 10 + '0';
    string[4]  = m_time->minute % 10 + '0';
    string[5]   = ':';
    string[6]  = m_time->second / 10 + '0';
    string[7]  = m_time->second % 10 + '0';
    string[8]  = '\0';
#if 0
    char temp_str[10];
    temp_str[0]   = m_time->hour / 10 + '0';
    temp_str[1]   = m_time->hour % 10 + '0';
    temp_str[2]   = '\0';
    eLIBs_strcat(string, temp_str);
    eLIBs_memset(temp_str, 0, sizeof(temp_str));
    get_menu_text(STRING_EXPLR_TIME_HOUR, temp_str, sizeof(temp_str));
    eLIBs_strcat(string, temp_str);
    temp_str[0]  = m_time->minute / 10 + '0';
    temp_str[1]  = m_time->minute % 10 + '0';
    temp_str[2]   = '\0';
    eLIBs_strcat(string, temp_str);
    eLIBs_memset(temp_str, 0, sizeof(temp_str));
    get_menu_text(STRING_EXPLR_TIME_MINUTE, temp_str, sizeof(temp_str));
    eLIBs_strcat(string, temp_str);
    temp_str[0]  = m_time->second / 10 + '0';
    temp_str[1]  = m_time->second % 10 + '0';
    temp_str[2]   = '\0';
    eLIBs_strcat(string, temp_str);
    eLIBs_memset(temp_str, 0, sizeof(temp_str));
    get_menu_text(STRING_EXPLR_TIME_MINUTE, temp_str, sizeof(temp_str));
    eLIBs_strcat(string, temp_str);
#endif
    return EPDK_OK;
}

//清除文件创建时间区域
__s32 explorer_clear_file_create_time_area(explr_list_para_t *list_para)
{
#if 0
    int ret = 0;
    explorer_viewer_ui_t *ui_param;
    GUI_RECT rect;
    void *p_buf ;
    ui_param = explorer_get_viewer_ui_param();
    p_buf = explorer_get_listview_icon_res(ID_EXP_BACKGROUND);
    rect.x0 = ui_param->list_item_ui_param.time_rect.x;
    rect.y0 = ui_param->list_item_ui_param.time_rect.y;
    rect.x1 = rect.x0 + ui_param->list_item_ui_param.time_rect.width - 1 ;
    rect.y1 = rect.y0 + ui_param->list_item_ui_param.time_rect.height - 1;
    APP_BMP_DrawEx(list_para->list_layer, p_buf, rect.x0, rect.y0, rect.x0, rect.y0, rect.x1, rect.y1) ;
    APP_BMP_DrawEx(list_para->list_layer, p_buf, rect.x0, rect.y0 + C_DATE_Y_SPACE, rect.x0, rect.y0, rect.x1, rect.y1 + C_DATE_Y_SPACE) ;
    //GUI_BMP_Draw( p_buf , rect.x0 ,rect.y0  ) ;
#endif
    return EPDK_OK;
}


__s32 explorer_draw_file_create_time(__lbar_draw_para_t *draw_param, char *file_name, explr_list_para_t *list_para)
{
    int ret = 0;
    GUI_RECT rect;
    ES_TIMESTAMP mtime;
    char temp_array[24] = {0};
    explorer_viewer_ui_t *ui_param = NULL;
    char string_detail_prompt[128] = {0};
    ui_param = explorer_get_viewer_ui_param();
    rect.x0 = ui_param->list_item_ui_param.time_rect.x;
    rect.y0 = ui_param->list_item_ui_param.time_rect.y;
    rect.x1 = rect.x0 + ui_param->list_item_ui_param.time_rect.width ;
    rect.y1 = rect.y0 + ui_param->list_item_ui_param.time_rect.height;
    GUI_ClearRectEx(&rect);
    ret = eLIBs_GetFileMTime(file_name, &mtime);
    __msg("mtime.year=%d, mtime.month=%d,mtime.day=%d,mtime.hour=%d,mtime.minute=%d,mtime.second=%d\n", mtime.year, mtime.month, mtime.day, mtime.hour, mtime.minute, mtime.second);
    dsk_langres_get_menu_text(STRING_EXPLR_FILE_TIME, string_detail_prompt, sizeof(string_detail_prompt));
    eLIBs_memset(temp_array, 0, sizeof(temp_array));
    mtime_to_date_string(&mtime, temp_array);
    __msg("file create day: %s\n", temp_array);
    eLIBs_strcat(string_detail_prompt, temp_array);
    __msg("dislpay string: %s\n", string_detail_prompt);
    GUI_DispStringInRect(string_detail_prompt, &rect, GUI_TA_LEFT);
    return EPDK_OK;
}

void explorer_delete_file_draw_process(file_op_info_t *op_info)
{
    char str[10];
    GUI_RECT rect;
    eLIBs_memclr(str, sizeof(str));
    eLIBs_sprintf(str, "%d % ", op_info->percent);
    rect.x0 = C_DEL_FILE_PECENT_AREA_X;
    rect.y0 = C_DEL_FILE_PECENT_AREA_Y;
    rect.x1 = C_DEL_FILE_PECENT_AREA_X + C_DEL_FILE_PECENT_AREA_W;
    rect.y1 = C_DEL_FILE_PECENT_AREA_Y + C_DEL_FILE_PECENT_AREA_H;
    GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
    GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    return;
}
//只显示字符串到左边窗口
void explorer_delete_file_draw_process_ext(explr_list_para_t *list_para)
{
    char str[64];
    GUI_RECT rect;
    eLIBs_memclr(str, sizeof(str));
    //eLIBs_sprintf(str, "%d % ",op_info->percent);
    GUI_LyrWinSel(list_para->list_layer);
    GUI_SetFont(SWFFont);
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(0xF0);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    __msg(" explorer_delete_file_draw_process_ext \n");
    rect.x0 = C_DEL_FILE_PECENT_AREA_X;
    rect.y0 = C_DEL_FILE_PECENT_AREA_Y;
    rect.x1 = C_DEL_FILE_PECENT_AREA_X + C_DEL_FILE_PECENT_AREA_W;
    rect.y1 = C_DEL_FILE_PECENT_AREA_Y + C_DEL_FILE_PECENT_AREA_H;
    get_menu_text(STRING_EXPLR_DELETE_HINT, str, sizeof(str));
    GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
    //GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER|GUI_TA_HCENTER);
    GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    return;
}


//清除删除文件字符提示区域
void explorer_clean_delete_file_hint_area(void)
{
    GUI_RECT rect;
    rect.x0 = C_DEL_FILE_PECENT_AREA_X;
    rect.y0 = C_DEL_FILE_PECENT_AREA_Y;
    rect.x1 = C_DEL_FILE_PECENT_AREA_X + C_DEL_FILE_PECENT_AREA_W;
    rect.y1 = C_DEL_FILE_PECENT_AREA_Y + C_DEL_FILE_PECENT_AREA_H;
    GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
}
//删除文件失败提示字符
void explorer_draw_delete_file_failed(void)
{
    char str[64];
    GUI_RECT rect;
    eLIBs_memclr(str, sizeof(str));
    //eLIBs_sprintf(str, "%d % ",op_info->percent);
    rect.x0 = C_DEL_FILE_PECENT_AREA_X;
    rect.y0 = C_DEL_FILE_PECENT_AREA_Y;
    rect.x1 = C_DEL_FILE_PECENT_AREA_X + C_DEL_FILE_PECENT_AREA_W;
    rect.y1 = C_DEL_FILE_PECENT_AREA_Y + C_DEL_FILE_PECENT_AREA_H;
    get_menu_text(STRING_EXPLR_DELETE_FAILED, str, sizeof(str));
    GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
    //GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER|GUI_TA_HCENTER);
    GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    return;
}
__s32 explorer_draw_file_manager_title(void)
{
    char str[30];
    explorer_viewer_ui_t *ui_param;
    GUI_RECT rect;
    ui_param = explorer_get_viewer_ui_param();
    rect.x0 = ui_param->list_item_ui_param.title_rect.x;
    rect.y0 = ui_param->list_item_ui_param.title_rect.y;
    rect.x1 = rect.x0 + ui_param->list_item_ui_param.title_rect.width;
    rect.y1 = rect.y0 + ui_param->list_item_ui_param.title_rect.height;
    get_menu_text(STRING_EXPLR_MANAGER_TITLE, str, sizeof(str));
    GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
    GUI_DispStringInRect(str, &rect, GUI_TA_TOP | GUI_TA_HCENTER);
    return EPDK_OK;
}
void *explorer_get_file_item_icon(explorer_view_mode_e view_mode, rat_media_type_t media_type, exp_icon_status_e status)
{
    void *pic_buf = NULL;

    switch (media_type)
    {
        case RAT_MEDIA_TYPE_VIDEO:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_VIDEO_ICON_FOCUSED);
            }
            else  if (status == UNFOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_VIDEO_ICON_UNFOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_PIC:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_PHOTO_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_PHOTO_ICON_UNFOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_AUDIO:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_MUSIC_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_MUSIC_ICON_UNFOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_EBOOK:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_EBOOK_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_EBOOK_ICON_UNFOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_GAME:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_GAME_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_GAME_ICON_UNFOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_FOLDER:
            if (status == FOCUSED)
            {
                //pic_buf = explorer_get_listview_icon_res(ID_EXP_FOLDER_ICON_FOCUSED);
                pic_buf = explorer_get_listview_icon_res(ID_EXP_LISTBAR_FOLDER_ICON);               ///////////
            }
            else
            {
                //pic_buf = explorer_get_listview_icon_res(ID_EXP_FOLDER_ICON_UNFOCUSED);
                pic_buf = explorer_get_listview_icon_res(ID_EXP_LISTBAR_FOLDER_ICON);       //////////
            }

            break;

        case RAT_MEDIA_TYPE_UNKNOWN:            //未知文件先以ebook为准
        case RAT_MEDIA_TYPE_FIRMWARE:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_UNKNOWN_FILE_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_UNKNOWN_FILE_ICON_UNFOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_USB_DEVICE:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_USB_ICON_FOCUSED);
            }
            else
            {
                //pic_buf = explorer_get_listview_icon_res(ID_EXP_USB_ICON_UNFOCUSED);
                pic_buf = explorer_get_listview_icon_res(ID_EXP_USB_ICON_FOCUSED);
            }

            break;

        case RAT_MEDIA_TYPE_SD_DEVICE:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_SD_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_SD_ICON_FOCUSED);
            }

            break;

        default:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_UNKNOWN_FILE_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_UNKNOWN_FILE_ICON_UNFOCUSED);
            }

            break;
    }

    return pic_buf;
}


void *explorer_get_file_mediatype_item_icon(__lbar_draw_para_t *draw_param, exp_icon_status_e status)
{
    void *pic_buf = NULL;
    explr_list_para_t *this = NULL;
    this = (explr_list_para_t *)draw_param->attr;

    //switch(draw_param->index)
    switch (this->mediatype_index)
    {
        case ID_MEDIATYPE_ALL_ITEM:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_LISTBAR_FOLDER_ICON);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_LISTBAR_FOLDER_ICON);
            }

            break;

        case ID_MEDIATYPE_MUSIC_ITEM:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_MUSIC_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_MUSIC_ICON_UNFOCUSED);
            }

            break;

        case ID_MEDIATYPE_MOVIE_ITEM:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_VIDEO_ICON_FOCUSED);
            }
            else  if (status == UNFOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_VIDEO_ICON_UNFOCUSED);
            }

            break;

        case ID_MEDIATYPE_EBOOK_ITEM:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_EBOOK_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_EBOOK_ICON_FOCUSED);
            }

            break;

        case ID_MEDIATYPE_PHOTO_ITEM:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_PHOTO_ICON_FOCUSED);
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_PHOTO_ICON_UNFOCUSED);
            }

            break;
#if GAME_SW

        case ID_MEDIATYPE_GAME_ITEM:
            if (status == FOCUSED)
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_GAME_ICON_FOCUSED);//ID_EXP_GAME_ICON_FOCUSED
            }
            else
            {
                pic_buf = explorer_get_listview_icon_res(ID_EXP_GAME_ICON_UNFOCUSED);//ID_EXP_GAME_ICON_UNFOCUSED
            }

            break;
#endif

        default:
            break;
    }

    return pic_buf;
}



