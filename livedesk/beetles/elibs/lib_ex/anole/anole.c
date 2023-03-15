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
#include "anole_i.h"
#include <kconfig.h>
#include <kapi.h>
#include <libc.h>
#include <log.h>

#define CHECK_PIC_ZOOM                      0
#define ANOLE_SYN_DELAY_TIME                50//
#define ANOLE_AUTO_PLAY_THREAD_DELAY_TIME   5
#define ANOLE_MOVE_BASE_DIST                32

#define trans_to_willow_switchmode()        ((anole_core.SwitchMode == ANOLE_FLAG_RANDOM_MODE) ? \
                                                (anole_core.switchmodearray[esKSRV_Random(100) % anole_core.arraynum]) : (anole_core.SwitchMode))

#define anole_error_proc(_cur_file_name, _cb_err_show, _cb_para)    if (_cb_err_show) {esKRNL_CallBack( _cb_err_show, _cb_para );}

__mp                        *mod_willow;
uint8_t                     mid_willow;
static __anole_core_t       anole_core;
static void                 *decoding_buf           = NULL;
static void                 *unsupported_buf        = NULL;
static __pCBK_t volatile    cb_show_pic_fail        = NULL;
static void *volatile       cb_ctx_show_pic_fail    = NULL;
//自动播放回调
static __pCBK_t volatile    cb_show_pic_next        = NULL;
static void *volatile       cb_ctx_show_pic_next    = NULL;
static HRATNPL volatile     anole_npl               = NULL;
static int32_t              LCD_WIDTH = 0;
static int32_t              LCD_HEIGHT = 0;
static __bool               g_app_photo_has_touch_move_flag = 0;
__krnl_event_t              *g_anole_sem = NULL;

#define ANOLE_ENTER_CRITICAL_SECTION                                            \
    {                                                                           \
        uint8_t     err;                                                        \
        if(g_anole_sem)                                                         \
        {                                                                       \
            __msg("**********ANOLE_ENTER_CRITICAL_SECTION*****************");   \
            esKRNL_SemPend(g_anole_sem, 0, &err);                               \
        }                                                                       \
    }

#define ANOLE_LEAVE_CRITICAL_SECTION                                            \
    {                                                                           \
        if(g_anole_sem)                                                         \
        {                                                                       \
            __msg("**********ANOLE_LEAVE_CRITICAL_SECTION*****************");   \
            esKRNL_SemPost(g_anole_sem);                                        \
        }                                                                       \
    }


int32_t anole_init_screen_size(int32_t *p_width, int32_t *p_height)
{
	__mp    *mp_display = NULL;

    /* open display driver */
	mp_display  = esKSRV_Get_Display_Hld();
	
    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        return EPDK_FAIL;
    }

    *p_width    = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH,    0, 0);
    *p_height   = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT,   0, 0);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               __wait_willow_ready
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void __wait_willow_ready(void)
{
    __u8 status;
    __s32 count;
    count = ANOLE_SYN_DELAY_TIME;
    /*check willow status*/
#ifndef SIM_PC_WIN

    while (1)
    {
        //__here__;
        status = esMODS_MIoctrl(mod_willow, WILLOW_CMD_QUERY_STATUS, 0, 0);

        //__here__;
        if (status != WILLOW_STATUS_BUSY)
        {
            break;
        }

        //esKRNL_TimeDly(10);
        esKRNL_TimeDly(1);
        count--;
        __msg("count=%d", count);
    }

#endif
}
/*
**********************************************************************************************************************
*                                               __wait_willow_finish
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static void __wait_willow_finish(void)
{
    __u8 status;
    __s32 count;
    count = ANOLE_SYN_DELAY_TIME;

    /*check willow status*/
    while (1)
    {
        //__here__;
        status = esMODS_MIoctrl(mod_willow, WILLOW_CMD_QUERY_STATUS, 0, 0);

        //__here__;
        if (status == WILLOW_STATUS_FINISH)
        {
            break;
        }

        //esKRNL_TimeDly(10);
        esKRNL_TimeDly(1);      ///by linyaoshu
        count--;
    }
}

/***************************************************************************************************
*Name        : create_npl
*Prototype   : __s32 create_npl( __s32 media_type )
*Arguments   : media_type     input. media type, such as audio, video and so on
*Return      : ==0    succeed
*              !=0    fail
*Description : create now playlist
*Other       :
***************************************************************************************************/
static __s32 __npl_create(rat_media_type_t media_type)
{
    if (anole_npl == NULL)
    {
        anole_npl = rat_npl_open(media_type);

        if (anole_npl == NULL)
        {
            __err("Error in creating now play list!");
            return -1;
        }
    }

    return 0;
}


/***************************************************************************************************
*Name        : delete_npl
*Prototype   : void  delete_npl( void )
*Arguments   : void
*Return      : void
*Description : delete now playlist
*Other       :
***************************************************************************************************/
static void  __npl_delete(void)
{
    if (anole_npl != NULL)
    {
        rat_npl_close(anole_npl);
        anole_npl = NULL;
    }
}

/***************************************************************************************************
*Name        : anole_npl_get_next
*Prototype   : __s32  anole_npl_get_next( void )
*Arguments   : void
*Return      : the index of the next file to be played in the now play list. if return -1, that means
*              no item to play in current "play mode" or some error occured.
*Description : get the index of the next file to be played in the now play list according to the
*              current "play mode" and the current item. this operation will adjust the playing
*              pointer to the next item.
*Other       :
***************************************************************************************************/
__s32  anole_npl_get_next(void)
{
    __s32  ret;
    __s32  index;
    index = rat_npl_get_next(anole_npl);
    return index;
}

