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
#ifndef _ENCODE_H_
#define _ENCODE_H_

#include "typedef.h"

// 一次加密16个字节
#define ENCODE_LEN 16

typedef void *HENCODE;

/*
*********************************************************************************************************
* Function   : Initial
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
HENCODE Initial(void *param0, void *param1, __u32 index);

#if 0
__s32 Encode(HENCODE hEncode, void *ibuf, void *obuf);
#endif

/*
*********************************************************************************************************
* Function   : Decode
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 Decode(HENCODE hEncode, void *ibuf, void *obuf);

/*
*********************************************************************************************************
* Function   :
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 UnInitial(HENCODE hEncode);

typedef HENCODE(*pInitial)(void *param0, void *param1, __u32 index);
typedef __s32(*pEnDecode)(HENCODE hEncode, void *ibuf, void *obuf);
typedef __s32(*pUnInitial)(HENCODE hEncode);

typedef struct tag_RC_ENDECODE_IF
{
    HENCODE     handle;
    pInitial    Initial;
    pEnDecode   EnDecode;
    pUnInitial  UnInitial;
} RC_ENDECODE_IF_t;

#endif /* _ENCODE_H_ */
