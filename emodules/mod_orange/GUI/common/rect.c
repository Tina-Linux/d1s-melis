/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    rect.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  rect region impelement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>       <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#include "common_i.h"


/*
**********************************************************************************************************************
*                                               IsRectEmpty
*
* author:
*
* date:             2009-7-21
*
* Description:      isrectempty
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/**************************** __gui_rect_tangle support ******************************/
__bool IsRectEmpty(const __gui_rect_t *prc)
{
    if (prc->left == prc->right)
    {
        return ORANGE_TRUE;
    }

    if (prc->top == prc->bottom)
    {
        return ORANGE_TRUE;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               EqualRect
*
* author:
*
* date:             2009-7-21
*
* Description:      equalrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool EqualRect(const __gui_rect_t *prc1, const __gui_rect_t *prc2)
{
    if (prc1->left != prc2->left)
    {
        return ORANGE_FALSE;
    }

    if (prc1->top != prc2->top)
    {
        return ORANGE_FALSE;
    }

    if (prc1->right != prc2->right)
    {
        return ORANGE_FALSE;
    }

    if (prc1->bottom != prc2->bottom)
    {
        return ORANGE_FALSE;
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               NormalizeRect
*
* author:
*
* date:             2009-7-21
*
* Description:      normalizerect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void NormalizeRect(__gui_rect_t *prect)
{
    int iTemp;

    if (prect->left > prect->right)
    {
        iTemp = prect->left;
        prect->left = prect->right;
        prect->right = iTemp;
    }

    if (prect->top > prect->bottom)
    {
        iTemp = prect->top;
        prect->top = prect->bottom;
        prect->bottom = iTemp;
    }
}

/*
**********************************************************************************************************************
*                                               IsCovered
*
* author:
*
* date:             2009-7-21
*
* Description:      iscovered
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool IsCovered(const __gui_rect_t *prc1, const __gui_rect_t *prc2)
{
    if (prc1->left < prc2->left
        || prc1->top < prc2->top
        || prc1->right > prc2->right
        || prc1->bottom > prc2->bottom)
    {
        return ORANGE_FALSE;
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               IntersectRect
*
* author:
*
* date:             2009-7-21
*
* Description:      intersectrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool IntersectRect(__gui_rect_t *pdrc, const __gui_rect_t *psrc1, const __gui_rect_t *psrc2)
{
    pdrc->left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
    pdrc->top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
    pdrc->right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
    pdrc->bottom = (psrc1->bottom < psrc2->bottom)
                   ? psrc1->bottom : psrc2->bottom;

    if (pdrc->left >= pdrc->right || pdrc->top >= pdrc->bottom)
    {
        return ORANGE_FALSE;
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               DoesIntersect
*
* author:
*
* date:             2009-7-21
*
* Description:      doesintersect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool DoesIntersect(const __gui_rect_t *psrc1, const __gui_rect_t *psrc2)
{
    __s32       left;
    __s32       top;
    __s32       right;
    __s32       bottom;
    left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
    top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
    right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
    bottom = (psrc1->bottom < psrc2->bottom)
             ? psrc1->bottom : psrc2->bottom;

    if (left >= right || top >= bottom)
    {
        return ORANGE_FALSE;
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               UnionRect
*
* author:
*
* date:             2009-7-21
*
* Description:      unionrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool UnionRect(__gui_rect_t *pdrc, const __gui_rect_t *psrc1, const __gui_rect_t *psrc2)
{
    __gui_rect_t src1;
    __gui_rect_t src2;
    orange_memcpy(&src1, psrc1, sizeof(__gui_rect_t));
    orange_memcpy(&src2, psrc2, sizeof(__gui_rect_t));
    NormalizeRect(&src1);
    NormalizeRect(&src2);

    if (src1.left == src2.left && src1.right == src2.right)
    {
        if (src1.top <= src2.top && src2.top <= src1.bottom)
        {
            pdrc->left = src1.left;
            pdrc->right = src1.right;
            pdrc->top = src1.top;
            pdrc->bottom = MAX(src1.bottom, src2.bottom);
            return ORANGE_TRUE;
        }
        else if (src1.top >= src2.top && src2.bottom >= src1.top)
        {
            pdrc->left = src1.left;
            pdrc->right = src1.right;
            pdrc->top = src2.top;
            pdrc->bottom = MAX(src1.bottom, src2.bottom);
            return ORANGE_TRUE;
        }

        return ORANGE_FALSE;
    }

    if (src1.top == src2.top && src1.bottom == src2.bottom)
    {
        if (src1.left <= src2.left && src2.left <= src1.right)
        {
            pdrc->top = src1.top;
            pdrc->bottom = src1.bottom;
            pdrc->left = src1.left;
            pdrc->right = MAX(src1.right, src2.right);
            return ORANGE_TRUE;
        }
        else if (src1.left >= src2.left && src2.right >= src1.left)
        {
            pdrc->top = src1.top;
            pdrc->bottom = src1.bottom;
            pdrc->left = src2.left;
            pdrc->right = MAX(src1.right, src2.right);
            return ORANGE_TRUE;
        }

        return ORANGE_FALSE;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GetBoundRect
*
* author:
*
* date:             2009-7-21
*
* Description:      getboundrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GetBoundRect(__gui_rect_t *pdrc,  const __gui_rect_t *psrc1, const __gui_rect_t *psrc2)
{
    __gui_rect_t src1;
    __gui_rect_t src2;
    orange_memcpy(&src1, psrc1, sizeof(__gui_rect_t));
    orange_memcpy(&src2, psrc2, sizeof(__gui_rect_t));
    NormalizeRect(&src1);
    NormalizeRect(&src2);
    pdrc->left = (src1.left < src2.left) ? src1.left : src2.left;
    pdrc->top  = (src1.top < src2.top) ? src1.top : src2.top;
    pdrc->right = (src1.right > src2.right) ? src1.right : src2.right;
    pdrc->bottom = (src1.bottom > src2.bottom)
                   ? src1.bottom : src2.bottom;
}

/*
**********************************************************************************************************************
*                                               SubtractRect
*
* author:
*
* date:             2009-7-21
*
* Description:      subtractrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SubtractRect(__gui_rect_t *rc, const __gui_rect_t *psrc1, const __gui_rect_t *psrc2)
{
    __gui_rect_t src, rcExpect, *prcExpect;
    int nCount = 0;
    src = *psrc1;
    rcExpect = *psrc2;
    prcExpect = &rcExpect;

    if (!DoesIntersect(&src, prcExpect))
    {
        nCount = 1;
        rc[0] = src;
    }
    else
    {
        if (prcExpect->top > src.top)
        {
            rc[nCount].left  = src.left;
            rc[nCount].top   = src.top;
            rc[nCount].right = src.right;
            rc[nCount].bottom = prcExpect->top;
            nCount++;
            src.top = prcExpect->top;
        }

        if (prcExpect->bottom < src.bottom)
        {
            rc[nCount].top  = prcExpect->bottom;
            rc[nCount].left   = src.left;
            rc[nCount].right = src.right;
            rc[nCount].bottom = src.bottom;
            nCount++;
            src.bottom = prcExpect->bottom;
        }

        if (prcExpect->left > src.left)
        {
            rc[nCount].left  = src.left;
            rc[nCount].top   = src.top;
            rc[nCount].right = prcExpect->left;
            rc[nCount].bottom = src.bottom;
            nCount++;
        }

        if (prcExpect->right < src.right)
        {
            rc[nCount].left  = prcExpect->right;
            rc[nCount].top   = src.top;
            rc[nCount].right = src.right;
            rc[nCount].bottom = src.bottom;
            nCount++;
        }
    }

    return nCount;
}

/*
**********************************************************************************************************************
*                                               SetRectEmpty
*
* author:
*
* date:             2009-7-21
*
* Description:      设置矩形数据结构为空
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void SetRectEmpty(__gui_rect_t *prc)
{
    (prc)->left = (prc)->top = (prc)->right = (prc)->bottom = 0;
}

void SetRect(__gui_rect_t *prc, __s32 left, __s32 top, __s32 right, __s32 bottom)
{
    (prc)->left = left;
    (prc)->top = top;
    (prc)->right = right;
    (prc)->bottom = bottom;
}

void CopyRect(__gui_rect_t *pdrc, const __gui_rect_t *psrc)
{
    (pdrc)->left = (psrc)->left;
    (pdrc)->top = (psrc)->top;
    (pdrc)->right = (psrc)->right;
    (pdrc)->bottom = (psrc)->bottom;
}

void OffsetRect(__gui_rect_t *prc, __s32 x, __s32 y)
{
    (prc)->left += x;
    (prc)->top += y;
    (prc)->right += x;
    (prc)->bottom += y;
}

void InflateRect(__gui_rect_t *prc, __s32 cx, __s32 cy)
{
    (prc)->left -= cx;
    (prc)->top -= cy;
    (prc)->right += cx;
    (prc)->bottom += cy;
}

void InflateRectToPt(__gui_rect_t *prc, __s32 x, __s32 y)
{
    if ((x) < (prc)->left)
    {
        (prc)->left = (x);
    }

    if ((y) < (prc)->top)
    {
        (prc)->top = (y);
    }

    if ((x) > (prc)->right)
    {
        (prc)->right = (x);
    }

    if ((y) > (prc)->bottom)
    {
        (prc)->bottom = (y);
    }
}

__bool PtInRect(const __gui_rect_t *prc, __s32 x, __s32 y)
{
    if (x >= prc->left && x < prc->right && y >= prc->top && y < prc->bottom)
    {
        return ORANGE_TRUE;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               SysRectToGuiRect
*
* author:
*
* date:             2009-7-24
*
* Description:      sysrecttoguirect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GuiRectToSysRect(__gui_rect_t *GuiRect, RECT *SysRect)
{
    if ((!SysRect) || (!GuiRect))
    {
        return ORANGE_FAIL;
    }

    SysRect->x      = GuiRect->left;
    SysRect->y      = GuiRect->top;
    SysRect->width  = GuiRect->right - GuiRect->left;
    SysRect->height  = GuiRect->bottom - GuiRect->top;
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               SysRectToGuiRect
*
* author:
*
* date:             2009-7-24
*
* Description:      sysrecttoguirect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SysRectToGuiRect(RECT *SysRect, __gui_rect_t *GuiRect)
{
    if ((!SysRect) || (!GuiRect))
    {
        return ORANGE_FAIL;
    }

    GuiRect->left       = SysRect->x;
    GuiRect->top        = SysRect->y;
    GuiRect->right      = SysRect->x + SysRect->width - 1;
    GuiRect->bottom     = SysRect->y + SysRect->height - 1;
    return ORANGE_OK;
}