/***************************************************************************************************
*Name        : anole_npl_get_prev
*Prototype   : __s32  anole_npl_get_prev( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. if return -1, that means fail
*Description : get the index of the item that the playing ponter points to.
*Other       :
***************************************************************************************************/
__s32  anole_npl_get_prev(void)
{
    __s32  ret;
    __s32  index;
    index = rat_npl_get_prev(anole_npl);
    return index;
}

/***************************************************************************************************
*Name        : anole_npl_get_cur
*Prototype   : __s32  anole_npl_get_prev( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. if return -1, that means fail
*Description : get the index of the item that the playing ponter points to.
*Other       :
***************************************************************************************************/
__s32  anole_npl_get_cur(void)
{
    __s32  ret;
    __s32  index;
    //__here__;
    index = rat_npl_get_cur(anole_npl);
    //__here__;
    return index;
}


/***************************************************************************************************
*Name        : anole_npl_file2index
*Prototype   : __s32  anole_npl_file2index( const char *file )
*Arguments   : file     input. full path.
*Return      : the index value of the file in the now play list. If return -1. that means fail.
*Description : get the index value of the file in the now play list.
*Other       :
***************************************************************************************************/
__s32  anole_npl_file2index(const char *file)
{
    __u32  index;
    index = rat_npl_file2index(anole_npl, file);
    return index;
}

/***************************************************************************************************
*Name        : anole_npl_get_total_count
*Prototype   : __s32  anole_npl_get_total_count( void )
*Arguments   :
*Return      : total count in npl
*Description :
*Other       :
***************************************************************************************************/
__s32 anole_npl_get_total_count(void)
{
    return rat_npl_get_total_count(anole_npl);
}


/***************************************************************************************************
*Name        : anole_npl_index2file
*Prototype   : __s32  anole_npl_index2file( __u32 index, char *file )
*Arguments   : index     input. the item index in the now play list.
*              file      output. the file corresponding to the index.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the file corresponding to its index in the now play list.
*Other       :
***************************************************************************************************/
__s32  anole_npl_index2file(__u32 index, char *file)
{
    __s32 ret;
    //__here__;
    ret = rat_npl_index2file(anole_npl, index, file);
    //__here__;
    return ret;
}

/***************************************************************************************************
*Name        : anole_npl_playing_seek
*Prototype   : __s32  anole_npl_playing_seek( __s32 index)
*Arguments   :
*Return      : seek npl
*Description :
*Other       :
***************************************************************************************************/
__s32 anole_npl_playing_seek(__s32 index)
{
    return rat_npl_set_cur(anole_npl, index);
}


/*
**********************************************************************************************************************
*                                               AutoPlay_ThreadDel
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32  anole_auto_play_thread_del(void)
{
    /* delete autoplay thread */
    if (anole_core.maintask != 0)
    {
        while (esKRNL_TDelReq(anole_core.maintask) != OS_TASK_NOT_EXIST)
        {
            //__here__
            esKRNL_TimeDly(5);
            //esKRNL_TimeDlyResume(anole_core.maintask);
        }

        anole_core.maintask = 0;
        __msg("\"loading\" thread has been deleted.");
    }

    return 0;
}

static __s32 __do_play_file(__willow_show_file_param_t *param)
{
    __msg("index = %d, file name = %s", param->img_no, param->filename);

    anole_core.scale_level = WILLOW_SCALER_BASE_RATIO;

    //set show img node
    __wait_willow_ready();
    //__here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHOW_IMG_FROM_FILE, 0, param);
    //__here__;
    __wait_willow_finish();
    //__here__;

    //check if the format is supported
    if (esMODS_MIoctrl(mod_willow, WILLOW_CMD_CHECK_IMG, 0, param) == EPDK_FAIL)
    {
        __msg("the file format is not supported!");
        __wait_willow_finish();
        //__here__;
        return EPDK_FAIL;
    }

    //__here__;
    //start decoding
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_DEC, 0, 0);
    //__here__;
    __wait_willow_finish();
    //__here__;
    //TestTimerReport(__FILE__, __LINE__);
    //start show
    __wait_willow_ready();
    //__here__;
    //TestTimerReport(__FILE__, __LINE__);

    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SW_SCALE, 0, 0);

    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_SHOW, 0, 0);
    //__here__;
    //TestTimerReport(__FILE__, __LINE__);
    __wait_willow_finish();
    //__here__;
    //TestTimerReport(__FILE__, __LINE__);
    return EPDK_OK;
}

#if CHECK_PIC_ZOOM
__bool anole_check_pic(__u32 index)
{
    __willow_show_file_param_t param;
    char filename[768];
    anole_npl_index2file(index, filename);
    param.img_no    = index;
    param.filename  = filename;
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHOW_IMG_FROM_FILE, 0, &param);
    __wait_willow_finish();

    //check if the format is supported
    if (esMODS_MIoctrl(mod_willow, WILLOW_CMD_CHECK_IMG, 0, &param) == EPDK_FAIL)
    {
        __wrn("%d :the file format is not supported!", index);
        __msg("the file format is not supported!");
        __wait_willow_finish();
        return EPDK_FALSE;
    }

    __wrn("%d :the file format is supported!", index);
    return EPDK_TRUE;
}
#endif

