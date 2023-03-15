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
#ifndef __RECORD_VIEW_DOC_H__
#define __RECORD_VIEW_DOC_H__

#include <emodules/mod_ginkgo.h>
#include "record_ui.h"

#define RECORD_FILTER                   ".mp3|.wav|.wma"

typedef enum RECORD_STATE
{
    RECORD_START,
    RECORD_PAUSE,
    RECORD_STOP

} RECORD_STATE;

typedef enum RECORD_AUDIO_TYPE
{
    RECORD_MP3,
    RECORD_WAV,
    //RECORD_WMA,
    RECORD_AUDIO_TYPE_MAX

} RECORD_AUDIO_TYPE;

typedef enum RECORD_TYPE
{
    RECORD_MIC,
    RECORD_FM
} RECORD_TYPE;

#define RECORD_SPCTRM_LEVEL 32

#define NO_FILES_TIMES      6

typedef struct tag_record_doc
{
    __s32           curRootType;
    char            curSearchPath[1024];
    char            curSearchDisk[10];

    __u32           fileSize;               //单位KB
    __u32           diskFreeTime;           //单位秒
    __u64           diskFreeSize;

    RECORD_STATE    recState;               //录音状态
    __u32           recordMaxTimes;         //最大可录音时间
    __u32           recordCurTimes;         //当前录音时间

    char    recordSrc[10];
    char    recordName[25];                 //录音文件名
    char    recordFilePath[1024];               //录音文件路径

    __u32                   mid_ginkgo;     //模块
    __mp                    *mod_ginkgo;    //模块
    __krnl_event_t          *msg_queue;     // 返馈消息队列
    __u16                   oldSpctrmLevel[RECORD_SPCTRM_LEVEL];//录音频谱
    __u16                   spctrmLevel[RECORD_SPCTRM_LEVEL];   //录音频谱
    __ginkgo_media_file_inf media_file;     // 录制文件信息
    RECORD_AUDIO_TYPE       audioType;      //.mp3 or wav
    RECORD_AUDIO_TYPE       audioTypeTemp;  //临时保存.mp3 or wav
    RECORD_TYPE             recordType;     //mic or FM
    QUALITY_STATE           recQuality;     //录音质量
    QUALITY_STATE           recQualityTemp; //临时保存录音质量

    __u32           timeCnt;
    __u32           existFileCnt;

} record_doc_t, *precord_doc_t;

extern __s32 record_init(precord_doc_t precord_doc);
extern __s32 record_unInit(precord_doc_t precord_doc);
extern __u32 record_get_files(precord_doc_t precord_doc);
extern __s32 record_start(precord_doc_t precord_doc);
extern __s32 record_pause(precord_doc_t precord_doc);
extern __s32 record_continue(precord_doc_t precord_doc);
extern __s32 record_stop(precord_doc_t precord_doc, __s32 bSave);
extern __s32 record_updateCurTime(precord_doc_t precord_doc);

extern void record_updateFreeTime(precord_doc_t precord_doc);











#endif//__RECORD_VIEW_DOC_H__

