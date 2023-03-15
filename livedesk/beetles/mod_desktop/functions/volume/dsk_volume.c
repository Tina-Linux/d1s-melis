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
#include "functions_i.h"
#include <kconfig.h>
#include "dfs_posix.h"
#define AUDIO_DEV "/dev/audio_play0"



#define PIOC_MEM_BASE               (0xf1c20800)

#define PIO_REG_CFG(n, i)           ((volatile unsigned int  *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)        ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)          ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n)             ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))


static __bool   dsk_volume_gate_on  = EPDK_TRUE;
static int32_t  dsk_volume          = EPDK_TRUE;
static int32_t  g_amplifier_status  = 0;
static __bool   app_in_line_in      = EPDK_NO;
static __hdle   g_hp_det_hdl        = NULL;
static __bool   g_open_amp          = EPDK_FALSE;
static __hdle   g_pa_hdl            = 0;

static int32_t  _set_volume(int32_t volume);
static int32_t  _get_volume(void);

static int32_t __init_hp_det_pin_init(void)
{
    int32_t             ret;
    user_gpio_set_t     gpio_set[1];

    memset(gpio_set, 0, sizeof(user_gpio_set_t));
    ret = esCFG_GetKeyValue("audio_para", "HP_DET", (__s32 *)gpio_set, sizeof(user_gpio_set_t) / 4);

    if (!ret)
    {
        g_hp_det_hdl = esPINS_PinGrpReq(gpio_set, 1);

        if (!g_hp_det_hdl)
        {
            __msg("request output_col pin failed");
            return EPDK_FAIL;
        }
    }
    else
    {
        __msg("fetch para from script failed");
        return EPDK_FAIL;
    }

    //ret = esPINS_SetPinPull(g_hp_det_hdl, PIN_PULL_DOWN, NULL);
    //esPINS_PinGrpRel(hdl, 0);
    return EPDK_OK;
}

static void __init_hp_det_pin_uninit(void)
{
    if (NULL == g_hp_det_hdl)
    {
        esPINS_PinGrpRel(g_hp_det_hdl, 0);
        g_hp_det_hdl = NULL;
    }
}

//onoff: 0->off  1->on
void dsk_hp_det_pin_det(void)
{
    if (NULL == g_hp_det_hdl)
    {
        return;
    }

    if (0 == esPINS_ReadPinData(g_hp_det_hdl, "HP_DET")) //in
    {
        if (dsk_amplifier_is_on() == EPDK_TRUE)
        {
            g_open_amp = EPDK_TRUE;
        }

        dsk_amplifier_onoff(0);
    }
    else
    {
        if (EPDK_TRUE == g_open_amp)
        {
            g_open_amp = EPDK_FALSE;
            dsk_amplifier_onoff(1);
        }
    }

    return ;
}

static int32_t dsk_pa_shdn_pin_init(void)
{
    int32_t             ret;
    user_gpio_set_t     gpio_set[1];

    memset(gpio_set, 0, sizeof(user_gpio_set_t));

    ret = esCFG_GetKeyValue("audio_para", "PA_SHDN", (__s32 *)gpio_set, sizeof(user_gpio_set_t) / 4);

    if (!ret)
    {
        g_pa_hdl = esPINS_PinGrpReq(gpio_set, 1);

        if (!g_pa_hdl)
        {
            __msg("request output_col pin failed");
            return EPDK_FAIL;
        }
    }
    else
    {
        __msg("fetch para from script failed");
        return EPDK_FAIL;
    }

    ret = esPINS_WritePinData(g_pa_hdl, 0, "PA_SHDN");

    //ret = esPINS_SetPinPull(hdl, PIN_PULL_DOWN, NULL);
    //esPINS_PinGrpRel(hdl, 0);

    __init_hp_det_pin_init();
    return EPDK_OK;
}

static void dsk_pa_shdn_pin_uninit(void)
{
    if (NULL == g_pa_hdl)
    {
        esPINS_PinGrpRel(g_pa_hdl, 0);

        g_pa_hdl = NULL;

        __init_hp_det_pin_uninit();
    }
}

void pull_down_gpio(uint32_t port, uint32_t port_num)
{
    volatile  uint32_t  *tmp_addr;
    uint32_t            tmp_data;
    uint32_t            tmp1;
    uint32_t            tmp2;

    //设置为输出
    tmp1        = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr    = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data    = *tmp_addr;
    tmp_data    &= ~(0x07 << tmp1);
    tmp_data    |= (0x01 << tmp1);
    *tmp_addr   = tmp_data;

    //设置为下拉
    tmp2        = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr    = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data    = *tmp_addr;
    tmp_data    &= ~(0x03 << tmp2);
    tmp_data    |= (0x02 << tmp2);
    *tmp_addr   = tmp_data;

    //输出低电平
    tmp_addr    = PIO_REG_DATA(port);
    tmp_data    = *tmp_addr;
    tmp_data    &= ~(1 << port_num);
    //tmp_data |=  (1 << port_num);
    *tmp_addr   = tmp_data;
}

