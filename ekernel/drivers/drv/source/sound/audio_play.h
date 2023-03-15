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
#ifndef  _AUDIO_PLAY_H_
#define  _AUDIO_PLAY_H_

#include <rtdef.h>
#include <ktype.h>
#include "circle_buf.h"

#ifdef CONFIG_SOC_SUN8IW19P1
#define AUDIO_PLAY_USER_MAX         1
#elif defined CONFIG_SOC_SUN20IW1P1
#define AUDIO_PLAY_USER_MAX         3
#endif

#define AUDIO_PLAY_MSGQ_SIZE        32          //main task message queue depth
#define AUDIO_PLAY_TMP_BUF_SIZE     (1024*8)    //dma cache buffer
#define AUDIO_PLAY_USER_BUF_SIZE    (1024*128)   //user cache buffer

#define INVALID_PLAY_USR_ID         (-1)

typedef struct AUDIO_PLAY_DEV_USR
{
    __s8                        usrId;  //[0, AUDIO_PLAY_USER_MAX), -1 is invalid
    __s8                        used;   //used or not
    __u8                        status; //idle/running/pause
    __u8                        playDataEnd;//is all data is written to the buffer
    __audio_dev_buf_manager_t   buf;    //audio play device data cache
} audio_play_dev_usr_t;

typedef struct AUDIO_PLAY_DEV_PARAMS
{
    __u32                   playMainTaskPrio; //audio play device main task priority
    __u8                    usrCnt;           //audio play device user count,[0, AUDIO_PLAY_USER_MAX]
    __s8                    activeUsrId;      //[0, AUDIO_PLAY_USER_MAX), -1 is invalid
    __s8                    usr_flag;         //user wakeup audio play device flag
    audio_play_dev_usr_t    usr[AUDIO_PLAY_USER_MAX]; //audio play device users
    __u8                    *pBuffer;         //dma cache
    __u32                   pBufferSize;       //dma cache size
    __krnl_event_t          *pSemProc;        //semaphore for wake up audio play main task
    __krnl_event_t          *msg;             //message from user to audio play main task

} audio_play_dev_params_t;

extern void  audio_play_main_task(void *parg);

extern rt_err_t  audio_play_init(rt_device_t dev);
extern rt_err_t  audio_play_open(rt_device_t dev, rt_uint16_t oflag);
extern rt_err_t  audio_play_close(rt_device_t dev);
extern rt_size_t audio_play_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
extern rt_size_t audio_play_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);
extern rt_err_t  audio_play_control(rt_device_t dev, int cmd, void *arg);

#endif

