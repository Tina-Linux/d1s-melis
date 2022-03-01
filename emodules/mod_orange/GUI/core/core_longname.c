/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    core_longname.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-11-24
*
*Description :  long string move implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2009-11-24         1.1.0          Create File
*
*******************************************************************************************************************
*/
#include "core_i.h"
#include <kconfig.h>

#define MAX_STRING_LEN          768
#define MAX_NR_SHOW             16

#define LONGSTRING_MOVE       1
#define LONGSTRING_STOP       0
#define LONGSTRING_REQSTOP    2

/*move file name of focus item if stop scan*/
typedef struct
{
    char                  name[MAX_STRING_LEN];
    __epdk_charset_enm_e  encode_id;
    __s32                 move_step;
    RECT                  region;
    __u32                 align;
    GUI_FONT              *pFont;

    __u32                 enocde_align_cnt;
    __u32                 enocde_align_group[MAX_STRING_LEN];

    SIZE                  show_size;
    GUI_COLOR             color;
    void                  *bk_bmp;
    __pos_t               bmp_pos;
    GUI_COLOR             fontColor;

    __bool                left_move;
    __bool                status;
    __bool                alpha_en;

    GUI_MEMDEV_Handle     hmem;
    H_LYR                 hlayer;
} __string_show_info_t;

static __u32                 string_show_task    = 0;
static __bool               str_Inited          = ORANGE_FALSE;
static  pthread_mutex_t     lstrLock            = NULL;
static  sem_t               run_sem             = NULL;

#define LONGSTR_LOCK()   pthread_mutex_lock(lstrLock)
#define LONGSTR_UNLOCK() pthread_mutex_unlock(lstrLock)

static  __string_show_info_t   *gshow_info[MAX_NR_SHOW];
/*
**********************************************************************************************************************
*                                               g_get_string_piexl_rect
*
* author:
*
* date:             2009-11-24
*
* Description:      g get string piexl rect
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void get_string_piexl_rect(__string_show_info_t    *show_info)
{
    int         len, i;
    __u16       uni;
    __u16       CharWidth;
    __s32       char_size;
    i = 0;
    len = eLIBs_strlen(show_info->name);
    show_info->show_size.width = 0;
    show_info->show_size.height = GUI_GetFontDistY();

    while (i < len)
    {
        char_size = esCHS_Char2Uni(show_info->encode_id, (__u8 *)show_info->name + i, len - i, &uni);

        if (char_size <= 0)
        {
            break;
        }

        CharWidth                   = GUI_GetCharDistX(uni);
        show_info->show_size.width  += CharWidth;
        i                           += char_size;
    }

    return;
}

/*
**********************************************************************************************************************
*                                               g_get_string_encode_group
*
* author:
*
* date:             2009-11-24
*
* Description:      g get string encode group
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void get_string_encode_group(__string_show_info_t *show_info)
{
    __u32 i = 0;
    __u32 total_cnt = 0;
    __u32 cnt;
    __s32 char_size;
    __u32 len;
    __u16 uni;
    cnt = 0;
    len = eLIBs_strlen(show_info->name);

    while (i < len)
    {
        char_size = esCHS_Char2Uni(show_info->encode_id, (__u8 *)show_info->name + i, len - i, &uni);

        if (char_size <= 0)
        {
            break;
        }

        show_info->enocde_align_group[cnt++] = total_cnt;
        total_cnt += char_size;
        i += char_size;
    }

    show_info->enocde_align_group[cnt] = total_cnt;
    show_info->enocde_align_cnt = cnt;
}

/*
**********************************************************************************************************************
*                                               local_get_string_piexl_rect
*
* author:
*
* date:             2009-11-24
*
* Description:      local get string piexl rect
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void local_get_string_piexl_rect(const char *str, __string_show_info_t *show_info, SIZE *size)
{
    int len, i;
    __u16 uni;
    __u16 CharWidth;
    __s32 char_size;
    i = 0;
    size->width = 0;
    len = eLIBs_strlen(str);
    size->height = GUI_GetFontDistY();

    while (i < len)
    {
        char_size = esCHS_Char2Uni(show_info->encode_id, (__u8 *)str + i, len - i, &uni);

        if (char_size <= 0)
        {
            break;
        }

        CharWidth = GUI_GetCharDistX(uni);
        size->width += CharWidth;
        i += char_size;
    }

    return;
}

/*
**********************************************************************************************************************
*                                               local_get_string_encode_group
*
* author:
*
* date:             2009-11-24
*
* Description:      local get string encode group
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void local_get_string_encode_group(const char *str, __string_show_info_t *show_info, __u32 *encode_cnt, __u32 *encode_group)
{
    __u32 i = 0;
    __u32 total_cnt = 0;
    __u32 cnt;
    __s32 char_size;
    __u32 len;
    __u16 uni;
    cnt = 0;
    len = eLIBs_strlen(str);

    while (i < len)
    {
        char_size = esCHS_Char2Uni(show_info->encode_id, (__u8 *)str + i, len - i, &uni);

        if (char_size <= 0)
        {
            break;
        }

        encode_group[cnt++] = total_cnt;
        total_cnt += char_size;
        i += char_size;
    }

    encode_group[cnt] = total_cnt;
    *encode_cnt = cnt;
}


/*
**********************************************************************************************************************
*                                               GUI_LongStringMove
*
* author:
*
* date:             2009-11-24
*
* Description:      gui longstringmove
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_LongStringMove(__string_show_info_t    *show_info)
{
    if (!show_info->hmem)
    {
        ORANGE_WRN("input parameter error!");
        return ORANGE_FAIL;
    }

    /*all can see*/
    if (show_info->show_size.width < show_info->region.width)
    {
        return ORANGE_OK;
    }

    /*clear screen*/
    GUI_LyrWinSel(show_info->hlayer);
    GUI_MEMDEV_Select(show_info->hmem);
    GUI_SetBkColor(show_info->color);
