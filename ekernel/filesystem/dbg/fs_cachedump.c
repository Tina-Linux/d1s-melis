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
#include <stdarg.h>
#include "fsys_debug.h"
#include "buffer_head.h"
#include "list.h"
#include "page_pool.h"

#if     FSYS_DEBUG_ON

extern mem_map_t  mem_map[];
extern int nr_active_pages;
extern int nr_inactive_pages;
extern int nr_debug_slab_pages;

#define MEMD_MAX 10
struct mem_d_t
{
    int start;
    int length;
    char *name;
}  memd_array[MEMD_MAX];
int memd_cnt = 0;

/**
 * fs_log - This struct controls all the logging in the library and tools.
 */
struct cache_dump_info cache_dump_set = {0};

static int pagebufsinteresting(struct page *page)
{
    struct buffer_head *___bh = page->buffers;
    int intresting = 0;

    if (!___bh)
    {
        return intresting;
    }

    do
    {
        if (atomic_read(&___bh->b_count))
        {
            intresting = 1;
        }
        if (buffer_dirty(___bh))
        {
            intresting = 1;
        }
        ___bh = ___bh->b_this_page;
    } while (___bh != page->buffers);

    return intresting;
}

void __dump_mem(char *buf, int dev_start, int len)
{
    int col;
    int j, i = 0;
    char cache[3 * 16 + 1];

    cache[3 * 16] = 0;
    while (i < len)
    {
        int  __dev_start;

        for (j = 0; j < 16 * 3; j++)
        {
            cache[j] = 0;
        }

        __dev_start = dev_start;
        for (col = 0; col < 16 && i < len; col++)
        {
            char a, b;

            if ((dev_start & 0x0f) != col)
            {
                cache[col * 3 + 0] = ' ';
                cache[col * 3 + 1] = ' ';
                cache[col * 3 + 2] = ' ';
            }
            else
            {
                a = buf[i] & 0x0f;
                b = buf[i] >> 4;
                a = a < 0x0a ? '0' + a : 'a' + a - 0x0a;
                b = b < 0x0a ? '0' + b : 'a' + b - 0x0a;
                cache[col * 3 + 0] = ' ';
                cache[col * 3 + 1] = b;
                cache[col * 3 + 2] = a;

                dev_start++;
                i++;
            }
        }
        __inf(" %x -- %s", __dev_start & ~0xf, cache);
    }
}

int debug_dump_mems(char *buf, char *name, int dev_start, int len)
{
    int i;
    int hitted = 0;
    struct mem_d_t *ma;

    if (!can_dump(dump_mems))
    {
        return hitted;
    }

    for (i = 0; ma = &memd_array[i], i < memd_cnt && i < MEMD_MAX; i++)
    {
        int s, e;  /* target start and end */

        if (ma->length == 0 || ma->name == NULL)
        {
            continue;
        }
        if (strcmp(ma->name, name))
        {
            continue;
        }

        s = max(dev_start, ma->start);
        e = min(dev_start + len, ma->start + ma->length);
        if (s < e)
        {
            __dump_mem(buf + (s - dev_start), s, e - s);
            hitted = 1;
        }
    }

    return hitted;
}

