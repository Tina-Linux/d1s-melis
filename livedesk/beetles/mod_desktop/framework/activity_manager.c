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
#include "mod_desktop_i.h"

#define MAX_ACTIVITY    32

struct _ActivityNode
{
    Activity    *activity;
    struct _ActivityNode *next;
};

struct _ActivityManager
{
    ActivityNode    run[MAX_ACTIVITY];
    ActivityNode    *backgrd;
    ActivityNode    *stack;
};

/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
ActivityManager *activity_manager_create(void)
{
    ActivityManager *thiz   = NULL;

    thiz    = (ActivityManager *)MALLOC(sizeof(ActivityManager));

    if (thiz == NULL)
    {
        __err("ActivityManager malloc error ");
        return NULL;
    }

    memset(thiz, 0, sizeof(ActivityManager));
    activity_init();
    return thiz;
}

static int32_t activity_manager_find_free_node(ActivityManager *thiz, ActivityNode **node)
{
    ActivityNode    *ptr = thiz->run;
    int i;

    for (i = 0; i < MAX_ACTIVITY; i++) /*查询空闲node */
    {
        if (ptr[i].activity == NULL)
        {
            break;
        }
    }

    __msg("activity_manager_find_node i=%d ", i);

    if (i == MAX_ACTIVITY)
    {
        __err(" activity exceed 32 ");
        *node = NULL;
        return -1;
    }

    *node = ptr + i;
    return 0;
}

Activity *activity_manager_top(ActivityManager *thiz)
{
    ActivityNode    *iter;

    return_val_if_fail(thiz != NULL, NULL);

    iter = thiz->stack;

    if (iter == NULL)
    {
        return NULL;
    }

    while (iter->next != NULL)
    {
        iter = iter->next;
    }

    return iter->activity;
}

__s32 activity_manager_push(ActivityManager *thiz, Activity *activity)
{
    ActivityNode    *top;
    ActivityNode    *node;

    return_val_if_fail(thiz != NULL, -1);
    activity_manager_find_free_node(thiz, &node);

    if (node == NULL)
    {
        return -1;
    }

    node->activity  = activity;
    node->next      = NULL;
    top             = thiz->stack;

    if (top == NULL)
    {
        thiz->stack = node;
        return 0;
    }

    while (top->next != NULL)        /*查询top node */
    {
        top = top->next;
    }

    top->next = node;
    return EPDK_OK;
}

__s32 activity_manager_pop(ActivityManager *thiz)
{
    ActivityNode    *prev, *cur;

    return_val_if_fail(thiz != NULL, -1);
    return_val_if_fail(thiz->stack != NULL, -1);

    if (thiz->stack->next == NULL)
    {
        thiz->stack->activity   = NULL;
        thiz->stack             = NULL;
        return EPDK_OK;
    }

    prev = cur = thiz->stack;

    while (cur->next != NULL)
    {
        prev = cur;
        cur = cur->next;
    }

    prev->next      = NULL;
    cur->activity   = NULL;
    cur->next       = NULL;
    return EPDK_OK;
}

__s32 activity_manager_top_to_backgrd(ActivityManager *thiz)
{
    /*ActivityNode *prev, *cur;

    return_val_if_fail( thiz!= NULL, -1);
    prev = cur = thiz->stack;

    return_val_if_fail( thiz->stack != NULL, -1);
    while( cur->next != NULL )
    {
        prev = cur;
        cur = cur->next;
    }

    prev->next = NULL;

    activity_manager_add_backgrd(thiz, cur->activity);

    cur->activity= NULL;
    cur->next = NULL;*/
    Activity *top   = activity_manager_top(thiz);

    activity_manager_add_backgrd(thiz, top);
    activity_manager_pop(thiz);
    return EPDK_OK;
}

__s32 activity_manager_add_backgrd(ActivityManager *thiz, Activity *activity)
{
    ActivityNode    *node;
    ActivityNode    *iter;

    activity_manager_find_free_node(thiz, &node);

    node->activity  = activity;
    node->next      = NULL;
    iter            = thiz->backgrd;

    if (iter == NULL)
    {
        thiz->backgrd   = node;
        return EPDK_OK;
    }

    while (iter->next != NULL)
    {
        iter    = iter->next;
    }

    iter->next  = node;
    return EPDK_OK;
}

__s32 activity_manager_delete_backgrd(ActivityManager *thiz, Activity *activity)
{
    ActivityNode    *prev, *cur;
    int             i = 0;

    prev = cur = thiz->backgrd;

    return_val_if_fail(thiz->backgrd != NULL, -1);

    while (cur != NULL)
    {
        if (cur->activity == activity)
        {
            if (i > 0)
            {
                prev->next = cur->next;
            }
            else
            {
                thiz->backgrd = cur->next;
            }

            cur->activity   = NULL;
            cur->next       = NULL;
            return EPDK_OK;
        }

        prev    = cur;
        cur     = cur->next;
        i++;
    }

    return EPDK_OK;
}

__s32 activity_manager_backgrd_to_top(ActivityManager *thiz, Activity *activity)
{
    activity_manager_delete_backgrd(thiz, activity);
    activity_manager_push(thiz, activity);
    return EPDK_OK;
}

__s32 activity_manager_foreach_suspend(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2)
{
    ActivityNode *ptr   = thiz->stack;
    int ret;

    if (ptr == NULL)
    {
        return EPDK_OK;
    }

    while (ptr->next != NULL)
    {
        ret = (*iter)(ptr->activity, ACTIVITY_ST_SUSPEND, ctx1, ctx2);

        if (ret < 0)
        {
            return EPDK_OK;
        }

        ptr = ptr->next;
    }

    (*iter)(ptr->activity, ACTIVITY_ST_TOPLEVEL, ctx1, ctx2);
    return EPDK_OK;
}

__s32 activity_manager_foreach_backgrd(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2)
{
    int             ret;
    ActivityNode    *ptr;

    ptr = thiz->backgrd;

    while (ptr != NULL)
    {
        ret = (*iter)(ptr->activity, ACTIVITY_ST_BACKGROUND, ctx1, ctx2);

        if (ret < 0)
        {
            return EPDK_OK;
        }

        ptr = ptr->next;
    }

    return EPDK_OK;
}

__s32 activity_manager_foreach(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2)
{
    activity_manager_foreach_suspend(thiz, iter, ctx1, ctx2);
    activity_manager_foreach_backgrd(thiz, iter, ctx1, ctx2);
    return EPDK_OK;
}

void  activity_manager_destroy(ActivityManager *thiz)
{
    if (thiz)
    {
        FREE(thiz);
    }
}
