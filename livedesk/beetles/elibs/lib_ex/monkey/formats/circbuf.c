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
#include "monkey_i.h"
#include "circbuf.h"

#define CB_MAX_SLOT         4
#define CACHE_MAX_LINE      10

/***********************************************************************************************************************
    slot[0]->CbSlot->CbSlot->CbSlot (分行信息)
    slot[1]->CbSlot->CbSlot->CbSlot (分行信息)
    |
    |
    |
    |
    slot[CB_MAX_SLOT-1]->CbSlot->CbSlot->CbSlot (分行信息)

    1. 环形队列缓冲 CB_MAX_SLOT 段的分行信息;
    2. 每段内包含的分行信息是靠读取固定缓冲区数据分行得到, 每段包含的行数不固定, 所以采用分块链表的方式组成;
    3. 所有的slot 组成一个环形队列;
***********************************************************************************************************************/

/**********************************************************************************************************************/
typedef struct tag_CbSlot
{
    MkLine      line[CACHE_MAX_LINE];
    int         count;
    struct tag_CbSlot *next;
} CbSlot;
/**********************************************************************************************************************/
/** 创建一个 slot */
static CbSlot *cbslot_create(void)
{
    CbSlot *slot;
    slot = (CbSlot *)g_malloc(sizeof(CbSlot));

    if (!slot)
    {
        __err(" cbslot_create error ");
        return NULL;
    }

    g_memset(slot, 0, sizeof(CbSlot));
    return slot;
}

/** 复位slot , 清除里面数据，但是不释放内存 */
static void cbslot_flush(CbSlot *slot)
{
    CbSlot *tmp;
    tmp = slot;

    while (tmp != NULL)         //注意，缓冲区不要释放，最后在free_txt_line中释放分配的缓冲
    {
        tmp->count = 0;
        g_memset(tmp->line, 0, sizeof(MkLine)*CACHE_MAX_LINE);
        tmp = tmp->next;
    }

    return;
}

/** 删除 slot, 释放内存 */
static void cbslot_delete(CbSlot *slot)
{
    CbSlot *tmp1, *tmp2;
    tmp1 = slot;

    while (tmp1->next != NULL)
    {
        tmp2 = tmp1;
        tmp1 = tmp1->next;
        g_free(tmp2);
    }

    g_free(tmp1);
    return;
}

///** 设置slot index 处数据 */
//static void cbslot_setValue( CbSlot *slot, int index, MkLine *line)
//{
//  CbSlot *tmp = slot;
//
//  while( index >CACHE_MAX_LINE )
//  {
//      index -= CACHE_MAX_LINE;
//      tmp = tmp->next;
//      if(tmp == NULL)
//      {
//          tmp = (CbSlot *)g_malloc(sizeof(CbSlot));
//          g_memset(tmp, 0, sizeof(CbSlot));
//      }
//  }
//
//  tmp->line[index-1].len      = line->len;
//  tmp->line[index-1].start    = line->start;
//  tmp->count = index;
//}

/* 查下 slot index 处数据*/
static MkLine *cbslot_getValue(CbSlot *slot, int index)
{
    CbSlot *tmp = slot;

    while (index >= CACHE_MAX_LINE)
    {
        index -= CACHE_MAX_LINE;
        tmp = tmp->next;
    }

    if (tmp == NULL)
    {
        return NULL;
    }

    return &(tmp->line[index]);
}

/** 向slot尾部添加数据*/
static __s32 cbslot_pushValue(CbSlot *slot, MkLine *line)
{
    int index;
    CbSlot *tmp = slot, *tmp1 = NULL;

    while (tmp->count == CACHE_MAX_LINE)
    {
        tmp1 = tmp;
        tmp  = tmp->next;
    }

    if (tmp == NULL)
    {
        tmp = (CbSlot *)g_malloc(sizeof(CbSlot));
        g_memset(tmp, 0, sizeof(CbSlot));
        tmp1->next = tmp;
    }

    index = tmp->count + 1;
    tmp->line[index - 1].len       = line->len;
    tmp->line[index - 1].start     = line->start;
    tmp->count = index;
    return EPDK_OK;
}

/** slot 内部包含的item 数目 */
static __s32 cbslot_getSize(CbSlot *slot)
{
    CbSlot *tmp = slot;
    int count = 0;

    while (tmp != NULL)
    {
        count += tmp->count;
        tmp = tmp->next;
    }

    return count;
}

