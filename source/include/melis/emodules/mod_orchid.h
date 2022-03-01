/*
************************************************************************************************************************
*                                                        ORCHID
*                                    the ORCHID Media Information Management Midware
*
*                             Copyright(C), 2006-2008, Softwinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : drv_orchid.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date :  2008.07.17
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.01.15       1.1.0        build the file
*
* Gary.Wang      2008.07.17       1.1.0        many modification
*
************************************************************************************************************************
*/
#ifndef  __MOD_ORCHID_h
#define  __MOD_ORCHID_h
#include <mediainfo/mediafmt.h>
#include <mod_defs.h>

#define ORC_CMD_BASE_PLAYLIST           0X1000
#define ORC_CMD_BASE_NPL                0X2000
#define ORC_CMD_BASE_MLIB               0X3000
#define ORC_CMD_BASE_ATTACH             0X4000
#define ORC_CMD_BASE_GLOBAL             0X5000

/* ORCHID io control command list */
typedef enum _orc_cmd_e
{
    /* now playlist */
    ORC_CMD_CONSTRUCT_NOW_PLAYLIST = ORC_CMD_BASE_NPL,
    ORC_CMD_DESTROY_NOW_PLAYLIST,
    ORC_CMD_ADD_ITEM_TO_NPL,
    ORC_CMD_DELETE_ITEM_FROM_NPL,
    ORC_CMD_FILE_TO_INDEX_IN_NPL,
    ORC_CMD_INDEX_TO_FILE_IN_NPL,
    ORC_CMD_CLEAR_NPL,
    ORC_CMD_GET_TOTAL_ITEM_COUNT_OF_NPL,
    ORC_CMD_GET_VALID_ITEM_COUNT_OF_NPL,
    ORC_CMD_SET_PLAY_MODE,
    ORC_CMD_GET_PLAY_MODE,
    ORC_CMD_PLAYING_SEEK,
    ORC_CMD_PLAYING_GET_CUR,
    ORC_CMD_PLAYING_AUTO_GET_NEXT,
    ORC_CMD_IS_VALID_ITEM_IN_NPL,
    ORC_CMD_INVALIDATE_ITEM_IN_NPL,
    ORC_CMD_NPL_IS_DIRTY,
    ORC_CMD_NPL_CLEAR_DIRTY_FLAG,
    ORC_CMD_SAVE_NPL,
    ORC_CMD_LOAD_LAST_NPL,
    /* attachment */
    ORC_CMD_SET_ATTACH         = ORC_CMD_BASE_ATTACH,
    ORC_CMD_DELETE_ATTACH,
    ORC_CMD_OPEN_ATTACH,
    ORC_CMD_CLOSE_ATTACH,
    ORC_CMD_GET_ATTACH_SIZE,
    ORC_CMD_GET_ATTACH_CONTENT,
    /* global */
    ORC_CMD_ADD_FILE_TO_ORCHID = ORC_CMD_BASE_GLOBAL,
    ORC_CMD_DELETE_FILE_FROM_ORCHID,
    ORC_CMD_SET_STAR,
    ORC_CMD_SET_RATE,
    ORC_CMD_GET_MEDIA_INFO,
    ORC_CMD_FILE_TO_UID,
    ORC_CMD_GET_ERROR,
    ORC_CMD_CLR_ERROR,
    ORC_CMD_UPDATE_ORCHID,
    ORC_CMD_BEGIN_BROWSE_ALL_ITEM,
    ORC_CMD_END_BROWSE_ALL_ITEM,
    ORC_CMD_GET_TOTAL_NUMBER,
    ORC_CMD_GET_NEXT_ITEM,
    ORC_CMD_ORCHID_IS_VALID,
    ORC_CMD_REBUILD_ORCHID,
    /* browse media lib and playlist module */
    ORC_CMD_OPEN_DIR           = ORC_CMD_BASE_MLIB,
    ORC_CMD_CLOSE_DIR,
    ORC_CMD_REWIND_DIR,
    ORC_CMD_GET_ENTRY_COUNT,
    ORC_CMD_GET_NEXT_ENTRY,
    ORC_CMD_ENTRY_TO_ATTR,
    ORC_CMD_ENTRY_TO_NAME,
    ORC_CMD_ENTRY_TO_ENCODING,
    /* playlist */
    ORC_CMD_CREATE_PLAYLIST    = ORC_CMD_BASE_PLAYLIST,
    ORC_CMD_DELETE_PLAYLIST,
    ORC_CMD_ADD_ITEM_TO_PLAYLIST,
    ORC_CMD_DELETE_ITEM_FROM_PLAYLIST,

    ORC_CMD_                            // always the last one
} orc_cmd_e;


