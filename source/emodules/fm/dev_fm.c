#include "drv_fm_i.h"
#include <rtthread.h>
#include <log.h>
#include <init.h>
#include "mod_fm.h"
#include "fm_all_drv.h"
#include "dfs_posix.h"
#include "sunxi_drv_pwm.h"
#include "sunxi_hal_pwm.h"
#include "circle_buf.h"
#include "mod_audio.h"

#define CIRCLE_BUF_SIZE 16*1024
#define SAVE_BUF_SIZE   8*1024 

sunxi_driver_fm_t        fm;//bsp
extern  __fm_drv_t       fm_drv;//driver
extern  __krnl_event_t	*fm_lock ;
int32_t    				 fiic   = -1;
static int32_t  rec_task_id     = 0;
static int32_t	play_task_id    = 0;
static int32_t	monitor_task_id = 0;
__audio_dev_buf_manager_t   circle_buf;

static __s32 switch_amplify_func_2_AB(void)
{
    __s32 ret = 0;
    user_gpio_set_t  gpio_set[1] = {0};
    ret = esCFG_GetKeyValue("amplify", "ab_or_d",
                            (int *)gpio_set, sizeof(user_gpio_set_t) / sizeof(int));

    if (EPDK_OK != ret)
    {
        __wrn("read linein para fail...\n");
        return EPDK_FAIL;
    }

    fm_drv.fm_dev.amplify_hdl = esPINS_PinGrpReq(gpio_set, 1);

    if (NULL == fm_drv.fm_dev.amplify_hdl)
    {
        __wrn("request linein pin fail...\n");
        return EPDK_FAIL;
    }

    ret = esPINS_SetPinPull(fm_drv.fm_dev.amplify_hdl, GPIO_PULL_DOWN, "ab_or_d");

    if (ret)
    {
        __wrn("app_root_globe_check_init() pull input_row1 failed\n");
        return EPDK_FAIL;
    }

    esPINS_WritePinData(fm_drv.fm_dev.amplify_hdl, 0, NULL);
    return EPDK_OK;
}

static __s32 switch_amplify_func_2_D(void)
{
    __s32 ret = 0;
    if (NULL == fm_drv.fm_dev.amplify_hdl)
    {
        __wrn("request linein pin fail...\n");
        return EPDK_FAIL;
    }

    ret = esPINS_SetPinPull(fm_drv.fm_dev.amplify_hdl, GPIO_PULL_UP, "ab_or_d");

    if (ret)
    {
        __wrn("app_root_globe_check_init() pull input_row1 failed\n");
        return EPDK_FAIL;
    }

    esPINS_WritePinData(fm_drv.fm_dev.amplify_hdl, 1, NULL);
    esPINS_PinGrpRel(fm_drv.fm_dev.amplify_hdl, 0);
    fm_drv.fm_dev.amplify_hdl = NULL;
    return EPDK_OK;
}