/** slot 的起始偏移量 */
static __s32 cbslot_getStartOffset(CbSlot *slot)
{
    return slot->line[0].start;
}

/** slot 的尾部偏移量 */
static __s32 cbslot_getEndOffset(CbSlot *slot)
{
    CbSlot *tmp1, *tmp2;
    int offset;
    tmp1 = tmp2 = slot;

    if (tmp1->count == 0)
    {
        return 0;
    }

    while (tmp1->count == CACHE_MAX_LINE)
    {
        tmp2 = tmp1;
        tmp1 = tmp1->next;
    }

    if ((tmp1->count == 0) || (tmp1 == NULL))
    {
        tmp1 = tmp2;
    }

    offset  = tmp1->line[tmp1->count - 1].start;
    offset += tmp1->line[tmp1->count - 1].len;
    return offset;
}

/**********************************************************************************************************************/
/*  环形缓冲区 */
struct tag_CircularBuff
{
    CbSlot             *slot[CB_MAX_SLOT];
    int                 qin, qout;                  /* 环形缓冲区 in, out */
    //  __s32               status;                     /* 环形队列状态, 免锁 */
    __bool              startFlag,  endFlag;        /* qin 是否已到文档最前面， qout 是否已到文档最后面 */

    __s32               startQIndex, startLIndex;   /* 当前显示页断行信息 */
    __s32               endQIndex,   endLIndex;
    //__s32             startOffset, endOffset;

    __u32               seekoffset;

    //__krnl_event_t        *cacheMbox;                 /* 消息邮箱, 发送命令   */
    __krnl_event_t      *cacheQ;
    __krnl_event_t      *respMbox;                  /* 回应邮箱, 回应命令   */
    __u32                tid;                       /* 后台分析线程 id      */
    txtEncodeParser     *plugin;                    /* 插件                 */
    LineConfig           config;
};

/* cache thread communication cmd*/
typedef enum
{
    QCACHE_CMD_NEXT_BUF = 1,
    QCACHE_CMD_PREV_BUF,
    QCACHE_CMD_NEXT_PAGE,
    QCACHE_CMD_PREV_PAGE,
    QCACHE_CMD_SEEK,
    QCACHE_CMD_REFRESH,
    QCACHE_CMD_EXIT
} QCacheCmd;

typedef struct
{
    MkEncode        encodeType;
    txtEncodeParser *(*plugin)(void);
} ParserPluginArray;

static ParserPluginArray pluginArray[] =
{
    { MK_CHAR_ENCODE_GBK,           gb_parserNew},
    { MK_CHAR_ENCODE_UNICODE,       utf16little_parserNew},
    { MK_CHAR_ENCODE_UNICODE_BIG,   utf16big_parserNew},
    { MK_CHAR_ENCODE_UTF8,          utf8_parserNew},
    { MK_CHAR_ENCODE_UNKNOWN,       gb_parserNew}
};

static void qcache_thread(void *p_arg);

/**********************************************************************************************************************/
/* 创建环形缓冲区 */
CircularBuff *circbuf_create(LineConfig *para)
{
    CircularBuff *cb;
    QCacheCmd   recev;
    int i;
    __u8 err;
    /* 分配句柄内存 */
    cb = (CircularBuff *)g_malloc(sizeof(CircularBuff));

    if (!cb)
    {
        __err(" CircularBuff malloc error ");
        return NULL;
    }

    g_memset(cb, 0, sizeof(CircularBuff));

    /* 装载解码插件*/
    for (i = 0; i < sizeof(pluginArray) / sizeof(pluginArray[0]); i++)
    {
        if (pluginArray[i].encodeType == para->encodeType)
        {
            cb->plugin = (*(pluginArray[i].plugin))();
            break;
        }
    }

    /* 配置插件 */
    cb->plugin->config(cb->plugin, para);
    /* 初始化环形队列 */
    cb->qin = cb->qout = 0;

    for (i = 0; i < CB_MAX_SLOT; i++)
    {
        cb->slot[i] = cbslot_create();
    }

    cb->startQIndex = cb->endQIndex = 0;
    cb->startLIndex = cb->endLIndex = 0;
    /* 创建通信邮箱 */
    cb->respMbox  = (__krnl_event_t *)esKRNL_MboxCreate(NULL);

    if (!cb->respMbox)
    {
        __err(" respMbox create error ");
        return NULL;
    }

    cb->cacheQ    = (__krnl_event_t *)esKRNL_QCreate(5);

    if (!cb->cacheQ)
    {
        __err(" cacheq create error ");
        return NULL;
    }

    /* 创建cache线程 */
    cb->tid = esKRNL_TCreate(qcache_thread, (void *)cb, 0x500, KRNL_priolevel5);

    if (!cb->tid)
    {
        __err(" qcache_thread create error ");
        return NULL;
    }

    /* 通知线程 */
    //esKRNL_MboxPost( cb->cacheMbox, (void *)QCACHE_CMD_REFRESH);
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_REFRESH);

    while (1)
    {
        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

        if (recev == QCACHE_CMD_REFRESH)
        {
            break;
        }
    }

    return cb;
}