#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined (CONFIG_SOC_SUN20IW1)
    GUI_ClearRect(show_info->region.x, show_info->region.y, show_info->region.x + show_info->region.width, show_info->region.y + show_info->region.height);
    __wrn("%d %d %d %d", show_info->region.x, show_info->region.y, show_info->region.x + show_info->region.width, show_info->region.y + show_info->region.height);

    if (show_info->bk_bmp != NULL)
    {
        __wrn("%d %d", show_info->bmp_pos.x, show_info->bmp_pos.y);
        GUI_BMP_Draw(show_info->bk_bmp, show_info->bmp_pos.x, show_info->bmp_pos.y);
    }

#elif defined CONFIG_SOC_SUN3IW1
    GUI_ClearRect(show_info->region.x, show_info->region.y,
                  show_info->region.x + show_info->region.width - 1,
                  show_info->region.y + show_info->region.height - 1);

    if (show_info->bk_bmp)
    {
        GUI_BMP_Draw(show_info->bk_bmp, show_info->region.x + show_info->bmp_pos.x,
                     show_info->region.y + show_info->bmp_pos.y);
    }
#endif

    if (show_info->alpha_en)
    {
        GUI_OpenAlphaBlend();
    }

    GUI_CharSetToEncode(show_info->encode_id);
    GUI_SetFont(show_info->pFont);
    GUI_SetColor(show_info->fontColor);

    if (show_info->align)
    {
        GUI_RECT  rect;
        rect.x0  = show_info->region.x;
        rect.y0  = show_info->region.y;
        rect.x1  = show_info->region.x + show_info->region.width;
        rect.y1  = show_info->region.y + show_info->region.height;
        GUI_DispStringInRect(show_info->name + show_info->enocde_align_group[show_info->move_step], &rect, show_info->align);
    }
    else
    {
        GUI_DispStringAt(show_info->name + show_info->enocde_align_group[show_info->move_step], show_info->region.x, show_info->region.y);
    }

    GUI_MEMDEV_CopyToLCD(show_info->hmem);
    GUI_MEMDEV_Select(0);

    if (show_info->alpha_en)
    {
        GUI_CloseAlphaBlend();
    }

    GUI_LyrWinFlushFB(show_info->hlayer);

    if (show_info->left_move)
    {
        SIZE    show_size;
        show_info->move_step++;
        local_get_string_piexl_rect(show_info->name + show_info->enocde_align_group[show_info->move_step], show_info,
                                    &show_size);

        if (show_size.width < show_info->region.width)
        {
            show_info->left_move = 0;
        }
    }
    else
    {
        show_info->move_step--;

        if (show_info->move_step < 0)
        {
            show_info->move_step = 0;
            show_info->left_move = 1;
        }
    }

    return  ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LongString_Task
