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
#include "mod_init_i.h"
#include "scnlock_uipara.h"

__screen_lock_para_t  scnlock_para =
{
    {110, 160, 0, 0, 110, 160, 0, 160, 110, 160, 0},
    {0, 0, 110, 160},
    0,
    {8, 136, 94, 24},
    0,
    0,
    0,
    0,
    0,
    {0, 0, 110, 160},
};

void init_scnlock_uipara(void)
{
    /*scnlock_para.screen_lock_layer                = {110,160,0,0,110,160,0,160,110,160};
    scnlock_para.frm_rect                       = {0,0,110,160};
    scnlock_para.text                           = STRING_SCREENUNLOCK;
    scnlock_para.text_rect                      = {8,136,94,24}
    scnlock_para.screen_lock_icon               = ID_INIT_SCNLOCK_INIT_BMP;
    scnlock_para.screen_unlock_start_icon       = ID_INIT_SCNUNLOCK_START_BMP;
    scnlock_para.screen_unlock1_icon            = ID_INIT_SCNUNLOCK1_BMP;
    scnlock_para.screen_unlock2_icon            = ID_INIT_SCNUNLOCK2_BMP;
    scnlock_para.screen_unlock_end_icon         = ID_INIT_SCNUNLOCK_END_BMP;
    scnlock_para.lock_icon_rect                 = {0,0,110,160};*/
    return;
}
