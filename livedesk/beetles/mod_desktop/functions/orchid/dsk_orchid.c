/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dsk_orchid.c
* By        : Andy.zhang
* Func      : orchid op
* Version   : v1.0
* ============================================================================================================
* 2010-1-8 9:46:40  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "functions_i.h"

#define ORCHID_DISK                     "z:\\"
#define ORCHID_FILE_PATH                "b:\\module\\orchid"

__s32 dsk_orchid_createDisk(void)
{
    __s32  ret = 0;
    ret = eLIBs_IsPartFormated(ORCHID_DISK);

    if (ret == ELIBS_PART_FMT_NFORMATED)
    {
        ret = eLIBs_format(ORCHID_DISK, "fat", 0);
    }
    else if (ret == ELIBS_PART_FMT_FORMATED)
    {
        ret = EPDK_OK;
    }
    else
    {
        ret = EPDK_FAIL;
    }

    return ret;
}

static __s32 dsk_orchid_rebuild(void)
{
    __mp  *horc    = 0;
    /* create a handle of orchid */
    
#if 0 /*langaojie temp test*/ //remove wrn  
    horc = eLIBs_fopen(ORCHID_FILE_PATH, "r+");
#endif
    if (horc == 0)
    {
        __wrn("Error in opening a handle of orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    /* 重建orchid 数据库 */
    if (esMODS_MIoctrl(horc, ORC_CMD_REBUILD_ORCHID, 0, 0))
    {
        __wrn("Error in updating orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }
#if 0 /*langaojie temp test*/ //remove wrn  

    eLIBs_fclose(horc);
#endif
    horc = 0;
    return EPDK_OK;
error:

    if (horc != 0)
    {
#if 0 /*langaojie temp test*/ //remove wrn  
    
        eLIBs_fclose(horc);
#endif
        horc = 0;
    }

    return EPDK_FAIL;
}

__s32 dsk_orchid_check(void)
{
    __mp  *horc    = 0;
    __bool valid;
    /* create a handle of orchid */
#if 0 /*langaojie temp test*/ //remove wrn  
    horc = eLIBs_fopen(ORCHID_FILE_PATH, "r+");
#endif
    if (horc == 0)
    {
        __wrn("Error in opening a handle of orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    /* orchid 是否有效 */
    valid = esMODS_MIoctrl(horc, ORC_CMD_ORCHID_IS_VALID, 0, 0);
    
#if 0 /*langaojie temp test*/ //remove wrn  
    eLIBs_fclose(horc);
#endif
    horc = 0;

    /* check orchid */
    if (!valid)
    {
        dsk_orchid_rebuild();
    }

    return EPDK_OK;
error:

    if (horc != 0)
    {
#if 0 /*langaojie temp test*/ //remove wrn  
        eLIBs_fclose(horc);
#endif
        horc = 0;
    }

    return EPDK_FAIL;
}


//__s32 dsk_orchid_notify_update(void)
//{
//  __gui_msg_t msg;
//
//  msg.id          = DSK_MSG_ORCHID_UPDATA_START;
//  msg.h_srcwin    = NULL;
//  msg.h_deswin    = GUI_WinGetHandFromName(APP_DESKTOP_MWIN);
//  msg.dwAddData1  = 0;
//  msg.dwAddData2  = 0;
//  msg.p_arg       = 0;
//
//  return GUI_SendNotifyMessage(&msg);
//}

__s32 dsk_orchid_update(void)
{
    __mp  *horc    = 0;
    /* create a handle of orchid */
    
#if 0 /*langaojie temp test*/ //remove wrn  
    horc = eLIBs_fopen(ORCHID_FILE_PATH, "r+");
#endif
    if (horc == 0)
    {
        __wrn("Error in opening a handle of orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    /* check valid */
    esMODS_MIoctrl(horc, ORC_CMD_ORCHID_IS_VALID, 0, 0);

    /* 刷新数据库 */
    if (esMODS_MIoctrl(horc, ORC_CMD_UPDATE_ORCHID, 0, 0))
    {
        __wrn("Error in updating orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

#if 0 /*langaojie temp test*/ //remove wrn  
     eLIBs_fclose(horc);
#endif
    horc = 0;
    return EPDK_OK;
error:

    if (horc != 0)
    {
#if 0 /*langaojie temp test*/ //remove wrn  
        eLIBs_fclose(horc);
#endif
        horc = 0;
    }

    return EPDK_FAIL;
}

__s32 dsk_orchid_save_npl(void)
{
    __s32    ret;
    __mp  *horc = 0;
    HNPL      hNpl;
    orc_arg_npl_t  arg;
#if 0 /*langaojie temp test*/ //remove wrn
    horc = eLIBs_fopen(ORCHID_FILE_PATH, "r+");
#endif
    if (horc == 0)
    {
        __wrn("Error in opening a handle of orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(horc, ORC_CMD_ORCHID_IS_VALID, 0, 0);
    arg.media_type = ORC_MEDIA_TYPE_AUDIO;
    arg.work_mode  = ORC_NPL_WORK_MODE_MANAGE;
    hNpl           = esMODS_MIoctrl(horc, ORC_CMD_CONSTRUCT_NOW_PLAYLIST, 0, (void *)&arg);

    if (0 == hNpl)
    {
        __err("Error in creating now play list!\n");
#if 0 /*langaojie temp test*/ //remove wrn
        eLIBs_fclose(horc);
#endif
        return EPDK_FAIL;
    }

    ret = esMODS_MIoctrl(horc, ORC_CMD_SAVE_NPL, 0, (void *)hNpl);
    esMODS_MIoctrl(horc, ORC_CMD_DESTROY_NOW_PLAYLIST, 0, (void *)hNpl);
#if 0 /*langaojie temp test*/ //remove wrn
    eLIBs_fclose(horc);
#endif
    return ret;
}

__s32 dsk_orchid_load_last_npl(void)
{
    __mp  *horc = 0;
    HNPL      hNpl;
    orc_arg_npl_t  arg;
    __s32     ret;
    __s32     total_npl;
    
#if 0 /*langaojie temp test*/ //remove wrn
    horc = eLIBs_fopen(ORCHID_FILE_PATH, "r+");
#endif
    if (horc == 0)
    {
        __wrn("Error in opening a handle of orchid! At line %u of file %s.\n", __LINE__, __FILE__);
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(horc, ORC_CMD_ORCHID_IS_VALID, 0, 0);
    arg.media_type = ORC_MEDIA_TYPE_AUDIO;
    arg.work_mode  = ORC_NPL_WORK_MODE_MANAGE;
    hNpl           = esMODS_MIoctrl(horc, ORC_CMD_CONSTRUCT_NOW_PLAYLIST, 0, (void *)&arg);

    if (0 == hNpl)
    {
        __err("Error in creating now play list!\n");
#if 0 /*langaojie temp test*/ //remove wrn
        eLIBs_fclose(horc);
#endif
        return EPDK_FAIL;
    }

    total_npl = esMODS_MIoctrl(horc, ORC_CMD_GET_TOTAL_ITEM_COUNT_OF_NPL, 0, (void *)hNpl);
    ret = EPDK_OK;

    if (total_npl == 0)
    {
        ret = esMODS_MIoctrl(horc, ORC_CMD_LOAD_LAST_NPL, 0, (void *)hNpl);
    }

    esMODS_MIoctrl(horc, ORC_CMD_DESTROY_NOW_PLAYLIST, 0, (void *)hNpl);
#if 0 /*langaojie temp test*/ //remove wrn
    eLIBs_fclose(horc);
#endif
    return ret;
}


