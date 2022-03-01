/*
************************************************************************************************************************
*                                                        SWAN
*                                                   the Video Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : fifo_db.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.03.31
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
* Gary.Wang      2009.03.31       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __fifo_db_c
#define  __fifo_db_c

#include <log.h>
#include "beetles_app.h"
#include "fifo_db.h"

#define DB_PATH                         "e:"
#define FIFO_DB_FILE_MAGIC              "FIFO_DB"
#define FIFO_DB_FILE_EXTS               ".fdb"
#define MAGIC_SIZE                      8
#define FIFO_DB_FILE_HEAD_SIZE          64

#define FIFO_DB_FILE_MAX_LEN            ( sizeof(DB_PATH) + 1 + FIFO_DB_TITLE_MAX_LEN + sizeof(FIFO_DB_FILE_EXTS) + 1 )

typedef struct _fifo_db_file_head_t
{
    char   magic[MAGIC_SIZE];
    __s32  max_content_num;
    __s32  content_num;
    __s32  content_size;
    __u8   reserved[FIFO_DB_FILE_HEAD_SIZE - MAGIC_SIZE - 4 - 4 - 4];
} fifo_db_file_head_t;                              // FIFO_DB_FILE_HEAD_SIZE

typedef struct _fifo_db_item_t
{
    char   content[FIFO_DB_ITEM_CONTENT_MAX_SIZE];
    char   key[FIFO_DB_KEY_MAX_LEN + 1];
} item_info_t;

static void    compose_fifo_db_file(const char *title, char *fifo_db_file);
static __s32   create_fifo_db_i(const char *title, __u32 item_max_nr, __u32 item_size);

static __bool has_init = 0;

static __krnl_event_t *g_fifo_sem = NULL;

#define FIFO_ENTER_CRITICAL_SECTION \
    {\
        __u8 err;\
        if(g_fifo_sem == NULL) \
            fifo_init(); \
        esKRNL_SemPend(g_fifo_sem, 0, &err);\
    }

#define FIFO_LEAVE_CRITICAL_SECTION \
    {\
        esKRNL_SemPost(g_fifo_sem);\
    }


__s32 fifo_init(void)
{
    if (1 == has_init)
    {
        return EPDK_OK;
    }

    if (0 == g_fifo_sem)
    {
        g_fifo_sem = esKRNL_SemCreate(1);
    }

    has_init = 1;
    return EPDK_OK;
}

__s32 fifo_deinit(void)
{
    __u8    err;

    if (0 == has_init)
    {
        return EPDK_OK;
    }

    if (g_fifo_sem)
    {
        esKRNL_SemDel(g_fifo_sem, OS_DEL_ALWAYS, &err);
        g_fifo_sem = NULL;
    }

    has_init = 0;
    return EPDK_OK;
}


__bool fifo_db_has_exist(const char *title)
{
    G_FILE  *fp = NULL;
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};

    if (g_strlen(title) > FIFO_DB_TITLE_MAX_LEN)
    {
        return EPDK_FALSE;
    }

    FIFO_ENTER_CRITICAL_SECTION
    compose_fifo_db_file(title, file);
    fp = g_fopen(file, "rb");

    if (fp == NULL)
    {
        FIFO_LEAVE_CRITICAL_SECTION
        return EPDK_FALSE;
    }
    else
    {
        fifo_db_file_head_t  head;
        /* check valid flag */
        g_fread(&head, 1, sizeof(head), fp);

        if (EPDK_YES != eLIBs_memcmp(FIFO_DB_FILE_MAGIC, (void *)(head.magic), MAGIC_SIZE))       // file is NOT valid
        {
            g_fclose(fp);
            fp = NULL;
            /* delete the file */
            g_remove(file);
            FIFO_LEAVE_CRITICAL_SECTION
            return EPDK_FALSE;
        }
        else
        {
            g_fclose(fp);
            fp = NULL;
            FIFO_LEAVE_CRITICAL_SECTION
            return EPDK_TRUE;
        }
    }
}