/* 清空复位环形队列         */
__s32   circbuf_flush(CircularBuff *cb)
{
    int i;
    cb->qin = cb->qout = 0;

    for (i = 0; i < CB_MAX_SLOT; i++)
    {
        cbslot_flush(cb->slot[i]);
    }

    return EPDK_OK;
}


/* 设置断行参数 */
__s32   circbuf_setting(CircularBuff *cb, LineConfig *para)
{
    QCacheCmd   recev;
    int  i;
    __u8 err;
    /* 刷新环形队列 */
    circbuf_flush(cb);
    /* 刷新当前显示页断行信息*/
    cb->startQIndex = cb->startLIndex = 0;
    cb->endQIndex   = cb->endLIndex   = 0;

    /* 更新插件 */
    if (cb->config.encodeType != para->encodeType)
    {
        cb->plugin->destroy(cb->plugin);
        cb->plugin = NULL;

        for (i = 0; i < sizeof(pluginArray) / sizeof(pluginArray[0]); i++)
        {
            if (pluginArray[i].encodeType == para->encodeType)
            {
                cb->plugin = (*(pluginArray[i].plugin))();
                break;
            }
        }
    }

    /* 配置插件 */
    cb->plugin->config(cb->plugin, para);
    /* 更新配置信息 */
    g_memcpy(&(cb->config), para, sizeof(LineConfig));
    /* 通知线程 */
    //esKRNL_MboxPost( cb->cacheMbox, (void *)QCACHE_CMD_REFRESH);
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_REFRESH);

    while (1)
    {
        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

        if (recev == QCACHE_CMD_REFRESH)
        {
            break;
        }
    }

    return EPDK_OK;
}

__s32   circbuf_seek(CircularBuff *cb, __u32 offset)
{
    QCacheCmd   recev;
    __u8        err;
    /* 刷新环形队列 */
    circbuf_flush(cb);
    /* 刷新当前显示页断行信息*/
    cb->startQIndex = cb->startLIndex = 0;
    cb->endQIndex   = cb->endLIndex   = 0;
    cb->seekoffset  = offset;
    //esKRNL_MboxPost( cb->cacheMbox, (void *)QCACHE_CMD_SEEK);
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_SEEK);

    while (1)
    {
        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

        if (recev == QCACHE_CMD_SEEK)
        {
            break;
        }
    }

    return EPDK_OK;
}

