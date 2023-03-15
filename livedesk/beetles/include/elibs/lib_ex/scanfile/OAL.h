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
#ifndef     __OAL_H__
#define     __OAL_H__

//#include  <stdarg.h>
//#include  "epdk.h"
#include       <emodules/mod_defs.h>

#define         rat_atoi(str)       ((int)eLIBs_strtol(str, NULL, 10))

#if      0
extern int  MCnt;
#define  rat_malloc(size)       (MCnt++, \
                                 __inf("MALLOC: [%s][%d], time:%d\n", __FILE__, __LINE__, MCnt),\
                                 eLIBs_malloc(size))

#define  rat_realloc(p, size)   (MCnt++, \
                                 __inf("REALLOC:[%s][%d], time:%d\n", __FILE__, __LINE__, MCnt),\
                                 eLIBs_realloc(p, size))

#define  rat_free(p)            (MCnt--, \
                                 __inf("FREE:[%s][%d], time:%d\n", __FILE__, __LINE__, MCnt),\
                                 eLIBs_free(p))
#else
#define  rat_malloc(size)       eLIBs_malloc(size)
#define  rat_realloc(p, size)   eLIBs_realloc(p, size)
#define  rat_free(p)            eLIBs_free(p)
#endif  /* DEBUG */


#define  RAT_ALIGN(val, align)      (((val) + ((align) - 1)) & ~((align) - 1))

extern void     rat_sprintf(char *buffer, const char *str, ...);
extern double   rat_atof(const char *string);
extern void     rat_dtoa(double value, char *string);

#endif      /* __OAL_H__ */
