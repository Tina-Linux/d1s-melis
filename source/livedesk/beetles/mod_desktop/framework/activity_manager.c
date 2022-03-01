/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : activity_manager.c
* By        : Andy.zhang
* Func      : activity manager
* Version   : v1.0
* ============================================================================================================
* 2010-9-6 19:43:50  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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

    for (i = 0; i < MAX_ACTIVITY; i++) /*²éÑ¯¿ÕÏÐnode */
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

    while (top->next != NULL)        /*²éÑ¯top node */
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
