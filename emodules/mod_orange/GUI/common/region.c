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
#include "common_i.h"

typedef void (*voidProcp1)(__gui_clipregn_t *region, const __gui_cliprect_t *r1, const __gui_cliprect_t *r1End,
                           const __gui_cliprect_t *r2, const __gui_cliprect_t *r2End, __s32 top, __s32 bottom);
typedef void (*voidProcp2)(__gui_clipregn_t *region, const __gui_cliprect_t *r, const __gui_cliprect_t *rEnd,
                           __s32 top, __s32 bottom);

#define ClipRectAlloc   MemBlockAlloc
#define FreeClipRect    MemBlockFree
/*  1 if two RECTs overlap.
 *  0 if two RECTs do not overlap.
 */
#define EXTENTCHECK(r1, r2) \
    ((r1)->right > (r2)->left && \
     (r1)->left < (r2)->right && \
     (r1)->bottom > (r2)->top && \
     (r1)->top < (r2)->bottom)

/*
 * Allocate a new clipping rect and add it to the region.
 */
#define NEWCLIPRECT(region, rect) \
    {\
        rect = ClipRectAlloc(region->heap);\
        rect->next = NULL;\
        rect->prev = region->tail;\
        if (region->tail)\
            region->tail->next = rect;\
        region->tail = rect;\
        if (region->head == NULL)\
            region->head = rect;\
    }

#define REGION_NOT_EMPTY(region) region->head

#define INRECT(r, x, y) \
    ( ( ((r).right >  x)) && \
      ( ((r).left <= x)) && \
      ( ((r).bottom >  y)) && \
      ( ((r).top <= y)) )


/*
**********************************************************************************************************************
*                                               PtInRegion
*
* author:
*
* date:             2009-7-21
*
* Description:      判断某个点是否在某一剪切域中
*
* parameters:
*
* return:           if success return lemon_ORANGE_TRUE
*                   if fail return the number of lemon_ORANGE_FALSE
* modify history:
**********************************************************************************************************************
*/

