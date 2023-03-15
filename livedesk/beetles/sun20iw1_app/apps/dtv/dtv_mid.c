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
#include <log.h>
#include "dtv_i.h"
#include <emodules/mod_ca.h>
#include <emodules/mod_display.h>



//在此定义的全局变量最好放到结构休中，
//仅声明该结构休的一个全局指针，指向在堆中申请的内存
//减小栈的使用
__u32 maple_mid;
__mp *maple = NULL;
maple_demod_info_t *demodParam = NULL;

typedef enum
{
    PIN_PULL_DEFAULT    =   0xFF,
    PIN_PULL_DISABLE    =   0x00,
    PIN_PULL_UP         =   0x01,
    PIN_PULL_DOWN       =   0x02,
    PIN_PULL_RESERVED   =   0x03
} pin_pull_t;

char *str = NULL;
char *strbw = NULL;

char *epgdate = NULL;
#if (MAPLE_TV_STANDAR != MAPLE_TV_CMMB)
maple_cas_param_t *ca_sn = NULL; //modify by hot.lee
#endif


static unsigned long               video_layer_hdl = NULL;
__mp *mp_display = NULL;

//服务列表(频道列表)
maple_sl_cb_param_t *sl_event_cbp = NULL;
__u32               sl_task_id;
maple_serv_list_t  *service_list = NULL;
maple_serv_list_t *search_service_list = NULL;//仅用于搜索时用

//当前频道的节目预告
maple_schdl_cb_param_t  *schdl_event_cbp = NULL;
maple_schdl_list_t      *schdl_list = NULL;
maple_schdl_list_day   *schdl_list_day = NULL;
__u32               schedule_task_id = 0;
dtvmid_time_t *tv_time = NULL;


//当前频道的当前正在播放和下一个将要播放的节目预告
maple_sepg_t            *sepg = NULL;
maple_sepg_cb_param_t   *sepg_finish_cbp = NULL;
__u32               sepg_task_id = 0;
char *sepg_string_c = NULL;
char *sepg_string_n = NULL;

__s32 g_weak_singal = 1;
__s32 g_record_flag = 0;
__s32 g_mute_flag = 0;
maple_ve_status_e *g_maple_ve_status = NULL;
reg_cmmb_para_t *cmmb_para = NULL;
reg_cmmb_para_t *cmmb_para_bak = NULL;



__s32 maple_cur_search_freq = 0;//当前搜索的频点

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
static __epdk_charset_enm_e g_type = EPDK_CHARSET_ENM_GB2312; //默认中国,需要根据出往哪个国家进行设置
#endif

static __u32 G_DTV_current_area = 1;//1;   //0:只用于深圳频率，1:全频段搜索



static __u32 g_programtype = SSET_NC_PROGRAMTYPE_ALL;

#define QSIZE 8
__krnl_event_t  *maple_CmdQ = NULL;
__krnl_event_t  *maple_pFedBakQ = NULL;


static int mode = MAPLE_VID_WINDOW_BESTSHOW;

//static H_WIN  h_play_dialog = NULL;
__bool b_full_screen = EPDK_TRUE;  //非缩略图播放


const Region_TPSects_t TPRegion_England[] =
{

#if(MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)
    {473143000, 6000},
    {479143000, 6000},
    {485143000, 6000},
    {491143000, 6000},
    {497143000, 6000},
    {503143000, 6000},
    {509143000, 6000},
    {515143000, 6000},
    {521143000, 6000},
    {527143000, 6000},
    {533143000, 6000},
    {539143000, 6000},
    {545143000, 6000},
    {551143000, 6000},
    {557143000, 6000},
    {563143000, 6000},
    {569143000, 6000},
    {575143000, 6000},
    {581143000, 6000},
    {587143000, 6000},
    {593143000, 6000},
    {599143000, 6000},
    {605143000, 6000},
    {611143000, 6000},
    {617143000, 6000},
    {623143000, 6000},
    {629143000, 6000},
    {635143000, 6000},
    {641143000, 6000},
    {647143000, 6000},
    {653143000, 6000},
    {659143000, 6000},
    {665143000, 6000},
    {671143000, 6000},
    {677143000, 6000},
    {683143000, 6000},
    {689143000, 6000},
    {695143000, 6000},
    {701143000, 6000},
    {707143000, 6000},
    {713143000, 6000},
    {719143000, 6000},
    {725143000, 6000},
    {731143000, 6000},
    {737143000, 6000},
    {743143000, 6000},
    {749143000, 6000},
    {755143000, 6000},
    {761143000, 6000},
    {767143000, 6000},
    {773143000, 6000},
    {779143000, 6000},
    {785143000, 6000},
    {791143000, 6000},
    {797143000, 6000},
    {803143000, 6000},

#elif(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)
#if 1
    { 52500000, 8000},
    { 60500000, 8000},
    { 68500000, 8000},
    { 80000000, 8000},
    { 88000000, 8000},
    {171000000, 8000},
    {179000000, 8000},
    {187000000, 8000},
    {195000000, 8000},
    {203000000, 8000},
    {211000000, 8000},
    {219000000, 8000},
#endif
    {474000000, 8000},
    {482000000, 8000},
    {490000000, 8000},
    {498000000, 8000},
    {506000000, 8000},
    {514000000, 8000},
    {522000000, 8000},
    {530000000, 8000},
    {538000000, 8000},
    {546000000, 8000},
    {554000000, 8000},
    {562000000, 8000},
    {570000000, 8000},
    {578000000, 8000},
    {586000000, 8000},
    {594000000, 8000},
    {602000000, 8000},
    {610000000, 8000},
    {618000000, 8000},
    {626000000, 8000},
    {634000000, 8000},
    {642000000, 8000},
    {650000000, 8000},
    {658000000, 8000},
    {666000000, 8000},
    {674000000, 8000},
    {682000000, 8000},
    {690000000, 8000},
    {698000000, 8000},
    {706000000, 8000},
    {714000000, 8000},
    {722000000, 8000},
    {730000000, 8000},
    {738000000, 8000},
    {746000000, 8000},
    {754000000, 8000},
    {762000000, 8000},
    {770000000, 8000},
    {778000000, 8000},
    {786000000, 8000},
    {794000000, 8000},
    {802000000, 8000},
    {810000000, 8000},
    {818000000, 8000},
    {826000000, 8000},
    {834000000, 8000},
    {842000000, 8000},
    {850000000, 8000},
    {858000000, 8000},

#elif(MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)
    {722000000, 8000},
    {730000000, 8000},
#elif(MAPLE_TV_STANDAR == MAPLE_TV_ATSC)
    {722000000, 8000},
    {730000000, 8000},
#endif

};

const Region_TPSects_t TPRegion_China[] =
{
#if(MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)
    {473143000, 6000},
    {479143000, 6000},
    {485143000, 6000},
#elif(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)
    {722000000, 8000},
    {730000000, 8000},
#elif(MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)
    {722000000, 8000},
    {730000000, 8000},
#elif(MAPLE_TV_STANDAR == MAPLE_TV_ATSC)
    {722000000, 8000},
    {730000000, 8000},
#endif
};