__s32 anole_play_file(char *filename)
{
    __s32 ret;
    __willow_show_file_param_t param;

    if (filename == NULL)
    {
        __msg("filename is NULL!");
        return EPDK_FAIL;
    }

    //__here__;
    param.img_no    = anole_npl_file2index(filename);
    param.filename  = filename;
    //__here__;
    ret = __do_play_file(&param);

    //__here__;
    if (ret == -1)
    {
        //__here__;
        ANOLE_ENTER_CRITICAL_SECTION
        anole_error_proc(filename, cb_show_pic_fail, cb_ctx_show_pic_fail);//esKRNL_CallBack( cb_show_pic_fail, cb_ctx_show_pic_fail );
        ANOLE_LEAVE_CRITICAL_SECTION
        //__here__;
    }

    //__here__;
    return ret;
}

__s32 anole_play_file_by_index(__s32 index)
{
    __s32 ret;
    char  filename[512];
    ret = anole_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("anole_npl_index2file fail...");
        return EPDK_FAIL;
    }

    if (anole_core.SwitchMode == ANOLE_FLAG_RANDOM_MODE)
    {
        __s32 switch_mode;
        switch_mode = trans_to_willow_switchmode();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SWITCH_MODE, switch_mode, NULL);
    }

    ret = anole_play_file(filename);

    if (ret == EPDK_OK)
    {
        anole_npl_playing_seek(index);
    }

    return ret;
}

/*
**********************************************************************************************************************
*                                               anole_play_prev
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_play_prev(void)
{
    __willow_show_file_param_t param;
    char path[ANOLE_MAX_FULL_FILE_PATH_LEN];
    __s32 filenum;
    __s32 total;
    __s32 ret;
    filenum = anole_npl_get_cur();

    if (filenum == -1)
    {
        return EPDK_FALSE;
    }

    total = anole_npl_get_total_count();

    if (-1 == total)
    {
        __msg("anole_npl_get_total_count = -1...");
        return EPDK_FALSE;
    }

    filenum--;

    if (filenum < 0)
    {
        filenum = total - 1;
    }

    if (filenum < 0)
    {
        filenum = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    ret = anole_npl_index2file(filenum, path);
    __msg("get photo(%d) name = %s", filenum, path);

    if (-1 == ret)
    {
        __msg("anole_npl_index2file fail...");
        return EPDK_FAIL;
    }

    anole_npl_playing_seek(filenum);
    //////////////////////////////////////////////////////////////////////////
    param.img_no = filenum;
    param.filename = path;
    //ret = _show_pic_file(&param);
    ret = __do_play_file(&param);

    if (ret == -1)
    {
        ANOLE_ENTER_CRITICAL_SECTION
        anole_error_proc(param.filename, cb_show_pic_fail, cb_ctx_show_pic_fail);//esKRNL_CallBack( cb_show_pic_fail, cb_ctx_show_pic_fail );
        ANOLE_LEAVE_CRITICAL_SECTION
        return EPDK_FALSE;
    }

    return EPDK_TRUE;
}

/*
**********************************************************************************************************************
*                                               anole_play_next
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_play_next(void)
{
    __willow_show_file_param_t param;
    char path[ANOLE_MAX_FULL_FILE_PATH_LEN];
    __s32 filenum;
    __s32 total;
    __s32 ret;
    filenum = anole_npl_get_cur();

    if (filenum == -1)
    {
        return EPDK_FALSE;
    }

    total = anole_npl_get_total_count();

    if (-1 == total)
    {
        __msg("anole_npl_get_total_count = -1...");
        return EPDK_FALSE;
    }

    filenum++;

    if (filenum >= total)
    {
        filenum = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    ret = anole_npl_index2file(filenum, path);
    __msg("get photo(%d) name = %s", filenum, path);

    if (-1 == ret)
    {
        __msg("anole_npl_index2file fail...");
        return EPDK_FAIL;
    }

    anole_npl_playing_seek(filenum);

    //////////////////////////////////////////////////////////////////////////
    param.img_no = filenum;
    param.filename = path;
    //ret = _show_pic_file(&param);
    ret = __do_play_file(&param);

    if (ret == -1)
    {
        ANOLE_ENTER_CRITICAL_SECTION
        anole_error_proc(param.filename, cb_show_pic_fail, cb_ctx_show_pic_fail);//esKRNL_CallBack( cb_show_pic_fail, cb_ctx_show_pic_fail );
        ANOLE_LEAVE_CRITICAL_SECTION
        return EPDK_FALSE;
    }

    return EPDK_TRUE;
}

__s32 anole_stop(void)
{
    return 0;
}

/*
**********************************************************************************************************************
*                                               __do_autoplay
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32 __do_autoplay(void)
{
    char    filename[512];
    __s32   index = 0;
    __s32 total;
    __s32 ret;
    __msg("__do_autoplay begin...");
    index = anole_npl_get_cur();
    __msg("anole_npl_get_cur() = %d ", index);

    if (index == -1)
    {
        //__here__;
        return EPDK_FAIL;
    }

    total = anole_npl_get_total_count();

    if (-1 == total)
    {
        __msg("anole_npl_get_total_count = -1...");
        return EPDK_FAIL;
    }

    index++;

    if (index >= total)
    {
        //__here__;
        index = 0;
    }

    ret = anole_npl_index2file(index, filename);
    __msg("get photo(%d) name = %s", index, filename);

    if (-1 == ret)
    {
        __msg("anole_npl_index2file fail...");
        return EPDK_FAIL;
    }

    anole_npl_playing_seek(index);
    {
        __s32 switch_mode;
        switch_mode = trans_to_willow_switchmode();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SWITCH_MODE, switch_mode, NULL);
        //anole_SetSwitchMode(switch_mode);
    }
    anole_play_file(filename);
    __msg("__do_autoplay end...");
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               __auto_play_maintsk
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
#define ANOLE_DEAD_LOOP_HERE    //__log("");
static void __auto_play_maintsk(void *arg)
{
    uint32_t    orign_delay;

    while (1)
    {
        ANOLE_DEAD_LOOP_HERE;

        //if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            ANOLE_DEAD_LOOP_HERE
            goto _exit_AUTO_maintsk;
        }

        ANOLE_DEAD_LOOP_HERE;

        //orign_delay = anole_GetPlayInterval();
        if (anole_get_sta() == ANOLE_MODE_PLAY)
        {
            orign_delay     = anole_get_play_interval();
            ANOLE_DEAD_LOOP_HERE;

            if (orign_delay != 1)
            {
                uint8_t count = (orign_delay - 1) * 100 / ANOLE_AUTO_PLAY_THREAD_DELAY_TIME; //延时多次

                while (count)
                {
                    esKRNL_TimeDly(ANOLE_AUTO_PLAY_THREAD_DELAY_TIME);  //每次延时ANOLE_AUTO_PLAY_THREAD_DELAY_TIME

                    if (anole_get_sta() != ANOLE_MODE_PLAY)
                    {
                        break ;
                    }
                    else
                    {
                        count-- ;
                    }
                }
            }

            ANOLE_DEAD_LOOP_HERE;

            /*adjust again*/
            if (anole_get_sta() == ANOLE_MODE_PLAY)
            {
                //              __do_autoplay();
                ANOLE_DEAD_LOOP_HERE;

                if (__do_autoplay() == EPDK_OK)
                {
                    ANOLE_DEAD_LOOP_HERE;
                    ANOLE_ENTER_CRITICAL_SECTION

                    if (cb_show_pic_next)
                    {
                        __msg("*********before esKRNL_CallBack( cb_show_pic_next*********");
                        esKRNL_CallBack(cb_show_pic_next, cb_ctx_show_pic_next);
                        __msg("*********after esKRNL_CallBack( cb_show_pic_next*********");
                    }

                    ANOLE_LEAVE_CRITICAL_SECTION
                    ANOLE_DEAD_LOOP_HERE;
                }
            }
        }

        esKRNL_TimeDly(5);
        ANOLE_DEAD_LOOP_HERE;
    }

