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

#include "alarm_select_song_uipara.h"

static alarm_sel_song_uipara_t alarm_sel_song_uipara ;

alarm_sel_song_uipara_t *alarm_sel_song_get_uipara(void)
{
    alarm_sel_song_uipara.FbRect.x = 0;
    alarm_sel_song_uipara.FbRect.y = 0;
    alarm_sel_song_uipara.FbRect.w = 240;
    alarm_sel_song_uipara.FbRect.h = 200;
    alarm_sel_song_uipara.ScnWinRect.x = (480 - 240) / 2;
    alarm_sel_song_uipara.ScnWinRect.y = (272 - 200) / 2;
    alarm_sel_song_uipara.ScnWinRect.w = 240;
    alarm_sel_song_uipara.ScnWinRect.h = 200;
    alarm_sel_song_uipara.SrcWinRect.x  = 0;
    alarm_sel_song_uipara.SrcWinRect.y  = 0;
    alarm_sel_song_uipara.SrcWinRect.w  = 240;
    alarm_sel_song_uipara.SrcWinRect.h  = 200;
    alarm_sel_song_uipara.ListBarAreaRect.x = 30;
    alarm_sel_song_uipara.ListBarAreaRect.y = 5;
    alarm_sel_song_uipara.ListBarAreaRect.w = 182;
    alarm_sel_song_uipara.ListBarAreaRect.h = 144;
    alarm_sel_song_uipara.ListBarItemRect.x = 1;    //ListBarItemRect 是相对于ListBarAreaRect 来说的
    alarm_sel_song_uipara.ListBarItemRect.y = 0;
    alarm_sel_song_uipara.ListBarItemRect.w = 180;
    alarm_sel_song_uipara.ListBarItemRect.h = 24;
    alarm_sel_song_uipara.OkBmpRect.x = 30;
    alarm_sel_song_uipara.OkBmpRect.y = 162;
    alarm_sel_song_uipara.OkBmpRect.w = 37;
    alarm_sel_song_uipara.OkBmpRect.h = 18;
    alarm_sel_song_uipara.CancleBmpRect.x = 164;
    alarm_sel_song_uipara.CancleBmpRect.y = 162;
    alarm_sel_song_uipara.CancleBmpRect.w = 37;
    alarm_sel_song_uipara.CancleBmpRect.h = 18;
    alarm_sel_song_uipara.ListenTestBmpRect.x = 97;
    alarm_sel_song_uipara.ListenTestBmpRect.y = 162;
    alarm_sel_song_uipara.ListenTestBmpRect.w = 37;
    alarm_sel_song_uipara.ListenTestBmpRect.h = 18;
    return &alarm_sel_song_uipara;
}

