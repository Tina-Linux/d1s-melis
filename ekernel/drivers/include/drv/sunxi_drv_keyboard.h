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
#ifndef SUNXIKBD_DEBUG_H
#define SUNXIKBD_DEBUG_H

#include "../hal/sunxi_hal_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUNXIKBD_DEBUG
#ifdef  SUNXIKBD_DEBUG
#define SUNXIKBD_INFO(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)
#else
#define SUNXIKBD_INFO(fmt, args...)
#endif

#define SUNXIKBD_ERR(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)

#define INITIAL_VALUE 0xff
#define KEY_MAX_CNT         (40)
#define VOL_NUM             KEY_MAX_CNT
#define MAXIMUM_INPUT_VOLTAGE       1800
#define DEVIATION           100
#define SUNXIKEY_DOWN           (MAXIMUM_INPUT_VOLTAGE-DEVIATION)
#define SUNXIKEY_UP         SUNXIKEY_DOWN
#define MAXIMUM_SCALE           128
#define SCALE_UNIT          (MAXIMUM_INPUT_VOLTAGE/MAXIMUM_SCALE)
#define SAMPLING_FREQUENCY      10
#define EVENT_DATA_SIZE         10

#define EVN_KEY 0x00
#define KEYBOARD_DOWN 1
#define KEYBOARD_UP 0
#define LONGKEY 2

#define TPKEY_DOWN_VALUE 3900
#define SHAKE_VALUE 50
#define WHEEL_MASK(n) (n + 1000)
#define WHEEL_DMASK(n) (n - 1000)

typedef enum SUNXI_TPKEY_CMD
{
    TPADC_KEY_REV = 0,
    TPADC_KEY_WHEEL_GET_VAL,
    TPADC_KEY_END,
}tpadc_key_t;

struct sunxikbd_config
{
    unsigned int measure;
    unsigned int key_num;
    unsigned int scankeycodes[KEY_MAX_CNT];
    unsigned int key_vol[KEY_MAX_CNT];
    char *name;
};


struct sunxikbd_drv_data
{
    unsigned int scankeycodes[KEY_MAX_CNT];
    unsigned char compare_later;
    unsigned char compare_before;
    unsigned char key_code;
    unsigned char last_key_code;
    unsigned char key_cnt;
};

int sunxi_keyboard_init(void);


#ifdef __cplusplus
}
#endif

#endif
