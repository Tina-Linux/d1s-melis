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
#ifndef _COMMON_I_H_
#define _COMMON_I_H_

#include "gui_i.h"

/*内存块空闲*/
#define MBS_FREE        0x0000

/*内存块*/
#define MBS_SPECIAL     0x0001

/*内存块被使用*/
#define MBS_USED        0x0002

extern void     InitMemBlockLink(pmemblock heap, __u32 bd_size, __u32 heap_size);
extern void    *MemBlockAlloc(pmemblock heap);
extern void     MemBlockFree(pmemblock heap, void *data);
extern void     DestroyMemBlockLink(pmemblock heap);

extern __s32    GuiRectToSysRect(__gui_rect_t *GuiRect, RECT *SysRect);
extern __s32    SysRectToGuiRect(RECT *SysRect, __gui_rect_t *GuiRect);
extern __bool   IsRectEmpty(const __gui_rect_t *prc);
extern __bool   EqualRect(const __gui_rect_t *prc1, const __gui_rect_t *prc2);
extern void     NormalizeRect(__gui_rect_t *prect);
extern __bool   IsCovered(const __gui_rect_t *prc1, const __gui_rect_t *prc2);
extern __bool   IntersectRect(__gui_rect_t *pdrc, const __gui_rect_t *psrc1, const __gui_rect_t *psrc2);
extern __bool   DoesIntersect(const __gui_rect_t *psrc1, const __gui_rect_t *psrc2);
extern void     GetBoundRect(__gui_rect_t *pdrc,  const __gui_rect_t *psrc1, const __gui_rect_t *psrc2);
extern __bool   UnionRect(__gui_rect_t *pdrc, const __gui_rect_t *psrc1, const __gui_rect_t *psrc2);
extern __s32    SubtractRect(__gui_rect_t *rc, const __gui_rect_t *psrc1, const __gui_rect_t *psrc2);

extern __bool   PtInRegion(pclipregn    region, __s32 x, __s32 y);
extern __bool   RectInRegion(pclipregn region, const __gui_rect_t *rect);
extern void     InitClipRgn(pclipregn pRgn, pmemblock heap);
extern void     GetClipRgnBoundRect(pclipregn pRgn, pguirect pRect);
extern __bool   IsEmptyClipRgn(const __gui_clipregn_t *pRgn);
extern void     EmptyClipRgn(pclipregn pRgn);
extern __bool   SetClipRgn(pclipregn pRgn, const __gui_rect_t *pRect);
extern __bool   ClipRgnCopy(pclipregn   pDstRgn, const pclipregn pSrcRgn);
extern __bool   ClipRgnIntersect(pclipregn  dst, const pclipregn  src1, const pclipregn  src2);
extern __bool   SubtractRegion(pclipregn  rgnD, const pclipregn  rgnM, const pclipregn  rgnS);
extern __bool   UnionRegion(pclipregn  dst, const pclipregn  src1, const pclipregn  src2);
extern __bool   XorRegion(pclipregn  dst, const pclipregn  src1, const pclipregn  src2);
extern __bool   AddClipRect(pclipregn region, const  pguirect rect);
extern __bool   IntersectClipRect(pclipregn region, const pguirect rect);
extern __bool   SubtractClipRect(pclipregn region, const __gui_rect_t *rect);
extern void     OffsetRegion(pclipregn region, __s32 x, __s32 y);

#endif  /* _GUI_I_H_ */
