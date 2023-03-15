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
#include <log.h>
#include "apps.h"
#include "com_echo.h"

ES_FILE *g_fp_rec = NULL;

//type:AUDIO_REC_USR_MIC/AUDIO_REC_USR_FM....
//auto_clean_buf:0代表不会自动清空buffer(录音应用)，为1代表自动清空buffer(非录音的应用)
//详细调用方法参见SDK的app_root_scene.c和record_view_doc.c
__s32 com_audio_echo_open(__audio_rec_user_t type, __s32 auto_clean_buf)
{
#if 0/*langaojie temp test*/
    __s32 aux;
    __audio_dev_para_t adev_para;

    if (g_fp_rec)
    {
        __wrn("rec hdl alreadly open...\n");
        return EPDK_FAIL;
    }

    g_fp_rec = eLIBs_fopen("b:\\AUDIO\\REC", "r+");

    if (NULL == g_fp_rec)
    {
        __wrn("open rec hdl fail...\n");
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_REG_USERMODE, type, 0);
    adev_para.chn = 2;
    adev_para.bps = 16;
    adev_para.fs = 44100;
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_PARA, 0, (void *)&adev_para);
    //esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_REG_USERMODE, type, 0);
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_SAMPCNT, 0, 0);//清adc buffer计数器;
    aux = 1;
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO, aux, 0);
    aux = auto_clean_buf;
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_BUF_MODE, aux, 0);
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_START, 0, 0);
    com_audio_echo_set_mic_pre_gain(1);
    com_audio_echo_set_mic_gain(0);
#endif
    return EPDK_OK;
}

//详细调用方法参见SDK的app_root_scene.c和record_view_doc.c
__s32 com_audio_echo_close(void)
{
    __s32 aux;
#if 0/*langaojie temp test*/

    if (NULL == g_fp_rec)
    {
        __wrn("rec hdl not open...\n");
        return EPDK_FAIL;
    }

    aux = 0;
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_ENABLE_ECHO, aux, 0);
    aux = 0;
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_BUF_MODE, aux, 0);
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_STOP, 0, 0);

    if (g_fp_rec)
    {
        eLIBs_fclose(g_fp_rec);
        g_fp_rec = NULL;
    }
#endif
    return EPDK_OK;
}

//该函数一般不要调用，按照默认的就行了
//0<= pre_gain <= 3
__s32 com_audio_echo_set_mic_pre_gain(__s32 pre_gain)
{
#if 0/*langaojie temp test*/

    pre_gain &= 0x03;
    *(volatile unsigned long *)(0xf1c23c28) &= ~(0x03 << 25);
    *(volatile unsigned long *)(0xf1c23c28) |= (pre_gain << 25);
#endif
    return EPDK_OK;
}

//该函数一般不要调用，按照默认的就行了
//0<= gain <= 7
__s32 com_audio_echo_set_mic_gain(__s32 gain)
{
#if 0/*langaojie temp test*/

    gain &= 0x07;
    *(volatile unsigned long *)(0xf1c23c28) &= ~(0x07 << 20);
    *(volatile unsigned long *)(0xf1c23c28) |= (gain << 20);
#endif
    return EPDK_OK;
}

//增加mic音量，-32到32，可以调节mic音量
__s32 com_audio_echo_gain_incr(void)
{
    __s32 aux;
    
    __wrn("com_audio_echo_gain_incr...\n");
#if 0/*langaojie temp test*/

    if (NULL == g_fp_rec)
    {
        __wrn("rec hdl not open...\n");
        return EPDK_FAIL;
    }

    aux = esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_GET_ECHO_REC_GAIN, 0, 0);

    if (aux < 32)
    {
        aux++;
    }

    __wrn("com_audio_echo_gain_incr, aux=%d\n", aux);
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_REC_GAIN, aux, 0);
#endif
    return EPDK_OK;
}

//减小mic音量 -32到32，可以调节mic音量
__s32 com_audio_echo_gain_decr(void)
{
    __s32 aux;
#if 0/*langaojie temp test*/

    if (NULL == g_fp_rec)
    {
        __wrn("rec hdl not open...\n");
        return EPDK_FAIL;
    }

    aux = esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_GET_ECHO_REC_GAIN, 0, 0);

    if (aux > -32)
    {
        aux--;
    }

    __wrn("com_audio_echo_gain_decr, aux=%d\n", aux);
    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_REC_GAIN, aux, 0);
#endif
    return EPDK_OK;
}

//该函数不调用，调试用
__s32 com_audio_echo_pt_regs(__u32 start, __u32 end)
{
    __u32 i;
    eLIBs_printf("*****com_audio_echo_pt_regs begin******\n");
#if 0/*langaojie temp test*/

    for (i = start ; i <= end ; i += 4)
    {
        eLIBs_printf("0x%8x:0x%8x\n", i, *(volatile unsigned long *)i);
    }

    eLIBs_printf("*****com_audio_echo_pt_regs end******\n");
#endif
    return EPDK_OK;
}

/*aux: 0<= aux <= 10 设置混响延时次数，默认为3*/
__s32 com_audio_echo_set_delay_num(__s32 aux)
{
    __wrn("com_audio_echo_set_delay_num...\n");
#if 0/*langaojie temp test*/

    if (NULL == g_fp_rec)
    {
        __wrn("rec hdl not open...\n");
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_DELAY_NUM, aux, 0);
#endif
    return EPDK_OK;
}

/*aux:10<= aux <= 3000,默认为100，以毫秒为单位*/
__s32 com_audio_echo_set_delay_time(__s32 aux)
{
    __wrn("com_audio_echo_set_delay_num...\n");
#if 0/*langaojie temp test*/

    if (NULL == g_fp_rec)
    {
        __wrn("rec hdl not open...\n");
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(g_fp_rec, AUDIO_DEV_CMD_SET_ECHO_DELAY_TIME, aux, 0);
#endif
    return EPDK_OK;
}