void debug_show_sb_inode(struct list_head *head, char *name, int align_level)
{
    int i = 0;
    struct list_head *p;
    struct inode *temp;
    char space[32] = {0};

    if (!can_dump(show_sb_inode))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (!list_empty(head))
    {
        list_for_each(p, head)
        {
            temp = list_entry(p, struct inode, i_sb_list);
            if (cache_dump_set.detail)
            {
                __inf("\n%s [count:%d] [d_name:%s]",
                      space, atomic_read(&temp->i_count), temp->debug_dentry->d_name.name);
            }
            i++;
        }
    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_lru_inode(struct list_head *head, char *name, int align_level)
{
    int i = 0;
    struct list_head *p;
    struct inode *temp;
    char space[32] = {0};

    if (!can_dump(show_lru_inode))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (!list_empty(head))
    {
        list_for_each_prev(p, head)
        {
            temp = list_entry(p, struct inode, i_list);
            if (cache_dump_set.detail)
            {
                __inf("\n%s [count:%d] [d_name:%s]",
                      space, atomic_read(&temp->i_count), temp->debug_dentry->d_name.name);
            }
            i++;
        }
    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_sb_dentry(struct list_head *head, char *name, int align_level)
{
    int i = 0;
    struct list_head *p;
    struct dentry *temp;
    char space[32] = {0};

    if (!can_dump(show_sb_dentry))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (!list_empty(head))
    {
        list_for_each(p, head)
        {
            temp = list_entry(p, struct dentry, d_sb_list);
            if (cache_dump_set.detail)
            {
                __inf("\n%s [count:0x%x] [dname:%s]", space,
                      atomic_read(&temp->d_count), temp->d_name.name);
            }
            i++;
        }
    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_lru_dentry(struct list_head *head, char *name, int align_level)
{
    int i = 0;
    struct list_head *p;
    struct dentry *temp;
    char space[32] = {0};

    if (!can_dump(show_lru_dentry))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (!list_empty(head))
    {
        list_for_each_prev(p, head)
        {
            temp = list_entry(p, struct dentry, d_lru);
            if (cache_dump_set.detail)
            {
                __inf("\n%s [count:0x%x] [dname:%s]", space,
                      atomic_read(&temp->d_count), temp->d_name.name);
            }
            i++;
        }
    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_free_page(struct list_head *head, char *name, int align_level)
{
    int i = 0;
    struct list_head *p;
    struct page *temp;
    char space[32] = {0};

    if (!can_dump(show_free_page))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (!list_empty(head))
    {
        list_for_each(p, head)
        {
            temp = list_entry(p, struct page, list);

            if (cache_dump_set.detail)
            {
                if (atomic_read(&temp->count) || pagebufsinteresting(temp))
                {
                    __inf("\n%s[page_num:%d] [count:0x%x] [virtral:0x%x]", space, (temp - mem_map), atomic_read(&temp->count), temp->virtual);
                    debug_show_bh_inpage(temp->buffers, "buffer of page", 3);
                }
            }
            i++;
        }
    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_lru_page(struct list_head *head, char *name, int align_level)
{
    int i = 0;
    struct list_head *p;
    struct page *temp;
    char space[32] = {0};

    if (!can_dump(show_lru_page))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (!list_empty(head))
    {
        list_for_each_prev(p, head)
        {
            temp = list_entry(p, struct page, lru);
            if (cache_dump_set.detail)
            {
                if (atomic_read(&temp->count) || pagebufsinteresting(temp))
                {
                    __inf("\n%s[page_num:%d] [count:0x%x] [virtral:0x%x]", space, (temp - mem_map), atomic_read(&temp->count), temp->virtual);
                    debug_show_bh_inpage(temp->buffers, "buffer of page", 3);
                }
            }
            i++;
        }
    }
    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_bh_list(struct buffer_head *head, char *name, int align_level)
{
    int i = 0;
    struct buffer_head *next = head;
    char space[32] = {0};

    if (!can_dump(show_bh_list))
    {
        return;
    }

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n\n%s>>>>>%s", space, name);

    i = 0;
    if (next != NULL)
    {
        while (next)
        {
            struct buffer_head *bh = next;
            next = bh->b_next_free;

            if (cache_dump_set.detail)
            {
                __inf("\n%s[blocknr:0x%x] [b_count:0x%x] [b_state:0x%x] [b_data:0x%x] [page_number:%d]",
                      space, bh->b_blocknr, atomic_read(&bh->b_count), bh->b_state, bh->b_data, bh->b_page - mem_map);
            }
            i++;

            if (next == head)
            {
                break;
            }
        }
    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_bh_inpage(struct buffer_head *head, char *name, int align_level)
{
    int i = 0;
    struct buffer_head *next = head;
    char space[32] = {0};

    for (i = 0; i < align_level; i++)
    {
        space[i] = ' ';
    }
    __inf("\n%s>>>>> %s ", space, name);

    i = 0;
    if (next != NULL)
    {
        while (next)
        {
            struct buffer_head *bh = next;
            next = bh->b_this_page;

            __inf("\n%s[blocknr:0x%x] [b_count:0x%x] [b_state:0x%x] [b_data:0x%x]", space, bh->b_blocknr, atomic_read(&bh->b_count), bh->b_state, bh->b_data);
            i++;

            if (next == head)
            {
                break;
            }
        }

    }

    __inf("\n%s--result-- : %d item in upper \"%s\" list.", space, i, name);
}

void debug_show_page_info(char *when)
{
    if (!can_dump(show_page_info))
    {
        return;
    }

    __inf("I:%d A:%d S:%d", nr_inactive_pages,
          nr_active_pages, nr_debug_slab_pages);
}


char str_buf[256];
void debug_show_all_caches(char *when)
{
    debug_show_all_bh_list(str_buf);

    debug_show_all_page_list(str_buf);

    debug_show_all_dentry(str_buf);

    debug_show_all_inode(str_buf);
}

BOOL fs_dump_parse_option(const char *option)
{
    if (strncmp(option, "+dmem:", 6) == 0 || strncmp(option, "+DMEM:", 6) == 0)
    {
        cache_dump_set.dump_mems = 1;

        if (memd_cnt < MEMD_MAX)
        {
            char *name, *ptr1, *ptr2, *ptr3;
            int name_len;
            struct mem_d_t *ma = &memd_array[memd_cnt];

            ptr1 = option + 6;
            ptr2 = strchr(option, ',') + 1;
            ptr3 = strchr(ptr2, ',') + 1;
            name_len = ptr2 - ptr1 - 1;

            name = malloc(name_len);
            strncpy(name, ptr1, name_len);
            name[name_len] = '\0';
            ma->name = name;
            ma->start = atoi(ptr2);
            ma->length = atoi(ptr3);
            memd_cnt++;
        }
        else
        {
            __inf("Ignore segment! Maxum segments reached.");
        }
        return TRUE;
    }
    else if (strcmp(option, "-dmem") == 0 || strcmp(option, "-DMEM") == 0)
    {
        int i;
        struct mem_d_t *ma;

        cache_dump_set.dump_mems = 0;

        for (i = 0; ma = &memd_array[i], i < memd_cnt; i++)
        {
            if (ma->name)
            {
                free(ma->name);
            }
            ma->name = NULL;
            ma->start = 0;
            ma->length = 0;
        }
        memd_cnt = 0;
        return TRUE;
    }
    else if (strcmp(option, "memseg") == 0 || strcmp(option, "MEMSEG") == 0)
    {
        int i;
        struct mem_d_t *ma;

        __inf("%d segment memory dumps.", memd_cnt);
        for (i = 0; ma = &memd_array[i], i < memd_cnt; i++)
        {
            __inf("start: %d, length: %d file: %s", ma->start, ma->length, ma->name);
        }
        return TRUE;
    }
    else if (strcmp(option, "+sbi") == 0 || strcmp(option, "+SBI") == 0)
    {
        cache_dump_set.show_sb_inode = 1;
        return TRUE;
    }
    else if (strcmp(option, "-sbi") == 0 || strcmp(option, "-SBI") == 0)
    {
        cache_dump_set.show_sb_inode = 0;
        return TRUE;
    }
    else if (strcmp(option, "+lrui") == 0 || strcmp(option, "+LRUI") == 0)
    {
        cache_dump_set.show_lru_inode = 1;
        return TRUE;
    }
    else if (strcmp(option, "-lrui") == 0 || strcmp(option, "-LRUI") == 0)
    {
        cache_dump_set.show_lru_inode = 0;
        return TRUE;
    }
    else if (strcmp(option, "+sbd") == 0 || strcmp(option, "+SBD") == 0)
    {
        cache_dump_set.show_sb_dentry = 1;
        return TRUE;
    }
    else if (strcmp(option, "-sbd") == 0 || strcmp(option, "-SBD") == 0)
    {
        cache_dump_set.show_sb_dentry = 1;
        return TRUE;
    }
    else if (strcmp(option, "+lrud") == 0 || strcmp(option, "+LRUD") == 0)
    {
        cache_dump_set.show_lru_dentry = 1;
        return TRUE;
    }
    else if (strcmp(option, "-lrud") == 0 || strcmp(option, "-LRUD") == 0)
    {
        cache_dump_set.show_lru_dentry = 0;
        return TRUE;
    }
    else if (strcmp(option, "+lruf") == 0 || strcmp(option, "+LRUF") == 0)
    {
        cache_dump_set.show_lru_fatcache = 1;
        return TRUE;
    }
    else if (strcmp(option, "-lruf") == 0 || strcmp(option, "-LRUF") == 0)
    {
        cache_dump_set.show_lru_fatcache = 0;
        return TRUE;
    }
    else if (strcmp(option, "+freep") == 0 || strcmp(option, "+FREEP") == 0)
    {
        cache_dump_set.show_free_page = 1;
        return TRUE;
    }
    else if (strcmp(option, "-freep") == 0 || strcmp(option, "-FREEP") == 0)
    {
        cache_dump_set.show_free_page = 0;
        return TRUE;
    }
    else if (strcmp(option, "+lrup") == 0 || strcmp(option, "+LRUP") == 0)
    {
        cache_dump_set.show_lru_page = 1;
        return TRUE;
    }
    else if (strcmp(option, "-lrup") == 0 || strcmp(option, "-LRUP") == 0)
    {
        cache_dump_set.show_lru_page = 0;
        return TRUE;
    }
    else if (strcmp(option, "+bhlist") == 0 || strcmp(option, "+BHLIST") == 0)
    {
        cache_dump_set.show_bh_list = 1;
        return TRUE;
    }
    else if (strcmp(option, "-bhlist") == 0 || strcmp(option, "-BHLIST") == 0)
    {
        cache_dump_set.show_bh_list = 0;
        return TRUE;
    }
    else if (strcmp(option, "+bhinfo") == 0 || strcmp(option, "+BHINFO") == 0)
    {
        cache_dump_set.show_bh_info = 1;
        return TRUE;
    }
    else if (strcmp(option, "-bhinfo") == 0 || strcmp(option, "-BHINFO") == 0)
    {
        cache_dump_set.show_bh_info = 0;
        return TRUE;
    }
    else if (strcmp(option, "+pinfo") == 0 || strcmp(option, "+PINFO") == 0)
    {
        cache_dump_set.show_page_info = 1;
        return TRUE;
    }
    else if (strcmp(option, "-pinfo") == 0 || strcmp(option, "-PINFO") == 0)
    {
        cache_dump_set.show_page_info = 0;
        return TRUE;
    }
    else if (strcmp(option, "+allon") == 0 || strcmp(option, "+ALLON") == 0)
    {
        cache_dump_set.show_all_cache_info = 1;
        return TRUE;
    }
    else if (strcmp(option, "-allon") == 0 || strcmp(option, "-ALLON") == 0)
    {
        cache_dump_set.show_all_cache_info = 0;
        return TRUE;
    }
    else if (strcmp(option, "+alloff") == 0 || strcmp(option, "+ALLON") == 0)
    {
        cache_dump_set.disable_all_cache_info = 1;
        return TRUE;
    }
    else if (strcmp(option, "-alloff") == 0 || strcmp(option, "-ALLON") == 0)
    {
        cache_dump_set.disable_all_cache_info = 0;
        return TRUE;
    }
    else if (strcmp(option, "+detail") == 0 || strcmp(option, "+DETAIL") == 0)
    {
        cache_dump_set.detail = 1;
        return TRUE;
    }
    else if (strcmp(option, "-detail") == 0 || strcmp(option, "-DETAIL") == 0)
    {
        cache_dump_set.detail = 0;
        return TRUE;
    }

    __inf("Unknown fs dump option '%s'", option);
    return FALSE;
}

#endif  /* FSYS_DEBUG_ON */