static void play_task(void *parg)
{
    __audio_dev_para_t adev_para;
    uint8_t * temp_buf  = NULL;
    int32_t  g_fp_play = -1;
    unsigned long  args[2] = {0};
    int32_t len = 0;
	uint8_t err;
	
    g_fp_play     = open("/dev/audio_play0",O_RDWR); 
	if(g_fp_play < 0)
	{
		__err("open audio_play err!!!");
		//goto _audio_play_task_exit;
	}
    adev_para.chn = 2;
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
		//goto _audio_play_task_exit;
    }
    //esKRNL_TimeDly(10); 

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
            //__log("play = 0!!!");
            esKRNL_TimeDly(1); 
            continue;
        }
        
		write(g_fp_play,temp_buf,len);
        //esKRNL_TimeDly(1); 
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
	//play_task_id = 0;
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

	g_fp_rec   = open("/dev/audio_rec0",O_WRONLY); 
	if(g_fp_rec < 0)
	{
		__err("open audio_rec fail\n");
		goto _audio_rec_task_exit;
	}

	args[0] = (unsigned long)AUDIO_REC_USR_FMIN;
	args[1] = 0;
	ioctl(g_fp_rec, AUDIO_DEV_CMD_REG_USERMODE, args);

	adev_para.chn = 2;
	adev_para.bps = 16;
	adev_para.fs  = 48000;
	args[0] = 0;
	args[1] = (unsigned long)&adev_para;
	ioctl(g_fp_rec, AUDIO_DEV_CMD_SET_PARA, args);
	ioctl(g_fp_rec, AUDIO_DEV_CMD_FLUSH_BUF, 0);
	ioctl(g_fp_rec, AUDIO_DEV_CMD_START, 0);
	
	temp_buf	  = (uint8_t *)malloc(SAVE_BUF_SIZE);
	if(temp_buf == NULL)
	{
		__err("malloc fail\n");
		goto _audio_rec_task_exit;
	}
    //esKRNL_TimeDly(10); 

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
	
	//rec_task_id = 0;
    esKRNL_TDel(EXEC_prioself);
}
void monitor_task(void *parg)
{
    unsigned long len = 0;
    unsigned long args[4] = {0,0,0,0};
	int rec_fp = -1;
	int len_rec;
	rec_fp = open("/dev/audio_rec0",O_RDWR);
	while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
        	goto _monitor_task_exit;
        }

        len = CircleBufQuerySize(&circle_buf, AUDIO_DEV_QUERY_BUF_SIZE_DATA);

		args[0] = (unsigned long)AUDIO_DEV_QUERY_BUF_SIZE_DATA;
        args[1] = 0;
        len_rec = ioctl(rec_fp, AUDIO_DEV_CMD_QUERY_BUFSIZE,args);	
		__log("monitor :%d rec = %d",len,len_rec);
        
        esKRNL_TimeDly(100); 
    }
_monitor_task_exit:
	esKRNL_TDel(EXEC_prioself);

}
int32_t audio_start(void)
{
    CircleBufCreate(&circle_buf,CIRCLE_BUF_SIZE);
	
    play_task_id = esKRNL_TCreate(play_task,0, 0x8000, KRNL_priolevel1);
    rec_task_id  = esKRNL_TCreate(rec_task, 0, 0x8000, KRNL_priolevel1);
    //monitor_task_id = esKRNL_TCreate(monitor_task,0, 0x1000, KRNL_priolevel3);
	return EPDK_OK;
}
int32_t audio_stop(void)
{
    if(play_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(play_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(play_task_id));
        play_task_id = 0;
    }
	
    if(rec_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(rec_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(rec_task_id));
        rec_task_id = 0;
    }
	
    if(monitor_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(monitor_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(monitor_task_id));
        monitor_task_id = 0;
        
    }
    CircleBufDestroy(&circle_buf);

    return EPDK_OK;

}

int32_t hal_fm_init(void)
{
    struct pwm_config config ;
	int32_t ret ;
	audio_start();
	
    fm_drv.fm_dev.pwm_dev  = open("/dev/pwm/",O_RDWR);
    if (fm_drv.fm_dev.pwm_dev < 0)
    {
        __err("FM Find PWM Dev File Failed\n");
        goto hal_fm_init_exit;
    }
    
    config.duty_ns   = 21;//24MHz 50%
    config.period_ns = 41;
    config.polarity  = PWM_POLARITY_NORMAL;
#ifdef CONFIG_PLAN_BEETLES  //Simply,should replace it with a configuration file
    ioctl(fm_drv.fm_dev.pwm_dev,2,&config);
#endif
#ifdef CONFIG_PLAN_SPRITE
    ioctl(fm_drv.fm_dev.pwm_dev,6,&config);
#endif
    esKRNL_TimeDly(10);

#ifdef CONFIG_PLAN_BEETLES
    fm_drv.fm_dev.twi_dev  = open("/dev/twi3/",O_RDWR);
#endif
#ifdef CONFIG_PLAN_SPRITE
	fm_drv.fm_dev.twi_dev  = open("/dev/twi2/",O_RDWR);
#endif

    if (fm_drv.fm_dev.twi_dev < 0)
    {
        __err("FM Find TWI Dev File Failed\n");
        goto hal_fm_init_exit;
    }
    fiic = fm_drv.fm_dev.twi_dev;

  
	
    return EPDK_OK;
hal_fm_init_exit:

	if(fm_drv.fm_dev.pwm_dev >= 0)
	{
		close(fm_drv.fm_dev.pwm_dev);
		fm_drv.fm_dev.pwm_dev = -1;
	}
	if(fm_drv.fm_dev.twi_dev >= 0)
	{
		close(fm_drv.fm_dev.twi_dev);
		fm_drv.fm_dev.twi_dev = -1;
	}
	fiic = -1;
	audio_stop();
	return EPDK_FAIL;
}