__bool PtInRegion(pclipregn    region, __s32 x, __s32 y)
{
    __s32           top;
    pcliprect       cliprect = region->head;

    /* check with bounding rect of clipping region */
    if (y >= region->tail->clip_rect.bottom || y < region->head->clip_rect.top)
    {
        return ORANGE_FALSE;
    }

    /* find the ban in which this point lies */
    cliprect = region->head;

    while (cliprect && y >= cliprect->clip_rect.bottom)
    {
        cliprect = cliprect->next;
    }

    if (!cliprect)
    {
        return ORANGE_FALSE;
    }

    /* check in this ban */
    top = cliprect->clip_rect.top;

    while (cliprect && cliprect->clip_rect.top == top)
    {
        if (INRECT(cliprect->clip_rect, x, y))
        {
            return ORANGE_TRUE;
        }

        cliprect = cliprect->next;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               RectInRegion
*
* author:
*
* date:             2009-7-21
*
* Description:      判断矩形是否有一部分在某个区域内
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool RectInRegion(pclipregn region, const __gui_rect_t *rect)
{
    pcliprect cliprect = region->head;
    __bool    ret = ORANGE_FALSE;

    if (cliprect && EXTENTCHECK(&region->rcBound, rect))
    {
        while (cliprect)
        {
            if (cliprect->clip_rect.bottom <= rect->top)
            {
                cliprect = cliprect->next;
                continue;             /* not far enough down yet */
            }

            if (cliprect->clip_rect.top >= rect->bottom)
            {
                ret = ORANGE_FALSE;          /* too far down */
                break;
            }

            if (cliprect->clip_rect.right <= rect->left)
            {
                cliprect = cliprect->next;
                continue;              /* not far enough over yet */
            }

            if (cliprect->clip_rect.left >= rect->right)
            {
                cliprect = cliprect->next;
                continue;
            }

            ret = ORANGE_TRUE;
            break;
        }
    }

    return ret;
}

/*
**********************************************************************************************************************
*                                               InitClipRgn
*
* author:
*
* date:             2009-7-21
*
* Description:      初始化一个剪切域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* Init a region */
void InitClipRgn(pclipregn pRgn, pmemblock heap)
{
    pRgn->type = GUI_NULLREGION;
    SetRectEmpty(&pRgn->rcBound);
    pRgn->head = NULL;
    pRgn->tail = NULL;
    pRgn->heap = heap;   // read-only field.
}


/*
**********************************************************************************************************************
*                                               GetClipRgnBoundRect
*
* author:
*
* date:             2009-7-21
*
* Description:      返回一个区域的边界矩形
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
void GetClipRgnBoundRect(pclipregn pRgn, pguirect pRect)
{
    *pRect = pRgn->rcBound;
}

/*
**********************************************************************************************************************
*                                               IsEmptyClipRgn
*
* author:
*
* date:             2009-7-21
*
* Description:      判断一个区域是否为空区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool IsEmptyClipRgn(const __gui_clipregn_t *pRgn)
{
    if (pRgn->head == NULL)
    {
        return ORANGE_TRUE;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               EmptyClipRgn
*
* author:
*
* date:             2009-7-21
*
* Description:      清空一个区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void EmptyClipRgn(pclipregn pRgn)
{
    pcliprect pCRect, pTemp;
    pCRect = pRgn->head;

    while (pCRect)
    {
        pTemp = pCRect->next;
        FreeClipRect(pRgn->heap, pCRect);
        pCRect = pTemp;
    }

    pRgn->type = GUI_NULLREGION;
    SetRectEmpty(&pRgn->rcBound);
    pRgn->head = NULL;
    pRgn->tail = NULL;
}


/*
**********************************************************************************************************************
*                                               SetClipRgn
*
* author:
*
* date:             2009-7-21
*
* Description:      初始化一个剪切域为一个矩形
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* Reset a region */
__bool SetClipRgn(pclipregn pRgn, const __gui_rect_t *pRect)
{
    pcliprect pClipRect;

    if (IsRectEmpty(pRect))
    {
        return ORANGE_FALSE;
    }

    // empty rgn first
    EmptyClipRgn(pRgn);
    // get a new clip rect from free list
    pClipRect = ClipRectAlloc(pRgn->heap);

    if (pClipRect == NULL)
    {
        return ORANGE_FALSE;
    }

    // set clip rect
    pClipRect->clip_rect = *pRect;
    pClipRect->next = NULL;
    pClipRect->prev = NULL;
    pRgn->type = GUI_SIMPLEREGION;
    pRgn->head = pRgn->tail = pClipRect;
    pRgn->rcBound = *pRect;
    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               ClipRgnCopy
*
* author:
*
* date:             2009-7-21
*
* Description:      剪切域互拷
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool ClipRgnCopy(pclipregn   pDstRgn, const pclipregn pSrcRgn)
{
    pcliprect   pcr;
    pcliprect   pnewcr;
    pcliprect   prev;

    // return ORANGE_FALSE if the destination region is not an empty one.
    if (pDstRgn == pSrcRgn)
    {
        return ORANGE_FALSE;
    }

    EmptyClipRgn(pDstRgn);
    pcr = pSrcRgn->head;

    if (!pcr)
    {
        return ORANGE_TRUE;
    }

    /*分配一个新的剪切矩形*/
    pnewcr = ClipRectAlloc(pDstRgn->heap);
    pDstRgn->head = pnewcr;
    pnewcr->clip_rect = pcr->clip_rect;
    prev = NULL;

    while (pcr->next)
    {
        pnewcr->next        = ClipRectAlloc(pDstRgn->heap);
        pnewcr->prev        = prev;
        prev                = pnewcr;
        pcr                 = pcr->next;
        pnewcr              = pnewcr->next;
        pnewcr->clip_rect   = pcr->clip_rect;
    }

    pnewcr->prev = prev;
    pnewcr->next = NULL;
    pDstRgn->tail = pnewcr;
    pDstRgn->type = pSrcRgn->type;
    pDstRgn->rcBound = pSrcRgn->rcBound;
    return ORANGE_TRUE;
}


/*
**********************************************************************************************************************
*                                               REGION_SetExtents
*
* author:
*
* date:             2009-7-21
*
* Description:      重新计算剪切域的边界矩形区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
static void REGION_SetExtents(pclipregn region)
{
    pcliprect       cliprect;
    __gui_rect_t    *pExtents;

    if (region->head == NULL)
    {
        region->rcBound.left = 0;
        region->rcBound.top = 0;
        region->rcBound.right = 0;
        region->rcBound.bottom = 0;
        return;
    }

    pExtents = &region->rcBound;
    pExtents->left = region->head->clip_rect.left;
    pExtents->top = region->head->clip_rect.top;
    pExtents->right = region->tail->clip_rect.right;
    pExtents->bottom = region->tail->clip_rect.bottom;
    cliprect = region->head;

    while (cliprect)
    {
        if (cliprect->clip_rect.left < pExtents->left)
        {
            pExtents->left = cliprect->clip_rect.left;
        }

        if (cliprect->clip_rect.right > pExtents->right)
        {
            pExtents->right = cliprect->clip_rect.right;
        }

        cliprect = cliprect->next;
    }
}

/*
**********************************************************************************************************************
*                                               REGION_Coalesce
*
* author:
*
* date:             2009-8-6
*
* Description:      整合区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static pcliprect REGION_Coalesce(
    pclipregn  region,      /* Region to coalesce */
    pcliprect  prevStart,  /* start of previous band */
    pcliprect  curStart    /* start of current band */
)
{
    __gui_cliprect_t *newStart;         /* Start of new band */
    __gui_cliprect_t *pPrevRect;        /* Current rect in previous band */
    __gui_cliprect_t *pCurRect;         /* Current rect in current band */
    __gui_cliprect_t *temp;             /* Temporary clipping rect */
    __u32             curNumRects;            /* Number of rectangles in current band */
    __u32             prevNumRects;           /* Number of rectangles in previous band */
    __u32             bandtop;                /* top coordinate for current band */

    if (prevStart == NULL)
    {
        prevStart = region->head;
    }

    if (curStart == NULL)
    {
        curStart = region->head;
    }

    if (prevStart == curStart)
    {
        return prevStart;
    }

    newStart = pCurRect = curStart;
    pPrevRect = prevStart;
    temp = prevStart;
    prevNumRects = 0;

    while (temp != curStart)
    {
        prevNumRects ++;
        temp = temp->next;
    }

    /*
     * Figure out how many rectangles are in the current band. Have to do
     * this because multiple bands could have been added in REGION_RegionOp
     * at the end when one region has been exhausted.
     */
    pCurRect = curStart;
    bandtop = pCurRect->clip_rect.top;
    curNumRects = 0;

    while (pCurRect && (pCurRect->clip_rect.top == bandtop))
    {
        curNumRects ++;
        pCurRect = pCurRect->next;
    }

    if (pCurRect)
    {
        /*
         * If more than one band was added, we have to find the start
         * of the last band added so the next coalescing job can start
         * at the right place... (given when multiple bands are added,
         * this may be pointless -- see above).
         */
        temp = region->tail;

        while (temp->prev->clip_rect.top == temp->clip_rect.top)
        {
            temp = temp->prev;
        }

        newStart = temp;
    }

    if ((curNumRects == prevNumRects) && (curNumRects != 0))
    {
        pCurRect = curStart;

        /*
         * The bands may only be coalesced if the bottom of the previous
         * matches the top scanline of the current.
         */
        if (pPrevRect->clip_rect.bottom == pCurRect->clip_rect.top)
        {
            /*
             * Make sure the bands have rects in the same places. This
             * assumes that rects have been added in such a way that they
             * cover the most area possible. I.e. two rects in a band must
             * have some horizontal space between them.
             */
            do
            {
                if ((pPrevRect->clip_rect.left != pCurRect->clip_rect.left) ||
                    (pPrevRect->clip_rect.right != pCurRect->clip_rect.right))
                {
                    /*
                     * The bands don't line up so they can't be coalesced.
                     */
                    return newStart;
                }

                pPrevRect = pPrevRect->next;
                pCurRect = pCurRect->next;
            } while (--prevNumRects);

            /*
             * If only one band was added to the region, we have to backup
             * newStart to the start of the previous band.
             */
            if (pCurRect == NULL)
            {
                newStart = prevStart;
            }

            /*
             * The bands may be merged, so set the bottom of each rect
             * in the previous band to that of the corresponding rect in
             * the current band.
             */
            /*
             * for implementation of MiniGUI, we should free
             * the clipping rects merged.
             */
            pCurRect = curStart;
            pPrevRect = prevStart;

            do
            {
                pPrevRect->clip_rect.bottom = pCurRect->clip_rect.bottom;
                pPrevRect = pPrevRect->next;

                if (pCurRect->next)
                {
                    pCurRect->next->prev = pCurRect->prev;
                }
                else
                {
                    region->tail = pCurRect->prev;
                }

                if (pCurRect->prev)
                {
                    pCurRect->prev->next = pCurRect->next;
                }
                else
                {
                    region->head = pCurRect->next;
                }

                temp = pCurRect->next;
                FreeClipRect(region->heap, pCurRect);
                pCurRect = temp;
            } while (--curNumRects);

            /*
             *
             * If more than one band was added to the region, copy the
             * other bands down. The assumption here is that the other bands
             * came from the same region as the current one and no further
             * coalescing can be done on them since it's all been done
             * already... newStart is already in the right place.
             */
            /* no need to copy for implementation of MiniGUI -- they are freed.
            if (temp == regionEnd) {
                newStart = prevStart;
            }

            else {
                do {
                    *pPrevRect++ = *pCurRect++;
                } while (pCurRect != regionEnd);
            }
            */
        }
    }

    return (newStart);
}

/***********************************************************************
 *           REGION_RegionOp
 *
 *      Apply an operation to two regions. Called by Union,
 *      Xor, Subtract, Intersect...
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      The new region is overwritten.
 *
 * Notes:
 *      The idea behind this function is to view the two regions as sets.
 *      Together they cover a rectangle of area that this function divides
 *      into horizontal bands where points are covered only by one region
 *      or by both. For the first case, the nonOverlapFunc is called with
 *      each the band and the band's upper and lower rcBound. For the
 *      second, the overlapFunc is called to process the entire band. It
 *      is responsible for clipping the rectangles in the band, though
 *      this function provides the boundaries.
 *      At the end of each band, the new region is coalesced, if possible,
 *      to reduce the number of rectangles in the region.
 *
 */
static void REGION_RegionOp(
    pclipregn  newReg, /* Place to store result */
    const pclipregn  reg1,   /* First region in operation */
    const pclipregn  reg2,   /* 2nd region in operation */
    voidProcp1 overlapFunc,     /* Function to call for over-lapping bands */
    voidProcp2 nonOverlap1Func, /* Function to call for non-overlapping bands in region 1 */
    voidProcp2 nonOverlap2Func  /* Function to call for non-overlapping bands in region 2 */
)
{
    __gui_clipregn_t        my_dst;
    __gui_clipregn_t       *pdst;
    const __gui_cliprect_t *r1;                 /* Pointer into first region */
    const __gui_cliprect_t *r2;                 /* Pointer into 2d region */
    const __gui_cliprect_t *r1BandEnd;          /* End of current band in r1 */
    const __gui_cliprect_t *r2BandEnd;          /* End of current band in r2 */
    __u32                   ybot;                           /* Bottom of intersection */
    __u32                   ytop;                           /* Top of intersection */
    __gui_cliprect_t       *prevBand;                 /* start of previous band in newReg */
    __gui_cliprect_t       *curBand;                  /* start of current band in newReg */
    __u32                   top;                            /* Top of non-overlapping band */
    __u32                   bot;                            /* Bottom of non-overlapping band */
    /*
     * Initialization:
     *  set r1, r2, r1End and r2End appropriately, preserve the important
     * parts of the destination region until the end in case it's one of
     * the two source regions, then mark the "new" region empty, allocating
     * another array of rectangles for it to use.
     */
    r1 = reg1->head;
    r2 = reg2->head;

    /*
     * newReg may be one of the src regions so we can't empty it. We keep a
     * note of its rects pointer (so that we can free them later), preserve its
     * rcBound and simply set numRects to zero.
     */
    /*
    oldRects = newReg->rects;
    newReg->numRects = 0;
     */

    /*
     * for implementation of MiniGUI, we create an empty region.
     */
    if (newReg == reg1 || newReg == reg2)
    {
        InitClipRgn(&my_dst, newReg->heap);
        pdst = &my_dst;
    }
    else
    {
        EmptyClipRgn(newReg);
        pdst = newReg;
    }

    /*
     * Allocate a reasonable number of rectangles for the new region. The idea
     * is to allocate enough so the individual functions don't need to
     * reallocate and copy the array, which is time consuming, yet we don't
     * have to worry about using too much memory. I hope to be able to
     * nuke the Xrealloc() at the end of this function eventually.
     */

    /* for implementation of MiniGUI, dst always is an empty region.
    newReg->size = MAX(reg1->numRects,reg2->numRects) * 2;

    if (! (newReg->rects = malloc( sizeof(__gui_rect_t) * newReg->size )))
    {
        newReg->size = 0;
        return;
    }
     */

    /*
     * Initialize ybot and ytop.
     * In the upcoming loop, ybot and ytop serve different functions depending
     * on whether the band being handled is an overlapping or non-overlapping
     * band.
     *  In the case of a non-overlapping band (only one of the regions
     * has points in the band), ybot is the bottom of the most recent
     * intersection and thus clips the top of the rectangles in that band.
     * ytop is the top of the next intersection between the two regions and
     * serves to clip the bottom of the rectangles in the current band.
     *  For an overlapping band (where the two regions intersect), ytop clips
     * the top of the rectangles of both regions and ybot clips the bottoms.
     */
    if (reg1->rcBound.top < reg2->rcBound.top)
    {
        ybot = reg1->rcBound.top;
    }
    else
    {
        ybot = reg2->rcBound.top;
    }

    /*
     * prevBand serves to mark the start of the previous band so rectangles
     * can be coalesced into larger rectangles. qv. miCoalesce, above.
     * In the beginning, there is no previous band, so prevBand == curBand
     * (curBand is set later on, of course, but the first band will always
     * start at index 0). prevBand and curBand must be indices because of
     * the possible expansion, and resultant moving, of the new region's
     * array of rectangles.
     */
    prevBand = pdst->head;

    do
    {
        curBand = pdst->tail;
        /*
         * This algorithm proceeds one source-band (as opposed to a
         * destination band, which is determined by where the two regions
         * intersect) at a time. r1BandEnd and r2BandEnd serve to mark the
         * rectangle after the last one in the current band for their
         * respective regions.
         */
        r1BandEnd = r1;

        while (r1BandEnd && (r1BandEnd->clip_rect.top == r1->clip_rect.top))
        {
            r1BandEnd = r1BandEnd->next;
        }

        r2BandEnd = r2;

        while (r2BandEnd && (r2BandEnd->clip_rect.top == r2->clip_rect.top))
        {
            r2BandEnd = r2BandEnd->next;
        }

        /*
         * First handle the band that doesn't intersect, if any.
         *
         * Note that attention is restricted to one band in the
         * non-intersecting region at once, so if a region has n
         * bands between the current position and the next place it overlaps
         * the other, this entire loop will be passed through n times.
         */
        if (r1->clip_rect.top < r2->clip_rect.top)
        {
            top = MAX(r1->clip_rect.top, ybot);
            bot = MIN(r1->clip_rect.bottom, r2->clip_rect.top);

            if ((top != bot) && (nonOverlap1Func != NULL))
            {
                (* nonOverlap1Func)(pdst, r1, r1BandEnd, top, bot);
            }

            ytop = r2->clip_rect.top;
        }
        else if (r2->clip_rect.top < r1->clip_rect.top)
        {
            top = MAX(r2->clip_rect.top, ybot);
            bot = MIN(r2->clip_rect.bottom, r1->clip_rect.top);

            if ((top != bot) && (nonOverlap2Func != NULL))
            {
                (* nonOverlap2Func)(pdst, r2, r2BandEnd, top, bot);
            }

            ytop = r1->clip_rect.top;
        }
        else
        {
            ytop = r1->clip_rect.top;
        }

        /*
         * If any rectangles got added to the region, try and coalesce them
         * with rectangles from the previous band. Note we could just do
         * this test in miCoalesce, but some machines incur a not
         * inconsiderable cost for function calls, so...
         */
        if (pdst->tail != curBand)
        {
            prevBand = REGION_Coalesce(pdst, prevBand, curBand);
        }

        /*
         * Now see if we've hit an intersecting band. The two bands only
         * intersect if ybot > ytop
         */
        ybot = MIN(r1->clip_rect.bottom, r2->clip_rect.bottom);
        curBand = pdst->tail;

        if (ybot > ytop)
        {
            (* overlapFunc)(pdst, r1, r1BandEnd, r2, r2BandEnd, ytop, ybot);
        }

        if (pdst->tail != curBand)
        {
            prevBand = REGION_Coalesce(pdst, prevBand, curBand);
        }

        /*
         * If we've finished with a band (bottom == ybot) we skip forward
         * in the region to the next band.
         */
        if (r1->clip_rect.bottom == ybot)
        {
            r1 = r1BandEnd;
        }

        if (r2->clip_rect.bottom == ybot)
        {
            r2 = r2BandEnd;
        }
    } while (r1 && r2);

    /*
     * Deal with whichever region still has rectangles left.
     */
    curBand = pdst->tail;

    if (r1)
    {
        if (nonOverlap1Func != NULL)
        {
            do
            {
                r1BandEnd = r1;

                while ((r1BandEnd) && (r1BandEnd->clip_rect.top == r1->clip_rect.top))
                {
                    r1BandEnd = r1BandEnd->next;
                }

                (* nonOverlap1Func)(pdst, r1, r1BandEnd,
                                    MAX(r1->clip_rect.top, ybot), r1->clip_rect.bottom);
                r1 = r1BandEnd;
            } while (r1);
        }
    }
    else if ((r2) && (nonOverlap2Func != NULL))
    {
        do
        {
            r2BandEnd = r2;

            while ((r2BandEnd) && (r2BandEnd->clip_rect.top == r2->clip_rect.top))
            {
                r2BandEnd = r2BandEnd->next;
            }

            (* nonOverlap2Func)(pdst, r2, r2BandEnd,
                                MAX(r2->clip_rect.top, ybot), r2->clip_rect.bottom);
            r2 = r2BandEnd;
        } while (r2);
    }

    if (pdst->tail != curBand)
    {
        (void) REGION_Coalesce(pdst, prevBand, curBand);
    }

    /*
     * A bit of cleanup. To keep regions from growing without bound,
     * we shrink the array of rectangles to match the new number of
     * rectangles in the region. This never goes to 0, however...
     *
     * Only do this stuff if the number of rectangles allocated is more than
     * twice the number of rectangles in the region (a simple optimization...).
     */
#if 0

    if (newReg->numRects < (newReg->size >> 1))
    {
        if (REGION_NOT_EMPTY(newReg))
        {
            __gui_rect_t *prev_rects = newReg->rects;
            newReg->size = newReg->numRects;
            newReg->rects = realloc(newReg->rects, sizeof(__gui_rect_t) * newReg->size);

            if (! newReg->rects)
            {
                newReg->rects = prev_rects;
            }
        }
        else
        {
            /*
             * No point in doing the extra work involved in an Xrealloc if
             * the region is empty
             */
            newReg->size = 1;
            free(newReg->rects);
            newReg->rects = malloc(sizeof(__gui_rect_t));
        }
    }

    free(oldRects);
#else

    if (pdst != newReg)
    {
        EmptyClipRgn(newReg);
        *newReg = my_dst;
    }

#endif
}

/***********************************************************************
 *          Region Intersection
 ***********************************************************************/


/***********************************************************************
 *             REGION_IntersectO
 *
 * Handle an overlapping band for REGION_Intersect.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Rectangles may be added to the region.
 *
 */
static void REGION_IntersectO(pclipregn  region, const __gui_cliprect_t *r1, const __gui_cliprect_t *r1End,
                              const __gui_cliprect_t *r2, const __gui_cliprect_t *r2End, __s32 top, __s32 bottom)

{
    __s32               left;
    __s32               right;
    __gui_cliprect_t    *newcliprect;

    while ((r1 != r1End) && (r2 != r2End))
    {
        left  = MAX(r1->clip_rect.left, r2->clip_rect.left);
        right = MIN(r1->clip_rect.right, r2->clip_rect.right);

        /*
         * If there's any overlap between the two rectangles, add that
         * overlap to the new region.
         * There's no need to check for subsumption because the only way
         * such a need could arise is if some region has two rectangles
         * right next to each other. Since that should never happen...
         */
        if (left < right)
        {
            NEWCLIPRECT(region, newcliprect);
            newcliprect->clip_rect.left = left;
            newcliprect->clip_rect.top = top;
            newcliprect->clip_rect.right = right;
            newcliprect->clip_rect.bottom = bottom;
        }

        /*
         * Need to advance the pointers. Shift the one that extends
         * to the right the least, since the other still has a chance to
         * overlap with that region's next rectangle, if you see what I mean.
         */
        if (r1->clip_rect.right < r2->clip_rect.right)
        {
            r1 = r1->next;
        }
        else if (r2->clip_rect.right < r1->clip_rect.right)
        {
            r2 = r2->next;
        }
        else
        {
            r1 = r1->next;
            r2 = r2->next;
        }
    }
}

/***********************************************************************
 *             Region Union
 ***********************************************************************/

/***********************************************************************
 *             REGION_UnionNonO
 *
 *      Handle a non-overlapping band for the union operation. Just
 *      Adds the rectangles into the region. Doesn't have to check for
 *      subsumption or anything.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      region->numRects is incremented and the final rectangles overwritten
 *      with the rectangles we're passed.
 *
 */
static void REGION_UnionNonO(pclipregn  region,
                             const __gui_cliprect_t *r,
                             const __gui_cliprect_t *rEnd,
                             __s32 top, __s32 bottom)
{
    __gui_cliprect_t *newcliprect;

    while (r != rEnd)
    {
        NEWCLIPRECT(region, newcliprect);
        newcliprect->clip_rect.left = r->clip_rect.left;
        newcliprect->clip_rect.top = top;
        newcliprect->clip_rect.right = r->clip_rect.right;
        newcliprect->clip_rect.bottom = bottom;
        r = r->next;
    }
}

/***********************************************************************
 *             REGION_UnionO
 *
 *      Handle an overlapping band for the union operation. Picks the
 *      left-most rectangle each time and merges it into the region.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Rectangles are overwritten in region->rects and region->numRects will
 *      be changed.
 *
 */
static void REGION_UnionO(pclipregn  region, const __gui_cliprect_t *r1, const __gui_cliprect_t *r1End,
                          const __gui_cliprect_t *r2, const __gui_cliprect_t *r2End, __s32 top, __s32 bottom)
{
    __gui_cliprect_t *newcliprect;
#define MERGERECT(r) \
    if ((region->head) &&  \
        (region->tail->clip_rect.top == top) &&  \
        (region->tail->clip_rect.bottom == bottom) &&  \
        (region->tail->clip_rect.right >= r->clip_rect.left))  \
    {  \
        if (region->tail->clip_rect.right < r->clip_rect.right)  \
        {  \
            region->tail->clip_rect.right = r->clip_rect.right;  \
        }  \
    }  \
    else  \
    {  \
        NEWCLIPRECT(region, newcliprect);  \
        newcliprect->clip_rect.top = top;  \
        newcliprect->clip_rect.bottom = bottom;  \
        newcliprect->clip_rect.left = r->clip_rect.left;  \
        newcliprect->clip_rect.right = r->clip_rect.right;  \
    }  \
    r = r->next;

    while ((r1 != r1End) && (r2 != r2End))
    {
        if (r1->clip_rect.left < r2->clip_rect.left)
        {
            MERGERECT(r1);
        }
        else
        {
            MERGERECT(r2);
        }
    }

    if (r1 != r1End)
    {
        do
        {
            MERGERECT(r1);
        } while (r1 != r1End);
    }
    else while (r2 != r2End)
        {
            MERGERECT(r2);
        }
}

/***********************************************************************
 *             Region Subtraction
 ***********************************************************************/

/***********************************************************************
 *             REGION_SubtractNonO1
 *
 *      Deal with non-overlapping band for subtraction. Any parts from
 *      region 2 we discard. Anything from region 1 we add to the region.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      region may be affected.
 *
 */
static void REGION_SubtractNonO1(pclipregn  region, const __gui_cliprect_t *r, const __gui_cliprect_t *rEnd,
                                 __s32 top, __s32 bottom)
{
    __gui_cliprect_t *newcliprect;

    while (r != rEnd)
    {
        NEWCLIPRECT(region, newcliprect);
        newcliprect->clip_rect.left = r->clip_rect.left;
        newcliprect->clip_rect.top = top;
        newcliprect->clip_rect.right = r->clip_rect.right;
        newcliprect->clip_rect.bottom = bottom;
        r = r->next;
    }
}


/***********************************************************************
 *             REGION_SubtractO
 *
 *      Overlapping band subtraction. x1 is the left-most point not yet
 *      checked.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      region may have rectangles added to it.
 *
 */
static void REGION_SubtractO(pclipregn  region, const __gui_cliprect_t *r1, const __gui_cliprect_t *r1End,
                             const __gui_cliprect_t *r2, const __gui_cliprect_t *r2End, __s32 top, __s32 bottom)
{
    __gui_cliprect_t *newcliprect;
    int left;
    left = r1->clip_rect.left;

    while ((r1 != r1End) && (r2 != r2End))
    {
        if (r2->clip_rect.right <= left)
        {
            /*
             * Subtrahend missed the boat: go to next subtrahend.
             */
            r2 = r2->next;
        }
        else if (r2->clip_rect.left <= left)
        {
            /*
             * Subtrahend preceeds minuend: nuke left edge of minuend.
             */
            left = r2->clip_rect.right;

            if (left >= r1->clip_rect.right)
            {
                /*
                 * Minuend completely covered: advance to next minuend and
                 * reset left fence to edge of new minuend.
                 */
                r1 = r1->next;

                if (r1 != r1End)
                {
                    left = r1->clip_rect.left;
                }
            }
            else
            {
                /*
                 * Subtrahend now used up since it doesn't extend beyond
                 * minuend
                 */
                r2 = r2->next;
            }
        }
        else if (r2->clip_rect.left < r1->clip_rect.right)
        {
            /*
             * Left part of subtrahend covers part of minuend: add uncovered
             * part of minuend to region and skip to next subtrahend.
             */
            NEWCLIPRECT(region, newcliprect);
            newcliprect->clip_rect.left = left;
            newcliprect->clip_rect.top = top;
            newcliprect->clip_rect.right = r2->clip_rect.left;
            newcliprect->clip_rect.bottom = bottom;
            left = r2->clip_rect.right;

            if (left >= r1->clip_rect.right)
            {
                /*
                 * Minuend used up: advance to new...
                 */
                r1 = r1->next;

                if (r1 != r1End)
                {
                    left = r1->clip_rect.left;
                }
            }
            else
            {
                /*
                 * Subtrahend used up
                 */
                r2 = r2->next;
            }
        }
        else
        {
            /*
             * Minuend used up: add any remaining piece before advancing.
             */
            if (r1->clip_rect.right > left)
            {
                NEWCLIPRECT(region, newcliprect);
                newcliprect->clip_rect.left = left;
                newcliprect->clip_rect.top = top;
                newcliprect->clip_rect.right = r1->clip_rect.right;
                newcliprect->clip_rect.bottom = bottom;
            }

            r1 = r1->next;

            if (r1 != r1End)
            {
                left = r1->clip_rect.left;
            }
        }
    }

    /*
     * Add remaining minuend rectangles to region.
     */
    while (r1 != r1End)
    {
        NEWCLIPRECT(region, newcliprect);
        newcliprect->clip_rect.left = left;
        newcliprect->clip_rect.top = top;
        newcliprect->clip_rect.right = r1->clip_rect.right;
        newcliprect->clip_rect.bottom = bottom;
        r1 = r1->next;

        if (r1 != r1End)
        {
            left = r1->clip_rect.left;
        }
    }
}

/***********************************************************************
 *             IntersectRegion
 */
__bool ClipRgnIntersect(pclipregn  dst, const pclipregn  src1, const pclipregn  src2)
{
    /* check for trivial reject */
    if ((!(src1->head)) || (!(src2->head))  ||
        (!EXTENTCHECK(&src1->rcBound, &src2->rcBound)))
    {
        EmptyClipRgn(dst);
        return ORANGE_TRUE;
    }
    else
        REGION_RegionOp(dst, src1, src2,
                        REGION_IntersectO, NULL, NULL);

    /*
     * Can't alter dst's rcBound before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the rcBound of those regions being the same. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */
    REGION_SetExtents(dst);
    dst->type = (dst->head) ? GUI_COMPLEXREGION : GUI_NULLREGION ;
    return ORANGE_TRUE;
}

/***********************************************************************
 *             SubtractRegion
 *
 *      Subtract rgnS from rgnM and leave the result in rgnD.
 *      S stands for subtrahend, M for minuend and D for difference.
 *
 * Results:
 *      ORANGE_TRUE.
 *
 * Side Effects:
 *      regD is overwritten.
 *
 */
__bool SubtractRegion(pclipregn  rgnD, const pclipregn  rgnM, const pclipregn  rgnS)
{
    /* check for trivial reject */
    if ((!(rgnM->head)) || (!(rgnS->head))  ||
        (!EXTENTCHECK(&rgnM->rcBound, &rgnS->rcBound)))
    {
        ClipRgnCopy(rgnD, rgnM);
        return ORANGE_TRUE;
    }

    REGION_RegionOp(rgnD, rgnM, rgnS, REGION_SubtractO,
                    REGION_SubtractNonO1, NULL);
    /*
     * Can't alter newReg's rcBound before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the rcBound of those regions being the unaltered. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */
    REGION_SetExtents(rgnD);
    rgnD->type = (rgnD->head) ? GUI_COMPLEXREGION : GUI_NULLREGION;
    return ORANGE_TRUE;
}

/***********************************************************************
 *             UnionRegion
 */
__bool UnionRegion(pclipregn  dst, const pclipregn  src1, const pclipregn  src2)
{
    /*  checks all the simple cases */

    /*
     * Region 1 and 2 are the same or region 1 is empty
     */
    if ((src1 == src2) || (!(src1->head)))
    {
        if (dst != src2)
        {
            ClipRgnCopy(dst, src2);
        }

        return ORANGE_TRUE;
    }

    /*
     * if nothing to union (region 2 empty)
     */
    if (!(src2->head))
    {
        if (dst != src1)
        {
            ClipRgnCopy(dst, src1);
        }

        return ORANGE_TRUE;
    }

    /*
     * Region 1 completely subsumes region 2
     */
    if ((src1->head == src1->tail) &&
        (src1->rcBound.left <= src2->rcBound.left) &&
        (src1->rcBound.top <= src2->rcBound.top) &&
        (src1->rcBound.right >= src2->rcBound.right) &&
        (src1->rcBound.bottom >= src2->rcBound.bottom))
    {
        if (dst != src1)
        {
            ClipRgnCopy(dst, src1);
        }

        return ORANGE_TRUE;
    }

    /*
     * Region 2 completely subsumes region 1
     */
    if ((src2->head == src2->tail) &&
        (src2->rcBound.left <= src1->rcBound.left) &&
        (src2->rcBound.top <= src1->rcBound.top) &&
        (src2->rcBound.right >= src1->rcBound.right) &&
        (src2->rcBound.bottom >= src1->rcBound.bottom))
    {
        if (dst != src2)
        {
            ClipRgnCopy(dst, src2);
        }

        return ORANGE_TRUE;
    }

    REGION_RegionOp(dst, src1, src2, REGION_UnionO,
                    REGION_UnionNonO, REGION_UnionNonO);
    dst->rcBound.left = MIN(src1->rcBound.left, src2->rcBound.left);
    dst->rcBound.top = MIN(src1->rcBound.top, src2->rcBound.top);
    dst->rcBound.right = MAX(src1->rcBound.right, src2->rcBound.right);
    dst->rcBound.bottom = MAX(src1->rcBound.bottom, src2->rcBound.bottom);
    dst->type = (dst->head) ? GUI_COMPLEXREGION : GUI_NULLREGION ;
    return ORANGE_TRUE;
}

/* XorRegion */
__bool XorRegion(pclipregn  dst, const pclipregn  src1, const pclipregn  src2)
{
    __gui_clipregn_t tmpa, tmpb;
    InitClipRgn(&tmpa, src1->heap);
    InitClipRgn(&tmpb, src2->heap);
    SubtractRegion(&tmpa, src1, src2);
    SubtractRegion(&tmpb, src2, src1);
    UnionRegion(dst, &tmpa, &tmpb);
    EmptyClipRgn(&tmpa);
    EmptyClipRgn(&tmpb);
    return ORANGE_TRUE;
}

/* Adds a rectangle to a region */
__bool AddClipRect(pclipregn region, const  pguirect rect)
{
    __gui_clipregn_t my_region;
    __gui_cliprect_t my_cliprect;

    if (IsRectEmpty(rect))
    {
        return ORANGE_FALSE;
    }

    my_cliprect.clip_rect = *rect;
    my_cliprect.next = NULL;
    my_cliprect.prev = NULL;
    my_region.type = GUI_SIMPLEREGION;
    my_region.rcBound = *rect;
    my_region.head = &my_cliprect;
    my_region.tail = &my_cliprect;
    my_region.heap = NULL;
    UnionRegion(region, region, &my_region);
    return ORANGE_TRUE;
}

/* Intersect a rect with a region */
__bool IntersectClipRect(pclipregn region, const pguirect rect)
{
    __gui_clipregn_t    my_region;
    __gui_cliprect_t    my_cliprect;

    if (IsRectEmpty(rect))
    {
        return ORANGE_FALSE;
    }

    my_cliprect.clip_rect       = *rect;
    my_cliprect.next            = NULL;
    my_cliprect.prev            = NULL;
    my_region.type              = GUI_SIMPLEREGION;
    my_region.rcBound           = *rect;
    my_region.head              = &my_cliprect;
    my_region.tail              = &my_cliprect;
    my_region.heap              = NULL;
    ClipRgnIntersect(region, region, &my_region);
    return ORANGE_TRUE;
}

__bool SubtractClipRect(pclipregn region, const __gui_rect_t *rect)
{
    __gui_clipregn_t my_region;
    __gui_cliprect_t my_cliprect;

    if (IsRectEmpty(rect))
    {
        return ORANGE_FALSE;
    }

    my_cliprect.clip_rect = *rect;
    my_cliprect.next = NULL;
    my_cliprect.prev = NULL;
    my_region.type = GUI_SIMPLEREGION;
    my_region.rcBound = *rect;
    my_region.head = &my_cliprect;
    my_region.tail = &my_cliprect;
    my_region.heap = NULL;
    SubtractRegion(region, region, &my_region);
    return ORANGE_TRUE;
}

void OffsetRegion(pclipregn region, __s32 x, __s32 y)
{
    __gui_cliprect_t *cliprect = region->head;

    while (cliprect)
    {
        OffsetRect(&cliprect->clip_rect, x, y);
        cliprect = cliprect->next;
    }

    if (region->head)
    {
        OffsetRect(&region->rcBound, x, y);
    }
}

#if 0
static void cb_region(void *context, int x1, int x2, int y)
{
#if 0
    CLIPRGN *region = (CLIPRGN *) context;
    CLIPRECT *newcliprect;
    newcliprect = ClipRectAlloc(region->heap);

    if (x2 > x1)
    {
        newcliprect->clip_rect.left = x1;
        newcliprect->clip_rect.right = x2 + 1;
    }
    else
    {
        newcliprect->clip_rect.left = x2;
        newcliprect->clip_rect.right = x1 + 1;
    }

    newcliprect->clip_rect.top = y;
    newcliprect->clip_rect.bottom = y + 1;

    if (region->head == NULL
        || (newcliprect->clip_rect.top >= region->tail->clip_rect.bottom
            && (newcliprect->clip_rect.left != region->tail->clip_rect.left
                || newcliprect->clip_rect.right != region->tail->clip_rect.right)))
    {
        /* simply append to tail */
        goto append;
    }
    else if (newcliprect->clip_rect.top == region->tail->clip_rect.bottom
             && newcliprect->clip_rect.left == region->tail->clip_rect.left
             && newcliprect->clip_rect.right == region->tail->clip_rect.right)
    {
        /* merge with the tail */
        region->tail->clip_rect.bottom += 1;
        FreeClipRect(region->heap, newcliprect);
    }
    else if (newcliprect->clip_rect.bottom == region->head->clip_rect.top
             && newcliprect->clip_rect.left == region->head->clip_rect.left
             && newcliprect->clip_rect.right == region->head->clip_rect.right)
    {
        /* merge with the head */
        region->head->clip_rect.top -= 1;
        FreeClipRect(region->heap, newcliprect);
    }
    else if (newcliprect->clip_rect.top > region->tail->clip_rect.bottom)
    {
        /* simply append to tail */
        goto append;
    }
    else if (newcliprect->clip_rect.bottom < region->head->clip_rect.top)
    {
        /* simply insert to head */
        newcliprect->prev = NULL;
        newcliprect->next = region->head;
        region->head->prev = newcliprect;
        region->head = newcliprect;
    }
    else
    {
        /* find a position to insert */
        CLIPRECT *cliprect = region->head;
        CLIPRECT *prev;
        printf("get here.\n");
        printf("new cliprc: (%d, %d, %d, %d)\n",
               newcliprect->clip_rect.left, newcliprect->clip_rect.top,
               newcliprect->clip_rect.right, newcliprect->clip_rect.bottom);
        printf("head cliprc: (%d, %d, %d, %d)\n",
               region->head->clip_rect.left, region->head->clip_rect.top,
               region->head->clip_rect.right, region->head->clip_rect.bottom);
        printf("tail cliprc: (%d, %d, %d, %d)\n",
               region->tail->clip_rect.left, region->tail->clip_rect.top,
               region->tail->clip_rect.right, region->tail->clip_rect.bottom);

        while (cliprect && newcliprect->clip_rect.top >= cliprect->clip_rect.top)
        {
            cliprect = cliprect->next;
        }

        if (cliprect == NULL) /* simply append to the tail */
        {
            goto append;
        }

        printf("current cliprc: (%d, %d, %d, %d)\n",
               cliprect->clip_rect.left, cliprect->clip_rect.top,
               cliprect->clip_rect.right, cliprect->clip_rect.bottom);

        /* merge with prev or next? */
        if ((prev = cliprect->prev)
            && prev->clip_rect.bottom == newcliprect->clip_rect.top
            && prev->clip_rect.left == newcliprect->clip_rect.left
            && prev->clip_rect.right == newcliprect->clip_rect.right)
        {
            prev->clip_rect.bottom += 1;
            FreeClipRect(region->heap, newcliprect);
            printf("merge with previous.\n");
        }
        else if (cliprect->clip_rect.top == newcliprect->clip_rect.bottom
                 && cliprect->clip_rect.left == newcliprect->clip_rect.left
                 && cliprect->clip_rect.right == newcliprect->clip_rect.right)
        {
            cliprect->clip_rect.top -= 1;
            FreeClipRect(region->heap, newcliprect);
            printf("merge with current.\n");
        }
        else
        {
            /* insert before of current cliprc */
            cliprect->prev->next = newcliprect;
            newcliprect->prev = cliprect->prev;
            cliprect->prev = newcliprect;
            newcliprect->next = cliprect;
            printf("insert before of current.\n");
        }
    }

    return;
append:
    newcliprect->next = NULL;
    newcliprect->prev = region->tail;

    if (region->tail)
    {
        region->tail->next = newcliprect;
    }

    region->tail = newcliprect;

    if (region->head == NULL)
    {
        region->head = newcliprect;
    }

#else
    __gui_clipregn_t *region = (__gui_clipregn_t *) context;
    __gui_clipregn_t newregion;
    __gui_clipregn_t newcliprect;

    if (x2 > x1)
    {
        newcliprect.clip_rect.left = x1;
        newcliprect.clip_rect.right = x2 + 1;
    }
    else
    {
        newcliprect.clip_rect.left = x2;
        newcliprect.clip_rect.right = x1 + 1;
    }

    newcliprect.clip_rect.top = y;
    newcliprect.clip_rect.bottom = y + 1;
    newcliprect.next = NULL;
    newcliprect.prev = NULL;
    newregion.type = GUI_SIMPLEREGION;
    newregion.rcBound = newcliprect.rc;
    newregion.head = &newcliprect;
    newregion.tail = &newcliprect;
    newregion.heap = NULL;
    UnionRegion(region, region, &newregion);
#endif
}

__bool InitCircleRegion(pclipregn dst, int x, int y, int r)
{
    EmptyClipRgn(dst);

    if (r < 1)
    {
        __gui_cliprect_t *newcliprect;
        NEWCLIPRECT(dst, newcliprect);
        newcliprect->clip_rect.left = x;
        newcliprect->clip_rect.top = y;
        newcliprect->clip_rect.right = x + 1;
        newcliprect->clip_rect.bottom = y + 1;
        return ORANGE_TRUE;
    }

    CircleGenerator(dst, x, y, r, cb_region);
    dst->type = GUI_COMPLEXREGION;
    //    REGION_SetExtents (dst);
    return ORANGE_TRUE;
}

__bool InitEllipseRegion(pclipregn dst, int x, int y, int rx, int ry)
{
    EmptyClipRgn(dst);

    if (rx < 1 || ry < 1)
    {
        __gui_cliprect_t *newcliprect;
        NEWCLIPRECT(dst, newcliprect);
        newcliprect->clip_rect.left = x;
        newcliprect->clip_rect.top = y;
        newcliprect->clip_rect.right = x + 1;
        newcliprect->clip_rect.bottom = y + 1;
        return ORANGE_TRUE;
    }

    EllipseGenerator(dst, x, y, rx, ry, cb_region);
    dst->type = GUI_COMPLEXREGION;
    //    REGION_SetExtents (dst);
    return ORANGE_TRUE;
}

__bool InitPolygonRegion(pclipregn dst, const POINT *pts, int vertices)
{
    EmptyClipRgn(dst);

    if (PolygonIsMonotoneVertical(pts, vertices))
    {
        if (MonotoneVerticalPolygonGenerator(dst, pts, vertices, cb_region))
        {
            goto ok;
        }
    }
    else if (PolygonGenerator(dst, pts, vertices, cb_region))
    {
        goto ok;
    }

    return ORANGE_FALSE;
ok:
    dst->type = COMPLEXREGION;
    //   REGION_SetExtents (dst);
    return ORANGE_TRUE;
}

#endif