const  char *const TPStr_England[] =
{
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

const  char *const TPStr_China[] =
{
    "2", "3", "4"
};

static void _clearprogramname(void);

DVBT_AreaTable_t dvb_t_areatp_table[SSET_NC_AREASET_AREAMAX] =
{

    {SSET_NC_AREASET_SHENZHEN, TPRegion_China, TPStr_China, sizeof(TPRegion_China) / sizeof(Region_TPSects_t), "CHN"},
    {SSET_NC_AREASET_ALLFREQ, TPRegion_England, TPStr_England, sizeof(TPRegion_England) / sizeof(Region_TPSects_t), "GBR"},

};


static __hdle g_antenna_switch_hdl = NULL;
static __s32 maple_antenna_switch_ctrl_init(void)
{
    __s32            ret;
    user_gpio_set_t  gpio_set[1];
    eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
    ret = esCFG_GetKeyValue("dtmb_para", "antenna_switch", (__s32 *)gpio_set, sizeof(user_gpio_set_t) / 4);

    if (!ret)
    {
        g_antenna_switch_hdl = esPINS_PinGrpReq(gpio_set, 1);

        if (!g_antenna_switch_hdl)
        {
            __err("request antenna_switch pin failed\n");
            return EPDK_FAIL;
        }
    }
    else
    {
        __msg("fetch para from script failed\n");
        return EPDK_FAIL;
    }

    __msg("maple_antenna_switch_ctrl_init ok\n");
    esPINS_SetPinPull(g_antenna_switch_hdl, PIN_PULL_UP, NULL);
    //  esPINS_PinGrpRel(hdl, 0);
    return EPDK_OK;
}
__s32  maple_get_cur_channel_Idx(void)
{
    __s32 i = 0;

    for (i = 0; i < service_list->servNum; i++)
    {
        if (service_list->curServIdx == service_list->pServItems[i].channelId)
        {
            return i;
        }
    }

    return -1;
}
__s32  maple_check_serviceid_exist(__s32 svcId)
{
    __s32 i = 0;
    __wrn("maple_check_serviceid_exist : %u\n", svcId);

    for (i = 0; i < service_list->servNum; i++)
    {
        __wrn("[%u]%u, %u\n", i, service_list->pServItems[i].servId, service_list->pServItems[i].channelId);

        if (svcId == service_list->pServItems[i].servId)
        {
            __msg("service id exist\n");
            return i;
        }
    }

    return -1;
}
static __s32  maple_get_serviceid(__s32 index)
{
    if (index > service_list->servNum - 1)
    {
        return EPDK_FAIL;
    }

    return service_list->pServItems[index].servId;
}
__u16 dtv_get_cur_service_id(void)
{
    return (__u16)maple_get_serviceid(dtv_ctr.cur_user_index);
}


static __s32 maple_antenna_switch_ctrl_uninit(void)
{
    if (g_antenna_switch_hdl)
    {
        esPINS_PinGrpRel(g_antenna_switch_hdl, 0);
    }

    return EPDK_OK;
}
//0:外置 1:内置
__s32 maple_get_antenna_mode(void)
{
    __msg("cmmb_para->default_antenna=%d \n", cmmb_para->default_antenna);
    return cmmb_para->default_antenna;
}

__s32 maple_set_antenna_mode(__u8 mode)
{
    cmmb_para->default_antenna = mode;

    if (mode == DTV_ANTENNA_LA)
    {
        esPINS_WritePinData(g_antenna_switch_hdl, 1, NULL); //pg0 控制天线切换的GPIO口,内置
    }
    else
    {
        esPINS_WritePinData(g_antenna_switch_hdl, 0, NULL); //pg0 控制天线切换的GPIO口,外
    }

    return 0;
}


/*
**********************************************************************************************************************
*                                            maple_open
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
#define MEM_RESERVED_SIZE  (1<<20)
__s32 maple_open(void)
{
    maple_mid = esMODS_MInstall("d:\\mod\\maple.mod", 0);

    if (!maple_mid)
    {
        __wrn("install maple fail.\n");
        //goto mod;
        maple = NULL;
        return EPDK_FAIL;
    }

    //__here__;
    maple = esMODS_MOpen(maple_mid, 0);

    if (!maple)
    {
        __wrn("open maple fail.\n");
        //goto mod1;
        goto err0;
    }

    esMODS_MIoctrl(maple, MAPLE_CMD_SET_RESERVED_MEM_SIZE, MEM_RESERVED_SIZE, NULL);
    //__here__;
	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __wrn("Open display driver failed!\n");
        goto err1;
    }

    //__here__;
    maple_set_tv_mode(1);
    sl_event_cbp = (maple_sl_cb_param_t *)esMEMS_Malloc(0, sizeof(maple_sl_cb_param_t));
    eLIBs_memset(sl_event_cbp, 0, sizeof(maple_sl_cb_param_t));
    service_list = (maple_serv_list_t *)esMEMS_Malloc(0, sizeof(maple_serv_list_t));
    eLIBs_memset(service_list, 0, sizeof(maple_serv_list_t));
    service_list->pServItems = (maple_serv_item_t *)esMEMS_Malloc(0, sizeof(maple_serv_item_t) * MAPLE_MAX_SERV_NUM);
    eLIBs_memset(service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
    search_service_list = (maple_serv_list_t *)esMEMS_Malloc(0, sizeof(maple_serv_list_t));
    eLIBs_memset(search_service_list, 0, sizeof(maple_serv_list_t));
    search_service_list->pServItems = (maple_serv_item_t *)esMEMS_Malloc(0, sizeof(maple_serv_item_t) * MAPLE_MAX_SERV_NUM);
    eLIBs_memset(search_service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
    schdl_event_cbp = (maple_schdl_cb_param_t *)esMEMS_Malloc(0, sizeof(maple_schdl_cb_param_t));
    eLIBs_memset(schdl_event_cbp, 0, sizeof(maple_schdl_cb_param_t));
    schdl_list = (maple_schdl_list_t *)esMEMS_Malloc(0, sizeof(maple_schdl_list_t));
    eLIBs_memset(schdl_list, 0, sizeof(maple_schdl_list_t));
    schdl_list->pSchdlEvents = (maple_schdl_event_t *)esMEMS_Malloc(0, sizeof(maple_schdl_event_t) * MAPLE_MAX_SCHDL_NUM);
    eLIBs_memset(schdl_list->pSchdlEvents, 0, (sizeof(maple_schdl_event_t)*MAPLE_MAX_SCHDL_NUM));
    sepg_finish_cbp = (maple_sepg_cb_param_t *)esMEMS_Malloc(0, sizeof(maple_sepg_cb_param_t));
    eLIBs_memset(sepg_finish_cbp, 0, sizeof(maple_sepg_cb_param_t));
    sepg = (maple_sepg_t *)esMEMS_Malloc(0, sizeof(maple_sepg_t));
    eLIBs_memset(sepg, 0, sizeof(maple_sepg_t));
    sepg_string_c = (char *)esMEMS_Malloc(0, MAX_SEPG_STRING);
    eLIBs_memset(sepg_string_c, 0, MAX_SEPG_STRING);
    sepg_string_n = (char *)esMEMS_Malloc(0, MAX_SEPG_STRING);
    eLIBs_memset(sepg_string_n, 0, MAX_SEPG_STRING);
    str = (char *)esMEMS_Malloc(0, MAX_TMP_STRING);
    eLIBs_memset(str, 0, MAX_TMP_STRING);
    strbw = (char *)esMEMS_Malloc(0, MAX_TMP_STRING);
    eLIBs_memset(strbw, 0, MAX_TMP_STRING);
    epgdate = (char *)esMEMS_Malloc(0, MAX_TMP_STRING);
    eLIBs_memset(epgdate, 0, MAX_TMP_STRING);
#if (MAPLE_TV_STANDAR != MAPLE_TV_CMMB)
    ca_sn = (maple_cas_param_t *)esMEMS_Malloc(0, MAX_TMP_STRING);
    eLIBs_memset(ca_sn, 0, MAX_TMP_STRING);
    __msg("ca_sn = %x--------------->>>\n", ca_sn);
    ca_sn->casNum = (__u8 *)esMEMS_Malloc(0, 32);
    eLIBs_memset(ca_sn->casNum, 0, 32);
    __msg("ca_sn->casNum = %x------>>>>\n", ca_sn->casNum);
#endif
    //__here__;
    schdl_list_day = (maple_schdl_list_day *)esMEMS_Malloc(0, MAX_EPG_DAY * sizeof(maple_schdl_list_day));
    eLIBs_memset(schdl_list_day, 0, MAX_EPG_DAY * sizeof(maple_schdl_list_day));
    tv_time = (dtvmid_time_t *)esMEMS_Malloc(0, sizeof(dtvmid_time_t));
    eLIBs_memset(tv_time, 0, sizeof(dtvmid_time_t));
    maple_CmdQ = esKRNL_QCreate(QSIZE);
    maple_pFedBakQ = esKRNL_QCreate(QSIZE);
    b_full_screen = EPDK_TRUE;
    //__here__;
    cmmb_para = esMEMS_Malloc(0, sizeof(reg_cmmb_para_t));
    eLIBs_memset(cmmb_para, 0, sizeof(reg_cmmb_para_t));
    cmmb_para_bak = esMEMS_Malloc(0, sizeof(reg_cmmb_para_t));
    eLIBs_memset(cmmb_para_bak, 0, sizeof(reg_cmmb_para_t));
    return EPDK_OK;
err1:
    esMODS_MClose(maple);
    maple = NULL;
err0:
    esMODS_MUninstall(maple_mid);
    maple_mid = NULL;
    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                            maple_close
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
__s32 maple_close(void)
{
    __u8 err;
    __s32 ret;

    if (cmmb_para)
    {
        esMEMS_Mfree(0, cmmb_para);
        cmmb_para = NULL;
    }

    if (cmmb_para_bak)
    {
        esMEMS_Mfree(0, cmmb_para_bak);
        cmmb_para_bak = NULL;
    }

    //* close demod driver.
    maple_set_tv_mode(0);//__here__;
    maple_time_monitor_stop();//__here__;
    maple_epg_search_stop();//__here__;
    maple_search_stop();//__here__;
    //__here__;
    maple_stop_program();

    //__here__;
    if (maple)
    {
        ret = esMODS_MClose(maple);
        maple = NULL;
        __wrn(" close maple ret = %d.\n", ret);
    }

    //* uninstall demod driver.
    if (maple_mid)
    {
        ret = esMODS_MUninstall(maple_mid);
        maple_mid = 0;
        __wrn("uninstall maple. ret = %d \n", ret);
    }

    //__here__;
    if (video_layer_hdl) // modify140325
    {
        __u64 arg[3];
        arg[0] = video_layer_hdl;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        video_layer_hdl = 0;
    }

    //__here__;

    if (service_list->pServItems)
    {
        esMEMS_Mfree(0, service_list->pServItems);
        service_list->pServItems = NULL;
    }

    esMEMS_Mfree(0, service_list);
    esMEMS_Mfree(0, sl_event_cbp);

    if (schdl_list->pSchdlEvents)
    {
        esMEMS_Mfree(0, schdl_list->pSchdlEvents);
        schdl_list->pSchdlEvents = NULL;
    }

    esMEMS_Mfree(0, schdl_list);
    esMEMS_Mfree(0, schdl_event_cbp);

    if (search_service_list->pServItems)
    {
        esMEMS_Mfree(0, search_service_list->pServItems);
        search_service_list->pServItems = NULL;
    }

    esMEMS_Mfree(0, search_service_list);
    esMEMS_Mfree(0, sepg);
    esMEMS_Mfree(0, sepg_finish_cbp);
    esMEMS_Mfree(0, sepg_string_c);
    esMEMS_Mfree(0, sepg_string_n);
    esMEMS_Mfree(0, str);
    esMEMS_Mfree(0, strbw);
    esMEMS_Mfree(0, epgdate);
    esMEMS_Mfree(0, schdl_list_day);
    esMEMS_Mfree(0, tv_time);
#if (MAPLE_TV_STANDAR != MAPLE_TV_CMMB)
    esMEMS_Mfree(0, ca_sn->casNum);
    ca_sn->casNum = 0;
#endif
    esMEMS_Mfree(0, ca_sn);

    if (maple_CmdQ)
    {
        esKRNL_QDel(maple_CmdQ, 0, &err);
        maple_CmdQ = NULL;
    }

    if (maple_pFedBakQ)
    {
        esKRNL_QDel(maple_pFedBakQ, 0, &err);
        maple_pFedBakQ = NULL;
    }

    maple_antenna_switch_ctrl_uninit();
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                            maple_connect_signal
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

__s32 maple_connect_signal(maple_demod_info_t *tp)
{
    maple_demod_info_t *tmp = NULL;

    if (tp == NULL)
    {
        __wrn(" tp NULL.\n");
        return EPDK_FAIL;
    }

    tmp = (maple_demod_info_t *)eLIBs_malloc(sizeof(maple_demod_info_t));

    if (tmp == NULL)
    {
        __wrn(" tmp eLIBs_malloc fail.\n");
        return EPDK_FAIL;
    }

    eLIBs_memset(tmp, 0x00, sizeof(maple_demod_info_t));
    eLIBs_memcpy(tmp, tp, sizeof(maple_demod_info_t));

    if (esMODS_MIoctrl(maple, MAPLE_CMD_TUNE, 0, tmp) != 0)
    {
        __wrn(" tune maple fail.\n");
        return EPDK_FAIL;
    }
    else
    {
        __wrn(" tune maple ok.\n");
        return EPDK_OK;
    }

    eLIBs_free(tmp);
    tmp = NULL;
}


//根据当前搜索第几个频点，获取该频点,add by hot.lee
__s32 maple_get_cur_search_freq(__s32 index)
{
    DVBT_AreaTable_t *area_p;
    area_p = &dvb_t_areatp_table[G_DTV_current_area];
    return area_p->pTPRegion_Area[index].StartFreq;
}

//当前需要搜索的总频点数,add by hot.lee
__s32 maple_get_cur_search_freq_tatol(void)
{
    DVBT_AreaTable_t *area_p;
    area_p = &dvb_t_areatp_table[G_DTV_current_area];
    return (area_p->TPIndex);
}

__s32 maple_get_cur_foucs_index_from_freq(__u32 freq)
{
    DVBT_AreaTable_t *area_p = NULL;
    __u8 i = 0;
    area_p = &dvb_t_areatp_table[G_DTV_current_area];

    for (i = 0; i < area_p->TPIndex; i++)
    {
        if (freq == area_p->pTPRegion_Area[i].StartFreq)
        {
            return i;
        }
    }

    return 0;
}

__u8 maple_get_first_picture_status(void)
{
    return esMODS_MIoctrl(maple, MAPLE_CMD_CHECK_PICTURE_OUT, 0, NULL);
}
__s32 maple_get_search_freq(__u32 index)
{
    DVBT_AreaTable_t *area_p;
    area_p = &dvb_t_areatp_table[G_DTV_current_area];
    return area_p->pTPRegion_Area[index].StartFreq;
}

__s32 maple_get_search_bandwidth(__u32 index)
{
    DVBT_AreaTable_t *area_p;
    area_p = &dvb_t_areatp_table[G_DTV_current_area];
    return area_p->pTPRegion_Area[index].BandWidth;
}

//add by hot.lee
//设置当前正在搜索的频点，用于更新搜索界面的频率显示
static void maple_set_cur_search_freq(__s32 freq, __u16 index, __u32 total)
{
    maple_cur_search_freq = freq;
#if 1

    if (dtv_ctr.h_scan)
    {
        __gui_msg_t     setmsg;
        setmsg.id           =  GUI_MSG_COMMAND;
        setmsg.h_srcwin     =  NULL;
        setmsg.h_deswin     =  dtv_ctr.h_scan;
        setmsg.dwAddData1   = (index << 16) | (total << 8) | SLIDE_UPDATE_FREQ;
        setmsg.dwAddData2   = freq;
        setmsg.p_arg        =  NULL;
        GUI_SendNotifyMessage(&setmsg);
    }

#endif
}

extern __s32 dtv_set_cur_user_index(__s32 index);


#if (MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)
static __s32 __get_valid_channel(void)
{
    __s32 i = 0, j = 0;
    __msg("search_service_list->servNum = %d\n", search_service_list->servNum);

    for (i = 0; i < search_service_list->servNum; i++)
    {
        __msg("search_service_list->pServItems->pmtId = %d\n", search_service_list->pServItems[i].pmtId);

        if (search_service_list->pServItems[i].pmtId >= 0x1fc8
            && search_service_list->pServItems[i].pmtId <= 0x1fcf)
        {
            eLIBs_memcpy(&service_list->pServItems[j], &search_service_list->pServItems[i], sizeof(maple_serv_item_t));
            j++;
        }
    }

    service_list->servNum = j;
    //更新中间件,因为中间在退出时保存时包括收费频道
    //不然退出再进入时频道号是错误的
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
        //if(cmmb_para->flag == EPDK_TRUE)
        {
            //经过MAPLE_CMD_SL_ADD后，channelId会发生变化，所以需要重新调用MAPLE_CMD_SL_GET
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_ADD, 0, (void *)service_list);
            service_list->servNum    = 0;
            service_list->curServIdx = 0;
            eLIBs_memset((void *)service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, 0, (void *)service_list);
        }
    }
    return EPDK_OK;
}

#endif

static void Bubble_1(maple_serv_item_t r[], int n)
{
    int i = n - 1; //初始时,最后位置保持不变

    while (i > 0)
    {
        int pos = 0; //每趟开始时,无记录交换
        int j = 0;

        for (; j < i; j++)
            if (r[j].servId > r[j + 1].servId)
            {
                maple_serv_item_t tmp;
                pos = j; //记录交换的位置
                //tmp = r[j]; r[j]=r[j+1];r[j+1]=tmp;
                eLIBs_memcpy(&tmp, (void *)&r[j], sizeof(maple_serv_item_t));
                eLIBs_memcpy((void *)&r[j], (void *)&r[j + 1], sizeof(maple_serv_item_t));
                eLIBs_memcpy((void *)&r[j + 1], &tmp, sizeof(maple_serv_item_t));
            }

        i = pos; //为下一趟排序作准备
    }
}

static void __sort_by_service_id(void)
{
    __s32 i = 0;

    if (service_list->servNum <= 1)
    {
        return ;
    }

    Bubble_1(&service_list->pServItems[i], service_list->servNum);
    return ;
}

static void __sort_free_channel_first(void)
{
    __s32 i = 0;
    __s32 j = 0;

    if (service_list->servNum <= 1)
    {
        return ;
    }

    for (i = 0; i < service_list->servNum; i++)
    {
        if (service_list->pServItems[service_list->servNum - i].isCAFree)
        {
            for (j = 0; j < service_list->servNum - i; j++)
            {
                if (service_list->pServItems[j].isCAFree == 0)
                {
                    maple_serv_item_t tmp;
                    eLIBs_memcpy(&tmp, (void *)&service_list->pServItems[j], sizeof(maple_serv_item_t));
                    eLIBs_memcpy((void *)&service_list->pServItems[j], (void *)&service_list->pServItems[service_list->servNum - i], sizeof(maple_serv_item_t));
                    eLIBs_memcpy((void *)&service_list->pServItems[service_list->servNum - i], &tmp, sizeof(maple_serv_item_t));
                }
            }
        }
    }
}




#if (MAPLE_TV_STANDAR == MAPLE_TV_DTMB)
static __s32 __get_valid_channel(void)
{
    __s32 i = 0, j = 0;
    __msg("search_service_list->servNum = %d\n", search_service_list->servNum);

    for (i = 0; i < search_service_list->servNum; i++)
    {
        __msg("search_service_list->pServItems->pmtId = %d\n", search_service_list->pServItems[i].pmtId);

        if (search_service_list->pServItems[i].pmtId <= 0x1fcf)
        {
            eLIBs_memcpy(&service_list->pServItems[j], &search_service_list->pServItems[i], sizeof(maple_serv_item_t));
            j++;
        }
    }

    service_list->servNum = j;
    //__sort_by_service_id();
    __sort_free_channel_first();
    //更新中间件,因为中间在退出时保存时包括收费频道
    //不然退出再进入时频道号是错误的
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
        //if(cmmb_para->flag == EPDK_TRUE)
        {
            //经过MAPLE_CMD_SL_ADD后，channelId会发生变化，所以需要重新调用MAPLE_CMD_SL_GET
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_ADD, 0, (void *)service_list);
            service_list->servNum    = 0;
            service_list->curServIdx = 0;
            eLIBs_memset((void *)service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, 0, (void *)service_list);
        }
    }
    return 0;
}

#endif
//modify by hot.lee
__s32 maple_service_list_get(maple_serv_group_type_e type)
{
    __wrn("service_list->num = %d service_list->curServIdx = %d\n", service_list->servNum, service_list->curServIdx);
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
    esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, type, (void *)search_service_list); //MAPLE_SERV_GROUP_TYPE_ALL
    __get_valid_channel();
#endif
#if (0)
    {
        __s32 i;

        for (i = 0; i < service_list->servNum; i++)
        {
            __msg("channelId = %d: %s\n", service_list->pServItems[i].channelId, service_list->pServItems[i].servName);
        }
    }
#endif
    return service_list->servNum;
}
Region_TPSects_t auto_serach_xuchang[1] = {{634 * 1000 * 1000, 8000}}; //许昌
void maple_all_freq_table(scan_para_t *scan_para)
{
    if (scan_para == NULL)
    {
        return;
    }

    scan_para->pTPRegion_Area = TPRegion_England;
    scan_para->TPIndex = sizeof(TPRegion_England) / sizeof(Region_TPSects_t);
    return ;
}
void maple_auto_search_major(scan_para_t *scan_para)
{
    __u32 i = 0;
    __u8 err = 0;
    __s32 ret = EPDK_FAIL;
    __u8 major_num = sizeof(auto_serach_xuchang) / sizeof(Region_TPSects_t);
    Region_TPSects_t *major_table = auto_serach_xuchang;
    sl_event_cbp->pOpaque   = NULL;
    sl_event_cbp->pOpaque = esKRNL_SemCreate(0);

    if (!sl_event_cbp->pOpaque)
    {
        __wrn("sl search semaphore create fail.\n");
        goto _sl_search_exit;
    }

    __wrn("%s %d major_num:%d\r\n", __FILE__, __LINE__, major_num);

    for (i = 0; i < major_num; i++)
    {
        esKRNL_TimeDly(1);
        eLIBs_memset(demodParam, 0x00, sizeof(maple_demod_info_t));
        demodParam->bandwidth = major_table[i].BandWidth;
        demodParam->frequency = major_table[i].StartFreq / 1000;
        //如果上次等待超时，则maple任务会被杀掉，maple多发一个finish，callback，信号量增加1，导致下一个频点搜索失败
        esKRNL_SemAccept(sl_event_cbp->pOpaque);

        if (esMODS_MIoctrl(maple, MAPLE_CMD_TUNE, 0, demodParam) != 0)
        {
            continue;
        }

        search_service_list->nit_freq_num = 0;
        esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_MAJOR_SERV, demodParam->frequency, (void *)search_service_list);
        //* wait a long time for search progress.
        esKRNL_SemPend(sl_event_cbp->pOpaque, 5000, &err);
        {
            ret = esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_SERV_STOP, 0, NULL);
        }

        if (search_service_list->nit_freq_num > 0)
        {
            break;
        }
    }

    __wrn("%s %d nit_freq_num:%d\r\n", __FILE__, __LINE__, search_service_list->nit_freq_num);
_sl_search_exit:

    if (sl_event_cbp->pOpaque)
    {
        esKRNL_SemDel(sl_event_cbp->pOpaque, OS_DEL_ALWAYS, &err);
        sl_event_cbp->pOpaque = NULL;
    }

    if (search_service_list->nit_freq_num > 0)
    {
        Region_TPSects_t *temp = NULL;
        temp = (Region_TPSects_t *)eLIBs_malloc(search_service_list->nit_freq_num *
                                                sizeof(Region_TPSects_t));

        if (temp == NULL)
        {
            __wrn("scan_para->pTPRegion_Area malloc err!\r\n");
            return ;
        }

        eLIBs_memset((void *)temp, 0x00, search_service_list->nit_freq_num *
                     sizeof(Region_TPSects_t));
        scan_para->TPIndex = search_service_list->nit_freq_num;

        for (i = 0; i < scan_para->TPIndex; i++)
        {
            temp[i].BandWidth = 8000;
            temp[i].StartFreq = search_service_list->nit_freq_khz[i] * 1000;
            __wrn("%s %d StartFreq:%d\r\n", __FILE__, __LINE__, temp[i].StartFreq);
        }

        scan_para->pTPRegion_Area = temp;
        scan_para->scan_major = EPDK_FALSE;
    }
    else
    {
        scan_para->TPIndex = major_num;
        scan_para->pTPRegion_Area = auto_serach_xuchang;
        scan_para->scan_major = EPDK_TRUE;
    }
}
static __bool search_task_break = EPDK_FALSE; //人为中断搜台线程,add by hot.lee
__s32 g_is_searching = 0;
void maple_break_search_task(void)
{
    //esKRNL_SemPost(sl_event_cbp->pOpaque);
    search_task_break = EPDK_TRUE;
}

//* task for searching service
//modify by hot.lee for
//__u32 freq=*(__u32 *)(arg); != 0 //单频点搜索
void sl_area_search_task(void *arg)
{
    __u32 i = 0;
    __u8 err = 0;
    __s32 ret = EPDK_FAIL;
    //DVBT_AreaTable_t * area_p = NULL;
    scan_para_t *scan_para = (scan_para_t *)arg;
    search_task_break = EPDK_FALSE;
    //esKRNL_SemPend(dtv_ctr.sem,0,NULL);
    //area_p=&dvb_t_areatp_table[G_DTV_current_area];
    service_list->servNum   = 0;
    service_list->curServIdx = 0;
    eLIBs_memset(service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
    sl_event_cbp->pOpaque   = NULL;
    sl_event_cbp->pOpaque = esKRNL_SemCreate(0);

    if (!sl_event_cbp->pOpaque)
    {
        __wrn("sl search semaphore create fail.\n");
        goto _sl_search_exit;
    }

    if (service_list->pServItems == NULL)
    {
        __wrn("malloc fail.\n");
        goto _sl_search_exit;
    }

    esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
    //__wrn("area_p->TPIndex=%d dvb_t_areatp_table=%d \n",area_p->TPIndex,dvb_t_areatp_table[G_DTV_current_area].TPIndex);
    __wrn("area_p->TPIndex=%d\n", scan_para->TPIndex);
    __wrn("%s %d\r\n", __FILE__, __LINE__);

    for (i = 0; i < scan_para->TPIndex; i++)
    {
        esKRNL_TimeDly(1);

        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            __msg("sl_area_search_task()... abort...\n");
            goto _sl_search_exit;
        }

        eLIBs_memset(demodParam, 0x00, sizeof(maple_demod_info_t));
        demodParam->bandwidth = scan_para->pTPRegion_Area[i].BandWidth;
        demodParam->frequency = scan_para->pTPRegion_Area[i].StartFreq / 1000;
        //如果上次等待超时，则maple任务会被杀掉，maple多发一个finish，callback，信号量增加1，导致下一个频点搜索失败
        esKRNL_SemAccept(sl_event_cbp->pOpaque);

        if (MANUAL_SERACH_TYPE == scan_para->scan_type) //手动搜索
        {
            search_service_list->nit_freq_num = -1;// will del ;add for auto search
            maple_set_cur_search_freq(demodParam->frequency, i + 1, 1);
        }
        else if (
#ifdef DTV_AUTO_SEARCH_ENABLE
            scan_para->scan_type == AUTO_SERACH_TYPE ||
#endif
            scan_para->scan_type == ALL_FREQ_SERACH_TYPE)
        {
            search_service_list->nit_freq_num = -1;// will del ;add for auto search
            maple_set_cur_search_freq(demodParam->frequency, i + 1, scan_para->TPIndex);
        }

        __wrn("%s %d\r\n", __FILE__, __LINE__);

        if (EPDK_TRUE == search_task_break)
        {
            goto  _sl_search_exit;
        }

        //tps.frequency = tps.frequency*1000;
        if (esMODS_MIoctrl(maple, MAPLE_CMD_TUNE,  0, demodParam) != 0)
        {
            __wrn(" tune maple fail.\n");

            if (MANUAL_SERACH_TYPE == scan_para->scan_type) //手动搜索
            {
                break;
            }

            continue;
        }

        if (EPDK_TRUE ==  search_task_break)
        {
            goto  _sl_search_exit;
        }

        //__here__;
        esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_SERV, demodParam->frequency, (void *)search_service_list);

        if (EPDK_TRUE == search_task_break)
        {
            goto  _sl_search_exit;
        }

        //* wait a long time for search progress.
        esKRNL_SemPend(sl_event_cbp->pOpaque, 5000, &err);
        {
            ret = esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_SERV_STOP, 0, NULL);
            //esKRNL_SemPost(sl_event_cbp->pOpaque);
        }

        if (EPDK_TRUE == search_task_break)
        {
            goto  _sl_search_exit;
        }

        if (MANUAL_SERACH_TYPE == scan_para->scan_type) //手动搜索
        {
            break;
        }
    }

#if 0
    maple_set_cur_search_freq(demodParam->frequency, i);
#endif
    maple_service_list_get(MAPLE_SERV_GROUP_TYPE_ALL);
    __msg("service_list->servNum=%d \n", service_list->servNum);

    if (service_list->servNum > 0) //搜台结束之后做一次保存
    {
        //maple_save_reg_current_program();
        //maple_program_save(DTV_DATABASE_NAME);
#if 0
        maple_program_save(DTV_DATABASE_NAME);
        maple_save_reg_video_window_ratio_mode();
        maple_save_reg_current_program();
#else
        cmmb_para->total = service_list->servNum;
        cmmb_para_bak->total = service_list->servNum;
        maple_reg_flush();
#endif
    }
    else
    {
        //是否不再保存上一次搜台结果 ?
        //eLIBs_format("z:\\","fat",0);
        maple_program_load(DTV_DATABASE_NAME, EPDK_FALSE);
    }

#if 1
    dtv_delete_all_menu(0);
#endif

    if (maple_get_cur_service_list_num() >= 1)
    {
        app_misc_audio_mute((dtv_mute_status == 0) ? 1 : 0);
    }

    if (err != 0)
    {
        __wrn("service arealist search fail.\n");
    }
    else
    {
        __wrn("service arealist search finish.\n");
    }

_sl_search_exit:

    if (ALL_FREQ_SERACH_TYPE != scan_para->scan_type && scan_para->scan_major != EPDK_TRUE)
    {
        eLIBs_free((void *)scan_para->pTPRegion_Area);
        scan_para->pTPRegion_Area = NULL;
    }

    eLIBs_free(scan_para);
    scan_para = NULL;
    __msg("sl_area_search_task()... exit...\n");
    search_task_break = EPDK_FALSE;
    //////esKRNL_SemPost(dtv_ctr.sem);//小心，会引发线程调度

    if (sl_event_cbp->pOpaque)
    {
        esKRNL_SemDel(sl_event_cbp->pOpaque, OS_DEL_ALWAYS, &err);
        sl_event_cbp->pOpaque = NULL;
    }

    __wrn("service area search task exit.\n");

    if (demodParam != NULL)
    {
        eLIBs_free(demodParam);
        demodParam = NULL;
    }

    maple_play_program();
    check_dtv_or_radio();
    /*移动 防止第一次收到节目后不能播放问题*/
    g_is_searching = 0;
    sl_task_id = NULL;
    esKRNL_TDel(EXEC_prioself);
}


