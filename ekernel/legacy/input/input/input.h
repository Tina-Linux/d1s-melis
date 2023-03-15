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
#ifndef __INPUT_H__
#define __INPUT_H__
#include <typedef.h>
#include <sys_device.h>
#include <sys_input.h>
#include <ktype.h>
#include <port.h>

#define INPUT_DEVICE_ID_EV_MAX              0x1f
#define INPUT_DEVICE_ID_KEY_MIN_INTERESTING 0x71
#define INPUT_DEVICE_ID_KEY_MAX             0x2ff
#define INPUT_DEVICE_ID_REL_MAX             0x0f
#define INPUT_DEVICE_ID_ABS_MAX             0x3f
#define INPUT_DEVICE_ID_LED_MAX             0x0f
#define INPUT_DEVICE_ID_SND_MAX             0x07
#define INPUT_DEVICE_ID_FF_MAX              0x7f

#define INPUT_DEVICE_ID_MATCH_PRODUCT       4
#define INPUT_DEVICE_ID_MATCH_VERSION       8

#define INPUT_DEVICE_ID_MATCH_EVBIT         0x0010
#define INPUT_DEVICE_ID_MATCH_KEYBIT        0x0020
#define INPUT_DEVICE_ID_MATCH_RELBIT        0x0040
#define INPUT_DEVICE_ID_MATCH_ABSBIT        0x0080
#define INPUT_DEVICE_ID_MATCH_LEDBIT        0x0200
#define INPUT_DEVICE_ID_MATCH_SNDBIT        0x0400

typedef struct __INPUT_LID
{
    uint16_t    flags;
    uint16_t    product;
    uint16_t    version;

    uint32_t    evbit[INPUT_DEVICE_ID_EV_MAX / BITS_PER_LONG + 1];
    uint32_t    keybit[INPUT_DEVICE_ID_KEY_MAX / BITS_PER_LONG + 1];
    uint32_t    relbit[INPUT_DEVICE_ID_REL_MAX / BITS_PER_LONG + 1];
    uint32_t    absbit[INPUT_DEVICE_ID_ABS_MAX / BITS_PER_LONG + 1];
    uint32_t    ledbit[INPUT_DEVICE_ID_LED_MAX / BITS_PER_LONG + 1];
    uint32_t    sndbit[INPUT_DEVICE_ID_SND_MAX / BITS_PER_LONG + 1];
    uint32_t    ffbit[INPUT_DEVICE_ID_FF_MAX / BITS_PER_LONG + 1];
} __input_lid_t;

typedef struct __INPUT_GRABER __input_graber_t;

typedef struct __INPUT_LDEV
{
    /* 以下4个成员为注册通用设备所用 */
    const char      *classname;
    const char      *name;
    const __dev_devop_t   *ops;
    void            *parg;

    /* 指向通用设备 */
    __hdle          devnode;

    /* 在全局逻辑设备表数组中的index序号 */
    unsigned char   seq;

    /* 以下两个成员用于建立逻辑输入设备和物理输入设备的拓扑关系 */
    __input_dev_t   *children[INPUT_CHILDREN_MAX];  /* 所有物理输入设备句柄数组 */
    uint32_t        children_cnt;                /* 物理输入设备个数         */

    /*
     * 逻辑设备的事件处理函数成员，input_event()函数将根据event类型把event分发到
     * 逻辑输入设备或物理输入设备，该成员函数将处理发送给逻辑输入设备的event
     */
    int32_t         (*event)(__input_dev_t *dev, __u32 type, __u32 code, __s32 value);

    /*
     * 逻辑事件缓冲区，当收到input_sync()时，将把该缓冲区的事件缓冲区传递给每个该
     * 逻辑设备的每个graber的回调函数。
     */
    __hdle          ev_sem;
    uint32_t        evbuf[INPUT_EV_BUFFER_MAXLEN];
    int32_t         ev_head;            /* 事件缓冲区的首指针   */
    int32_t         ev_tail;            /* 事件缓冲区的尾指针   */
    int32_t         ev_len;             /* 事件缓冲区的长度     */
    int32_t         ev_maxlen;

    /*
      * 该指针指向一个逻辑输入设备的身份识别表组。最后一组一定以NULL结尾。
      * 逻辑输入设备可以支持多种类型的物理输入设备，注册物理输入设备时，将会使用该
      * 身份识别表组来识别是否物理输入设备被逻辑输入设备支持。只要物理输入设备符合
      * 其中的一组，将被认为物理输入设备被该逻辑输入设备支持。
      */
    const __input_lid_t     *id_table;

    /*
      * 逻辑输入设备的上层用户(apps/desktop_managers etc.)每调用一次grab系统调用，
      * 将在逻辑输入设备上注册一个graber，该grabers指针是所有这些graber的链表首指
      * 针。
      */
    __input_graber_t        *grabers;

    /* 逻辑输入设备的graber扫描子线程id */
    __hdle          tid;
} __input_ldev_t;

typedef struct __INPUT_GRABER
{
    __pCBK_t                callback;  /* 上层应用程序创建graber时，传递的回调函数 */
    void                    *pGrabArg;  /*  注册回调函数时传入的私有句柄            */
    __input_ldev_t          *ldev;      /* input逻辑设备句柄                        */
    struct __INPUT_GRABER   *next;
} __input_graber_t;


extern int32_t INPUT_CoreInit(void);
extern int32_t INPUT_CoreExit(void);
extern int32_t IsEventSupported(uint32_t code, unsigned long *bm, uint32_t max);
extern void  INPUT_RepeatKey(void *parg);
extern int32_t INPUT_SetRepPeriod(int32_t LDevID, uint32_t period);
extern int32_t esINPUT_RegLdev(__input_ldev_t *ldev);
extern int32_t esINPUT_UnregLdev(__input_ldev_t *ldev);

#endif  /* __INPUT_H__ */