static __bool circbuf_isFull(CircularBuff *cb)
{
    if ((cb->qout + 1) % CB_MAX_SLOT == cb->qin)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

static CbSlot *circbuf_getIndex(CircularBuff *cb, int index)
{
    return cb->slot[index % CB_MAX_SLOT];
}

static CbSlot *circbuf_getHead(CircularBuff *cb)
{
    return cb->slot[cb->qin];
}

static CbSlot *circbuf_getTail(CircularBuff *cb)
{
    return cb->slot[cb->qout];
}

static CbSlot *circbuf_addHead(CircularBuff *cb)
{
    cb->qin = (cb->qin - 1 + CB_MAX_SLOT) % CB_MAX_SLOT;
    return cb->slot[cb->qin];
}

static CbSlot *circbuf_addTail(CircularBuff *cb)
{
    cb->qout = (cb->qout + 1) % CB_MAX_SLOT;
    return cb->slot[cb->qout];
}

__s32   circbuf_getStartLines(CircularBuff *cb, MkLine *line, int num)
{
    CbSlot      *slot;
    MkLine      *tmpline;
    int i, curline, size, recev;
    __u8 err;
    g_memset(line, 0, sizeof(MkLine)*num);
    slot    = circbuf_getIndex(cb, cb->startQIndex);
    size    = cbslot_getSize(slot);
    curline = cb->startLIndex;

    if (curline == size)
    {
        if (cb->startQIndex == cb->qout)
        {
            if (cb->endFlag == EPDK_TRUE)
            {
                return -1;
            }
        }
    }

    for (i = 0; i < num; i++)                                   //写lineofpage行断行信息到页表中去show
    {
        if (curline == size)                                    //到了queue item 的末端
        {
            if (cb->startQIndex == cb->qout)                    //下一个queue是否存在
            {
                if (cb->endFlag == EPDK_TRUE)                   //如果已经到了文章尾部
                {
                    cb->endLIndex = curline - 1;
                    return 0;
                }
                else                                            //如果没到文章尾部
                {
                    esKRNL_QFlush(cb->cacheQ);
                    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_BUF);

                    while (1)
                    {
                        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

                        if (recev == QCACHE_CMD_NEXT_BUF)
                        {
                            break;
                        }
                    }
                }
            }

            curline = 0;
            cb->endQIndex   = (cb->startQIndex + 1) % CB_MAX_SLOT;
            slot = circbuf_getIndex(cb, cb->endQIndex);
            size = cbslot_getSize(slot);
            esKRNL_QFlush(cb->cacheQ);
            esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_BUF);
        }

        tmpline = cbslot_getValue(slot, curline);
        g_memcpy(line + i, tmpline, sizeof(MkLine));
        curline++;
    }

    /* 定位结束位置 */
    cb->endLIndex = curline - 1;
    /* 启动分页线程 */
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_PAGE);         //启动分页线程
    return 0;
}

/* 向后读取 num 行断行信息  */
__s32   circbuf_getNextLines(CircularBuff *cb, MkLine *line, int num)
{
    CbSlot      *slot;
    MkLine      *tmpline;
    QCacheCmd   recev;
    int         i, curline, size;
    __u8        err;
    g_memset(line, 0, sizeof(MkLine)*num);
    /*  定位待读取数据在环形队列中的开始位置 */
    slot = circbuf_getIndex(cb, cb->endQIndex);
    size = cbslot_getSize(slot);

    if ((cb->endLIndex + 1) < size)                 /* 开始位置在endQIndex slot */
    {
        cb->startQIndex = cb->endQIndex;
        cb->startLIndex = cb->endLIndex + 1;
    }
    else                                            /* 开始位置在下一个slot */
    {
        if (cb->endQIndex == cb->qout)              /* 下一个slot数据不在环形队列 */
        {
            if (EPDK_TRUE == cb->endFlag)           /* 如果环形队列数据已经到了文章尾部 */
            {
                return -1;                          /* 返回-1, 已经到了文章尾部，没有断行数据 */
            }
            else                                    /* 通知线程写入环形队列下一个slot数据*/
            {
                esKRNL_QFlush(cb->cacheQ);
                esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_BUF);

                while (1)
                {
                    recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

                    if (recev == QCACHE_CMD_NEXT_BUF)
                    {
                        break;
                    }
                }
            }
        }

        cb->startQIndex = (cb->endQIndex + 1) % CB_MAX_SLOT;
        cb->startLIndex = 0;
        slot = circbuf_getIndex(cb, cb->startQIndex);
        size = cbslot_getSize(slot);
    }

    cb->endQIndex   = cb->startQIndex;
    curline         = cb->startLIndex;

    /* 读取num行断行信息 */
    for (i = 0; i < num; i++)
    {
        if (curline == size)                                /* 到了startQIndex slot 的末端 */
        {
            if (cb->startQIndex == cb->qout)                /* 下一个slot不在队列内, 需启动线程写数据 */
            {
                if (cb->endFlag == EPDK_TRUE)               /* 如果环形队列数据已经到了文章尾部 */
                {
                    cb->endLIndex = curline - 1;
                    return 0;
                }
                else                                        /* 如果没到文章尾部, 通知线程写入环形队列下一个slot数据*/
                {
                    esKRNL_QFlush(cb->cacheQ);
                    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_BUF);

                    while (1)
                    {
                        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

                        if (recev == QCACHE_CMD_NEXT_BUF)
                        {
                            break;
                        }
                    }
                }
            }

            curline = 0;
            cb->endQIndex = (cb->startQIndex + 1) % CB_MAX_SLOT;
            slot = circbuf_getIndex(cb, cb->endQIndex);
            size = cbslot_getSize(slot);

            /* 启动分页线程 */
            if ((cb->startQIndex - cb->qout + CB_MAX_SLOT) % CB_MAX_SLOT > CB_MAX_SLOT / 2)
            {
                esKRNL_QFlush(cb->cacheQ);
                esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_BUF);
            }
        }

        // 复制断行数据
        tmpline = cbslot_getValue(slot, curline);
        g_memcpy(line + i, tmpline, sizeof(MkLine));
        curline++;
    }

    /* 定位结束位置 */
    cb->endLIndex = curline - 1;
    __msg(" size = %d , startq = %d , startl = %d , endq = %d , endl = %d  ", size, cb->startQIndex, cb->startLIndex, cb->endQIndex, cb->endLIndex);
    /* 启动分页线程 */
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_NEXT_PAGE);         //启动分页线程
    return 0;
}