/* global error code */
typedef enum _orc_error_e
{
    ORC_OK                   = 0,
    ORC_ERR_TITLE_USED,
    ORC_ERR_LIST_END,
    ORC_ERR_INVALID_TITLE,
    ORC_ERR_INVALID_UID,
    ORC_ERR_INVALID_INPUT,
    ORC_ERR_INVALID_MEDIA_TYPE,
    ORC_ERR_INVALID_SORT_MODE,
    ORC_ERR_INVALID_PLAY_MODE,
    ORC_ERR_INVALID_STAR,
    ORC_ERR_INVALID_IOCTRL_CMD,
    ORC_ERR_INVALID_ITEM,
    ORC_ERR_FULL,
    ORC_ERR_NOW_PLAYLIST_EMPTY,
    ORC_ERR_NO_ITEM_TO_PLAY,
    ORC_ERR_SLOP_OVER,
    ORC_ERR_EXTRACT_INFO_FAIL,
    ORC_ERR_NO_ITEM_BEING_PLAYED,
    ORC_ERR_FILE_OP_FAIL,
    ORC_ERR_MALLOC_FAIL,
    ORC_ERR_FATAL_ERR,
    ORC_ERR_NPL_HANDLE_DIRTY,
    ORC_ERR_NPL_WORK_MODE_EXIST,
    ORC_ERR_NPL_IN_MANAGE_MODE,
    ORC_ERR_NPL_ILLEGAL_OP,

    ORC_ERR                     = -1             // always the last one.
} orc_error_e;


/* media type */
typedef enum _orc_media_type_e
{
    ORC_MEDIA_TYPE_AUDIO = 0,
    ORC_MEDIA_TYPE_VIDEO,
    ORC_MEDIA_TYPE_PIC,
    ORC_MEDIA_TYPE_EBOOK,
    ORC_MEDIA_TYPE_FLASH,

    ORC_MEDIA_TYPE_UNKNOWN = -1
} orc_media_type_e;


/* play mode */
typedef enum _orc_play_mode_e
{
    ORC_PLAY_MODE_ONLY_ONCE = 0,
    ORC_PLAY_MODE_ROTATE_ONE,
    ORC_PLAY_MODE_ROTATE_ALL,
    ORC_PLAY_MODE_SEQUENCE,
    ORC_PLAY_MODE_RANDOM,

    ORC_PLAY_MODE_UNKNOWN   = -1                  // always the last one
} orc_play_mode_e;


/* npl work mode */
typedef enum
{
    ORC_NPL_WORK_MODE_MANAGE = 0,       // 管理模式，可以进行所有操作。一旦有句柄处于此模式，所有相同媒体类型的句柄的操作都将失败。每种媒体类型，最多只能有一个句柄处于此模式。
    ORC_NPL_WORK_MODE_USE,              // 使用模式，可以读，可以操作playing pointer. 每种媒体类型，最多只能有一个句柄处于此模式。
    ORC_NPL_WORK_MODE_QUERY,            // 查询模式，只读。可以同时以此模式打开多个句柄。

    ORC_NPL_WORK_MODE_UNKNOWN = -1
} orc_npl_work_mode_e;


#define ORC_MEDIA_LIB_ROOT                        "media_lib_root"
#define ORC_PLAYLIST_ROOT                         "playlist_root"

#define AUDIO_TYPE_STR                            "audio"
#define VIDEO_TYPE_STR                            "video"
#define PIC_TYPE_STR                              "pic"
#define EBOOK_TYPE_STR                            "ebook"
#define GAME_TYPE_STR                             "game"