__s32  create_fifo_db(const char *title, __u32 item_max_nr, __u32 item_size)
{
    __s32 ret;
    FIFO_ENTER_CRITICAL_SECTION
    ret = create_fifo_db_i(title, item_max_nr, item_size);
    FIFO_LEAVE_CRITICAL_SECTION
    return ret;
}



__s32  delete_fifo_db(const char *title)
{
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};

    if (g_strlen(title) > FIFO_DB_TITLE_MAX_LEN)
    {
        return -1;
    }

    FIFO_ENTER_CRITICAL_SECTION
    compose_fifo_db_file(title, file);
    g_remove(file);
    FIFO_LEAVE_CRITICAL_SECTION
    return 0;
}



__s32  get_item_content_from_fifo_db(const char *title, const char *key, void *buf)
{
    G_FILE  *fp = NULL;
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};
    fifo_db_file_head_t   head;
    item_info_t  item_info;
    __s32  ret;
    __s32 i;

    if (g_strlen(title) > FIFO_DB_TITLE_MAX_LEN
        || g_strlen(key) > FIFO_DB_KEY_MAX_LEN)
    {
        return -1;
    }

    /* open fifo db file */
    compose_fifo_db_file(title, file);
    fp = g_fopen(file, "rb+");

    if (fp == NULL)
    {
        __wrn("Fail in opening fifo db file.");
        return -1;
    }

    FIFO_ENTER_CRITICAL_SECTION
    /* read file head */
    g_fread(&head, 1, sizeof(head), fp);

    if (EPDK_YES != eLIBs_memcmp(FIFO_DB_FILE_MAGIC, (void *)(head.magic), MAGIC_SIZE))
    {
        __wrn("file header incorrect...");
        goto del_file;
    }

    if (head.content_num <= 0)
    {
        __wrn("no item is in fifo db...");
        ret = -1;
        goto exit;
    }

    if (head.content_num > FIFO_DB_ITEM_MAX_NR)
    {
        __wrn("head.content_num > FIFO_DB_ITEM_MAX_NR");
        goto del_file;
    }

    if (head.content_size > sizeof(item_info.content))
    {
        __wrn("head.content_size > sizeof(item_info.content)");
        goto del_file;
    }

    for (i = 0 ; i < head.content_num ; i++)
    {
        g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
        g_fread(&item_info, 1, sizeof(item_info_t), fp);

        if (0 == g_strncmp(key, item_info.key, FIFO_DB_KEY_MAX_LEN))
        {
            if (buf)
            {
                eLIBs_memcpy(buf, item_info.content, head.content_size);
            }
            else
            {
                __wrn("input para is null...");
            }

            __wrn("item found,key=%s, head.content_num=%d", key, head.content_num);
            ret = 0;
            goto exit;
        }
    }

    __wrn("item not found to delete,key=%s, head.content_num=%d", key, head.content_num);
    ret = -1;
    goto exit;
exit:

    if (fp != NULL)
    {
        g_fseek(fp, 0, SEEK_SET);
        g_fwrite(&head, 1, sizeof(head), fp);
        g_fclose(fp);
        fp = NULL;
    }

    FIFO_LEAVE_CRITICAL_SECTION
    return ret;
del_file:

    if (fp != NULL)
    {
        g_fclose(fp);
        fp = NULL;
    }

    /* delete file */
    g_remove(file);
    __wrn("to delete db file %s.", file);
    FIFO_LEAVE_CRITICAL_SECTION
    return -1;
}