/* 向前读取 num 行断行信息  */
__s32   circbuf_getPrevLines(CircularBuff *cb, MkLine *line, int num)
{
    CbSlot          *slot;
    MkLine          *tmpline;
    QCacheCmd       recev;
    int             i, curline, size;
    __u8            err;
    g_memset(line, 0, sizeof(MkLine)*num);
    /* 定位结束位置 */
    slot    = circbuf_getIndex(cb, cb->startQIndex);
    size    = cbslot_getSize(slot);

    if (cb->startLIndex > 0)                        /* 结束位置在 startQIndex slot */
    {
        cb->endQIndex       = cb->startQIndex;
        cb->endLIndex       = cb->startLIndex - 1;
    }
    else                                            /* 结束位置在 在上一个 slot */
    {
        if (cb->startQIndex == cb->qin)             /* 上一个slot数据不在环形队列 */
        {
            if (cb->startFlag == EPDK_TRUE)         /* 如果环形队列数据已经到了文章首部 */
            {
                return -1;                          /* 返回-1, 已经到了文章首部，没有断行数据 */
            }
            else                                    /* 通知线程写入环形队列上一个slot数据*/
            {
                esKRNL_QFlush(cb->cacheQ);
                esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_PREV_BUF);

                while (1)
                {
                    recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

                    if (recev == QCACHE_CMD_PREV_BUF)
                    {
                        break;
                    }
                }
            }
        }

        cb->endQIndex = (cb->startQIndex - 1 + CB_MAX_SLOT) % CB_MAX_SLOT;
        slot = circbuf_getIndex(cb, cb->endQIndex);
        size = cbslot_getSize(slot);
        cb->endLIndex = size - 1;
    }

    curline         = cb->endLIndex;
    cb->startQIndex = cb->endQIndex;

    for (i = num - 1; i >= 0; i--)
    {
        if (curline < 0)                                                /* 到了 endQIndex slot 的首端 */
        {
            if (cb->endQIndex == cb->qin)                               /* 上一个slot不在队列中，需启动线程去写 */
            {
                if (cb->startFlag == EPDK_TRUE)                         /* 如果环形队列数据已经到了文章首部 */
                {
                    break;
                }
                else                                        /* 如果没到文章首部, 通知线程写入环形队列上一个slot数据*/
                {
                    esKRNL_QFlush(cb->cacheQ);
                    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_PREV_BUF);

                    while (1)
                    {
                        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

                        if (recev == QCACHE_CMD_PREV_BUF)
                        {
                            break;
                        }
                    }
                }
            }

            cb->startQIndex = (cb->endQIndex - 1 + CB_MAX_SLOT) % CB_MAX_SLOT;
            slot    = circbuf_getIndex(cb, cb->startQIndex);
            size    = cbslot_getSize(slot);
            curline = size - 1;
            esKRNL_QFlush(cb->cacheQ);
            esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_PREV_BUF);
        }

        tmpline = cbslot_getValue(slot, curline);
        //__msg(" curline = %d, start = %d, len = %d ", curline, tmpline->start, tmpline->len);
        g_memcpy(line + i, tmpline, sizeof(MkLine));
        curline--;
    }

    cb->startLIndex = curline + 1;

    if (i >= 0)     // 上一页数据不够
    {
        int j;
        //__getc();
        g_memset(line, 0, sizeof(MkLine)*num);
        curline = cb->startLIndex;

        for (j = 0; j <= num - 1; j++)
        {
            if (curline == size)
            {
                curline = 0;
                cb->endQIndex = (cb->startQIndex + 1) % CB_MAX_SLOT;
                slot = circbuf_getIndex(cb, cb->endQIndex);
                size = cbslot_getSize(slot);
            }

            // 复制断行数据
            tmpline = cbslot_getValue(slot, curline);
            g_memcpy(line + j, tmpline, sizeof(MkLine));
            curline++;
        }

        return -2;
    }

    __msg(" size = %d , startq = %d , startl = %d , endq = %d , endl = %d  ", size, cb->startQIndex, cb->startLIndex, cb->endQIndex, cb->endLIndex);
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_PREV_PAGE);
    return 0;
}