__s32 maple_register_search_callback(__pCBK_t cb_search_list_event_callback, __pCBK_t cb_search_list_finish_callback)
{
    __pCBK_t            callback;
    callback = esKRNL_GetCallBack(cb_search_list_event_callback);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET_PARAM, MAPLE_CB_SRCH_SL_EVENT, (void *)sl_event_cbp);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET, MAPLE_CB_SRCH_SL_EVENT, (void *)callback);
    callback = esKRNL_GetCallBack(cb_search_list_finish_callback);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET_PARAM, MAPLE_CB_SRCH_SL_FINISH, (void *)sl_event_cbp);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET, MAPLE_CB_SRCH_SL_FINISH, (void *)callback);
    __wrn("maple_register_search_callback exit.\n");
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET, MAPLE_CB_SIGNAL_LOCK_EVENT, (void *)NULL);
    return EPDK_OK;
}


__s32 maple_thread_delete(__u32 thread_id)
{
    if (thread_id != 0)
    {
        __s8 ret;

        //    esKRNL_TDelReq(thread_id);
        //    esKRNL_TimeDly(5);
        //    esKRNL_TimeDlyResume(thread_id);
        while ((ret = esKRNL_TDelReq(thread_id)) != OS_TASK_NOT_EXIST)
        {
            eLIBs_printf("thread_id = %d : %d\n", thread_id, ret);
            esKRNL_TimeDlyResume(thread_id);
            esKRNL_TimeDly(5);
        }

        thread_id = 0;
        __msg("thread has been deleted.\n");
    }

    return EPDK_OK;
}