*
* author:
*
* date:             2009-11-24
*
* Description:      gui longstring task
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_LongString_Task(void *arg)
{
    __u32 i;
    __bool move_flags = ORANGE_FALSE;

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            break;
        }

        orange_Dalay(40);
        LONGSTR_LOCK();

        for (i = 0; i < MAX_NR_SHOW; i++)
        {
            if (gshow_info[i])
            {
                if (gshow_info[i]->status == LONGSTRING_MOVE)
                {
                    move_flags = ORANGE_TRUE;
                    break;
                }
            }
        }

        LONGSTR_UNLOCK();

        if (i == MAX_NR_SHOW)
        {
            move_flags = ORANGE_FALSE;
        }

        if (move_flags == ORANGE_FALSE)
        {
            /*need not move*/
            sem_wait(&run_sem);
        }

        if (GUI_LockAccept() == EPDK_OK)
        {
            LONGSTR_LOCK();

            for (i = 0; i < MAX_NR_SHOW; i++)
            {
                if (gshow_info[i] && gshow_info[i]->status == LONGSTRING_MOVE)
                {
                    GUI_LongStringMove(gshow_info[i]);
                    orange_Dalay(10);
                }
            }

            LONGSTR_UNLOCK();
            GUI_Unlock();
        }
    }

    esKRNL_TDel(EXEC_prioself);
    return;
}

