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

#include "scan_proc_uipara.h"

static dvb_scan_proc_uipara_t dvb_scan_proc_uipara =
    /* _480_270  */
{
    {0, 50, 480, 44},
    {0, 50 + 44, 480, 44},
    {0, 50 + 44 + 44, 480, 44},
    {(480 - 300) / 2, (190)}
};

dvb_scan_proc_uipara_t *submenu_dvb_scan_proc_get_uipara()
{
    __s32           screen_width;
    __s32           screen_height;

    /* get lcd size */
    //居中显示
    if (dsk_display_get_size((__s32 *)&screen_width, (__s32 *)&screen_height) == EPDK_OK)
    {
        dvb_scan_proc_uipara.dvb_scan_str.x = 0;
        dvb_scan_proc_uipara.dvb_scan_str.y = (screen_height - 204) / 2;
        dvb_scan_proc_uipara.dvb_scan_str.width = screen_width;
        dvb_scan_proc_uipara.dvb_scan_str.height = 48;
        dvb_scan_proc_uipara.dvb_scan_freq.x = 0;
        dvb_scan_proc_uipara.dvb_scan_freq.y = (screen_height - 204) / 2 + 48;
        dvb_scan_proc_uipara.dvb_scan_freq.width = screen_width;
        dvb_scan_proc_uipara.dvb_scan_freq.height = 48;
        dvb_scan_proc_uipara.dvb_scan_ch.x = 0;
        dvb_scan_proc_uipara.dvb_scan_ch.y = (screen_height - 204) / 2 + 48 * 2;
        dvb_scan_proc_uipara.dvb_scan_ch.width = screen_width;
        dvb_scan_proc_uipara.dvb_scan_ch.height = 48;
        dvb_scan_proc_uipara.dvb_scan_proc_bmp_pos.x = (screen_width - 300) / 2;
        dvb_scan_proc_uipara.dvb_scan_proc_bmp_pos.y = (screen_height - 204) / 2 + 48 * 3;
    }

    return &dvb_scan_proc_uipara;
}