__s32 maple_area_search(scan_para_t *scan_para)
{
    if (sl_task_id != 0)
    {
        g_is_searching = 0;
        return EPDK_OK;
    }

    sl_task_id = esKRNL_TCreate(sl_area_search_task,
                                (void *)scan_para,
                                0x1000,
                                KRNL_priolevel4);

    if (sl_task_id == 0)
    {
        g_is_searching = 0;
        __wrn("create sl_areasearch_task fail.\n");
        return EPDK_FAIL;
    }
    else
    {
        __wrn("create sl_areasearch_task[id = %d] success.\n", sl_task_id);
        return EPDK_OK;
    }
}

__s32 maple_search_stop(void)
{
    __s8 ret;

    if (sl_task_id != 0)
    {
        ret = esKRNL_TDelReq(sl_task_id);
        __msg("TDelReq status = %d \n", ret); //use to debug
        ret = esKRNL_TimeDlyResume(sl_task_id);
        __msg("TimeDlyResume status = %d \n", ret);

        while (sl_task_id && esKRNL_TDelReq(sl_task_id) != OS_TASK_NOT_EXIST)
        {
            ret = esKRNL_TimeDlyResume(sl_task_id);
            __msg("TimeDlyResume status = %d \n", ret);
            esKRNL_TimeDly(10);
        }

        sl_task_id = 0;
    }

    return EPDK_OK;
}




/****************************************************************************************************************************
*save / load program ,查询是否有备份的播放列表,modify by hot.lee
******************************************************************************************************************************/
__s32 maple_program_load(char *database_file_name, __u8 first_load)
{
    __s32 ret;
    ES_FILE *pf = NULL;
    char str_filename[128] = {0};
    char baseinf_filename[64] = {0};
    eLIBs_strcpy(str_filename, database_file_name);
    pf = eLIBs_fopen(str_filename, "rb");

    if (NULL == pf)
    {
        //存台文件不存在，默认搜台
        __msg("file [%s] not exist , search\n", str_filename);
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
        cmmb_para->b_show_subtitle = EPDK_TRUE;   //默认字幕存在
        //dtv_subtitle_set_subtitle_flag(EPDK_TRUE);
#endif
        //return EPDK_FAIL;
        ret = EPDK_FAIL;
    }
    else
    {
        eLIBs_fclose(pf);
        pf = NULL;
        ret = EPDK_TRUE;
    }

    //台可能没有，设置可能改变
    eLIBs_strcpy(baseinf_filename, DTV_BASEINFO_NAME);
    pf = eLIBs_fopen(baseinf_filename, "rb");

    if (pf)
    {
        __u8   temp_antenna = 0;

        if (first_load == EPDK_FALSE)
        {
            temp_antenna = cmmb_para->default_antenna;
        }

        eLIBs_fread(cmmb_para, 1, sizeof(reg_cmmb_para_t), pf);
        eLIBs_fclose(pf);
        pf = NULL;

        if (first_load == EPDK_FALSE)
        {
            cmmb_para->default_antenna = temp_antenna;
        }

        eLIBs_memcpy(cmmb_para_bak, cmmb_para, sizeof(reg_cmmb_para_t));
    }

    if (first_load == EPDK_TRUE)
    {
        maple_antenna_switch_ctrl_init();//天线设置
        maple_set_antenna_mode(cmmb_para->default_antenna);
    }

    if (ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    ret = esMODS_MIoctrl(maple, MAPLE_CMD_SL_OPEN, 0, (void *)str_filename);

    if (EPDK_FAIL == ret)
    {
        __msg("reg ok , why open file [%s] ?\n", str_filename);
        return EPDK_FAIL;
    }

#if 0 //原来不修改ESG
    service_list->servNum    = 0;
    service_list->curServIdx = 0;
    eLIBs_memset((void *)service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);

    //在此获取的是上一次保存的
    if (g_programtype == SSET_NC_PROGRAMTYPE_TV)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_TV, (void *)service_list);
    }
    else if (g_programtype == SSET_NC_PROGRAMTYPE_RADIO)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_RADIO, (void *)service_list);
    }
    else if (g_programtype == SSET_NC_PROGRAMTYPE_ALL)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_ALL, (void *)service_list);
    }

    esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
    //if(cmmb_para->flag == EPDK_TRUE)
    {
        //经过MAPLE_CMD_SL_ADD后，channelId会发生变化，所以需要重新调用MAPLE_CMD_SL_GET
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_ADD, 0, (void *)service_list);
        __msg("service_list->servNum = %d  cmmb_para->index = %d\n", service_list->servNum, cmmb_para->index);
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
        dtv_ctr.b_has_subtitle = cmmb_para->b_show_subtitle;
        //dtv_subtitle_set_subtitle_flag(dtv_ctr.b_has_subtitle);
#endif
#if 1
        service_list->servNum    = 0;
        service_list->curServIdx = 0;
        eLIBs_memset((void *)service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
#endif
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, 0, (void *)service_list);
        {
            service_list->curServIdx = service_list->pServItems[cmmb_para->index].channelId;
            __msg("service_list->curServIdx = %d name = %s servId=%d...\n", service_list->curServIdx, service_list->pServItems[cmmb_para->index].servName, service_list->pServItems[cmmb_para->index].servId);
        }
#else //修改ESG后
#if 0
    service_list->servNum    = 0;
    service_list->curServIdx = 0;
    eLIBs_memset((void *)service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);

    //在此获取的是上一次保存的
    if (g_programtype == SSET_NC_PROGRAMTYPE_TV)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_TV, (void *)service_list);
    }
    else if (g_programtype == SSET_NC_PROGRAMTYPE_RADIO)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_RADIO, (void *)service_list);
    }
    else if (g_programtype == SSET_NC_PROGRAMTYPE_ALL)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_ALL, (void *)service_list);
    }

    esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
    __wrn("app1 :channelId[%x],servId[%x],subfrmId[%x],timeSlot[%x],timeSlotCnt[%x],demodArg[%x]\n", \
          service_list->pServItems[cmmb_para->index].channelId, service_list->pServItems[cmmb_para->index].servId, service_list->pServItems[cmmb_para->index].pmtId, \
          service_list->pServItems[cmmb_para->index].tsId, service_list->pServItems[cmmb_para->index].networkId, service_list->pServItems[cmmb_para->index].demodArg);
    __wrn("ldpcRate[%x],modulateMode[%x],byteInterMode[%x],rsRate[%x],scrambleMode[%x],muxfrmId[%x]\n", \
          service_list->pServItems[cmmb_para->index].ldpcRate, service_list->pServItems[cmmb_para->index].modulateMode, service_list->pServItems[cmmb_para->index].byteInterMode, \
          service_list->pServItems[cmmb_para->index].rsRate, service_list->pServItems[cmmb_para->index].scrambleMode, service_list->pServItems[cmmb_para->index].muxfrmId);
#endif
    //if(cmmb_para->flag == EPDK_TRUE)
    {
        //经过MAPLE_CMD_SL_ADD后，channelId会发生变化，所以需要重新调用MAPLE_CMD_SL_GET
        //////  esMODS_MIoctrl(maple, MAPLE_CMD_SL_ADD, 0, (void*)service_list);
        ///////  __wrn("service_list->servNum = %d  cmmb_para->index = %d\n",service_list->servNum,cmmb_para->index);
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
        dtv_ctr.b_has_subtitle = cmmb_para->b_show_subtitle;
        //dtv_subtitle_set_subtitle_flag(dtv_ctr.b_has_subtitle);
#endif
#if 1
        service_list->servNum    = 0;
        service_list->curServIdx = 0;
        eLIBs_memset((void *)service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
#endif

        if (g_programtype == SSET_NC_PROGRAMTYPE_TV)
        {
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_TV, (void *)service_list);
        }
        else if (g_programtype == SSET_NC_PROGRAMTYPE_RADIO)
        {
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_RADIO, (void *)service_list);
        }
        else if (g_programtype == SSET_NC_PROGRAMTYPE_ALL)
        {
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_ALL, (void *)service_list);
        }

        if (cmmb_para->index >= service_list->servNum)
        {
            cmmb_para->index = service_list->servNum - 1;
        }

        if (cmmb_para->index < 0)
        {
            cmmb_para->index = 0;
        }

        {
            //播放最后一次播放的节目
            service_list->curServIdx = service_list->pServItems[cmmb_para->index].channelId;
            __msg("service_list->curServIdx = %d name = %s servId=%d...\n", service_list->curServIdx, service_list->pServItems[cmmb_para->index].servName, service_list->pServItems[cmmb_para->index].servId);
        }

        __wrn("app2 :channelId[%x],servId[%x],subfrmId[%x],timeSlot[%x],timeSlotCnt[%x],demodArg[%x]\n", \
              service_list->pServItems[cmmb_para->index].channelId, service_list->pServItems[cmmb_para->index].servId, service_list->pServItems[cmmb_para->index].pmtId, \
              service_list->pServItems[cmmb_para->index].tsId, service_list->pServItems[cmmb_para->index].networkId, service_list->pServItems[cmmb_para->index].demodArg);
        __wrn("ldpcRate[%x],modulateMode[%x],byteInterMode[%x],rsRate[%x],scrambleMode[%x],muxfrmId[%x]\n", \
              service_list->pServItems[cmmb_para->index].ldpcRate, service_list->pServItems[cmmb_para->index].modulateMode, service_list->pServItems[cmmb_para->index].byteInterMode, \
              service_list->pServItems[cmmb_para->index].rsRate, service_list->pServItems[cmmb_para->index].scrambleMode, service_list->pServItems[cmmb_para->index].muxfrmId);
#endif
        //esMODS_MIoctrl(maple,MAPLE_CMD_SERV_ITEM_SET_LOCK,service_list->curServIdx,0);
#if 0
        __wrn("service_list->servNum = %d \n", service_list->servNum)

        for (i = 0; i < service_list->servNum; i++)
        {
            j = 0;
            __wrn(" ========= service %d: %s. channelid = %d servId = %d\n",
                  i, service_list->pServItems[i].servName, service_list->pServItems[i].channelId, service_list->pServItems[i].servId);
            //while(service_list->pServItems[i].servName[j]!='\0')
            //{
            //    __wrn("--%x",(int)service_list->pServItems[i].servName[j]);
            //    j++;
            //}
            //__wrn("\n");
        }