_exit_AUTO_maintsk:
    //esKRNL_TDel(EXEC_prioself);
    esKRNL_TDel(OS_PRIO_SELF);
    return;
}


/*
**********************************************************************************************************************
*                                               __open_show
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static void __open_show(void)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_OPEN_SHOW, 0, 0);
    __wait_willow_finish();
}

/*
**********************************************************************************************************************
*                                               anole_GetShowParam
*
* Description: get anole show para, such as show region and picture layer
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
void anole_get_show_param(__willow_showing_img_info_t *info)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_SHOW_PARAM, 0, info);
    __wait_willow_finish();
}




__hdle anole_get_pic_lyr(void)
{
    __willow_showing_img_info_t inf;
    anole_get_show_param(&inf);
    return (inf.layer_hdle);
}


__bool anole_show_pic_from_buffer(__u8 *buffer, __s32 width, __s32 height)
{
    __willow_show_buffer_param_t param;
    param.img_buf = buffer;
    param.img_format = PIXEL_COLOR_ARGB8888;
    param.buf_size =   width * height * 4;
    param.img_size.width = width;
    param.img_size.height = height;
    //__here__;
    __wait_willow_ready();
    //__here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHOW_IMG_FROM_BUFFER, 0, &param);
    //__here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_DEC, 0, 0);
    //__here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_SHOW, 0, 0);
    //__here__;
    __wait_willow_finish();
    //__here__;
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t anole_init(/*__anole_para_t *para*/void)
{
    int32_t ret = 0;

    if (NULL == g_anole_sem)
    {
        g_anole_sem = esKRNL_SemCreate(1);
    }

    /* install willow */
    mid_willow  = esMODS_MInstall("d:\\mod\\willow\\willow.mod", 0);

    if (!mid_willow)
    {
        __wrn("install willow fail!");
        return -1;
    }

    /*open willow*/
    mod_willow  = esMODS_MOpen(mid_willow, 0);

    if (mod_willow == 0)
    {
        __wrn("open willow fail!");
        return -1;
    }

    //create npl
    if (__npl_create(RAT_MEDIA_TYPE_PIC) != 0)
    {
        __msg("create npl failed");
        return -1;
    }
    __open_show();

    /* start thread */
    anole_core.maintask = esKRNL_TCreate(__auto_play_maintsk, 0, 0x2000, KRNL_priolevel4);

    if (anole_core.maintask == 0)
    {
        return -1;
    }

    anole_init_screen_size(&LCD_WIDTH, &LCD_HEIGHT);
    
    anole_core.scale_level  = WILLOW_SCALER_BASE_RATIO;
    return ret;
}

