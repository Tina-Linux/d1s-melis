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
#ifndef __JT_TTS_H__
#define __JT_TTS_H__

#include <emodules/mod_defs.h>
#include "eJTTS.h"
#include "app_ebook_i.h"


#define MAX_PAGE_CHAR                   4096
#define MAX_WORD_COUNT      1500    //一页画面中包含断句的数量(以",",".","?","!"为分界点)


extern  int f_audiodev;


/* 编码格式 */
typedef enum
{
    MK_CHAR_ENCODE_UNKNOWN = 0,
    MK_CHAR_ENCODE_UTF8,
    MK_CHAR_ENCODE_GBK,
    MK_CHAR_ENCODE_UNICODE,
    MK_CHAR_ENCODE_UNICODE_BIG
} MkEncode_jt_tts;

typedef struct TTS_THREAD_ARGMEN            /*传给线程的参数的结构体*/
{
    unsigned long   woner_win;
    char *data_add;
    __s32 char_num;                     /*内容的字节数*/
} __tts_thread_argmen_t;





typedef struct   tag_TTS_PLAY_ST
{
    ES_FILE  *fd;

    __u32    tts_thread;        // TTS  语音朗读引擎句柄
    __krnl_event_t     *tts_sem;  //TTS语音朗读引擎信号量
    unsigned long  jt_hTTS;    // 合成引擎句柄


    /****and by gan****/
    char   *tts_text_curt_statr;   /*当前页数据的起始地址*/
    char   *tts_text_line_add; /*当前页分析到的一句话的地址*/
    __s32  tts_page_leave_char;    /*当前页剩余的字节数*/
    __bool  tts_from_page_stat;    /*是否是重当前页的重头读，0 表示是，否则从分析到的句读*/
    /****************/


    __u32    encode_type;


} tts_char_parser_play;

typedef struct tagUserData                // 回调用户数据定义

{
    ES_FILE    *pInputFile;     // 文本输入文件
    ES_FILE    *pOutputFile;    // 合成语音数据输出文件
    unsigned long   hTTS;   // 合成引擎句柄
} jtUserData;

//define wave header, for decode pcm data
typedef struct __WAVE_HEADER1
{
    __u32       uRiffFcc;       // four character code, "RIFF"
    __u32       uFileLen;       // file total length, don't care it

    __u32       uWaveFcc;       // four character code, "WAVE"

    __u32       uFmtFcc;        // four character code, "fmt "
    __u32       uFmtDataLen;    // Length of the fmt data (=16)
    __u16       uWavEncodeTag;  // WAVE File Encoding Tag
    __u16       uChannels;      // Channels: 1 = mono, 2 = stereo
    __u32       uSampleRate;    // Samples per second: e.g., 44100
    __u32       uBytesPerSec;   // sample rate * block align
    __u16       uBlockAlign;    // channels * bits/sample / 8
    __u16       uBitsPerSample; // 8 or 16

    __u32       uDataFcc;       // four character code "data"
    __u32       uSampDataSize;  // Sample data size(n)

} __wave_header_t1;//extern const unsigned char CNPackage[];

extern tts_char_parser_play  *_tts_play_struct;

extern void   TTS_init_all_res(void);
extern   __s32  TTS_res_release(void);
extern jtErrCode TTS_play_server_init(void *arg, void *arg1);

extern void TTS_stop_server(void);
extern __s32  get_cur_audio_buffer(void);

extern void TTS_play_server_start(void *arg, void *arg1, __s32 char_num, H_WIN  woner_win);

//extern void  delete_cuckoo( void );
__bool  cur_audio_speark_over(void);

jtErrCode tts_speaker_deint(void);



#endif /* __JT_TTS_H__ */
