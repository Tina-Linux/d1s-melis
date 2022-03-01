/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : bookmark.c
* By      : Andy.zhang
* Version : V1.00
**************************************************************************************************************
*/

#include "bookmark.h"
#include "bmktable.h"

#define MAX_EBOOK_MARK      6

/* book mark 文件格式定义*/
#pragma pack(push, 1)       //128字节
typedef struct
{
    __u8  SecName[8];
    __u8  version;
    __u8  reserv1[3];
    __u32 bookmarknum;
    __u32 last_read_offset;
    __u32 book_mark_offset;
    __u32 item_size;
    __u8  reserv2[100];
} BmkHead;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct  //32字节
{
    __u32 exist;
    __u32 offset;
    __u8  reserv[24];
} BmkBookMark;
#pragma pack(pop)

typedef struct
{
    ES_FILE *fd;
    __u32 last_read_offset;
    __u32 book_mark_offset;
    __u32 item_size;
} BmkInfo;

static void bmk_create_file(ES_FILE *fd)
{
    BmkHead head;
    BmkBookMark mark;
    int i;
    eLIBs_memset(&head, 0, sizeof(head));
    eLIBs_strncpy((char *)head.SecName, ".bmkdate", 8);
    head.version          = 100;
    head.bookmarknum      = MAX_EBOOK_MARK;
    head.item_size        = sizeof(BmkBookMark);
    head.last_read_offset = sizeof(BmkHead);
    head.book_mark_offset = head.last_read_offset + head.item_size;
    eLIBs_fseek(fd, 0, ELIBS_SEEK_SET);
    eLIBs_fwrite(&head, sizeof(head), 1, fd);
    eLIBs_memset(&mark, 0, sizeof(mark));
    mark.exist       = EPDK_FALSE;
    mark.offset      = 0x55;

    for (i = 0; i < MAX_EBOOK_MARK; i++)
    {
        eLIBs_fwrite(&mark, sizeof(BmkBookMark), 1, fd);
    }
}

HBMK bmk_open(char *szAppFile, __u32 mode)
{
    BmkInfo *binfo;
    char file[256];
    binfo = (BmkInfo *)esMEMS_Malloc(0, sizeof(BmkInfo));

    if (binfo == NULL)
    {
        __err(" BmkInfo malloc error \n");
        return NULL;
    }

    eLIBs_memset(binfo, 0, sizeof(BmkInfo));
    file_2_bmk_name(szAppFile, file);
    binfo->fd = eLIBs_fopen(file, "r");

    if (binfo->fd == NULL)
    {
        binfo->fd = eLIBs_fopen(file, "wb+");
        binfo->last_read_offset = sizeof(BmkHead);
        binfo->book_mark_offset = sizeof(BmkHead) + sizeof(BmkBookMark);
        binfo->item_size        = sizeof(BmkBookMark);
        bmk_create_file(binfo->fd);
        return (HBMK)binfo;
    }

    eLIBs_fclose(binfo->fd);
    binfo->fd = eLIBs_fopen(file, "rb+");
    binfo->last_read_offset = sizeof(BmkHead);
    binfo->book_mark_offset = sizeof(BmkHead) + sizeof(BmkBookMark);
    binfo->item_size        = sizeof(BmkBookMark);
    //esMEMS_Mfree(0, file);
    return (HBMK)binfo;
}

void    bmk_set_offset(HBMK hBmk, int i, int offset)
{
    BmkBookMark item;
    int off;
    BmkInfo *info = (BmkInfo *)hBmk;
    off = info->book_mark_offset + info->item_size * i;
    eLIBs_memset(&item, 0, sizeof(BmkBookMark));
    item.exist  = EPDK_TRUE;
    item.offset = offset;
    eLIBs_fseek(info->fd, off, ELIBS_SEEK_SET);
    eLIBs_fwrite(&item, sizeof(BmkBookMark), 1, info->fd);
}

__bool  bmk_get_offset(HBMK hBmk, int i, int *p_offset)
{
    BmkBookMark item;
    int offset;
    BmkInfo *info = (BmkInfo *)hBmk;
    offset = info->book_mark_offset + info->item_size * i;
    eLIBs_memset(&item, 0, sizeof(BmkBookMark));
    eLIBs_fseek(info->fd, offset, ELIBS_SEEK_SET);
    eLIBs_fread(&item, sizeof(BmkBookMark), 1, info->fd);
    *p_offset = item.offset;
    return item.exist;
}

__bool  bmk_get_last_offset(HBMK hBmk, int *p_offset)
{
    BmkBookMark item;
    int off;
    BmkInfo *info = (BmkInfo *)hBmk;
    off = info->last_read_offset;
    eLIBs_memset(&item, 0, sizeof(BmkBookMark));
    eLIBs_fseek(info->fd, off, ELIBS_SEEK_SET);
    eLIBs_fread(&item, sizeof(BmkBookMark), 1, info->fd);
    *p_offset = item.offset;
    return item.exist;
}

void    bmk_set_last_offset(HBMK hBmk, int offset)
{
    BmkBookMark item;
    int off;
    BmkInfo *info = (BmkInfo *)hBmk;
    off = info->last_read_offset;
    eLIBs_memset(&item, 0, sizeof(BmkBookMark));
    item.exist  = EPDK_TRUE;
    item.offset = offset;
    eLIBs_fseek(info->fd, off, ELIBS_SEEK_SET);
    eLIBs_fwrite(&item, sizeof(BmkBookMark), 1, info->fd);
}

__s32   bmk_close(HBMK hBmk)
{
    BmkInfo *info = (BmkInfo *)hBmk;
    eLIBs_fclose(info->fd);
    esMEMS_Mfree(0, hBmk);
    return EPDK_TRUE;
}