/*
**********************************************************************************************************************
*                                               GUI_LongStringStart
*
* author:
*
* date:             2009-11-24
*
* Description:      gui longstringstart
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_LongStringStart(__hdle hString)
{
    __string_show_info_t        *show_info;

    if (!hString)
    {
        ORANGE_WRN("input parameter error!");
        return ORANGE_FAIL;
    }

    show_info  = (__string_show_info_t *)hString;

    if (show_info->hmem == 0)
    {
        GUI_LyrWinSel(show_info->hlayer);
        show_info->hmem = GUI_MEMDEV_Create(show_info->region.x, show_info->region.y, show_info->region.width, show_info->region.height);

        if (!show_info->hmem)
        {
            ORANGE_WRN("create show info mem device fail");
            return  ORANGE_FAIL;
        }
    }

    /*all can see*/
    if (show_info->show_size.width < show_info->region.width)
    {
        return ORANGE_OK;
    }

    LONGSTR_LOCK();
    show_info->status = LONGSTRING_MOVE;
    LONGSTR_UNLOCK();

    if (run_sem)
    {
        int   sem_value;
        sem_getvalue(&run_sem, &sem_value);

        if (sem_value <= 0)
        {
            sem_post(&run_sem);
        }
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LongStringReset
*
* author:
*
* date:             2009-11-25
*
* Description:      gui longstringreset
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_LongStringReset(__hdle hString, __show_info_move_t *str_move)
{
    __string_show_info_t        *show_info;

    if ((!hString) || (!str_move))
    {
        ORANGE_WRN("input parameter error!");
        return ORANGE_FAIL;
    }

    show_info  = (__string_show_info_t *)hString;
    show_info->hlayer           = str_move->hlayer;
    show_info->region.x         = str_move->region.x;
    show_info->region.y         = str_move->region.y;
    show_info->region.width     = str_move->region.width;
    show_info->region.height    = str_move->region.height;
    show_info->pFont            = str_move->pFont;
    show_info->encode_id        = str_move->encode_id;
    show_info->move_step        = 0;
    show_info->left_move        = 1;
    show_info->color            = str_move->bkColor;
    show_info->fontColor        = str_move->fontColor;
    show_info->align            = str_move->align;
    show_info->bk_bmp           = str_move->bmp;
    show_info->bmp_pos.x        = str_move->bmp_pos.x;
    show_info->bmp_pos.y        = str_move->bmp_pos.y;
    show_info->alpha_en         = str_move->alpha_en;
    GUI_CharSetToEncode(show_info->encode_id);
    GUI_SetFont(show_info->pFont);
    eLIBs_strcpy(show_info->name, str_move->string);
    get_string_piexl_rect(show_info);
    get_string_encode_group(show_info);
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LongStringStop
*
* author:
*
* date:             2009-11-24
*
* Description:      gui longstringstop
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_LongStringStop(__hdle hString)
{
    __string_show_info_t    *show_info;

    if (!hString)
    {
        ORANGE_WRN("input parameter error!");
        return ORANGE_FAIL;
    }

    show_info  = (__string_show_info_t *)hString;

    if (show_info->hmem)
    {
        if (show_info->hlayer)
        {
            GUI_LyrWinSel(show_info->hlayer);
            GUI_MEMDEV_Delete(show_info->hmem);
        }

        show_info->hmem = 0;
    }

    /*all can see*/
    if (show_info->show_size.width < show_info->region.width)
    {
        return ORANGE_OK;
    }

    LONGSTR_LOCK();
    /*等待move结束之后，然后返回*/
    show_info->status = LONGSTRING_STOP;
    LONGSTR_UNLOCK();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LongStringInit
*
* author:
*
* date:             2009-11-25
*
* Description:      系统初始化时调用，为了节省资源，并不马上分配资源，仅在有实例时才分配资源
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void  GUI_LongStringInit(void)
{
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_LongStringMalloc
*
* author:
*
* date:             2009-11-25
*
* Description:      分配实质资源，尽在有LongString实例创建时使用
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool  GUI_LongStringMalloc(void)
{
    __u32       i;
    pthread_mutex_init(&lstrLock, (void *)1);
    sem_init(&run_sem, 0, 0);

    if (run_sem == NULL)
    {
        ORANGE_WRN("create longstring semphore failed!");
        return ORANGE_FALSE;
    }

    for (i = 0; i < MAX_NR_SHOW; i++)
    {
        gshow_info[i] = NULL;
    }

    if (string_show_task == 0)
    {
        /*create thread for show*/
        string_show_task        = esKRNL_TCreate(GUI_LongString_Task, (void *)0, 0x8000, KRNL_priolevel5);

        if (string_show_task     == 0)
        {
            ORANGE_WRN("create breath thread fail");
            goto error;
        }
    }

    str_Inited = ORANGE_TRUE;
    return ORANGE_TRUE;
error:

    if (run_sem)
    {
        sem_destroy(&run_sem);
        run_sem = NULL;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_LongStringExit
*
* author:
*
* date:             2009-11-25
*
* Description:      gui longstringexit
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void  GUI_LongStringExit(void)
{
    __u32       i;

    for (i = 0; i < MAX_NR_SHOW; i++)
    {
        if (gshow_info[i] != NULL)
        {
            orange_mfree(gshow_info[i]);
        }

        gshow_info[i] = NULL;
    }

    if (string_show_task)
    {
        /*kill breath thread*/
        while (1)
        {
            if (esKRNL_TDelReq(string_show_task) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            if (run_sem)
            {
                int   sem_value;
                sem_getvalue(&run_sem, &sem_value);

                if (sem_value <= 0)
                {
                    sem_post(&run_sem);
                }
            }

            orange_Dalay(3);
        }
    }

    if (run_sem)
    {
        sem_destroy(&run_sem);
    }

    if (lstrLock)
    {
        pthread_mutex_destroy(lstrLock);
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LongNameInit
*
* author:
*
* date:             2009-11-24
*
* Description:      gui longnameinit
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__hdle GUI_LongStringCreate(__show_info_move_t *str_move)
{
    __u32                    i;
    __u32                    slot = 0;
    __string_show_info_t    *show_info = NULL;

    if (!str_move)
    {
        ORANGE_WRN("input parameter error!");
        return NULL;
    }

    if (str_Inited == ORANGE_FALSE)
    {
        /*分配资源给longstring对象*/
        str_Inited = GUI_LongStringMalloc();

        if (str_Inited == ORANGE_FALSE)
        {
            ORANGE_WRN("create longstring fail!");
            return NULL;
        }
    }

    LONGSTR_LOCK();

    for (i = 0; i < MAX_NR_SHOW; i++)
    {
        if (gshow_info[i] == NULL)
        {
            slot = i;
            break;
        }
    }

    if (slot == MAX_NR_SHOW)
    {
        LONGSTR_UNLOCK();
        return NULL;
    }

    show_info = (__string_show_info_t *)orange_malloc(sizeof(__string_show_info_t));

    if (show_info == NULL)
    {
        ORANGE_WRN("malloc show info failed!");
        LONGSTR_UNLOCK();
        return  NULL;
    }

    orange_memset(show_info, 0, sizeof(__string_show_info_t));
    show_info->hlayer           = str_move->hlayer;
    show_info->region.x         = str_move->region.x;
    show_info->region.y         = str_move->region.y;
    show_info->region.width     = str_move->region.width;
    show_info->region.height    = str_move->region.height;
    show_info->pFont            = str_move->pFont;
    show_info->encode_id        = str_move->encode_id;
    show_info->move_step        = 0;
    show_info->left_move        = 1;
    show_info->color            = str_move->bkColor;
    show_info->fontColor        = str_move->fontColor;
    show_info->status           = LONGSTRING_STOP;
    show_info->align            = str_move->align;
    show_info->hmem             = 0;
    show_info->bk_bmp           = str_move->bmp;
    show_info->bmp_pos.x        = str_move->bmp_pos.x;
    show_info->bmp_pos.y        = str_move->bmp_pos.y;
    show_info->alpha_en         = str_move->alpha_en;
    eLIBs_strcpy(show_info->name, str_move->string);
    GUI_CharSetToEncode(show_info->encode_id);
    GUI_SetFont(show_info->pFont);
    get_string_piexl_rect(show_info);
    get_string_encode_group(show_info);

    /*all can see*/
    if (show_info->show_size.width < show_info->region.width)
    {
        gshow_info[slot]    = show_info;
        LONGSTR_UNLOCK();
        return GUI_P2Handle(show_info);
    }

    gshow_info[slot] = show_info;
    LONGSTR_UNLOCK();
    return GUI_P2Handle(show_info);
}

/*
**********************************************************************************************************************
*                                               GUI_LongNameExit
*
* author:
*
* date:             2009-11-24
*
* Description:      gui longnameexit
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_LongStringDelete(__hdle hString)
{
    __u32                    i;
    __string_show_info_t    *show_info;

    if (!hString)
    {
        ORANGE_WRN("input parameter error!");
        return ORANGE_FAIL;
    }

    /*先停止字符串滚动*/
    __msg("before GUI_LongStringStop");
    GUI_LongStringStop(hString);
    __msg("after GUI_LongStringStop");
    show_info  = (__string_show_info_t *)hString;
    LONGSTR_LOCK();

    for (i = 0; i < MAX_NR_SHOW; i++)
    {
        if (gshow_info[i] && gshow_info[i] == show_info)
        {
            /*all can see*/
            if (show_info->show_size.width < show_info->region.width)
            {
                gshow_info[i] = NULL;
                orange_mfree((void *)show_info);
                LONGSTR_UNLOCK();
                return ORANGE_OK;
            }

            gshow_info[i] = NULL;
            orange_mfree((void *)show_info);
            LONGSTR_UNLOCK();
            return ORANGE_OK;
        }
    }

    LONGSTR_UNLOCK();
    return ORANGE_OK;
}