/* category attribute */
#define ARTIST                                    "artist"
#define ALBUM                                     "album"
#define GENRE                                     "genre"
#define YEAR                                      "year"
#define STAR                                      "star"
#define DATE                                      "date"
#define ORC_ALL                                   "all"
#define UNKNOWN_VARIETY                           "Unknown"

/* variety name */
#define ORC_VARIETY_NAME_0STAR                    "0star"
#define ORC_VARIETY_NAME_1STAR                    "1star"
#define ORC_VARIETY_NAME_2STAR                    "2star"
#define ORC_VARIETY_NAME_3STAR                    "3star"
#define ORC_VARIETY_NAME_4STAR                    "4star"
#define ORC_VARIETY_NAME_5STAR                    "5star"

/* orchid entry attribute */
typedef enum _orc_entry_attr_e
{
    ORC_ENTRY_ATTR_MLIB_ROOT   = 0,
    ORC_ENTRY_ATTR_MLIB_MEDIA_TYPE,
    ORC_ENTRY_ATTR_MLIB_CATE_ATTR,
    ORC_ENTRY_ATTR_MLIB_VARIETY,
    ORC_ENTRY_ATTR_ITEM,
    ORC_ENTRY_ATTR_PL_ROOT,
    ORC_ENTRY_ATTR_PL_PLAYLIST,
    ORC_ENTRY_ATTR_PL_ITEM,

    ORC_ENTRY_ATTR_UNKNOWN    = -1
} orc_entry_attr_e;


/* origin mode */
#define ORC_NPL_SEEK_SET                          0
#define ORC_NPL_SEEK_CUR                          1
#define ORC_NPL_SEEK_END                          2


#define ORC_MEDIA_FILE_NAME_SIZE                ( 255 + 512 ) // add by Gary 2010-6-9 11:33:41
#define ORC_TITLE_SIZE                           47
#define ORC_ARTIST_SIZE                          47
#define ORC_ALBUM_SIZE                           47
#define ORC_GENRE_SIZE                           31
#define ORC_FILE_FORMAT_SIZE                      7
#define ATTACH_TITLE_MAX_SIZE                     7

#define ORC_STR_MAX_SIZE                         47


#define ORC_ATTACH_TITLE_THUMB                    "THUMB"


typedef struct _orc_date_t
{
    __u8   day;
    __u8   month;
    __u16  year;
} orc_date_t;


typedef struct _orc_id3_pic_info_t
{
    __u32               length;      // 0 means no picture. The length of the picture info in the file.
    __u32               offset;      // The offset of the picture info in the file.
    __sw_pic_format_e   format;      // jpg, bmp, and so on
} orc_id3_pic_info_t;


typedef struct _audio_info_t
{
    __s32  media_type;
    __u32  UID;
    char   media_file[ORC_MEDIA_FILE_NAME_SIZE + 1];
    char   title[ORC_TITLE_SIZE + 1];
    __s32  title_encoding;
    char   artist[ORC_ARTIST_SIZE + 1];                     // null string : artist is unknown.
    __s32  artist_encoding;
    char   album[ORC_ALBUM_SIZE + 1];                       // null string : album is unknown.
    __s32  album_encoding;
    char   genre[ORC_GENRE_SIZE + 1];                       // null string : genre is unknown.
    __s32  genre_encoding;
    __s32  file_format;
    __u32  rate;                                            // 0 : rate is unknown; unit is Kbps.
    __u32  star;
    __u32  year;
    __u32  duration;                                        // 0 : the duration is unknown.
    __u32  sample_rate;
    orc_id3_pic_info_t  album_cover_info;
} audio_info_t;


typedef struct _video_info_t
{
    __s32  media_type;
    __u32  UID;
    char   media_file[ORC_MEDIA_FILE_NAME_SIZE + 1];
    char   title[ORC_TITLE_SIZE + 1];
    __s32  title_encoding;
    char   artist[ORC_ARTIST_SIZE + 1];                     // null string : artist is unknown.
    __s32  artist_encoding;
    __s32  file_format;
    __u32  rate;                                            // 0 : rate is unknown; unit is Kbps.
    __u32  star;
    __u32  year;
    __u32  duration;                                        // 0 : the duration is unknown.
} video_info_t;