int32_t hal_fm_uninit(void)
{
    rt_err_t ret;
    
    audio_stop();
	
    if(fm_drv.fm_dev.twi_dev >= 0)
    {
        ret = close(fm_drv.fm_dev.twi_dev);
        if (ret != 0)
        {
            __wrn("Close TWI Dev File Failed\n");
            return EPDK_FAIL;
        }
        fm_drv.fm_dev.twi_dev = -1;
    }
    
    if(fm_drv.fm_dev.pwm_dev >= 0)
    {
        ret = close(fm_drv.fm_dev.pwm_dev);
        if (ret != 0)
        {
            __wrn("Close TWI Dev File Failed\n");
            return EPDK_FAIL;
        }
        fm_drv.fm_dev.pwm_dev = -1;
    }
	fiic = -1;
    return EPDK_OK;
}

int32_t hal_fm_control(int cmd, void *args)
{
    unsigned long  *ubuffer = NULL;
    unsigned long   aux     = 0;
    unsigned long  *pbuffer = NULL;
	unsigned long  *r_arg   = NULL;
    int ret = 0;
    __u8  err;
    if(args != NULL)
    {
        ubuffer = (unsigned long *)args;
        aux     = ubuffer[0];
        pbuffer = (unsigned long *)ubuffer[1];
		r_arg   = (unsigned long *)ubuffer[2];
    }
    esKRNL_SemPend(fm_lock, 0, &err);
    switch (cmd)
    {
        case DRV_FM_CMD_INIT:
        {
            switch_amplify_func_2_AB();
            fm_init();
            fm_drv.fm_dev.status = FM_DRV_STATUS_RECEIVE;
            ret = fm_drv.fm_dev.status;
            break;
        }

        case DRV_FM_CMD_EXIT:
        {
            fm_exit();
            switch_amplify_func_2_D();
            ret = EPDK_OK;
            break;
        }

        case DRV_FM_CMD_RECVE_INIT:
        {
            break;
        }

        case DRV_FM_CMD_RECVE_EXIT:
        {
            break;
        }

        case DRV_FM_CMD_VOL_SET:
            ret=fm_vol_set(aux);
            break;
        case DRV_FM_CMD_VOL_GET:
            *r_arg=fm_vol_get();
			ret = EPDK_OK;
            break;

        case DRV_FM_CMD_RECVE_PLAY:
        {
            fm_play((long)pbuffer);
            ret = EPDK_OK;
            break;
        }

        case DRV_FM_CMD_RECVE_SEARCH:
        {
            if ((aux & 0xff) == DRV_FM_AUTO_SEARCH)
            {
                __wrn(" ret = %d,  aux & 0xff00 = 0x%x\n", ret, aux & 0xff00);
                *r_arg = fm_auto_search((__s32)(__u64)pbuffer, aux & 0xff00);
				ret = EPDK_OK;
            }
            else if ((aux & 0xff) == DRV_FM_MANUAL_SEARCH)
            {
                __wrn(" ret = %d,  aux & 0xff00 = 0x%x\n", ret, aux & 0xff00);
                *r_arg = fm_manual_search((__s32)(__u64)pbuffer, aux & 0xff00);
				ret = EPDK_OK;
            }

            break;
        }

        case DRV_FM_CMD_AREA_SET:
        {
            ret = fm_area_choose(aux,pbuffer);
            break;
        }

        case DRV_FM_CMD_STEREO_SET:
        {
            ret = fm_stereo_choose(aux);
            break;
        }

        case DRV_FM_CMD_MUTE:
        {
            ret = fm_mute(aux);
            break;
        }

        case DRV_FM_CMD_SLEVEL_SET:
        {
            ret = fm_signal_level(aux);
            break;
        }

        case DRV_FM_CMD_SEND_INIT:
        {
            ret = fm_send_on();

            if (ret == EPDK_OK)
            {
                fm_drv.fm_dev.status = FM_DRV_STATUS_SEND;
            }

            break;
        }

        case DRV_FM_CMD_SEND_EXIT:
        {
            ret = fm_send_off();
            break;
        }

        case DRV_FM_CMD_SEND_START:
        {
            ret = fm_send((long)pbuffer);
            break;
        }

        case DRV_FM_CMD_SEND_PAGAIN_SET:
        {
            ret = fm_pa_gain(aux);
            break;
        }

        case DRV_FM_CMD_QUERY_STATUS:
        {
            //ret = fm_drv.fm_dev.status;
            fm_drv.fm_dev.status = fm_get_status();
            *r_arg = fm_drv.fm_dev.status;
			ret = EPDK_OK;
            break;
        }

        default:
        {
            __wrn("Unkonwn Command...\n");
            break;
        }
    }
    
    esKRNL_SemPost(fm_lock);
    return ret;
}