#endif
        return EPDK_OK;
    }
    return EPDK_FAIL;
}
static void maple_filter_ca_program(void)
{
    __u8 Index = 0, i = 0;

    if (search_service_list->servNum > 0)
    {
        for (i = 0; i < search_service_list->servNum; i++)
        {
            if (search_service_list->pServItems[i].isCAFree == EPDK_FALSE)
            {
                continue;
            }
            else
            {
                eLIBs_memcpy(&search_service_list->pServItems[Index], &search_service_list->pServItems[i],
                             sizeof(maple_serv_item_t));
                Index++;
            }
        }

        search_service_list->servNum = Index;
    }

    return ;
}
__s32 maple_program_add_srvlist(void)
{
    __wrn("==maple_program_add_srvlist---service_list->servNum=%d\n", search_service_list->servNum);

    if (search_service_list->servNum > 0)
    {
        //#if   (MAPLE_CA_STANDAR == DTV_CA_NONE)
        //      maple_filter_ca_program();
        //#endif
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_ADD, 0, (void *)search_service_list);
        search_service_list->servNum = 0;
        search_service_list->curServIdx = 0;
        eLIBs_memset(search_service_list->pServItems, 0, sizeof(maple_serv_item_t)*MAPLE_MAX_SERV_NUM);
    }

    return EPDK_OK;
}
__s32 maple_program_get_srvlist(__u32 programtype)
{
    if (programtype == SSET_NC_PROGRAMTYPE_TV)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_TV, (void *)service_list);
    }
    else if (programtype == SSET_NC_PROGRAMTYPE_RADIO)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_RADIO, (void *)service_list);
    }
    else if (programtype == SSET_NC_PROGRAMTYPE_ALL)
    {
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, MAPLE_SERV_GROUP_TYPE_ALL, (void *)service_list);
    }

    return service_list->servNum;
}
__s32 maple_cur_flash_save_program_index(void)
{
    return cmmb_para->index;
}
__s32 maple_set_cur_flash_save_program_index(__s32 index)
{
    if (index == cmmb_para->index)
    {
        return EPDK_FAIL;
    }

    cmmb_para->index = index;
    return EPDK_OK;
}
//有变更时才重刷flash
__s32 maple_reg_flush(void)
{
    __s32 ret = EPDK_FAIL;
    ES_FILE *fp = NULL;
    char baseinf_filename[64] = {0};
    __bool b_write_flash = EPDK_FALSE;

    //return EPDK_OK;
    if (EPDK_TRUE != eLIBs_memcmp(cmmb_para_bak, cmmb_para, sizeof(reg_cmmb_para_t)))
    {
        //__here__;
        eLIBs_strcpy(baseinf_filename, DTV_BASEINFO_NAME);
        //__here__;
        fp = eLIBs_fopen(baseinf_filename, "wb");

        //__here__;
        if (fp == NULL) //写不成功时才格式化Z盘
        {
            //__here__;
            eLIBs_format("z:\\", "fat", 0);
            esKRNL_TimeDly(10);
            fp = eLIBs_fopen(baseinf_filename, "wb");

            if (fp == NULL)
            {
                __wrn("2open file fail:%s\n", DTV_BASEINFO_NAME);

                if (fp)
                {
                    eLIBs_fclose(fp);
                }

                return EPDK_FAIL;
            }
        }

        //__here__;
        eLIBs_fwrite(cmmb_para, 1, sizeof(reg_cmmb_para_t), fp);
        eLIBs_fclose(fp);
        maple_program_save(DTV_DATABASE_NAME);
        b_write_flash = EPDK_TRUE;
        //__here__;
    }

    //其实可以不放在这里，我们可以在需要的时候才保存
    //这样我们只需要保存当前播放的信息到独立分区
    //而不需要和台的信息放在同一盘符

    if (b_write_flash == EPDK_TRUE)
    {
        //
#if 0/*langaojie temp test*/
        ES_FILE *hFile;
        //here  actury  flush user disk data to NOR flash
        hFile = eLIBs_fopen("b:\\STORAGE\\DTV", "rb");

        if (hFile == NULL)
        {
            __wrn("open NOR file failed\n");
            return EPDK_FAIL;
        }

        __msg("flush cmmb file ....\n");
        esMODS_MIoctrl(hFile, DEV_IOC_USR_FLUSH_CACHE, 0, 0);
        __msg("flush cmmb end ....\n");
        eLIBs_fclose(hFile);
        //__here__;
#endif
    }

    return EPDK_OK;
}
//文件名自定义，中间件会去创建
__s32 maple_program_save(char *database_file_name)
{
    if (service_list->servNum > 0)
    {
        __s32 ret;
        char str[128];
        eLIBs_strcpy(str, DTV_DATABASE_NAME);
        ret = esMODS_MIoctrl(maple, MAPLE_CMD_SL_SAVE, 0, (void *)str);
        __msg("ret = %d\n", ret);

        if (ret != EPDK_OK)
        {
            __wrn("save program fail\n");
        }
        else
        {
            ES_FILE *pf = NULL;
            pf = eLIBs_fopen(database_file_name, "rb");

            if (pf)
            {
                eLIBs_fclose(pf);
                pf = NULL;
            }
            else
            {
                __wrn("why open file [%s] fail\n", database_file_name);
            }
        }

        return EPDK_OK;
    }

    return EPDK_FAIL;
}
/***************************************************************************************************
*Name        : maple_request_video_layer
*Prototype   : __s32  maple_request_video_layer( const RECT *rect_p, __s32 pipe, __s32 prio )
*Arguments   : rect_p    input. the coordinate and size of the video layer.
*              pipe      input. layer pipe.
*              prio      input. layer priority.
*Return      : the video layer handle. if return NULL, means failing.
*Description : request the video layer.
*Other       :
***************************************************************************************************/
__hdle  maple_request_video_layer(const RECT * rect_p, __s32 pipe, __s32 prio)
{
    //__disp_layer_para_t     image_layer_para;
    //FB                      image_fb_para;
    RECT                    image_win;
    __u64 arg[3];
    //__s32 ret;

    if (rect_p == NULL)
    {
        return NULL;
    }

#if 1

    if (NULL == video_layer_hdl)
    {
        //* request a layer for playing video,
        arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
        arg[1] = 0;
        arg[2] = 0;
        __msg("DISP_CMD_LAYER_REQUEST.............\n");
        video_layer_hdl = esMODS_MIoctrl(mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);

        if (video_layer_hdl == NULL)
        {
            __wrn("Error in applying for the video layer.\n");
            goto error;
        }
    }

    arg[0] = video_layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
#endif
    image_win.x      = rect_p->x;
    image_win.y      = rect_p->y;
    image_win.width  = rect_p->width ;
    image_win.height = rect_p->height;
    esMODS_MIoctrl(maple, MAPLE_CMD_SET_VID_LAYER, 0, (void *)video_layer_hdl);
    esMODS_MIoctrl(maple, MAPLE_CMD_SET_VID_WINDOW, 0, (void *)&image_win);
    // esMODS_MIoctrl(maple, MAPLE_CMD_SET_VID_SHOW_MODE, mode, NULL);
    return (__hdle)video_layer_hdl;
error:
    return NULL;
}
__s32 maple_get_play_status(void)
{
    if (!maple)
    {
        return -1;
    }

    return esMODS_MIoctrl(maple, MAPLE_CMD_GET_STATUS, 0, NULL);
}
__s32 app_misc_audio_mute(__s32 flag)
{
    //开关关功放操作
    return EPDK_OK;
}
__s32 maple_play_program(void)
{
    RECT                video_window;
    __u64 arg[3];
    __msg("maple_play_program............\n");

    if (service_list->servNum <= 0)
    {
        __wrn("service_list->servNum<=0");
        return EPDK_FAIL;
    }

    dtv_loading_show();
    b_full_screen = EPDK_TRUE;
    //* set video layer handle to dvbc
    video_window.x      = 0;
    video_window.y      = 0;
    video_window.width  = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    //预留字幕显示区域，避免影响观看效果
    video_window.height = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    __wrn("w = %d  h = %d \n", video_window.width, video_window.height);

    if (!video_layer_hdl)
    {
        video_layer_hdl = (unsigned long)maple_request_video_layer(&video_window, 0, 0xff);
    }

    __wrn("service_list->num = %d service_list->curServIdx = %d\n", service_list->servNum, service_list->curServIdx);
    maple_set_screensize(mode);
    //__here__;
    esMODS_MIoctrl(maple, MAPLE_CMD_ENABLE_DEINTERLACE, 2, NULL);

    //__here__;
    if (esMODS_MIoctrl(maple, MAPLE_CMD_PLAY, service_list->curServIdx, NULL) != EPDK_OK)
    {
        __wrn("play service %d fail.\n", service_list->curServIdx);
        //if(NULL == h_play_dialog)
        {
            //  __s32 lang_id[]={ STRING_DTV_TIPS , STRING_DTV_CUR_PROGRAM_OPEN_FAIL };
            //  default_dialog(h_play_dialog,GUI_WinGetHandFromName(APP_DTV),DVB_PROT_DIALOG_ID,ADLG_OK,lang_id);
        }
        app_misc_audio_mute(0);//播放失败关闭功放
        return EPDK_FAIL;
    }
    else
    {
        //display = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
        __wrn("service id = %d ................\n", service_list->pServItems[dtv_ctr.cur_user_index].servId);

        if (video_layer_hdl)
        {
            arg[0] = video_layer_hdl;
            arg[1] = 0;
            arg[2] = 0;
            //esMODS_MIoctrl(mp_display, DISP_CMD_LAYER_OPEN, 0, (void *)arg);// modify140325
            esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
        }

        //esMODS_MIoctrl(mp_display, DISP_CMD_LCD_ON, 0, 0); //不需要，如果用的话TVOUT时会有问题
        //eLIBs_fclose(display);
        app_misc_audio_mute(1);//播放成功再打开功放
        dtv_ctr.play_cnt = 1;
        cur_playing_index = service_list->curServIdx;
    }

    //__here__;
    /////////////////////maple_time_monitor_start();
    maple_epg_search_stop();
    esKRNL_TimeDly(1);
    maple_epg_search();
    ///maple_time_monitor_stop();
    ///esKRNL_TimeDly(1);
    ///maple_time_monitor_start();
    //check_auth_timer();
    return EPDK_OK;
}
void dtv_loading_show(void)
{
#ifdef DTV_LOADING_ENABLE
    app_loading_win_show(dtv_ctr.h_loading);

    if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_LOADING_TIMER_ID) == EPDK_FALSE)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        GUI_SetTimer(g_dtv_mainwin, DVB_LOADING_TIMER_ID, 50, NULL);
    }
    else
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        GUI_ResetTimer(g_dtv_mainwin, DVB_LOADING_TIMER_ID, 50, NULL);
    }

#endif
    return ;
}
void dtv_loading_hide(void)
{
#ifdef DTV_LOADING_ENABLE
    eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
    app_loading_win_hide(dtv_ctr.h_loading);

    if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_LOADING_TIMER_ID) == EPDK_TRUE)
    {
        GUI_KillTimer(g_dtv_mainwin, DVB_LOADING_TIMER_ID);
    }

#endif
    return ;
}
void dtv_loading_destory_timer(void)
{
#ifdef DTV_LOADING_ENABLE

    if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_LOADING_TIMER_ID))
    {
        GUI_KillTimer(g_dtv_mainwin, DVB_LOADING_TIMER_ID);
    }

#endif
    return ;
}
/*默认只要带ca就是无授权*/
void check_auth_timer(void)
{
#if (MAPLE_CA_STANDAR == DTV_CA_NONE)

    if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_AUTH_TIMER_ID) == EPDK_TRUE)
    {
        GUI_ResetTimer(g_dtv_mainwin, DVB_AUTH_TIMER_ID, 500, NULL);
    }
    else
    {
        GUI_SetTimer(g_dtv_mainwin, DVB_AUTH_TIMER_ID, 500, NULL);
    }

#endif
    return ;
}
void check_auth_timer_destroy(void)
{
#if (MAPLE_CA_STANDAR == DTV_CA_NONE)

    if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_AUTH_TIMER_ID))
    {
        GUI_KillTimer(g_dtv_mainwin, DVB_AUTH_TIMER_ID);
    }

#endif
    return ;
}
void check_dtv_have_auth(void)
{
#if (MAPLE_CA_STANDAR == DTV_CA_NONE)

    if (service_list->servNum <= 0 && dtv_ctr.h_setting == NULL)
    {
        return ;
    }

    if (service_list->pServItems[dtv_ctr.cur_user_index].isCAFree == EPDK_FALSE && dtv_ctr.h_no_auth == NULL)
    {
        __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_SETTING_CA_NO_AUTHORIZATION};
        default_dialog(dtv_ctr.h_no_auth, g_dtv_mainwin, SUBMENU_DVB_SETTING_DLG_ID, ADLG_NOTHING, lang_id);
    }
    else if (service_list->pServItems[dtv_ctr.cur_user_index].isCAFree == EPDK_TRUE && dtv_ctr.h_no_auth != NULL)
    {
        app_dialog_destroy(dtv_ctr.h_no_auth);
        dtv_ctr.h_no_auth = NULL;
    }