__pCBK_t anole_set_cb(anole_event_e event, __pCBK_t cb, void *ctx)
{
    __pCBK_t  cb_old;
    ANOLE_ENTER_CRITICAL_SECTION

    switch (event)
    {
        case ANOLE_EVENT_SHOW_PIC_FAIL:
        {
            cb_old                  = cb_show_pic_fail;
            cb_show_pic_fail        = cb;
            cb_ctx_show_pic_fail    = ctx;
            break;
        }

        case ANOLE_EVENT_SHOW_PIC_NEXT:
        {
            cb_old                  = cb_show_pic_next;
            cb_show_pic_next        = cb;
            cb_ctx_show_pic_next    = ctx;
            break;
        }

        default :
        {
            cb_old      = NULL;
            break;
        }
    }

    ANOLE_LEAVE_CRITICAL_SECTION
    return cb_old;
}

__pCBK_t anole_get_cb(anole_event_e event)
{
    __pCBK_t    cb;
    ANOLE_ENTER_CRITICAL_SECTION

    switch (event)
    {
        case ANOLE_EVENT_SHOW_PIC_FAIL:
        {
            cb  = cb_show_pic_fail;
            break;
        }

        case ANOLE_EVENT_SHOW_PIC_NEXT:
        {
            cb  = cb_show_pic_next;
            break;
        }

        default :
        {
            cb  = NULL;
            break;
        }
    }

    ANOLE_LEAVE_CRITICAL_SECTION
    return cb;
}

