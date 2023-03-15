/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
