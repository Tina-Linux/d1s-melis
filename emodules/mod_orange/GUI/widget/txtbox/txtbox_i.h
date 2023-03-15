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
#ifndef __TXTBOX_I_H__
#define __TXTBOX_I_H__

#include "txtbox_e.h"
#include "txtlayer.h"


#define TXTBOX_FRM_NUMBER        64
#define TXTBOX_FRM_TXQ_DEPTH     16
#define TXTBOX_FRM_RXQ_DEPTH     64

/*txtbox控件所在图层的状态信息*/
typedef enum TXTBOX_LYRSTS
{
    TXTBOX_WAKEUP,                    /*txtbox占有framebuffer*/
    TXTBOX_SLEEP                      /*txtboxframebuffer释放*/
} TXTBOX_LYRSTS_E;

typedef enum TXTBOX_STATUS
{
    TXTBOX_ONRES        = 0,
    TXTBOX_REQDEL       = 1,
    TXTBOX_ONDRAW       = 1 << 1,
    TXTBOX_DRAWFINISH   = 1 << 2,
    TXTBOX_INIT         = 1 << 3
} TXTBOX_STATUS_e;

typedef struct tag_TXTBOX_DATA
{
    txtlayer_man_t          *txtlayer;
    __u32                    mode;
    H_WIN                    handle;
    __pGUI_WIN_CB            user_cb;
    __u32                    status;
    __u32                    item_w;
    TXTBOX_LYRSTS_E          lyrsts;
} __txtbox_data_t;

__u32  GUI_TxtboxOnDraw(H_WIN txt);

#endif