__s32  add_item_to_fifo_db(const char *title, const char *key, const void *buf)
{
    G_FILE  *fp = NULL;
    fifo_db_file_head_t   head;
    item_info_t  item_info;
    char file[FIFO_DB_KEY_MAX_LEN + 1] = {0};
    __s32  ret;
    __s32 i;
    __s32 item_exist;

    if (g_strlen(title) > FIFO_DB_TITLE_MAX_LEN
        || g_strlen(key) > FIFO_DB_KEY_MAX_LEN)
    {
        return -1;
    }

    /* open fifo db file */
    compose_fifo_db_file(title, file);
    fp = g_fopen(file, "rb+");

    if (fp == NULL)
    {
        __wrn("Fail in opening fifo db file.");
        return -1;
    }

    FIFO_ENTER_CRITICAL_SECTION
    /* read file head */
    g_fread(&head, 1, sizeof(head), fp);

    if (EPDK_YES != eLIBs_memcmp(FIFO_DB_FILE_MAGIC, head.magic, MAGIC_SIZE))
    {
        __wrn("file header incorrect...");
        goto del_file;
    }

    if (head.content_num > FIFO_DB_ITEM_MAX_NR)
    {
        __wrn("head.content_num > FIFO_DB_ITEM_MAX_NR");
        goto del_file;
    }

    if (head.max_content_num > FIFO_DB_ITEM_MAX_NR)
    {
        __wrn("head.max_content_num > FIFO_DB_ITEM_MAX_NR");
        goto del_file;
    }

    if (head.content_num > head.max_content_num)
    {
        __wrn("head.content_num > head.max_content_num");
        goto del_file;
    }

    if (NULL == buf)
    {
        __wrn("input para is null...");
        ret = -1;
        goto exit;
    }

    //查询是否已经存在
    item_exist = 0;

    for (i = 0 ; i < head.content_num ; i++)
    {
        g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
        g_fread(&item_info, 1, sizeof(item_info_t), fp);

        if (0 == g_strncmp(key, item_info.key, FIFO_DB_KEY_MAX_LEN))
        {
            //if(head.content_num > 1)//当前item移动到最后面
            {
                item_info_t item_tmp = {0};

                for (i++ ; i < head.content_num ; i++)
                {
                    g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
                    g_fread(&item_tmp, 1, sizeof(item_info_t), fp);
                    g_fseek(fp, sizeof(head) + (i - 1)*sizeof(item_info_t), SEEK_SET);
                    g_fwrite(&item_tmp, 1, sizeof(item_info_t), fp);
                    __wrn("i->i-1 i-1=%d", i - 1);
                }

                if (buf)
                {
                    eLIBs_memcpy(item_info.content, buf, head.content_size);
                }
                else
                {
                    __wrn("buf is null...");
                }

                g_fseek(fp, sizeof(head) + (head.content_num - 1)*sizeof(item_info_t), SEEK_SET);
                g_fwrite(&item_info, 1, sizeof(item_info_t), fp);
            }
            item_exist = 1;
        }
    }

    if (1 == item_exist)
    {
        __wrn("item alreadly exist, key=%s, head.content_num=%d", key, head.content_num);
        ret = 0;
        goto exit;
    }

    if (head.content_num >= head.max_content_num)
    {
        head.content_num = head.max_content_num;

        for (i = 1 ; i < head.content_num ; i++)
        {
            g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
            g_fread(&item_info, 1, sizeof(item_info_t), fp);
            g_fseek(fp, sizeof(head) + (i - 1)*sizeof(item_info_t), SEEK_SET);
            g_fwrite(&item_info, 1, sizeof(item_info_t), fp);
            __wrn("i->i-1 i-1=%d", i - 1);
        }

        head.content_num--;
        __wrn("delete last item, head.content_num=%d", head.content_num);
    }

    eLIBs_memset(&item_info, 0, sizeof(item_info_t));
    eLIBs_strcpy(item_info.key, key);

    if (buf)
    {
        eLIBs_memcpy(item_info.content, buf, head.content_size);
    }
    else
    {
        __wrn("buf is null...");
    }

    g_fseek(fp, sizeof(head) + head.content_num * sizeof(item_info_t), SEEK_SET);
    g_fwrite(&item_info, 1, sizeof(item_info_t), fp);
    head.content_num++;

    if (head.content_num > head.max_content_num)
    {
        head.content_num = head.max_content_num;
    }

    __wrn("item added, key=%s, head.content_num=%d", key, head.content_num);
    ret = 0;
    goto exit;
exit:

    if (fp != NULL)
    {
        g_fseek(fp, 0, SEEK_SET);
        g_fwrite(&head, 1, sizeof(head), fp);
        g_fclose(fp);
        fp = NULL;
    }

    FIFO_LEAVE_CRITICAL_SECTION
    return ret;
del_file:

    if (fp != NULL)
    {
        g_fclose(fp);
        fp = NULL;
    }

    /* delete file */
    g_remove(file);
    __wrn("to delete db file %s.", file);
    FIFO_LEAVE_CRITICAL_SECTION
    return -1;
}



