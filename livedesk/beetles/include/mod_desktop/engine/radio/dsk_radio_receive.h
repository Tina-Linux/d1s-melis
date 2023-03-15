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
#ifndef __DSK_RADIO_REC_I_H__
#define __DSK_RADIO_REC_I_H__

#include "apps.h"
#include <kconfig.h>

#define CHANNEL_NAME_LEN    128

typedef struct
{
    char name[CHANNEL_NAME_LEN];
    unsigned long freq;
} dsk_radio_chan_t;

typedef enum
{
    PLAY_LIST_SEARCH_RESULT = 0,
    PLAY_LIST_FAVORITE
} __play_list_type_e;

extern __pCBK_t volatile    cb_search_success;
extern void    *volatile    cb_ctx_search_success;

extern __pCBK_t volatile    cb_search_fail;
extern void    *volatile    cb_ctx_search_fail;

extern __pCBK_t volatile    cb_search_over;
extern void *volatile       cb_ctx_search_over;

/*手动搜索时,波段为FM_APP_BAND_JAPAN的搜索步长*/
#define LARK_SEARCH_STEP_JAPAN 50
/*手动搜索时,波段为FM_APP_BAND_US_EUP的搜索步长*/
#define LARK_SEARCH_STEP_US  100
#define CHANNEL_SIZE (CHANNEL_NAME_LEN+14)

extern __pCBK_t volatile  cb_search_freq;
extern void    *volatile  cb_ctx_search_freq;

__s32 dsk_radio_rcv_set_search_cb(__pCBK_t cb, void *ctx);              //add by libaiao

__s32 dsk_radio_rcv_search_stop(void);                                  //add by Kingvan

__u16 dek_radio_set_FM1_3_ID(__s16 fm1_3_id);                            //add by Kingvan

__s32 dek_radio_set_FM1_3(__u16 fm1_3_id, __u16 fm_index, __u32 freq);  //add by Kingvan

__u32 dek_radio_get_FM1_3(__u16 fm1_3_id, __u16 fm_index);           //add by Kingvan

__s32 dsk_radio_get_search_flag(__u32 *search_flag);                    //add by cky

__s32 dsk_radio_set_search_flag(__u32 search_flag);             //add by cky

__s32 dsk_radio_rcv_save_cur_freq(void);

__s32 dsk_radio_rcv_open(void);

__bool dsk_radio_rcv_is_open(void);

__s32 dsk_radio_rcv_set_freq_play(signed long freq);

__s32 dsk_radio_rcv_auto_search(void);

__s32 dsk_radio_rcv_get_cur_freq(__u32 *freq);

__s32 dsk_radio_rcv_set_search_mode(__u32 search_mode);

__s32 dsk_radio_set_manual_way(__u32 manual_way);                       //add by Kingvan

__s32 dsk_radio_set_automanual_mode(__u32 search_mode);

//__s32 dsk_radio_rcv_search_all(void);
__s32 dsk_radio_rcv_search_all(__u32 freq_cur, __s32 channel_cur);

__s32 dsk_radio_rcv_next_freq_play(void);

__s32 dsk_radio_rcv_pre_freq_play(void);

__s32 dsk_radio_rcv_set_play_list_type(__play_list_type_e play_list_type);

__s32 dsk_radio_rcv_get_play_list_type(__s32 *play_list_type);

__s32 dsk_radio_rcv_get_search_result_vn(void);

__s32 dsk_radio_rcv_get_favorite_vn(__s32 *valid_num);

__s32 dsk_radio_rcv_get_search_result_chaninfo(__s32 id, dsk_radio_chan_t *chan_info);

__s32 dsk_radio_rcv_get_favorite_chaninfo(__s32 id, dsk_radio_chan_t *chan_info);

__s32 dsk_radio_rcv_get_favorite(char *file_name);

__s32 dsk_radio_rcv_save_favorite(char *file_name);

//__s32 dsk_radio_rcv_get_search_result(char *file_name);
__s32 dsk_radio_rcv_get_search_result(reg_fm_para_t *para);

//__s32 dsk_radio_rcv_save_search_result(char *file_name);
__s32 dsk_radio_rcv_save_search_result(void);

__s32 dsk_radio_rcv_add_chan_to_favorite(__u32 id, dsk_radio_chan_t *chan_info);

__s32 dsk_radio_rcv_del_chan_from_favorite(__s32 id);

__s32 dsk_radio_rcv_close(void);


#endif