typedef struct _pic_info_t
{
    __s32  media_type;
    __u32  UID;
    char   media_file[ORC_MEDIA_FILE_NAME_SIZE + 1];
    char   title[ORC_TITLE_SIZE + 1];
    __s32  title_encoding;
    __s32  file_format;
    __u32  star;
    orc_date_t date;                                        // 0 : the date is unknown
} pic_info_t;


typedef union _media_info_t
{
    audio_info_t  audio_info;
    video_info_t  video_info;
    pic_info_t    pic_info;
} media_info_t;


typedef __u32      HDIR;
typedef __u32      HENT;
typedef __u32      HATH;
typedef unsigned long      HNPL;
typedef __u32      HAIS;



/* ORCHID io control argument type list */

typedef struct _orc_arg_seek_t
{
    HNPL   hnpl;              // 输入。now playlist对象的句柄
    __s32  offset;            // 输入。偏移量
    __s32  origin;            // 输入。起点位置，可选值有三种：
    // ORC_NPL_SEEK_SET：从开始处，
    // ORC_NPL_SEEK_CUR：从当前位置，
    // ORC_NPL_SEEK_END：从末尾处。
} orc_arg_seek_t;

typedef struct _orc_arg_set_attach_t
{
    __u32      UID;           // 输入。条目的UID
    const char *title;        // 输入。附件的标题
    const void *buf;          // 输入。附件的内容
    __u32      size;          // 输入。附件的尺寸
} orc_arg_set_attach_t;

typedef struct _orc_arg_get_attach_t
{
    HATH    hath;             // 输入。附件对象的句柄
    void    *buf;             // 输出。用来存放读取出的附件内容的缓冲区
    __u32   size;             // 输入。希望读取的尺寸值。如果此值小于等于附件真正的尺寸值，将返回
    // size 大小的附件内容；否则，报错
} orc_arg_get_attach_t;

typedef struct _orc_arg_index_to_file_t
{
    HNPL    hnpl;             // 输入。now playlist对象的句柄
    __u32   index;            // 输入。条目在now playlist中的索引号。
    char   *file;             // 输出。条目的文件名
} orc_arg_index_to_file_t;



typedef struct _orc_arg_npl_t
{
    orc_media_type_e     media_type;    // 输入。媒体类型
    orc_npl_work_mode_e  work_mode;     // 输入。工作模式。
} orc_arg_npl_t;