__s32  delete_item_from_fifo_db(const char *title, const char *key)
{
    G_FILE  *fp = NULL;
    fifo_db_file_head_t   head;
    item_info_t  item_info;
    char file[FIFO_DB_KEY_MAX_LEN + 1] = {0};
    __s32  ret;
    __s32 i;

    if (g_strlen(title) > FIFO_DB_TITLE_MAX_LEN
        || g_strlen(key) > FIFO_DB_KEY_MAX_LEN)
    {
        return -1;
    }

    /* open fifo db file */
    compose_fifo_db_file(title, file);
    fp = g_fopen(file, "rb+");

    if (fp == NULL)
    {
        __wrn("Fail in opening fifo db file.");
        return -1;
    }

    FIFO_ENTER_CRITICAL_SECTION
    /* read file head */
    g_fread(&head, 1, sizeof(head), fp);

    if (EPDK_YES != eLIBs_memcmp(FIFO_DB_FILE_MAGIC, head.magic, MAGIC_SIZE))
    {
        __wrn("file header incorrect...");
        goto del_file;
    }

    if (head.content_num <= 0)
    {
        __wrn("no item is in fifo db...");
        ret = 0;
        goto exit;
    }

    if (head.content_num > FIFO_DB_ITEM_MAX_NR)
    {
        __wrn("head.content_num > FIFO_DB_ITEM_MAX_NR");
        goto del_file;
    }

    for (i = 0 ; i < head.content_num ; i++)
    {
        g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
        g_fread(&item_info, 1, sizeof(item_info_t), fp);

        if (0 == g_strncmp(key, item_info.key, FIFO_DB_KEY_MAX_LEN))
        {
            for (i++ ; i < head.content_num ; i++)
            {
                g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
                g_fread(&item_info, 1, sizeof(item_info_t), fp);
                g_fseek(fp, sizeof(head) + (i - 1)*sizeof(item_info_t), SEEK_SET);
                g_fwrite(&item_info, 1, sizeof(item_info_t), fp);
                __wrn("i->i-1 i-1=%d", i - 1);
            }

            eLIBs_memset(&item_info, 0, sizeof(item_info_t));

            for (; i < head.max_content_num; i++)
            {
                g_fseek(fp, sizeof(head) + i * sizeof(item_info_t), SEEK_SET);
                g_fwrite(&item_info, 1, sizeof(item_info_t), fp);
                __wrn("write i to empty item, i=%d", i);
            }

            head.content_num--;

            if (head.content_num < 0)
            {
                head.content_num = 0;
            }

            __wrn("item deleted,key=%s, head.content_num=%d", key, head.content_num);
            ret = 0;
            goto exit;
        }
    }

    __wrn("item not found to delete,key=%s, head.content_num=%d", key, head.content_num);
    ret = 0;
    goto exit;
exit:

    if (fp != NULL)
    {
        g_fseek(fp, 0, SEEK_SET);
        g_fwrite(&head, 1, sizeof(head), fp);
        g_fclose(fp);
        fp = NULL;
    }

    FIFO_LEAVE_CRITICAL_SECTION
    return ret;
del_file:

    if (fp != NULL)
    {
        g_fclose(fp);
        fp = NULL;
    }

    /* delete file */
    g_remove(file);
    __wrn("to delete db file %s.", file);
    FIFO_LEAVE_CRITICAL_SECTION
    return -1;
}

