#ifndef  _AUDIO_REC_H_
#define  _AUDIO_REC_H_

#include <rtdef.h>
#include <ktype.h>
#include "circle_buf.h"

#ifdef CONFIG_SOC_SUN8IW19P1
#define AUDIO_REC_USER_MAX          1
#elif defined CONFIG_SOC_SUN20IW1P1
#define AUDIO_REC_USER_MAX          3
#endif

#define AUDIO_REC_MSGQ_SIZE         32          //main task message queue depth
#define AUDIO_REC_TMP_BUF_SIZE      (1024*4)    //dma cache buffer
#define AUDIO_REC_USR_BUF_SIZE      (1024*1024*1)//(1024*64)   //user cache buffer

typedef struct AUDIO_REC_DEV_PARAMS
{
    __u32               main_task_prio; //audio rec device main task priority
    __u8                status;         //audio rec device current status

    __audio_dev_buf_manager_t   buf;    //audio rec device data cache
    __u8                        *pBuffer;         //dma cache
    __u32                       pBufferSize;       //dma cache size

    __krnl_event_t      *pSemProc;      //semaphor for wake up audio rec main task
    __krnl_event_t      *msg;           //message from user to audio rec main task
    __u32  usemode;//fm/mic/line in
    __u32  pcm_cnt;

} audio_rec_dev_params_t;

extern void  audio_rec_task(void *parg);

extern rt_err_t audio_rec_init(rt_device_t dev);
extern rt_err_t audio_rec_open(rt_device_t dev, rt_uint16_t oflag);
extern rt_err_t audio_rec_close(rt_device_t dev);
extern rt_size_t audio_rec_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
extern rt_size_t audio_rec_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);
extern rt_err_t audio_rec_control(rt_device_t dev, int cmd, void *arg);

#endif