__s32 DRV_OrchidInit(void);
__s32 DRV_OrchidExit(void);
__mp *DRV_OrchidOpen(__u32 id, __u32 mode);
__s32 DRV_OrchidClose(__mp *mp);
__u32 DRV_OrchidRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DRV_OrchidWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 DRV_OrchidIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);
/*--------------------------------------------------------------------------------------------------
*                             construct_now_playlist
* mp          :  handle
* cmd         :  ORC_CMD_CONSTRUCT_NOW_PLAYLIST
* aux         :  void.
* pbuffer     :  orc_arg_npl_t *arg;    // 输入。
* return type :  HNPL
*   NULL         创建失败
*   !NULL        创建成功，返回值为构造出的now playlist的句柄
* description :  创建一个 now playlist 对象
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             destroy_now_playlist
* mp          :  handle
* cmd         :  ORC_CMD_DESTROY_NOW_PLAYLIST
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  销毁一个 now playlist 对象
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             add_item_to_npl
* mp          :  handle
* cmd         :  ORC_CMD_ADD_ITEM_TO_NPL
* aux         :  HNPL  hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  const char *file;       // 输入。文件名。
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将某条目添加到now playlist中。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             delete_item_from_npl
* mp          :  handle
* cmd         :  ORC_CMD_DELETE_ITEM_FROM_NPL
* aux         :  HNPL  hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  const char *file;       // 输入。文件名。
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  从now_playlist中删除某个条目。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             file_to_index_in_npl
* mp          :  handle
* cmd         :  ORC_CMD_FILE_TO_INDEX_IN_NPL
* aux         :  HNPL  hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  const char *file;       // 输入。文件名。
* return type :  __u32
*  0xFFFFFFFF    操作失败
*  other value   操作成功，返回条目在now playlist中的索引号
* description :  获取条目在now playlist中的索引号
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             index_to_file_in_npl
* mp          :  handle
* cmd         :  ORC_CMD_INDEX_TO_FILE_IN_NPL
* aux         :  none
* pbuffer     :  orc_arg_index_to_file_t  *arg;   // 输入
* return type :  __u32
*  0xFFFFFFFF    操作失败
*  other value   操作成功，返回条目的UID。
* description :  根据条目在now playlist中的索引号获取条目的UID。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             clear_npl
* mp          :  handle
* cmd         :  ORC_CMD_CLEAR_NPL
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  清空 now playlist.
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_total_item_count_of_npl
* mp          :  handle
* cmd         :  ORC_CMD_GET_TOTAL_ITEM_COUNT_OF_NPL
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __u32
*   0xFFFFFFFF   操作失败
*   其他值       操作成功，now playlist中条目的总数目
* description :  获取 now playlist 中条目的总数目
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_valid_item_count_of_npl
* mp          :  handle
* cmd         :  ORC_CMD_GET_VALID_ITEM_COUNT_OF_NPL
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __u32
*   0xFFFFFFFF   操作失败
*   其他值       操作成功，now playlist中有效条目的总数目
* description :  获取 now playlist 中有效条目的总数目
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             set_play_mode
* mp          :  handle
* cmd         :  ORC_CMD_SET_PLAY_MODE
* aux         :  HNPL hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  __s32 play_mode        // 输入。播放模式
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  设置播放模式
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_play_mode
* mp          :  handle
* cmd         :  ORC_CMD_GET_PLAY_MODE
* aux         :  HNPL hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  __s32 *play_mode_p     // 输出。播放模式
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  设置播放模式
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             playing_seek
* mp          :  handle
* cmd         :  ORC_CMD_PLAYING_SEEK
* aux         :  none
* pbuffer     :  orc_arg_seek_t  *arg;   // 输入。
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  设置播放指针(playing pointer)的位置
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             playing_get_cur
* mp          :  handle
* cmd         :  ORC_CMD_PLAYING_GET_CUR
* aux         :  HNPL   hnpl;         // 输入。now playlist对象的句柄
* pbuffer     :  __u32  *index_p;     // 输出。返回playing pointer所指向条目在now_playlist中的索引号
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  获取播放指针(playing pointer)当前所指向条目在now_playlist中的索引号。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             playing_auto_get_next
* mp          :  handle
* cmd         :  ORC_CMD_PLAYING_AUTO_GET_NEXT
* aux         :  HNPL   hnpl;         // 输入。now playlist对象的句柄
* pbuffer     :  __u32  *index_p;     // 输出。存储条目在now_playlist中的索引号
* return type :  __s32
*   ORC_OK                       操作成功
*   ORC_ERR_NO_ITEM_TO_PLAY      在当前模式下，找不到能播放的条目
*   ORC_ERR_NOW_PLAYLIST_EMPTY   now playlist is empty
*   ORC_ERR                      操作失败
* description :  在自动播放模式下，依据设置的播放模式(play mode)获得下一个待播放的曲目在now_playlist
*                中的索引号。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             invalidate_item_in_npl
* mp          :  handle
* cmd         :  ORC_CMD_INVALIDATE_ITEM_IN_NPL
* aux         :  HNPL hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  __u32 index;           // 输入。条目的索引值
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将某条目设置为”无效“状态
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             is_valid_item_in_npl
* mp          :  handle
* cmd         :  ORC_CMD_IS_VALID_ITEM_IN_NPL
* aux         :  HNPL hnpl;             // 输入。now playlist对象的句柄
* pbuffer     :  __u32 index;           // 输入。条目的索引值
* return type :  __bool
*                如果此条目有效，返回”真“，否则返回”假“
* description :  察看某条目是否有效
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             npl_clear_dirty_flag
* mp          :  handle
* cmd         :  ORC_CMD_NPL_CLEAR_DIRTY_FLAG
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  清除此句柄的dirty标志
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             npl_is_dirty
* mp          :  handle
* cmd         :  ORC_CMD_NPL_IS_DIRTY
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __bool
* description :  查看此now playlist是否为"dirty"。如果为dirty，需要调用ORC_CMD_IS_VALID_ITEM_IN_NPL
*                清空"dirty"标志，然后再进行其他操作。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             save_npl
* mp          :  handle
* cmd         :  ORC_CMD_SAVE_NPL
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  保存 now playlist.
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             load_last_npl
* mp          :  handle
* cmd         :  ORC_CMD_LOAD_LAST_NPL
* aux         :  none
* pbuffer     :  HNPL hnpl;             // 输入。now playlist对象的句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将上次的播放列表加载到npl中
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             set_attach
* mp          :  handle
* cmd         :  ORC_CMD_SET_ATTACH
* aux         :  none
* pbuffer     :  orc_arg_set_attach_t *
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  设置条目某个附件的内容
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             delete_attach
* mp          :  handle
* cmd         :  ORC_CMD_DELETE_ATTACH
* aux         :  __u32      UID;        // 输入。条目的UID
* pbuffer     :  const char *title;     // 输入。附件的标题
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  删除条目的某个附件
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             open_attach
* mp          :  handle
* cmd         :  ORC_CMD_OPEN_ATTACH
* aux         :  __u32      UID;        // 输入。条目的UID
* pbuffer     :  const char *title;     // 输入。附件的标题
* return type :  HATH
*   NULL         操作失败
*   !NULL        操作成功，返回HATH类型的句柄
* description :  打开条目的某个附件，以便获取附件的信息
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             close_attach
* mp          :  handle
* cmd         :  ORC_CMD_CLOSE_ATTACH
* aux         :  none
* pbuffer     :  HATH   hath;           // 输入。附件对象的句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  关闭附件。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_attach_size
* mp          :  handle
* cmd         :  ORC_CMD_GET_ATTACH_SIZE
* aux         :  none
* pbuffer     :  HATH   hath;           // 输入。附件对象的句柄
* return type :  __u32
*   0xFFFFFFFF   操作失败
*   其他值       操作成功，返回值便是附件的尺寸值
* description :  获取附件的尺寸值。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_attach_content
* mp          :  handle
* cmd         :  ORC_CMD_GET_ATTACH_CONTENT
* aux         :  none
* pbuffer     :  orc_arg_get_attach_t *
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  获取附件的内容
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             add_file_to_orchid
* mp          :  handle
* cmd         :  ORC_CMD_ADD_FILE_TO_ORCHID
* aux         :  const char *file;      // 输入。文件的完整路径名
* pbuffer     :  __u32   *UID_p;        // 输出。此文件的UID。此参数可以为空
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将文件添加到ORCHID模块。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             delete_file_from_orchid
* mp          :  handle
* cmd         :  ORC_CMD_DELETE_FILE_FROM_ORCHID
* aux         :  none
* pbuffer     :  const char *file;      // 输入。文件的完整路径名
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  从orchid模块中删除文件
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             set_star
* mp          :  handle
* cmd         :  ORC_CMD_SET_STAR
* aux         :  __u32      UID;        // 输入。条目的UID
* pbuffer     :  __s32      star;       // 输入。星级，可选值: 0--5
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  设置某条目的星级
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             set_rate
* mp          :  handle
* cmd         :  ORC_CMD_SET_RATE
* aux         :  __u32      UID;        // 输入。条目的UID
* pbuffer     :  __s32      rate;       // 输入。速率，单位为：Kbps
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  设置某条目的速率
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_media_info
* mp          :  handle
* cmd         :  ORC_CMD_GET_MEDIA_INFO
* aux         :  __u32      UID;        // 输入。条目的UID
* pbuffer     :  media_info_t  *info_p; // 输出。存放获取的信息
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  获取条目的媒体信息。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             file_to_UID
* mp          :  handle
* cmd         :  ORC_CMD_FILE_TO_UID
* aux         :  none
* pbuffer     :  const char *file;      // 输入。文件的完整路径名
* return type :  __u32
*   0xFFFFFFFF   操作失败
*   其他值       操作成功，返回值便是文件对应的UID
* description :  获取文件对应的UID
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_error
* mp          :  handle
* cmd         :  ORC_CMD_GET_ERROR
* aux         :  none
* pbuffer     :  none
* return type :  __s32        错误码
* description :  获取错误码
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             clr_error
* mp          :  handle
* cmd         :  ORC_CMD_CLR_ERROR
* aux         :  none
* pbuffer     :  none
* return type :  void
* description :  清空错误码
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             update_orchid
* mp          :  handle
* cmd         :  ORC_CMD_UPDATE_ORCHID
* aux         :  none
* pbuffer     :  none
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  更新orchid模块
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             add_portable_disks
* mp          :  handle
* cmd         :  ORC_CMD_ADD_PORTABLE_DISKS
* aux         :  none
* pbuffer     :  none
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  当有可移动存储设备插入到系统中时，调用此函数更新orchid模块
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             remove_portable_disks
* mp          :  handle
* cmd         :  ORC_CMD_REMOVE_PORTABLE_DISKS
* aux         :  none
* pbuffer     :  none
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  当可移动存储设备从系统中移出时，调用此函数更新orchid模块
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             begin_browse_all_item
* mp          :  handle
* cmd         :  ORC_CMD_BEGIN_BROWSE_ALL_ITEM
* aux         :  none
* pbuffer     :  __s32 media_type;           // 输入。媒体类型
* return type :  HAIS
*   NULL         操作失败
*   其他值       操作成功，返回一个操作句柄
* description :  获取浏览某类型媒体所有item的句柄
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             end_browse_all_item
* mp          :  handle
* cmd         :  ORC_CMD_END_BROWSE_ALL_ITEM
* aux         :  none
* pbuffer     :  HAIS  hais;           // 输入。浏览某类型媒体所有item的句柄
* return type :  __s32
*   ORC_ERR      操作失败
*   ORC_OK       操作成功
* description :  关闭浏览某类型媒体所有item的句柄
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_total_number
* mp          :  handle
* cmd         :  ORC_CMD_GET_TOTAL_NUMBER
* aux         :  none
* pbuffer     :  HAIS  hais;           // 输入。浏览某类型媒体所有item的句柄
* return type :  __u32
*   0xFFFFFFFF   操作失败
*   其他值       操作成功，所有条目的总数目
* description :  获取某类型媒体所有条目的总数目
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_next_item
* mp          :  handle
* cmd         :  ORC_CMD_GET_NEXT_ITEM
* aux         :  HAIS  hais;           // 输入。浏览某类型媒体所有item的句柄
* pbuffer     :  uint32 *UID_p;        // 输出。存储条目的UID
* return type :  __s32
*   ORC_ERR            操作失败
*   ORC_OK             操作成功
*   ORC_ERR_LIST_END   到达列表末尾
* description :  获取某类型媒体下一个条目的UID
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             rebuild_orchid
* mp          :  handle
* cmd         :  ORC_CMD_REBUILD_ORCHID
* aux         :  none
* pbuffer     :  none
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  在orchid无效的情况下，重建orchid模块
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             orchid_is_valid
* mp          :  handle
* cmd         :  ORC_CMD_ORCHID_IS_VALID
* aux         :  none
* pbuffer     :  none
* return type :  __bool
*   EPDK_TRUE    orchid有效
*   EPDK_FALSE   orchid无效
* description :  察看orchid当前是否有效。若无效，必须调用rebuild_orchid接口重建orchid，才能正常使用
*                orchid.
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             open_dir
* mp          :  handle
* cmd         :  ORC_CMD_OPEN_DIR
* aux         :  none
* pbuffer     :  const char *path;      // 输入。目录的完整路径
* return type :  HDIR
*   NULL         操作失败
*   !NULL        操作成功，返回HDIR类型的句柄
* description :  打开目录浏览句柄
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             close_dir
* mp          :  handle
* cmd         :  ORC_CMD_CLOSE_DIR
* aux         :  none
* pbuffer     :  HDIR   hdir;           // 输入。目录句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  关闭目录浏览句柄
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             rewind_dir
* mp          :  handle
* cmd         :  ORC_CMD_REWIND_DIR
* aux         :  none
* pbuffer     :  HDIR   hdir;           // 输入。目录句柄
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将浏览指针重新置位于起始点，以便从头开始依次读取所有的entry.
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_entry_count
* mp          :  handle
* cmd         :  ORC_CMD_GET_ENTRY_COUNT
* aux         :  none
* pbuffer     :  HDIR   hdir;           // 输入。目录句柄
* return type :  __u32
*   0xFFFFFFFF   操作失败
*   其他值       操作成功，返回值便是目录下entry的总数目
* description :  获取目录下entry的总数目
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             get_next_entry
* mp          :  handle
* cmd         :  ORC_CMD_GET_NEXT_ENTRY
* aux         :  none
* pbuffer     :  HDIR   hdir;           // 输入。目录句柄
* return type :  HENT
*   NULL         操作失败或者NO more entry. 此时查询错误码，若错误码值为ORC_ERR_LIST_END，则说明NO
*                    more entry; 否则，则说明有错误发生。
*   !NULL        操作成功，返回HENT类型的句柄
* description :  获取下一个entry。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             entry_to_attr
* mp          :  handle
* cmd         :  ORC_CMD_ENTRY_TO_ATTR
* aux         :  none
* pbuffer     :  HENT   hent;           // 输入。entry句柄
* return type :  __s32
*   ORC_ENTRY_ATTR_UNKNOWN  操作失败
*   其他值                  操作成功，返回值便是entry的属性值
* description :  获取entry的属性值
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             entry_to_name
* mp          :  handle
* cmd         :  ORC_CMD_ENTRY_TO_NAME
* aux         :  HENT   hent;           // 输入。entry句柄
* pbuffer     :  void   *buf;           // 输出。如果entry的属性是ORC_ENTRY_ATTR_ITEM, 那么void *buf
*                                          // 的真实类型为uint32 *UID_p, 用来存放条目的UID。如果
*                                          // entry的属性不是ORC_ENTRY_ATTR_ITEM，那么void *buf的真
*                                          // 实类型为char *name, 用来存放entry的name，此时name的长
*                                          // 度应为ORC_STR_MAX_SIZE+1。
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  获取entry的name或UID
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             entry_to_encoding
* mp          :  handle
* cmd         :  ORC_CMD_ENTRY_TO_ENCODING
* aux         :  none
* pbuffer     :  HENT   hent;           // 输入。entry句柄
* return type :  __s32
*   ORC_ENTRY_ENCODING_UNKNOWN  操作失败
*   其他值                      操作成功，返回值便是entry的属性值
* description :  获取entry的字符编码类型
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             create_playlist
* mp          :  handle
* cmd         :  ORC_CMD_CREATE_PLAYLIST
* aux         :  const char *title;       // 输入。播放列表的标题，标题的长度必须小于ORC_TITLE_SIZE.
* pbuffer     :  __s32 media_type;        // 输入。媒体类型。
* return type :  __s32
*   ORC_OK              成功创建一个新的播放列表playlist
*   ORC_ERR             操作失败
*   ORC_ERR_TITLE_USED  具有此标题title的播放列表已经存在
* description :  创建一个新的播放列表。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             delete_playlist
* mp          :  handle
* cmd         :  ORC_CMD_DELETE_PLAYLIST
* aux         :  none
* pbuffer     :  const char *title;       // 输入。播放列表的标题，标题的长度必须小于ORC_TITLE_SIZE.
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  删除一个播放列表
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             add_item_to_playlist
* mp          :  handle
* cmd         :  ORC_CMD_ADD_ITEM_TO_PLAYLIST
* aux         :  const char *file;      // 输入
* pbuffer     :  const char *title;     // 输入。播放列表的标题，标题的长度必须小于ORC_TITLE_SIZE
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将某个条目添加到某播放列表中。
--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------
*                             delete_item_from_playlist
* mp          :  handle
* cmd         :  ORC_CMD_DELETE_ITEM_FROM_PLAYLIST
* aux         :  const char *file;      // 输入
* pbuffer     :  const char *title;     // 输入。播放列表的标题，标题的长度必须小于ORC_TITLE_SIZE
* return type :  __s32
*   ORC_OK       操作成功
*   ORC_ERR      操作失败
* description :  将某条目从某个播放列表中删除。
--------------------------------------------------------------------------------------------------*/

#endif     //  ifndef __drv_orchid_h

/* end of drv_orchid.h  */