static void compose_fifo_db_file(const char *title, char *fifo_db_file)
{
    if (g_strlen(title) >= FIFO_DB_TITLE_MAX_LEN)
    {
        g_strcpy(fifo_db_file, DB_PATH);
        g_strcat(fifo_db_file, "\\");
        g_strncpy(fifo_db_file + g_strlen(fifo_db_file), title, FIFO_DB_TITLE_MAX_LEN);
        g_strcat(fifo_db_file, FIFO_DB_FILE_EXTS);
    }
    else
    {
        g_strcpy(fifo_db_file, DB_PATH);
        g_strcat(fifo_db_file, "\\");
        g_strcat(fifo_db_file, title);
        g_strcat(fifo_db_file, FIFO_DB_FILE_EXTS);
    }
}



static __s32 init_fifo_db_file(G_FILE *fp, const char *title, __u32 max_nr, __u32 content_size)
{
    /* write FILE head */
    {
        __s32 i;
        fifo_db_file_head_t head;
        item_info_t item_info = {0};

        if (max_nr > FIFO_DB_ITEM_MAX_NR)
        {
            __wrn("max_nr > FIFO_DB_ITEM_MAX_NR, set to FIFO_DB_ITEM_MAX_NR");
            max_nr = FIFO_DB_ITEM_MAX_NR;
        }

        if (content_size > FIFO_DB_ITEM_CONTENT_MAX_SIZE)
        {
            __wrn("content_size > FIFO_DB_ITEM_CONTENT_MAX_SIZE");
            return EPDK_FAIL;
        }

        g_memset(&head, 0, sizeof(head));
        g_strncpy(head.magic, FIFO_DB_FILE_MAGIC, MAGIC_SIZE);
        head.content_num = 0;
        head.content_size = content_size;
        head.max_content_num = max_nr;
        g_fwrite(&head, 1, sizeof(head), fp);

        //write empty content
        for (i = 0 ; i < head.max_content_num; i++)
        {
            g_fwrite(&item_info, 1, sizeof(item_info_t), fp);
        }
    }
    return 0;
}


static __s32  create_fifo_db_i(const char *title, __u32 item_max_nr, __u32 item_size)
{
    char   file[FIFO_DB_FILE_MAX_LEN + 1] = {0};
    G_FILE *fp = NULL;
    ES_DIR    *pdir = NULL;

    if (g_strlen(title) > FIFO_DB_TITLE_MAX_LEN)
    {
        return -1;
    }

    if (item_max_nr > FIFO_DB_ITEM_MAX_NR || item_size > FIFO_DB_ITEM_CONTENT_MAX_SIZE)
    {
        goto error;
    }

    /* check whether folder exist or not */
    pdir = g_opendir(DB_PATH);

    if (pdir == NULL)                             // folder not exist
    {
        g_mkdir(DB_PATH);               // create folder
    }
    else
    {
        g_closedir(pdir);
        pdir = NULL;
    }

    /* create fifo db file */
    compose_fifo_db_file(title, file);
    fp = g_fopen(file, "wb+");

    if (fp == NULL)
    {
        __wrn("Fail in creating fifo db file, file=%s", file);
        goto error;
    }

    /* init fifo db file */
    if (init_fifo_db_file(fp, title, item_max_nr, item_size))
    {
        goto error;
    }

    g_fclose(fp);
    fp = NULL;
    return 0;
error:

    if (fp != NULL)
    {
        g_fclose(fp);
        fp = NULL;
        /* delete invalid file */
        g_remove(file);
    }

    return -1;
}

#endif     //  ifndef __fifo_db_c

/* end of fifo_db.c */


