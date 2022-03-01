#ifndef __RAT_H__
#define __RAT_H__
#include <kconfig.h>

typedef ScanedEntry_t rat_entry_t;//to store rat entry information.

typedef struct  __rat_list
{
    HSCAN   hscan;
    rat_media_type_t media_type;
    char    str_path[RAT_MAX_FULL_PATH_LEN];
    __s32   open_cnt;   //引用计数
    __s32   npl_cur_index;
    ScanListCursor_t NplCursor; /* point to the npl cursor*/
    rat_play_mode_e play_mode;
    __s32   modify_flag;//修改标记，标记list里面的文件是否被删除或复制
} __rat_list_t;

#define RAT_ENTER_CRITICAL_SECTION \
    {\
        __u8 err;\
        esKRNL_SemPend(g_rat_sem, 0, &err);\
    }

#define RAT_LEAVE_CRITICAL_SECTION \
    {\
        esKRNL_SemPost(g_rat_sem);\
    }

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   open the rat process.
*
*  Parameters:
*   rat_root_type -     rat_type_t RAT_TF、RAT_USB
*   media_type  -   types of media file.
*   OnceCnt     -   read items number when read once.
*                   if zero, I will read all items one time.
*
*  Return value:
*   NULL        -   setup rat process failed.
*   others      -   handle to rat process.
****************************************************************************************************
*/
HRAT rat_open_ex(rat_type_t rat_root_type, rat_media_type_t media_type, __u32 OnceCnt);
HRAT rat_reopen(const char *pDir, rat_media_type_t media_type, __u32 OnceCnt);
/*
****************************************************************************************************
*
*
*
*  Description:
*               -   open the rat process.
*
*  Parameters:
*   pDir        -   pointer to directory(e:\ f:\....  ).
*   media_type  -   types of media file.
*   OnceCnt     -   read items number when read once.
*                   if zero, I will read all items one time.
*
*  Return value:
*   NULL        -   setup rat process failed.
*   others      -   handle to rat process.
****************************************************************************************************
*/
HRAT rat_open(const char *pDir, rat_media_type_t media_type, __u32 OnceCnt);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   move rat scan handle cursor position to first entry.
*
*  Parameters:
*   hRat        -   handle to rat information.
*
*  Return value:
*   EPDK_OK     -   move succeeded.
*   EPDK_FAIL   -   move failed.
****************************************************************************************************
*/
__s32 rat_move_cursor_to_first(HRAT hRat);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   move rat handle cursor position forward.
*
*  Parameters:
*   hRat        -   handle to rat information.
*
*  Return value:
*   __u32       -   count of cursor has actually move.
*
****************************************************************************************************
*/
__u32 rat_move_cursor_forward(HRAT hRat, __u32 cnt);


/*
*******************************************************************************
*********************
*
*
*
*  Description:
*               -   move rat handle cursor backward
*
*  Parameters:
*   hRat        -   handle to rat information.
*
*  Return value:
*   __u32       -   count of cursor has actually move.
*
*******************************************************************************
*********************
*/
__u32 rat_move_cursor_backward(HRAT hRat, __u32 cnt);


/*
****************************************************************************************************
*
*
*
*  Description:
*               -   check npl handle cursor position pointing after the end entry.
*
*  Parameters:
*   hRat        -   handle to npl information.
*
*  Return value:
*   EPDK_TRUE   -   cursor position pointing the end entry.
*   EPDK_FALSE  -   cursor position not pointing at the end entry.
****************************************************************************************************
*/
__bool rat_is_cursor_end(HRAT hRat);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   free npl handle.
*
*  Parameters:
*   hRat        -   handle to npl information.
*
*  Return value:
*               NONE
****************************************************************************************************
*/
__s32 rat_close(HRAT hRat);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get current npled file entry.
*
*  Parameters:
*   hRat        -   handle to npl information.
*   entry       -   to store entry info
*  Return value:
*   EPDK_OK         -   get current entry ok.
*   EPDK_FAIL       -   get current entry failed.
****************************************************************************************************
*/
__s32 rat_get_cur_item_info(HRAT hRat, rat_entry_t *entry);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get current item info by index.
*
*  Parameters:
*   hRat        -   handle to rat.
*   index       -   zero base index
*   entry       -   to store entry info
*  Return value:
*   EPDK_OK             -   current item info
*   EPDK_FAIL           -   no such item
*   others      -
****************************************************************************************************
*/
__s32 rat_get_item_info_by_index(HRAT hRat, __s32 index, rat_entry_t *entry);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   set current file to be play.
*
*  Parameters:
*   hRat        -   handle to rat.
*   file        -   full file path
*
*  Return value:
*   EPDK_FAIL           -   no such file
*   EPDK_TRUE           -   ok
*   others      -
****************************************************************************************************
*/
__s32 rat_set_file_for_play(HRAT hRat, char *file);

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get current item info by index.
*
*  Parameters:
*   hRat        -   handle to rat.
*   index       -   zero base index
*
*  Return value:
*   rat_entry_t*        -   current item info
*   NULL                -   no such item
*   others      -
****************************************************************************************************
*/
__u32 rat_get_cur_scan_cnt(HRAT hRat);

__s32 rat_init(void);

__s32 rat_deinit(void);

//clear rat list and npl by root
__s32 rat_clear_all_list_by_root(void *arg);

//clear all rat list and npl
__s32 rat_clear_all_list(void);

rat_media_type_t rat_get_file_type(const char *file_name);

__s32 rat_set_modify_flag(rat_media_type_t media_type, __s32 flag);

__s32 rat_set_modify_flag_all(__s32 flag);

__s32 rat_set_search_cb(prat_search_cb cb, void *arg);


#endif//__RAT_H__