#endif
    return ;
}
#if 0
void check_dtv_or_radio(void)
{
    eLIBs_printf("%s %d %s\r\n", __FILE__, __LINE__, __FUNCTION__);
    eLIBs_printf("%s %d servType:%d\r\n", __FILE__, __LINE__, service_list->pServItems[service_list->curServIdx].servType);

    if (service_list->pServItems[service_list->curServIdx].servType == MAPLE_SERV_TYPE_RADIO)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);

        if (video_layer_hdl != NULL)
        {
            __u32 arg[3];
            eLIBs_printf("%s %d %d\r\n", __FILE__, __LINE__, GUI_LyrWinGetSta(video_layer_hdl));
            GUI_LyrWinSetSta(video_layer_hdl, GUI_LYRWIN_STA_ON);
            eLIBs_printf("%s %d %d\r\n", __FILE__, __LINE__, GUI_LyrWinGetSta(video_layer_hdl));
            GUI_LyrWinSel(video_layer_hdl);
            GUI_LyrWinSetBottom(video_layer_hdl);
            GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_ctr.h_radio_program_bmp), 0, 0);
        }
    }

    return ;
}
#else
void check_dtv_or_radio(void)
{
    if (service_list->servNum <= 0)
    {
        return ;
    }

    if (service_list->pServItems[service_list->curServIdx].servType == MAPLE_SERV_TYPE_RADIO && dtv_ctr.h_radio_tip == NULL)
    {
        __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_IS_PLAYING_RADIO_PROGRAM};
        default_dialog(dtv_ctr.h_radio_tip, g_dtv_mainwin, SUBMENU_DVB_SETTING_DLG_ID, ADLG_NOTHING, lang_id);
    }
    else if (service_list->pServItems[service_list->curServIdx].servType != MAPLE_SERV_TYPE_RADIO && dtv_ctr.h_radio_tip != NULL)
    {
        app_dialog_destroy(dtv_ctr.h_radio_tip);
        dtv_ctr.h_radio_tip = NULL;
    }

    return ;
}
#endif
__s32 maple_get_freq_from_index(__s32 index)
{
    return service_list->pServItems[index].demod_pm.frequency;
}
__s32 maple_get_current_program_freq(void)
{
    return service_list->pServItems[service_list->curServIdx].demod_pm.frequency;
}
//add by hot.lee
//video_window = NULL , means  full screen
//video_window:播放区域，用于节目预览
//index: 逻辑频道号
__s32 maple_play_program_ex(RECT * video_window, __s32 index)
{
    __s32 channel;
    __u64 arg[3];
    __s32 ret;

    if (index < 0 || index > service_list->servNum - 1)
    {
        __wrn("channel< 0 \n");
        return EPDK_FAIL;
    }

    channel = service_list->pServItems[index].channelId;

    if (video_window)
    {
        video_layer_hdl = (unsigned long)maple_request_video_layer(video_window, 0, 0xff);
    }
    else
    {
        //* set video layer handle to dvbc
        RECT window_rect;
        // b_full_screen = EPDK_TRUE;
        window_rect.x       = 0;
        window_rect.y       = 0;
        window_rect.width   = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
        window_rect.height = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
        __wrn("w = %d  h = %d \n", window_rect.width, window_rect.height);
        video_layer_hdl = (unsigned long)maple_request_video_layer(&window_rect, 0, 0xff);
    }

#if (0)
    {
        __s32 i;
        __msg("service_list->servNum = %d\n", service_list->servNum);

        for (i = 0; i < service_list->servNum; i++)
        {
            __msg("vdStreamNum = %d adStreamNum=%d \n", (service_list->pServItems[i]).vdStreamNum, (service_list->pServItems[i]).adStreamNum);
            __msg("pmtId = %d channelId = %d \n", (service_list->pServItems[i]).pmtId, (service_list->pServItems[i]).channelId);
        }
    }
    __wrn("service_list->num = %d service_list->curServIdx = %d\n", service_list->servNum, service_list->curServIdx);
#endif
    esMODS_MIoctrl(maple, MAPLE_CMD_ENABLE_DEINTERLACE, 2, NULL);

    if (esMODS_MIoctrl(maple, MAPLE_CMD_PLAY, channel, NULL) != EPDK_OK)
    {
        if (video_layer_hdl)
        {
            arg[0] = video_layer_hdl;
            arg[1] = 0;
            arg[2] = 0;
            ret = esMODS_MIoctrl(mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
            __msg("DISP_CMD_LAYER_RELEASE........\n");

            if (EPDK_FAIL == ret)
            {
                __wrn("stop maple fail .......\n");
                return EPDK_FAIL;
            }

            video_layer_hdl = 0;
        }

        //if(NULL == h_play_dialog)
        {
            //  __s32 lang_id[]={ STRING_DTV_TIPS , STRING_DTV_CUR_PROGRAM_OPEN_FAIL };
            //  default_dialog(h_play_dialog,g_dtv_mainwin,DVB_PROT_DIALOG_ID,ADLG_OK,lang_id);
        }
        __wrn("play service %d fail.\n", channel);
        return EPDK_FAIL;
    }
    else
    {
        //ES_FILE* display;
        //if(h_play_dialog)
        {
            //  app_dialog_destroy(h_play_dialog);
            //  h_play_dialog = NULL;
        }
        __wrn("service id = %d.................. \n", service_list->pServItems[channel].servId);
        arg[0] = video_layer_hdl;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_OPEN, 0, (void *)arg);

        ////esMODS_MIoctrl(mp_display, DISP_CMD_LCD_ON, 0, 0); //不需要，如果用的话TVOUT时会有问题

        if (video_window)
        {
            b_full_screen = EPDK_FALSE ;
        }
        else
        {
            b_full_screen = EPDK_TRUE;
        }
    }

    maple_epg_search_stop();
    esKRNL_TimeDly(1);
    maple_epg_search();
    ///maple_time_monitor_stop();
    ///esKRNL_TimeDly(1);
    ///maple_time_monitor_start();
    return EPDK_OK;
}
__s32 maple_stop_program(void)
{
    cur_playing_index = -1;
    dtv_ctr.play_cnt = 0;

    if (service_list->servNum <= 0)
    {
        __wrn("service_list->servNum = %d \n", service_list->servNum);
        return EPDK_FAIL;
    }

    if (maple_get_play_status() == MAPLE_STAT_STOP)
    {
        return EPDK_OK;
    }

    _clearprogramname();
    //maple_epg_search_stop();
    maple_time_monitor_stop();
#if 1
    __wrn("maple_stop_program 2..\n");
    esMODS_MIoctrl(maple, MAPLE_CMD_STOP, 0, NULL);
    __wrn("maple_stop_program 3..\n");
#else

    while (i--)
    {
        ret = esMODS_MIoctrl(maple, MAPLE_CMD_STOP, 0, NULL);

        if (EPDK_FAIL == ret)
        {
            __wrn("stop maple fail .......\n");
            //return EPDK_FAIL;
            continue;
        }
        else
        {
            break;
        }
    }

    if (i == 0)
    {
        __wrn("stop maple fail .......\n");
        return EPDK_FAIL;
    }

#endif
#if 0

    if (video_layer_hdl)
    {
        arg[0] = video_layer_hdl;
        arg[1] = 0;
        arg[2] = 0;
        ret = esMODS_MIoctrl(mp_display, DISP_CMD_LAYER_RELEASE, 0, (void *)arg);

        if (EPDK_FAIL == ret)
        {
            __wrn("stop maple fail .......\n");
            return EPDK_FAIL;
        }

        video_layer_hdl = 0;
    }
    else
    {
        __msg("video_layer_hdl = NULL\n");
    }

#endif
    return EPDK_OK;
}
/************************************************************************************************************
*search epg
*
*
***********************************************************************************************************/
//* task for searching schedule
//已经废弃
static void schedule_search_task(void *arg)
{
    __u8                    err;
    schdl_list->eventNum     = 0;
    schdl_event_cbp->pOpaque = NULL;
    schdl_event_cbp->pOpaque = esKRNL_SemCreate(0);

    if (!schdl_event_cbp->pOpaque)
    {
        __wrn("schedule search semaphore create fail.\n");
        goto _schdl_search_exit;
    }

    if (schdl_list->pSchdlEvents == NULL)
    {
        __wrn("malloc fail.\n");
        goto _schdl_search_exit;
    }

    if (service_list->servNum == 0)
    {
        __wrn("no program.\n");
        goto _schdl_search_exit;
    }

    esMODS_MIoctrl(maple,
                   MAPLE_CMD_SRCH_SCHDL,
                   service_list->curServIdx,
                   (void *)schdl_list);
    //* wait a long time for search progress.
    esKRNL_SemPend(schdl_event_cbp->pOpaque, 60000, &err);

    if (err != 0)
    {
        __wrn("schedule list search fail.\n");
    }
    else
    {
        __wrn("schedule list search finish.\n");
    }

_schdl_search_exit:
    esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_SCHDL_STOP, 0, NULL);

    if (schdl_event_cbp->pOpaque)
    {
        esKRNL_SemDel(schdl_event_cbp->pOpaque, OS_DEL_ALWAYS, &err);
        schdl_event_cbp->pOpaque = NULL;
    }

    schedule_task_id = 0;
    __wrn("schedule search task exit.\n");
    esKRNL_TDel(EXEC_prioself);
}
void maple_epg_reset_schdl_list_day(__u32 serviceid)
{
    int i;
    eLIBs_memset(schdl_list_day, 0, MAX_EPG_DAY * sizeof(maple_schdl_list_day));
#if 0

    for (i = 0; i < 7; i++)
#else
    for (i = 0; i < 1; i++) //只猎取当天的节目列表
#endif
    {
        schdl_list_day[i].eventNum = 0;
        schdl_list_day[i].MJD = tv_time->MJD + i;
        schdl_list_day[i].serviceid = service_list->curServIdx;
        __msg("schdl_list_day[%d].MJD = %d \n", i, schdl_list_day[i].MJD);
    }
}
__s32 maple_epg_add_event_to_schdl(maple_schdl_event_t *event)
{
    int i;
#if 0

    for (i = 0; i < 7; i++)
#else
    for (i = 0; i < 1; i++) //只获取当天的就OK
#endif
    {
        //__msg("schdl_list_day[i].MJD = %d   event->startTime.MJD=%d\n",schdl_list_day[i].MJD,event->startTime.MJD);
#if 0
        if (schdl_list_day[i].MJD == event->startTime.MJD)
#endif
        {
            eLIBs_memcpy(schdl_list_day[i].pSchdlEvents + schdl_list_day[i].eventNum, event, sizeof(maple_schdl_event_t));
            schdl_list_day[i].eventNum += 1;
#if 1
            schdl_list_day[i].MJD = event->startTime.MJD;
#endif
            __msg("schdl_list_day[%d].eventNum = %d\n", i, schdl_list_day[i].eventNum);
            //__msg("schdl_list_day[%d].name = %s\n",i,schdl_list_day[i].pSchdlEvents[0].name);
        }
    };

    //__msg("schdl_list_day[0].eventNum = %d\n",schdl_list_day[0].eventNum);
    return 0;
}
__s32 maple_register_epg_callback(__pCBK_t cb_search_schedule_event_callback, __pCBK_t cb_search_schedule_finish_callback)
{
    __pCBK_t            callback;
    callback = esKRNL_GetCallBack(cb_search_schedule_event_callback);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET_PARAM, MAPLE_CB_SRCH_SCHDL_EVENT, (void *)NULL); //schdl_event_cbp
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET, MAPLE_CB_SRCH_SCHDL_EVENT, (void *)callback);
    callback = esKRNL_GetCallBack(cb_search_schedule_finish_callback);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET_PARAM, MAPLE_CB_SRCH_SCHDL_FINISH, (void *)NULL); //schdl_event_cbp
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET, MAPLE_CB_SRCH_SCHDL_FINISH, (void *)callback);
    __wrn("maple_register_epg_callback exit.\n");
    return EPDK_OK;
}
//maple_epg_search()之后并不会马上得到epg数据
//所以马上进入schedule界面有可能数据还是空的
__s32 maple_epg_getdata(void)
{
    int i = 0;
    maple_epg_reset_schdl_list_day(0);  //delete by hot.lee
    esMODS_MIoctrl(maple, MAPLE_CMD_GET_SCHEDULE, service_list->curServIdx, schdl_list);
    __msg("schdl_list->eventNum = %d\n", schdl_list->eventNum);

    for (i = 0; i < schdl_list->eventNum; i++)
    {
        __msg("name : %s \n", schdl_list->pSchdlEvents[i].name);
        maple_epg_add_event_to_schdl(&(schdl_list->pSchdlEvents[i]));
    }

    return EPDK_OK;
}
//获取当前正在播放的节目是一天中的第几个节目，add by hot.lee
__s32 maple_get_cur_schedule_index(__s32 day_index)
{
    __s32 i;
    char tmp[128];

    for (i = 0; i < schdl_list_day[day_index].eventNum; i++)
    {
#if 1   //endTime 都 是0，所以  //取起始时间相等就OK
        eLIBs_sprintf(tmp, "  %d:%d:%d - %d:%d:%d : ",
                      schdl_list_day[day_index].pSchdlEvents[i].startTime.hour,
                      schdl_list_day[day_index].pSchdlEvents[i].startTime.min,
                      schdl_list_day[day_index].pSchdlEvents[i].startTime.sec,
                      schdl_list_day[day_index].pSchdlEvents[i].endTime.hour,
                      schdl_list_day[day_index].pSchdlEvents[i].endTime.min,
                      schdl_list_day[day_index].pSchdlEvents[i].endTime.sec);
        __msg("tmp = %s  sepg_string_c = %s\n", tmp, sepg_string_c);

        if (eLIBs_strncmp(tmp, sepg_string_c, 10) == 0) //取起始时间相等就OK
#else
        if (sepg->curEventstartTime.hour == schdl_list_day[day_index].pSchdlEvents[i].startTime.hour
            || sepg->curEventstartTime.min == schdl_list_day[day_index].pSchdlEvents[i].startTime.min
            || sepg->curEventstartTime.sec == schdl_list_day[day_index].pSchdlEvents[i].startTime.sec)
#endif
        {
            __msg("i = %d\n", i);
            return i;
        }
    }

    return 0;
}
//获取某一天的event num
__s32 maple_epg_get_eventnum(__s32 day_index)
{
    __msg("schdl_list_day[%d].eventNum = %d\n", day_index, schdl_list_day[day_index].eventNum);
    return schdl_list_day[day_index].eventNum;
}
//获取某一天指定event index的schdl events
maple_schdl_event_t *maple_epg_get_schdl_event(__s32 day_index, __s32 event_index)
{
    return &schdl_list_day[day_index].pSchdlEvents[event_index];
}
__s32 maple_epg_search(void)
{
    __s32 ret;
    //return EPDK_OK;// modify140325
    __msg(".maple_epg_search\n");
    maple_epg_reset_schdl_list_day(0);
    // ret = esMODS_MIoctrl(maple, MAPLE_CMD_EPG_MONITOR_START, 0, NULL);
    ret = esMODS_MIoctrl(maple, MAPLE_CMD_EPG_MONITOR_START, service_list->curServIdx, NULL);
    __msg("ret = %d \n", ret);
    return ret;
}
__s32 maple_epg_search_stop(void)
{
    //return EPDK_OK;// modify140325
    esMODS_MIoctrl(maple, MAPLE_CMD_EPG_MONITOR_STOP, 0, NULL);
    return EPDK_OK;
}
void maple_time_monitor_start(void)
{
    return ;// modify140325
    esMODS_MIoctrl(maple, MAPLE_CMD_TIME_MONITOR_START, 0, NULL);
}
void maple_time_monitor_stop(void)
{
    return ;// modify140325
    esMODS_MIoctrl(maple, MAPLE_CMD_TIME_MONITOR_STOP, 0, NULL);
}
__s32 maple_time_monitor_get(maple_time_t *tv_time)
{
    return EPDK_OK;// modify140325
    return esMODS_MIoctrl(maple, MAPLE_CMD_TIME_GET, 0, tv_time);
}
//add by hot.lee
//获取当前台数
__s32 maple_get_cur_service_list_num(void)
{
    return service_list->servNum;
}
//add by hot.lee
//获取当前播放节目的逻辑频道号
__s32 maple_get_cur_program_preview_index(void)
{
    __s32 i;

    for (i = 0; i < service_list->servNum; i++)
    {
        if (service_list->curServIdx == service_list->pServItems[i].channelId)
        {
            return i;
        }
    }

    return 0;
}
//根据0 1 2 3 等设置curServIdx
//根据逻辑频道号去播放节目，更符合使用习惯,add by hot.lee
__s32  maple_set_cur_channel(__s32 index)
{
    if (index < 0)
    {
        index = 0;
    }

    if (index > service_list->servNum - 1)
    {
        __msg("index(%d) > service_list->servNum(%d)\n", index);
        return EPDK_FAIL;
    }

    __msg("index=%d  service_list->curServIdx=%d\n", index, service_list->curServIdx);
    service_list->curServIdx = service_list->pServItems[index].channelId;
    return EPDK_OK;
}
__s32  maple_get_cur_channel(__s32 index)
{
    if (index > service_list->servNum - 1)
    {
        return EPDK_FAIL;
    }

    service_list->curServIdx = service_list->pServItems[index].channelId;
    return service_list->curServIdx;
}
// 获取频道名称
//根据逻辑频道号，获取频道名称,add by hot.lee
char  *maple_get_name_of_channel(__s32 index)
{
    if (index >= service_list->servNum  || index < 0)
    {
        //__wrn("index = %d service_list->servNum=%d\n",index,service_list->servNum);
        return NULL;
    }

    //    __wrn("NUM:%d NAME:%s \n",index,service_list->pServItems[index].servName);
    return service_list->pServItems[index].servName;
}
// 获取频道是否加锁
//根据逻辑频道号，获取频道是否加锁标志 //add by shiql
__u8 maple_get_server_type_of_channel(__s32 index)
{
    if (index >= service_list->servNum  || index < 0)
    {
        return EPDK_FALSE;
    }

    // eLIBs_printf("NUM:%d servType:%d \n",index,service_list->pServItems[index].servType);
    return service_list->pServItems[index].servType;
}
// 获取频道是否加锁
//根据逻辑频道号，获取频道是否加锁标志 //add by shiql
__u8 maple_get_cafree_status_of_channel(__s32 index)
{
    if (index >= service_list->servNum  || index < 0)
    {
        return EPDK_FALSE;
    }

    // eLIBs_printf("NUM:%d Locked Status:%d \n",index,service_list->pServItems[index].isCAFree);
    return service_list->pServItems[index].isCAFree;
}
//add by hot.lee
char *maple_get_channel_of_curprogramname(void)
{
    return sepg_string_c;
}
//add by hot.lee
char *maple_get_channel_of_nextprogramname(void)
{
    return sepg_string_n;
}
//add by hot.lee
static void _clearprogramname(void)
{
    eLIBs_memset(sepg_string_c, 0, MAX_SEPG_STRING);
    eLIBs_memset(sepg_string_n, 0, MAX_SEPG_STRING);
}
//* task for serching simple epg
static void sepg_search_task(void *arg)
{
    __u8                    err;
    sepg_finish_cbp->pOpaque = esKRNL_SemCreate(0);

    if (!sepg_finish_cbp->pOpaque)
    {
        __wrn("sepg search semaphore create fail.\n");
        goto _sepg_search_exit;
    }

    if (service_list->servNum == 0)
    {
        __wrn("no program.\n");
        goto _sepg_search_exit;
    }

    esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_SEPG, service_list->curServIdx, (void *)sepg);
    //* wait a long time for search progress.
    esKRNL_SemPend(sepg_finish_cbp->pOpaque, 60000, &err);

    if (err != 0)
    {
        __wrn("sepg search fail.\n");
    }
    else
    {
        __wrn("sepg search finish.\n");
    }