/* 删除环形缓冲区           */
__s32   circbuf_delete(CircularBuff *cb)
{
    __u8            err;
    int             i;
    QCacheCmd       recev;
    /* 删除线程 */
    esKRNL_QFlush(cb->cacheQ);
    esKRNL_QPost(cb->cacheQ, (unsigned long)QCACHE_CMD_EXIT);

    while (1)
    {
        recev = (QCacheCmd)(__s32)esKRNL_MboxPend(cb->respMbox, 0, &err);

        if (recev == QCACHE_CMD_EXIT)
        {
            break;
        }
    }

    /* 删除邮箱 */
    esKRNL_MboxDel(cb->respMbox, 0, &err);
    esKRNL_QDel(cb->cacheQ, 0, &err);

    /* 释放环形队列 */
    for (i = 0; i < CB_MAX_SLOT; i++)
    {
        cbslot_delete(cb->slot[i]);
    }

    /* 卸载插件 */
    cb->plugin->destroy(cb->plugin);
    /* 释放句柄*/
    g_free(cb);
    return EPDK_OK;
}

/**********************************************************************************************************************/
//static void qcache_onCmdStart( CircularBuff *cb )
//{
//  txtEncodeParser  *parser;
//  CbSlot           *slot;
//  MkLine            line;
//  __s32             ret;
//
//  parser = cb->plugin;
//
//  slot   = (CbSlot *)circbuf_getIndex(cb, 0);
//  parser->next( parser, parser->start );
//  while(1)
//  {
//      g_memset(&line, 0, sizeof(MkLine));
//
//      ret = parser->analysis(parser, &line);
//      if( ret< 0)         // 分析完成
//          break;
//      cbslot_pushValue(slot, &line);
//  }
//
//  cb->startFlag = parser->bof;
//  cb->endFlag   = parser->eof;
//  return ;
//}

/** 清缓冲区，从新断行，但是不需要从新定位开始位置 */
static void qcache_onCmdRefresh(CircularBuff *cb)
{
    txtEncodeParser  *parser;
    MkLine            line;
    CbSlot           *slot;
    __s32             ret;
    parser = cb->plugin;
    //circbuf_flush(cb);
    slot   = (CbSlot *)circbuf_getIndex(cb, 0);
    parser->next(parser, parser->start);

    while (1)
    {
        g_memset(&line, 0, sizeof(MkLine));
        ret = parser->analysis(parser, &line);  //分析一行数据

        //      cbslot_pushValue( slot, &line );        //将分析得到的数据写入 circbuf
        //      if( ret< 0)
        //          break;
        if (ret == -2)
        {
            cbslot_pushValue(slot, &line);      //将分析得到的数据写入 circbuf
            break;
        }
        else if (ret == -1)
        {
            break;
        }
        else
        {
            cbslot_pushValue(slot, &line);          //将分析得到的数据写入 circbuf
        }
    }

    cb->startFlag = parser->bof;
    cb->endFlag   = parser->eof;
    return;
}

/** 清缓冲区，从新断行，跳转到一个偏移量, 需要从新定位开始位置 */
static void qcache_onCmdSeek(CircularBuff *cb)
{
    txtEncodeParser  *parser;
    MkLine            line;
    CbSlot           *slot;
    __s32             ret;
    parser = cb->plugin;
    circbuf_flush(cb);
    slot   = (CbSlot *)circbuf_getIndex(cb, 0);
    parser->start_seek(parser, cb->seekoffset);
    parser->next(parser, parser->start);

    while (1)
    {
        g_memset(&line, 0, sizeof(MkLine));
        ret = parser->analysis(parser, &line);  //分析一行数据

        //      cbslot_pushValue( slot, &line );        //将分析得到的数据写入 circbuf
        //      if( ret< 0)
        //          break;
        if (ret == -2)
        {
            cbslot_pushValue(slot, &line);      //将分析得到的数据写入 circbuf
            break;
        }
        else if (ret == -1)
        {
            break;
        }
        else
        {
            cbslot_pushValue(slot, &line);          //将分析得到的数据写入 circbuf
        }
    }

    cb->startFlag = parser->bof;
    cb->endFlag   = parser->eof;
    return;
}

