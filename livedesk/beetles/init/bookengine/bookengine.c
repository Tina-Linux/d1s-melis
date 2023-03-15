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
#include <log.h>

static __u8                bookengine_mid = 0;        // mod id
static __mp               *bookengine_mod = NULL;               // mod句柄

__s32 bookengine_open(void)
{
    /*already open*/
    if (bookengine_mod)
    {
        return 0;
    }

    if (0== bookengine_mid)
    {
        /* install bookengine module */
        bookengine_mid = esMODS_MInstall(BEETLES_APP_ROOT"mod\\bookengine.mod", 0);

        if (!bookengine_mid)
        {
            __msg("Install bookengine module failed");
            return -1;
        }

        __msg("install bookengine OK");
    }

    if (NULL == bookengine_mod)
    {
        /* open bookengine module */
        bookengine_mod = esMODS_MOpen(bookengine_mid, 0);

        if (bookengine_mod == (__mp *)0)
        {
            if (bookengine_mid)
            {
                esMODS_MUninstall(bookengine_mid);
                bookengine_mid  = 0;
            }

            __msg("esMODS_MOpen failed");
            return -1;
        }

        __msg("bookengine module open OK");
    }

    return 0;
}

__s32 bookengine_close(void)
{
    if (bookengine_mod)
    {
        esMODS_MClose(bookengine_mod);
    }

    if (bookengine_mid)
    {
        esMODS_MUninstall(bookengine_mid);
    }

    bookengine_mod = NULL;
    bookengine_mid = 0;
    return 0;
}
