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
#ifndef __RECORD_LIST_VIEW_DOC_H__
#define __RECORD_LIST_VIEW_DOC_H__


#include "record_ui.h"

#define RECORD_MUSIC_NAME_HEIGHT    38//19
#define RECORD_MUSIC_LIST_NUM       5//10   //RECORD_MUSIC_NAME_HEIGHT*RECORD_MUSIC_LIST_NUM < 192

#define RECORD_LIST_SPCTRM_LEVEL    20

#define REC_LIST_TIMER_TIMES        80
#define VOLUME_EXIST_TIMES          (10*100/REC_LIST_TIMER_TIMES)       //10s


#define RECORD_LIST_FILTER                  ".mp3|.wav|.wma"

typedef struct tag_record_list_doc
{
    __s32   curRootType;
    char    curSearchPath[1024];

    __s32   total;
    __s32   curIndex;
    __s32   playIndex;
    __s32   topIndex;

    __u16   oldSpctrmLevel[RECORD_LIST_SPCTRM_LEVEL];
    __u16   spctrmLevel[RECORD_LIST_SPCTRM_LEVEL];

    __cedar_status_t    playSt;

    __u32   musicTimeID;
    __u32   musicMaxTimes;
    __u32   musicCurTimes;

    HRAT    rat_handle;

} record_list_doc_t, *precord_list_doc_t;







//Rat 初始化
extern __s32 record_list_MusicInit(precord_list_doc_t precord_list_doc);
//rat release
extern __s32 record_list_MusciUninit(precord_list_doc_t precord_list_doc);

extern __s32 record_list_getFileName(precord_list_doc_t precord_list_doc, __s32 index, char *name);
extern __s32 record_list_getFilePath(precord_list_doc_t precord_list_doc, __s32 index, char *name);
extern __s32 record_list_play(precord_list_doc_t precord_list_doc);
extern __s32 record_list_playNext(precord_list_doc_t precord_list_doc);
extern __s32 record_list_continue(precord_list_doc_t precord_list_doc);
extern __s32 record_list_pause(precord_list_doc_t precord_list_doc);
extern __s32 record_list_next(precord_list_doc_t precord_list_doc);
extern __s32 record_list_pre(precord_list_doc_t precord_list_doc);
extern __s32 record_list_backward(precord_list_doc_t precord_list_doc);
extern __s32 record_list_forward(precord_list_doc_t precord_list_doc);
extern __s32 record_list_delete(precord_list_doc_t precord_list_doc);
extern __s32 record_list_timer(precord_list_doc_t precord_list_doc);
















#endif//__RECORD_LIST_VIEW_DOC_H__
