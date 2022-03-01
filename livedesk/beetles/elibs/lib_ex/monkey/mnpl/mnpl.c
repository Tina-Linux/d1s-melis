/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                             All Rights Reserved
*
* File      : npl.c
* By        : Andy.zhang
* Func      : now play list
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "monkey_i.h"
#include "mnpl.h"

#define ORCHID_FILE_PATH                "b:\\module\\orchid"

struct tag_MNPL
{
    ES_FILE *hOrchid;
    HNPL     hNpl;
};


MNPL   *mnpl_create(__s32 media_type)
{
    MNPL *npl;
    orc_arg_npl_t  arg;
    npl = (MNPL *)g_malloc(sizeof(struct tag_MNPL));

    if (!npl)
    {
        __err(" MNPL malloc error \n");
        return NULL;
    }

    npl->hOrchid = g_fopen(ORCHID_FILE_PATH, "r+");

    if (NULL == npl->hOrchid)
    {
        __err(" orchid open error \n");
        return NULL;
    }

    arg.media_type = media_type;
    arg.work_mode  = ORC_NPL_WORK_MODE_MANAGE;
    npl->hNpl     = g_fioctrl(npl->hOrchid, ORC_CMD_CONSTRUCT_NOW_PLAYLIST, 0, (void *)&arg);

    if (NULL == npl->hNpl)
    {
        __err("Error in creating now play list!\n");
        return NULL;
    }

    return npl;
}

__s32   mnpl_delete(MNPL *npl)
{
    if (NULL != npl->hNpl)
    {
        g_fioctrl(npl->hOrchid, ORC_CMD_DESTROY_NOW_PLAYLIST, 0, (void *)npl->hNpl);
    }

    if (NULL != npl->hOrchid)
    {
        g_fclose(npl->hOrchid);
    }

    g_free(npl);
    return 0;
}


__s32   mnpl_addItem(MNPL *npl, const char *file)
{
    __s32  ret;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_ADD_ITEM_TO_NPL, (__s32)npl->hNpl, (void *)file);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in add file %s to npl.\n", file);
        return -1;
    }

    return 0;
}

__s32   mnpl_deleteItem(MNPL *npl, const char *file)
{
    __s32  ret;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_DELETE_ITEM_FROM_NPL, (__s32)npl->hNpl, (void *)file);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in deleting file %s from npl.\n", file);
        return -1;
    }

    return 0;
}

__s32   mnpl_clear(MNPL *npl)
{
    __s32  ret;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_CLEAR_NPL, 0, (void *)npl->hNpl);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in clearing the npl.\n");
        return -1;
    }

    return 0;
}

__s32   mnpl_playingSeek(MNPL *npl,  __s32 origin, __s32 offset)
{
    __s32           ret;
    orc_arg_seek_t  seek_arg;
    seek_arg.hnpl   = npl->hNpl;
    seek_arg.offset = offset;
    seek_arg.origin = origin;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_PLAYING_SEEK, 0, &seek_arg);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in seeking playing pointer.\n");
        return -1;
    }

    return 0;
}

__s32   mnpl_getNext(MNPL *npl)
{
    __s32  ret;
    __s32  index;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_PLAYING_AUTO_GET_NEXT, (__s32)npl->hNpl, &index);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in getting next item.\n");
        return -1;
    }

    return index;
}

__s32   mnpl_getCur(MNPL *npl)
{
    __s32  ret;
    __s32  index;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_PLAYING_GET_CUR, (__s32)npl->hNpl, &index);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in getting next item.\n");
        return -1;
    }

    return index;
}

__s32   mnpl_fileToIndex(MNPL *npl, const char *file)
{
    __u32  index;
    index = g_fioctrl(npl->hOrchid, ORC_CMD_FILE_TO_INDEX_IN_NPL, (__s32)npl->hNpl, (void *)file);

    if (index == (__u32) - 1)
    {
        return -1;
    }
    else
    {
        return index;
    }
}

__s32   mnpl_indexToFile(MNPL *npl, __u32 index, char *file)
{
    __s32  ret;
    orc_arg_index_to_file_t  arg;
    arg.hnpl  = npl->hNpl;
    arg.index = index;
    arg.file  = file;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_INDEX_TO_FILE_IN_NPL, 0, (void *)&arg);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in converting index to file.\n");
        return -1;
    }

    return 0;
}

__s32   mnpl_invalidItem(MNPL *npl, __u32 index)
{
    __s32  ret;
    ret = g_fioctrl(npl->hOrchid, ORC_CMD_INVALIDATE_ITEM_IN_NPL, (__s32)npl->hNpl, (void *)index);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in invalidate item %d.\n", index);
        return -1;
    }

    return 0;
}

__s32   mnpl_itemIsValid(MNPL *npl, __u32 index)
{
    return g_fioctrl(npl->hOrchid, ORC_CMD_IS_VALID_ITEM_IN_NPL, (__s32)npl->hNpl, (void *)index);
}

__s32   mnpl_getTotalCount(MNPL *npl)
{
    __s32  nr;
    nr = g_fioctrl(npl->hOrchid, ORC_CMD_GET_TOTAL_ITEM_COUNT_OF_NPL, 0, (void *)npl->hNpl);

    if (nr == -1)
    {
        return -1;
    }
    else
    {
        return nr;
    }
}

__s32   mnpl_getValidCount(MNPL *npl)
{
    __s32  nr;
    nr = g_fioctrl(npl->hOrchid, ORC_CMD_GET_VALID_ITEM_COUNT_OF_NPL, 0, (void *)npl->hNpl);

    if (nr == -1)
    {
        return -1;
    }
    else
    {
        return nr;
    }
}