_sepg_search_exit:
    esMODS_MIoctrl(maple, MAPLE_CMD_SRCH_SEPG_STOP, 0, NULL);

    if (sepg_finish_cbp->pOpaque)
    {
        esKRNL_SemDel(sepg_finish_cbp->pOpaque, OS_DEL_ALWAYS, &err);
        sepg_finish_cbp->pOpaque = NULL;
    }

    sepg_task_id = 0;
    __wrn("sepg search task exit.\n");
    esKRNL_TDel(EXEC_prioself);
}
__s32 maple_register_sepg_callback(__pCBK_t search_sepg_finish_callback)
{
    __pCBK_t            callback;
    callback = esKRNL_GetCallBack(search_sepg_finish_callback);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET_PARAM, MAPLE_CB_SRCH_SEPG_FINISH, (void *)sepg_finish_cbp);
    esMODS_MIoctrl(maple, MAPLE_CMD_CB_SET, MAPLE_CB_SRCH_SEPG_FINISH, (void *)callback);
    __wrn("maple_register_epg_callback exit.\n");
    return EPDK_OK;
}
__s32 maple_sepg_clean(void)
{
    // eLIBs_memset(sepg_string_c,0,MAX_SEPG_STRING);
    // eLIBs_memset(sepg_string_n,0,MAX_SEPG_STRING);
    sepg_string_c[0] = NULL;
    sepg_string_n[0] = NULL;
    return 0;
}
__s32 maple_sepg_search(void)
{
    int hassepg = 0;
    int i = 0;

    for (i = 0; i < 3; i++)
    {
        esKRNL_TimeDly(10);
        esMODS_MIoctrl(maple, MAPLE_CMD_GET_SEPG, service_list->curServIdx, (void *)sepg);

        if (sepg->hasCurEventInfo || sepg->hasNextEventInfo)
        {
            break;
        }
    }

    eLIBs_memset(sepg_string_c, 0, MAX_SEPG_STRING);
    eLIBs_memset(sepg_string_n, 0, MAX_SEPG_STRING);

    if (sepg->hasCurEventInfo)
    {
        eLIBs_sprintf(sepg_string_c, "  %d:%d:%d - %d:%d:%d : ",
                      sepg->curEventstartTime.hour,
                      sepg->curEventstartTime.min,
                      sepg->curEventstartTime.sec,
                      sepg->curEventEndTime.hour,
                      sepg->curEventEndTime.min,
                      sepg->curEventEndTime.sec);
        strcat(sepg_string_c, (char *)sepg->curEventName);
        hassepg = 1;
    }
    else
    {
        __wrn(" ======== \n");
    }

    __msg("sepg_string_c = %s\n", sepg_string_c);

    if (sepg->hasNextEventInfo)
    {
        eLIBs_sprintf(sepg_string_n, "  %d:%d:%d - %d:%d:%d :",
                      sepg->nextEventstartTime.hour,
                      sepg->nextEventstartTime.min,
                      sepg->nextEventstartTime.sec,
                      sepg->nextEventEndTime.hour,
                      sepg->nextEventEndTime.min,
                      sepg->nextEventEndTime.sec);
        eLIBs_strcat(sepg_string_n, sepg->nextEventName);
        hassepg = 1;
    }
    else
    {
        __wrn(" ======== \n");
    }

    __msg("sepg_string_c = %s\n", sepg_string_n);
    return hassepg;
}
__s32 maple_get_current_av_info(void)
{
    __s32 ret = 0;

    if (maple)
    {
        ret = esMODS_MIoctrl(maple, MAPLE_CMD_GET_SERVICE_INFO, 0, NULL);
    }

    __msg("ret = %d \n", ret);
    return ret;
}
#if 0
__s32 maple_sepg_search_stop(void)
{
    if (sepg_task_id != 0)
    {
        esKRNL_TimeDlyResume(sepg_task_id);

        while (esKRNL_TDelReq(sepg_task_id) != OS_TASK_NOT_EXIST)
        {
            esKRNL_TimeDlyResume(sepg_task_id);
            esKRNL_TimeDly(1);
        }

        sepg_task_id = 0;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}
__s32 maple_get_ss_status(__s32 * strength, __s32 * quality)
{
    __wrn("get_ss_status: S: %d, Q: %d\n", strength, quality);
    return 0;
}
#endif
//对于CMMB此函数只能在信号检测定时器里调用
//其它要获取信号状态的地方，请用全局变量获取
//因为MAPLE_CMD_GET_SS每次都会从通道里获取数据
//需要防止线程访问冲突造成的死锁
__s32   maple_get_ss(maple_demod_ss_t *ss)
{
    if (esMODS_MIoctrl(maple, MAPLE_CMD_GET_SS, 0, (void *)ss) != 0)
    {
        ss->strength = 0;
        ss->quality = 0;
    }

    __wrn("GET:S: %d, Q: %d\n", ss->strength, ss->quality);
    //ss->strength /= 20;//will display number
    return EPDK_OK;
}
__u8  maple_control_get_lock_status(void)
{
    __u8 ret;
    ret = esMODS_MIoctrl(maple, MAPLE_CMD_GET_LOCK_STATUS, 0, 0);
    __wrn(" MAPLE_CMD_GET_LOCK_STATUS ======= %d \n", ret);

    /*after reset to factory setting, clear service , no channel ,
    but tune freq and locked, so if channle num <= 0, then return no locked*/

    if (maple_get_cur_service_list_num() <= 0)
    {
        __wrn("*******service num <= 0***********\n");
        ret = FALSE;
    }

    return ret;
}
//检测解码状态，add by hot.lee
//只用在定时器检测线程中
maple_ve_status_e *maple_get_decode_status(void)
{
    signed long result;
    maple_ve_status_e *maple_ve_status;
    result = esMODS_MIoctrl(maple, MAPLE_CMD_GET_VE_STATUS, 0, NULL);
    maple_ve_status = (maple_ve_status_e *)result;
    return maple_ve_status;
}
//现在cedar与maple合并，需要设置TV模式
__s32 maple_set_tv_mode(__u8 flag)
{
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_TV_MODE, flag, NULL);
}
__s32 maple_save_reg_current_program(void)
{
#if 0
    __hdle   h_node = NULL;
    char     key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __u32 current_program_no;
    current_program_no = service_list->curServIdx;
    h_node = eLIBs_RegOpenNode("\\dtv\\dtv", ELIBS_REG_READWRITE);

    if (h_node == NULL)
    {
        __wrn("reg node cannot be opened ! \n");
        return EPDK_FAIL;
    }

    key_value[0] = current_program_no / 1000 + 0x30;
    key_value[1] = (current_program_no % 1000) / 100 + 0x30;
    key_value[2] = (current_program_no % 100) / 10 + 0x30;
    key_value[3] = (current_program_no % 10) + 0x30;
    key_value[4] = '\0';
    eLIBs_RegSetKeyValue(h_node, "config", "current_program_no", key_value);
    eLIBs_RegCloseNode(h_node);
#else
    //  reg_cmmb_para_t *cmmb_para;
    //  cmmb_para = dsk_reg_get_para_by_app(REG_APP_CMMB);

    //搜索到频道后，只要大于0，就覆盖之前的
    if (service_list->servNum > 0)
    {
        //__s32 i;
        __s32 ret;
        cmmb_para->flag = EPDK_TRUE;
        cmmb_para->index = dtv_ctr.cur_user_index;
        //ret = dsk_reg_flush();
        //__msg("dsk_reg_flush ret = %d\n",ret);
        /*eLIBs_memcpy(cmmb_para->service_list,service_list,sizeof(maple_serv_list_t));

        for(i=0; i<service_list->servNum; i++)
        {
            eLIBs_memcpy(cmmb_para->service_item[i],&service_list->pServItems[i],sizeof(maple_serv_item_t));
        }   */
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
        cmmb_para->b_show_subtitle = dtv_has_subtitle();
#endif
    }
    else
    {
        __msg("search result :channel = 0\n");
    }

#endif
    return EPDK_OK;
}
__s32 maple_load_reg_current_program(void)
{
#if 0
    __hdle   h_node = NULL;
    char     key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __u32    program_no = 0;
#if 1
    h_node = eLIBs_RegOpenNode("\\dtv\\dtv", ELIBS_REG_READWRITE);

    if (h_node == NULL)
    {
        __wrn("reg node cannot be opened! \n");
        return EPDK_FAIL;
    }

    if (eLIBs_RegGetKeyValue(h_node, "config", "current_program_no", key_value) == EPDK_FAIL)
    {
        eLIBs_RegCloseNode(h_node);
        return EPDK_FAIL;
    }

    program_no = (key_value[0] - 0x30) * 1000 + (key_value[1] - 0x30) * 100 + (key_value[2] - 0x30) * 10 + (key_value[3] - 0x30);
    service_list->curServIdx = program_no;
    eLIBs_RegCloseNode(h_node);
    return program_no;
#endif
#else
#if 0
    reg_cmmb_para_t *cmmb_para;
    cmmb_para = dsk_reg_get_para_by_app(REG_APP_CMMB);

    if (cmmb_para->flag == EPDK_TRUE)
    {
        __s32 i;
        esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
        eLIBs_memcpy(service_list, cmmb_para->service_list, sizeof(maple_serv_list_t));

        for (i = 0; i < service_list->servNum; i++)
        {
            eLIBs_memcpy(&service_list->pServItems[i], cmmb_para->service_item[i], sizeof(maple_serv_item_t));
        }

        if (service_list->servNum > 0)
        {
            esMODS_MIoctrl(maple, MAPLE_CMD_SL_ADD, 0, (void *)service_list);
        }

        maple_service_list_get(MAPLE_SERV_GROUP_TYPE_ALL);
#if (0)
        {
            __s32 i;

            for (i = 0; i < service_list->servNum; i++)
            {
                __msg("%s  channelId = %d\n", service_list->pServItems[i].servName, service_list->pServItems[i].channelId);
            }
        }
#endif
    }
    else
    {
        return EPDK_FAIL;
    }

#endif
#endif
    return EPDK_OK;
}
void maple_set_video_window_ratio_mode(maple_vd_win_ratio_mode_e radio_mode)
{
    mode = radio_mode;
}
__s32  maple_get_video_window_ratio_mode(void)
{
    return maple_get_screensize();
}
__s32 maple_save_reg_video_window_ratio_mode(void)
{
#if 0
    __hdle                  h_node;
    char     key_value[ ELIBS_KEY_VALUE_LEN + 1];
    h_node = eLIBs_RegOpenNode("\\dtv\\dtv", ELIBS_REG_READWRITE);

    if (h_node == NULL)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    if (mode == MAPLE_VID_WINDOW_NOTCARE)
    {
        eLIBs_strcpy(key_value, "notcare");
    }
    else if (mode == MAPLE_VID_WINDOW_ORIGINAL)
    {
        eLIBs_strcpy(key_value, "original");
    }
    else if (mode == MAPLE_VID_WINDOW_BESTSHOW)
    {
        eLIBs_strcpy(key_value, "bestshow");
    }
    else if (mode == MAPLE_VID_WINDOW_FULLSCN)
    {
        eLIBs_strcpy(key_value, "fullscn");
    }
    else if (mode == MAPLE_VID_WINDOW_4R3MODE)
    {
        eLIBs_strcpy(key_value, "4r3mode");
    }
    else if (mode == MAPLE_VID_WINDOW_16R9MODE)
    {
        eLIBs_strcpy(key_value, "16r9mode");
    }
    else
    {
        eLIBs_strcpy(key_value, "bestshow");
    }

    eLIBs_RegSetKeyValue(h_node, "config", "vedio_window_ratio_mode", key_value);
    eLIBs_RegCloseNode(h_node);
#else
    //reg_cmmb_para_t    *cmmb_para;
    //cmmb_para = dsk_reg_get_para_by_app(REG_APP_CMMB);
    cmmb_para->mode = mode;
    //   __msg("cmmb_para->mode <= %d\n", mode);
    //mode = MAPLE_VID_WINDOW_4R3MODE;
#endif
    return EPDK_OK;
}
__s32 maple_load_reg_video_window_ratio_mode(void)
{
    __hdle                  h_node;
    char     key_value[ ELIBS_KEY_VALUE_LEN + 1];
    maple_vd_win_ratio_mode_e         video_window_ratio_modex;
#if 0
    h_node = eLIBs_RegOpenNode("\\dtv\\dtv", ELIBS_REG_READWRITE);

    if (!h_node)
    {
        __wrn("reg node cannot be opened!\n");
        video_window_ratio_modex = MAPLE_VID_WINDOW_BESTSHOW;
    }
    else
    {
        eLIBs_RegGetKeyValue(h_node, "config", "vedio_window_ratio_mode", key_value);

        if (!eLIBs_strcmp(key_value, "notcare"))
        {
            video_window_ratio_modex = MAPLE_VID_WINDOW_NOTCARE;
        }
        else if (!eLIBs_strcmp(key_value, "original"))
        {
            video_window_ratio_modex = MAPLE_VID_WINDOW_ORIGINAL;
        }
        else if (!eLIBs_strcmp(key_value, "bestshow"))
        {
            video_window_ratio_modex = MAPLE_VID_WINDOW_BESTSHOW;
        }
        else if (!eLIBs_strcmp(key_value, "fullscn"))
        {
            video_window_ratio_modex = MAPLE_VID_WINDOW_FULLSCN;
        }
        else if (!eLIBs_strcmp(key_value, "4r3mode"))
        {
            video_window_ratio_modex = MAPLE_VID_WINDOW_4R3MODE;
        }
        else if (!eLIBs_strcmp(key_value, "16r9mode"))
        {
            video_window_ratio_modex = MAPLE_VID_WINDOW_16R9MODE;
        }
        else
        {
            __wrn("video window ratio mode not load!\n");
            video_window_ratio_modex = MAPLE_VID_WINDOW_BESTSHOW;
        }
    }

    eLIBs_RegCloseNode(h_node);
#else
    //reg_cmmb_para_t    *cmmb_para;
    // cmmb_para = dsk_reg_get_para_by_app(REG_APP_CMMB);
    mode = MAPLE_VID_WINDOW_FULLSCN;//cmmb_para->mode%MAPLE_VID_WINDOW_;
    video_window_ratio_modex = mode;
    //video_window_ratio_modex = MAPLE_VID_WINDOW_BESTSHOW;
#endif
    return video_window_ratio_modex;
}
__s32 maple_inc_light(void)
{
    __s32 light;
    light = dsk_display_get_lcd_bright() / 10;
    light = light + 1;

    if (light <= MAX_LIGHT)
    {
        dsk_display_set_lcd_bright(light * 10);
        return light;
    }
    else
    {
        return MAX_LIGHT;
    }
}
__s32 maple_dec_light(void)
{
    __s32 light;
    light = dsk_display_get_lcd_bright() / 10;
    light = light - 1;

    if (light >= 0)
    {
        dsk_display_set_lcd_bright(light * 10);
        return light;
    }
    else
    {
        return 0;
    }
}
__s32 maple_get_light(void)
{
    return dsk_display_get_lcd_bright() / 10;
}
__s32 maple_inc_volume(void)
{
    dsk_volume_inc();
    return 0;
}
__s32 maple_dec_volume(void)
{
    dsk_volume_dec();
    return 0;
}
__s32 maple_get_volume(void)
{
    return dsk_volume_get();
}
extern __s32 get_next_mode(__s32 mode_id);
__s32 change_window_ratio(void)
{
    __msg("last mode => %d\n", mode);
    mode = get_next_mode(mode);
    __msg("current mode => %d\n", mode);
    maple_save_reg_video_window_ratio_mode();
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_VID_SHOW_MODE, mode, NULL);
}
__s32 maple_set_screensize(maple_vd_win_ratio_mode_e usr_mode)
{
    //mode = mode + 1;
    if (usr_mode > MAPLE_VID_WINDOW_16R9MODE)
    {
        mode = MAPLE_VID_WINDOW_NOTCARE;
    }
    else
    {
        mode = usr_mode;
    }

    maple_save_reg_video_window_ratio_mode();
    //__here__;
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_VID_SHOW_MODE, usr_mode, NULL);
}
maple_vd_win_ratio_mode_e maple_get_screensize(void)
{
    //__here__;
    return mode;//esMODS_MIoctrl(maple, MAPLE_CMD_GET_VID_SHOW_MODE, 0, NULL);
}
__s32 maple_set_init_screensize(void)
{
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_VID_SHOW_MODE, maple_load_reg_video_window_ratio_mode(), NULL);
}
__s32 maple_set_area(__u32 area)
{
    __hdle   h_node = NULL;
    char     key_value[ ELIBS_KEY_VALUE_LEN + 1];
    G_DTV_current_area = area;
    h_node = eLIBs_RegOpenNode("\\dtv\\dtv", ELIBS_REG_READWRITE);

    if (h_node == NULL)
    {
        __wrn("reg node cannot be opened ! \n");
        return EPDK_FAIL;
    }

    key_value[0] = G_DTV_current_area / 1000 + 0x30;
    key_value[1] = (G_DTV_current_area % 1000) / 100 + 0x30;
    key_value[2] = (G_DTV_current_area % 100) / 10 + 0x30;
    key_value[3] = (G_DTV_current_area % 10) + 0x30;
    key_value[4] = '\0';
    eLIBs_RegSetKeyValue(h_node, "config", "area", key_value);
    eLIBs_RegCloseNode(h_node);
    return G_DTV_current_area;
}
__s32 maple_get_area(void)
{
    __hdle   h_node = NULL;
    char     key_value[ ELIBS_KEY_VALUE_LEN + 1];
    h_node = eLIBs_RegOpenNode("\\dtv\\dtv", ELIBS_REG_READWRITE);

    if (h_node == NULL)
    {
        __wrn("reg node cannot be opened! \n");
        return EPDK_FAIL;
    }

    if (eLIBs_RegGetKeyValue(h_node, "config", "area", key_value) == EPDK_FAIL)
    {
        eLIBs_RegCloseNode(h_node);
        return EPDK_FAIL;
    }

    G_DTV_current_area = (key_value[0] - 0x30) * 1000 + (key_value[1] - 0x30) * 100 + (key_value[2] - 0x30) * 10 + (key_value[3] - 0x30);
    eLIBs_RegCloseNode(h_node);
    return G_DTV_current_area;
}
__s32 maple_set_programtype(__u32 programtype)
{
    g_programtype = programtype;
    return g_programtype;
}
__s32 maple_clear_database(char *database_file_name)
{
    esMODS_MIoctrl(maple, MAPLE_CMD_SL_DELETE, 0, NULL);
    maple_program_save(database_file_name);
    return EPDK_OK;
}
__u32 ymd2mjd(__u32 year, __u32 month, __u32 date)
{
    __u32 mjd;
    __u32 L;
    __u32 tmp;

    if (year < 1900)
    {
        return 0;
    }

    year -= 1900;

    if (month == 1 || month == 2)
    {
        L = 1;
    }
    else
    {
        L = 0;
    }

    mjd = 14956 + date;
    tmp = (year - L) * 36525;
    tmp /= 100;
    mjd += tmp;
    tmp = (month + 1 + L * 12) * 306001;
    tmp /= 10000;
    mjd += tmp;
    return mjd;
}
/**********************************************************************************
**********************************************************************************/
void mjd2ymd(__u32 MJD, __u32 * pYear, __u32 * pMonth, __u32 * pDate)
{
    __u32 yearVal;
    __u32 monthVal;
    __u32 dayVal;
    __u32 k;
    yearVal  = (__u16)((MJD * 100 - 1507820) / 36525);
    monthVal = (__u16)((MJD * 10000 - 149561000 - (yearVal * 3652500)) / 306001);
    dayVal   = (__u16)(MJD - 14956 - (yearVal * 3652500) / 10000 - (monthVal * 306001) / 10000);
    k = ((monthVal == 14) || (monthVal == 15)) ? 1 : 0;
    (*pYear)   = yearVal + k + 1900;
    (*pMonth)  = (__u32)(monthVal - 1 - k * 12);
    (*pDate)   = (__u32) dayVal;
}
/********************************************************************************
********************************************************************************/
void getweekday(__u32 year, __u32 month, __u32 day, __u32 * pWeekDay)
{
    __u32 century;
    __s32 w;
    century = 0;
    w       = 0;

    if (month < 3)
    {
        month += 12;
        year  -= 1;
    }

    century =  year / 100;
    year    -= century * 100;
    w = year + (year >> 2) + (century >> 2) - (century << 1) + (26 * (month + 1) / 10) + day - 1;
    w %= 7;

    if (w < 0)
    {
        w += 7;
    }

    *pWeekDay = w;
}
//设置屏幕旋转角度
__s32 maple_set_rotate(__s32  ratate)
{
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_ROTATE, ratate, NULL);
}
/***********************************************************************************
*************************************************************************************/
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
maple_cas_param_t *maple_ca_get_sn(void)
{
    __s32 ret;
    ret = esMODS_MIoctrl(maple, MAPLE_CMD_CA_GET_SN, MAX_TMP_STRING, ca_sn);

    if (EPDK_OK == ret)
    {
        __msg("ca_sn  = %x\n", ca_sn);
        return ca_sn;
    }

    //__here__;
    return NULL;
}
#endif
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
//只有isdbt有字幕，CMMB没有,add by hot.lee
__s32 maple_set_subtitle_cb(__pCBK_t isdbt_cb)
{
    __pCBK_t            callback;
    callback = esKRNL_GetCallBack(isdbt_cb);
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_SPLY_DRAW_CB, NULL, (void *)callback);
}
//字幕解码类型
__s32 maple_set_default_char_coding_type(void)
{
    return esMODS_MIoctrl(maple, MAPLE_CMD_SET_DEFAULT_CHAR_CODING_TYPE, g_type, NULL);
}
#endif