void pull_up_gpio(uint32_t port, uint32_t port_num)
{
    volatile  uint32_t      *tmp_addr;
    uint32_t                tmp_data;
    uint32_t                tmp1;
    uint32_t                tmp2;

    //设置为输出
    tmp1        = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr    = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data    = *tmp_addr;
    tmp_data    &= ~(0x07 << tmp1);
    tmp_data    |= (0x01 << tmp1);
    *tmp_addr   = tmp_data;

    //设置为上拉
    tmp2        = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr    = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data    = *tmp_addr;
    tmp_data    &= ~(0x03 << tmp2);
    tmp_data    |= (0x01 << tmp2);
    *tmp_addr   = tmp_data;

    //输出高电平
    tmp_addr    = PIO_REG_DATA(port);
    tmp_data    = *tmp_addr;
    tmp_data    &= ~(1 << port_num);
    tmp_data    |= (1 << port_num);
    *tmp_addr   = tmp_data;
}


/*
****************************************************************************************************
*Name        : dsk_volume_set_volume
*Prototype   : __s32 dsk_volume_set_volume(__s32 volume)
*Arguments   : volume     input. volume value, ranges AUDIO_DEVICE_VOLUME_MIN~~AUDIO_DEVICE_VOLUME_MAX.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : set the volume.
*Other       :
****************************************************************************************************
*/
int32_t dsk_volume_set(int32_t volume)  // 设置当前音量
{
    int32_t     ret;

    if (!dsk_volume_gate_on)
    {
        return EPDK_FAIL;
    }

    ret = _set_volume(volume);

    if (ret != EPDK_FAIL)
    {
        dsk_volume = volume;
        {
            reg_system_para_t *para;

            para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

            if (para)
            {
                para->volume = volume;
            }
        }
    }

    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_volume_get_volume
*Prototype   : __s32 dsk_volume_get_volume(void)
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : get the curret volume value.
*Other       :
****************************************************************************************************
*/
int32_t dsk_volume_get(void)      // 当前音量大小
{
    if (!dsk_volume_gate_on)
    {
        __log("dsk_volume_gate_on = NULL");
        return 0;
    }
    return _get_volume();
}



/*
****************************************************************************************************
*Name        : dsk_volume_inc_volume
*Prototype   : __s32 dsk_volume_inc_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
int32_t dsk_volume_inc(void)      // 增大音量
{
    int32_t     volume;
    int32_t     ret;

    __wrn("***************** dsk_volume_inc ");

    if (!dsk_volume_gate_on)
    {
        dsk_volume_gate_on = EPDK_TRUE;//shiql add
        __wrn("***************** dsk_volume_gate_on = 0");
        // return -1;
    }

    volume  = _get_volume();
    __wrn("volume=%d", volume);

    if (volume == -1)
    {
        return -1;
    }

    if (volume < AUDIO_DEVICE_VOLUME_MAX)
    {
        volume++;
        ret     = _set_volume(volume);

        if (ret != EPDK_FAIL)
        {
            dsk_volume = volume;
            {
                reg_system_para_t *para;

                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    para->volume = volume;
                }
            }
            return volume;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return volume;
    }
}

/*
****************************************************************************************************
*Name        : dsk_volume_dec_volume
*Prototype   : __s32 dsk_volume_dec_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
int32_t dsk_volume_dec(void)      // 减小音量
{
    int32_t     volume;
    int32_t     ret;

    if (!dsk_volume_gate_on)
    {
        dsk_volume_gate_on = EPDK_TRUE;// shiql add
        //return -1;
    }
    volume = dsk_volume_get();

__log("dsk_volume_dec now val = %d",volume);

    if (volume == -1)
    {
        return -1;
    }

    if (volume > AUDIO_DEVICE_VOLUME_MIN)
    {
        volume--;
        __log("xxxx volume = %d",volume);
        ret = _set_volume(volume);

        if (ret != EPDK_FAIL)
        {
            dsk_volume = volume;
            {
                reg_system_para_t *para;

                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    para->volume = volume;
                }
            }
            return volume;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return volume;
    }
}



/*
****************************************************************************************************
*Name        : dsk_volume_on
*Prototype   : __s32 dsk_volume_on(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice on.
*Other       :
****************************************************************************************************
*/
int32_t dsk_volume_on(void)
{
    if (dsk_volume_gate_on)
    {
        return EPDK_OK;
    }

    _set_volume(dsk_volume);

    dsk_volume_gate_on = EPDK_TRUE;
    return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_volume_off
*Prototype   : __s32 dsk_volume_off(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice off(mute).
*Other       :
****************************************************************************************************
*/
int32_t dsk_volume_off(void)               // close voice
{
    if (!dsk_volume_gate_on)
    {
        return EPDK_OK;
    }

    _set_volume(0);                // mute

    dsk_volume_gate_on = EPDK_FALSE;
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_volume_is_on
*Prototype   : __bool dsk_volume_is_on( void )
*Arguments   : void
*Return      : the status of the voice, on or off(mute).
*Description : get the status of the voice, on or off(mute).
*Other       :
****************************************************************************************************
*/
__bool dsk_volume_is_on(void)
{
    return  dsk_volume_gate_on;
}


int32_t dsk_volume_app_line_in(__bool app_line_in)
{
    app_in_line_in = app_line_in;
    return EPDK_OK;
}


static int32_t _set_volume(__s32 volume)
{
    int audio_fd = -1;
    unsigned long         args[4]={0};
    int32_t     ret;

    if (volume < AUDIO_DEVICE_VOLUME_MIN || volume > AUDIO_DEVICE_VOLUME_MAX)
    {
        return EPDK_FAIL;
    }

    audio_fd = open("/dev/audio_play0",O_RDWR);

    if ( audio_fd < 0)
    {
        __err("open dac device fail!");
        return EPDK_FAIL;
    }

    if (NULL == g_pa_hdl)
    {
        __log("2");
        dsk_pa_shdn_pin_init();
    }

    if (AUDIO_DEVICE_VOLUME_MIN == volume)
    {
        /////pull_up_gpio(7, 6);//关闭功放
        if (g_pa_hdl)
        {
            esPINS_WritePinData(g_pa_hdl, 1, "PA_SHDN");
        }
    }
    else
    {
        if (g_amplifier_status == 0)
        {
            //pull_up_gpio(7, 6);//关闭功放
            if (g_pa_hdl)
            {
                esPINS_WritePinData(g_pa_hdl, 1, "PA_SHDN");
            }
        }
        else
        {
            //pull_down_gpio(7, 6);//打开功放
            if (g_pa_hdl)
            {
                esPINS_WritePinData(g_pa_hdl, 0, "PA_SHDN");
            }
        }
    }

    args[0] = volume;
    __log("volume: =%d", volume);

#ifdef CONFIG_SOC_SUN20IW1P1

    reg_movie_para_t *para;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (para)
    {
        args[2] = (unsigned long)para->channel;         //using args[2] instead of args[1] to avoid data corruption later on
        __log("@@@ para->channel = %d @@@", para->channel);
    }

#endif

    ret = ioctl(audio_fd, AUDIO_DEV_CMD_SET_VOLUME, args);

    close(audio_fd);
    return ret;
}

static int32_t _get_volume(void)
{
    int audio_fd  = -1;
    int32_t     volume;

    if (1) //音量限制为28最高后，直接注册表读取
    {
        reg_system_para_t *para;

        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            //__log(".._get_volume: volume=%d", para->volume);
            return para->volume;
        }
    }
    audio_fd = open("/dev/audio_play0",O_RDWR);

    if (audio_fd<0)
    {
        __err("open dac device fail!");
        return -1;
    }
    volume = ioctl(audio_fd, AUDIO_DEV_CMD_GET_VOLUME, 0);

    close(audio_fd);
    __log("volume================%d ", volume);
    return volume;
}

int32_t dsk_amplifier_onoff(int32_t flag)//设置功放开关
{
    g_amplifier_status = flag;

    if (NULL == g_pa_hdl)
    {
        dsk_pa_shdn_pin_init();
    }

    if (0 == flag)
    {
        //pull_up_gpio(7, 6);//关闭功放
        if (g_pa_hdl)
        {
            esPINS_WritePinData(g_pa_hdl, 1, "PA_SHDN");
        }

        if(g_hp_det_hdl)
        {
            if (1 == esPINS_ReadPinData(g_hp_det_hdl, "HP_DET")) //not in
            {
                g_open_amp = EPDK_FALSE;
            }
        }

    }
    else
    {
        if (g_open_amp == EPDK_FALSE)
        {
            reg_system_para_t *para;

            para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

            if (para && AUDIO_DEVICE_VOLUME_MIN == para->volume)
            {
                //pull_up_gpio(7, 6);//关闭功放
                if (g_pa_hdl)
                {
                    esPINS_WritePinData(g_pa_hdl, 1, "PA_SHDN");
                }
            }
            else
            {
                //pull_down_gpio(7, 6);//打开功放
                if (g_pa_hdl && g_hp_det_hdl)
                {
                    if (1 == esPINS_ReadPinData(g_hp_det_hdl, "HP_DET"))
                    {
                        esPINS_WritePinData(g_pa_hdl, 0, "PA_SHDN");
                    }
                }

                g_open_amp = EPDK_TRUE;
            }
        }
    }

    return EPDK_OK;
}

__bool dsk_amplifier_is_on(void)//查询功放开关
{
    if (1 == g_amplifier_status)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}