static void qcache_onCmdNextPage(CircularBuff *cb)
{
    CbSlot           *slot;
    txtEncodeParser  *parser;
    MkLine            line;
    int               start;
    __bool            full;
    __s32             ret;
    parser  = cb->plugin;
    full    = circbuf_isFull(cb);

    if ((full == EPDK_FALSE) && (cb->endFlag == EPDK_FALSE))
    {
        slot  = circbuf_getTail(cb);
        start = cbslot_getEndOffset(slot);
        parser->next(parser, start);
        slot  = circbuf_addTail(cb);
        cbslot_flush(slot);

        while (1)
        {
            g_memset(&line, 0, sizeof(MkLine));
            ret = parser->analysis(parser, &line);  //分析一行数据

            //          cbslot_pushValue( slot, &line );        //将分析得到的数据写入 circbuf
            //          if( ret< 0)
            //              break;
            if (ret == -2)
            {
                cbslot_pushValue(slot, &line);      //将分析得到的数据写入 circbuf
                break;
            }
            else if (ret == -1)
            {
                break;
            }
            else
            {
                cbslot_pushValue(slot, &line);          //将分析得到的数据写入 circbuf
            }
        }

        cb->endFlag = parser->eof;
        __msg(" startFlag = %d, endFlag = %d , qin = %d , qout = %d ", cb->startFlag, cb->endFlag, cb->qin, cb->qout);
    }
}

static void qcache_onCmdPrevPage(CircularBuff *cb)
{
    CbSlot          *slot;
    int             start;
    __bool          full;
    __s32           ret;
    txtEncodeParser  *parser;
    MkLine            line;
    parser  = cb->plugin;
    full    = circbuf_isFull(cb);

    if ((full == EPDK_FALSE) && (cb->startFlag == EPDK_FALSE))
    {
        slot  = circbuf_getHead(cb);
        start = cbslot_getStartOffset(slot);
        parser->prev(parser, start);
        slot  = circbuf_addHead(cb);
        cbslot_flush(slot);

        while (1)
        {
            g_memset(&line, 0, sizeof(MkLine));
            ret = parser->analysis(parser, &line);  //分析一行数据

            if (ret == -2)
            {
                cbslot_pushValue(slot, &line);      //将分析得到的数据写入 circbuf
                break;
            }
            else if (ret == -1)
            {
                break;
            }
            else
            {
                cbslot_pushValue(slot, &line);          //将分析得到的数据写入 circbuf
            }
        }

        cb->startFlag = parser->bof;
        __msg(" startFlag = %d, endFlag = %d , qin = %d , qout = %d ", cb->startFlag, cb->endFlag, cb->qin, cb->qout);
    }
}

static void qcache_onCmdNext(CircularBuff *cb)
{
    CbSlot           *slot;
    int               start;
    __s32             ret;
    __bool            full;
    txtEncodeParser  *parser;
    MkLine            line;
    parser  = cb->plugin;
    full    = circbuf_isFull(cb);

    if (cb->endFlag == EPDK_FALSE)
    {
        slot    = circbuf_getTail(cb);
        start   = cbslot_getEndOffset(slot);
        parser->next(parser, start);

        if (full)
        {
            cb->qin = (cb->qin + 1) % CB_MAX_SLOT;
            cb->startFlag = EPDK_FALSE;
        }

        slot    = circbuf_addTail(cb);
        cbslot_flush(slot);

        while (1)
        {
            g_memset(&line, 0, sizeof(MkLine));
            ret = parser->analysis(parser, &line);  //分析一行数据

            //          cbslot_pushValue( slot, &line );        //将分析得到的数据写入 circbuf
            //
            //          if( ret< 0)
            //              break;
            if (ret == -2)
            {
                cbslot_pushValue(slot, &line);      //将分析得到的数据写入 circbuf
                break;
            }
            else if (ret == -1)
            {
                break;
            }
            else
            {
                cbslot_pushValue(slot, &line);          //将分析得到的数据写入 circbuf
            }
        }

        cb->endFlag = parser->eof;
        __msg(" startFlag = %d, endFlag = %d , qin = %d , qout = %d ", cb->startFlag, cb->endFlag, cb->qin, cb->qout);
    }

    return;
}

