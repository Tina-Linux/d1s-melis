#include "log.h"
#include "kapi.h"
#include "dfs_posix.h"
#include "mod_audio.h"
#include "circle_buf.h"

#define CIRCLE_BUF_SIZE     128*1024
#define SAVE_BUF_SIZE       8*1024

__audio_dev_buf_manager_t   circle_buf;

static void play_task(void *parg)
{
    __audio_dev_para_t adev_para;
    uint8_t *temp_buf  = NULL;
    int32_t  g_fp_play = -1;
    unsigned long  args[2] = {0};
    int32_t len = 0;
    uint8_t err;

    g_fp_play = open("/dev/audio_play0",O_RDWR);
    if(g_fp_play < 0)
    {
        __err("open audio_play err!!!");
        goto _audio_play_task_exit;
    }

    adev_para.chn = 1;
    adev_para.bps = 16;
    adev_para.fs  = 48000;
    args[0] = 0;
    args[1] = (unsigned long)&adev_para;
    ioctl(g_fp_play, AUDIO_DEV_CMD_SET_PARA,args);

    ioctl(g_fp_play, AUDIO_DEV_CMD_FLUSH_BUF, 0);
    ioctl(g_fp_play, AUDIO_DEV_CMD_START, 0);

    temp_buf = (uint8_t *)malloc(SAVE_BUF_SIZE);
    if(temp_buf == NULL)
    {
        __err("malloc fail\n");
        goto _audio_play_task_exit;
    }
    esKRNL_TimeDly(10);

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto _audio_play_task_exit;
        }

        len = CircleBufQuerySize(&circle_buf, AUDIO_DEV_QUERY_BUF_SIZE_DATA);
        len = (len >SAVE_BUF_SIZE)?(SAVE_BUF_SIZE):(len);
        if (len > 0)
        {
              CircleBufRead(&circle_buf, temp_buf, len);
              //esMEMS_CleanFlushDCacheRegion(temp_buf, len);
        }
        else
        {
            esKRNL_TimeDly(1);
            continue;
        }

        write(g_fp_play, temp_buf, len);
        esKRNL_TimeDly(1);
    }

_audio_play_task_exit:

    if(g_fp_play >= 0)
    {
        ioctl(g_fp_play, AUDIO_DEV_CMD_STOP, 0);
        close(g_fp_play);
        g_fp_play = -1;
    }

    if(temp_buf != NULL)
    {
        free(temp_buf);
        temp_buf = NULL;
    }

    esKRNL_TDel(EXEC_prioself);
}

static void rec_task(void *parg)
{
    __audio_dev_para_t adev_para;
    int32_t len = 0;
    uint8_t * temp_buf = NULL;
    int32_t g_fp_rec = -1;
    int32_t len_rec = -1;
    unsigned long  args[2] = {0};

    g_fp_rec = open("/dev/audio_rec0",O_WRONLY);
    if(g_fp_rec < 0)
    {
        __err("open audio_rec fail\n");
        goto _audio_rec_task_exit;
    }

    args[0] = (unsigned long)AUDIO_REC_USR_MIC;
    args[1] = 0;
    ioctl(g_fp_rec, AUDIO_DEV_CMD_REG_USERMODE, args);

    adev_para.chn = 1;
    adev_para.bps = 16;
    adev_para.fs  = 48000;
    args[0] = 0;
    args[1] = (unsigned long)&adev_para;
    ioctl(g_fp_rec, AUDIO_DEV_CMD_SET_PARA, args);

    ioctl(g_fp_rec, AUDIO_DEV_CMD_FLUSH_BUF, 0);
    ioctl(g_fp_rec, AUDIO_DEV_CMD_START, 0);

    temp_buf = (uint8_t *)malloc(SAVE_BUF_SIZE);
    if(temp_buf == NULL)
    {
        __err("malloc fail\n");
        goto _audio_rec_task_exit;
    }
    esKRNL_TimeDly(10);

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto _audio_rec_task_exit;
        }

        args[0] = (unsigned long)SAVE_BUF_SIZE;
        args[1] = (unsigned long)temp_buf;
        len = ioctl(g_fp_rec, AUDIO_DEV_CMD_READ_DATA,args);
        if(len == 0)
        {
            esKRNL_TimeDly(1);
            continue;
        }

        if (CircleBufQuerySize(&circle_buf, AUDIO_DEV_QUERY_BUF_SIZE_FREE) >= len)
        {
            CircleBufWrite(&circle_buf, temp_buf, len);
            //esMEMS_CleanFlushDCacheRegion(temp_buf, len);
        }
        else
        {
            __wrn("audio record buffer overflow!");
        }

        //esKRNL_TimeDly(1);
    }

_audio_rec_task_exit:

    if(g_fp_rec >= 0)
    {
        ioctl(g_fp_rec, AUDIO_DEV_CMD_STOP, 0);
        close(g_fp_rec);
        g_fp_rec = -1;
    }

    if(temp_buf != NULL)
    {
        free(temp_buf);
        temp_buf = NULL;
    }

    esKRNL_TDel(EXEC_prioself);
}


static int cmd_rec_play_test(int argc, char **argv)
{
    __u32  tsk_prio_play = 0, tsk_prio_rec = 0;

    CircleBufCreate(&circle_buf,CIRCLE_BUF_SIZE);
    tsk_prio_play = esKRNL_TCreate(play_task,0, 0x8000, KRNL_priolevel1);
    tsk_prio_rec = esKRNL_TCreate(rec_task, 0, 0x8000, KRNL_priolevel1);

    esKRNL_TimeDly(10);
    __log("Please enter 'q' or Ctrl-C to quit rec_play_test command.\n");
    while (1)
    {
        char ch = getchar();
        if (ch == 'q' || ch == 3)
        {
            if (tsk_prio_play)
            {
                while (esKRNL_TDelReq(tsk_prio_play) != OS_TASK_NOT_EXIST)
                {
                    esKRNL_TimeDly(1);
                }

                tsk_prio_play = 0;
            }

            if (tsk_prio_rec)
            {
                while (esKRNL_TDelReq(tsk_prio_rec) != OS_TASK_NOT_EXIST)
                {
                    esKRNL_TimeDly(1);
                }

                tsk_prio_rec = 0;
            }
            break;
        }
    }
    return 0;

}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_rec_play_test, __cmd_rec_play_test, record_and_play_test);