/*
**********************************************************************************************************************
*                                               anole_AWILLOW_Uninitial
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/


void anole_exit(void)
{
    //__hdle   hnode = NULL;
    //char     key_value[ELIBS_KEY_VALUE_LEN + 1];
    if (1 == g_app_photo_has_touch_move_flag)
    {
        anole_slider_finish(10, 10);
    }

    anole_auto_play_thread_del();

    __msg("********before __wait_willow_ready********");

    __wait_willow_ready();

    __msg("********after __wait_willow_ready********");

    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHUT_SHOW, 0, 0);

    __msg("********before __wait_willow_finish********");

    __wait_willow_finish();

    __msg("********after __wait_willow_finish********");

    esMODS_MClose(mod_willow);

    esMODS_MUninstall(mid_willow);

    __npl_delete();

    if (g_anole_sem)
    {
        __u8 err;
        esKRNL_SemDel(g_anole_sem, OS_DEL_ALWAYS, &err);
        g_anole_sem = NULL;
    }

    //__here__
    return;
}


/*
**********************************************************************************************************************
*                                               anole_GetSta
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__anole_sta_t anole_get_sta(void)
{
    return (anole_core.status);
}

/*
**********************************************************************************************************************
*                                               anole_BackNormal
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_backnormal(void)
{
    if (anole_core.status == ANOLE_MODE_ZOOM || anole_core.status == ANOLE_MODE_PLAY)
    {
        if (anole_core.status == ANOLE_MODE_ZOOM)
        {
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_COME_BACK, 0, NULL);
            __wait_willow_finish();
        }

        anole_core.status = ANOLE_MODE_NORMAL;
        anole_core.scale_level = WILLOW_SCALER_BASE_RATIO;
    }

    return 0;
}


/*
************************************************************************************************************************
*                                       anole_Play
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/

__bool anole_auto_play(void)
{
    anole_core.status = ANOLE_MODE_PLAY; //let the thread to play pictrue
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_RotateLeft
*
* Description: picture rotate left
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_rotate_left(void)
{
    if (anole_core.status == ANOLE_MODE_PLAY)
    {
        return EPDK_FAIL;
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_ROTATE, -90, NULL);
    __wait_willow_finish();
    anole_core.scale_level = WILLOW_SCALER_BASE_RATIO;
    return 0;
}



/*
**********************************************************************************************************************
*                                               anole_RotateRight
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_rotate_right(void)
{
    if (anole_core.status == ANOLE_MODE_PLAY)
    {
        return EPDK_FAIL;
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_ROTATE, 90, NULL);
    __wait_willow_finish();
    anole_core.scale_level = WILLOW_SCALER_BASE_RATIO;
    return 0;
}

//////////////////////////////////////////////////////////////////////////
static __s32 anole_SCALE_EXT_para_2_SCALE_para(__s32 scale_ext_level, __s32 *scale_level)
{
    __s32 i;
    const __s32 map[2][3] =
    {
        {
            WILLOW_SCALER_BASE_RATIO / 8, //-3
            WILLOW_SCALER_BASE_RATIO / 4, //-2
            WILLOW_SCALER_BASE_RATIO / 2, //-1
        },
        {
            WILLOW_SCALER_BASE_RATIO * 8, // 3
            WILLOW_SCALER_BASE_RATIO * 4, // 2
            WILLOW_SCALER_BASE_RATIO * 2, // 1
        },
    };

    if (map[0][2] < scale_ext_level && scale_ext_level < map[1][2])
    {
        *scale_level     = 0;
        return EPDK_TRUE;
    }

    for (i = 0; i < 3; i++)
    {
        if (scale_ext_level <= map[0][i])
        {
            *scale_level     = i - 3;
            return EPDK_TRUE;
        }

        if (map[1][i] <= scale_ext_level)
        {
            *scale_level     = (3 - i);
            return EPDK_TRUE;
        }
    }

    return EPDK_FALSE;
}

static __s32 anole_SCALE_para_2_SCALE_EXT_para(__s32 scale_level, __s32 *scale_ext_level)
{
    if (-3 <= scale_level && scale_level <= 3)
    {
        const __s32 map[] =
        {
            WILLOW_SCALER_BASE_RATIO / 8, //-3
            WILLOW_SCALER_BASE_RATIO / 4, //-2
            WILLOW_SCALER_BASE_RATIO / 2, //-1
            WILLOW_SCALER_BASE_RATIO,     // 0
            WILLOW_SCALER_BASE_RATIO * 2, // 1
            WILLOW_SCALER_BASE_RATIO * 4, // 2
            WILLOW_SCALER_BASE_RATIO * 8, // 3
        };
        *scale_ext_level = map[scale_level + 3];
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FAIL;
    }
}

/*
**********************************************************************************************************************
*                                               anole_ZoomInc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_zoom_inc(void)
{
    __s32 level;
#if CHECK_PIC_ZOOM

    if (anole_check_pic(anole_npl_get_cur()) == EPDK_FALSE)
    {
        return EPDK_FAIL;
    }

#endif

    if (anole_core.status == ANOLE_MODE_PLAY)
    {
        return EPDK_FAIL;
    }

    anole_core.status = ANOLE_MODE_ZOOM;

    if (anole_SCALE_EXT_para_2_SCALE_para(anole_core.scale_level, &level))
    {
        if ((level < 3))
        {
            anole_SCALE_para_2_SCALE_EXT_para(++level, &anole_core.scale_level);
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE, level, NULL);
            __wait_willow_finish();
        }

        if (anole_core.scale_level == WILLOW_SCALER_BASE_RATIO)
        {
            anole_core.status = ANOLE_MODE_NORMAL;
        }
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_ZoomDec
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_zoom_dec(void)
{
    int32_t     level;
#if CHECK_PIC_ZOOM

    if (anole_check_pic(anole_npl_get_cur()) == EPDK_FALSE)
    {
        return EPDK_FAIL;
    }

#endif

    if (anole_core.status == ANOLE_MODE_PLAY)
    {
        return EPDK_FAIL;
    }

    anole_core.status = ANOLE_MODE_ZOOM;

    if (anole_SCALE_EXT_para_2_SCALE_para(anole_core.scale_level, &level))
    {
        if (level > -3)
        {
            anole_SCALE_para_2_SCALE_EXT_para(--level, &anole_core.scale_level);
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE, level, NULL);
            __wait_willow_finish();
        }

        if (anole_core.scale_level == WILLOW_SCALER_BASE_RATIO)
        {
            anole_core.status = ANOLE_MODE_NORMAL;
        }
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_zoom_set
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_zoom_set(__s32 level)//todo:for anole_core.scale_level
{
    if (anole_core.status == ANOLE_MODE_PLAY)
    {
        return EPDK_FAIL;
    }

    __msg("anole_core.scale_level = %d, level = %d", anole_core.scale_level, level);

    if (level < -3 || level > 3)
    {
        __msg("level is invalid...");
        return EPDK_FALSE;
    }

    anole_core.status = ANOLE_MODE_ZOOM;
    anole_core.scale_level = level;
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE, anole_core.scale_level, NULL);
    __wait_willow_finish();

    if (anole_core.scale_level == 0)
    {
        anole_core.status = ANOLE_MODE_NORMAL;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_zoom_get
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_zoom_get(void)
{
    return anole_core.scale_level;
}

/*
**********************************************************************************************************************
*                                               anole_SetScrRect
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_set_scn_rect(RECT *rect)
{
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_COME_BACK, 0, NULL);
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SCN, 1, rect);
    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_SetPlayInterval
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_set_play_interval(__u32 sec)
{
    if (sec > 15 || sec < 1)
    {
        return EPDK_FAIL;
    }

    anole_core.interval = sec;
    return 0;
}



/*
**********************************************************************************************************************
*                                               anole_SetSwitchMode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 anole_set_switch_mode(__anole_switch_mode_t mode)
{
    anole_core.SwitchMode = mode;
    mode = trans_to_willow_switchmode();
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SWITCH_MODE, mode, 0);
    __wait_willow_finish();
    return 0;
}


__s32 anole_set_switch_mode_array_for_play(__s32 *switch_mode_array, __s32 array_num)
{
    anole_core.switchmodearray  = switch_mode_array;
    anole_core.arraynum         = array_num;
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_SetPlayOrder
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 anole_set_play_order(__anole_playorder_t order)
{
    if (ANOLE_PLAY_NEXT == order || ANOLE_PLAY_PREV == order)
    {
        rat_npl_set_play_mode(anole_npl, RAT_PLAY_MODE_ROTATE_ALL);
    }
    else
    {
        rat_npl_set_play_mode(anole_npl, RAT_PLAY_MODE_RANDOM);
    }

    anole_core.PlayOrder = order;
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_GetSwichMode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/


__anole_switch_mode_t anole_get_switch_mode(void)
{
    return (anole_core.SwitchMode);
}


/*
**********************************************************************************************************************
*                                               anole_GetScrRect
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_get_scn_rect(RECT *rect)
{
    __willow_showing_img_info_t img_inf;
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_SHOW_PARAM, 0, &img_inf);
    __wait_willow_finish();
    rect->x = img_inf.show_region.x;
    rect->y = img_inf.show_region.y;
    rect->width = img_inf.show_region.width;
    rect->height = img_inf.show_region.height;
    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_GetPlayInterval
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__u32 anole_get_play_interval(void)
{
    return (anole_core.interval);
}


/*
**********************************************************************************************************************
*                                               anole_SetBgPic
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_set_bg_pic(FB *fb)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_CATCH_SCREEN, 0, fb);
    __wait_willow_finish();
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_GetPlayOrder
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__anole_playorder_t anole_get_play_order(void)
{
    return (anole_core.PlayOrder);
}


/*
**********************************************************************************************************************
*                                               anole_GetThumb
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_get_thumb(char *filename, void *buf, __anole_thumbs_fmt_t *param)
{
    __willow_get_thumbs_param_t willow_para;

    if (buf == NULL || filename == NULL)
    {
        return EPDK_FAIL;
    }

    willow_para.buf = (__u8 *)buf;
    willow_para.filename = filename;
    willow_para.format = param->format;
    willow_para.size.width = param->size.width;
    willow_para.size.height = param->size.height;
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_THUMBS, 0, &willow_para);
    __wait_willow_finish();

    if (willow_para.filename)
    {
        if (esMODS_MIoctrl(mod_willow, WILLOW_CMD_CHECK_IMG, 0, &willow_para) == EPDK_FAIL)
        {
            return EPDK_FAIL;
        }

        __wait_willow_ready();
        //esMODS_MIoctrl(mod_willow,WILLOW_CMD_START_DEC,0,0);
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_SHOW, 0, 0);
        __wait_willow_finish();
    }

    param->size.width = willow_para.size.width;
    param->size.height = willow_para.size.height;
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               anole_GetPicInfo
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_get_pic_info(__u32 index, __anole_pic_info_t *info)
{
    __willow_show_file_param_t param;
    char filename[768];
    anole_npl_index2file(index, filename);
    param.img_no    = index;
    param.filename  = filename;
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHOW_IMG_FROM_FILE, 0, &param);
    __wait_willow_finish();

    //check if the format is supported
    if (esMODS_MIoctrl(mod_willow, WILLOW_CMD_CHECK_IMG, 0, &param) == EPDK_FAIL)
    {
        __msg("the file format is not supported!");
        __wait_willow_finish();
        return EPDK_FAIL;
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_IMG_INFO, 0, (__willow_img_info_t *)info);
    __wait_willow_finish();
    return 0;
}

static __s32 __get_file_name_by_index(__willow_show_file_param_t *param)
{
    anole_npl_index2file(param->img_no, param->filename);
    return 0;
}

__s32 anole_slider_init(FB *decoding_fb, FB *unsupported_fb)
{
    __willow_img_plst_info_t playlist;
    playlist.total = anole_npl_get_total_count();
    playlist.get_imagename_by_index = esKRNL_GetCallBack((__pCBK_t)__get_file_name_by_index);
    playlist.decoding_fb[0] = decoding_fb;
    playlist.decoding_fb[1] = NULL;
    playlist.decoding_fb[2] = NULL;
    playlist.unsupported_fb = unsupported_fb;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_PLAYLIST, 0, &playlist);
    return 0;
}

__s32 anole_slider_start(__s16 x, __s16 y)
{

    if (0 == g_app_photo_has_touch_move_flag)
    {
        slider_step_para_t slider_cmd;
        slider_cmd.cmd      = SLIDER_STEP_START;
        slider_cmd.offset.x = x;
        slider_cmd.offset.y = y;
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOV_TRACK, 0, &slider_cmd);
        g_app_photo_has_touch_move_flag = 1;
    }

    return 0;
}

__s32 anole_slider_move(__s16 x, __s16 y)
{
    slider_step_para_t slider_cmd;
    slider_cmd.cmd      = SLIDER_STEP_MOVE;
    slider_cmd.offset.x = x;
    slider_cmd.offset.y = y;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOV_TRACK, 0, &slider_cmd);
    return 0;
}

__s32 anole_slider_finish(__s16 x, __s16 y)
{
    if (1 == g_app_photo_has_touch_move_flag)
    {
        slider_step_para_t slider_cmd;
        __s32 index;
        slider_cmd.cmd      = SLIDER_STEP_FINISH;
        slider_cmd.offset.x = x;
        slider_cmd.offset.y = y;
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOV_TRACK, 0, &slider_cmd);
        __wait_willow_ready();
        index = esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_CUR_INDEX, 0, 0);
        __wait_willow_finish();
        anole_npl_playing_seek(index);
        g_app_photo_has_touch_move_flag = 0;
    }
    return 0;
}

__s32 anole_set_layer_sta(__anole_layer_sta layer_sta)
{
    if (layer_sta == ANOLE_LAYER_TOP)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_TOP, 0);
    }
    else if (layer_sta == ANOLE_LAYER_BOTTOM)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_BOTTOM, 0);
    }
    else if (layer_sta == ANOLE_LAYER_SHOW)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_SHOW, 0);
    }
    else if (layer_sta == ANOLE_LAYER_HIDE)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_HIDE, 0);
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_set_show_mode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_set_show_mode(__s32 index)
{
    __s32 tmp_index;

    if (index == 0)
    {
        tmp_index = WILLOW_IMAGE_SHOW_AUTO;
    }
    else if (index == 1)
    {
        tmp_index = WILLOW_IMAGE_SHOW_ORISIZE;
    }
    else if (index == 2)
    {
        tmp_index = WILLOW_IMAGE_SHOW_CUTOFF;
    }
    else
    {
        tmp_index = WILLOW_IMAGE_SHOW_STRETCH;
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SHOW_MODE, tmp_index, NULL);
    __wait_willow_finish();
    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_scale_move
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_scale_move(__anole_diretion_t way)
{
    __willow_showing_img_info_t info;

    //__here__
    if (anole_core.status != ANOLE_MODE_ZOOM)
    {
        return EPDK_FAIL;
    }

    anole_get_show_param(&info);

    switch (way)
    {
        case ANOLE_MOVE_UP:
            //if( (info.show_region.y + info.show_region.height - ANOLE_MOVE_BASE_DIST) >= LCD_HEIGHT )
        {
            __pos_t     param;

            param.x     = 0;
            param.y     = -ANOLE_MOVE_BASE_DIST;
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
            __wait_willow_finish();;
        }
        break;

        case ANOLE_MOVE_DOWN:
            //if( (info.show_region.y + ANOLE_MOVE_BASE_DIST) <= 0)
        {
            __pos_t     param;
            param.x     = 0;
            param.y     = ANOLE_MOVE_BASE_DIST;
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
            __wait_willow_finish();
        }
        break;

        case ANOLE_MOVE_LEFT:
            //if((info.show_region.x + info.show_region.width - ANOLE_MOVE_BASE_DIST) >= LCD_WIDTH)
        {
            __pos_t     param;

            param.x     = -ANOLE_MOVE_BASE_DIST;
            param.y     = 0;
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
            __wait_willow_finish();
        }
        break;

        case ANOLE_MOVE_RIGHT:
            //if((info.show_region.x + ANOLE_MOVE_BASE_DIST) <= 0)
        {
            __pos_t     param;

            param.x     = ANOLE_MOVE_BASE_DIST;
            param.y     = 0;
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
            __wait_willow_finish();
        }
        break;

        default:
            break;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////////add for touch
__bool anole_scale_touch_move(__pos_t offset)
{
    __willow_showing_img_info_t info;
    __pos_t     param;

    if (anole_core.status != ANOLE_MODE_ZOOM)
    {
        return EPDK_FAIL;
    }

    anole_get_show_param(&info);

    param.x     = offset.x;
    param.y     = offset.y;

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
    __wait_willow_finish();
    return 0;
}

uint32_t anole_scale_get_level(void)
{
    return anole_core.scale_level;
}

__bool anole_scale_inc(__fp32 scale)
{
#if CHECK_PIC_ZOOM

    if (anole_check_pic(anole_npl_get_cur()) == EPDK_FALSE)
    {
        return EPDK_FAIL;
    }

#endif

    if (anole_core.status == ANOLE_MODE_PLAY)
    {
        return EPDK_FAIL;
    }

    if (scale <= 0.00001)
    {
        return EPDK_FAIL;
    }

    anole_core.status = ANOLE_MODE_ZOOM;
    anole_core.scale_level *= scale;

    if (anole_core.scale_level > WILLOW_SCALER_MAX_RATIO)
    {
        anole_core.scale_level = WILLOW_SCALER_MAX_RATIO;
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE_EXT, anole_core.scale_level, NULL);
    __wait_willow_finish();

    if (anole_core.scale_level == WILLOW_SCALER_BASE_RATIO)
    {
        anole_core.status = ANOLE_MODE_NORMAL;
    }

    return 0;
}

__bool anole_scale_dec(__fp32 scale)
{
#if CHECK_PIC_ZOOM

    if (anole_check_pic(anole_npl_get_cur()) == EPDK_FALSE)
    {
        return EPDK_FAIL;
    }

#endif

    if (scale <= 0.00001)
    {
        return EPDK_FAIL;
    }

    anole_core.status = ANOLE_MODE_ZOOM;
    anole_core.scale_level /= scale;

    if (anole_core.scale_level < 1)
    {
        anole_core.scale_level = 1;
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE_EXT, anole_core.scale_level, NULL);
    __wait_willow_finish();

    if (anole_core.scale_level == WILLOW_SCALER_BASE_RATIO)
    {
        anole_core.status = ANOLE_MODE_NORMAL;
    }

    return 0;
}

void anole_scale_back_normal(void)
{
    int32_t step;

    __msg("scale_level = %d", anole_core.scale_level);

    if (anole_core.scale_level < WILLOW_SCALER_BASE_RATIO)
    {
        step    = (WILLOW_SCALER_BASE_RATIO - anole_core.scale_level) / 5;

        if (step > 1)
        {
            for (; WILLOW_SCALER_BASE_RATIO - anole_core.scale_level > step; anole_core.scale_level += step)
            {
                __msg("scale_level = %d", anole_core.scale_level);
                __wait_willow_ready();
                esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE_EXT, anole_core.scale_level, NULL);
                __wait_willow_finish();
            }
        }
    }

    if (anole_core.scale_level > WILLOW_SCALER_BASE_RATIO)
    {
        step    = (anole_core.scale_level - WILLOW_SCALER_BASE_RATIO) / 5;

        if (step > 1)
        {
            for (; anole_core.scale_level - WILLOW_SCALER_BASE_RATIO > step; anole_core.scale_level -= step)
            {
                __msg("scale_level = %d", anole_core.scale_level);
                __wait_willow_ready();
                esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE_EXT, anole_core.scale_level, NULL);
                __wait_willow_finish();
            }
        }
    }

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE_EXT, WILLOW_SCALER_BASE_RATIO, NULL);
    __wait_willow_finish();
    anole_core.status = ANOLE_MODE_NORMAL;
}

__s32 anole_set_backcolor(__u32 color)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_BACKCOLOR, color, NULL);
    __wait_willow_finish();
    return 0;
}
