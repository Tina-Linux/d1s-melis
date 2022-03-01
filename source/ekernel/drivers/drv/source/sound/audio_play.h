#ifndef  _AUDIO_PLAY_H_
#define  _AUDIO_PLAY_H_

#include <rtdef.h>
#include <ktype.h>
#include "circle_buf.h"

#ifdef CONFIG_SOC_SUN8IW19P1
#define AUDIO_PLAY_USER_MAX         1
#elif defined CONFIG_SOC_SUN20IW1P1
#define AUDIO_PLAY_USER_MAX         2
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