static rt_err_t sunxi_fm_init(struct rt_device *dev)
{
    return 0;
}

static rt_err_t sunxi_fm_open(struct rt_device *dev, rt_uint16_t oflag)
{
    rt_size_t ret = 0;
    sunxi_driver_fm_t *pusr, *pvfy;
    sunxi_hal_driver_fm_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_fm_t, base);
    pusr = (sunxi_driver_fm_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_fm_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->initialize)
    {
        ret = hal_drv->initialize();
    }

    return ret;
}

static rt_err_t sunxi_fm_close(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_fm_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_fm_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_fm_t, base);
    pusr = (sunxi_driver_fm_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_fm_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->uninitialize)
    {
        ret = hal_drv->uninitialize();
    }

    return ret;
}

static rt_size_t sunxi_fm_write(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    return 0;
}

static rt_size_t sunxi_fm_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t sunxi_fm_control(struct rt_device *dev, int cmd, void *args)
{
    rt_size_t ret = 0;
    sunxi_driver_fm_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_fm_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_fm_t, base);
    pusr = (sunxi_driver_fm_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_fm_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->control)
    {
        ret = hal_drv->control(cmd, args);
    }

    return ret;
}
 
static void init_fm_device(struct rt_device *dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;

    dev->open       = sunxi_fm_open;
    dev->close      = sunxi_fm_close;
    dev->init       = sunxi_fm_init;
    dev->write      = sunxi_fm_write;
    dev->read       = sunxi_fm_read;
    dev->control    = sunxi_fm_control;
    dev->user_data  = usr_data;

    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        while (1);
    }
    return; 
}

static const sunxi_hal_driver_fm_t sunxi_hal_fm_driver =
{
    .initialize     = hal_fm_init,
    .uninitialize   = hal_fm_uninit,
    .control        = hal_fm_control,
};

int sunxi_driver_fm_init(void)
{
    struct rt_device *device;

    device = &fm.base;
    fm.dev_id = 0;
    fm.hal_drv = &sunxi_hal_fm_driver;

    init_fm_device(device, &fm, "fm");

    return 0;
}

int sunxi_driver_fm_uninit(void)
{
    rt_err_t ret;
    
    ret = rt_device_unregister(&fm.base);
    if (ret != 0)
    {
        __err("fatal error, ret %d, unregister device from rtsystem failure.\n", ret);
        while (1);
        return EPDK_FAIL;
    }
    
    return EPDK_OK;
}

