/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    common_e.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  common extern interface definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef _COMMON_E_H_
#define _COMMON_E_H_

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

extern void     SetRectEmpty(__gui_rect_t *prc);
extern void     SetRect(__gui_rect_t *prc, __s32 left, __s32 top, __s32 right, __s32 bottom);
extern void     CopyRect(__gui_rect_t *pdrc, const __gui_rect_t *psrc);
extern void     OffsetRect(__gui_rect_t *prc, __s32 x, __s32 y);
extern void     InflateRect(__gui_rect_t *prc, __s32 cx, __s32 cy);
extern void     InflateRectToPt(__gui_rect_t *prc, __s32 x, __s32 y);
extern __bool   PtInRect(const __gui_rect_t *prc, __s32 x, __s32 y);
#endif  /* _COMMON_E_H_ */