static void qcache_onCmdPrev(CircularBuff *cb)
{
    CbSlot           *slot;
    txtEncodeParser  *parser;
    int               start;
    MkLine            line;
    __s32             ret;
    __bool            full;
    parser  = cb->plugin;
    full    = circbuf_isFull(cb);

    if (cb->startFlag == EPDK_FALSE)
    {
        slot    = circbuf_getHead(cb);
        start   = cbslot_getStartOffset(slot);
        parser->prev(parser, start);

        if (full)
        {
            cb->qout = (cb->qout - 1 + CB_MAX_SLOT) % CB_MAX_SLOT;
            cb->endFlag = EPDK_FALSE;
        }

        slot    = circbuf_addHead(cb);
        cbslot_flush(slot);

        while (1)
        {
            g_memset(&line, 0, sizeof(MkLine));
            ret = parser->analysis(parser, &line);  //分析一行数据

            //          cbslot_pushValue( slot, &line );        //将分析得到的数据写入 circbuf
            //          if( ret< 0)
            //              break;
            if (ret == -2)
            {
                cbslot_pushValue(slot, &line);      //将分析得到的数据写入 circbuf
                break;
            }
            else if (ret == -1)
            {
                break;
            }
            else
            {
                cbslot_pushValue(slot, &line);          //将分析得到的数据写入 circbuf
            }
        }

        cb->startFlag = parser->bof;
        __msg(" startFlag = %d, endFlag = %d , qin = %d , qout = %d ", cb->startFlag, cb->endFlag, cb->qin, cb->qout);
    }

    return;
}

/* 写环形缓冲区线程 (优先级比读线程低)*/
static void qcache_thread(void *p_arg)
{
    CircularBuff *cb = (CircularBuff *)p_arg;
    QCacheCmd       cmd;
    __u8            err;
    OS_MBOX_DATA    mbox_data;

    for (; ;)
    {
        cmd = (QCacheCmd)(long)esKRNL_QPend(cb->cacheQ, 0, &err);
        __msg("cmd = %d ", cmd);

        switch (cmd)
        {
            case QCACHE_CMD_PREV_BUF:           /* 分析上一个cache buffer */
                qcache_onCmdPrev(cb);
                esKRNL_MboxQuery(cb->respMbox, &mbox_data);

                if (mbox_data.OSEventGrp != 0)                      // 有任务等待, 则回应同步消息
                {
                    esKRNL_MboxPost(cb->respMbox, (void *)QCACHE_CMD_PREV_BUF);
                }

                break;

            case QCACHE_CMD_NEXT_BUF:           /* 分析下一个cache buffer */
                qcache_onCmdNext(cb);
                esKRNL_MboxQuery(cb->respMbox, &mbox_data);

                if (mbox_data.OSEventGrp != 0)                      // 有任务等待, 则回应同步消息
                {
                    esKRNL_MboxPost(cb->respMbox, (void *)QCACHE_CMD_NEXT_BUF);
                }

                break;

            case QCACHE_CMD_NEXT_PAGE:      /* 如果环形队列没满, 则分析上一个cache buffer */
                qcache_onCmdNextPage(cb);
                break;

            case QCACHE_CMD_PREV_PAGE:      /* 如果环形队列没满, 则分析下一个cache buffer */
                qcache_onCmdPrevPage(cb);
                break;

            case QCACHE_CMD_SEEK:           /* 跳转功能，需要先找到起始位置， */
                qcache_onCmdSeek(cb);
                esKRNL_MboxQuery(cb->respMbox, &mbox_data);

                if (mbox_data.OSEventGrp != 0)                      // 有任务等待, 则回应同步消息
                {
                    esKRNL_MboxPost(cb->respMbox, (void *)QCACHE_CMD_SEEK);
                }

                break;

            case QCACHE_CMD_REFRESH:        /* 更新配置信息 */
                qcache_onCmdRefresh(cb);
                esKRNL_MboxQuery(cb->respMbox, &mbox_data);

                if (mbox_data.OSEventGrp != 0)                      // 有任务等待, 则回应同步消息
                {
                    esKRNL_MboxPost(cb->respMbox, (void *)QCACHE_CMD_REFRESH);
                }

                break;

            case QCACHE_CMD_EXIT:
                esKRNL_MboxPost(cb->respMbox, (void *)QCACHE_CMD_EXIT);
                esKRNL_TDel(OS_PRIO_SELF);
                break;

            default:
                break;
        }
    }
}
